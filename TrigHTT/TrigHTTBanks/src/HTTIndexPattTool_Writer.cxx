/*
 * @file HTTIndexPattTool_Writer.cxx
 * @brief See header.
 * @author Riley Xu - rixu@cern.ch
 * @date 2019-06-14
 */

#include "TrigHTTBanks/HTTIndexPattTool_Writer.h"
#include "TrigHTTUtils/HTTFunctions.h"

#include "TTree.h"
#include <numeric>
#include <algorithm>
#include <utility>


HTTIndexPattTool_Writer::HTTIndexPattTool_Writer(std::string const & type, std::string const & name, IInterface const * parent)
    : HTTIndexPattTool_Base(type, name, parent)
{
    declareInterface<HTTIndexPattTool_Writer>(this);
    declareProperty("description", m_description, "Description of the pattern bank to store in the metadata");
}

StatusCode HTTIndexPattTool_Writer::initialize()
{
    m_option = "RECREATE";
    ATH_CHECK(HTTIndexPattTool_Base::initialize());

    // Create the output trees
    m_indexTree = new TTree("Index", "Sector-ordered pattern index (SSID lookup-table)");
    m_dataTree = new TTree("Data", "SSID lookup-table indexes");
    m_metadataTree = new TTree("Metadata", "Pattern bank metadata");
    if (!m_indexTree || !m_dataTree || !m_metadataTree)
    {
        ATH_MSG_ERROR("HTTIndexPattTool_Writer() couldn't create TTrees");
        return StatusCode::FAILURE;
    }

    Int_t an_int = 0;
    Int_t a_pint[1] = {};
    UInt_t a_puint[1] = {};
    UChar_t a_puchar[1] = {};
    Long64_t a_long = 0;

    m_indexTree->Branch("sector", &an_int);
    m_indexTree->Branch("nCoverages", &an_int);
    m_indexTree->Branch("nPatterns", &a_puint, "nPatterns[nCoverages]/i");
    m_indexTree->Branch("coverages", &a_puint, "coverages[nCoverages]/i");
    m_indexTree->Branch("firstDataEntry", &a_long);
    m_dataTree->Branch("nPatterns", &an_int);
    for (size_t layer = 0; layer < m_nLayers; layer++)
    {
        std::string nssidBName = "nSSIDs" + std::to_string(layer);
        std::string ssidBName = "SSIDs" + std::to_string(layer);
        std::string LTIndexBName = "LTIndex" + std::to_string(layer);
        m_indexTree->Branch(nssidBName.c_str(), &an_int);
        m_indexTree->Branch(ssidBName.c_str(), &a_pint, (ssidBName + "[" + nssidBName + "]/I").c_str());
        m_dataTree->Branch(LTIndexBName.c_str(), &a_puchar, (LTIndexBName + "[nPatterns]/b").c_str());
    }

    return StatusCode::SUCCESS;
}


StatusCode HTTIndexPattTool_Writer::finalize()
{
    if (!m_sectorIndex.empty()) write();
        // write() clears m_sectorIndex, so user didn't call write() yet

    ATH_CHECK(HTTIndexPattTool_Base::finalize());
    return StatusCode::SUCCESS;
}


template<typename T>
inline size_t findIndex(std::vector<T> vec, T val)
{
    auto it = find(vec.begin(), vec.end(), val);
    return std::distance(vec.begin(), it);
}


// Updates m_sectorIndex by adding new sectors/SSIds if necessary, and either adding a
// new pattern to LTIndexes or incrementing coverages.
void HTTIndexPattTool_Writer::addPattern(sector_t sector, pattern_t const & pattern, size_t coverage)
{
    // Get the sector data
    auto it_SI = m_sectorIndex.find(sector);
    if (it_SI == m_sectorIndex.end()) // create new sector entry
    {
        IWSectorData data;
        data.SSIDs.resize(m_nLayers);
        data.tempEntry.resize(m_nTemps, -1);
        auto ret = m_sectorIndex.insert({ sector, data });
        it_SI = ret.first;
    }
    IWSectorData & sectorData = it_SI->second;

    // Get the LTIndexes
    std::vector<UChar_t> LTIndexes(m_nLayers);
    for (size_t layer = 0; layer < m_nLayers; layer++)
    {
        size_t LTIndex = findIndex(sectorData.SSIDs[layer], pattern[layer]);
        if (LTIndex == sectorData.SSIDs[layer].size()) // new SSID
            sectorData.SSIDs[layer].push_back(pattern[layer]);
        LTIndexes[layer] = LTIndex;
    }

    // Add the pattern
    auto it_LTI_cov = sectorData.LTI_cov_map.find(LTIndexes);
    if (it_LTI_cov == sectorData.LTI_cov_map.end()) // new pattern
    {
        sectorData.LTI_cov_map.insert({ LTIndexes, coverage });
        m_nPatterns++;
        if (m_nPatterns >= maxPatterns) flush();
    }
    else
    {
        it_LTI_cov->second += coverage; // increment coverage
    }
}


