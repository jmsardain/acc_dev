/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTTHITMONITORTOOL_H
#define HTTHITMONITORTOOL_H

#include "TrigHTTMonitor/HTTMonitorBase.h"

/////////////////////////////////////////////////////////////////////////////
class HTTHitMonitorTool: public HTTMonitorBase
{
public:

  HTTHitMonitorTool (std::string const & algname, std::string const & name, IInterface const * ifc);
  ~HTTHitMonitorTool() {}

  static const InterfaceID& interfaceID();

  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms() override;
  virtual StatusCode finalize() override;

protected:

  virtual StatusCode bookHistograms() override;

private:

  // Book histograms
  StatusCode bookHTTHitHistograms_1st();
  StatusCode bookHTTHitHistograms_2nd();
  StatusCode bookMissedHitHistograms_1st();
  StatusCode bookMissedHitHistograms_2nd();

  // Fill histograms
  void fillHTTHitHistograms_1st(std::vector<HTTHit> const * hits_1st_mapped);
  void fillHTTHitHistograms_2nd(std::vector<HTTHit> const * hits_2nd_mapped);
  void fillMissedHitHistograms_1st(std::vector<HTTHit> const * hits_1st_miss, std::vector<HTTHit> const * hits_1st_mapped);
  void fillMissedHitHistograms_2nd(std::vector<HTTHit> const * hits_2nd_miss, std::vector<HTTHit> const * hits_2nd_mapped);

  // usefull tool
  int getUniqueLayerId(HTTHit const & hit);

  // Flags
  BooleanProperty m_fast = false;
  BooleanProperty m_outputMonitor = false;
  BooleanProperty m_runSecondStage = false;

  //====================
  // Histograms
  //====================

  // First Stage Hit Histograms
  TH1I*   h_nHits_1st;
  TH1I*   h_nLayers_hit_1st;
  TH1I*   h_hit_1st_Layer;

  TH2I*   h_nHits_1st_layer;                    // x: layer,     y: nhits

  std::vector<TH2I*>  h_Hit_1st_coord_layer;

  // Second Stage Hit Histograms
  TH1I*   h_nHits_2nd;
  TH1I*   h_nLayers_hit_2nd;
  TH1I*   h_hit_2nd_Layer;

  TH2I*   h_nHits_2nd_layer;                    // x: layer,     y: nhits

  std::vector<TH2I*>  h_Hit_2nd_coord_layer;

  // Histograms for First Stage Missed Hits
  TH1I*   h_nMissedHits_1st;
  TH1F*   h_nMissedHits_1st_Frac;
  TH2F*   h_nMissedHits_1st_vs_truth_pt;
  TH2F*   h_nMissedHits_1st_vs_truth_eta;
  TH2F*   h_nMissedHits_1st_vs_truth_phi;
  TH2F*   h_nMissedHits_1st_vs_truth_d0;
  TH2F*   h_nMissedHits_1st_vs_truth_z0;

  TH1I*   h_MissedHit_1st_eta;
  TH1I*   h_MissedHit_1st_phi;
  TH1I*   h_MissedHit_1st_type;
  TH1I*   h_MissedHit_1st_barrelEC;
  TH1I*   h_MissedHit_1st_layer;
  TH1I*   h_MissedHit_1st_physLayer;

  // Histograms for Second Stage Missed Hits
  TH1I*   h_nMissedHits_2nd;
  TH1F*   h_nMissedHits_2nd_Frac;
  TH2F*   h_nMissedHits_2nd_vs_truth_pt;
  TH2F*   h_nMissedHits_2nd_vs_truth_eta;
  TH2F*   h_nMissedHits_2nd_vs_truth_phi;
  TH2F*   h_nMissedHits_2nd_vs_truth_d0;
  TH2F*   h_nMissedHits_2nd_vs_truth_z0;

  TH1I*   h_MissedHit_2nd_eta;
  TH1I*   h_MissedHit_2nd_phi;
  TH1I*   h_MissedHit_2nd_type;
  TH1I*   h_MissedHit_2nd_barrelEC;
  TH1I*   h_MissedHit_2nd_layer;
  TH1I*   h_MissedHit_2nd_physLayer;
};

#endif // HTTHitMonitorTool_h
