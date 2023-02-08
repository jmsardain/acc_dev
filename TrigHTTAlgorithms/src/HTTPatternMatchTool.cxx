/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file HTTPatternMatchTool.cxx
 * @author Riley Xu - riley.xu@cern.ch
 * @date October 22nd, 2019
 * @brief See header file.
 */

#include "TrigHTTAlgorithms/HTTPatternMatchTool.h"
#include "TrigHTTUtils/HTTMacros.h"
#include "TrigHTTMaps/ITrigHTTMappingSvc.h"
#include "TrigHTTMaps/HTTSSMap.h"
#include "TrigHTTBanks/ITrigHTTBankSvc.h"
#include "TrigHTTBanks/HTTPatternBank.h"


#include <limits>

///////////////////////////////////////////////////////////////////////////////
// AthAlgTool

HTTPatternMatchTool::HTTPatternMatchTool(const std::string& algname, const std::string &name, const IInterface *ifc) :
    AthAlgTool(algname, name, ifc),
    m_HTTBankSvc("TrigHTTBankSvc", name),
    m_HTTMapping("TrigHTTMappingSvc", name)
{
    declareInterface<HTTRoadFinderToolI>(this);
    declareProperty("TrigHTTBankSvc", m_HTTBankSvc);
    declareProperty("max_misses", m_maxMissingLayers, "Max allowed number of missing layers for a pattern to still fire");
    declareProperty("min_hitlayer", m_minHitLayers, "Min allowed number of real hit layers for a pattern to still fire");
    //declareProperty("region", m_region, "");
}

StatusCode HTTPatternMatchTool::initialize()
{
    ATH_CHECK(m_HTTBankSvc.retrieve());
    ATH_CHECK(m_HTTMapping.retrieve());
    m_bank = m_HTTBankSvc->PatternBank();
    m_nLayers = m_HTTMapping->PlaneMap_1st()->getNLogiLayers();

    bool ok = false;
    if (m_nLayers > std::numeric_limits<layer_bitmask_t>::digits)
        ATH_MSG_ERROR("initialize() m_nLayers " << m_nLayers << " too large for bitmask ("
                << std::numeric_limits<layer_bitmask_t>::digits << " bits)");
    else if (!m_bank)
        ATH_MSG_ERROR("initialize() retrieved pattern bank was null");
    else
        ok = true;
    if (!ok) return StatusCode::FAILURE;


    // Update bitmasks with wildcards
    processWildcards();

    return StatusCode::SUCCESS;
}

StatusCode HTTPatternMatchTool::finalize()
{
    return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// HTTRoadFinderToolI

StatusCode HTTPatternMatchTool::getRoads(const std::vector<const HTTHit*> & hits, std::vector<HTTRoad*> & roads)
{
    size_t nPatterns = m_bank->getMetadata().nPatterns;

    // Reset accumulators
    roads.clear();
    m_roads.clear();
    m_masks_hit.clear();
    m_masks_hit.resize(nPatterns);
    m_pattern_hits.clear();

    // Find ssids of hits
    std::map<ssid_t, std::vector<const HTTHit*>> ssid_map;
    for (const HTTHit* hit : hits)
    {
        ssid_t ssid = m_HTTMapping->SSMap()->getSSID(*hit);
        ssid_map[ssid].push_back(hit);
    }

    // Main algo. Update accumulators for each ssid fired
    updateAccumulators(ssid_map);

    // Create roads from patterns with sufficient hits
    for (size_t pid = 0; pid < nPatterns; pid++)
    {
      if ( (__builtin_popcount(m_masks_hit[pid] | m_masks_wc[pid]) >= m_nLayers - m_maxMissingLayers)  && __builtin_popcount(m_masks_hit[pid]) >= m_minHitLayers)
        {
            auto hits = ::sortByLayer(m_pattern_hits[pid]);
            hits.resize(m_nLayers); // If no hits in last layer, return from sortByLayer will be too short

            m_roads.push_back({});
            m_roads.back().setRoadID(m_nRoads++);
            m_roads.back().setPID(pid);
            m_roads.back().setSector(m_pid2sector_map[pid]);
            m_roads.back().setHitLayers(m_masks_hit[pid]);
            m_roads.back().setWCLayers(m_masks_wc[pid]);            
            m_roads.back().setHits(hits);

            // we are not setting the wildcard type, but witth const * we cannot 
            /*
            for (const HTTHit* hit : hits)
            {
              if (m_masks_wc[pid] & (1 << hit->getLayer() ) )
                hit->setHitType(HitType::wildcard) 
            }
            */
        }
    }

    // Fill the output vector
    roads.reserve(m_roads.size());
    for (HTTRoad & r : m_roads) roads.push_back(&r);

    return StatusCode::SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
// Helpers

// Updates m_masks_wc bitmask with all patterns with wildcards.
// This only needs to be run once per bank during initialization.
void HTTPatternMatchTool::processWildcards()
{
    m_masks_wc.clear();
    m_masks_wc.resize(m_bank->getMetadata().nPatterns);

    for (size_t layer = 0; layer < (size_t)m_nLayers; layer++)
    {
        size_t iSSID = m_bank->getISSID(layer, SSID_WILDCARD);
        if (iSSID == m_bank->getNSSIDs(layer)) continue; // SSID_WILDCARD not in bank

        layer_bitmask_t layer_bit = 1 << layer;
        for (size_t iSector = 0; iSector < m_bank->getNSectors(layer, iSSID); iSector++)
        {            
            vector<pid_t> pids = m_bank->getPatternIDs(layer, iSSID, iSector);
            for (pid_t pid : pids) m_masks_wc[pid] |= layer_bit;            
        }
    }
    
}


// Updates m_pattern_hits and m_masks_hit for all pids containing ssid.
void HTTPatternMatchTool::updateAccumulators(std::map<ssid_t, std::vector<const HTTHit*>> const & ssid_map)
{
    for (const auto & it : ssid_map)
    {
        // Use first hit to get the layer of the ssid
        if (it.second.empty()) throw "No hits for a fired SSID?";
        size_t layer = it.second.front()->getLayer();

        // Get all PIDs in all sectors that have this ssid
        size_t iSSID = m_bank->getISSID(layer, it.first);

        if (iSSID == m_bank->getNSSIDs(layer)) continue; // SSID not in bank

        for (size_t iSector = 0; iSector < m_bank->getNSectors(layer, iSSID); iSector++)
        {
            vector<pid_t> pids = m_bank->getPatternIDs(layer, iSSID, iSector);
            for (pid_t pid : pids)
            {
                m_masks_hit[pid] |= 1 << layer;
                m_pattern_hits[pid].insert(m_pattern_hits[pid].end(), it.second.begin(), it.second.end());
                m_pid2sector_map[pid] = m_bank->getSector(layer, iSSID, iSector);
            }
        }
    }
}

