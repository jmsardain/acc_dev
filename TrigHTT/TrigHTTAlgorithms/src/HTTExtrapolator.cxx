/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file HTTExtrapolator.cxx
 * @author Chris Delitzsch - chris.malena.delitzsch@cern.ch
 * @date March 17, 2021
 * @brief See header file.
 */

#include "TrigHTTAlgorithms/HTTExtrapolator.h"
#include "TrigHTTUtils/HTTMacros.h"
#include "TrigHTTMaps/ITrigHTTMappingSvc.h"
#include "TrigHTTMaps/HTTSSMap.h"
#include "TrigHTTBanks/ITrigHTTBankSvc.h"
#include "TrigHTTBanks/HTTPatternBank.h"
#include "TrigHTTMaps/HTTConnectionMap.h"

#include <limits>


///////////////////////////////////////////////////////////////////////////////
// AthAlgTool

HTTExtrapolator::HTTExtrapolator(std::string const & algname, std::string const & name, IInterface const * ifc) :
  AthAlgTool(algname, name, ifc),
  m_HTTBankSvc("TrigHTTBankSvc", name),
  m_HTTMapping("TrigHTTMappingSvc", name)
{
  declareProperty("TrigHTTBankSvc", m_HTTBankSvc);
  declareProperty("Ncombinations", m_NCombMax);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTExtrapolator::initialize()
{
  ATH_CHECK(m_HTTBankSvc.retrieve());
  ATH_CHECK(m_HTTMapping.retrieve());

  m_nLayers_2nd      = m_HTTMapping->PlaneMap_2nd()->getNLogiLayers();
  m_constantBank_2nd = m_HTTBankSvc->FitConstantBank_2nd();

  if (!m_constantBank_2nd) {
    ATH_MSG_FATAL("Constants needed for second stage don't exist.");
    return StatusCode::FAILURE;
  }

  m_sectors_2nd = m_HTTBankSvc->SectorBank_2nd();

  /*
    Need to determine the layers fo the second stage that correspond to first stage
    This is taken from HTTMatrixReductionAlgo, maybe there's an easier way to retrieve information
    without copying the code and without looping twice
  */

  m_pmap_1st = m_HTTMapping->PlaneMap_1st();
  m_pmap_2nd = m_HTTMapping->PlaneMap_2nd();

  layers_1st_to_2nd.reserve(m_pmap_1st->getNLogiLayers());

  for (unsigned layer_1st = 0; layer_1st < m_pmap_1st->getNLogiLayers(); layer_1st++) {
    LayerInfo const & info_1st = m_pmap_1st->getLayerInfo(layer_1st, 0);
    LayerSection const & ls_2nd = m_pmap_2nd->getLayerSection(info_1st);
    layers_1st_to_2nd[layer_1st] = ls_2nd.layer;
  }

  // Now revert the map
  layers_2nd_to_1st.reserve(m_pmap_2nd->getNLogiLayers());

  for (unsigned layer_2nd = 0; layer_2nd < m_pmap_2nd->getNLogiLayers(); layer_2nd++) {
    unsigned match_layer_1st = 999;

    for (unsigned layer_1st = 0; layer_1st < m_pmap_1st->getNLogiLayers(); layer_1st++) {
      if (layers_1st_to_2nd[layer_1st] == layer_2nd) {
        match_layer_1st = layer_1st;
      }
    }

    layers_2nd_to_1st[layer_2nd] = match_layer_1st;
  }

  return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTExtrapolator::finalize()
{
  return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTExtrapolator::getSecondStageFittingInputs(std::vector<HTTHit const *> const & phits_2nd, std::vector<HTTTrack> const & tracks_1st_passOR, std::vector<HTTRoad*> & roads_2nd)
{
  ATH_MSG_DEBUG("Extrapolation for second stage");

  // Map the list of all hits to modules:
  std::map<module_t, std::vector<const HTTHit*>> module_hit_map;
  for (HTTHit const * hit : phits_2nd) {
    module_t module = hit->getHTTIdentifierHash();
    module_hit_map[module].push_back(hit);
  }

  m_roads_2nd.clear();
  roads_2nd.clear();

  std::unordered_map<int, size_t> mult_sectorID_2nd;

  for (auto track : tracks_1st_passOR) {

    // ------------------------------------
    // (0) Get the hits from the first stage track
    // ------------------------------------
    const std::vector<HTTHit> track_hits_1st = track.getHTTHits();

    layer_bitmask_t masks_hit_1st = 0;
    layer_bitmask_t masks_wildcard_1st = 0;
    std::vector<std::vector<const HTTHit*> > hits_for_road_1st = getFirstStageHits(track_hits_1st, phits_2nd, masks_hit_1st, masks_wildcard_1st);

    // ------------------------------------
    // (1) Get the second stage sector IDs associated to the first stage
    // ------------------------------------
    sector_t sectorID = track.getFirstSectorID();
    std::vector<uint32_t> secondStageIDs = m_HTTMapping->ConnectionMap()->getSecondSectorIDs(sectorID);

    size_t nComb = secondStageIDs.size();

    ATH_MSG_DEBUG("Found " << nComb << " second stage sector IDs for first stage sector ID = " << sectorID);

    for (uint32_t secondStageID : secondStageIDs) {
      mult_sectorID_2nd[secondStageID]++;
    }

    if (nComb > m_NCombMax) nComb = m_NCombMax; // Pick only the first eight second stage sector IDs

    // ------------------------------------  
    // (2) For each second stage sector ID, extrapolate the track to the additional layers;
    // ------------------------------------
    for (unsigned int iComb = 0; iComb < nComb; iComb++) {

      int sector_2nd = secondStageIDs[iComb];

      if (sector_2nd > m_constantBank_2nd->getNSectors()) {
        ATH_MSG_FATAL("Constants for sector " << sector_2nd << " don't exist");
        return StatusCode::FAILURE;
      }
      else if (!m_constantBank_2nd->getIsGood(sector_2nd)) {
        ATH_MSG_FATAL("Constants for sector " << sector_2nd << " are not valid");
        return StatusCode::FAILURE;
      }

      // Get the hits corresponding to the second stage sector ID (per layer)
      std::vector<std::vector<const HTTHit*>> sector_hits = getSectorHits(module_hit_map, sector_2nd);

      int counter_layers = 0;
      for (unsigned int il = 0; il < m_pmap_2nd->getNLogiLayers(); il++) {
        if (layers_2nd_to_1st[il] == 999 && sector_hits[il].size() == 0) {
          counter_layers++;
        }
      }

      // If there's more than one layer without hits, ignore combination
      if (counter_layers > 1) continue;

      // Define new track for extrapolation into other layers
      HTTTrack temp;
      temp.setNLayers(m_HTTMapping->PlaneMap_2nd()->getNLogiLayers());
      temp.setBankID(-1);
      temp.setPatternID(track.getPatternID());
      temp.setFirstSectorID(track.getFirstSectorID());
      temp.setSecondSectorID(sector_2nd);
      temp.setTrackCorrType(TrackCorrType::None);
      temp.setQOverPt(0.0);

      // Fill the hits and bit map from the first stage
      layer_bitmask_t missing_mask = 0;

      for (unsigned int layer = 0; layer < m_pmap_2nd->getNLogiLayers(); layer++) {
      	if (layers_2nd_to_1st[layer] == 999) {
      	  temp.setHTTHit(layer, HTTHit());
      	}
      	else {
      	  temp.setHTTHit(layer, track_hits_1st[layers_2nd_to_1st[layer]]);

      	  int ix = m_pmap_2nd->getCoordOffset(layer);
      	  int iy = ix + 1;
      	  if (m_pmap_2nd->isSCT(layer))
      	    missing_mask |= 1 << ix;
      	  else
      	    missing_mask |= (1<<ix) | (1<<iy);
      	}
      }

      temp.setHitMap(missing_mask);

      // Guess the missing hits in the additional second-stage layers (extrapolation)
      m_constantBank_2nd->missing_point_guess(sector_2nd, temp, true, true);

      // Get the hits of the track (includes the extrapolated coordinates)
      std::vector<HTTHit> track_hits_all_layers = temp.getHTTHits();

      // Select hits close to the extrapolation point
      layer_bitmask_t masks_hit = masks_hit_1st;

      //Get the hits in additional layers close to extrapolation point
      std::vector<std::vector<const HTTHit*> > hits_for_road = getHitsForRoad(hits_for_road_1st, sector_hits, track_hits_all_layers, masks_hit);

      // Don't consider road if we have more than one second stage layers with zero hits:
      int counter_missing = 0;
      for (unsigned int j = 0 ; j < hits_for_road.size(); j++) {
        if(layers_2nd_to_1st[j] == 999 && hits_for_road[j].size() == 0) {
          counter_missing++;
        }
      }

      // Build the road! (only one missing hit possible)
      if (hits_for_road.size() > 0 && counter_missing < 2) {
        m_roads_2nd.push_back({});
        m_roads_2nd.back().setRoadID(m_nRoads_2nd++);
        m_roads_2nd.back().setSector(sector_2nd);
        m_roads_2nd.back().setHitLayers(masks_hit);
        m_roads_2nd.back().setWCLayers(masks_wildcard_1st);
        m_roads_2nd.back().setHits(hits_for_road);
      }
    } // End loop over all possible combinations
  }

  roads_2nd.reserve(m_roads_2nd.size());
  for (HTTRoad & r : m_roads_2nd) roads_2nd.push_back(&r);

  ATH_MSG_DEBUG("HTTExtrapolator::getSecondStageFittingInputs Number of roads for second stage fitting " << roads_2nd.size());

  m_nConstants_extrapolate = mult_sectorID_2nd.size();

  return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
std::vector<std::vector<const HTTHit*> > HTTExtrapolator::getHitsForRoad(std::vector<std::vector<const HTTHit*> > hits_1st, std::vector<std::vector<const HTTHit*>> sector_hits, std::vector<HTTHit> track_hits, layer_bitmask_t & masks_hit) {

  std::vector<std::vector<const HTTHit*> > hits_for_road = hits_1st;

  // Previous implementation
  /*

  for (unsigned int il = 0; il < sector_hits.size(); il++){
    if(layers_2nd_to_1st[il] == 999){

      if (il >= hits_for_road.size()) hits_for_road.resize(il + 1);

      for(unsigned int ih = 0; ih < sector_hits[il].size(); ih++){
        hits_for_road[il].push_back(sector_hits[il].at(ih));
        masks_hit |= 1 << il;
      }
    }
  }
  */

  //Select the hits close to the extrapolation point, these are just dummy values
  float window_SCT = 5;
  float X = 30;
  float Y = 40;

  // Get the hits corresponding to the unused layers for specific second stage sector:
  for (unsigned int layer = 0; layer < m_pmap_2nd->getNLogiLayers(); layer++) {
    if (layers_2nd_to_1st[layer] == 999) {
      ATH_MSG_DEBUG("Number of hits matching second stage sector ID : " << sector_hits[layer].size() << " in layer " << layer);
      if (m_pmap_2nd->isSCT(layer)) {
        float phiCoord_extrap = track_hits[layer].getPhiCoord();

        ATH_MSG_DEBUG("Extrapolated phi coordinate " << phiCoord_extrap);

        if (phiCoord_extrap == -1) continue;

        for (unsigned int i = 0; i < sector_hits[layer].size(); i++) {
          float phiCoord_realHit = sector_hits[layer].at(i)->getPhiCoord();

          if (std::abs(phiCoord_realHit - phiCoord_extrap) < window_SCT) {
            ATH_MSG_DEBUG("Found a close-by hit with phi = " << phiCoord_realHit << " compared to " << phiCoord_extrap);
            if (layer >= hits_for_road.size()) hits_for_road.resize(layer + 1);
            hits_for_road[layer].push_back(sector_hits[layer].at(i));
            masks_hit |= 1 << layer;
          }
        }
      }
      else {
        float phiCoord_extrap = track_hits[layer].getPhiCoord();
        float etaCoord_extrap = track_hits[layer].getEtaCoord();

        ATH_MSG_DEBUG("Extrapolated phi coordinate " << phiCoord_extrap << " and eta coordinate " << etaCoord_extrap);

        if(phiCoord_extrap == -1 && etaCoord_extrap == -1)
          continue;

        for(unsigned int i = 0; i < sector_hits[layer].size(); i++){
          float phiCoord_realHit = sector_hits[layer].at(i)->getPhiCoord();
          float etaCoord_realHit = sector_hits[layer].at(i)->getEtaCoord();

          if(std::abs(phiCoord_realHit - phiCoord_extrap) < X){
            if(std::abs(etaCoord_realHit - etaCoord_extrap) < Y){
              ATH_MSG_DEBUG("Found a close-by hit with phi = " << phiCoord_realHit << " and eta = " << etaCoord_realHit << " compared to " << phiCoord_extrap << " " << etaCoord_extrap);
              if (layer >= hits_for_road.size()) hits_for_road.resize(layer + 1);
              hits_for_road[layer].push_back(sector_hits[layer].at(i));
              masks_hit |= 1 << layer;
            }
          }
        }
      }
    }
  }

  return hits_for_road;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
std::vector< std::vector<HTTHit const *> > HTTExtrapolator::getFirstStageHits(std::vector<HTTHit> const & track_hits_1st, std::vector<HTTHit const *> const & phits_2nd, layer_bitmask_t & masks_hit, layer_bitmask_t & masks_wildcard) {

  std::vector< std::vector<HTTHit const *> > hits_1st;

  for (auto hit_1st : track_hits_1st) {

    size_t layer_2nd = layers_1st_to_2nd[hit_1st.getLayer()];
    
    if (layer_2nd >= hits_1st.size()) hits_1st.resize(layer_2nd + 1);

    if (hit_1st.getHitType() == HitType::wildcard) {
      masks_wildcard |= 1 << layer_2nd;
    }
    else if (hit_1st.getHitType() != HitType::guessed) {
      for (HTTHit const * hit : phits_2nd) {
        if(hit->getHTTIdentifierHash() == hit_1st.getHTTIdentifierHash()) {
          if(hit_1st.getPhiCoord() == hit->getPhiCoord() && hit_1st.getEtaCoord() == hit->getEtaCoord()) {
            hits_1st[layer_2nd].push_back(hit);
            masks_hit |= 1 << layer_2nd;
          }
        }
      }
    }
  }

  return hits_1st;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// Copied for the moment from HTTSectorMatchTool
std::vector<std::vector<const HTTHit*>> HTTExtrapolator::getSectorHits(std::map< module_t, std::vector<HTTHit const *> > const & module_hit_map, sector_t sector) const
{
  std::vector<std::vector<const HTTHit*>> sector_hits(m_nLayers_2nd);

  for (auto const & it : module_hit_map) {
    module_t module = it.first;
    std::vector<const HTTHit*> const & hits = it.second;

    ATH_MSG_DEBUG("Looking at module " << module << " and how many hits " << hits.size() << " and the layer " << hits.front()->getLayer() );

    // Use first hit to get the layer of the module
    if (hits.empty()) throw "No hits for a fired module?";
    size_t layer = hits.front()->getLayer();

    // ATH_MSG_DEBUG("Which layer " << layer);
    if ((m_sectors_2nd->getModules(sector)).size() == 0) {
      ATH_MSG_DEBUG("I couldn't find anything for sector = " << sector);
      continue;
    }

    ATH_MSG_DEBUG("Sector : " << sector << " ... Looking at hit module " << module << " and the corresponding sector module " <<  m_sectors_2nd->getModules(sector)[layer]);

    // Add hits to list if module is in sector
    if (module == m_sectors_2nd->getModules(sector)[layer]) {
      sector_hits[layer].insert(sector_hits[layer].end(), hits.begin(), hits.end());
    }
  }
  
  return sector_hits;
}

