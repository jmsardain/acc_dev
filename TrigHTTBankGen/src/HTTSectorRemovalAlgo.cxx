// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

/**
 * @file HTTSectorRemovalAlgo.cxx
 * @author Riley Xu - riley.xu@cern.ch
 * @date Aug 4th, 2020
 * @brief Algorithm to handle duplicate removal of sectors in a matrix file.
 *
 * See header.
 */

#include "TrigHTTMaps/HTTPlaneMap.h"
#include "TrigHTTBankGen/HTTSectorRemovalAlgo.h"
#include "TrigHTTBankGen/HTTMatrixIO.h"

#include "TTree.h"
#include "TFile.h"
#include "TROOT.h"

#include <fstream>

///////////////////////////////////////////////////////////////////////////////
// Constructor / Initialize
///////////////////////////////////////////////////////////////////////////////

HTTSectorRemovalAlgo::HTTSectorRemovalAlgo(const std::string& name, ISvcLocator* pSvcLocator) :
    AthAlgorithm(name, pSvcLocator),
    m_HTTMapping("TrigHTTMappingSvc", name)
{
    declareProperty("input", m_inpath, "Input path to matrix file");
    declareProperty("output", m_outpath, "Output path to list of sectors to remove");
    declareProperty("region", m_region);
    declareProperty("method", m_sMethod, "Main method to use. Can be: \n"
            "discard_wildcards -- TODO");
}


StatusCode HTTSectorRemovalAlgo::initialize()
{
    ATH_MSG_DEBUG("initialize()");

    ATH_CHECK(m_HTTMapping.retrieve());
    if (m_sMethod == "discard_wildcards")
        m_method = Method::discardWc;
    else if (m_sMethod == "prefer_wildcards")
        m_method = Method::preferWc;
    else if (m_sMethod == "coverage_threshold")
        m_method = Method::coverageThresh;

    // Open file and get tree
    TFile* file = TFile::Open(m_inpath.c_str());
    if (!file || file->IsZombie())
    {
        ATH_MSG_FATAL("Could not open " << m_inpath);
        return StatusCode::FAILURE;
    }
    TTree *matrix_tree = dynamic_cast<TTree*>(file->Get(Form("am%d", m_region)));

    // Get sizing from matrix file (not plane map, since don't know 1st or 2nd)
    matrix_tree->SetBranchAddress("nplanes", &m_nLayers);
    matrix_tree->SetBranchAddress("ndim", &m_nCoords);
    matrix_tree->GetEntry(0);
    ATH_MSG_INFO(m_nLayers << " " << m_nCoords);

    // Resize
    m_moduleMap.resize(m_nLayers);
    m_sectorInfo.resize(matrix_tree->GetEntries());
    m_removeList.resize(matrix_tree->GetEntries());

    // Read
    readTree(matrix_tree);

    // Execute
    makeRemovalList();

    return StatusCode::SUCCESS;
}


// Reads the tree, populating m_moduleMap and m_sectorInfo
void HTTSectorRemovalAlgo::readTree(TTree *tree)
{
    float coverage = 0; // get coverage directly instead of from track_bins

    HTTMatrixReader reader(tree, m_nLayers, m_nCoords);
    tree->SetBranchStatus("*", 0); // disable all branches
    tree->SetBranchStatus("hashID", 1);
    tree->SetBranchStatus("nhit", 1);
    tree->SetBranchAddress("nhit", &coverage);

    while (reader.nextEntry())
    {
        std::vector<module_t> & modules = reader.getModules();
        //HTTMatrixAccumulator & acc = reader.getAccumulator();

        for (int layer = 0; layer < m_nLayers; layer++)
            m_moduleMap[layer][modules[layer]].push_back(reader.getEntry());

        m_sectorInfo[reader.getEntry()].coverage = static_cast<size_t>(coverage);
        m_sectorInfo[reader.getEntry()].modules = modules;
    }

    ATH_MSG_INFO("Read " << m_sectorInfo.size() << " sectors");
}


///////////////////////////////////////////////////////////////////////////////
// Execute
///////////////////////////////////////////////////////////////////////////////


StatusCode HTTSectorRemovalAlgo::execute()
{
    // Do nothing; this class does not process events. The main algorithm is
    // called in initialize() and finalize().
    return StatusCode::SUCCESS;
}


