/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTTPERFORMANCEMONITORTOOL_H
#define HTTPERFORMANCEMONITORTOOL_H

#include "TrigHTTMonitor/HTTMonitorBase.h"

#include "TrigHTTObjects/HTTRoad.h"


/////////////////////////////////////////////////////////////////////////////
class HTTPerformanceMonitorTool: public HTTMonitorBase
{
public:

  HTTPerformanceMonitorTool (std::string const & algname, std::string const & name, IInterface const * ifc);
  ~HTTPerformanceMonitorTool() {}

  static const InterfaceID& interfaceID();

  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms() override;
  virtual StatusCode finalize() override;

protected:

  virtual StatusCode bookHistograms() override;
  virtual StatusCode makeEffHistograms() override;

private:

  // Book histograms
  StatusCode bookCrudeEffHistograms_1st();
  StatusCode bookCrudeEffHistograms_2nd();
  StatusCode bookTruthMatchHistograms_1st();
  StatusCode bookTruthMatchHistograms_2nd();

  // Fill histograms
  void fillCrudeEffHistograms_1st(std::vector<HTTRoad*> const * roads_1st, std::vector<HTTTrack> const * tracks_1st);
  void fillCrudeEffHistograms_2nd(std::vector<HTTRoad*> const * roads_2nd, std::vector<HTTTrack> const * tracks_2nd);
  void fillHTTRoadTruthMatchHistograms_1st(std::vector<HTTRoad*> const * roads_1st);
  void fillHTTRoadTruthMatchHistograms_2nd(std::vector<HTTRoad*> const * roads_2nd);
  void fillHTTTrackTruthMatchHistograms_1st(std::vector<HTTTrack> const * tracks_1st);
  void fillHTTTrackTruthMatchHistograms_2nd(std::vector<HTTTrack> const * tracks_2nd);

  // Calculate efficiencies
  void makeCrudeEffHistograms_1st();
  void makeCrudeEffHistograms_2nd();
  void makeTruthMatchEffHistograms_1st();
  void makeTruthMatchEffHistograms_2nd();

  // usefull tool
  void calculateTruth(std::vector<HTTRoad*> const * roads, std::map<typename HTTMultiTruth::Barcode, std::pair<std::vector<HTTRoad const *>, float>> & m);

  BooleanProperty m_fast = false;
  BooleanProperty m_outputMonitor = false;
  BooleanProperty m_clustering = false;
  BooleanProperty m_doSpacepoints = false;
  BooleanProperty m_runSecondStage = false;

  unsigned m_nTruth_Tracks_Full = 0;
  unsigned m_nOffline_Tracks = 0;
  HTTTrack const * m_bestHTTTrack_1st = nullptr;
  unsigned m_event = 0;

  // Histograms to use for doing crude efficiencies
  //TGraphAsymmErrors*   h_RoadEfficiency_1st_pt;
  //TGraphAsymmErrors*   h_RoadEfficiency_1st_eta;
  //TGraphAsymmErrors*   h_RoadEfficiency_1st_phi;
  //TGraphAsymmErrors*   h_RoadEfficiency_1st_d0;
  //TGraphAsymmErrors*   h_RoadEfficiency_1st_z0;

  //TGraphAsymmErrors*   h_TrackEfficiency_1st_pt;
  //TGraphAsymmErrors*   h_TrackEfficiency_1st_eta;
  //TGraphAsymmErrors*   h_TrackEfficiency_1st_phi;
  //TGraphAsymmErrors*   h_TrackEfficiency_1st_d0;
  //TGraphAsymmErrors*   h_TrackEfficiency_1st_z0;

  //TGraphAsymmErrors*   h_FullEfficiency_1st_pt;
  //TGraphAsymmErrors*   h_FullEfficiency_1st_eta;
  //TGraphAsymmErrors*   h_FullEfficiency_1st_phi;
  //TGraphAsymmErrors*   h_FullEfficiency_1st_d0;
  //TGraphAsymmErrors*   h_FullEfficiency_1st_z0;

  TH1F*   h_RoadEfficiency_1st_pt;
  TH1F*   h_RoadEfficiency_1st_eta;
  TH1F*   h_RoadEfficiency_1st_phi;
  TH1F*   h_RoadEfficiency_1st_d0;
  TH1F*   h_RoadEfficiency_1st_z0;

