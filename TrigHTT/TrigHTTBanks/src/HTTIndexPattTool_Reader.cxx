/*
 * @file HTTIndexPattIO.cxx
 * @brief See HTTIndexPattIO.h
 * @author Riley Xu - rixu@cern.ch
 * @date 2019-06-14
 */

#include "TrigHTTBanks/HTTIndexPattTool_Reader.h"
#include "TrigHTTUtils/HTTFunctions.h"

#include "TTree.h"
#include "TTreeReaderArray.h"

#include <stdexcept>
#include <numeric>
#include <algorithm>
#include <utility>


/*****************************************************************************/
/* HTTIndexPattTool_ReaderI                                                  */
/*****************************************************************************/

HTTIndexPattTool_ReaderI::HTTIndexPattTool_ReaderI(std::string const & type, std::string const & name, IInterface const * parent)
    : HTTIndexPattTool_Base(type, name, parent)
{
    declareInterface<HTTIndexPattTool_ReaderI>(this);
}

std::map<size_t, size_t> HTTIndexPattTool_ReaderI::calcCovTotals() const
{
    std::map<size_t, size_t> covmap;
    for (SectorIndexData const & data : m_sectorIndex)
    {
        for (size_t icov = 0; icov < data.coverages.size(); icov++)
            covmap[data.coverages[icov]] += data.nPatterns[icov];
    }
    return covmap;
}

std::vector<pattern_t> HTTIndexPattTool_ReaderI::readPatterns(sector_t sector,
        size_t maxCoverage, size_t minCoverage, std::vector<size_t> * coverages)
{
    // Get index data for this sector
    SectorIndexData indexData;
    ErrorStatus err = getSectorIndex(sector, indexData);
    if (err)
    {
        ATH_MSG_WARNING("readPatterns() didn't find sector " << sector);
        return std::vector<pattern_t>();
    }
    return readPatterns_base(sector, indexData, maxCoverage, minCoverage, coverages);
}

std::vector<pattern_t> HTTIndexPattTool_ReaderI::readPatterns(std::vector<SectorIndexData>::const_iterator it,
        size_t maxCoverage, size_t minCoverage, std::vector<size_t> * coverages)
{
    sector_t sector = m_sectorIndex.getKey(it);
    return readPatterns_base(sector, *it, maxCoverage, minCoverage, coverages);
}


/*****************************************************************************/
/* HTTIndexPattTool_Reader                                                   */
/*****************************************************************************/

// Sets up all TTreeReaders and checks for branch existence. Reads the index and metadata.
StatusCode HTTIndexPattTool_Reader::initialize()
{
    m_option = "READ";
    ATH_CHECK(HTTIndexPattTool_Base::initialize());

    // Set up the TTreeReader
    TTree *t = dynamic_cast<TTree*>(m_file->Get("Data"));
    if (!t)
    {
        ATH_MSG_ERROR("Data tree not found");
        return StatusCode::FAILURE;
    }
    m_dataReader.SetTree(t);
    m_dataReader.Restart();

    // Setup the data tree readers values
    m_LTIndex.clear();
    for (size_t i = 0; i < m_nLayers; i++)
    {
        m_LTIndex.push_back(TTreeReaderArray<UChar_t>(m_dataReader, ("LTIndex" + std::to_string(i)).c_str() ));
    }

    // Read the entire index
    try {
        readIndex();
    }
    catch (std::runtime_error const & e) {
        return StatusCode::FAILURE;
    }

    // Read metadata
    TTree *metadataTree = dynamic_cast<TTree*>(m_file->Get("Metadata"));
    if (metadataTree) m_metadata = HTTBankMetadata::readTree(metadataTree);
    if (!metadataTree || !m_metadata.isValid)
        calculateMetadata(m_sectorIndex.getData());

    return StatusCode::SUCCESS;
}


