/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTMonitor/HTTSecondStageMonitorTool.h"

#include "TrigHTTConfig/HTTRegionSlices.h"
#include "TrigHTTObjects/HTTEventInputHeader.h"
#include "TrigHTTObjects/HTTLogicalEventInputHeader.h"
#include "TrigHTTObjects/HTTLogicalEventOutputHeader.h"
#include "TrigHTTObjects/HTTMatchInfo.h"
#include "TrigHTTUtils/HTTHash.h"


typedef std::unordered_set<std::unordered_set<HTTHit const *>, HTT::set_hash_fast<std::unordered_set<HTTHit const *>>> RoadHitsSet;

static const InterfaceID IID_HTTSecondStageMonitorTool("HTTSecondStageMonitorTool", 1, 0);
const InterfaceID& HTTSecondStageMonitorTool::interfaceID()
{ return IID_HTTSecondStageMonitorTool; }


/////////////////////////////////////////////////////////////////////////////
HTTSecondStageMonitorTool::HTTSecondStageMonitorTool(const std::string& algname, const std::string& name, const IInterface *ifc) :
    HTTMonitorBase(algname,name,ifc)
{
    declareInterface<HTTSecondStageMonitorTool>(this);
    declareProperty("fastMon",              m_fast,                 "only do fast monitoring");
    declareProperty("BarcodeFracCut",       m_cut_barcodefrac,      "cut on barcode fraction used in truth matching");
    declareProperty("Chi2ndofCut",          m_cut_chi2ndof,         "cut on Chi2 of HTTTrack");
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTSecondStageMonitorTool::initialize()
{
    ATH_MSG_INFO("HTTSecondStageMonitorTool::initialize()");
    ATH_CHECK(HTTMonitorBase::initialize());
    ATH_CHECK(bookHistograms());

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTSecondStageMonitorTool::bookHistograms()
{
    clearHistDir();
    ATH_CHECK(bookTrackHistograms());
    ATH_CHECK(bookEfficiencyHistograms());
    ATH_CHECK(bookTruthMatchHistograms());
    ATH_CHECK(bookBadChi2Histograms());

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTSecondStageMonitorTool::fillHistograms()
{
    // Use event selection to pass or reject event based on truth info, sample type etc
    ATH_CHECK(selectEvent());

    ATH_CHECK(passMonitorTracks(m_logicEventOutputHeader->getHTTTracks_2nd(), "Tracks_2nd"));

    fill_track_histograms(getMonitorTracks("Tracks_2nd"));
    fill_truthmatch_histograms(getMonitorTracks("Tracks_2nd"));
    fill_bad_chi2_histograms(getMonitorTracks("Tracks_2nd"));

    ///////////////////////////////////////////////////////

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTSecondStageMonitorTool::makeEffHistograms()
{
    make_efficiency_histograms();
    make_truthmatch_efficiency_histograms();

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTSecondStageMonitorTool::finalize()
{
    ATH_MSG_INFO("HTTSecondStageMonitorTool::finalize()");
    ATH_CHECK(HTTMonitorBase::finalize());
    if (!m_fast) ATH_CHECK(makeEffHistograms()); // Slow monitoring

    return StatusCode::SUCCESS;
}




// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTSecondStageMonitorTool::bookTrackHistograms()
{
    setHistDir("/2ndStageHist_debug/TrackHist2ndStage/AllTrackHist/");

    h_Track_pt =  new TH1F("h_Track_pt", "pt for all tracks", getNXbinsPT(), m_xbinsPT); ATH_CHECK(regHist(getHistDir(),h_Track_pt));
    h_Track_eta = new TH1F("h_Track_eta", "eta of all tracks", 40, m_etamin, m_etamax); ATH_CHECK(regHist(getHistDir(),h_Track_eta));
    h_Track_phi = new TH1F("h_Track_phi", "phi of all tracks", 40, m_phimin, m_phimax); ATH_CHECK(regHist(getHistDir(),h_Track_phi));
    h_Track_d0 = new TH1F("h_Track_d0", "d0 of all tracks", 100, -2.5, 2.5); ATH_CHECK(regHist(getHistDir(),h_Track_d0));
    h_Track_z0 = new TH1F("h_Track_z0", "z0 of all tracks", 100, -200., 200.); ATH_CHECK(regHist(getHistDir(),h_Track_z0));
    h_Track_chi2 = new TH1F("h_Track_chi2", "chi^2 of all tracks", 100, 0., 100.); ATH_CHECK(regHist(getHistDir(),h_Track_chi2));
    h_Track_chi2ndof = new TH1F("h_Track_chi2ndof", "chi^2 per ndof of all tracks", 100, 0., 100.); ATH_CHECK(regHist(getHistDir(),h_Track_chi2ndof));
    h_Track_coords = new TH1I("h_Track_coords", "Number of coordinates in all tracks", m_nLayers_2nd+2, 0, m_nLayers_2nd+2); ATH_CHECK(regHist(getHistDir(),h_Track_coords));
    h_Track_sector = new TH1I("h_Track_sector", "Sector ID of all tracks", 75, 0, 75000); ATH_CHECK(regHist(getHistDir(),h_Track_sector));
    h_Track_nmissing = new TH1I("h_Track_nmissing", "NMissing on all tracks", 4, 0, 4); ATH_CHECK(regHist(getHistDir(),h_Track_nmissing));


    setHistDir("/2ndStageHist_debug/TrackHist2ndStage/NTracks/");

    h_nTracks = new TH1I("h_nTracks", "number of tracks found per event;# tracks;# events", 2500, -0.5, 2500-0.5); ATH_CHECK(regHist(getHistDir(), h_nTracks));
    h_nTracks_passChi2  = new TH1I("h_nTracks_passChi2", "number of tracks passing chi2 found per event;# tracks;# events", 2500, -0.5, 2500-0.5); ATH_CHECK(regHist(getHistDir(), h_nTracks_passChi2));
    h_nTracks_passOR = new TH1I("h_nTracks_passOR", "number of tracks passing OR found per event;# tracks;# events", 2500, -0.5, 2500-0.5); ATH_CHECK(regHist(getHistDir(), h_nTracks_passOR));


    setHistDir("/2ndStageHist_debug/TrackHist2ndStage/BestTrackHist/");

    h_Track_best_pt =  new TH1F("h_Track_best_pt", "pt of best track", getNXbinsPT(), m_xbinsPT); ATH_CHECK(regHist(getHistDir(),h_Track_best_pt));
    h_Track_best_eta = new TH1F("h_Track_best_eta", "eta of best track", 40, m_etamin, m_etamax); ATH_CHECK(regHist(getHistDir(),h_Track_best_eta));
    h_Track_best_phi = new TH1F("h_Track_best_phi", "phi of best track", 40, m_phimin, m_phimax); ATH_CHECK(regHist(getHistDir(),h_Track_best_phi));
    h_Track_best_d0 = new TH1F("h_Track_best_d0", "d0 of best track", 100, -2.5, 2.5); ATH_CHECK(regHist(getHistDir(),h_Track_best_d0));
    h_Track_best_z0 = new TH1F("h_Track_best_z0", "z0 of best track", 100, -200., 200.); ATH_CHECK(regHist(getHistDir(),h_Track_best_z0));
    h_Track_best_chi2 = new TH1F("h_Track_best_chi2", "chi^2 of best track", 100, 0., 100.); ATH_CHECK(regHist(getHistDir(),h_Track_best_chi2));
    h_Track_best_chi2ndof = new TH1F("h_Track_best_chi2ndof", "chi^2 per ndof of best track", 100, 0., 100.); ATH_CHECK(regHist(getHistDir(),h_Track_best_chi2ndof));
    h_Track_best_coords = new TH1I("h_Track_best_coords", "Number of coordinates in best track", m_nLayers_2nd+2, 0, m_nLayers_2nd+2); ATH_CHECK(regHist(getHistDir(),h_Track_best_coords));
    h_Track_best_sector = new TH1I("h_Track_best_sector", "Sector ID of best track", 75, 0, 75000); ATH_CHECK(regHist(getHistDir(),h_Track_best_sector));
    h_Track_best_nmissing = new TH1I("h_Track_best_nmissing", "NMissing of best track", 4, 0, 4); ATH_CHECK(regHist(getHistDir(),h_Track_best_nmissing));


    for(int i(0); i<4; i++){
        std::ostringstream Si;
        Si.str("");
        Si << i;

        setHistDir("/2ndStageHist_debug/TrackHist2ndStage/AllTrackHist/");

        h_Track_chi2PerMissingHit[i] = new TH1F(("h_Track_chi2PerMissingHit"+Si.str()).c_str(), ("chi^2 for all tracks w/ missing hits"+Si.str()).c_str(), 100, 0., 100.); ATH_CHECK(regHist(getHistDir(),h_Track_chi2PerMissingHit[i]));
        h_Track_chi2ndofPerMissingHit[i] = new TH1F(("h_Track_chi2ndofPerMissingHit"+Si.str()).c_str(), ("chi^2ndof for all tracks w/ missing hits"+Si.str()).c_str(), 100, 0., 100.); ATH_CHECK(regHist(getHistDir(),h_Track_chi2ndofPerMissingHit[i]));

        setHistDir("/2ndStageHist_debug/TrackHist2ndStage/BestTrackHist/");

        h_Track_best_chi2PerMissingHit[i] = new TH1F(("h_Track_best_chi2PerMissingHit"+Si.str()).c_str(), ("chi^2 for best tracks w/ missing hits"+Si.str()).c_str(), 100, 0., 100.); ATH_CHECK(regHist(getHistDir(),h_Track_best_chi2PerMissingHit[i]));
        h_Track_best_chi2ndofPerMissingHit[i] = new TH1F(("h_Track_best_chi2ndofPerMissingHit"+Si.str()).c_str(), ("chi^2 perndof for best tracks w/ missing hits"+Si.str()).c_str(), 100, 0., 100.); ATH_CHECK(regHist(getHistDir(),h_Track_best_chi2ndofPerMissingHit[i]));
    }


    setHistDir("/2ndStageHist_debug/TrackHist2ndStage/AllTrackHist/");

    h_Track_dof = new TH1I("h_Track_dof","Number of degrees of freedom from calc", 6,0,6); ATH_CHECK(regHist(getHistDir(),h_Track_dof));

    h_chi2_sectorID = new TH2F("h_chi2_sectorID", "chi^2 and sector ID of all fits",75, 0, 75000, 100,0.,100.); ATH_CHECK(regHist(getHistDir(),h_chi2_sectorID));
    h_chi2ndof_sectorID = new TH2F("h_chi2ndof_sectorID", "chi^2 ndof and sector ID of all fits",75, 0, 75000, 100,0.,100.); ATH_CHECK(regHist(getHistDir(),h_chi2ndof_sectorID));


    setHistDir("/2ndStageHist_debug/TrackHist2ndStage/TruthDiffHist/");

    h_TruthDiff_Track_qoverpt = new TH1F("h_TruthDiff_Track_qoverpt","qoverpt of truth minus best track", 200, -0.1, 0.1); ATH_CHECK(regHist(getHistDir(),h_TruthDiff_Track_qoverpt));
    h_TruthDiff_Track_pt = new TH1F("h_TruthDiff_Track_pt", "pt of truth minus best track", 200, -10., 10.); ATH_CHECK(regHist(getHistDir(),h_TruthDiff_Track_pt));
    h_TruthDiff_Track_eta = new TH1F("h_TruthDiff_Track_eta", "eta of truth minus best track", 100, -0.01, 0.01); ATH_CHECK(regHist(getHistDir(),h_TruthDiff_Track_eta));
    h_TruthDiff_Track_phi = new TH1F("h_TruthDiff_Track_phi", "phi of truth minus best track", 100, -0.01, 0.01); ATH_CHECK(regHist(getHistDir(),h_TruthDiff_Track_phi));
    h_TruthDiff_Track_d0 = new TH1F("h_TruthDiff_Track_d0", "d0 of truth minus best track", 100, -1, 1); ATH_CHECK(regHist(getHistDir(),h_TruthDiff_Track_d0));
    h_TruthDiff_Track_z0 = new TH1F("h_TruthDiff_Track_z0", "z0 of truth minus best track", 100, -2., 2.); ATH_CHECK(regHist(getHistDir(),h_TruthDiff_Track_z0));


    clearHistDir();

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTSecondStageMonitorTool::bookEfficiencyHistograms()
{
    setHistDir("/2ndStageHist_debug/CrudeEff2ndStage/EffHist/TrackEff/");

    h_FullEfficiency_pt = new TGraphAsymmErrors(); h_FullEfficiency_pt->SetNameTitle("h_FullEfficiency_pt","Full Efficiency as function of truth pt;p_{T} (GeV);"); ATH_CHECK(regGraph(getHistDir(),h_FullEfficiency_pt));
    h_FullEfficiency_eta = new TGraphAsymmErrors(); h_FullEfficiency_eta->SetNameTitle("h_FullEfficiency_eta","Full Efficiency as function of truth eta;#eta;"); ATH_CHECK(regGraph(getHistDir(),h_FullEfficiency_eta));
    h_FullEfficiency_phi = new TGraphAsymmErrors(); h_FullEfficiency_phi->SetNameTitle("h_FullEfficiency_phi","Full Efficiency as function of truth phi;#phi (rad);"); ATH_CHECK(regGraph(getHistDir(),h_FullEfficiency_phi));
    h_FullEfficiency_d0 = new TGraphAsymmErrors(); h_FullEfficiency_d0->SetNameTitle("h_FullEfficiency_d0","Full Efficiency as function of truth d0;d_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(),h_FullEfficiency_d0));
    h_FullEfficiency_z0 = new TGraphAsymmErrors(); h_FullEfficiency_z0->SetNameTitle("h_FullEfficiency_z0","Full Efficiency as function of truth z0;z_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(),h_FullEfficiency_z0));



    setHistDir("/2ndStageHist_debug/CrudeEff2ndStage/TruthTrackHist/");

    h_Truth_Track_Leading_pt = new TH1F("h_Truth_Track_Leading_pt", "pt of leading truth track", getNXbinsPT(), m_xbinsPT); ATH_CHECK(regHist(getHistDir(),h_Truth_Track_Leading_pt));
    h_Truth_Track_Leading_eta = new TH1F("h_Truth_Track_Leading_eta", "eta of leading truth track", 40, m_etamin, m_etamax); ATH_CHECK(regHist(getHistDir(),h_Truth_Track_Leading_eta));
    h_Truth_Track_Leading_phi = new TH1F("h_Truth_Track_Leading_phi", "phi of leading truth track", 40, m_phimin, m_phimax); ATH_CHECK(regHist(getHistDir(),h_Truth_Track_Leading_phi));
    h_Truth_Track_Leading_d0 = new TH1F("h_Truth_Track_Leading_d0", "d0 of leading truth track", 100, -2.5, 2.5); ATH_CHECK(regHist(getHistDir(),h_Truth_Track_Leading_d0));
    h_Truth_Track_Leading_z0 = new TH1F("h_Truth_Track_Leading_z0", "z0 of leading truth track", 100, -200., 200.); ATH_CHECK(regHist(getHistDir(),h_Truth_Track_Leading_z0));




    setHistDir("/2ndStageHist_debug/CrudeEff2ndStage/MatchedTrackHist/withtrack/");

    h_Truth_Track_withtrack_pt = new TH1F("h_Truth_Track_withtrack_pt", "pt of truth track w/ track", getNXbinsPT(), m_xbinsPT); ATH_CHECK(regHist(getHistDir(),h_Truth_Track_withtrack_pt));
    h_Truth_Track_withtrack_eta = new TH1F("h_Truth_Track_withtrack_eta", "eta of truth track w/ track", 40, m_etamin, m_etamax); ATH_CHECK(regHist(getHistDir(),h_Truth_Track_withtrack_eta));
    h_Truth_Track_withtrack_phi = new TH1F("h_Truth_Track_withtrack_phi", "phi of truth track w/ track", 40, m_phimin, m_phimax); ATH_CHECK(regHist(getHistDir(),h_Truth_Track_withtrack_phi));
    h_Truth_Track_withtrack_d0 = new TH1F("h_Truth_Track_withtrack_d0", "d0 of truth track w/ track", 100, -2.5, 2.5); ATH_CHECK(regHist(getHistDir(),h_Truth_Track_withtrack_d0));
    h_Truth_Track_withtrack_z0 = new TH1F("h_Truth_Track_withtrack_z0", "z0 of truth track w/ track", 100, -200., 200.); ATH_CHECK(regHist(getHistDir(),h_Truth_Track_withtrack_z0));


    clearHistDir();

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTSecondStageMonitorTool::bookTruthMatchHistograms()
{
    setHistDir("/2ndStageHist_debug/TruthMatchHist2ndStage/TruthMatchEff/EffHist/");

    h_HTT_vs_Truth_eff_pt = new TGraphAsymmErrors(); h_HTT_vs_Truth_eff_pt->SetNameTitle("h_HTT_vs_Truth_eff_pt","HTT vs. Truth Efficiency as function of truth pt;p_{T} (GeV);"); ATH_CHECK(regGraph(getHistDir(),h_HTT_vs_Truth_eff_pt));
    h_HTT_vs_Truth_eff_eta = new TGraphAsymmErrors(); h_HTT_vs_Truth_eff_eta->SetNameTitle("h_HTT_vs_Truth_eff_eta","HTT vs. Truth Efficiency as function of truth eta;#eta;"); ATH_CHECK(regGraph(getHistDir(),h_HTT_vs_Truth_eff_eta));
    h_HTT_vs_Truth_eff_phi = new TGraphAsymmErrors(); h_HTT_vs_Truth_eff_phi->SetNameTitle("h_HTT_vs_Truth_eff_phi","HTT vs. Truth Efficiency as function of truth phi;#phi (rad);"); ATH_CHECK(regGraph(getHistDir(),h_HTT_vs_Truth_eff_phi));
    h_HTT_vs_Truth_eff_d0 = new TGraphAsymmErrors(); h_HTT_vs_Truth_eff_d0->SetNameTitle("h_HTT_vs_Truth_eff_d0","HTT vs. Truth Efficiency as function of truth d0;d_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(),h_HTT_vs_Truth_eff_d0));
    h_HTT_vs_Truth_eff_z0 = new TGraphAsymmErrors(); h_HTT_vs_Truth_eff_z0->SetNameTitle("h_HTT_vs_Truth_eff_z0","HTT vs. Truth Efficiency as function of truth z0;z_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(),h_HTT_vs_Truth_eff_z0));

    h_HTT_vs_Truth_eff_muon_pt = new TGraphAsymmErrors(); h_HTT_vs_Truth_eff_muon_pt->SetNameTitle("h_HTT_vs_Truth_eff_muon_pt","HTT vs. Truth Muon Efficiency as function of truth pt;p_{T} (GeV);"); ATH_CHECK(regGraph(getHistDir(),h_HTT_vs_Truth_eff_muon_pt));
    h_HTT_vs_Truth_eff_muon_eta = new TGraphAsymmErrors(); h_HTT_vs_Truth_eff_muon_eta->SetNameTitle("h_HTT_vs_Truth_eff_muon_eta","HTT vs. Truth Muon Efficiency as function of truth eta;#eta;"); ATH_CHECK(regGraph(getHistDir(),h_HTT_vs_Truth_eff_muon_eta));
    h_HTT_vs_Truth_eff_muon_phi = new TGraphAsymmErrors(); h_HTT_vs_Truth_eff_muon_phi->SetNameTitle("h_HTT_vs_Truth_eff_muon_phi","HTT vs. Truth Muon Efficiency as function of truth phi;#phi (rad);"); ATH_CHECK(regGraph(getHistDir(),h_HTT_vs_Truth_eff_muon_phi));
    h_HTT_vs_Truth_eff_muon_d0 = new TGraphAsymmErrors(); h_HTT_vs_Truth_eff_muon_d0->SetNameTitle("h_HTT_vs_Truth_eff_muon_d0","HTT vs. Truth Muon Efficiency as function of truth d0;d_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(),h_HTT_vs_Truth_eff_muon_d0));
    h_HTT_vs_Truth_eff_muon_z0 = new TGraphAsymmErrors(); h_HTT_vs_Truth_eff_muon_z0->SetNameTitle("h_HTT_vs_Truth_eff_muon_z0","HTT vs. Truth Muon Efficiency as function of truth z0;z_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(),h_HTT_vs_Truth_eff_muon_z0));


    setHistDir("/2ndStageHist_debug/TruthMatchHist2ndStage/TruthMatchEff/TruthTrackHist/");

    h_Truth_Track_Full_pt = new TH1F("h_Truth_Track_Full_pt", "pt of all truth tracks", getNXbinsPT(), m_xbinsPT); ATH_CHECK(regHist(getHistDir(),h_Truth_Track_Full_pt));
    h_Truth_Track_Full_eta = new TH1F("h_Truth_Track_Full_eta", "eta of all truth tracks", 40, m_etamin, m_etamax); ATH_CHECK(regHist(getHistDir(),h_Truth_Track_Full_eta));
    h_Truth_Track_Full_phi = new TH1F("h_Truth_Track_Full_phi", "phi of all truth tracks", 40, m_phimin, m_phimax); ATH_CHECK(regHist(getHistDir(),h_Truth_Track_Full_phi));
    h_Truth_Track_Full_d0 = new TH1F("h_Truth_Track_Full_d0", "d0 of all truth tracks", 100, -2.5, 2.5); ATH_CHECK(regHist(getHistDir(),h_Truth_Track_Full_d0));
    h_Truth_Track_Full_z0 = new TH1F("h_Truth_Track_Full_z0", "z0 of all truth tracks", 100, -200., 200.); ATH_CHECK(regHist(getHistDir(),h_Truth_Track_Full_z0));

    h_Truth_Track_Full_muon_pt = new TH1F("h_Truth_Track_Full_muon_pt", "pt of all truth muon tracks", getNXbinsPT(), m_xbinsPT); ATH_CHECK(regHist(getHistDir(),h_Truth_Track_Full_muon_pt));
    h_Truth_Track_Full_muon_eta = new TH1F("h_Truth_Track_Full_muon_eta", "eta of all truth muon tracks", 40, m_etamin, m_etamax); ATH_CHECK(regHist(getHistDir(),h_Truth_Track_Full_muon_eta));
    h_Truth_Track_Full_muon_phi = new TH1F("h_Truth_Track_Full_muon_phi", "phi of all truth muon tracks", 40, m_phimin, m_phimax); ATH_CHECK(regHist(getHistDir(),h_Truth_Track_Full_muon_phi));
    h_Truth_Track_Full_muon_d0 = new TH1F("h_Truth_Track_Full_muon_d0", "d0 of all truth muon tracks", 100, -2.5, 2.5); ATH_CHECK(regHist(getHistDir(),h_Truth_Track_Full_muon_d0));
    h_Truth_Track_Full_muon_z0 = new TH1F("h_Truth_Track_Full_muon_z0", "z0 of all truth muon tracks", 100, -200., 200.); ATH_CHECK(regHist(getHistDir(),h_Truth_Track_Full_muon_z0));


    setHistDir("/2ndStageHist_debug/TruthMatchHist2ndStage/TruthMatchEff/MatchedTrackHist/");

    h_Truth_Track_HTT_matched_pt = new TH1F("h_Truth_Track_HTT_matched_pt", "pt of truth track w/ htt track matched", getNXbinsPT(), m_xbinsPT); ATH_CHECK(regHist(getHistDir(),h_Truth_Track_HTT_matched_pt));
    h_Truth_Track_HTT_matched_eta = new TH1F("h_Truth_Track_HTT_matched_eta", "eta of truth track w/ htt track matched", 40, m_etamin, m_etamax); ATH_CHECK(regHist(getHistDir(),h_Truth_Track_HTT_matched_eta));
    h_Truth_Track_HTT_matched_phi = new TH1F("h_Truth_Track_HTT_matched_phi", "phi of truth track w/ htt track matched", 40, m_phimin, m_phimax); ATH_CHECK(regHist(getHistDir(),h_Truth_Track_HTT_matched_phi));
    h_Truth_Track_HTT_matched_d0 = new TH1F("h_Truth_Track_HTT_matched_d0", "d0 of truth track w/ htt track matched", 100, -2.5, 2.5); ATH_CHECK(regHist(getHistDir(),h_Truth_Track_HTT_matched_d0));
    h_Truth_Track_HTT_matched_z0 = new TH1F("h_Truth_Track_HTT_matched_z0", "z0 of truth track w/ htt track matched", 100, -200., 200.); ATH_CHECK(regHist(getHistDir(),h_Truth_Track_HTT_matched_z0));

    h_Truth_Track_HTT_matched_muon_pt = new TH1F("h_Truth_Track_HTT_matched_muon_pt", "pt of truth muon track w/ htt track matched", getNXbinsPT(), m_xbinsPT); ATH_CHECK(regHist(getHistDir(),h_Truth_Track_HTT_matched_muon_pt));
    h_Truth_Track_HTT_matched_muon_eta = new TH1F("h_Truth_Track_HTT_matched_muon_eta", "eta of truth muon track w/ htt track matched", 40, m_etamin, m_etamax); ATH_CHECK(regHist(getHistDir(),h_Truth_Track_HTT_matched_muon_eta));
    h_Truth_Track_HTT_matched_muon_phi = new TH1F("h_Truth_Track_HTT_matched_muon_phi", "phi of truth muon track w/ htt track matched", 40, m_phimin, m_phimax); ATH_CHECK(regHist(getHistDir(),h_Truth_Track_HTT_matched_muon_phi));
    h_Truth_Track_HTT_matched_muon_d0 = new TH1F("h_Truth_Track_HTT_matched_muon_d0", "d0 of truth muon track w/ htt track matched", 100, -2.5, 2.5); ATH_CHECK(regHist(getHistDir(),h_Truth_Track_HTT_matched_muon_d0));
    h_Truth_Track_HTT_matched_muon_z0 = new TH1F("h_Truth_Track_HTT_matched_muon_z0", "z0 of truth muon track w/ htt track matched", 100, -200., 200.); ATH_CHECK(regHist(getHistDir(),h_Truth_Track_HTT_matched_muon_z0));


    setHistDir("/2ndStageHist_debug/TruthMatchHist2ndStage/TruthMatchPurity/PurityHist/");

    h_Truth_vs_HTT_purity_pt = new TGraphAsymmErrors(); h_Truth_vs_HTT_purity_pt->SetNameTitle("h_Truth_vs_HTT_purity_pt","Truth vs. HTT Purity as function of htt track pt;p_{T} (GeV);"); ATH_CHECK(regGraph(getHistDir(),h_Truth_vs_HTT_purity_pt));
    h_Truth_vs_HTT_purity_eta = new TGraphAsymmErrors(); h_Truth_vs_HTT_purity_eta->SetNameTitle("h_Truth_vs_HTT_purity_eta","Truth vs. HTT Purity as function of htt track eta;#eta;"); ATH_CHECK(regGraph(getHistDir(),h_Truth_vs_HTT_purity_eta));
    h_Truth_vs_HTT_purity_phi = new TGraphAsymmErrors(); h_Truth_vs_HTT_purity_phi->SetNameTitle("h_Truth_vs_HTT_purity_phi","Truth vs. HTT Purity as function of htt track phi;#phi (rad);"); ATH_CHECK(regGraph(getHistDir(),h_Truth_vs_HTT_purity_phi));
    h_Truth_vs_HTT_purity_d0 = new TGraphAsymmErrors(); h_Truth_vs_HTT_purity_d0->SetNameTitle("h_Truth_vs_HTT_purity_d0","Truth vs. HTT Purity as function of htt track d0;d_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(),h_Truth_vs_HTT_purity_d0));
    h_Truth_vs_HTT_purity_z0 = new TGraphAsymmErrors(); h_Truth_vs_HTT_purity_z0->SetNameTitle("h_Truth_vs_HTT_purity_z0","Truth vs. HTT Purity as function of htt track z0;z_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(),h_Truth_vs_HTT_purity_z0));


    setHistDir("/2ndStageHist_debug/TruthMatchHist2ndStage/TruthMatchPurity/MatchedTrackHist/");

    h_HTT_Track_Truth_matched_pt = new TH1F("h_HTT_Track_Truth_matched_pt", "pt of htt track w/ truth track matched", getNXbinsPT(), m_xbinsPT); ATH_CHECK(regHist(getHistDir(),h_HTT_Track_Truth_matched_pt));
    h_HTT_Track_Truth_matched_eta = new TH1F("h_HTT_Track_Truth_matched_eta", "eta of htt track w/ truth track matched", 40, m_etamin, m_etamax); ATH_CHECK(regHist(getHistDir(),h_HTT_Track_Truth_matched_eta));
    h_HTT_Track_Truth_matched_phi = new TH1F("h_HTT_Track_Truth_matched_phi", "phi of htt track w/ truth track matched", 40, m_phimin, m_phimax); ATH_CHECK(regHist(getHistDir(),h_HTT_Track_Truth_matched_phi));
    h_HTT_Track_Truth_matched_d0 = new TH1F("h_HTT_Track_Truth_matched_d0", "d0 of htt track w/ truth track matched", 100, -2.5, 2.5); ATH_CHECK(regHist(getHistDir(),h_HTT_Track_Truth_matched_d0));
    h_HTT_Track_Truth_matched_z0 = new TH1F("h_HTT_Track_Truth_matched_z0", "z0 of htt track w/ truth track matched", 100, -200., 200.); ATH_CHECK(regHist(getHistDir(),h_HTT_Track_Truth_matched_z0));


    setHistDir("/2ndStageHist_debug/TruthMatchHist2ndStage/TruthMatchRes/1D_Hist/");

    h_HTT_vs_Truth_Res_qoverpt = new TH1F("h_HTT_vs_Truth_Res_qoverpt","qoverpt resolution of truth track w/ htt track matched", 200, -0.1, 0.1); ATH_CHECK(regHist(getHistDir(),h_HTT_vs_Truth_Res_qoverpt));
    h_HTT_vs_Truth_Res_pt = new TH1F("h_HTT_vs_Truth_Res_pt", "pt resolution of truth track w/ htt track matched", 200, -10., 10.); ATH_CHECK(regHist(getHistDir(),h_HTT_vs_Truth_Res_pt));
    h_HTT_vs_Truth_Res_eta = new TH1F("h_HTT_vs_Truth_Res_eta", "eta resolution of truth track w/ htt track matched",100, -0.01, 0.01); ATH_CHECK(regHist(getHistDir(),h_HTT_vs_Truth_Res_eta));
    h_HTT_vs_Truth_Res_phi = new TH1F("h_HTT_vs_Truth_Res_phi", "phi resolution of truth track w/ htt track matched",100, -0.01, 0.01); ATH_CHECK(regHist(getHistDir(),h_HTT_vs_Truth_Res_phi));
    h_HTT_vs_Truth_Res_d0 = new TH1F("h_HTT_vs_Truth_Res_d0", "d0 resolution of truth track w/ htt track matched",100, -1, 1); ATH_CHECK(regHist(getHistDir(),h_HTT_vs_Truth_Res_d0));
    h_HTT_vs_Truth_Res_z0 = new TH1F("h_HTT_vs_Truth_Res_z0", "z0 resolution of truth track w/ htt track matched",100, -2, 2); ATH_CHECK(regHist(getHistDir(),h_HTT_vs_Truth_Res_z0));


    setHistDir("/2ndStageHist_debug/TruthMatchHist2ndStage/TruthMatchRes/2D_Hist/");

    h_HTT_vs_Truth_Res_pt_vspt = new TH2F("h_HTT_vs_Truth_Res_pt_vspt", "pt resolution of truth track w/ htt track matched vs pt",getNXbinsPT(), m_xbinsPT, 200, -10., 10.); ATH_CHECK(regHist(getHistDir(),h_HTT_vs_Truth_Res_pt_vspt));
    h_HTT_vs_Truth_Res_eta_vseta = new TH2F("h_HTT_vs_Truth_Res_eta_vseta", "eta resolution of truth track w/ htt track matched vs eta",40, m_etamin, m_etamax,100, -0.01, 0.01); ATH_CHECK(regHist(getHistDir(),h_HTT_vs_Truth_Res_eta_vseta));
    h_HTT_vs_Truth_Res_phi_vsphi = new TH2F("h_HTT_vs_Truth_Res_phi_vsphi", "phi resolution of truth track w/ htt track matched vs phi",40, m_phimin, m_phimax,100, -0.01, 0.01); ATH_CHECK(regHist(getHistDir(),h_HTT_vs_Truth_Res_phi_vsphi));
    h_HTT_vs_Truth_Res_d0_vsd0 = new TH2F("h_HTT_vs_Truth_Res_d0_vsd0", "d0 resolution of truth track w/ htt track matched vs d0", 100, -2.5, 2.5,100, -1, 1); ATH_CHECK(regHist(getHistDir(),h_HTT_vs_Truth_Res_d0_vsd0));
    h_HTT_vs_Truth_Res_z0_vsz0 = new TH2F("h_HTT_vs_Truth_Res_z0_vsz0", "z0 resolution of truth track w/ htt track matched vs z0",100, -200., 200.,100, -2, 2); ATH_CHECK(regHist(getHistDir(),h_HTT_vs_Truth_Res_z0_vsz0));


    clearHistDir();

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTSecondStageMonitorTool::bookBadChi2Histograms()
{
    setHistDir("/2ndStageHist_debug/BadChi2Hist2ndStage/AllBadChi2Hist/");

    h_Track_BadChi2_pt =  new TH1F("h_Track_BadChi2_pt", "pt for Bad-Chi2 event tracks", getNXbinsPT(), m_xbinsPT); ATH_CHECK(regHist(getHistDir(),h_Track_BadChi2_pt));
    h_Track_BadChi2_eta = new TH1F("h_Track_BadChi2_eta", "eta of Bad-Chi2 event tracks", 40, m_etamin, m_etamax); ATH_CHECK(regHist(getHistDir(),h_Track_BadChi2_eta));
    h_Track_BadChi2_phi = new TH1F("h_Track_BadChi2_phi", "phi of Bad-Chi2 event tracks", 40, m_phimin, m_phimax); ATH_CHECK(regHist(getHistDir(),h_Track_BadChi2_phi));
    h_Track_BadChi2_d0 = new TH1F("h_Track_BadChi2_d0", "d0 of Bad-Chi2 event tracks", 100, -2.5, 2.5); ATH_CHECK(regHist(getHistDir(),h_Track_BadChi2_d0));
    h_Track_BadChi2_z0 = new TH1F("h_Track_BadChi2_z0", "z0 of Bad-Chi2 event tracks", 100, -200., 200.); ATH_CHECK(regHist(getHistDir(),h_Track_BadChi2_z0));
    h_Track_BadChi2_chi2 = new TH1F("h_Track_BadChi2_chi2", "chi^2 of Bad-Chi2 event tracks", 100, 0., 500.); ATH_CHECK(regHist(getHistDir(),h_Track_BadChi2_chi2));
    h_Track_BadChi2_chi2ndof = new TH1F("h_Track_BadChi2_chi2ndof", "chi^2 per ndof of Bad-Chi2 event tracks", 100, 0., 500.); ATH_CHECK(regHist(getHistDir(),h_Track_BadChi2_chi2ndof));
    h_Track_BadChi2_coords = new TH1I("h_Track_BadChi2_coords", "Number of coordinates in Bad-Chi2 event tracks", m_nLayers_2nd+2, 0, m_nLayers_2nd+2); ATH_CHECK(regHist(getHistDir(),h_Track_BadChi2_coords));
    h_Track_BadChi2_sector = new TH1I("h_Track_BadChi2_sector", "Sector ID of Bad-Chi2 event tracks", 75, 0, 75000); ATH_CHECK(regHist(getHistDir(),h_Track_BadChi2_sector));
    h_Track_BadChi2_nmissing = new TH1I("h_Track_BadChi2_nmissing", "NMissing on Bad-Chi2 event tracks", 4, 0, 4); ATH_CHECK(regHist(getHistDir(),h_Track_BadChi2_nmissing));


    setHistDir("/2ndStageHist_debug/BadChi2Hist2ndStage/BestBadChi2Hist/");

    h_Track_BestBadChi2_pt =  new TH1F("h_Track_BestBadChi2_pt", "pt for Best Bad-Chi2 event tracks", getNXbinsPT(), m_xbinsPT); ATH_CHECK(regHist(getHistDir(),h_Track_BestBadChi2_pt));
    h_Track_BestBadChi2_eta = new TH1F("h_Track_BestBadChi2_eta", "eta of Best Bad-Chi2 event tracks", 40, m_etamin, m_etamax); ATH_CHECK(regHist(getHistDir(),h_Track_BestBadChi2_eta));
    h_Track_BestBadChi2_phi = new TH1F("h_Track_BestBadChi2_phi", "phi of Best Bad-Chi2 event tracks", 40, m_phimin, m_phimax); ATH_CHECK(regHist(getHistDir(),h_Track_BestBadChi2_phi));
    h_Track_BestBadChi2_d0 = new TH1F("h_Track_BestBadChi2_d0", "d0 of Best Bad-Chi2 event tracks", 100, -2.5, 2.5); ATH_CHECK(regHist(getHistDir(),h_Track_BestBadChi2_d0));
    h_Track_BestBadChi2_z0 = new TH1F("h_Track_BestBadChi2_z0", "z0 of Best Bad-Chi2 event tracks", 100, -200., 200.); ATH_CHECK(regHist(getHistDir(),h_Track_BestBadChi2_z0));
    h_Track_BestBadChi2_chi2 = new TH1F("h_Track_BestBadChi2_chi2", "chi^2 of Best Bad-Chi2 event tracks", 100, 0., 500.); ATH_CHECK(regHist(getHistDir(),h_Track_BestBadChi2_chi2));
    h_Track_BestBadChi2_chi2ndof = new TH1F("h_Track_BestBadChi2_chi2ndof", "chi^2 per ndof of Best Bad-Chi2 event tracks", 100, 0., 500.); ATH_CHECK(regHist(getHistDir(),h_Track_BestBadChi2_chi2ndof));
    h_Track_BestBadChi2_coords = new TH1I("h_Track_BestBadChi2_coords", "Number of coordinates in Best Bad-Chi2 event tracks", m_nLayers_2nd+2, 0, m_nLayers_2nd+2); ATH_CHECK(regHist(getHistDir(),h_Track_BestBadChi2_coords));
    h_Track_BestBadChi2_sector = new TH1I("h_Track_BestBadChi2_sector", "Sector ID of Best Bad-Chi2 event tracks", 75, 0, 75000); ATH_CHECK(regHist(getHistDir(),h_Track_BestBadChi2_sector));
    h_Track_BestBadChi2_nmissing = new TH1I("h_Track_BestBadChi2_nmissing", "NMissing on Best Bad-Chi2 event tracks", 4, 0, 4); ATH_CHECK(regHist(getHistDir(),h_Track_BestBadChi2_nmissing));


    clearHistDir();

    return StatusCode::SUCCESS;
}






// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void HTTSecondStageMonitorTool::fill_track_histograms(std::vector<HTTTrack> const * tracks)
{
    HTTTrack const * best(0x0);

    int ntracks(0),ntrackschi2(0),ntracksor(0);

    std::vector<HTTTruthTrack> const & truthtracks = m_logicEventInputHeader_1st->optional().getTruthTracks();

    ATH_MSG_DEBUG("Have " << tracks->size() << " HTTTracks  and " << truthtracks.size() << " Truth Tracks");

    int bestchi2ndof(9999999.);
    //Fill track quantities for all tracks found
    for (HTTTrack const & t : *tracks){
        h_Track_pt->Fill(fabs(t.getPt())*0.001);
        h_Track_eta->Fill(t.getEta());
        h_Track_phi->Fill(t.getPhi());
        h_Track_d0->Fill(t.getD0());
        h_Track_z0->Fill(t.getZ0());
        h_Track_chi2->Fill(t.getChi2());
        h_Track_chi2ndof->Fill(t.getChi2ndof());
        h_Track_coords->Fill(t.getNCoords()-t.getNMissing());
        h_Track_sector->Fill(t.getFirstSectorID());
        h_chi2_sectorID->Fill(t.getFirstSectorID(),t.getChi2());
        h_chi2ndof_sectorID->Fill(t.getFirstSectorID(),t.getChi2ndof());
        h_Track_chi2PerMissingHit[t.getNMissing()]->Fill(t.getChi2());
        h_Track_chi2ndofPerMissingHit[t.getNMissing()]->Fill(t.getChi2ndof());
        h_Track_nmissing->Fill(t.getNMissing());
        h_Track_dof->Fill(t.getNCoords()-t.getNMissing()-5);
        if(t.getChi2ndof()<bestchi2ndof){
            bestchi2ndof=t.getChi2ndof();
            best=&t;
        }
        ntracks++;
        if(t.getChi2ndof()<=m_cut_chi2ndof) ntrackschi2++;
        if(t.passedOR() == 1) ntracksor++;
    }

    h_nTracks->Fill(ntracks);
    h_nTracks_passChi2->Fill(ntrackschi2);
    h_nTracks_passOR->Fill(ntracksor);

    if(bestchi2ndof<=m_cut_chi2ndof){
        h_Track_best_pt->Fill(fabs(best->getPt())*0.001);
        h_Track_best_eta->Fill(best->getEta());
        h_Track_best_phi->Fill(best->getPhi());
        h_Track_best_d0->Fill(best->getD0());
        h_Track_best_z0->Fill(best->getZ0());
        h_Track_best_chi2->Fill(best->getChi2());
        h_Track_best_chi2ndof->Fill(best->getChi2ndof());
        h_Track_best_coords->Fill(best->getNCoords() - best->getNMissing());
        h_Track_best_sector->Fill(best->getFirstSectorID());
        h_Track_best_chi2PerMissingHit[best->getNMissing()]->Fill(best->getChi2());
        h_Track_best_chi2ndofPerMissingHit[best->getNMissing()]->Fill(best->getChi2ndof());
        h_Track_best_nmissing->Fill(best->getNMissing());
    }

    

    //fill difference of truth minus best (lowest chi^2 per ndof)
    if(bestchi2ndof<=m_cut_chi2ndof){
        for (HTTTruthTrack const & truth_t : truthtracks){
            h_TruthDiff_Track_qoverpt->Fill((abs(truth_t.getQOverPt()) - abs(best->getQOverPt()))*1000.);
            h_TruthDiff_Track_pt->Fill((truth_t.getPt() - best->getPt()) * 0.001);
            h_TruthDiff_Track_eta->Fill(truth_t.getEta() - best->getEta());
            h_TruthDiff_Track_phi->Fill(truth_t.getPhi() - best->getPhi());
            h_TruthDiff_Track_d0->Fill(truth_t.getD0() - best->getD0());
            h_TruthDiff_Track_z0->Fill(truth_t.getZ0() - best->getZ0());
        }
    }
    

    ///////////////////////////////////////////////////////
    // Slow monitoring
    ///////////////////////////////////////////////////////

    if (m_fast) return;
    
    //fill the leading truth track 5 parameters
    h_Truth_Track_Leading_pt->Fill(truthtracks.front().getPt()*0.001);
    h_Truth_Track_Leading_eta->Fill(truthtracks.front().getEta());
    h_Truth_Track_Leading_phi->Fill(truthtracks.front().getPhi());
    h_Truth_Track_Leading_d0->Fill(truthtracks.front().getD0());
    h_Truth_Track_Leading_z0->Fill(truthtracks.front().getZ0());

    if(bestchi2ndof<=m_cut_chi2ndof){
        h_Truth_Track_withtrack_pt->Fill(truthtracks.front().getPt()*0.001);
        h_Truth_Track_withtrack_eta->Fill(truthtracks.front().getEta());
        h_Truth_Track_withtrack_phi->Fill(truthtracks.front().getPhi());
        h_Truth_Track_withtrack_d0->Fill(truthtracks.front().getD0());
        h_Truth_Track_withtrack_z0->Fill(truthtracks.front().getZ0());
    }
    
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void HTTSecondStageMonitorTool::fill_bad_chi2_histograms(std::vector<HTTTrack> const * tracks)
{
    const HTTTrack* best(0x0);

    int bestchi2ndof = std::numeric_limits<int>::max();
    //Fill track quantities for all tracks found
    for (HTTTrack const & t : *tracks){
        if(t.getChi2ndof()<bestchi2ndof){
            bestchi2ndof=t.getChi2ndof();
            best=&t;
        }
    }

    //No good tracks chi^2ndof<40. found so fill histograms looking at inefficiencies
    if(bestchi2ndof>m_cut_chi2ndof) { //All bad-chi2 track
        for (HTTTrack const & b : *tracks){
            h_Track_BadChi2_pt->Fill(fabs(b.getPt())*0.001);
            h_Track_BadChi2_eta->Fill(b.getEta());
            h_Track_BadChi2_phi->Fill(b.getPhi());
            h_Track_BadChi2_d0->Fill(b.getD0());
            h_Track_BadChi2_z0->Fill(b.getZ0());
            h_Track_BadChi2_chi2->Fill(b.getChi2());
            h_Track_BadChi2_chi2ndof->Fill(b.getChi2ndof());
            h_Track_BadChi2_coords->Fill(b.getNCoords()-b.getNMissing());
            h_Track_BadChi2_sector->Fill(b.getFirstSectorID());
        }
        if(best){ //The "best" bad-chi2 track
            h_Track_BestBadChi2_pt->Fill(fabs(best->getPt())*0.001);
            h_Track_BestBadChi2_eta->Fill(best->getEta());
            h_Track_BestBadChi2_phi->Fill(best->getPhi());
            h_Track_BestBadChi2_d0->Fill(best->getD0());
            h_Track_BestBadChi2_z0->Fill(best->getZ0());
            h_Track_BestBadChi2_chi2->Fill(best->getChi2());
            h_Track_BestBadChi2_chi2ndof->Fill(best->getChi2ndof());
            h_Track_BestBadChi2_coords->Fill(best->getNCoords() - best->getNMissing());
            h_Track_BestBadChi2_sector->Fill(best->getFirstSectorID());
        }
    }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void HTTSecondStageMonitorTool::fill_truthmatch_histograms(std::vector<HTTTrack> const * tracks)
{
    typedef std::multimap<HTTMatchInfo,const HTTTrack*> HTTBarcodeMM;

    HTTBarcodeMM httmatchinfo;

    // Information on HTT tracks relative to the hard-scattering events
    // are collected in a vector and later used to build matching maps
    for (HTTTrack const & htt_t : *tracks) {
        if (htt_t.getEventIndex() == 0 && htt_t.getBarcodeFrac() > m_cut_barcodefrac && htt_t.getChi2ndof() <= m_cut_chi2ndof)
                httmatchinfo.insert(std::pair< HTTMatchInfo, HTTTrack const * >(HTTMatchInfo(htt_t.getBarcode(), htt_t.getEventIndex()), &htt_t));
    }

    int tr(0);
    for(const auto& truth_t : m_logicEventInputHeader_1st->optional().getTruthTracks()){
        tr++;
        //ATH_MSG_DEBUG("BMA Running over truth track" << tr );
        if(truth_t.getBarcode()>10000000 || truth_t.getBarcode()==0) continue;
        if(truth_t.getEventIndex()!=0 && truth_t.getQ()==0) continue;
        if (!m_evtSel->passCuts(truth_t)) continue;

        h_Truth_Track_Full_pt->Fill(truth_t.getPt()*0.001);
        h_Truth_Track_Full_eta->Fill(truth_t.getEta());
        h_Truth_Track_Full_phi->Fill(truth_t.getPhi());
        h_Truth_Track_Full_d0->Fill(truth_t.getD0());
        h_Truth_Track_Full_z0->Fill(truth_t.getZ0());

        if (abs(truth_t.getPDGCode())==13) {
          // muon block
            h_Truth_Track_Full_muon_pt->Fill(truth_t.getPt()*0.001);
            h_Truth_Track_Full_muon_eta->Fill(truth_t.getEta());
            h_Truth_Track_Full_muon_phi->Fill(truth_t.getPhi());
            h_Truth_Track_Full_muon_d0->Fill(truth_t.getD0());
            h_Truth_Track_Full_muon_z0->Fill(truth_t.getZ0());
       }

        //ATH_MSG_DEBUG( "BMA truth_t.getBarcode() = " << truth_t.getBarcode() << " truth_t.getEventIndex() " << truth_t.getEventIndex() );
        HTTMatchInfo reftruth(truth_t.getBarcode(),truth_t.getEventIndex());
        std::pair<HTTBarcodeMM::const_iterator,HTTBarcodeMM::const_iterator> mrange = httmatchinfo.equal_range(reftruth);
        if (mrange.first != mrange.second) {
            //Matched truth parameters
            h_Truth_Track_HTT_matched_pt->Fill(truth_t.getPt()*0.001);
            h_Truth_Track_HTT_matched_eta->Fill(truth_t.getEta());
            h_Truth_Track_HTT_matched_phi->Fill(truth_t.getPhi());
            h_Truth_Track_HTT_matched_d0->Fill(truth_t.getD0());
            h_Truth_Track_HTT_matched_z0->Fill(truth_t.getZ0());

            if (abs(truth_t.getPDGCode())==13) {
                // matched muon block
                h_Truth_Track_HTT_matched_muon_pt->Fill(truth_t.getPt()*0.001);
                h_Truth_Track_HTT_matched_muon_eta->Fill(truth_t.getEta());
                h_Truth_Track_HTT_matched_muon_phi->Fill(truth_t.getPhi());
                h_Truth_Track_HTT_matched_muon_d0->Fill(truth_t.getD0());
                h_Truth_Track_HTT_matched_muon_z0->Fill(truth_t.getZ0());
            }
            const HTTTrack *besthtt(0x0);
            for(HTTBarcodeMM::const_iterator httI = mrange.first;httI!=mrange.second;++httI) {
                if (!besthtt) {
                    besthtt = (*httI).second;
                } else if (besthtt->getBarcodeFrac()<(*httI).second->getBarcodeFrac()) {
                    besthtt = (*httI).second;
                }
            }

            if (besthtt) {
                h_HTT_vs_Truth_Res_qoverpt->Fill((abs(truth_t.getQOverPt()) - abs(besthtt->getQOverPt()))*1000.);
                h_HTT_vs_Truth_Res_pt->Fill((truth_t.getPt() - besthtt->getPt()) * 0.001);
                h_HTT_vs_Truth_Res_eta->Fill(truth_t.getEta() - besthtt->getEta());
                h_HTT_vs_Truth_Res_phi->Fill(truth_t.getPhi() - besthtt->getPhi());
                h_HTT_vs_Truth_Res_d0->Fill(truth_t.getD0() - besthtt->getD0());
                h_HTT_vs_Truth_Res_z0->Fill(truth_t.getZ0() - besthtt->getZ0());

                h_HTT_vs_Truth_Res_pt_vspt->Fill(truth_t.getPt() * 0.001, (truth_t.getPt() - besthtt->getPt()) * 0.001);
                h_HTT_vs_Truth_Res_eta_vseta->Fill(truth_t.getEta(), truth_t.getEta() - besthtt->getEta());
                h_HTT_vs_Truth_Res_phi_vsphi->Fill(truth_t.getPhi(), truth_t.getPhi() - besthtt->getPhi());
                h_HTT_vs_Truth_Res_d0_vsd0->Fill(truth_t.getD0(), truth_t.getD0() - besthtt->getD0());
                h_HTT_vs_Truth_Res_z0_vsz0->Fill(truth_t.getZ0(), truth_t.getZ0() - besthtt->getZ0());

            }
        }
    }

    // HTT vs. Truth Purity code starts here.

}



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void HTTSecondStageMonitorTool::make_efficiency_histograms()
{
    getEff(h_FullEfficiency_pt, h_Truth_Track_withtrack_pt, h_Truth_Track_Leading_pt);
    getEff(h_FullEfficiency_eta, h_Truth_Track_withtrack_eta, h_Truth_Track_Leading_eta);
    getEff(h_FullEfficiency_phi, h_Truth_Track_withtrack_phi, h_Truth_Track_Leading_phi);
    getEff(h_FullEfficiency_d0, h_Truth_Track_withtrack_d0, h_Truth_Track_Leading_d0);
    getEff(h_FullEfficiency_z0, h_Truth_Track_withtrack_z0, h_Truth_Track_Leading_z0);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void HTTSecondStageMonitorTool::make_truthmatch_efficiency_histograms()
{
    getEff(h_HTT_vs_Truth_eff_pt, h_Truth_Track_HTT_matched_pt, h_Truth_Track_Full_pt);
    getEff(h_HTT_vs_Truth_eff_eta, h_Truth_Track_HTT_matched_eta, h_Truth_Track_Full_eta);
    getEff(h_HTT_vs_Truth_eff_phi, h_Truth_Track_HTT_matched_phi, h_Truth_Track_Full_phi);
    getEff(h_HTT_vs_Truth_eff_d0, h_Truth_Track_HTT_matched_d0, h_Truth_Track_Full_d0);
    getEff(h_HTT_vs_Truth_eff_z0, h_Truth_Track_HTT_matched_z0, h_Truth_Track_Full_z0);

    getEff(h_Truth_vs_HTT_purity_pt, h_HTT_Track_Truth_matched_pt, h_Track_pt);
    getEff(h_Truth_vs_HTT_purity_eta, h_HTT_Track_Truth_matched_eta, h_Track_eta);
    getEff(h_Truth_vs_HTT_purity_phi, h_HTT_Track_Truth_matched_phi, h_Track_phi);
    getEff(h_Truth_vs_HTT_purity_d0, h_HTT_Track_Truth_matched_d0, h_Track_d0);
    getEff(h_Truth_vs_HTT_purity_z0, h_HTT_Track_Truth_matched_z0, h_Track_z0);
}