  TH1F*   h_TrackEfficiency_1st_pt;
  TH1F*   h_TrackEfficiency_1st_eta;
  TH1F*   h_TrackEfficiency_1st_phi;
  TH1F*   h_TrackEfficiency_1st_d0;
  TH1F*   h_TrackEfficiency_1st_z0;

  TH1F*   h_FullEfficiency_1st_pt;
  TH1F*   h_FullEfficiency_1st_eta;
  TH1F*   h_FullEfficiency_1st_phi;
  TH1F*   h_FullEfficiency_1st_d0;
  TH1F*   h_FullEfficiency_1st_z0;

  TGraphAsymmErrors*   h_RoadEfficiency_2nd_pt;
  TGraphAsymmErrors*   h_RoadEfficiency_2nd_eta;
  TGraphAsymmErrors*   h_RoadEfficiency_2nd_phi;
  TGraphAsymmErrors*   h_RoadEfficiency_2nd_d0;
  TGraphAsymmErrors*   h_RoadEfficiency_2nd_z0;

  TGraphAsymmErrors*   h_TrackEfficiency_2nd_pt;
  TGraphAsymmErrors*   h_TrackEfficiency_2nd_eta;
  TGraphAsymmErrors*   h_TrackEfficiency_2nd_phi;
  TGraphAsymmErrors*   h_TrackEfficiency_2nd_d0;
  TGraphAsymmErrors*   h_TrackEfficiency_2nd_z0;

  TGraphAsymmErrors*   h_FullEfficiency_2nd_pt;
  TGraphAsymmErrors*   h_FullEfficiency_2nd_eta;
  TGraphAsymmErrors*   h_FullEfficiency_2nd_phi;
  TGraphAsymmErrors*   h_FullEfficiency_2nd_d0;
  TGraphAsymmErrors*   h_FullEfficiency_2nd_z0;

  TH1F*   h_Truth_Track_Leading_pt;
  TH1F*   h_Truth_Track_Leading_eta;
  TH1F*   h_Truth_Track_Leading_phi;
  TH1F*   h_Truth_Track_Leading_d0;
  TH1F*   h_Truth_Track_Leading_z0;

  TH1F*   h_Truth_Track_withroad_1st_pt;
  TH1F*   h_Truth_Track_withroad_1st_eta;
  TH1F*   h_Truth_Track_withroad_1st_phi;
  TH1F*   h_Truth_Track_withroad_1st_d0;
  TH1F*   h_Truth_Track_withroad_1st_z0;

  TH1F*   h_Truth_Track_withtrack_1st_pt;
  TH1F*   h_Truth_Track_withtrack_1st_eta;
  TH1F*   h_Truth_Track_withtrack_1st_phi;
  TH1F*   h_Truth_Track_withtrack_1st_d0;
  TH1F*   h_Truth_Track_withtrack_1st_z0;

  TH1F*   h_Truth_Track_withroad_2nd_pt;
  TH1F*   h_Truth_Track_withroad_2nd_eta;
  TH1F*   h_Truth_Track_withroad_2nd_phi;
  TH1F*   h_Truth_Track_withroad_2nd_d0;
  TH1F*   h_Truth_Track_withroad_2nd_z0;

  TH1F*   h_Truth_Track_withtrack_2nd_pt;
  TH1F*   h_Truth_Track_withtrack_2nd_eta;
  TH1F*   h_Truth_Track_withtrack_2nd_phi;
  TH1F*   h_Truth_Track_withtrack_2nd_d0;
  TH1F*   h_Truth_Track_withtrack_2nd_z0;

  // Histograms for doing truth matched efficiencies, resolutions
  TH1I*   h_nTruth_Tracks_Full;
  TH1F*   h_Truth_Track_Full_pt;
  TH1F*   h_Truth_Track_Full_eta;
  TH1F*   h_Truth_Track_Full_phi;
  TH1F*   h_Truth_Track_Full_d0;
  TH1F*   h_Truth_Track_Full_z0;

  TH1F*   h_nOffline_Tracks;
  TH1F*   h_nOffline_Tracks_before_match;
  TH1F*   h_nOffline_Tracks_before_match_0GeV;
  TH1F*   h_nOffline_Tracks_before_match_1GeV;
  TH1F*   h_nOffline_Tracks_before_match_2GeV;
  TH1F*   h_Offline_Track_pt;
  TH1F*   h_Offline_Track_eta;
  TH1F*   h_Offline_Track_phi;
  TH1F*   h_Offline_Track_d0;
  TH1F*   h_Offline_Track_z0;