// Reads the index tree in full, populating m_sectorIndex.
void HTTIndexPattTool_Reader::readIndex()
{
    const std::string msgName = "readIndex() ";

    // Setup readers
    TTreeReader indexReader("Index", m_file.get());
    if (!checkTTR(indexReader)) ATH_MSG_FATAL(msgName << "index reader init failed");
    TTreeReaderValue<Int_t>     sector(indexReader, "sector");
    TTreeReaderArray<UInt_t>    nPatterns(indexReader, "nPatterns");
    TTreeReaderArray<UInt_t>    coverages(indexReader, "coverages");
    TTreeReaderValue<Long64_t>  firstDataEntry(indexReader, "firstDataEntry");
    std::vector<TTreeReaderArray<Int_t>> SSIDs;

    for (size_t i = 0; i < m_nLayers; i++)
        SSIDs.push_back(TTreeReaderArray<Int_t>(indexReader, ("SSIDs" + std::to_string(i)).c_str() ));

    // Append info to these vectors
    std::vector<sector_t> sectors;
    std::vector<SectorIndexData> indexData;
    sectors.reserve(indexReader.GetEntries(false));
    indexData.reserve(indexReader.GetEntries(false));

    // Do the read
    while (indexReader.Next())
    {
        if (!checkTTRV(sector, nPatterns, coverages, firstDataEntry))
            ATH_MSG_FATAL(msgName << "index reader values failed");

        SectorIndexData data;
        data.nPatterns = std::vector<UInt_t>(nPatterns.begin(), nPatterns.end());
        data.coverages = std::vector<UInt_t>(coverages.begin(), coverages.end());
        data.firstDataEntry = *firstDataEntry;
        data.SSIDs.resize(m_nLayers);
        for (size_t i = 0; i < m_nLayers; i++)
        {
            if (!checkTTRV(SSIDs[i]))
                ATH_MSG_FATAL(msgName << "index reader values failed");
            data.SSIDs[i] = std::vector<Int_t>(SSIDs[i].begin(), SSIDs[i].end());
        }

        sectors.push_back(*sector);
        indexData.push_back(data);
    }

    m_sectorIndex.assign(std::move(sectors), std::move(indexData));
}


// Given coverages sorted largest to smallest, returns the [start, end) indices
// of the subset within [max, min]
std::pair<size_t, size_t> findCoverageEntries(std::vector<UInt_t> const & coverages,
        UInt_t max, UInt_t min)
{
    size_t start, end;
    for (start = 0; start < coverages.size(); start++)
        if (coverages[start] <= max) break;
    for (end = start; end < coverages.size(); end++)
        if (coverages[end] < min) break;
    return std::make_pair(start, end);
}


// Returns a vector where each value vals[i] is repeated n[i] times, in order,
// with i in the given range (iend exclusive).
template <class T, class N>
std::vector<T> unflatten(std::vector<T> vals, std::vector<N> n, size_t istart, size_t iend)
{
    std::vector<T> out;
    for (size_t i = istart; i < iend; i++)
    {
        for (size_t x = 0; x < (size_t)n[i]; x++)
            out.push_back(vals[i]);
    }
    return out;
}


// Main read function
std::vector<pattern_t> HTTIndexPattTool_Reader::readPatterns_base(
        sector_t sector, SectorIndexData const & indexData,
        size_t maxCoverage, size_t minCoverage, std::vector<size_t> * coverages)
{
    const std::string msgName("readPatterns_base() ");

    std::vector<pattern_t> patterns;

    // Find range to read in Data tree
    auto se = findCoverageEntries(indexData.coverages, maxCoverage, minCoverage);
    Long64_t istart = indexData.firstDataEntry + se.first;
    Long64_t iend = indexData.firstDataEntry + se.second; // exclusive

    //TTreeReader::EEntryStatus err = m_dataReader.SetEntriesRange(istart, iend);
    // THIS IS BUGGED IN ROOT 6.08; istart can't be 0.

    // Read the patterns,
    for (Long64_t i = istart; i < iend; i++)
    {
        // Set the entry and check for errors
        TTreeReader::EEntryStatus err = m_dataReader.SetEntry(i);
        if (err)
            ATH_MSG_FATAL(msgName << "sector " << sector << " encountered TTreeReader error " << err);
        for (auto const & rv : m_LTIndex)
            if (!checkTTRV(rv)) ATH_MSG_FATAL(msgName << "data reader values failed");

        // Populate patterns
        size_t nPatterns = m_LTIndex[0].GetSize();
        patterns.reserve(patterns.size() + nPatterns);
        for (size_t iPatt = 0; iPatt < nPatterns; iPatt++)
        {
            pattern_t patt(m_nLayers);
            for (size_t layer = 0; layer < m_nLayers; layer++)
            {
                UChar_t index = m_LTIndex[layer][iPatt];
                patt[layer] = indexData.SSIDs[layer][index];
            }
            patterns.push_back(patt);
        }
    }

    // Update coverages
    if (coverages)
    {
        std::vector<size_t> covs(indexData.coverages.begin(), indexData.coverages.end()); // cast to size_t
        *coverages = unflatten(covs, indexData.nPatterns, se.first, se.second);
    }

    return patterns;
}


/*****************************************************************************/
/* HTTIndexPattTool_ReaderLegacy                                             */
/*****************************************************************************/


