/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file HTTSectorMatchTool.cxx
 * @author Riley Xu - rixu@cern.ch
 * @date October 22nd, 2019
 * @brief See header file.
 */

#include "TrigHTTAlgorithms/HTTSectorMatchTool.h"
#include "TrigHTTUtils/HTTMacros.h"
#include "TrigHTTMaps/ITrigHTTMappingSvc.h"
#include "TrigHTTMaps/HTTSSMap.h"
#include "TrigHTTBanks/ITrigHTTBankSvc.h"
#include "TrigHTTBanks/HTTPatternBank.h"


#include <limits>

///////////////////////////////////////////////////////////////////////////////
// AthAlgTool

HTTSectorMatchTool::HTTSectorMatchTool(const std::string& algname, const std::string &name, const IInterface *ifc) :
    AthAlgTool(algname, name, ifc),
    m_HTTBankSvc("TrigHTTBankSvc", name),
    m_HTTMapping("TrigHTTMappingSvc", name)
{
    declareInterface<HTTRoadFinderToolI>(this);
    declareProperty("TrigHTTBankSvc", m_HTTBankSvc);
    declareProperty("max_misses", m_maxMissingLayers, "Max allowed number of missing layers for a pattern to still fire; default 0");
}

StatusCode HTTSectorMatchTool::initialize()
{
    ATH_CHECK(m_HTTBankSvc.retrieve());
    ATH_CHECK(m_HTTMapping.retrieve());

    m_nLayers = m_HTTMapping->PlaneMap_1st()->getNLogiLayers();
    m_sectors = m_HTTBankSvc->SectorBank_1st();

    bool ok = false;
    if (m_nLayers > std::numeric_limits<layer_bitmask_t>::digits)
        ATH_MSG_ERROR("initialize() m_nLayers " << m_nLayers << " too large for bitmask ("
                << std::numeric_limits<layer_bitmask_t>::digits << " bits)");
    else if (!m_sectors)
        ATH_MSG_ERROR("initialize() sector bank is null");
    else
        ok = true;
    if (!ok) return StatusCode::FAILURE;

    processWildcards();

    return StatusCode::SUCCESS;
}

StatusCode HTTSectorMatchTool::finalize()
{
    return StatusCode::SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
// HTTRoadFinderToolI

StatusCode HTTSectorMatchTool::getRoads(const std::vector<const HTTHit*> & hits, std::vector<HTTRoad*> & roads)
{
    roads.clear();
    m_roads.clear();

    // Find modules of hits
    std::map<module_t, std::vector<const HTTHit*>> module_hit_map;
    for (const HTTHit* hit : hits)
    {
        module_t module = hit->getHTTIdentifierHash();
        module_hit_map[module].push_back(hit);
    }

    // Create the layer bitmasks for hit layers in each sector
    std::vector<layer_bitmask_t> hit_masks = createBitmasks(module_hit_map);

    // Create roads from sectors with sufficient hits
    for (size_t sector = 0; sector < m_sectors->getNSectors(); sector++)
        if (__builtin_popcount(hit_masks[sector] | m_masks_wc[sector]) >= m_nLayers - m_maxMissingLayers)
            m_roads.emplace_back(m_nRoads++, sector, sector, hit_masks[sector], m_masks_wc[sector], getSectorHits(module_hit_map, sector));

    // Fill the pointers
    roads.reserve(m_roads.size());
    for (HTTRoad & r : m_roads) roads.push_back(&r);

    return StatusCode::SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
// Helpers


// Updates m_masks_wc bitmask with all sectors with wildcards.
// This only needs to be run once during initialization.
void HTTSectorMatchTool::processWildcards()
{
    m_masks_wc.clear();
    m_masks_wc.resize(m_sectors->getNSectors());

    for (size_t layer = 0; layer < m_nLayers; layer++)
    {
        std::vector<sector_t> sectors = m_sectors->getSectors(layer, MODULE_BADMODULE);
        for (sector_t sector : sectors)
            m_masks_wc[sector] |= 1 << layer;
    }
}


// Creates the layer bitmasks for hit sector layers from the module hits in module_hit_map.
std::vector<layer_bitmask_t> HTTSectorMatchTool::createBitmasks(std::map<module_t, std::vector<const HTTHit*>> const & module_hit_map) const
{
    // Bitmask for hit layers for each sector. Index by sector.
    std::vector<layer_bitmask_t> hit_masks(m_sectors->getNSectors());

    for (const auto & it : module_hit_map)
    {
        module_t module = it.first;
        std::vector<const HTTHit*> const & hits = it.second;

        // Use first hit to get the layer of the module
        if (hits.empty()) throw "No hits for a fired module?";
        size_t layer = hits.front()->getLayer();

        // Get all sectors that have this module
        std::vector<sector_t> sectors = m_sectors->getSectors(layer, module);
        for (sector_t sector : sectors)
            hit_masks[sector] |= 1 << layer;
    }

    return hit_masks;
}


// Returns a list of hits in the sector from module_hit_map, sorted by layer
std::vector<std::vector<const HTTHit*>> HTTSectorMatchTool::getSectorHits(
        std::map<module_t, std::vector<const HTTHit*>> const & module_hit_map, sector_t sector) const
{
    std::vector<std::vector<const HTTHit*>> sector_hits(m_nLayers);

    for (const auto & it : module_hit_map)
    {
        module_t module = it.first;
        std::vector<const HTTHit*> const & hits = it.second;

        // Use first hit to get the layer of the module
        if (hits.empty()) throw "No hits for a fired module?";
        size_t layer = hits.front()->getLayer();

        // Add hits to list if module is in sector
        if (module == m_sectors->getModules(sector)[layer])
            sector_hits[layer].insert(sector_hits[layer].end(), hits.begin(), hits.end());
    }

    return sector_hits;
}