  TH1F*   h_Offline_vs_Truth_Res_no_truthmatch_qoverpt;
  TH1F*   h_Offline_vs_Truth_Res_no_truthmatch_pt;
  TH1F*   h_Offline_vs_Truth_Res_no_truthmatch_eta;
  TH1F*   h_Offline_vs_Truth_Res_no_truthmatch_phi;
  TH1F*   h_Offline_vs_Truth_Res_no_truthmatch_d0;
  TH1F*   h_Offline_vs_Truth_Res_no_truthmatch_z0;
  TH1F*   h_Offline_vs_Truth_Res_no_truthmatch_z0_wide;

  // Offline vs Truth
  TGraphAsymmErrors*   h_Offline_vs_Truth_eff_pt;
  TGraphAsymmErrors*   h_Offline_vs_Truth_eff_eta;
  TGraphAsymmErrors*   h_Offline_vs_Truth_eff_phi;
  TGraphAsymmErrors*   h_Offline_vs_Truth_eff_d0;
  TGraphAsymmErrors*   h_Offline_vs_Truth_eff_z0;

  TH1F*   h_Truth_Track_Offline_matched_pt;
  TH1F*   h_Truth_Track_Offline_matched_eta;
  TH1F*   h_Truth_Track_Offline_matched_phi;
  TH1F*   h_Truth_Track_Offline_matched_d0;
  TH1F*   h_Truth_Track_Offline_matched_z0;

  TH1F*   h_Offline_vs_Truth_Res_truthmatch_pt;
  TH1F*   h_Offline_vs_Truth_Res_truthmatch_eta;
  TH1F*   h_Offline_vs_Truth_Res_truthmatch_phi;
  TH1F*   h_Offline_vs_Truth_Res_truthmatch_d0;
  TH1F*   h_Offline_vs_Truth_Res_truthmatch_z0;
  TH1F*   h_Offline_vs_Truth_Res_truthmatch_qoverpt;

  TH2F*   h_Offline_vs_Truth_Res_truthmatch_pt_vspt;
  TH2F*   h_Offline_vs_Truth_Res_truthmatch_eta_vspt;
  TH2F*   h_Offline_vs_Truth_Res_truthmatch_phi_vspt;
  TH2F*   h_Offline_vs_Truth_Res_truthmatch_d0_vspt;
  TH2F*   h_Offline_vs_Truth_Res_truthmatch_z0_vspt;
  TH2F*   h_Offline_vs_Truth_Res_truthmatch_qoverpt_vspt;

  TH2F*   h_Offline_vs_Truth_Res_truthmatch_pt_vseta;
  TH2F*   h_Offline_vs_Truth_Res_truthmatch_eta_vseta;
  TH2F*   h_Offline_vs_Truth_Res_truthmatch_phi_vseta;
  TH2F*   h_Offline_vs_Truth_Res_truthmatch_d0_vseta;
  TH2F*   h_Offline_vs_Truth_Res_truthmatch_z0_vseta;
  TH2F*   h_Offline_vs_Truth_Res_truthmatch_qoverpt_vseta;

  TH1F*   h_ratio_Offline_Truth;

  // HTT vs Truth Road Efficiency for first stage
  //TGraphAsymmErrors*   h_HTT_1st_vs_Truth_Road_eff_pt;
  //TGraphAsymmErrors*   h_HTT_1st_vs_Truth_Road_eff_eta;
  //TGraphAsymmErrors*   h_HTT_1st_vs_Truth_Road_eff_phi;
  //TGraphAsymmErrors*   h_HTT_1st_vs_Truth_Road_eff_d0;
  //TGraphAsymmErrors*   h_HTT_1st_vs_Truth_Road_eff_z0;

  TH1F*   h_HTT_1st_vs_Truth_Road_eff_pt;
  TH1F*   h_HTT_1st_vs_Truth_Road_eff_eta;
  TH1F*   h_HTT_1st_vs_Truth_Road_eff_phi;
  TH1F*   h_HTT_1st_vs_Truth_Road_eff_d0;
  TH1F*   h_HTT_1st_vs_Truth_Road_eff_z0;

  TH1F*   h_Truth_Track_Road_1st_matched_pt;
  TH1F*   h_Truth_Track_Road_1st_matched_eta;
  TH1F*   h_Truth_Track_Road_1st_matched_phi;
  TH1F*   h_Truth_Track_Road_1st_matched_d0;
  TH1F*   h_Truth_Track_Road_1st_matched_z0;

