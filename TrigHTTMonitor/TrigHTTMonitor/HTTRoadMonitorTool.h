/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTTROADMONITORTOOL_H
#define HTTROADMONITORTOOL_H

#include "TrigHTTMonitor/HTTMonitorBase.h"

#include "TrigHTTObjects/HTTRoad.h"


/////////////////////////////////////////////////////////////////////////////
class HTTRoadMonitorTool: public HTTMonitorBase
{
public:

  HTTRoadMonitorTool (std::string const & algname, std::string const & name, IInterface const * ifc);
  ~HTTRoadMonitorTool() {}

  static const InterfaceID& interfaceID();

  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms() override;
  virtual StatusCode finalize() override;

protected:

  virtual StatusCode bookHistograms() override;

private:

  // Book histograms
  StatusCode bookHTTRoadHistograms_1st();
  StatusCode bookHTTRoadHistograms_2nd();

  // Fill histograms
  void fillHTTRoadHistograms_1st(std::vector<HTTRoad*> const * roads_1st);
  void fillHTTRoadHistograms_2nd(std::vector<HTTRoad*> const * roads_2nd);

  // usefull tool
  void calculateTruth(std::vector<HTTRoad*> const * roads, std::map<typename HTTMultiTruth::Barcode, std::pair<std::vector<HTTRoad const *>, float>> & m);
  void fillBitmask(TH1I* h, uint32_t m, size_t n) { for (size_t l = 0; l < n; l++) if (m & (1 << l)) h->Fill(l); }

  // Flags
  BooleanProperty m_fast = false;
  BooleanProperty m_runSecondStage = false;

  //====================
  // Histograms
  //====================

  // First Stage Road Histograms
  TH1I*   h_nRoads_1st;
  TH1I*   h_nRoads_1st_uHits;
  TH1I*   h_nRoads_1st_matched;
  TH1I*   h_nRoads_1st_dupeBarcode;
  TH1I*   h_nRoads_1st_fakeBarcode;
  TH1I*   h_nRoadHitCombos_1st;
  TH2F*   h_nRoads_1st_vs_truth_pt;
  TH2F*   h_nRoads_1st_vs_truth_eta;
  TH2F*   h_nRoads_1st_vs_truth_phi;
  TH2F*   h_nRoads_1st_vs_truth_d0;
  TH2F*   h_nRoads_1st_vs_truth_z0;
  TH1I*   h_road_1st_highestBarcodeFrac;

  TH1I*   h_nHits_road_1st;
  TH1I*   h_nLayers_road_1st;
  TH1I*   h_hitLayers_road_1st;
  TH1I*   h_wcLayers_road_1st;
  TH1I*   h_matchedLayers_road_1st;
  TH1I*   h_Categories_road_1st;

  TH1I*   h_nLayers_best_road_1st;
  TH1I*   h_hitLayers_best_road_1st;
  TH1I*   h_wcLayers_best_road_1st;
  TH1I*   h_matchedLayers_best_road_1st;
  TH1I*   h_Categories_best_road_1st;

  TH1I*   h_pattMult_1st;
  TH1I*   h_sectMult_1st;

  // Second Stage Road Histograms
  TH1I*   h_nRoads_2nd;
  TH1I*   h_nRoads_2nd_uHits;
  TH1I*   h_nRoads_2nd_matched;
  TH1I*   h_nRoads_2nd_dupeBarcode;
  TH1I*   h_nRoads_2nd_fakeBarcode;
  TH1I*   h_nRoadHitCombos_2nd;
  TH2F*   h_nRoads_2nd_vs_truth_pt;
  TH2F*   h_nRoads_2nd_vs_truth_eta;
  TH2F*   h_nRoads_2nd_vs_truth_phi;
  TH2F*   h_nRoads_2nd_vs_truth_d0;
  TH2F*   h_nRoads_2nd_vs_truth_z0;
  TH1I*   h_road_2nd_highestBarcodeFrac;

  TH1I*   h_nHits_road_2nd;
  TH1I*   h_nLayers_road_2nd;
  TH1I*   h_hitLayers_road_2nd;
  TH1I*   h_wcLayers_road_2nd;
  TH1I*   h_matchedLayers_road_2nd;
  TH1I*   h_Categories_road_2nd;

  TH1I*   h_nLayers_best_road_2nd;
  TH1I*   h_hitLayers_best_road_2nd;
  TH1I*   h_wcLayers_best_road_2nd;
  TH1I*   h_matchedLayers_best_road_2nd;
  TH1I*   h_Categories_best_road_2nd;

  TH1I*   h_pattMult_2nd;
  TH1I*   h_sectMult_2nd;
};

#endif // HTTRoadMonitorTool_h
