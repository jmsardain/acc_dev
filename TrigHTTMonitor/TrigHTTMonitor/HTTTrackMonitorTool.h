/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTTTRACKMONITORTOOL_H
#define HTTTRACKMONITORTOOL_H

#include "TrigHTTMonitor/HTTMonitorBase.h"

/////////////////////////////////////////////////////////////////////////////
class HTTTrackMonitorTool: public HTTMonitorBase
{
public:

  HTTTrackMonitorTool (std::string const & algname, std::string const & name, IInterface const * ifc);
  ~HTTTrackMonitorTool() {}

  static const InterfaceID& interfaceID();

  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms() override;
  virtual StatusCode finalize() override;

protected:

  virtual StatusCode bookHistograms() override;

private:

  // Book histograms
  void resizeHTTTrackHistogramArrays();
  StatusCode bookHTTTrackHistograms();
  StatusCode registerHTTTrackHistograms();
  StatusCode bookGuessedTrackHistograms_1st();
  StatusCode bookGuessedTrackHistograms_2nd();

  // Fill histograms
  void fillHTTTrackHistograms(std::vector<HTTTrack> const * tracks, bool isFirstStage = true);
  void fillGuessedTrackHistograms(std::vector<HTTTrack> const * tracks_guessed, std::vector<HTTTrack> const * tracks_nomiss, bool isFirstStage = true);

  // Flags
  BooleanProperty m_fast = false;
  BooleanProperty m_outputMonitor = false;
  BooleanProperty m_doMissingHitsChecks = false;
  BooleanProperty m_runSecondStage = false;

  // Histogram Types
  unsigned nHistTypes           = 0;   // Number of histograms stored in each histogram array
  const unsigned NHISTTYPES_1st = 4;   // If we run with only 1st stage, there are 4 histograms in each array
  const unsigned NHISTTYPES_2nd = 8;   // If we run with 2nd stage, there are 8 (4*2) histograms in each array
  enum HistType
  {
    HistType_all_1st        = 0,  // HTT First Stage Track Histograms (all)
    HistType_passChi2_1st   = 1,  // HTT First Stage Track Histograms (passing Chi^2 cut)
    HistType_afterOR_1st    = 2,  // HTT First Stage Track Histograms (after overlap removal)
    HistType_badChi2_1st    = 3,  // HTT First Stage Track Histograms (with bad Chi^2 values)
    HistType_all_2nd        = 4,  // HTT Second Stage Track Histograms (all)
    HistType_passChi2_2nd   = 5,  // HTT Second Stage Track Histograms (passing Chi^2 cut)
    HistType_afterOR_2nd    = 6,  // HTT Second Stage Track Histograms (after overlap removal)
    HistType_badChi2_2nd    = 7   // HTT Second Stage Track Histograms (with bad Chi^2 values)
  };

  //====================
  // Histograms
  //====================

  std::vector<TH1I*>  h_nTracks;
  std::vector<TH1I*>  h_nTracks_wide;
  std::vector<TH1I*>  h_nTracks_per_patt;
  std::vector<TH1I*>  h_nConstants;
  std::vector<TH1F*>  h_Track_pt;
  std::vector<TH1F*>  h_Track_eta;
  std::vector<TH1F*>  h_Track_phi;
  std::vector<TH1F*>  h_Track_d0;
  std::vector<TH1F*>  h_Track_z0;
  std::vector<TH1F*>  h_Track_chi2;
  std::vector<TH1F*>  h_Track_chi2ndof;
  std::vector<TH1I*>  h_Track_nCoords;
  std::vector<TH1I*>  h_Track_sectorID;
  std::vector<TH2F*>  h_Track_sectorID_chi2;
  std::vector<TH2F*>  h_Track_sectorID_chi2ndof;
  std::vector<TH1I*>  h_Track_nmissing;
  std::vector<TH1I*>  h_Track_hitType;
  std::vector<TH2F*>  h_Track_chi2PerMissingHit;        // x: layer,     y: Chi^2
  std::vector<TH2F*>  h_Track_chi2ndofPerMissingHit;    // x: layer,     y: Chi^2ndof
  std::vector<TH2F*>  h_Track_Chi2PerMissingLayer;

  std::vector<TH1F*>  h_Track_best_pt;
  std::vector<TH1F*>  h_Track_best_eta;
  std::vector<TH1F*>  h_Track_best_phi;
  std::vector<TH1F*>  h_Track_best_d0;
  std::vector<TH1F*>  h_Track_best_z0;
  std::vector<TH1F*>  h_Track_best_chi2;
  std::vector<TH1F*>  h_Track_best_chi2ndof;
  std::vector<TH1I*>  h_Track_best_nCoords;
  std::vector<TH1I*>  h_Track_best_sectorID;
  std::vector<TH2F*>  h_Track_best_sectorID_chi2;
  std::vector<TH2F*>  h_Track_best_sectorID_chi2ndof;
  std::vector<TH1I*>  h_Track_best_nmissing;
  std::vector<TH2F*>  h_Track_best_chi2PerMissingHit;     // x: layer,     y: Chi^2
  std::vector<TH2F*>  h_Track_best_chi2ndofPerMissingHit; // x: layer,     y: Chi^2ndof

  std::vector<TH1F*>  h_HTT_vs_Truth_Res_no_truthmatch_qoverpt;
  std::vector<TH1F*>  h_HTT_vs_Truth_Res_no_truthmatch_pt;
  std::vector<TH1F*>  h_HTT_vs_Truth_Res_no_truthmatch_eta;
  std::vector<TH1F*>  h_HTT_vs_Truth_Res_no_truthmatch_phi;
  std::vector<TH1F*>  h_HTT_vs_Truth_Res_no_truthmatch_d0;
  std::vector<TH1F*>  h_HTT_vs_Truth_Res_no_truthmatch_z0;
  std::vector<TH1F*>  h_HTT_vs_Truth_Res_no_truthmatch_z0_wide;

  // Histograms for first stage missing hits check
  TH1F*   h_chi2pdf_1st_8outof8;
  TH1F*   h_chi2pdf_1st_guessed[8];
  TH1F*   h_dx_1st_guessed[8];
  TH1F*   h_dy_1st_guessed[8];

  // Histograms for second stage missing hits check
  TH1F*   h_chi2pdf_2nd_13outof13;
  TH1F*   h_chi2pdf_2nd_guessed[13];
  TH1F*   h_dx_2nd_guessed[13];
  TH1F*   h_dy_2nd_guessed[13];
};

#endif // HTTTrackMonitorTool_h