  // HTT vs Offline Road Efficiency for first stage
  TGraphAsymmErrors*   h_HTT_1st_vs_Offline_Road_eff_pt;
  TGraphAsymmErrors*   h_HTT_1st_vs_Offline_Road_eff_eta;
  TGraphAsymmErrors*   h_HTT_1st_vs_Offline_Road_eff_phi;
  TGraphAsymmErrors*   h_HTT_1st_vs_Offline_Road_eff_d0;
  TGraphAsymmErrors*   h_HTT_1st_vs_Offline_Road_eff_z0;

  // HTT vs Truth (All) for first stage
  TGraphAsymmErrors*   h_HTT_1st_vs_Truth_eff_pt;
  TGraphAsymmErrors*   h_HTT_1st_vs_Truth_eff_eta;
  TGraphAsymmErrors*   h_HTT_1st_vs_Truth_eff_phi;
  TGraphAsymmErrors*   h_HTT_1st_vs_Truth_eff_d0;
  TGraphAsymmErrors*   h_HTT_1st_vs_Truth_eff_z0;

  TH1F*   h_Truth_Track_HTT_1st_matched_pt;
  TH1F*   h_Truth_Track_HTT_1st_matched_eta;
  TH1F*   h_Truth_Track_HTT_1st_matched_phi;
  TH1F*   h_Truth_Track_HTT_1st_matched_d0;
  TH1F*   h_Truth_Track_HTT_1st_matched_z0;

  TH1F*   h_HTT_1st_vs_Truth_Res_truthmatch_pt;
  TH1F*   h_HTT_1st_vs_Truth_Res_truthmatch_eta;
  TH1F*   h_HTT_1st_vs_Truth_Res_truthmatch_phi;
  TH1F*   h_HTT_1st_vs_Truth_Res_truthmatch_d0;
  TH1F*   h_HTT_1st_vs_Truth_Res_truthmatch_z0;
  TH1F*   h_HTT_1st_vs_Truth_Res_truthmatch_qoverpt;

  TH2F*   h_HTT_1st_vs_Truth_Res_truthmatch_pt_vspt;
  TH2F*   h_HTT_1st_vs_Truth_Res_truthmatch_eta_vspt;
  TH2F*   h_HTT_1st_vs_Truth_Res_truthmatch_phi_vspt;
  TH2F*   h_HTT_1st_vs_Truth_Res_truthmatch_d0_vspt;
  TH2F*   h_HTT_1st_vs_Truth_Res_truthmatch_z0_vspt;
  TH2F*   h_HTT_1st_vs_Truth_Res_truthmatch_qoverpt_vspt;

  TH2F*   h_HTT_1st_vs_Truth_Res_truthmatch_pt_vseta;
  TH2F*   h_HTT_1st_vs_Truth_Res_truthmatch_eta_vseta;
  TH2F*   h_HTT_1st_vs_Truth_Res_truthmatch_phi_vseta;
  TH2F*   h_HTT_1st_vs_Truth_Res_truthmatch_d0_vseta;
  TH2F*   h_HTT_1st_vs_Truth_Res_truthmatch_z0_vseta;
  TH2F*   h_HTT_1st_vs_Truth_Res_truthmatch_qoverpt_vseta;

  // HTT vs Truth (passing Chi^2 cut) for first stage
  TGraphAsymmErrors*   h_HTT_1st_vs_Truth_passChi2_eff_pt;
  TGraphAsymmErrors*   h_HTT_1st_vs_Truth_passChi2_eff_eta;
  TGraphAsymmErrors*   h_HTT_1st_vs_Truth_passChi2_eff_phi;
  TGraphAsymmErrors*   h_HTT_1st_vs_Truth_passChi2_eff_d0;
  TGraphAsymmErrors*   h_HTT_1st_vs_Truth_passChi2_eff_z0;

  TH1F*   h_Truth_Track_HTT_1st_passChi2_matched_pt;
  TH1F*   h_Truth_Track_HTT_1st_passChi2_matched_eta;
  TH1F*   h_Truth_Track_HTT_1st_passChi2_matched_phi;
  TH1F*   h_Truth_Track_HTT_1st_passChi2_matched_d0;
  TH1F*   h_Truth_Track_HTT_1st_passChi2_matched_z0;