// Sets up all the branch addresses. Reads the index.
StatusCode HTTIndexPattTool_ReaderLegacy::initialize()
{
    m_option = "READ";
    ATH_CHECK(HTTIndexPattTool_Base::initialize());

    m_cpatterns.resize(m_nLayers);

    // Setup the data tree addresses
    m_dataTree = dynamic_cast<TTree*>(m_file->Get("CpatternData"));
    if (!m_dataTree)
    {
        ATH_MSG_ERROR("initialize() unable to initialize data tree");
        return StatusCode::FAILURE;
    }

    m_dataTree->SetBranchAddress("npatt", &m_npatt);
    UInt_t npatt_max = m_dataTree->GetMaximum("npatt");
    for (size_t i = 0; i < m_nLayers; i++)
    {
        m_cpatterns[i].resize(npatt_max);
        m_dataTree->SetBranchAddress(("cpattern" + std::to_string(i)).c_str(), m_cpatterns[i].data());
    }

    // Read the entire index
    try {
        readIndex();
    }
    catch (std::runtime_error const & e) {
        return StatusCode::FAILURE;
    }

    // Calculate metadata
    calculateMetadata(m_sectorIndex.getData());

    return StatusCode::SUCCESS;
}


// Reads the index tree in full, populating m_sectorIndex.
void HTTIndexPattTool_ReaderLegacy::readIndex()
{
    // Setup addresses
    TTree *indexTree = dynamic_cast<TTree*>(m_file->Get("CpatternIndex"));
    if (!indexTree) ATH_MSG_FATAL("HTTIndexPattTool_ReaderLegacy::readIndex() unable initialize index tree");

    UInt_t sector, ncov;
    indexTree->SetBranchAddress("sector", &sector);
    indexTree->SetBranchAddress("ncov", &ncov);

    UInt_t ncov_max = indexTree->GetMaximum("ncov");
    std::vector<UInt_t> npattern(ncov_max), coverage(ncov_max);
    indexTree->SetBranchAddress("npattern", npattern.data());
    indexTree->SetBranchAddress("coverage", coverage.data());

    std::vector<UInt_t> ndecoder(m_nLayers);
    std::vector<std::vector<UInt_t>> decoderData(m_nLayers);
    for (size_t i = 0; i < m_nLayers; i++)
    {
        UInt_t ndecoder_max = indexTree->GetMaximum(("ndecoder" + std::to_string(i)).c_str());
        decoderData[i].resize(ndecoder_max);
        indexTree->SetBranchAddress(("ndecoder" + std::to_string(i)).c_str(), &(ndecoder[i]));
        indexTree->SetBranchAddress(("decoderData" + std::to_string(i)).c_str(), decoderData[i].data());
    }

    // Append info to these vectors
    size_t nEntries = indexTree->GetEntries();
    std::vector<sector_t> sectors;           sectors.reserve(nEntries);
    std::vector<SectorIndexData> indexData;  indexData.reserve(nEntries);

    // Do the read
    Long64_t firstDataEntry = 0;
    for (size_t i = 0; i < nEntries; i++)
    {
        indexTree->GetEntry(i);

        SectorIndexData data;
        data.nPatterns = std::vector<UInt_t>(npattern.begin(), npattern.begin() + ncov);
        data.coverages = std::vector<UInt_t>(coverage.begin(), coverage.begin() + ncov);
        data.firstDataEntry = firstDataEntry;
        data.SSIDs.resize(m_nLayers);
        for (size_t i = 0; i < m_nLayers; i++)
        {
            data.SSIDs[i] = std::vector<Int_t>(decoderData[i].begin(), decoderData[i].begin() + ndecoder[i]);
        }

        firstDataEntry += ncov;
        sectors.push_back(sector);
        indexData.push_back(data);
    }

    m_sectorIndex.assign(std::move(sectors), std::move(indexData));
}


// Main read function
std::vector<pattern_t> HTTIndexPattTool_ReaderLegacy::readPatterns_base(
        sector_t sector, SectorIndexData const & indexData,
        size_t maxCoverage, size_t minCoverage, std::vector<size_t> * coverages)
{
    (void)sector; // useful for printing, if needed
    std::vector<pattern_t> patterns;

    // Find range to read in Data tree
    auto se = findCoverageEntries(indexData.coverages, maxCoverage, minCoverage); // (start, end)
    Long64_t istart = indexData.firstDataEntry + se.first;
    Long64_t iend = indexData.firstDataEntry + se.second; // exclusive

    // Read the patterns,
    for (Long64_t i = istart; i < iend; i++)
    {
        m_dataTree->GetEntry(i);

        patterns.reserve(patterns.size() + m_npatt);
        for (size_t iPatt = 0; iPatt < m_npatt; iPatt++)
        {
            pattern_t patt(m_nLayers);
            for (size_t layer = 0; layer < m_nLayers; layer++)
            {
                UChar_t index = m_cpatterns[layer][iPatt];
                patt[layer] = indexData.SSIDs[layer][index];
            }
            patterns.push_back(patt);
        }
    }

    // Update coverages
    if (coverages)
    {
        std::vector<size_t> covs(indexData.coverages.begin(), indexData.coverages.end()); // cast to size_t
        *coverages = unflatten(covs, indexData.nPatterns, se.first, se.second);
    }

    return patterns;
}

