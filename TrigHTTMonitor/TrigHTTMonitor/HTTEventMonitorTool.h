/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTTEVENTMONITORTOOL_H
#define HTTEVENTMONITORTOOL_H

#include "TrigHTTMonitor/HTTMonitorBase.h"

#include "TrigHTTObjects/HTTRoad.h"


/////////////////////////////////////////////////////////////////////////////
class HTTEventMonitorTool: public HTTMonitorBase
{
public:

  HTTEventMonitorTool (std::string const & algname, std::string const & name, IInterface const * ifc);
  ~HTTEventMonitorTool() {}

  static const InterfaceID& interfaceID();

  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms() override;
  virtual StatusCode printHistograms(PrintDetail detail = SUMMARY) const override;
  virtual StatusCode finalize() override;

protected:

  virtual StatusCode retrieveHistograms() override;

private:

  void calculateStatistics();

  // usefull tool
  void calculateTruth(std::vector<HTTRoad*> const * roads, std::map<typename HTTMultiTruth::Barcode, std::pair<std::vector<HTTRoad const *>, float>> & m);
  void printHist(TH1* h, bool profile = false) const;

  // Flags
  BooleanProperty m_fast = false;
  BooleanProperty m_outputMonitor = false;
  BooleanProperty m_clustering = false;
  BooleanProperty m_doSpacepoints = false;
  BooleanProperty m_runSecondStage = false;

  // Counters. Histograms sometimes overflow, so use int counters for 100% accuracy
  size_t m_nRoads_1st = 0;
  size_t m_nRoads_2nd = 0;
  size_t m_nCombos_1st = 0; // # of hit combinations in first stage roads
  size_t m_nCombos_2nd = 0; // # of hit combinations in second stage roads
  size_t m_nEvents_with_road_1st = 0;
  size_t m_nEvents_with_road_2nd = 0;
  size_t m_nTruthTracks = 0; // # truth tracks PASSING CUTS
  size_t m_nTruthTracks_with_road_1st = 0;
  size_t m_nTruthTracks_with_road_2nd = 0;

  // Histograms used for event monitoring
  TH1* h_Truth_Track_Road_1st_matched_pt;
  TH1* h_Truth_Track_Road_2nd_matched_pt;

  TH1* h_Truth_Track_Full_pt;
  TH1* h_Truth_Track_Offline_matched_pt;

  TH1* h_nTracks_1st_afterOR;
  TH1* h_Truth_Track_HTT_1st_matched_pt;
  TH1* h_Truth_Track_HTT_1st_passChi2_matched_pt;
  TH1* h_Truth_Track_HTT_1st_afterOR_matched_pt;

  TH1* h_nTracks_2nd_afterOR;
  TH1* h_Truth_Track_HTT_2nd_matched_pt;
  TH1* h_Truth_Track_HTT_2nd_passChi2_matched_pt;
  TH1* h_Truth_Track_HTT_2nd_afterOR_matched_pt;

  TH1* h_nHits_1st;
  TH1* h_nLayers_hit_1st;

  TH1* h_nHits_2nd;
  TH1* h_nLayers_hit_2nd;

  TH1* h_nRoads_1st;
  TH1* h_nRoads_1st_uHits;
  TH1* h_nRoads_1st_matched;
  TH1* h_nRoads_1st_dupeBarcode;
  TH1* h_nRoads_1st_fakeBarcode;
  TH1* h_nRoadHitCombos_1st;
  TH1* h_nHits_road_1st;
  TH1* h_nLayers_road_1st;
  TH1* h_hitLayers_road_1st;
  TH1* h_wcLayers_road_1st;
  TH1* h_matchedLayers_road_1st;

  TH1* h_nRoads_2nd;
  TH1* h_nRoads_2nd_uHits;
  TH1* h_nRoads_2nd_matched;
  TH1* h_nRoads_2nd_dupeBarcode;
  TH1* h_nRoads_2nd_fakeBarcode;
  TH1* h_nRoadHitCombos_2nd;
  TH1* h_nHits_road_2nd;
  TH1* h_nLayers_road_2nd;
  TH1* h_hitLayers_road_2nd;
  TH1* h_wcLayers_road_2nd;
  TH1* h_matchedLayers_road_2nd;

  TH1* h_nTracks_1st;
  TH1* h_nTracks_1st_passChi2;
  TH1* h_nTracks_1st_per_patt;
  TH1* h_nTracks_1st_per_patt_passChi2;
  TH1* h_nTracks_1st_per_patt_afterOR;
  TH1* h_nConstants_1st;
  TH1* h_nConstants_1st_passChi2;
  TH1* h_nConstants_1st_afterOR;
  TH1* h_Track_1st_pt;
  TH1* h_Track_1st_eta;
  TH1* h_Track_1st_phi;
  TH1* h_Track_1st_d0;
  TH1* h_Track_1st_z0;
  TH1* h_Track_1st_chi2;
  TH1* h_Track_1st_chi2ndof;
  TH1* h_Track_1st_nmissing;

  TH1* h_nTracks_2nd;
  TH1* h_nTracks_2nd_passChi2;
  TH1* h_nTracks_2nd_per_patt;
  TH1* h_nTracks_2nd_per_patt_passChi2;
  TH1* h_nTracks_2nd_per_patt_afterOR;
  TH1* h_nConstants_2nd;
  TH1* h_nConstants_2nd_passChi2;
  TH1* h_nConstants_2nd_afterOR;
  TH1* h_Track_2nd_pt;
  TH1* h_Track_2nd_eta;
  TH1* h_Track_2nd_phi;
  TH1* h_Track_2nd_d0;
  TH1* h_Track_2nd_z0;
  TH1* h_Track_2nd_chi2;
  TH1* h_Track_2nd_chi2ndof;
  TH1* h_Track_2nd_nmissing;

  TH1* h_nTruth_Tracks_Full;
  TH1* h_Truth_Track_Full_eta;
  TH1* h_Truth_Track_Full_phi;
  TH1* h_Truth_Track_Full_d0;
  TH1* h_Truth_Track_Full_z0;

  TH1* h_nClustersOff;

  TH1* h_nClusters_1st;
  TH1* h_Cluster_1st_eta;
  TH1* h_Cluster_1st_phi;
  TH1* h_Cluster_1st_etaWidth;
  TH1* h_Cluster_1st_phiWidth;
  TH1* h_Cluster_1st_barrelEC;
  TH1* h_Cluster_1st_layer;
  TH1* h_Cluster_1st_nSubregions;
  TH1* h_nClusters_1st_subregion;
  TH1* h_nClusters_1st_layer;
  TH1* h_nClusters_1st_PerSubregion_layer;

  TH1* h_nClusters_2nd;
  TH1* h_Cluster_2nd_eta;
  TH1* h_Cluster_2nd_phi;
  TH1* h_Cluster_2nd_etaWidth;
  TH1* h_Cluster_2nd_phiWidth;
  TH1* h_Cluster_2nd_barrelEC;
  TH1* h_Cluster_2nd_layer;
  TH1* h_Cluster_2nd_nSubregions;
  TH1* h_nClusters_2nd_subregion;
  TH1* h_nClusters_2nd_layer;
  TH1* h_nClusters_2nd_PerSubregion_layer;

  TH1* h_nSpacepoints_1st;
  TH1* h_nSpacepoints_2nd;
};

#endif // HTTEventMonitorTool_h