  TH1F*   h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_pt;
  TH1F*   h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_eta;
  TH1F*   h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_phi;
  TH1F*   h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_d0;
  TH1F*   h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_z0;
  TH1F*   h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_qoverpt;

  TH2F*   h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_pt_vspt;
  TH2F*   h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_eta_vspt;
  TH2F*   h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_phi_vspt;
  TH2F*   h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_d0_vspt;
  TH2F*   h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_z0_vspt;
  TH2F*   h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_qoverpt_vspt;

  TH2F*   h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_pt_vseta;
  TH2F*   h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_eta_vseta;
  TH2F*   h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_phi_vseta;
  TH2F*   h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_d0_vseta;
  TH2F*   h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_z0_vseta;
  TH2F*   h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_qoverpt_vseta;

  // HTT vs Truth (after overlap removal) for first stage
  TGraphAsymmErrors*   h_HTT_1st_vs_Truth_afterOR_eff_pt;
  TGraphAsymmErrors*   h_HTT_1st_vs_Truth_afterOR_eff_eta;
  TGraphAsymmErrors*   h_HTT_1st_vs_Truth_afterOR_eff_phi;
  TGraphAsymmErrors*   h_HTT_1st_vs_Truth_afterOR_eff_d0;
  TGraphAsymmErrors*   h_HTT_1st_vs_Truth_afterOR_eff_z0;

  TH1I*   h_Truth_Track_HTT_1st_afterOR_Nmatched;
  TH1F*   h_Truth_Track_HTT_1st_afterOR_matched_pt;
  TH1F*   h_Truth_Track_HTT_1st_afterOR_matched_eta;
  TH1F*   h_Truth_Track_HTT_1st_afterOR_matched_phi;
  TH1F*   h_Truth_Track_HTT_1st_afterOR_matched_d0;
  TH1F*   h_Truth_Track_HTT_1st_afterOR_matched_z0;

  TH1F*   h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_pt;
  TH1F*   h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_eta;
  TH1F*   h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_phi;
  TH1F*   h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_d0;
  TH1F*   h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_z0;
  TH1F*   h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_qoverpt;

  TH2F*   h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_pt_vspt;
  TH2F*   h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_eta_vspt;
  TH2F*   h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_phi_vspt;
  TH2F*   h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_d0_vspt;
  TH2F*   h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_z0_vspt;
  TH2F*   h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_qoverpt_vspt;

  TH2F*   h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_pt_vseta;
  TH2F*   h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_eta_vseta;
  TH2F*   h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_phi_vseta;
  TH2F*   h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_d0_vseta;
  TH2F*   h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_z0_vseta;
  TH2F*   h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_qoverpt_vseta;

  // HTT vs Offline (All) for first stage
  TGraphAsymmErrors*   h_HTT_1st_vs_Offline_eff_pt;
  TGraphAsymmErrors*   h_HTT_1st_vs_Offline_eff_eta;
  TGraphAsymmErrors*   h_HTT_1st_vs_Offline_eff_phi;
  TGraphAsymmErrors*   h_HTT_1st_vs_Offline_eff_d0;
  TGraphAsymmErrors*   h_HTT_1st_vs_Offline_eff_z0;

  // HTT vs Offline (passing Chi^2 cut) for first stage
  TGraphAsymmErrors*   h_HTT_1st_vs_Offline_passChi2_eff_pt;
  TGraphAsymmErrors*   h_HTT_1st_vs_Offline_passChi2_eff_eta;
  TGraphAsymmErrors*   h_HTT_1st_vs_Offline_passChi2_eff_phi;
  TGraphAsymmErrors*   h_HTT_1st_vs_Offline_passChi2_eff_d0;
  TGraphAsymmErrors*   h_HTT_1st_vs_Offline_passChi2_eff_z0;

  // HTT vs Offline (after overlap removal) for first stage
  TGraphAsymmErrors*   h_HTT_1st_vs_Offline_afterOR_eff_pt;
  TGraphAsymmErrors*   h_HTT_1st_vs_Offline_afterOR_eff_eta;
  TGraphAsymmErrors*   h_HTT_1st_vs_Offline_afterOR_eff_phi;
  TGraphAsymmErrors*   h_HTT_1st_vs_Offline_afterOR_eff_d0;
  TGraphAsymmErrors*   h_HTT_1st_vs_Offline_afterOR_eff_z0;

