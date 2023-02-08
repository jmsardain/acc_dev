/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTTSECONDSTAGEMONITORTOOL_H
#define HTTSECONDSTAGEMONITORTOOL_H

#include "TrigHTTMonitor/HTTMonitorBase.h"
#include "TGraphAsymmErrors.h"

/////////////////////////////////////////////////////////////////////////////
class HTTSecondStageMonitorTool: public HTTMonitorBase {

public:

  HTTSecondStageMonitorTool (const std::string&, const std::string&, const IInterface*);
  ~HTTSecondStageMonitorTool() {}

  static const InterfaceID& interfaceID();

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  virtual StatusCode fillHistograms() override;

protected:

    virtual StatusCode bookHistograms() override;
    virtual StatusCode makeEffHistograms() override;

private:

  StatusCode bookTrackHistograms();
  StatusCode bookEfficiencyHistograms();
  StatusCode bookTruthMatchHistograms();
  StatusCode bookBadChi2Histograms();
  
  // Histogram filling functions called within fillHistograms()
  void fill_track_histograms(std::vector<HTTTrack> const * tracks);
  void fill_truthmatch_histograms(std::vector<HTTTrack> const * tracks);
  void fill_bad_chi2_histograms(std::vector<HTTTrack> const * tracks);

  void make_efficiency_histograms();
  void make_truthmatch_efficiency_histograms();

  void fill_bitmask(TH1I *h, uint32_t m, size_t n) {for (size_t l = 0; l < n; l++) if (m & (1 << l)) h->Fill(l);}
  void printHist(std::stringstream & msg, TH1* h) {for (Int_t i = 0; i < h->GetNcells(); i++) msg << h->GetBinContent(i) << " ";}

  BooleanProperty m_fast = false; // only do fast monitoring

  std::vector<size_t> m_sectMult; // number of time each sector has been found 

  // Counters
  size_t m_nEvents = 0;
  size_t m_nEventsRoad = 0; // # events with roads
  size_t m_nRoads = 0;
  size_t m_nCombos = 0; // # of hit combinations in roads


  // HTT Track Histograms

  TH1F*   h_Track_pt;
  TH1F*   h_Track_eta;
  TH1F*   h_Track_phi;
  TH1F*   h_Track_d0;
  TH1F*   h_Track_z0;
  TH1F*   h_Track_chi2;
  TH1F*   h_Track_chi2ndof;
  TH1I*   h_Track_coords;
  TH1I*   h_Track_sector;
  TH1I*   h_Track_nmissing;
  TH1F*   h_Track_chi2PerMissingHit[4];
  TH1F*   h_Track_chi2ndofPerMissingHit[4];

  TH1I*   h_nTracks;
  TH1I*   h_nTracks_passChi2;
  TH1I*   h_nTracks_passOR;

  TH1I*   h_Track_dof;

  TH2F*   h_chi2_sectorID;
  TH2F*   h_chi2ndof_sectorID;

  TH1F*   h_Track_best_pt;
  TH1F*   h_Track_best_eta;
  TH1F*   h_Track_best_phi;
  TH1F*   h_Track_best_d0;
  TH1F*   h_Track_best_z0;
  TH1F*   h_Track_best_chi2;
  TH1F*   h_Track_best_chi2ndof;
  TH1I*   h_Track_best_coords;
  TH1I*   h_Track_best_sector;
  TH1I*   h_Track_best_nmissing;
  TH1F*   h_Track_best_chi2PerMissingHit[4];
  TH1F*   h_Track_best_chi2ndofPerMissingHit[4];

  TH1F*   h_TruthDiff_Track_qoverpt;
  TH1F*   h_TruthDiff_Track_pt;
  TH1F*   h_TruthDiff_Track_eta;
  TH1F*   h_TruthDiff_Track_phi;
  TH1F*   h_TruthDiff_Track_d0;
  TH1F*   h_TruthDiff_Track_z0;


  TGraphAsymmErrors*   h_FullEfficiency_pt;
  TGraphAsymmErrors*   h_FullEfficiency_eta;
  TGraphAsymmErrors*   h_FullEfficiency_phi;
  TGraphAsymmErrors*   h_FullEfficiency_d0;
  TGraphAsymmErrors*   h_FullEfficiency_z0;

  TH1F*   h_Truth_Track_Leading_pt;
  TH1F*   h_Truth_Track_Leading_eta;
  TH1F*   h_Truth_Track_Leading_phi;
  TH1F*   h_Truth_Track_Leading_d0;
  TH1F*   h_Truth_Track_Leading_z0;

  TH1F*   h_Truth_Track_withtrack_pt;
  TH1F*   h_Truth_Track_withtrack_eta;
  TH1F*   h_Truth_Track_withtrack_phi;
  TH1F*   h_Truth_Track_withtrack_d0;
  TH1F*   h_Truth_Track_withtrack_z0;

