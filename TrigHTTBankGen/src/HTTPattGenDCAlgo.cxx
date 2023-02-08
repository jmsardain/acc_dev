// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

/*
 * HTTPattGenDCAlgo.cxx: See header.
 *
 * Created: 2019/06/11
 * Author: Riley Xu
 * Email: rixu@cern.ch
 */


#include "TrigHTTUtils/HTTMacros.h"
#include "TrigHTTUtils/HTTFunctions.h"
#include "TrigHTTObjects/HTT_SSID.h"
#include "TrigHTTMaps/HTTPlaneMap.h"
#include "TrigHTTBankGen/HTTPattGenDCAlgo.h"

#include <sstream>

using std::vector;
using std::string;

/*****************************************************************************/
/* Constructor and Initialize                                                */
/*****************************************************************************/

HTTPattGenDCAlgo::HTTPattGenDCAlgo(const std::string& name, ISvcLocator* pSvcLocator) :
    AthAlgorithm(name, pSvcLocator),
    m_HTTMapping("TrigHTTMappingSvc", name),
    m_reader("HTTIndexPattTool_Reader"),
    m_writer("HTTCCachePattIO", name)
{
    declareProperty("maxPatts",  m_maxNPatts,        "Number of patterns that can fit in the bank, or -1");
    declareProperty("nDCMax",    m_nDCMax,           "Max number of DC bits per pattern, or -1");
    declareProperty("nDC",       m_nDC,              "Number of DC bits in each layer (vector)");
    declareProperty("outbank",   m_outBankPath,      "Output DC bank filepath");
    declareProperty("dumpFile",  m_dumpPatternsFile, "Optional filepath to dump list of patterns");
    declareProperty("description",  m_description,   "Description of the bank to put in the metadata");
}


StatusCode HTTPattGenDCAlgo::initialize()
{
    const std::string msgName("initialize() ");

    ATH_CHECK(m_HTTMapping.retrieve());
    ATH_CHECK(m_reader.retrieve());
    ATH_CHECK(m_writer.retrieve());

    m_nLayers = m_HTTMapping->PlaneMap_1st()->getNLogiLayers();
    if (m_nDC.size() != m_nLayers)
    {
        ATH_MSG_ERROR(msgName << "nDC has " << m_nDC.size() << " layers but expected " << m_nLayers);
        return StatusCode::FAILURE;
    }

    createBitmasks();

    return StatusCode::SUCCESS;
}


// Converts m_nDC into bitmasks m_allowedDCBits
void HTTPattGenDCAlgo::createBitmasks()
{
    std::string const msgName("createBitmasks() ");

    m_allowedDCBits.resize(m_nLayers);

    std::stringstream ss;
    ss << msgName << "using DC bit masks: [";
    for (size_t layer = 0; layer < m_nLayers; layer++)
    {
        if (m_nDC[layer] < 0)
            ATH_MSG_FATAL(msgName << "number of DC bits must be non-negative");

        if (m_HTTMapping->PlaneMap_1st()->isSCT(layer))
            m_allowedDCBits[layer] = (1ull << m_nDC[layer]) - 1;
        else // pixel
        {
            // prioritizes dcBit in x direction if nDC is not even
            size_t nBits_x = m_nDC[layer] / 2 + m_nDC[layer] % 2;
            size_t nBits_y = m_nDC[layer] / 2;

            m_allowedDCBits[layer] = (1ull << nBits_x) - 1;
            m_allowedDCBits[layer] |= ((1ull << nBits_y) - 1) << NBITS_PHI;
        }

        if (layer != 0) ss << ",";
        ss << formatHex(m_allowedDCBits[layer]);
    }
    ss << "]";
    ATH_MSG_INFO(ss.str());
}


/*****************************************************************************/
/* Execute and helpers                                                       */
/*                                                                           */
/* Read the TSP bank and merge them into DC bank stored in memory (m_dcPatts)*/
/*****************************************************************************/