  // Fake Rate Histograms for first stage
  TH1F*   h_ratio_HTT_1st_Truth;
  TH1F*   h_ratio_HTT_1st_passChi2_Truth;
  TH1F*   h_ratio_HTT_1st_afterOR_Truth;
  TH1F*   h_ratio_HTT_1st_Offline;
  TH1F*   h_ratio_HTT_1st_passChi2_Offline;
  TH1F*   h_ratio_HTT_1st_afterOR_Offline;

  // HTT vs Truth Road Efficiency for second stage
  TGraphAsymmErrors*   h_HTT_2nd_vs_Truth_Road_eff_pt;
  TGraphAsymmErrors*   h_HTT_2nd_vs_Truth_Road_eff_eta;
  TGraphAsymmErrors*   h_HTT_2nd_vs_Truth_Road_eff_phi;
  TGraphAsymmErrors*   h_HTT_2nd_vs_Truth_Road_eff_d0;
  TGraphAsymmErrors*   h_HTT_2nd_vs_Truth_Road_eff_z0;

  TH1F*   h_Truth_Track_Road_2nd_matched_pt;
  TH1F*   h_Truth_Track_Road_2nd_matched_eta;
  TH1F*   h_Truth_Track_Road_2nd_matched_phi;
  TH1F*   h_Truth_Track_Road_2nd_matched_d0;
  TH1F*   h_Truth_Track_Road_2nd_matched_z0;

  // HTT vs Offline Road Efficiency for second stage
  TGraphAsymmErrors*   h_HTT_2nd_vs_Offline_Road_eff_pt;
  TGraphAsymmErrors*   h_HTT_2nd_vs_Offline_Road_eff_eta;
  TGraphAsymmErrors*   h_HTT_2nd_vs_Offline_Road_eff_phi;
  TGraphAsymmErrors*   h_HTT_2nd_vs_Offline_Road_eff_d0;
  TGraphAsymmErrors*   h_HTT_2nd_vs_Offline_Road_eff_z0;

  // HTT vs Truth (All) for second stage
  TGraphAsymmErrors*   h_HTT_2nd_vs_Truth_eff_pt;
  TGraphAsymmErrors*   h_HTT_2nd_vs_Truth_eff_eta;
  TGraphAsymmErrors*   h_HTT_2nd_vs_Truth_eff_phi;
  TGraphAsymmErrors*   h_HTT_2nd_vs_Truth_eff_d0;
  TGraphAsymmErrors*   h_HTT_2nd_vs_Truth_eff_z0;

  TH1F*   h_Truth_Track_HTT_2nd_matched_pt;
  TH1F*   h_Truth_Track_HTT_2nd_matched_eta;
  TH1F*   h_Truth_Track_HTT_2nd_matched_phi;
  TH1F*   h_Truth_Track_HTT_2nd_matched_d0;
  TH1F*   h_Truth_Track_HTT_2nd_matched_z0;

  TH1F*   h_HTT_2nd_vs_Truth_Res_truthmatch_pt;
  TH1F*   h_HTT_2nd_vs_Truth_Res_truthmatch_eta;
  TH1F*   h_HTT_2nd_vs_Truth_Res_truthmatch_phi;
  TH1F*   h_HTT_2nd_vs_Truth_Res_truthmatch_d0;
  TH1F*   h_HTT_2nd_vs_Truth_Res_truthmatch_z0;
  TH1F*   h_HTT_2nd_vs_Truth_Res_truthmatch_qoverpt;

  TH2F*   h_HTT_2nd_vs_Truth_Res_truthmatch_pt_vspt;
  TH2F*   h_HTT_2nd_vs_Truth_Res_truthmatch_eta_vspt;
  TH2F*   h_HTT_2nd_vs_Truth_Res_truthmatch_phi_vspt;
  TH2F*   h_HTT_2nd_vs_Truth_Res_truthmatch_d0_vspt;
  TH2F*   h_HTT_2nd_vs_Truth_Res_truthmatch_z0_vspt;
  TH2F*   h_HTT_2nd_vs_Truth_Res_truthmatch_qoverpt_vspt;

  TH2F*   h_HTT_2nd_vs_Truth_Res_truthmatch_pt_vseta;
  TH2F*   h_HTT_2nd_vs_Truth_Res_truthmatch_eta_vseta;
  TH2F*   h_HTT_2nd_vs_Truth_Res_truthmatch_phi_vseta;
  TH2F*   h_HTT_2nd_vs_Truth_Res_truthmatch_d0_vseta;
  TH2F*   h_HTT_2nd_vs_Truth_Res_truthmatch_z0_vseta;
  TH2F*   h_HTT_2nd_vs_Truth_Res_truthmatch_qoverpt_vseta;