  // Histograms for doing truth matched efficiencies, purities, resolutions
  TGraphAsymmErrors*   h_HTT_vs_Truth_eff_pt;
  TGraphAsymmErrors*   h_HTT_vs_Truth_eff_eta;
  TGraphAsymmErrors*   h_HTT_vs_Truth_eff_phi;
  TGraphAsymmErrors*   h_HTT_vs_Truth_eff_d0;
  TGraphAsymmErrors*   h_HTT_vs_Truth_eff_z0;

  TGraphAsymmErrors*   h_HTT_vs_Truth_eff_muon_pt;
  TGraphAsymmErrors*   h_HTT_vs_Truth_eff_muon_eta;
  TGraphAsymmErrors*   h_HTT_vs_Truth_eff_muon_phi;
  TGraphAsymmErrors*   h_HTT_vs_Truth_eff_muon_d0;
  TGraphAsymmErrors*   h_HTT_vs_Truth_eff_muon_z0;

  TH1F*   h_Truth_Track_Full_pt;
  TH1F*   h_Truth_Track_Full_eta;
  TH1F*   h_Truth_Track_Full_phi;
  TH1F*   h_Truth_Track_Full_d0;
  TH1F*   h_Truth_Track_Full_z0;

  TH1F*   h_Truth_Track_Full_muon_pt;
  TH1F*   h_Truth_Track_Full_muon_eta;
  TH1F*   h_Truth_Track_Full_muon_phi;
  TH1F*   h_Truth_Track_Full_muon_d0;
  TH1F*   h_Truth_Track_Full_muon_z0;


  TH1F*   h_Truth_Track_HTT_matched_pt;
  TH1F*   h_Truth_Track_HTT_matched_eta;
  TH1F*   h_Truth_Track_HTT_matched_phi;
  TH1F*   h_Truth_Track_HTT_matched_d0;
  TH1F*   h_Truth_Track_HTT_matched_z0;

  TH1F*   h_Truth_Track_HTT_matched_muon_pt;
  TH1F*   h_Truth_Track_HTT_matched_muon_eta;
  TH1F*   h_Truth_Track_HTT_matched_muon_phi;
  TH1F*   h_Truth_Track_HTT_matched_muon_d0;
  TH1F*   h_Truth_Track_HTT_matched_muon_z0;

  TGraphAsymmErrors*   h_Truth_vs_HTT_purity_pt;
  TGraphAsymmErrors*   h_Truth_vs_HTT_purity_eta;
  TGraphAsymmErrors*   h_Truth_vs_HTT_purity_phi;
  TGraphAsymmErrors*   h_Truth_vs_HTT_purity_d0;
  TGraphAsymmErrors*   h_Truth_vs_HTT_purity_z0;

  TH1F*   h_HTT_Track_Truth_matched_pt;
  TH1F*   h_HTT_Track_Truth_matched_eta;
  TH1F*   h_HTT_Track_Truth_matched_phi;
  TH1F*   h_HTT_Track_Truth_matched_d0;
  TH1F*   h_HTT_Track_Truth_matched_z0;

  TH1F*   h_HTT_vs_Truth_Res_qoverpt;
  TH1F*   h_HTT_vs_Truth_Res_pt;
  TH1F*   h_HTT_vs_Truth_Res_eta;
  TH1F*   h_HTT_vs_Truth_Res_phi;
  TH1F*   h_HTT_vs_Truth_Res_d0;
  TH1F*   h_HTT_vs_Truth_Res_z0;

  TH2F*   h_HTT_vs_Truth_Res_pt_vspt;
  TH2F*   h_HTT_vs_Truth_Res_eta_vseta;
  TH2F*   h_HTT_vs_Truth_Res_phi_vsphi;
  TH2F*   h_HTT_vs_Truth_Res_d0_vsd0;
  TH2F*   h_HTT_vs_Truth_Res_z0_vsz0;

  //Histograms for tracks in events which have no good fits (bad chi2)
  TH1F*   h_Track_BadChi2_pt;
  TH1F*   h_Track_BadChi2_eta;
  TH1F*   h_Track_BadChi2_phi;
  TH1F*   h_Track_BadChi2_d0;
  TH1F*   h_Track_BadChi2_z0;
  TH1F*   h_Track_BadChi2_chi2;
  TH1F*   h_Track_BadChi2_chi2ndof;
  TH1I*   h_Track_BadChi2_coords;
  TH1I*   h_Track_BadChi2_sector;
  TH1I*   h_Track_BadChi2_nmissing;

  TH1F*   h_Track_BestBadChi2_pt;
  TH1F*   h_Track_BestBadChi2_eta;
  TH1F*   h_Track_BestBadChi2_phi;
  TH1F*   h_Track_BestBadChi2_d0;
  TH1F*   h_Track_BestBadChi2_z0;
  TH1F*   h_Track_BestBadChi2_chi2;
  TH1F*   h_Track_BestBadChi2_chi2ndof;
  TH1I*   h_Track_BestBadChi2_coords;
  TH1I*   h_Track_BestBadChi2_sector;
  TH1I*   h_Track_BestBadChi2_nmissing;

 

};

#endif // HTTPerformanceMonitorTool_h