StatusCode HTTPattGenDCAlgo::execute()
{
    // Failsafe to make sure not run more than once. Better way?
    static int eventNum = 0;
    if (eventNum > 0) return StatusCode::SUCCESS;
    eventNum++;

    /* Add patterns by decreasing coverage. Want to load as much as possible at
     * once (ROOT compression / better locality), so estimate range of coverages
     * that will guarantee fit in the AM bank and add them all at once. Then, add
     * coverages one by one until bank is full.
     *
     * TODO previous tests using FTK++ showed that continuing to merge patterns even
     * after the bank is full improved road-finding efficiency quite a bit.
     */

    // [covMax, covMin) are guaranteed to fit in bank.
    std::map<size_t, size_t> covTotals = m_reader->calcCovTotals();
    auto it_covMax = covTotals.rbegin();
    auto it_covMin = it_covMax;
    while (it_covMax != covTotals.rend() && m_nPatts < m_maxNPatts)
    {
        size_t nPatts = m_nPatts;
        for (it_covMin = it_covMax; it_covMin != covTotals.rend(); it_covMin++)
        {
            nPatts += it_covMin->second; // assume no patterns are merged
            if (nPatts >= m_maxNPatts) break;
        }

        if (it_covMin == it_covMax) // add a single coverage (this coverage may fill bank)
        {
            addCovRange(it_covMax->first, it_covMax->first);
            it_covMax++;
        }
        else if (it_covMin == covTotals.rend()) // break statement not hit. add everything remaining
        {
            addCovRange(it_covMax->first, 0);
            break;
        }
        else // add a range of coverages
        {
            // guarantee all fit, so no need to worry about low-coverage patterns
            // taking spots from high-coverage patterns
            addCovRange(it_covMax->first, it_covMin->first + 1);
            it_covMax = it_covMin;
        }
    }

    return StatusCode::SUCCESS;
}


// Read the TSP patterns with coverage [maxCov, minCov] and merge them into the
// DC bank.
//
// Note that if the bank is full in the middle of adding this coverage range, patterns from
// earlier sectors will be added while later sectors can only be merged. Might be worth
// doing some randomization on the final coverage range? Difficult to know when the bank
// will be full, though.
void HTTPattGenDCAlgo::addCovRange(size_t maxCov, size_t minCov)
{
    // Metrics for printing below
    size_t nPatts_before = m_nPatts;
    size_t nPatts_TSP = 0;

    // Loop through every sector and read the given coverage range
    auto const & sectorIndex = m_reader->getIndex();
    for (auto it = sectorIndex.cbegin(); it != sectorIndex.cend(); it++)
    {
        sector_t sector = sectorIndex.getKey(it);
        vector<HTTDCPattern> & dcPatts = m_dcPatts[sector];

        vector<size_t> coverages;
        vector<pattern_t> patterns = m_reader->readPatterns(it, maxCov, minCov, &coverages);

        for (size_t i = 0; i < patterns.size(); i++)
            nPatts_TSP += addPattern(dcPatts, patterns[i], coverages[i]);
    }


    ATH_MSG_INFO("addCovRange() Added " << m_nPatts - nPatts_before << " DC patterns from "
            << nPatts_TSP << " TSP patterns with coverages [" << maxCov << ", " << minCov << "]");
}


// Returns true if patt has been added or merged into dcPatts
bool HTTPattGenDCAlgo::addPattern(vector<HTTDCPattern> & dcPatts, pattern_t const & patt, size_t coverage)
{
    // Try to merge with an existing pattern
    for (HTTDCPattern & tgt_patt : dcPatts)
        if (mergePattern(tgt_patt, patt, coverage)) return true;

    // Add a new pattern
    if (m_nPatts < m_maxNPatts)
    {
        HTTDCPattern new_patt(patt, vector<ssid_t>(m_nLayers), coverage);
        dcPatts.push_back(new_patt);
        m_nPatts++;
        return true;
    }
    return false;
}


// Returns true if bit difference between ss1 and ss2 is only in allowed_bits.
// If either are wildcards, both must be in order to be merged.
bool HTTPattGenDCAlgo::mergeable(ssid_t ss1, ssid_t ss2, ssid_t allowed_bits)
{
    if (ss1 == SSID_WILDCARD || ss2 == SSID_WILDCARD) return ss1 == ss2;
    return ( ((ss1 ^ ss2) & (~allowed_bits)) == 0 );
}


