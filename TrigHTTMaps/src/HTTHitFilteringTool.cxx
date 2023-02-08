/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTMaps/HTTHitFilteringTool.h"
#include "PathResolver/PathResolver.h"
#include "TrigHTTMaps/geometry_constants.h"
#include "TrigHTTObjects/HTTMultiTruth.h"
#include "TrigHTTObjects/HTTCluster.h"
#include "TrigHTTMaps/HTTClusteringTool.h"

#include <vector>
#include <fstream>
#include <cmath>
#include <iostream>
#include <sstream>
#include <boost/algorithm/string.hpp>

void fill_nearby_hits(HTTHit, std::vector<std::pair<HTTHit,unsigned>> &, std::vector<std::pair<HTTHit,unsigned>> &);
int  countHitsFromHeader(HTTLogicalEventInputHeader &);
void printHitsFromHeader(HTTLogicalEventInputHeader &);

HTTHitFilteringTool::HTTHitFilteringTool(const std::string &algname, const std::string &name, const IInterface *ifc)
  : base_class(algname, name, ifc)
{
  declareInterface<HTTHitFilteringToolI>(this);
  declareProperty("doRandomRemoval",       m_doRandomRemoval,    "remove hits/clusters at random");
  declareProperty("pixelHitRmFrac",        m_rndPixelHitRmFrac,  "fraction of pixel hits to randomly remove");
  declareProperty("stripHitRmFrac",        m_rndStripHitRmFrac,  "fraction of strip hits to randomly remove");
  declareProperty("pixelClusRmFrac",      m_rndPixelClustRmFrac, "fraction of pixel clusters to randomly remove");
  declareProperty("stripClusRmFrac",      m_rndStripClustRmFrac, "fraction of strip clusters to randomly remove");
  declareProperty("doStubs",            m_doStubs, "flag to enable dphi stub hit filtering");
  declareProperty("stubCutsFile",       m_stubCutsFile,       "read in stub cuts from this file");
  declareProperty("barrelStubDphiCut",  m_barrelStubDphiCut,  "barrel stub dPhi cut. Overridden by stubCutsFile");
  declareProperty("endcapStubDphiCut",  m_endcapStubDphiCut,  "endcap stub dPhi cut. Overridden by stubCutsFile");
  declareProperty("useNstrips",         m_useNstrips,       "use nStrips instead of dPhi for stub filtering");
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTHitFilteringTool::initialize()
{
  ATH_MSG_INFO("HTTHitFilteringTool::initialize()");

  // random removal
  if(m_rndStripHitRmFrac != 0 || m_rndStripClustRmFrac != 0 || m_rndPixelHitRmFrac != 0 || m_rndPixelClustRmFrac != 0)
    m_doRandomRemoval = true;
  if(m_doRandomRemoval) {
    m_random.SetSeed(1); // for reproducibility
    ATH_MSG_INFO("Doing random removal with pixel hit, cluster = " << m_rndPixelHitRmFrac << ", " << m_rndPixelClustRmFrac << " and strip = " << m_rndStripHitRmFrac << ", " << m_rndStripClustRmFrac);
  }

  // stubs
  if(m_doStubs) {
    ATH_MSG_INFO("Doing stub filtering");

    if(m_stubCutsFile != "") {
      // look for it via absolute path
      ATH_MSG_INFO("looking for stub cuts file " << m_stubCutsFile);
      std::ifstream inFile;
      inFile.open(m_stubCutsFile);
      if(inFile) {
        ATH_MSG_INFO("reading in from there");
      }
      else { // if not there, try PathResolver (might be in TrigHttHitFiltering/config/)
        std::string stubCutsFile = PathResolver::find_file(m_stubCutsFile, "DATAPATH");
        inFile.open(stubCutsFile);
        if(inFile) {
          ATH_MSG_INFO("read in " << stubCutsFile);
        }
        else {
          ATH_MSG_ERROR("Couldn't find it");
          return StatusCode::FAILURE;
        }
      }

      std::string line;

      while(!inFile.eof()) {
        std::getline(inFile, line);
        if(line.substr(0,1) == "#" || line == "")
          continue;
        std::vector<std::string> strs, cuts_str;
        boost::split(strs, line, boost::is_any_of("\t"));
        if(m_useNstrips)
          boost::split(cuts_str, strs[4], boost::is_any_of(","));
        else
          boost::split(cuts_str, strs[3], boost::is_any_of(","));
        std::pair<float,float> cuts;
        cuts.first = std::stof(cuts_str[0]);
        cuts.second = std::stof(cuts_str[1]);
        m_stubCutMap[strs[0]][std::stoi(strs[1])][std::stoi(strs[2])] = cuts;
      }
    }

    else {
      // populate it with uniform values
      ATH_MSG_INFO("using global values for stub cuts");

      if(m_useNstrips) {
        ATH_MSG_ERROR("you asked to use Nstrips in stub selection, but that only works when reading in a file (defaults not set)");
        return StatusCode::FAILURE;
      }
      for(int i=0; i<4; i++) {
        for(int j=-14; j<14; j++) {
          m_stubCutMap["bar"][i][j] = std::pair<float,float>(-m_barrelStubDphiCut, m_barrelStubDphiCut);
        }
      }
      for(int i=0; i<6; i++) {
        for(int j=0; j<6; j++) {
          m_stubCutMap["EC+"][i][j] = std::pair<float,float>(-m_endcapStubDphiCut, m_endcapStubDphiCut);
          m_stubCutMap["EC-"][i][j] = std::pair<float,float>(-m_endcapStubDphiCut, m_endcapStubDphiCut);
        }
      }
    }

    // print out the stubCutMap
    if(m_useNstrips)
      ATH_MSG_INFO("using stub cut map (nStrips):");
    else
      ATH_MSG_INFO("using stub cut map (dPhi):");
    for(auto zone_map : m_stubCutMap) {
      for(auto layer_map : zone_map.second) {
        for(auto ring_map : layer_map.second) {
          std::cout << "  " << zone_map.first;
          std::cout << "  " << layer_map.first;
          std::cout << "  " << ring_map.first;
          std::cout << "  " << ring_map.second.first << ", " << ring_map.second.second << std::endl;
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode HTTHitFilteringTool::finalize()
{
    ATH_MSG_INFO("HTTHitFilteringTool::finalize()");
    return StatusCode::SUCCESS;
}


StatusCode HTTHitFilteringTool::DoRandomRemoval(HTTLogicalEventInputHeader &header, bool hit_or_cluster)
{
  if(!m_doRandomRemoval)
    return StatusCode::SUCCESS;

  float pixelFrac = hit_or_cluster ? m_rndPixelHitRmFrac : m_rndPixelClustRmFrac;
  float stripFrac = hit_or_cluster ? m_rndStripHitRmFrac : m_rndStripClustRmFrac;

  if(pixelFrac==0 && stripFrac==0)
    return StatusCode::SUCCESS;

  for (int i = 0; i < header.nTowers(); ++i) {
    HTTTowerInputHeader &tower = *header.getTower(i);
    std::vector<HTTHit>  hits;
    hits = tower.hits();
    attachTruth(hits); // not sure if this is needed

    std::vector<HTTHit> filteredHits;
    for(auto hit : hits) {
      if(hit.isStrip()) {
        if(m_random.Rndm() >= stripFrac)
          filteredHits.push_back(hit);
      }
      else {
        if(m_random.Rndm() >= pixelFrac)
          filteredHits.push_back(hit);
      }
    }

    tower.clearHits();
    for (auto hit : filteredHits) {
      tower.addHit(hit);
    }

    std::string hit_or_cluster_str = hit_or_cluster ? "hit" : "cluster";
    ATH_MSG_INFO("After random " << hit_or_cluster_str << " removal - (pixel, strip) = (" << pixelFrac << ", " << stripFrac << ") - have " << filteredHits.size() << " left of " << hits.size() << " " << hit_or_cluster_str << "s");

  }
  return StatusCode::SUCCESS;
}



StatusCode HTTHitFilteringTool::GetPairedStripPhysLayers(const HTTPlaneMap *planeMap, std::vector<int> &paired_strip_layers)
{
  std::vector<int> all_strip_layers;

  for(int logiLayer = 0; logiLayer < int(planeMap->getNLogiLayers()); logiLayer++) {
    for(int section = 0; section < int(planeMap->getNSections(logiLayer)); section++) {
      LayerInfo thisLayerInfo = planeMap->getLayerInfo(logiLayer,section);
      // std::cout << logiLayer << ", " << section << ": " << thisLayerInfo.siTech << ", " << thisLayerInfo.zone << ", " << thisLayerInfo.physLayer << ", " << thisLayerInfo.stereo << std::endl;
      if(thisLayerInfo.siTech ==  SiliconTech::pixel) {
        continue;
      }
      else if (thisLayerInfo.siTech ==  SiliconTech::strip) {
        all_strip_layers.push_back(thisLayerInfo.physLayer);
        // check assumptions about physLayer and stereo parity
        if((thisLayerInfo.physLayer - thisLayerInfo.stereo) % 2  != 0) {
          ATH_MSG_ERROR("expected parity of strip physLayer and stereo doesn't hold: " <<
                          logiLayer << ", " << section << ": " << thisLayerInfo.siTech <<
                          ", " << thisLayerInfo.zone << ", " << thisLayerInfo.physLayer <<
                          ", " << thisLayerInfo.stereo);
          return StatusCode::FAILURE;
        }

      }
      else {
        ATH_MSG_WARNING("siTech not in (pixel,strip)");
        continue;
      }
    }
  }
  // need both stereo sides
  // for each odd one, need x-1, then odd one is good
  // for each even one, need x+1, then even one is good
  for(auto physLayer : all_strip_layers) {
    if(physLayer % 2 == 1) {
      if(std::find(all_strip_layers.begin(), all_strip_layers.end(), physLayer-1) != all_strip_layers.end())
        paired_strip_layers.push_back(physLayer);
    }
    else {
      if(std::find(all_strip_layers.begin(), all_strip_layers.end(), physLayer+1) != all_strip_layers.end())
        paired_strip_layers.push_back(physLayer);
    }
  }
  return StatusCode::SUCCESS;
}


StatusCode HTTHitFilteringTool::DoHitFiltering(HTTLogicalEventInputHeader &header,
                                               std::vector<int> filter_pixel_physLayers,
                                               std::vector<int> filter_strip_physLayers,
                                               std::vector<HTTCluster> &filteredClusters)
{

  int nHits_preFilt = countHitsFromHeader(header);

  for (int i = 0; i < header.nTowers(); ++i) {
    HTTTowerInputHeader &tower = *header.getTower(i);
    std::vector<HTTHit>  hits; // are these hits or clusters? I think they are clusters?
    hits = tower.hits();
    attachTruth(hits);

    std::vector<HTTHit> filteredHits;
    FilterHits(hits, filter_pixel_physLayers, filter_strip_physLayers, filteredHits);

    // replace old clusters with new ones (the things called hits here are actually clusters)
    tower.clearHits();
    for (auto hit : filteredHits) {
      tower.addHit(hit);
      // make HTTCluster for monitoring
      HTTCluster cluster;
      if(hit.isPixel()){
	updatePixelCluster(cluster, hit, true);
      }
      if(hit.isStrip()){
	updateStripCluster(cluster, hit, true);
      }
      filteredClusters.push_back(cluster);
    }
  }

  int nHits_postFilt = countHitsFromHeader(header);

  if(m_doStubs) { // random removal doesn't impact this so rather boring output
    if(nHits_postFilt == nHits_preFilt) {
      ATH_MSG_INFO("Stub filtering had no impact: " << nHits_preFilt << " hits/clusters");
    }
    else if(nHits_postFilt < nHits_preFilt) {
      ATH_MSG_INFO("Stub filtering reduced from " << nHits_preFilt << " to " << nHits_postFilt << " hits/clusters");
    }
    else {
      ATH_MSG_WARNING("Stub filtering has led to duplication - started with " << nHits_preFilt << " and ended up with " << nHits_postFilt << " hits/clusters");
      printHitsFromHeader(header);
    }
  }

  return StatusCode::SUCCESS;
}


void HTTHitFilteringTool::FilterHits(std::vector<HTTHit> &hits,
                                     std::vector<int> &filter_pixel_physLayers,
                                     std::vector<int> &filter_strip_physLayers,
                                     std::vector<HTTHit> &filteredHits)
{

  // there are two types of clusters
  // 1. Those in a part of the detector I am not going to try and filter
  // 2. Those in a part of the detector I am am going to try and filter

  // for stub-type filtering, I want to compare all hits on one side to all hits on the other side
  // don't want to duplicate by doing this twice
  // use pair<HTTHit,ID> to deal with duplicates

  filteredHits.clear();

  if(m_doStubs) {
    // step 1: make a vector of just the hits I am going to try and filter, add the others straight to the passed list. Also give each hit an ID

    std::vector< std::pair<HTTHit,unsigned> > filteredHits_ID, hits_to_filter;
    unsigned counter = 0;
    for (auto &hit : hits) {
      // assign hit ID
      std::pair<HTTHit,unsigned> hit_ID;
      hit_ID.first = hit;
      hit_ID.second = counter;
      counter ++;

      // check pixel layer
      if(!hit.isStrip()) {
        if( std::any_of(std::begin(filter_pixel_physLayers), std::end(filter_pixel_physLayers),
                        [&](unsigned i) { return i == hit.getPhysLayer(); }) )
          hits_to_filter.push_back(hit_ID);
        else
          filteredHits_ID.push_back(hit_ID);
      }
      // check strip layer
      else {
        if( std::any_of(std::begin(filter_strip_physLayers), std::end(filter_strip_physLayers),
                        [&](unsigned i) { return i == hit.getPhysLayer(); }) )
          hits_to_filter.push_back(hit_ID);
        else
          filteredHits_ID.push_back(hit_ID);
      }
    }

    // step 2: split into two lists: inner and outer side modules
    std::vector< std::pair<HTTHit,unsigned> > hits_to_filter_inner, hits_to_filter_outer;
    // ideally use hit.GetSide(), but since this doesn't exist and can't be added
    // (until I add a friend class, coming soon hopefully)
    // have to use the hit physLayer

    for (auto &hit : hits_to_filter) {
      if (hit.first.getPhysLayer() % 2 == 0)
        hits_to_filter_inner.push_back(hit);
      else
        hits_to_filter_outer.push_back(hit);
    }

    // step 3: do the stub filtering
    std::vector< std::pair<HTTHit,unsigned> > passed_inner_hits, passed_outer_hits;
    for (auto innerHit : hits_to_filter_inner) {
      std::vector<std::pair<HTTHit,unsigned>> nearby_outer_hits;
      fill_nearby_hits(innerHit.first, hits_to_filter_outer, nearby_outer_hits);

      // get cut values to apply
      float cut_p, cut_m;
      fill_cut_values(innerHit.first, cut_p, cut_m);

      for (auto outerHit : nearby_outer_hits) {
        bool hitPassed = check_hit_stub(innerHit.first, outerHit.first, cut_p, cut_m);
        if(hitPassed) {
          // will have duplicates. Maybe interesting to count them, so remove later
          passed_inner_hits.push_back(innerHit);
          passed_outer_hits.push_back(outerHit);
        }
      }
    }

    // step 4: remove duplicates among the passed hits
    std::vector< std::pair<HTTHit,unsigned> > passed_outer_hits_unique;
    for (auto &hit : passed_outer_hits) {
      auto it = std::find_if(passed_outer_hits_unique.begin(), passed_outer_hits_unique.end(),
                             [&hit](const std::pair<HTTHit,unsigned>& h) {return h.second == hit.second;} );
      if(it == passed_outer_hits_unique.end()) {
        // std::cout << hit.second << " has not been added yet" << std::endl;
        passed_outer_hits_unique.push_back(hit);
      }
      else {
        // std::cout << hit.second << " has been added already, skipping" << std::endl;
      }
    }
    std::vector< std::pair<HTTHit,unsigned> > passed_inner_hits_unique;
    for (auto &hit : passed_inner_hits) {
      auto it = std::find_if(passed_inner_hits_unique.begin(), passed_inner_hits_unique.end(),
                             [&hit](const std::pair<HTTHit,unsigned>& h) {return h.second == hit.second;} );
      if(it == passed_inner_hits_unique.end()) {
        // std::cout << hit.second << " has not been added yet" << std::endl;
        passed_inner_hits_unique.push_back(hit);
      }
      else {
        // std::cout << hit.second << " has been added already, skipping" << std::endl;
      }
    }


    // step 5: combine them all and sort
    for (auto &hit : passed_inner_hits_unique) {
      filteredHits_ID.push_back(hit);
    }
    for (auto &hit : passed_outer_hits_unique) {
      filteredHits_ID.push_back(hit);
    }
    if(true) {
      // can sort by the hit ID
      std::sort(filteredHits_ID.begin(), filteredHits_ID.end(),
                [](const std::pair<HTTHit,unsigned> & hit1, const std::pair<HTTHit,unsigned> & hit2){
                  return hit1.second < hit2.second;
                });
      for (auto hit_ID : filteredHits_ID) {
        filteredHits.push_back(hit_ID.first);
      }
    }

  }
  else {
    // fill up filteredHits with the input hits (needed for random removal, which isn't done here)
    filteredHits = hits;
  }

}


void HTTHitFilteringTool::fill_cut_values(HTTHit hit, float &cut_m, float &cut_p)
{
  // if we're doing this globally, then no need to use map
  // but probably best to for consistency
  // if(m_stubCutsFile == "") {
    // if(hit.isBarrel())
      // cut_p = m_barrelStubDphiCut;
    // else
      // cut_p = m_endcapStubDphiCut;
    // cut_m = -cut_p;
    // return;
  // }

  // get cut from map - first, find out which zone, layer and module ring we're in
  // can maybe do this from module ID, might be faster
  std::string zone;
  int layer, ring;
  if(hit.isBarrel()) {
    zone = "bar";
    layer = hit.getLayerDisk();
    // barrel modules are spaced at 100mm intervals. This might go wrong at the ends, need to verify
    ring = int(floor(hit.getZ() / 100.));
  }
  else {
    zone = (hit.getZ() < 0) ? "EC-" : "EC+";
    layer = hit.getLayerDisk();

    // use hit radius to determine module ring from endcap geometry in geometry_constants namespace
    int index = -1;
    if (hit.getR() < geometry_constants::strip_endcap_innerRadii.front() ||
        hit.getR() > geometry_constants::strip_endcap_innerRadii.back()) {
      ATH_MSG_ERROR("hit radius " << hit.getR() << "is not within bounds of endcap innerRadii vector");
      ring = hit.getR() < geometry_constants::strip_endcap_innerRadii.front() ? 0 : 5;
    }
    else {
      auto it = upper_bound(geometry_constants::strip_endcap_innerRadii.begin(),
                            geometry_constants::strip_endcap_innerRadii.end(), hit.getR());
      index = it - geometry_constants::strip_endcap_innerRadii.begin() - 1;
      int temp = index;
      // subtract numbers from the start until get below zero
      for(ring=0; ring < int(geometry_constants::strip_endcap_nRows.size()); ring++) {
        temp -= geometry_constants::strip_endcap_nRows[ring];
        if(temp<0)
          break;
      }
    }
  }

  // std::cout << "this hit, r,z = " << hit.getR() << ", " << hit.getZ() << ", is in zone, layer, ring: " << zone << ", " << layer << ", " << ring << std::endl;
  cut_m = m_stubCutMap[zone][layer][ring].first;
  cut_p = m_stubCutMap[zone][layer][ring].second;

}


bool HTTHitFilteringTool::check_hit_stub(HTTHit innerHit, HTTHit outerHit, float cut_m, float cut_p)
{

  float val;

  if(!m_useNstrips) {
    // calculate dphi - accounting for 2 pi wrapping
    float pi = 3.14159265358979;
    float dPhi = (outerHit.getGPhi() - innerHit.getGPhi());
    if(dPhi > pi)
      dPhi -= 2*pi;
    if(dPhi < -pi)
      dPhi += 2*pi;

    // we want it in mrad
    val = dPhi * 1000;
  }

  else {
    val = 0;
  }

  // apply cuts
  return (val > cut_m && val < cut_p);

  // std::cout << "hit details:" << std::endl;
  // std::cout << "  barrel? strip? pixel? " << inHit.isBarrel() << ", " << inHit.isStrip() << ", " << inHit.isPixel() << std::endl;
  // std::cout << "  x, y, z = " << inHit.getX() << ", " << inHit.getY() << ", " << inHit.getZ() << std::endl;
  // std::cout << "  rho, phi = " << inHit.getR() << ", " << inHit.getGPhi() << std::endl;
  // std::cout << "  layer, section = " << inHit.getLayer() << ", " << inHit.getSection() << std::endl;
  // std::cout << "  phiM, etaM = " << inHit.getPhiModule() << ", " << inHit.getHTTEtaModule() << std::endl;

  // position
  // float getZ();
  // float getR();
  // float getGPhi();

  // module info
  // unsigned getLayer();
  // unsigned getSection();
  // unsigned getLayerDisk() const { return m_layer_disk; } // ITk layer number
  // unsigned getPhysLayer() const; // Returns the HTT-defined physical layer index (see HTTPlaneMap.h)
  // unsigned getEtaWidth() const { return m_etaWidth; }
  // unsigned getPhiWidth() const { return m_phiWidth; }
  // unsigned getEtaModule() const { return m_etaModule; }
  // int getHTTEtaModule() const; // TODO UPDATE THIS FOR REL22
  // unsigned getPhiModule() const { return m_phiModule; }

  // eta, phi index
  // unsigned getPhiIndex() const { return m_phiIndex; }
  // unsigned getEtaIndex() const { return m_etaIndex; }
  // float getPhiCoord() const { return m_phiIndex; }
  // float getEtaCoord() const { return m_etaIndex; }

}



void fill_nearby_hits(HTTHit innerHit, std::vector< std::pair<HTTHit,unsigned> > & outerHits, std::vector< std::pair<HTTHit,unsigned> > & nearby_outer_hits)
{
  for (auto outerHit : outerHits) {
    // layer needs to be inner hit physLayer + 1
    if(outerHit.first.getPhysLayer() != innerHit.getPhysLayer() + 1)
      continue;

    // want to do just +-1 in eta/phi as appropriate
    // but need to interpret module IDs first

    // for now, just do by z in barrel and rho in EC
    // strips are at most 50mm long in barrel, 60mm long in EC
    // adjacent one is at most a bit over 60mm away

    if(innerHit.isBarrel()) {
      if(fabs(innerHit.getZ() - outerHit.first.getZ()) > 80)
        continue;
    }
    else {
      if(fabs(innerHit.getR() - outerHit.first.getR()) > 80)
        continue;
    }

    // now distance in x-y plane
    if(fabs(innerHit.getX() - outerHit.first.getX()) > 80)
      continue;
    if( fabs(innerHit.getY() - outerHit.first.getY()) > 80)
      continue;

    nearby_outer_hits.push_back(outerHit);
  }
}


int countHitsFromHeader(HTTLogicalEventInputHeader &header)
{
    int nHits = 0;
    for (int i = 0; i < header.nTowers(); ++i) {
        HTTTowerInputHeader &tower = *header.getTower(i);
        std::vector<HTTHit>  hits = tower.hits();
        nHits += hits.size();
    }
    return nHits;
}


void printHitsFromHeader(HTTLogicalEventInputHeader &header)
{
    for (int i = 0; i < header.nTowers(); ++i) {
        HTTTowerInputHeader &tower = *header.getTower(i);
        std::vector<HTTHit>  hits = tower.hits();
        std::cout << "tower " << i << " with " << hits.size() << " hits:" << std::endl;
        for(auto hit : hits) {
          std::cout << "  isStrip, side, x, y, z, rho, phi, physLayer, layer, section, phiM, etaM = " << hit.isStrip() << ", " << hit.getPhysLayer()%2 << ", " << hit.getX() << ", " << hit.getY() << ", " << hit.getZ() << ", " << hit.getR() << ", " << hit.getGPhi() << ", " << hit.getPhysLayer() << ", " << hit.getLayer() << ", " << hit.getSection() << ", " << hit.getPhiModule() << ", " << hit.getHTTEtaModule() << std::endl;
        }
    }
}
