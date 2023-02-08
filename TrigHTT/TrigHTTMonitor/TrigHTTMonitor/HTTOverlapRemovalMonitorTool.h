/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
* @file TrigHTTAlgorithms/HTTOverlapRemovalMonitorTool.h
* @author Zhaoyuan.Cui@cern.ch
* @date Dec. 3, 2020
* @brief Overlap removal monitoring tool.
*/

#ifndef HTTOVERLAPREMOVALMONITORTOOL_H
#define HTTOVERLAPREMOVALMONITORTOOL_H

#include "TrigHTTMonitor/HTTMonitorBase.h"

#include "TrigHTTObjects/HTTMatchInfo.h"
#include "TrigHTTObjects/HTTTrack.h"

#include "TGraphAsymmErrors.h"

typedef std::multimap<HTTMatchInfo,const HTTTrack*> HTTBarcodeMM;

class HTTTruthTrack;
class HTTSectorSlice;
class IHTTEventSelectionSvc;

/**
* @class HTTOverlapRemovalMonitorTool
* @brief A tool for monitoring Overlap Removal.
*
* This class mainly creates histograms for validating HTTOverlapRemovalTool.
*/
class HTTOverlapRemovalMonitorTool: public HTTMonitorBase
{
public:
  HTTOverlapRemovalMonitorTool(const std::string&, const std::string&, const IInterface*);
  ~HTTOverlapRemovalMonitorTool(){}

  static const InterfaceID& interfaceID();

  StatusCode initialize() override;
  StatusCode finalize() override;
  StatusCode bookHistograms() override;
  StatusCode bookTracksHistograms();
  StatusCode bookEfficiencyHistograms();
  StatusCode bookResolutionHistograms();
  StatusCode bookTruthHistogrmas();

  StatusCode fillAllTrackHistograms(std::vector<HTTTrack> const & tracks);
  StatusCode fillORHistograms(std::vector<HTTTrack> const & tracks);
  StatusCode fillbestTrackHistograms(std::vector<HTTTrack> const & tracks);
  StatusCode fillEfficiencyHistograms();
  StatusCode fillORTruthMatchHistograms(std::vector<HTTTruthTrack> const & truth_tracks, std::vector<HTTTrack> const & tracks);
  StatusCode fillBestTruthMatchHistograms(std::vector<HTTTruthTrack> const & truth_tracks, std::vector<HTTTrack> const & tracks);


  //  ServiceHandle
  ServiceHandle<IHTTEventSelectionSvc>  m_EvtSel;

private:
  // All traks histograms
  TH1F* h_nTracks;

  //  OR histograms
  TH1F* h_OR_nTracks;
  TH1F* h_OR_chi2ndof;
  TH1F* h_OR_pt;
  TH1F* h_OR_phi;
  TH1F* h_OR_eta;
  TH1F* h_OR_d0;
  TH1F* h_OR_z0;

  TH1F* h_OR_res_pt;
  TH1F* h_OR_res_phi;
  TH1F* h_OR_res_eta;
  TH1F* h_OR_res_d0;
  TH1F* h_OR_res_z0;

  TH1F* h_truth_with_OR_pt;
  TH1F* h_truth_with_OR_phi;
  TH1F* h_truth_with_OR_eta;
  TH1F* h_truth_with_OR_d0;
  TH1F* h_truth_with_OR_z0;

  //  Truth track histograms
  TH1F* h_truth_pt;
  TH1F* h_truth_phi;
  TH1F* h_truth_eta;
  TH1F* h_truth_d0;
  TH1F* h_truth_z0;

  //  Best track histograms
  TH1F* h_bestTrack_nTracks;
  TH1F* h_bestTrack_pt;
  TH1F* h_bestTrack_phi;
  TH1F* h_bestTrack_eta;
  TH1F* h_bestTrack_d0;
  TH1F* h_bestTrack_z0;
  TH1F* h_bestTrack_chi2ndof;

  TH1F* h_best_res_pt;
  TH1F* h_best_res_phi;
  TH1F* h_best_res_eta;
  TH1F* h_best_res_d0;
  TH1F* h_best_res_z0;

  TH1F* h_truth_with_best_pt;
  TH1F* h_truth_with_best_phi;
  TH1F* h_truth_with_best_eta;
  TH1F* h_truth_with_best_d0;
  TH1F* h_truth_with_best_z0;


  //  efficiencies histograms
  TGraphAsymmErrors* h_OR_eff_pt;
  TGraphAsymmErrors* h_OR_eff_eta;
  TGraphAsymmErrors* h_OR_eff_phi;
  TGraphAsymmErrors* h_OR_eff_d0;
  TGraphAsymmErrors* h_OR_eff_z0;


  TGraphAsymmErrors* h_best_eff_pt;
  TGraphAsymmErrors* h_best_eff_phi;
  TGraphAsymmErrors* h_best_eff_eta;
  TGraphAsymmErrors* h_best_eff_d0;
  TGraphAsymmErrors* h_best_eff_z0;

};

#endif  //  HTTOVERLAPREMOVALMONITORTOOL_H