void HTTSectorRemovalAlgo::makeRemovalList()
{
    for (size_t s1 = 0; s1 < m_sectorInfo.size(); s1++)
    {
        if (m_removeList[s1]) continue;
        std::vector<sector_t> matches(getMatches(s1));
        compareMatches(s1, matches);
    }
}

// Get a list of sectors that match the sector given, allowing for wildcards.
// The wildcards of one sector must be a super/sub-set of the other sector.
// Only returns sectors > s1.
std::vector<sector_t> HTTSectorRemovalAlgo::getMatches(sector_t s1)
{
    std::vector<module_t> const & modules = m_sectorInfo[s1].modules;

    char nWc = 0; // Number of WC in target sector (this entry)
    struct Count
    {
        char nMatch = 0;   // Number of layers sector matches target sector
        char nWcBoth = 0;  // Number of layers both sector and target sector have WC
        char nWcMatch = 0; // Number of layers sector has WC but target sector doesn't
    };
    std::map<sector_t, Count> sectorCount;

    for (int layer = 0; layer < m_nLayers; layer++)
    {
        if (modules[layer] == MODULE_BADMODULE)
        {
            nWc++;
            for (sector_t sector : m_moduleMap[layer][MODULE_BADMODULE])
                if (sector > s1)
                    sectorCount[sector].nWcBoth++;
        }
        else
        {
            for (sector_t sector : m_moduleMap[layer][modules[layer]])
                if (sector > s1)
                    sectorCount[sector].nMatch++;
            for (sector_t sector : m_moduleMap[layer][MODULE_BADMODULE])
                if (sector > s1)
                    sectorCount[sector].nWcMatch++;
        }
    }

    std::vector<sector_t> matches;
    for (auto const & count : sectorCount)
    {
        bool match = (nWc + count.second.nMatch + count.second.nWcMatch == m_nLayers); // Two sectors are the same up to wildcards
        bool tgtMoreWc = (count.second.nWcMatch == 0);
        bool tgtLessWc = (nWc == count.second.nWcBoth);
        if (match && (tgtMoreWc != tgtLessWc))
            matches.push_back(count.first);
    }

    return matches;
}


int nWc(std::vector<module_t> const & modules)
{
    int nWc = 0;
    for (module_t m : modules)
        if (m == MODULE_BADMODULE)
            nWc++;
    return nWc;
}


// Compares each sector in matches with s1, and adds duplicates to m_removalList
// based on the policy given by m_method.
void HTTSectorRemovalAlgo::compareMatches(sector_t s1, std::vector<sector_t> const & matches)
{
    for (sector_t s2 : matches)
    {
        if (m_removeList[s2]) continue;
        switch (m_method)
        {
            case Method::discardWc:
                if (nWc(m_sectorInfo[s1].modules) > nWc(m_sectorInfo[s2].modules))
                {
                    m_removeList[s1] = true;
                    return; // This ends the matching against s1
                }
                else // Note that two matching sectors will never have to same number of wcs, so s1 < s2 here
                {
                    m_removeList[s2] = true;
                }
                break;
            case Method::preferWc:
                if (nWc(m_sectorInfo[s1].modules) > nWc(m_sectorInfo[s2].modules))
                {
                    m_removeList[s2] = true;
                }
                else
                {
                    m_removeList[s1] = true;
                    return;
                }
                break;
            case Method::coverageThresh:
                // TODO random choice of 20% for now
                if (nWc(m_sectorInfo[s1].modules) > nWc(m_sectorInfo[s2].modules)
                        && !(m_sectorInfo[s1].coverage > m_sectorInfo[s2].coverage / 5))
                {
                    m_removeList[s1] = true;
                    return;
                }
                else if (!(m_sectorInfo[s2].coverage > m_sectorInfo[s1].coverage / 5))
                {
                    m_removeList[s2] = true;
                }
                break;
            default:
                break;
        }
    }
}



///////////////////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////////////////

StatusCode HTTSectorRemovalAlgo::finalize()
{
    std::ofstream file(m_outpath, std::ios_base::trunc);

    int count = 0;
    for (size_t i = 0; i < m_removeList.size(); i++)
    {
        if (m_removeList[i])
        {
            file << i << "\n";
            count++;
        }

    }
    ATH_MSG_INFO("Marked " << count << " sectors for removal");
    return StatusCode::SUCCESS;
}