  // HTT vs Truth (passing Chi^2 cut) for second stage
  TGraphAsymmErrors*   h_HTT_2nd_vs_Truth_passChi2_eff_pt;
  TGraphAsymmErrors*   h_HTT_2nd_vs_Truth_passChi2_eff_eta;
  TGraphAsymmErrors*   h_HTT_2nd_vs_Truth_passChi2_eff_phi;
  TGraphAsymmErrors*   h_HTT_2nd_vs_Truth_passChi2_eff_d0;
  TGraphAsymmErrors*   h_HTT_2nd_vs_Truth_passChi2_eff_z0;

  TH1F*   h_Truth_Track_HTT_2nd_passChi2_matched_pt;
  TH1F*   h_Truth_Track_HTT_2nd_passChi2_matched_eta;
  TH1F*   h_Truth_Track_HTT_2nd_passChi2_matched_phi;
  TH1F*   h_Truth_Track_HTT_2nd_passChi2_matched_d0;
  TH1F*   h_Truth_Track_HTT_2nd_passChi2_matched_z0;

  TH1F*   h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_pt;
  TH1F*   h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_eta;
  TH1F*   h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_phi;
  TH1F*   h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_d0;
  TH1F*   h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_z0;
  TH1F*   h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_qoverpt;

  TH2F*   h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_pt_vspt;
  TH2F*   h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_eta_vspt;
  TH2F*   h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_phi_vspt;
  TH2F*   h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_d0_vspt;
  TH2F*   h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_z0_vspt;
  TH2F*   h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_qoverpt_vspt;

  TH2F*   h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_pt_vseta;
  TH2F*   h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_eta_vseta;
  TH2F*   h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_phi_vseta;
  TH2F*   h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_d0_vseta;
  TH2F*   h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_z0_vseta;
  TH2F*   h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_qoverpt_vseta;

  // HTT vs Truth (after overlap removal) for second stage
  TGraphAsymmErrors*   h_HTT_2nd_vs_Truth_afterOR_eff_pt;
  TGraphAsymmErrors*   h_HTT_2nd_vs_Truth_afterOR_eff_eta;
  TGraphAsymmErrors*   h_HTT_2nd_vs_Truth_afterOR_eff_phi;
  TGraphAsymmErrors*   h_HTT_2nd_vs_Truth_afterOR_eff_d0;
  TGraphAsymmErrors*   h_HTT_2nd_vs_Truth_afterOR_eff_z0;

  TH1I*   h_Truth_Track_HTT_2nd_afterOR_Nmatched;
  TH1F*   h_Truth_Track_HTT_2nd_afterOR_matched_pt;
  TH1F*   h_Truth_Track_HTT_2nd_afterOR_matched_eta;
  TH1F*   h_Truth_Track_HTT_2nd_afterOR_matched_phi;
  TH1F*   h_Truth_Track_HTT_2nd_afterOR_matched_d0;
  TH1F*   h_Truth_Track_HTT_2nd_afterOR_matched_z0;

  TH1F*   h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_pt;
  TH1F*   h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_eta;
  TH1F*   h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_phi;
  TH1F*   h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_d0;
  TH1F*   h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_z0;
  TH1F*   h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_qoverpt;

  TH2F*   h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_pt_vspt;
  TH2F*   h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_eta_vspt;
  TH2F*   h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_phi_vspt;
  TH2F*   h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_d0_vspt;
  TH2F*   h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_z0_vspt;
  TH2F*   h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_qoverpt_vspt;

  TH2F*   h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_pt_vseta;
  TH2F*   h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_eta_vseta;
  TH2F*   h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_phi_vseta;
  TH2F*   h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_d0_vseta;
  TH2F*   h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_z0_vseta;
  TH2F*   h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_qoverpt_vseta;

  // HTT vs Offline (All) for second stage
  TGraphAsymmErrors*   h_HTT_2nd_vs_Offline_eff_pt;
  TGraphAsymmErrors*   h_HTT_2nd_vs_Offline_eff_eta;
  TGraphAsymmErrors*   h_HTT_2nd_vs_Offline_eff_phi;
  TGraphAsymmErrors*   h_HTT_2nd_vs_Offline_eff_d0;
  TGraphAsymmErrors*   h_HTT_2nd_vs_Offline_eff_z0;