/* Adds patt to tgt_patt by adding dc bits to tgt_patt, if possible. Will also
 * add/remove wildcards in tgt_patt (see mergeWildcards).
 *
 * Returns true if successfully merged.
 */
bool HTTPattGenDCAlgo::mergePattern(HTTDCPattern & tgt_patt, pattern_t const & patt, size_t coverage)
{
    size_t nDCBits = 0;
    vector<ssid_t> dcBits(m_nLayers); // new dc bits if merge is possible
    for (size_t layer = 0; layer < m_nLayers; layer++)
    {
        if (!mergeable(tgt_patt.getSSID(layer), patt[layer], m_allowedDCBits[layer]))
            return false;

        // Update existing dcBits with bit difference of new pattern
        if (tgt_patt.getSSID(layer) == SSID_WILDCARD || patt[layer] == SSID_WILDCARD)
            dcBits[layer] = tgt_patt.getDCBits(layer);
        else
            dcBits[layer] = tgt_patt.getDCBits(layer) | (tgt_patt.getSSID(layer) ^ patt[layer]);

        // Check total number of dc bits
        nDCBits += __builtin_popcount(dcBits[layer]);
        if (nDCBits > m_nDCMax) return false;
    }

    // Do the merge
    tgt_patt.setDCBits(dcBits);
    tgt_patt.setCoverage(tgt_patt.getCoverage() + coverage);
    return true;
}



/*****************************************************************************/
/* Finalize and helpers                                                      */
/*                                                                           */
/* Reformat the bank in memory into the ccache form, and write to file       */
/*****************************************************************************/

StatusCode HTTPattGenDCAlgo::finalize()
{
    // Assign start pid to each sector
    std::map<sector_t, pid_t> startPIDs;
    pid_t pid = 0;
    for (auto const & sp : m_dcPatts)
    {
        startPIDs[sp.first] = pid;
        pid += sp.second.size();
    }

    // Reformat m_dcPatts into an HTTPatternBank
    HTTPatternBank bank;
    reformatBank(bank, startPIDs);

    // Update metadata
    updateMetadata(bank);

    // Write bank to file
    int err = m_writer->write(m_outBankPath, bank);
    if (err) return StatusCode::FAILURE;

    // Dump patterns if needed for debugging
    if (!m_dumpPatternsFile.empty()) dumpPatterns();

    return StatusCode::SUCCESS;
}


// Reformat m_dcPatts into bank. Also keep track of nTSP metadata since doing this loop anyways.
void HTTPattGenDCAlgo::reformatBank(HTTPatternBank & bank, std::map<sector_t, pid_t> const & startPIDs)
{
    bank.setNLayers(m_nLayers);

    // Metadata
    vector<ULong64_t> nTSP_layer(m_nLayers);
    vector<ULong64_t> nTSP(m_nPatts, 1); // per PID

    // Set bank.m_data
    for (size_t layer = 0; layer < m_nLayers; layer++)
    {
        // Sort pids in a single layer by ssid, sector
        std::map<ssid_t, std::map<sector_t, vector<pid_t>>> ccachePatts;
            // ccache pids (not encoded) for a specific layer. index by (ssid, sector)

        for (auto const & sp : m_dcPatts)
        {
            sector_t sector = sp.first;
            vector<HTTDCPattern> const & dcPatts = sp.second;
            pid_t startPID = startPIDs.at(sector);
            for (pid_t offset = 0; (size_t)offset < dcPatts.size(); offset++)
            {
                vector<ssid_t> ssids = dcPatts[offset].getMatchingSSIDs(layer);
                for (ssid_t ssid : ssids)
                    ccachePatts[ssid][sector].push_back(startPID + offset);

                // metadata
                nTSP[startPID + offset] *= ssids.size();
                nTSP_layer[layer] += ssids.size();
            }
        }

        // Transform map into HTTVectorMap
        vector<ssid_t> ssids;
        vector<HTTVectorMap<sector_t, HTTPatternBank::CCacheData>> ssid_data;
        ssids.reserve(ccachePatts.size()); ssid_data.reserve(ccachePatts.size());
        for (auto const & sm : ccachePatts) // (ssid, map<sector_t, vector<pid_t>>
        {
            vector<sector_t> sectors;
            vector<HTTPatternBank::CCacheData> sector_data;
            sectors.reserve(sm.second.size()); sector_data.reserve(sm.second.size());
            for (auto const & sp : sm.second) // (sector, vector<pid_t>)
            {
                sectors.push_back(sp.first);
                sector_data.push_back(HTTPatternBank::encodeCCache(sp.second));
            }
            ssids.push_back(sm.first);
            ssid_data.push_back(HTTVectorMap<sector_t, HTTPatternBank::CCacheData>(std::move(sectors), std::move(sector_data)));
        }
        bank.m_data[layer].assign(std::move(ssids), std::move(ssid_data));
    }

    // Update metadata
    for (auto x : nTSP) bank.m_metadata.nPatterns_TSP += x;
    bank.m_metadata.nPatterns_layer = nTSP_layer;
}