// Writes the currently stored patterns to the next temp tree, and frees up memory.
void HTTIndexPattTool_Writer::flush()
{
    if (m_nPatterns == 0) return;
    ATH_MSG_INFO("flush() flushing " << m_nPatterns << " patterns");

    m_nPatterns = 0;

    std::string treeName("Temp" + std::to_string(m_nTemps));
    m_file->cd();
    TTree *tempTree = new TTree(treeName.c_str(), "Temporary pattern data");
    m_nTemps++;
    m_tempTrees.push_back(tempTree);

    // Setup branches with dummy variables
    std::map<std::vector<UChar_t>, UInt_t> dummy_map;
    tempTree->Branch("LTI_cov_map", &dummy_map);

    // Fill the tree
    Long64_t ientry = 0;
    for (auto & kv : m_sectorIndex)
    {
        IWSectorData & sectorData = kv.second;
        tempTree->SetBranchAddress("LTI_cov_map", &sectorData.LTI_cov_map);
        tempTree->Fill();

        sectorData.tempEntry.push_back(ientry);
        sectorData.LTI_cov_map.clear();
        ientry++;
    }
}


// Merges the Temp trees into the final Data tree, and writes the Index tree.
// For each sector, loop through the temp trees and merge them.
void HTTIndexPattTool_Writer::write()
{

    ATH_MSG_INFO("write() writing " << m_sectorIndex.size() << " sectors");
    m_file->cd();

    // Clear up memory to allow merging
    if (m_nTemps > 0) flush();

    // Write one sector at a time
    Long64_t iDataEntry = 0;
    for (auto & kv : m_sectorIndex)
    {
        sector_t sector = kv.first;
        IWSectorData & sectorData = kv.second;

        // Read Temp trees and merge into memory
        mergeTemps(sectorData);

        // Reformat the map into a coverage sorted list (i.e. match the file format)
        SectorIndexData indexData; // store reformated lists here
        std::vector<std::vector<UChar_t>> LTIndexes; // index by (layer, pattern number)
        reformat(sectorData, indexData.nPatterns, indexData.coverages, LTIndexes);

        // Write this sector's Index entry
        indexData.firstDataEntry = iDataEntry;
        indexData.SSIDs = sectorData.SSIDs;
        writeIndex(sector, indexData);

        // Write this sector's Data entries
        writeData(indexData.nPatterns, LTIndexes);
        iDataEntry += indexData.coverages.size();

        // Update metadata for this sector
        updateMetadata(indexData);
    }

    // Write metadata
    m_metadata.isValid = true;
    m_metadata.description = m_description;
    m_metadata.writeTree(m_metadataTree);
    
    // Cleanup
    m_indexTree->Write();  m_dataTree->Write();
    m_sectorIndex.clear(); m_tempTrees.clear();
    for (size_t itemp = 0; itemp < m_nTemps; itemp++)
        gDirectory->Delete(("Temp" + std::to_string(itemp) + ";*").c_str());
}


// Merges the Temp trees to m_sectorIndex.LTI_cov_map.
void HTTIndexPattTool_Writer::mergeTemps(IWSectorData & sectorData)
{
    for (size_t itemp = 0; itemp < m_nTemps; itemp++)
    {
        if (sectorData.tempEntry[itemp] < 0) continue;

        // Read temp from tree
        std::map<std::vector<UChar_t>, UInt_t> tempMap;
        m_tempTrees[itemp]->SetBranchAddress("LTI_cov_map", &tempMap);
        m_tempTrees[itemp]->GetEntry(sectorData.tempEntry[itemp]);

        // Append to map in memory
        for (auto const & LTI_cov : tempMap)
            sectorData.LTI_cov_map[LTI_cov.first] += LTI_cov.second;
    }
}


// Sorts patternCovs and LTIndexes[i] by coverage, largest to smallest.
void sortByCoverage(std::vector<UInt_t> & patternCovs, std::vector<std::vector<UChar_t>> & LTIndexes)
{
    size_t size = patternCovs.size();

    // initialize original index locations
    std::vector<size_t> idx(size);
    std::iota(idx.begin(), idx.end(), 0);

    // sort indexes based on comparing values in patternCovs
    std::stable_sort(idx.begin(), idx.end(),
            [&](size_t i1, size_t i2) {return patternCovs[i1] > patternCovs[i2]; });

    // permute all vectors in-place
    std::vector<bool> done(size);
    for (size_t i = 0; i < size; i++)
    {
        if (done[i]) continue;

        // move value in iSource to iTarget
        size_t iTarget = i;
        size_t iSource = idx[i];
        while (i != iSource)
        {
            std::swap(patternCovs[iTarget], patternCovs[iSource]);
            for (size_t layer = 0; layer < LTIndexes.size(); layer++)
                std::swap(LTIndexes[layer][iTarget], LTIndexes[layer][iSource]);
            done[iTarget] = true;
            iTarget = iSource;
            iSource = idx[iSource];
        }
        done[iTarget] = true;
    }
}


