/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTMonitor/HTTTrackMonitorTool.h"

#include "TrigHTTObjects/HTTLogicalEventInputHeader.h"
#include "TrigHTTObjects/HTTLogicalEventOutputHeader.h"


static const InterfaceID IID_HTTTrackMonitorTool("HTTTrackMonitorTool", 1, 0);
const InterfaceID& HTTTrackMonitorTool::interfaceID()
{ return IID_HTTTrackMonitorTool; }


/////////////////////////////////////////////////////////////////////////////
HTTTrackMonitorTool::HTTTrackMonitorTool(std::string const & algname, std::string const & name, IInterface const * ifc) :
    HTTMonitorBase(algname,name,ifc)
{
    declareInterface<HTTTrackMonitorTool>(this);
    declareProperty("fastMon",              m_fast,                 "only do fast monitoring");
    declareProperty("OutputMon",            m_outputMonitor,        "do monitoring on output data (HTTLogicalEventInputHeader, HTTLogicalEventOutputHeader)");
    declareProperty("DoMissingHitsChecks",  m_doMissingHitsChecks,  "flag to enable missing hits check");
    declareProperty("RunSecondStage",       m_runSecondStage,       "flag to enable running the second stage fitting");
    declareProperty("Chi2ndofCut",          m_cut_chi2ndof,         "cut on Chi2 of HTTTrack");
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTTrackMonitorTool::initialize()
{
    ATH_MSG_INFO("HTTTrackMonitorTool::initialize()");
    ATH_CHECK(HTTMonitorBase::initialize());
    ATH_CHECK(bookHistograms());
    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTTrackMonitorTool::bookHistograms()
{
	resizeHTTTrackHistogramArrays();
    ATH_CHECK(bookHTTTrackHistograms());
    ATH_CHECK(registerHTTTrackHistograms());
    if (m_doMissingHitsChecks && !m_outputMonitor && !m_fast) {
        ATH_CHECK(bookGuessedTrackHistograms_1st());
        if (m_runSecondStage) ATH_CHECK(bookGuessedTrackHistograms_2nd());
    }
    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTTrackMonitorTool::fillHistograms()
{
    // Use event selection to pass or reject event based on truth info, sample type etc
    ATH_CHECK(selectEvent());

    std::vector<HTTTrack> const * tracks_1st = nullptr;
    std::vector<HTTTrack> const * tracks_2nd = nullptr;

    if (!m_logicEventOutputHeader) {
        tracks_1st = getMonitorTracks("Tracks_1st");
        if (m_runSecondStage) tracks_2nd = getMonitorTracks("Tracks_2nd");
    }
    else {
        tracks_1st = &(m_logicEventOutputHeader->getHTTTracks_1st());
        if (m_runSecondStage) tracks_2nd = &(m_logicEventOutputHeader->getHTTTracks_2nd());
    }

    fillHTTTrackHistograms(tracks_1st);
    if (m_runSecondStage) fillHTTTrackHistograms(tracks_2nd, false);

    if (m_eventInputHeader && m_doMissingHitsChecks) {
        fillGuessedTrackHistograms(getMonitorTracks("Tracks_1st_Guessed"), getMonitorTracks("Tracks_1st_NoMiss"));
        if (m_runSecondStage) fillGuessedTrackHistograms(getMonitorTracks("Tracks_2nd_Guessed"), getMonitorTracks("Tracks_2nd_NoMiss"), false);
    }

    clearMonitorData();

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTTrackMonitorTool::finalize()
{
    ATH_MSG_INFO("HTTTrackMonitorTool::finalize()");
    ATH_CHECK(HTTMonitorBase::finalize());
    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void HTTTrackMonitorTool::resizeHTTTrackHistogramArrays()
{
	nHistTypes = m_runSecondStage ? NHISTTYPES_2nd : NHISTTYPES_1st;

	h_nTracks.resize(nHistTypes);
	h_nTracks_wide.resize(nHistTypes);
	h_nTracks_per_patt.resize(nHistTypes);
	h_nConstants.resize(nHistTypes);
	h_Track_pt.resize(nHistTypes);
	h_Track_eta.resize(nHistTypes);
	h_Track_phi.resize(nHistTypes);
	h_Track_d0.resize(nHistTypes);
	h_Track_z0.resize(nHistTypes);
	h_Track_chi2.resize(nHistTypes);
	h_Track_chi2ndof.resize(nHistTypes);
	h_Track_nCoords.resize(nHistTypes);
	h_Track_sectorID.resize(nHistTypes);
	h_Track_sectorID_chi2.resize(nHistTypes);
	h_Track_sectorID_chi2ndof.resize(nHistTypes);
	h_Track_nmissing.resize(nHistTypes);
	h_Track_hitType.resize(nHistTypes);
	h_Track_chi2PerMissingHit.resize(nHistTypes);
	h_Track_chi2ndofPerMissingHit.resize(nHistTypes);
	h_Track_Chi2PerMissingLayer.resize(nHistTypes);

	h_Track_best_pt.resize(nHistTypes);
	h_Track_best_eta.resize(nHistTypes);
	h_Track_best_phi.resize(nHistTypes);
	h_Track_best_d0.resize(nHistTypes);
	h_Track_best_z0.resize(nHistTypes);
	h_Track_best_chi2.resize(nHistTypes);
	h_Track_best_chi2ndof.resize(nHistTypes);
	h_Track_best_nCoords.resize(nHistTypes);
	h_Track_best_sectorID.resize(nHistTypes);
	h_Track_best_sectorID_chi2.resize(nHistTypes);
	h_Track_best_sectorID_chi2ndof.resize(nHistTypes);
	h_Track_best_nmissing.resize(nHistTypes);
	h_Track_best_chi2PerMissingHit.resize(nHistTypes);
	h_Track_best_chi2ndofPerMissingHit.resize(nHistTypes);

	h_HTT_vs_Truth_Res_no_truthmatch_qoverpt.resize(nHistTypes);
	h_HTT_vs_Truth_Res_no_truthmatch_pt.resize(nHistTypes);
	h_HTT_vs_Truth_Res_no_truthmatch_eta.resize(nHistTypes);
	h_HTT_vs_Truth_Res_no_truthmatch_phi.resize(nHistTypes);
	h_HTT_vs_Truth_Res_no_truthmatch_d0.resize(nHistTypes);
	h_HTT_vs_Truth_Res_no_truthmatch_z0.resize(nHistTypes);
	h_HTT_vs_Truth_Res_no_truthmatch_z0_wide.resize(nHistTypes);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTTrackMonitorTool::bookHTTTrackHistograms()
{
    for (unsigned n = 0; n < nHistTypes; n++) {
        std::string str_stage_short, str_stage_long, str_all, str_cut_short, str_cut_long;

        if (n < NHISTTYPES_1st) {
            str_stage_short = "_1st";
            str_stage_long = "first ";
        }
        else {
            str_stage_short = "_2nd";
            str_stage_long = "second ";
        }

        switch (n) {
            case HistType_all_1st:
            case HistType_all_2nd:
                str_all = "all ";
                break;
            case HistType_passChi2_1st:
            case HistType_passChi2_2nd:
                str_cut_short = "_passChi2";
                str_cut_long = " passing Chi^2 cut";
                break;
            case HistType_afterOR_1st:
            case HistType_afterOR_2nd:
                str_cut_short = "_afterOR";
                str_cut_long = " after overlap removal";
                break;
            case HistType_badChi2_1st:
            case HistType_badChi2_2nd:
                str_cut_short = "_badChi2";
                str_cut_long = " with bad Chi^2 value";
                break;
        }

        //===============================================
        // Histograms of tracks with different cuts
        //===============================================
        h_nTracks[n] = new TH1I(("h_nTracks" + str_stage_short + str_cut_short).c_str(), ("number of " + str_all + str_stage_long + "stage tracks found per event" + str_cut_long + "; # tracks; # events").c_str(), 200, -0.5, 200-0.5);

        if (n == HistType_all_1st || n == HistType_all_2nd) h_nTracks_wide[n] = new TH1I(("h_nTracks" + str_stage_short + str_cut_short + "_wide").c_str(), ("number of " + str_all + str_stage_long + "stage tracks found per event" + str_cut_long + "; # tracks; # events").c_str(), 200, -0.5, 2000-0.5);
        
        if (n != HistType_badChi2_1st && n != HistType_badChi2_2nd) {
            h_nTracks_per_patt[n] = new TH1I(("h_nTracks"    + str_stage_short + "_per_patt" + str_cut_short).c_str(), ("number of " + str_all + str_stage_long + "stage tracks found per pattern per event" + str_cut_long + "; # tracks; # patterns" ).c_str(), 150, -0.5, 150-0.5);
            h_nConstants[n]       = new TH1I(("h_nConstants" + str_stage_short + str_cut_short              ).c_str(), ("number of unique "    + str_stage_long + "stage constants used per event"           + str_cut_long + "; # constants; # events").c_str(), 500, -0.5, 500-0.5);
        }

        h_Track_pt[n]       = new TH1F(("h_Track" + str_stage_short + str_cut_short + "_pt"      ).c_str(), ("pt of "             + str_all + str_stage_long + "stage tracks" + str_cut_long + "; p_{T} (GeV)").c_str(), getNXbinsPT(), m_xbinsPT);
        h_Track_eta[n]      = new TH1F(("h_Track" + str_stage_short + str_cut_short + "_eta"     ).c_str(), ("eta of "            + str_all + str_stage_long + "stage tracks" + str_cut_long + "; #eta"       ).c_str(), 50, m_etamin, m_etamax);
        h_Track_phi[n]      = new TH1F(("h_Track" + str_stage_short + str_cut_short + "_phi"     ).c_str(), ("phi of "            + str_all + str_stage_long + "stage tracks" + str_cut_long + "; #phi"       ).c_str(), 50, m_phimin, m_phimax);
        h_Track_d0[n]       = new TH1F(("h_Track" + str_stage_short + str_cut_short + "_d0"      ).c_str(), ("d0 of "             + str_all + str_stage_long + "stage tracks" + str_cut_long + "; d_{0} (mm)" ).c_str(), 50, m_d0Range[0], m_d0Range[1]);
        h_Track_z0[n]       = new TH1F(("h_Track" + str_stage_short + str_cut_short + "_z0"      ).c_str(), ("z0 of "             + str_all + str_stage_long + "stage tracks" + str_cut_long + "; z_{0} (mm)" ).c_str(), 50, m_z0Range[0], m_z0Range[1]);
        h_Track_chi2[n]     = new TH1F(("h_Track" + str_stage_short + str_cut_short + "_chi2"    ).c_str(), ("Chi^2 of "          + str_all + str_stage_long + "stage tracks" + str_cut_long + "; #Chi^{2}"   ).c_str(), 100, 0, 30000);
        h_Track_chi2ndof[n] = new TH1F(("h_Track" + str_stage_short + str_cut_short + "_chi2ndof").c_str(), ("Chi^2 per ndof of " + str_all + str_stage_long + "stage tracks" + str_cut_long + "; #Chi^{2}"   ).c_str(), 100, 0, 10000);
        
        if (n < NHISTTYPES_1st)
            h_Track_nCoords[n] = new TH1I(("h_Track" + str_stage_short + str_cut_short + "_nCoords").c_str(), ("Number of coordinates in " + str_all + str_stage_long + "stage tracks" + str_cut_long + "; n.  coordinates").c_str(), m_nLayers_1st + 2, -0.5, m_nLayers_1st + 1.5);
        else
            h_Track_nCoords[n] = new TH1I(("h_Track" + str_stage_short + str_cut_short + "_nCoords").c_str(), ("Number of coordinates in " + str_all + str_stage_long + "stage tracks" + str_cut_long + "; n.  coordinates").c_str(), m_nLayers_2nd + 6, -0.5, m_nLayers_2nd + 5.5);

        h_Track_sectorID[n]              = new TH1I(("h_Track" + str_stage_short + str_cut_short + "_sectorID"             ).c_str(), ("Sector ID of "                + str_all + str_stage_long + "stage tracks"                                       + str_cut_long + "; sector ID"                     ).c_str(), 100, 0, 250000);
        h_Track_sectorID_chi2[n]         = new TH2F(("h_Track" + str_stage_short + str_cut_short + "_sectorID_chi2"        ).c_str(), ("Sector ID and Chi^2 of "      + str_all + str_stage_long + "stage tracks"                                       + str_cut_long + "; sector ID;Chi^2"               ).c_str(), 100, 0, 250000, 100, 0, 30000);
        h_Track_sectorID_chi2ndof[n]     = new TH2F(("h_Track" + str_stage_short + str_cut_short + "_sectorID_chi2ndof"    ).c_str(), ("Sector ID and Chi^2 ndof of " + str_all + str_stage_long + "stage tracks"                                       + str_cut_long + "; sector ID;Chi^2 ndof"          ).c_str(), 100, 0, 250000, 100, 0, 10000);
        h_Track_nmissing[n]              = new TH1I(("h_Track" + str_stage_short + str_cut_short + "_nmissing"             ).c_str(), ("NMissing on "                 + str_all + str_stage_long + "stage tracks"                                       + str_cut_long + "; # missing coordinates on track").c_str(), 3, -0.5, 3-0.5);
        h_Track_chi2PerMissingHit[n]     = new TH2F(("h_Track" + str_stage_short + str_cut_short + "_chi2PerMissingHit"    ).c_str(), ("chi^2 of "                    + str_all + str_stage_long + "stage tracks w/ different # of missing coordinates" + str_cut_long + "; # of missing hits;chi^2"       ).c_str(), 3, -0.5, 3-0.5, 100, 0, 30000);
        h_Track_chi2ndofPerMissingHit[n] = new TH2F(("h_Track" + str_stage_short + str_cut_short + "_chi2ndofPerMissingHit").c_str(), ("chi^2ndof of "                + str_all + str_stage_long + "stage tracks w/ different # of missing coordinates" + str_cut_long + "; # of missing hits;chi^2ndof"   ).c_str(), 3, -0.5, 3-0.5, 100, 0, 10000);

        h_Track_hitType[n] = new TH1I(("h_Track" + str_stage_short + str_cut_short + "_hitType").c_str(), ("Types of hits on " + str_all + str_stage_long + "stage tracks" + str_cut_long + "; # Hit type; tracks").c_str(), static_cast<int>(HitType::undefined) + 1,  static_cast<int>(HitType::unmapped), static_cast<int>(HitType::undefined) + 1);
        TAxis* raxis = h_Track_hitType[n]->GetXaxis();
        raxis->SetBinLabel(static_cast<int>(HitType::unmapped)  + 1, "unmapped");
        raxis->SetBinLabel(static_cast<int>(HitType::mapped)    + 1, "mapped");
        raxis->SetBinLabel(static_cast<int>(HitType::clustered) + 1, "clustered");
        raxis->SetBinLabel(static_cast<int>(HitType::wildcard)  + 1, "wildcard");
        raxis->SetBinLabel(static_cast<int>(HitType::guessed)   + 1, "guessed");
        raxis->SetBinLabel(static_cast<int>(HitType::undefined) + 1, "undefined");

        h_Track_Chi2PerMissingLayer[n] = new TH2F(("h_Track" + str_stage_short + str_cut_short + "_Chi2PerMissingLayer").c_str(), ("chi^2 of " + str_all + str_stage_long + "stage tracks per missing layer mask in the fit" + str_cut_long + "; Layer ID; ").c_str(), 256, 0, 256, 1000, 0, 100000);

        if (n == HistType_all_1st || n == HistType_all_2nd || n == HistType_afterOR_1st || n == HistType_afterOR_2nd) {
            //============================================================
            // Histograms of best track (track with lowest Chi^2 cut)
            //============================================================
            h_Track_best_pt[n]       = new TH1F(("h_Track" + str_stage_short + str_cut_short + "_best_pt"      ).c_str(), ("pt of best "             + str_stage_long + "stage track" + str_cut_long + "; p_{T} (GeV)").c_str(), 100, 0, 400);
            h_Track_best_eta[n]      = new TH1F(("h_Track" + str_stage_short + str_cut_short + "_best_eta"     ).c_str(), ("eta of best "            + str_stage_long + "stage track" + str_cut_long + "; #eta"       ).c_str(), 50, m_etamin, m_etamax);
            h_Track_best_phi[n]      = new TH1F(("h_Track" + str_stage_short + str_cut_short + "_best_phi"     ).c_str(), ("phi of best "            + str_stage_long + "stage track" + str_cut_long + "; #phi"       ).c_str(), 50, m_phimin, m_phimax);
            h_Track_best_d0[n]       = new TH1F(("h_Track" + str_stage_short + str_cut_short + "_best_d0"      ).c_str(), ("d0 of best "             + str_stage_long + "stage track" + str_cut_long + "; d_{0} (mm)" ).c_str(), 50, m_d0Range[0], m_d0Range[1]);
            h_Track_best_z0[n]       = new TH1F(("h_Track" + str_stage_short + str_cut_short + "_best_z0"      ).c_str(), ("z0 of best "             + str_stage_long + "stage track" + str_cut_long + "; z_{0} (mm)" ).c_str(), 50, m_z0Range[0], m_z0Range[1]);
            h_Track_best_chi2[n]     = new TH1F(("h_Track" + str_stage_short + str_cut_short + "_best_chi2"    ).c_str(), ("Chi^2 of best "          + str_stage_long + "stage track" + str_cut_long + "; #Chi^{2}"   ).c_str(), 100, 0, 100);
            h_Track_best_chi2ndof[n] = new TH1F(("h_Track" + str_stage_short + str_cut_short + "_best_chi2ndof").c_str(), ("Chi^2 per ndof of best " + str_stage_long + "stage track" + str_cut_long + "; #Chi^{2}"   ).c_str(), 100, 0, 50);

            if (n < NHISTTYPES_1st)
                h_Track_best_nCoords[n] = new TH1I(("h_Track" + str_stage_short + str_cut_short + "_best_nCoords").c_str(), ("Number of coordinates in best " + str_stage_long + "stage track" + str_cut_long + "; $ coordianates").c_str(), m_nLayers_1st + 2, -0.5, m_nLayers_1st + 1.5);
            else
                h_Track_best_nCoords[n] = new TH1I(("h_Track" + str_stage_short + str_cut_short + "_best_nCoords").c_str(), ("Number of coordinates in best " + str_stage_long + "stage track" + str_cut_long + "; $ coordianates").c_str(), m_nLayers_2nd + 6, -0.5, m_nLayers_2nd + 5.5);

            h_Track_best_sectorID[n]              = new TH1I(("h_Track" + str_stage_short + str_cut_short + "_best_sectorID"             ).c_str(), ("Sector ID of best "                + str_stage_long + "stage track"                                + str_cut_long + "; sector ID"                  ).c_str(), 100, 0, 200000);
            h_Track_best_sectorID_chi2[n]         = new TH2F(("h_Track" + str_stage_short + str_cut_short + "_best_sectorID_chi2"        ).c_str(), ("Sector ID and Chi^2 of best "      + str_stage_long + "stage track"                                + str_cut_long + "; sector ID; #Chi^{2}"        ).c_str(), 100, 0, 200000, 100, 0, 100);
            h_Track_best_sectorID_chi2ndof[n]     = new TH2F(("h_Track" + str_stage_short + str_cut_short + "_best_sectorID_chi2ndof"    ).c_str(), ("Sector ID and chi^2 ndof of best " + str_stage_long + "stage track"                                + str_cut_long + "; sector ID; #Chi^{2}"        ).c_str(), 1000, 0, 40000, 100, 0, 5000);
            h_Track_best_nmissing[n]              = new TH1I(("h_Track" + str_stage_short + str_cut_short + "_best_nmissing"             ).c_str(), ("NMissing of best "                 + str_stage_long + "stage track"                                + str_cut_long + "; # missing hits on track"    ).c_str(), 3, -0.5, 3-0.5);
            h_Track_best_chi2PerMissingHit[n]     = new TH2F(("h_Track" + str_stage_short + str_cut_short + "_best_chi2PerMissingHit"    ).c_str(), ("chi^2 of best "                    + str_stage_long + "stage track w/ different # of missing hits" + str_cut_long + "; # of missing hits;chi^2"    ).c_str(), 3, -0.5, 3-0.5, 100, 0, 20);
            h_Track_best_chi2ndofPerMissingHit[n] = new TH2F(("h_Track" + str_stage_short + str_cut_short + "_best_chi2ndofPerMissingHit").c_str(), ("chi^2ndof of best "                + str_stage_long + "stage track w/ different # of missing hits" + str_cut_long + "; # of missing hits;chi^2ndof").c_str(), 3, -0.5, 3-0.5, 100, 0, 10);

            //============================================================
            // Histograms of resolution
            //============================================================
            h_HTT_vs_Truth_Res_no_truthmatch_qoverpt[n] = new TH1F(("h_HTT" + str_stage_short + "_vs_Truth"  + str_cut_short + "_Res_no_truthmatch_qoverpt").c_str(), ("qoverpt resolution of truth track w/ " + str_stage_long + "stage htt track matched" + str_cut_long + " without truth matching; #Delta q/p_{T} (GeV^{-1})"     ).c_str(), 100, (-1.)*ResHistLimit_ipt, ResHistLimit_ipt);
            h_HTT_vs_Truth_Res_no_truthmatch_pt[n]      = new TH1F(("h_HTT" + str_stage_short + "_vs_Truth"  + str_cut_short + "_Res_no_truthmatch_pt"     ).c_str(), ("pt resolution of truth track w/ "      + str_stage_long + "stage htt track matched" + str_cut_long + " without truth matching; #Delta p_{T} (GeV)"            ).c_str(), 100, (-1.)*ResHistLimit_pt, ResHistLimit_pt);
            h_HTT_vs_Truth_Res_no_truthmatch_eta[n]     = new TH1F(("h_HTT" + str_stage_short + "_vs_Truth"  + str_cut_short + "_Res_no_truthmatch_eta"    ).c_str(), ("eta resolution of truth track w/ "     + str_stage_long + "stage htt track matched" + str_cut_long + " without truth matching; #Delta #eta"                   ).c_str(), 100, (-1.)*ResHistLimit_eta, ResHistLimit_eta);
            h_HTT_vs_Truth_Res_no_truthmatch_phi[n]     = new TH1F(("h_HTT" + str_stage_short + "_vs_Truth"  + str_cut_short + "_Res_no_truthmatch_phi"    ).c_str(), ("phi resolution of truth track w/ "     + str_stage_long + "stage htt track matched" + str_cut_long + " without truth matching; #Delta #phi"                   ).c_str(), 100, (-1.)*ResHistLimit_phi, ResHistLimit_phi);
            h_HTT_vs_Truth_Res_no_truthmatch_d0[n]      = new TH1F(("h_HTT" + str_stage_short + "_vs_Truth"  + str_cut_short + "_Res_no_truthmatch_d0"     ).c_str(), ("d0 resolution of truth track w/ "      + str_stage_long + "stage htt track matched" + str_cut_long + " without truth matching; #Delta d_{0} (mm)"             ).c_str(), 100, (-1.)*ResHistLimit_d0, ResHistLimit_d0);
            h_HTT_vs_Truth_Res_no_truthmatch_z0[n]      = new TH1F(("h_HTT" + str_stage_short + "_vs_Truth"  + str_cut_short + "_Res_no_truthmatch_z0"     ).c_str(), ("z0 resolution of truth track w/ "      + str_stage_long + "stage htt track matched" + str_cut_long + " without truth matching; #Delta z_{0} (mm)"             ).c_str(), 100, (-1.)*ResHistLimit_z0, ResHistLimit_z0);
            h_HTT_vs_Truth_Res_no_truthmatch_z0_wide[n] = new TH1F(("h_HTT" + str_stage_short + "_vs_Truth"  + str_cut_short + "_Res_no_truthmatch_z0_wide").c_str(), ("z0 resolution of truth track w/ "      + str_stage_long + "stage htt track matched" + str_cut_long + " without truth matching (wide range); #Delta z_{0} (mm)").c_str(), 100, -25000, 25000);
        }
    }

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTTrackMonitorTool::registerHTTTrackHistograms()
{
	for (unsigned n = 0; n < nHistTypes; n++) {
    	switch (n) {
    		case HistType_all_1st:
    			setHistDir(createDirName("/HTTTrackHist_1st/AllTrackHist/TotalTrackHist/"));
    			break;
    		case HistType_passChi2_1st:
    			setHistDir(createDirName("/HTTTrackHist_1st/GoodChi2TrackHist/TotalTrackHist/"));
    			break;
    		case HistType_afterOR_1st:
    			setHistDir(createDirName("/HTTTrackHist_1st/ORTrackHist/TotalTrackHist/"));
    			break;
    		case HistType_badChi2_1st:
    			setHistDir(createDirName("/HTTTrackHist_1st/BadChi2TrackHist/TotalTrackHist/"));
    			break;
    		case HistType_all_2nd:
    			setHistDir(createDirName("/HTTTrackHist_2nd/AllTrackHist/TotalTrackHist/"));
    			break;
    		case HistType_passChi2_2nd:
    			setHistDir(createDirName("/HTTTrackHist_2nd/GoodChi2TrackHist/TotalTrackHist/"));
    			break;
    		case HistType_afterOR_2nd:
    			setHistDir(createDirName("/HTTTrackHist_2nd/ORTrackHist/TotalTrackHist/"));
    			break;
    		case HistType_badChi2_2nd:
    			setHistDir(createDirName("/HTTTrackHist_2nd/BadChi2TrackHist/TotalTrackHist/"));
    			break;
    	}

    	if (h_nTracks[n]) 						ATH_CHECK(regHist(getHistDir(), h_nTracks[n]));
    	if (h_nTracks_wide[n]) 					ATH_CHECK(regHist(getHistDir(), h_nTracks_wide[n]));
	    if (h_nTracks_per_patt[n]) 				ATH_CHECK(regHist(getHistDir(), h_nTracks_per_patt[n]));
	    if (h_nConstants[n]) 					ATH_CHECK(regHist(getHistDir(), h_nConstants[n]));
	    if (h_Track_pt[n]) 						ATH_CHECK(regHist(getHistDir(), h_Track_pt[n]));
	    if (h_Track_eta[n]) 					ATH_CHECK(regHist(getHistDir(), h_Track_eta[n]));
	    if (h_Track_phi[n]) 					ATH_CHECK(regHist(getHistDir(), h_Track_phi[n]));
	    if (h_Track_d0[n]) 						ATH_CHECK(regHist(getHistDir(), h_Track_d0[n]));
	    if (h_Track_z0[n]) 						ATH_CHECK(regHist(getHistDir(), h_Track_z0[n]));
	    if (h_Track_chi2[n]) 					ATH_CHECK(regHist(getHistDir(), h_Track_chi2[n]));
	    if (h_Track_chi2ndof[n]) 				ATH_CHECK(regHist(getHistDir(), h_Track_chi2ndof[n]));
	    if (h_Track_nCoords[n]) 				ATH_CHECK(regHist(getHistDir(), h_Track_nCoords[n]));
	    if (h_Track_sectorID[n]) 				ATH_CHECK(regHist(getHistDir(), h_Track_sectorID[n]));
	    if (h_Track_sectorID_chi2[n]) 			ATH_CHECK(regHist(getHistDir(), h_Track_sectorID_chi2[n]));
	    if (h_Track_sectorID_chi2ndof[n]) 		ATH_CHECK(regHist(getHistDir(), h_Track_sectorID_chi2ndof[n]));
	    if (h_Track_nmissing[n]) 				ATH_CHECK(regHist(getHistDir(), h_Track_nmissing[n]));
	    if (h_Track_chi2PerMissingHit[n]) 		ATH_CHECK(regHist(getHistDir(), h_Track_chi2PerMissingHit[n]));
	    if (h_Track_chi2ndofPerMissingHit[n]) 	ATH_CHECK(regHist(getHistDir(), h_Track_chi2ndofPerMissingHit[n]));
	    if (h_Track_hitType[n]) 				ATH_CHECK(regHist(getHistDir(), h_Track_hitType[n]));
	    if (h_Track_Chi2PerMissingLayer[n]) 	ATH_CHECK(regHist(getHistDir(), h_Track_Chi2PerMissingLayer[n]));


	    switch (n) {
    		case HistType_all_1st:
    			setHistDir(createDirName("/HTTTrackHist_1st/AllTrackHist/BestTrackHist/"));
    			break;
    		case HistType_afterOR_1st:
    			setHistDir(createDirName("/HTTTrackHist_1st/ORTrackHist/BestTrackHist/"));
    			break;
    		case HistType_all_2nd:
    			setHistDir(createDirName("/HTTTrackHist_2nd/AllTrackHist/BestTrackHist/"));
    			break;
    		case HistType_afterOR_2nd:
    			setHistDir(createDirName("/HTTTrackHist_2nd/ORTrackHist/BestTrackHist/"));
    			break;
    	}

    	if (h_Track_best_pt[n]) 					ATH_CHECK(regHist(getHistDir(), h_Track_best_pt[n]));
	    if (h_Track_best_eta[n]) 					ATH_CHECK(regHist(getHistDir(), h_Track_best_eta[n]));
	    if (h_Track_best_phi[n]) 					ATH_CHECK(regHist(getHistDir(), h_Track_best_phi[n]));
	    if (h_Track_best_d0[n]) 					ATH_CHECK(regHist(getHistDir(), h_Track_best_d0[n]));
	    if (h_Track_best_z0[n]) 					ATH_CHECK(regHist(getHistDir(), h_Track_best_z0[n]));
	    if (h_Track_best_chi2[n]) 					ATH_CHECK(regHist(getHistDir(), h_Track_best_chi2[n]));
	    if (h_Track_best_chi2ndof[n]) 				ATH_CHECK(regHist(getHistDir(), h_Track_best_chi2ndof[n]));
	    if (h_Track_best_nCoords[n]) 				ATH_CHECK(regHist(getHistDir(), h_Track_best_nCoords[n]));
	    if (h_Track_best_sectorID[n]) 				ATH_CHECK(regHist(getHistDir(), h_Track_best_sectorID[n]));
	    if (h_Track_best_sectorID_chi2[n]) 			ATH_CHECK(regHist(getHistDir(), h_Track_best_sectorID_chi2[n]));
	    if (h_Track_best_sectorID_chi2ndof[n]) 		ATH_CHECK(regHist(getHistDir(), h_Track_best_sectorID_chi2ndof[n]));
	    if (h_Track_best_nmissing[n]) 				ATH_CHECK(regHist(getHistDir(), h_Track_best_nmissing[n]));
	    if (h_Track_best_chi2PerMissingHit[n]) 		ATH_CHECK(regHist(getHistDir(), h_Track_best_chi2PerMissingHit[n]));
	    if (h_Track_best_chi2ndofPerMissingHit[n]) 	ATH_CHECK(regHist(getHistDir(), h_Track_best_chi2ndofPerMissingHit[n]));


	    switch (n) {
    		case HistType_all_1st:
    			setHistDir(createDirName("/HTTTrackHist_1st/AllTrackHist/ResHist/"));
    			break;
    		case HistType_afterOR_1st:
    			setHistDir(createDirName("/HTTTrackHist_1st/ORTrackHist/ResHist/"));
    			break;
    		case HistType_all_2nd:
    			setHistDir(createDirName("/HTTTrackHist_2nd/AllTrackHist/ResHist/"));
    			break;
    		case HistType_afterOR_2nd:
    			setHistDir(createDirName("/HTTTrackHist_2nd/ORTrackHist/ResHist/"));
    			break;
    	}

    	if (h_HTT_vs_Truth_Res_no_truthmatch_qoverpt[n]) 	ATH_CHECK(regHist(getHistDir(), h_HTT_vs_Truth_Res_no_truthmatch_qoverpt[n]));
	    if (h_HTT_vs_Truth_Res_no_truthmatch_pt[n]) 		ATH_CHECK(regHist(getHistDir(), h_HTT_vs_Truth_Res_no_truthmatch_pt[n]));
	    if (h_HTT_vs_Truth_Res_no_truthmatch_eta[n]) 		ATH_CHECK(regHist(getHistDir(), h_HTT_vs_Truth_Res_no_truthmatch_eta[n]));
	    if (h_HTT_vs_Truth_Res_no_truthmatch_phi[n]) 		ATH_CHECK(regHist(getHistDir(), h_HTT_vs_Truth_Res_no_truthmatch_phi[n]));
	    if (h_HTT_vs_Truth_Res_no_truthmatch_d0[n]) 		ATH_CHECK(regHist(getHistDir(), h_HTT_vs_Truth_Res_no_truthmatch_d0[n]));
	    if (h_HTT_vs_Truth_Res_no_truthmatch_z0[n]) 		ATH_CHECK(regHist(getHistDir(), h_HTT_vs_Truth_Res_no_truthmatch_z0[n]));
	    if (h_HTT_vs_Truth_Res_no_truthmatch_z0_wide[n]) 	ATH_CHECK(regHist(getHistDir(), h_HTT_vs_Truth_Res_no_truthmatch_z0_wide[n]));


	    clearHistDir();
    }

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTTrackMonitorTool::bookGuessedTrackHistograms_1st()
{
    setHistDir(createDirName("/GuessedTrackHist_1st/"));

    h_chi2pdf_1st_8outof8 = new TH1F("h_chi2pdf_1st_8outof8", "h_chi2pdf_1st_8outof8", 100, 0., 100); ATH_CHECK(regHist(getHistDir(), h_chi2pdf_1st_8outof8));

    for (unsigned i = 0; i < m_nLayers_1st; i++)  {
        h_chi2pdf_1st_guessed[i] = new TH1F(Form("h_chi2pdf_1st_guessed_%d",i), Form("h_chi2pdf_1st_guessed_%d",i), 100, 0., 100); ATH_CHECK(regHist(getHistDir(), h_chi2pdf_1st_guessed[i]));
        h_dx_1st_guessed[i] = new TH1F(Form("h_dx_1st_guessed_%d",i), Form("h_dx_1st_guessed_%d",i), 200, -1000., 1000); ATH_CHECK(regHist(getHistDir(), h_dx_1st_guessed[i]));
        h_dy_1st_guessed[i] = new TH1F(Form("h_dy_1st_guessed_%d",i), Form("h_dy_1st_guessed_%d",i), 200, -1000., 1000); ATH_CHECK(regHist(getHistDir(), h_dy_1st_guessed[i]));
    }

    clearHistDir();

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTTrackMonitorTool::bookGuessedTrackHistograms_2nd()
{
    setHistDir(createDirName("/GuessedTrackHist_2nd/"));

    h_chi2pdf_2nd_13outof13 = new TH1F("h_chi2pdf_2nd_13outof13", "h_chi2pdf_2nd_13outof13", 100, 0., 100); ATH_CHECK(regHist(getHistDir(), h_chi2pdf_2nd_13outof13));

    for (unsigned i = 0; i < m_nLayers_2nd; i++)  {
        h_chi2pdf_2nd_guessed[i] = new TH1F(Form("h_chi2pdf_2nd_guessed_%d",i), Form("h_chi2pdf_2nd_guessed_%d",i), 100, 0., 100); ATH_CHECK(regHist(getHistDir(), h_chi2pdf_2nd_guessed[i]));
        h_dx_2nd_guessed[i] = new TH1F(Form("h_dx_2nd_guessed_%d",i), Form("h_dx_2nd_guessed_%d",i), 200, -1000., 1000); ATH_CHECK(regHist(getHistDir(), h_dx_2nd_guessed[i]));
        h_dy_2nd_guessed[i] = new TH1F(Form("h_dy_2nd_guessed_%d",i), Form("h_dy_2nd_guessed_%d",i), 200, -1000., 1000); ATH_CHECK(regHist(getHistDir(), h_dy_2nd_guessed[i]));
    }

    clearHistDir();

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void HTTTrackMonitorTool::fillHTTTrackHistograms(std::vector<HTTTrack> const * tracks, bool isFirstStage)
{
    if (!tracks) {
    	if (isFirstStage)
        	ATH_MSG_WARNING("Failed to retrieve first stage tracks.");
        else
        	ATH_MSG_WARNING("Failed to retrieve second stage tracks.");
        return;
    }

    unsigned histTypeOffset = isFirstStage ? 0 : (NHISTTYPES_2nd - NHISTTYPES_1st);

    h_nTracks[HistType_all_1st + histTypeOffset]->Fill(tracks->size());
    h_nTracks_wide[HistType_all_1st + histTypeOffset]->Fill(tracks->size());

    size_t nTracks_passChi2 = 0;
    size_t nTracks_afterOR  = 0;
    size_t nTracks_badChi2  = 0;

    std::unordered_map<int, size_t> nTracks_per_patt;
    std::unordered_map<int, size_t> nTracks_per_patt_passChi2;
    std::unordered_map<int, size_t> nTracks_per_patt_afterOR;

    std::unordered_map<int, size_t> nTracks_per_sect;
    std::unordered_map<int, size_t> nTracks_per_sect_passChi2;
    std::unordered_map<int, size_t> nTracks_per_sect_afterOR;

    HTTTrack const * best         = nullptr;
    HTTTrack const * best_afterOR = nullptr;

    int bestchi2ndof          = std::numeric_limits<int>::max();
    int bestchi2ndof_afterOR  = std::numeric_limits<int>::max();

    // Fill track quantities for all tracks found
    for (HTTTrack const & t : *tracks) {
        nTracks_per_patt[t.getPatternID()]++;
        if (isFirstStage)
        	nTracks_per_sect[t.getFirstSectorID()]++;
        else
        	nTracks_per_sect[t.getSecondSectorID()]++;
    
    	h_Track_pt[HistType_all_1st + histTypeOffset]->Fill(t.getPt()*0.001);
        h_Track_eta[HistType_all_1st + histTypeOffset]->Fill(t.getEta());
        h_Track_phi[HistType_all_1st + histTypeOffset]->Fill(t.getPhi());
        h_Track_d0[HistType_all_1st + histTypeOffset]->Fill(t.getD0());
        h_Track_z0[HistType_all_1st + histTypeOffset]->Fill(t.getZ0());

        h_Track_chi2[HistType_all_1st + histTypeOffset]->Fill(t.getChi2());
        h_Track_chi2ndof[HistType_all_1st + histTypeOffset]->Fill(t.getChi2ndof());
        h_Track_nCoords[HistType_all_1st + histTypeOffset]->Fill(t.getNCoords() - t.getNMissing());
        if (isFirstStage) {
        	h_Track_sectorID[HistType_all_1st + histTypeOffset]->Fill(t.getFirstSectorID());
	        h_Track_sectorID_chi2[HistType_all_1st + histTypeOffset]->Fill(t.getFirstSectorID(), t.getChi2());
	        h_Track_sectorID_chi2ndof[HistType_all_1st + histTypeOffset]->Fill(t.getFirstSectorID(), t.getChi2ndof());
        }
        else {
        	h_Track_sectorID[HistType_all_1st + histTypeOffset]->Fill(t.getSecondSectorID());
	        h_Track_sectorID_chi2[HistType_all_1st + histTypeOffset]->Fill(t.getSecondSectorID(), t.getChi2());
	        h_Track_sectorID_chi2ndof[HistType_all_1st + histTypeOffset]->Fill(t.getSecondSectorID(), t.getChi2ndof());
        }
        h_Track_nmissing[HistType_all_1st + histTypeOffset]->Fill(t.getNMissing());
        h_Track_chi2PerMissingHit[HistType_all_1st + histTypeOffset]->Fill(t.getNMissing(), t.getChi2());
        h_Track_chi2ndofPerMissingHit[HistType_all_1st + histTypeOffset]->Fill(t.getNMissing(), t.getChi2ndof());

        // calcualte missing hits
        u_int nmiss = 0;
        int layer = 0;
        for (auto & hit: t.getHTTHits()) {
            HitType type = hit.getHitType();
            h_Track_hitType[HistType_all_1st + histTypeOffset]->Fill(static_cast<int>(type));
            if (type == HitType::guessed  || type == HitType::wildcard) {
                nmiss |= 1<<layer; 
            }
            layer++;
        }
        h_Track_Chi2PerMissingLayer[HistType_all_1st + histTypeOffset]->Fill(nmiss, t.getChi2ndof());

        if (t.getChi2ndof() < bestchi2ndof) {
            bestchi2ndof = t.getChi2ndof();
            best = &t;
        }

        if (t.getChi2ndof() <= m_cut_chi2ndof) {
            nTracks_passChi2++;
            nTracks_per_patt_passChi2[t.getPatternID()]++;
            nTracks_per_sect_passChi2[t.getFirstSectorID()]++;

            h_Track_pt[HistType_passChi2_1st + histTypeOffset]->Fill(t.getPt()*0.001);
            h_Track_eta[HistType_passChi2_1st + histTypeOffset]->Fill(t.getEta());
            h_Track_phi[HistType_passChi2_1st + histTypeOffset]->Fill(t.getPhi());
            h_Track_d0[HistType_passChi2_1st + histTypeOffset]->Fill(t.getD0());
            h_Track_z0[HistType_passChi2_1st + histTypeOffset]->Fill(t.getZ0());
            h_Track_chi2[HistType_passChi2_1st + histTypeOffset]->Fill(t.getChi2());
            h_Track_chi2ndof[HistType_passChi2_1st + histTypeOffset]->Fill(t.getChi2ndof());
            h_Track_nCoords[HistType_passChi2_1st + histTypeOffset]->Fill(t.getNCoords()-t.getNMissing());
            if (isFirstStage) {
            	h_Track_sectorID[HistType_passChi2_1st + histTypeOffset]->Fill(t.getFirstSectorID());
	            h_Track_sectorID_chi2[HistType_passChi2_1st + histTypeOffset]->Fill(t.getFirstSectorID(),t.getChi2());
	            h_Track_sectorID_chi2ndof[HistType_passChi2_1st + histTypeOffset]->Fill(t.getFirstSectorID(),t.getChi2ndof());
            }
            else {
            	h_Track_sectorID[HistType_passChi2_1st + histTypeOffset]->Fill(t.getSecondSectorID());
	            h_Track_sectorID_chi2[HistType_passChi2_1st + histTypeOffset]->Fill(t.getSecondSectorID(),t.getChi2());
	            h_Track_sectorID_chi2ndof[HistType_passChi2_1st + histTypeOffset]->Fill(t.getSecondSectorID(),t.getChi2ndof());
            }
            h_Track_nmissing[HistType_passChi2_1st + histTypeOffset]->Fill(t.getNMissing());
            h_Track_chi2PerMissingHit[HistType_passChi2_1st + histTypeOffset]->Fill(t.getNMissing(), t.getChi2());
            h_Track_chi2ndofPerMissingHit[HistType_passChi2_1st + histTypeOffset]->Fill(t.getNMissing(), t.getChi2ndof());

            for (auto & hit: t.getHTTHits()) {
                h_Track_hitType[HistType_passChi2_1st + histTypeOffset]->Fill(static_cast<int>(hit.getHitType()));
            }
            h_Track_Chi2PerMissingLayer[HistType_passChi2_1st + histTypeOffset]->Fill(nmiss, t.getChi2ndof());
        }

        if (t.passedOR() == 1) {
            nTracks_afterOR++;
            nTracks_per_patt_afterOR[t.getPatternID()]++;
            nTracks_per_sect_afterOR[t.getFirstSectorID()]++;

            h_Track_pt[HistType_afterOR_1st + histTypeOffset]->Fill(t.getPt()*0.001);
            h_Track_eta[HistType_afterOR_1st + histTypeOffset]->Fill(t.getEta());
            h_Track_phi[HistType_afterOR_1st + histTypeOffset]->Fill(t.getPhi());
            h_Track_d0[HistType_afterOR_1st + histTypeOffset]->Fill(t.getD0());
            h_Track_z0[HistType_afterOR_1st + histTypeOffset]->Fill(t.getZ0());
            h_Track_chi2[HistType_afterOR_1st + histTypeOffset]->Fill(t.getChi2());
            h_Track_chi2ndof[HistType_afterOR_1st + histTypeOffset]->Fill(t.getChi2ndof());
            h_Track_nCoords[HistType_afterOR_1st + histTypeOffset]->Fill(t.getNCoords()-t.getNMissing());
            if (isFirstStage) {
            	h_Track_sectorID[HistType_afterOR_1st + histTypeOffset]->Fill(t.getFirstSectorID());
	            h_Track_sectorID_chi2[HistType_afterOR_1st + histTypeOffset]->Fill(t.getFirstSectorID(),t.getChi2());
	            h_Track_sectorID_chi2ndof[HistType_afterOR_1st + histTypeOffset]->Fill(t.getFirstSectorID(),t.getChi2ndof());
            }
            else {
            	h_Track_sectorID[HistType_afterOR_1st + histTypeOffset]->Fill(t.getSecondSectorID());
	            h_Track_sectorID_chi2[HistType_afterOR_1st + histTypeOffset]->Fill(t.getSecondSectorID(),t.getChi2());
	            h_Track_sectorID_chi2ndof[HistType_afterOR_1st + histTypeOffset]->Fill(t.getSecondSectorID(),t.getChi2ndof());
            }
            h_Track_nmissing[HistType_afterOR_1st + histTypeOffset]->Fill(t.getNMissing());
            h_Track_chi2PerMissingHit[HistType_afterOR_1st + histTypeOffset]->Fill(t.getNMissing(), t.getChi2());
            h_Track_chi2ndofPerMissingHit[HistType_afterOR_1st + histTypeOffset]->Fill(t.getNMissing(), t.getChi2ndof());

            for (auto & hit: t.getHTTHits()) {
                h_Track_hitType[HistType_afterOR_1st + histTypeOffset]->Fill(static_cast<int>(hit.getHitType()));
            }
            h_Track_Chi2PerMissingLayer[HistType_afterOR_1st + histTypeOffset]->Fill(nmiss, t.getChi2ndof());

            if (t.getChi2ndof() < bestchi2ndof_afterOR) {
                bestchi2ndof_afterOR = t.getChi2ndof();
                best_afterOR = &t;
            }
        }

        if (t.getChi2ndof() > m_cut_chi2ndof) {
            nTracks_badChi2++;

            h_Track_pt[HistType_badChi2_1st + histTypeOffset]->Fill(t.getPt()*0.001);
            h_Track_eta[HistType_badChi2_1st + histTypeOffset]->Fill(t.getEta());
            h_Track_phi[HistType_badChi2_1st + histTypeOffset]->Fill(t.getPhi());
            h_Track_d0[HistType_badChi2_1st + histTypeOffset]->Fill(t.getD0());
            h_Track_z0[HistType_badChi2_1st + histTypeOffset]->Fill(t.getZ0());
            h_Track_chi2[HistType_badChi2_1st + histTypeOffset]->Fill(t.getChi2());
            h_Track_chi2ndof[HistType_badChi2_1st + histTypeOffset]->Fill(t.getChi2ndof());
            h_Track_nCoords[HistType_badChi2_1st + histTypeOffset]->Fill(t.getNCoords()-t.getNMissing());
            if (isFirstStage) {
            	h_Track_sectorID[HistType_badChi2_1st + histTypeOffset]->Fill(t.getFirstSectorID());
	            h_Track_sectorID_chi2[HistType_badChi2_1st + histTypeOffset]->Fill(t.getFirstSectorID(),t.getChi2());
	            h_Track_sectorID_chi2ndof[HistType_badChi2_1st + histTypeOffset]->Fill(t.getFirstSectorID(),t.getChi2ndof());
            }
            else {
            	h_Track_sectorID[HistType_badChi2_1st + histTypeOffset]->Fill(t.getSecondSectorID());
	            h_Track_sectorID_chi2[HistType_badChi2_1st + histTypeOffset]->Fill(t.getSecondSectorID(),t.getChi2());
	            h_Track_sectorID_chi2ndof[HistType_badChi2_1st + histTypeOffset]->Fill(t.getSecondSectorID(),t.getChi2ndof());
            }
            h_Track_nmissing[HistType_badChi2_1st + histTypeOffset]->Fill(t.getNMissing());
            h_Track_chi2PerMissingHit[HistType_badChi2_1st + histTypeOffset]->Fill(t.getNMissing(), t.getChi2());
            h_Track_chi2ndofPerMissingHit[HistType_badChi2_1st + histTypeOffset]->Fill(t.getNMissing(), t.getChi2ndof());

            for (auto & hit: t.getHTTHits()) {
                h_Track_hitType[HistType_badChi2_1st + histTypeOffset]->Fill(static_cast<int>(hit.getHitType()));
            }
            h_Track_Chi2PerMissingLayer[HistType_badChi2_1st + histTypeOffset]->Fill(nmiss, t.getChi2ndof());
        }
    }

    h_nTracks[HistType_passChi2_1st + histTypeOffset]->Fill(nTracks_passChi2);
    h_nTracks[HistType_afterOR_1st + histTypeOffset]->Fill(nTracks_afterOR);
    h_nTracks[HistType_badChi2_1st + histTypeOffset]->Fill(nTracks_badChi2);

    h_nConstants[HistType_all_1st + histTypeOffset]->Fill(nTracks_per_sect.size());
    h_nConstants[HistType_passChi2_1st + histTypeOffset]->Fill(nTracks_per_sect_passChi2.size());
    h_nConstants[HistType_afterOR_1st + histTypeOffset]->Fill(nTracks_per_sect_afterOR.size());

    for (std::pair<int, size_t> p : nTracks_per_patt) {
        h_nTracks_per_patt[HistType_all_1st + histTypeOffset]->Fill(p.second);
    }

    for (std::pair<int, size_t> p : nTracks_per_patt_passChi2) {
        h_nTracks_per_patt[HistType_passChi2_1st + histTypeOffset]->Fill(p.second);
    }

    for (std::pair<int, size_t> p : nTracks_per_patt_afterOR) {
        h_nTracks_per_patt[HistType_afterOR_1st + histTypeOffset]->Fill(p.second);
    }

    std::vector<HTTTruthTrack> const & truthtracks = m_logicEventInputHeader_1st->optional().getTruthTracks();

    if (best) {
        h_Track_best_pt[HistType_all_1st + histTypeOffset]->Fill(best->getPt()*0.001);
        h_Track_best_eta[HistType_all_1st + histTypeOffset]->Fill(best->getEta());
        h_Track_best_phi[HistType_all_1st + histTypeOffset]->Fill(best->getPhi());
        h_Track_best_d0[HistType_all_1st + histTypeOffset]->Fill(best->getD0());
        h_Track_best_z0[HistType_all_1st + histTypeOffset]->Fill(best->getZ0());
        h_Track_best_chi2[HistType_all_1st + histTypeOffset]->Fill(best->getChi2());
        h_Track_best_chi2ndof[HistType_all_1st + histTypeOffset]->Fill(best->getChi2ndof());
        h_Track_best_nCoords[HistType_all_1st + histTypeOffset]->Fill(best->getNCoords() - best->getNMissing());
        if (isFirstStage) {
        	h_Track_best_sectorID[HistType_all_1st + histTypeOffset]->Fill(best->getFirstSectorID());
	        h_Track_best_sectorID_chi2[HistType_all_1st + histTypeOffset]->Fill(best->getFirstSectorID(), best->getChi2());
	        h_Track_best_sectorID_chi2ndof[HistType_all_1st + histTypeOffset]->Fill(best->getFirstSectorID(), best->getChi2ndof());
        }
        else {
        	h_Track_best_sectorID[HistType_all_1st + histTypeOffset]->Fill(best->getSecondSectorID());
	        h_Track_best_sectorID_chi2[HistType_all_1st + histTypeOffset]->Fill(best->getSecondSectorID(), best->getChi2());
	        h_Track_best_sectorID_chi2ndof[HistType_all_1st + histTypeOffset]->Fill(best->getSecondSectorID(), best->getChi2ndof());
        }
        h_Track_best_nmissing[HistType_all_1st + histTypeOffset]->Fill(best->getNMissing());
        h_Track_best_chi2PerMissingHit[HistType_all_1st + histTypeOffset]->Fill(best->getNMissing(), best->getChi2());
        h_Track_best_chi2ndofPerMissingHit[HistType_all_1st + histTypeOffset]->Fill(best->getNMissing(), best->getChi2ndof());

        for (HTTTruthTrack const & truth_t : truthtracks) {
            h_HTT_vs_Truth_Res_no_truthmatch_qoverpt[HistType_all_1st + histTypeOffset]->Fill((abs(best->getQOverPt()) - abs(truth_t.getQOverPt())) * 1000.);
            h_HTT_vs_Truth_Res_no_truthmatch_pt[HistType_all_1st + histTypeOffset]->Fill((best->getPt() - truth_t.getPt()) * 0.001);
            h_HTT_vs_Truth_Res_no_truthmatch_eta[HistType_all_1st + histTypeOffset]->Fill(best->getEta() - truth_t.getEta());
            h_HTT_vs_Truth_Res_no_truthmatch_phi[HistType_all_1st + histTypeOffset]->Fill(best->getPhi() - truth_t.getPhi());
            h_HTT_vs_Truth_Res_no_truthmatch_d0[HistType_all_1st + histTypeOffset]->Fill(best->getD0() - truth_t.getD0());
            h_HTT_vs_Truth_Res_no_truthmatch_z0[HistType_all_1st + histTypeOffset]->Fill(best->getZ0() - truth_t.getZ0());
            h_HTT_vs_Truth_Res_no_truthmatch_z0_wide[HistType_all_1st + histTypeOffset]->Fill(best->getZ0() - truth_t.getZ0());
        }
    }

    if (best_afterOR) {
        h_Track_best_pt[HistType_afterOR_1st + histTypeOffset]->Fill(best_afterOR->getPt()*0.001);
        h_Track_best_eta[HistType_afterOR_1st + histTypeOffset]->Fill(best_afterOR->getEta());
        h_Track_best_phi[HistType_afterOR_1st + histTypeOffset]->Fill(best_afterOR->getPhi());
        h_Track_best_d0[HistType_afterOR_1st + histTypeOffset]->Fill(best_afterOR->getD0());
        h_Track_best_z0[HistType_afterOR_1st + histTypeOffset]->Fill(best_afterOR->getZ0());
        h_Track_best_chi2[HistType_afterOR_1st + histTypeOffset]->Fill(best_afterOR->getChi2());
        h_Track_best_chi2ndof[HistType_afterOR_1st + histTypeOffset]->Fill(best_afterOR->getChi2ndof());
        h_Track_best_nCoords[HistType_afterOR_1st + histTypeOffset]->Fill(best_afterOR->getNCoords() - best_afterOR->getNMissing());
        if (isFirstStage) {
        	h_Track_best_sectorID[HistType_afterOR_1st + histTypeOffset]->Fill(best_afterOR->getFirstSectorID());
	        h_Track_best_sectorID_chi2[HistType_afterOR_1st + histTypeOffset]->Fill(best_afterOR->getFirstSectorID(), best_afterOR->getChi2());
	        h_Track_best_sectorID_chi2ndof[HistType_afterOR_1st + histTypeOffset]->Fill(best_afterOR->getFirstSectorID(), best_afterOR->getChi2ndof());
        }
        else {
        	h_Track_best_sectorID[HistType_afterOR_1st + histTypeOffset]->Fill(best_afterOR->getSecondSectorID());
	        h_Track_best_sectorID_chi2[HistType_afterOR_1st + histTypeOffset]->Fill(best_afterOR->getSecondSectorID(), best_afterOR->getChi2());
	        h_Track_best_sectorID_chi2ndof[HistType_afterOR_1st + histTypeOffset]->Fill(best_afterOR->getSecondSectorID(), best_afterOR->getChi2ndof());
        }
        h_Track_best_nmissing[HistType_afterOR_1st + histTypeOffset]->Fill(best_afterOR->getNMissing());
        h_Track_best_chi2PerMissingHit[HistType_afterOR_1st + histTypeOffset]->Fill(best_afterOR->getNMissing(), best_afterOR->getChi2());
        h_Track_best_chi2ndofPerMissingHit[HistType_afterOR_1st + histTypeOffset]->Fill(best_afterOR->getNMissing(), best_afterOR->getChi2ndof());

        for (HTTTruthTrack const & truth_t : truthtracks) {
            h_HTT_vs_Truth_Res_no_truthmatch_qoverpt[HistType_afterOR_1st + histTypeOffset]->Fill((abs(best_afterOR->getQOverPt()) - abs(truth_t.getQOverPt())) * 1000.);
            h_HTT_vs_Truth_Res_no_truthmatch_pt[HistType_afterOR_1st + histTypeOffset]->Fill((best_afterOR->getPt() - truth_t.getPt()) * 0.001);
            h_HTT_vs_Truth_Res_no_truthmatch_eta[HistType_afterOR_1st + histTypeOffset]->Fill(best_afterOR->getEta() - truth_t.getEta());
            h_HTT_vs_Truth_Res_no_truthmatch_phi[HistType_afterOR_1st + histTypeOffset]->Fill(best_afterOR->getPhi() - truth_t.getPhi());
            h_HTT_vs_Truth_Res_no_truthmatch_d0[HistType_afterOR_1st + histTypeOffset]->Fill(best_afterOR->getD0() - truth_t.getD0());
            h_HTT_vs_Truth_Res_no_truthmatch_z0[HistType_afterOR_1st + histTypeOffset]->Fill(best_afterOR->getZ0() - truth_t.getZ0());
            h_HTT_vs_Truth_Res_no_truthmatch_z0_wide[HistType_afterOR_1st + histTypeOffset]->Fill(best_afterOR->getZ0() - truth_t.getZ0());
        }
    }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void HTTTrackMonitorTool::fillGuessedTrackHistograms(std::vector<HTTTrack> const * tracks_guessed, std::vector<HTTTrack> const * tracks_nomiss, bool isFirstStage)
{
    if (!tracks_guessed || !tracks_nomiss) {
    	if (isFirstStage)
        	ATH_MSG_WARNING("Failed to retrieve first stage guessed/nomiss tracks.");
        else
        	ATH_MSG_WARNING("Failed to retrieve second stage guessed/nomiss tracks.");
        return;
    }

    for (HTTTrack const & t : *tracks_guessed) {
        unsigned int bitmask = t.getHitMap();
        int missing = -1;
        for (int i = 0; i < t.getNCoords(); i++) {
            if (((bitmask >> i) & 0x1) == 0) {
                missing = i;
                break;
            }
        }
        if (missing == -1) ATH_MSG_WARNING("Supposed to have a missing hit, but did not find one!");
        if (missing > 1) missing = missing - 1; // kludge to account for the pixel, which is the first two coordinates

        int patternID = t.getPatternID();

        for (HTTTrack const & t_nomiss : *tracks_nomiss) {
            if (t_nomiss.getPatternID() != patternID) continue;
            // potential match, check that other hits are in common
            unsigned nmatch(0);
            int ihit = 0;
            for (const auto& nomiss_hit : t_nomiss.getHTTHits()) {
                if (missing == ihit) continue; // don't look at missing hits
                if (t.getHTTHits().at(ihit).getHTTIdentifierHash() != nomiss_hit.getHTTIdentifierHash()) continue; // different modules
                if (abs(t.getHTTHits().at(ihit).getEtaCoord() - nomiss_hit.getEtaCoord()) > 1e-8) continue; // positions are different
                if (abs(t.getHTTHits().at(ihit).getPhiCoord() - nomiss_hit.getPhiCoord()) > 1e-8) continue;
                nmatch++;
                ihit++;
            }
            if (isFirstStage) {
            	if (nmatch == m_nLayers_1st) {
	                h_chi2pdf_1st_8outof8->Fill(t_nomiss.getChi2());
	                h_chi2pdf_1st_guessed[missing]->Fill(t.getChi2ndof());
	                h_dx_1st_guessed[missing]->Fill(t.getHTTHits().at(missing).getEtaCoord() - t_nomiss.getHTTHits().at(missing).getEtaCoord());
	                if (missing == 0) h_dy_1st_guessed[missing]->Fill(t.getHTTHits().at(missing).getPhiCoord() - t_nomiss.getHTTHits().at(missing).getPhiCoord());
	                continue; // we found a match, can stop
	            }
            }
            else {
            	if (nmatch == m_nLayers_2nd) {
	                h_chi2pdf_2nd_13outof13->Fill(t_nomiss.getChi2());
	                h_chi2pdf_2nd_guessed[missing]->Fill(t.getChi2ndof());
	                h_dx_2nd_guessed[missing]->Fill(t.getHTTHits().at(missing).getEtaCoord() - t_nomiss.getHTTHits().at(missing).getEtaCoord());
	                if (missing == 0) h_dy_2nd_guessed[missing]->Fill(t.getHTTHits().at(missing).getPhiCoord() - t_nomiss.getHTTHits().at(missing).getPhiCoord());
	                continue; // we found a match, can stop
	            }
            }
        }
    }
}

