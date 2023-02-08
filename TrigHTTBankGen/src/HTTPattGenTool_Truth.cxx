/*
 * HTTPattGenTool_Truth.cxx: See HTTPattGenTool_Truth.h
 *
 * Created: August 8th, 2019
 * Author: Riley Xu
 * Email: rixu@cern.ch
 */

#include "TrigHTTBankGen/HTTPattGenTool_Truth.h"

#include "TFile.h"
#include "TTree.h"

#include "TrigHTTUtils/HTTMacros.h"
#include "TrigHTTUtils/HTTFunctions.h"
#include "TrigHTTObjects/HTTHit.h"
#include "TrigHTTObjects/HTTEventInputHeader.h"
#include "TrigHTTObjects/HTTLogicalEventInputHeader.h"
#include "TrigHTTBanks/HTTSectorBank.h"

using std::vector;

/*****************************************************************************/
/* Constructor                                                               */
/*****************************************************************************/


HTTPattGenTool_Truth::HTTPattGenTool_Truth(const std::string& algname, const std::string &name, const IInterface *ifc) :
    HTTPattGenToolI(algname, name, ifc),
    m_hitInputTool("HTTInputHeaderTool/HTTInputHeaderTool"),
    m_hitMapTool("HTT_RawToLogicalHitsTool/HTT_RawToLogicalHitsTool")
{
    declareProperty("truth", m_truthPath, "Path to wrapper file containing hits from truth tracks");
    declareProperty("rawInput", m_rawInput, "whether truth file contains raw hits or logical hits");
}


StatusCode HTTPattGenTool_Truth::initialize()
{
    const std::string msgName("initialize() ");

    ATH_CHECK(HTTPattGenToolI::initialize());

    m_header = new HTTLogicalEventInputHeader();
    if (m_rawInput)
    {
        ATH_CHECK(m_hitInputTool.retrieve());
        ATH_CHECK(m_hitMapTool.retrieve());
    }
    else
    {
        // Check path
        if (m_truthPath.empty())
        {
            ATH_MSG_ERROR(msgName << "Truth path not set");
            return StatusCode::FAILURE;
        }

        // Open file
        m_truthFile = std::unique_ptr<TFile>(TFile::Open(m_truthPath.c_str()));
        if (!m_truthFile || m_truthFile->IsZombie())
        {
            ATH_MSG_ERROR(msgName << "Couldn't open " << m_truthPath);
            return StatusCode::FAILURE;
        }
        ATH_MSG_INFO("Reading truth file " << m_truthPath);

        // Setup trees / branches
        m_eventTree = dynamic_cast<TTree*>(m_truthFile->Get("HTTLogicalEventTree"));
        m_eventTree->GetBranch("HTTLogicalEventInputHeader")->SetAddress(&m_header);
    }

    return StatusCode::SUCCESS;
}

StatusCode HTTPattGenTool_Truth::finalize()
{
    if (m_header) delete m_header;
    return StatusCode::SUCCESS;
}

/*****************************************************************************/
/* Core Algorithm                                                            */
/*****************************************************************************/

static const int towerID = 0; // TODO

// As noted in the header, we can generate patterns without knowing the sector.
// But since other components like file storage and pattern matching require
// sectors, we must do a reverse search for the sector.
ErrorStatus HTTPattGenTool_Truth::nextCandidates(std::vector<HTTPattern> & pattCands, size_t & layersMissed)
{
    const std::string msgName("nextCandidates() ");

    // Read hits from tree
    if (m_rawInput)
    {
        HTTEventInputHeader eventHeader;
        bool last=false;
        StatusCode sc = m_hitInputTool->readData(&eventHeader, last);
        if (sc == StatusCode::FAILURE) {
	  return ES_FAIL;
	}
        if (last) return ES_EOF;
        unsigned stage = 1; // TODO hardcoded
        sc = m_hitMapTool->convert(stage, eventHeader, *m_header);
        if (sc == StatusCode::FAILURE) {
	  return ES_FAIL;
	}
    }
    else
    {
        if (m_iter >= (size_t)m_eventTree->GetEntries()) return ES_EOF;
        m_eventTree->GetEntry(m_iter);
    }
    std::vector<HTTHit> const & hits = m_header->towers()[towerID].hits();

    // Initialize variables to be filled
    pattern_t patt(m_nLayers, SSID_WILDCARD);
    vector<module_t> modules(m_nLayers, MODULE_BADMODULE); // modules of the ssids in patt
    layersMissed = m_nLayers; // init with max and decrement as we find hits

    // Populate the pattern ssids and modules
    for (HTTHit const & hit : hits)
    {
        size_t layer = hit.getLayer();
        if (layer >= m_nLayers) continue;
        if (patt[layer] == SSID_WILDCARD) // Use first hit in each layer TODO other options when multiple hits in a layer?
        {
            patt[layer] = m_HTTMapping->SSMap()->getSSID(hit);
            modules[layer] = hit.getHTTIdentifierHash();
            layersMissed--;
        }
    }

    // Check if the truth pattern itself has too many / bad WCs
    m_nLayersMissed_truth = layersMissed;
    if (layersMissed > m_maxWCs || !checkAllowedWCs(patt))
    {
        ATH_MSG_DEBUG(msgName << "bad truth pattern on iteration " << m_iter << ": " << patt);
        return ES_CONSTRAINT;
    }

    // Match the sector
    sector_t sector = m_sectorBank->findSector(modules);
    if (sector < 0)
    {
        ATH_MSG_DEBUG("HTTPattGenTool_Truth::next() found a pattern that didn't perfectly match any sector: " << patt);
        // if this message occurs a lot, it means we need to generate more sectors
        return ES_FAIL;
    }

    // Add the pattern
    pattCands.clear();
    pattCands.push_back({ sector, 1, patt }); // 1 = coverage
    return ES_OK;
}



/*****************************************************************************/
/* Helpers                                                                   */
/*****************************************************************************/

bool HTTPattGenTool_Truth::checkAllowedWCs(pattern_t patt) const
{
    for (size_t layer = 0; layer < m_nLayers; layer++)
    {
        if (patt[layer] == SSID_WILDCARD && !m_WCAllowed[layer])
            return false;
    }
    return true;
}