// Sorts sectorData.LTI_cov_map by coverage and reformats the data to nPatterns, coverages, LTIndexes.
// Also empties m_sectorIndex.LTI_cov_map.
void HTTIndexPattTool_Writer::reformat(IWSectorData & sectorData, std::vector<UInt_t> & nPatterns,
        std::vector<UInt_t> & coverages, std::vector<std::vector<UChar_t>> & LTIndexes)
{
    // Reserve vector sizes
    LTIndexes.resize(m_nLayers);
    for (size_t layer = 0; layer < m_nLayers; layer++)
        LTIndexes.reserve(sectorData.LTI_cov_map.size());
    std::vector<UInt_t> patternCovs; // Coverage for each pattern (same order as LTIndexes)
    patternCovs.reserve(sectorData.LTI_cov_map.size());

    // Fill the per-pattern vectors with data from map, not ordered by coverage yet
    for (auto const & LTI_cov : sectorData.LTI_cov_map)
    {
        for (size_t layer = 0; layer < m_nLayers; layer++)
            LTIndexes[layer].push_back(LTI_cov.first[layer]);
        patternCovs.push_back(LTI_cov.second);
    }
    sectorData.LTI_cov_map.clear();

    // Sort all the vectors by coverage
    sortByCoverage(patternCovs, LTIndexes);

    // Reduce patternCovs to 'coverages' and calculate 'nPatterns'
    UInt_t curr_cov = patternCovs.front();
    UInt_t nPatts = 0;
    for (size_t ipatt = 0; ipatt < patternCovs.size(); ipatt++)
    {
        if (patternCovs[ipatt] != curr_cov)
        {
            coverages.push_back(curr_cov);
            nPatterns.push_back(nPatts);
            curr_cov = patternCovs[ipatt];
            nPatts = 1;
        }
        else
        {
            nPatts++;
        }
    }
    coverages.push_back(curr_cov);
    nPatterns.push_back(nPatts);
}


// Write a sector's index entry
void HTTIndexPattTool_Writer::writeIndex(sector_t sector, SectorIndexData & indexData)
{
    Int_t stor = (Int_t)sector;
    Int_t nCoverages = (Int_t)indexData.coverages.size();
    Long64_t firstDataEntry = (Long64_t)indexData.firstDataEntry;
    std::vector<Int_t> nSSIDs(m_nLayers);

    m_indexTree->SetBranchAddress("sector", &stor);
    m_indexTree->SetBranchAddress("nCoverages", &nCoverages);
    m_indexTree->SetBranchAddress("nPatterns", indexData.nPatterns.data());
    m_indexTree->SetBranchAddress("coverages", indexData.coverages.data());
    m_indexTree->SetBranchAddress("firstDataEntry", &firstDataEntry);
    for (size_t layer = 0; layer < m_nLayers; layer++)
    {
        nSSIDs[layer] = (Int_t)indexData.SSIDs[layer].size();
        m_indexTree->SetBranchAddress(("nSSIDs" + std::to_string(layer)).c_str(), &nSSIDs[layer]);
        m_indexTree->SetBranchAddress(("SSIDs" + std::to_string(layer)).c_str(), indexData.SSIDs[layer].data());
    }

    m_indexTree->Fill();
}


// Write a sector's data entries. nPatterns is the same as the index tree nPatterns: the
// number of patterns for a given coverage. The LTIndexes contain all the patterns in the sector (not folded).
void HTTIndexPattTool_Writer::writeData(std::vector<UInt_t> & nPatterns, std::vector<std::vector<UChar_t>> & LTIndexes)
{
    size_t startIndex = 0; // where in LTIndexes to start reading for each entry
    for (size_t icov = 0; icov < nPatterns.size(); icov++)
    {
        Int_t nPatts = nPatterns[icov];
        m_dataTree->SetBranchAddress("nPatterns", &nPatts);
        for (size_t layer = 0; layer < m_nLayers; layer++)
        {
            m_dataTree->SetBranchAddress(("LTIndex" + std::to_string(layer)).c_str(),
                    LTIndexes[layer].data() + startIndex);
        }

        m_dataTree->Fill();
        startIndex += nPatts;
    }
    // assert startIndex == LTIndexes[0].size()
}