void HTTPattGenDCAlgo::updateMetadata(HTTPatternBank & bank)
{
    // NOTE: nPatterns_TSP and nPatterns_layer set in reformatBank()
    bank.m_metadata.isValid = true;
    bank.m_metadata.description = m_description + "tsp bank tags:\n" + m_reader->getMetadata().description;
    bank.m_metadata.hasDC = bank.m_metadata.nPatterns_TSP != m_nPatts;

    bank.m_metadata.nPatterns = m_nPatts;

    ULong64_t maxCov = 0, minCov = -1;
    for (auto const & sp : m_dcPatts)
    {
        vector<HTTDCPattern> const & dcPatts = sp.second;
        if (!dcPatts.empty()) bank.m_metadata.nSectors++;
        for (HTTDCPattern const & patt : dcPatts)
        {
            size_t cov = patt.getCoverage();
            if (cov < minCov) minCov = cov;
            if (cov > maxCov) maxCov = cov;
        }
    }
    bank.m_metadata.maxCoverage = maxCov;
    bank.m_metadata.minCoverage = minCov;
}


// Dumps patterns in a user-friendly format for reading patterns from pids.
// Writes a ROOT file with one entry per pattern, with branches
//      sector
//      ssid[0-7]
//      dcBits[0-7]
// Note the entry number is the same as the pid.
void HTTPattGenDCAlgo::dumpPatterns()
{
    // Create the tree and branches
    TFile *outfile = TFile::Open(m_dumpPatternsFile.c_str(), "RECREATE");
    if (!outfile || outfile->IsZombie())
    {
        ATH_MSG_WARNING("dumpPatterns() couldn't open file " << m_dumpPatternsFile);
        return;
    }

    sector_t aSector = 0;
    ssid_t aSSID = 0;

    TTree *tree = new TTree("patterns", "HTTSim DC pattern dump");
    tree->Branch("sector", &aSector);
    for (size_t layer = 0; layer < m_nLayers; layer++)
    {
        tree->Branch(("ssid" + std::to_string(layer)).c_str(), &aSSID);
        tree->Branch(("dcBits" + std::to_string(layer)).c_str(), &aSSID);
    }

    // Fill the tree
    for (auto & sp : m_dcPatts)
    {
        sector_t sector = sp.first;
        tree->SetBranchAddress("sector", &sector);
        for (HTTDCPattern & patt : sp.second)
        {
            pattern_t ssids = patt.getSSIDs();
            pattern_t dcBits = patt.getDCBits();
            for (size_t layer = 0; layer < m_nLayers; layer++)
            {
                tree->SetBranchAddress(("ssid" + std::to_string(layer)).c_str(), &ssids[layer]);
                tree->SetBranchAddress(("dcBits" + std::to_string(layer)).c_str(), &dcBits[layer]);
            }
            tree->Fill();
        }
    }

    outfile->Write();
    delete outfile;
    ATH_MSG_INFO("dumpPatterns() wrote to file " << m_dumpPatternsFile);
}