  // HTT vs Offline (passing Chi^2 cut) for second stage
  TGraphAsymmErrors*   h_HTT_2nd_vs_Offline_passChi2_eff_pt;
  TGraphAsymmErrors*   h_HTT_2nd_vs_Offline_passChi2_eff_eta;
  TGraphAsymmErrors*   h_HTT_2nd_vs_Offline_passChi2_eff_phi;
  TGraphAsymmErrors*   h_HTT_2nd_vs_Offline_passChi2_eff_d0;
  TGraphAsymmErrors*   h_HTT_2nd_vs_Offline_passChi2_eff_z0;

  // HTT vs Offline (after overlap removal) for second stage
  TGraphAsymmErrors*   h_HTT_2nd_vs_Offline_afterOR_eff_pt;
  TGraphAsymmErrors*   h_HTT_2nd_vs_Offline_afterOR_eff_eta;
  TGraphAsymmErrors*   h_HTT_2nd_vs_Offline_afterOR_eff_phi;
  TGraphAsymmErrors*   h_HTT_2nd_vs_Offline_afterOR_eff_d0;
  TGraphAsymmErrors*   h_HTT_2nd_vs_Offline_afterOR_eff_z0;

  // Second stage vs First stage track efficiency and resolution.
  TGraphAsymmErrors*   h_HTT_2nd_vs_HTT_1st_eff_pt;
  TGraphAsymmErrors*   h_HTT_2nd_vs_HTT_1st_eff_eta;
  TGraphAsymmErrors*   h_HTT_2nd_vs_HTT_1st_eff_phi;
  TGraphAsymmErrors*   h_HTT_2nd_vs_HTT_1st_eff_d0;
  TGraphAsymmErrors*   h_HTT_2nd_vs_HTT_1st_eff_z0;

  TH1F*   h_HTT_2nd_vs_HTT_1st_Res_truthmatch_pt;
  TH1F*   h_HTT_2nd_vs_HTT_1st_Res_truthmatch_eta;
  TH1F*   h_HTT_2nd_vs_HTT_1st_Res_truthmatch_phi;
  TH1F*   h_HTT_2nd_vs_HTT_1st_Res_truthmatch_d0;
  TH1F*   h_HTT_2nd_vs_HTT_1st_Res_truthmatch_z0;
  TH1F*   h_HTT_2nd_vs_HTT_1st_Res_truthmatch_qoverpt;

  TH2F*   h_HTT_2nd_vs_HTT_1st_Res_truthmatch_pt_vspt;
  TH2F*   h_HTT_2nd_vs_HTT_1st_Res_truthmatch_eta_vspt;
  TH2F*   h_HTT_2nd_vs_HTT_1st_Res_truthmatch_phi_vspt;
  TH2F*   h_HTT_2nd_vs_HTT_1st_Res_truthmatch_d0_vspt;
  TH2F*   h_HTT_2nd_vs_HTT_1st_Res_truthmatch_z0_vspt;
  TH2F*   h_HTT_2nd_vs_HTT_1st_Res_truthmatch_qoverpt_vspt;

  TH2F*   h_HTT_2nd_vs_HTT_1st_Res_truthmatch_pt_vseta;
  TH2F*   h_HTT_2nd_vs_HTT_1st_Res_truthmatch_eta_vseta;
  TH2F*   h_HTT_2nd_vs_HTT_1st_Res_truthmatch_phi_vseta;
  TH2F*   h_HTT_2nd_vs_HTT_1st_Res_truthmatch_d0_vseta;
  TH2F*   h_HTT_2nd_vs_HTT_1st_Res_truthmatch_z0_vseta;
  TH2F*   h_HTT_2nd_vs_HTT_1st_Res_truthmatch_qoverpt_vseta;

  // Fake Rate Histograms for second stage
  TH1F*   h_ratio_HTT_2nd_Truth;
  TH1F*   h_ratio_HTT_2nd_passChi2_Truth;
  TH1F*   h_ratio_HTT_2nd_afterOR_Truth;
  TH1F*   h_ratio_HTT_2nd_Offline;
  TH1F*   h_ratio_HTT_2nd_passChi2_Offline;
  TH1F*   h_ratio_HTT_2nd_afterOR_Offline;

};

#endif // HTTPerformanceMonitorTool_h
