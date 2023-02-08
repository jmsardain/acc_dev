/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTMonitor/HTTPerformanceMonitorTool.h"

#include "TrigHTTObjects/HTTLogicalEventInputHeader.h"
#include "TrigHTTObjects/HTTLogicalEventOutputHeader.h"


static const InterfaceID IID_HTTPerformanceMonitorTool("HTTPerformanceMonitorTool", 1, 0);
const InterfaceID& HTTPerformanceMonitorTool::interfaceID()
{ return IID_HTTPerformanceMonitorTool; }


/////////////////////////////////////////////////////////////////////////////
HTTPerformanceMonitorTool::HTTPerformanceMonitorTool(std::string const & algname, std::string const & name, IInterface const * ifc) :
    HTTMonitorBase(algname,name,ifc)
{
    declareInterface<HTTPerformanceMonitorTool>(this);
    declareProperty("fastMon",              m_fast,                 "only do fast monitoring");
    declareProperty("RunSecondStage",       m_runSecondStage,       "flag to enable running the second stage fitting");
    declareProperty("BarcodeFracCut",       m_cut_barcodefrac,      "cut on barcode fraction used in truth matching");
    declareProperty("Chi2ndofCut",          m_cut_chi2ndof,         "cut on Chi2 of HTTTrack");
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTPerformanceMonitorTool::initialize()
{
    ATH_CHECK(HTTMonitorBase::initialize());
    ATH_CHECK(bookHistograms());

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTPerformanceMonitorTool::bookHistograms()
{
    if (m_fast) return StatusCode::SUCCESS;

    if (!m_withPU) {
        ATH_CHECK(bookCrudeEffHistograms_1st());
        if (m_runSecondStage) ATH_CHECK(bookCrudeEffHistograms_2nd());
    }
    ATH_CHECK(bookTruthMatchHistograms_1st());
    if (m_runSecondStage) ATH_CHECK(bookTruthMatchHistograms_2nd());

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTPerformanceMonitorTool::fillHistograms()
{
    // Use event selection to pass or reject event based on truth info, sample type etc
    ATH_CHECK(selectEvent());

    if (m_fast) return StatusCode::SUCCESS;

    m_nEvents++;

    std::vector<HTTRoad*> const * roads_1st = nullptr;
    std::vector<HTTRoad*> const * roads_2nd = nullptr;
    std::vector<HTTTrack> const * tracks_1st = nullptr;
    std::vector<HTTTrack> const * tracks_2nd = nullptr;

    std::vector<HTTRoad*> proads_1st, proads_2nd;

    if (!m_logicEventOutputHeader) {
        roads_1st = getMonitorRoads("Roads_1st");
        tracks_1st = getMonitorTracks("Tracks_1st");
        if (m_runSecondStage) {
            roads_2nd = getMonitorRoads("Roads_2nd");
            tracks_2nd = getMonitorTracks("Tracks_2nd");
        }
    }
    else {
        m_logicEventOutputHeader->getHTTRoads_1st(proads_1st);
        roads_1st = &proads_1st;
        tracks_1st = &(m_logicEventOutputHeader->getHTTTracks_1st());
        if (m_runSecondStage) {
            m_logicEventOutputHeader->getHTTRoads_2nd(proads_2nd);
            roads_2nd = &proads_2nd;
            tracks_2nd = &(m_logicEventOutputHeader->getHTTTracks_2nd());
        }
    }

    if (!m_withPU) {
        fillCrudeEffHistograms_1st(roads_1st, tracks_1st);
        if (m_runSecondStage) fillCrudeEffHistograms_2nd(roads_2nd, tracks_2nd);
    }

    fillHTTRoadTruthMatchHistograms_1st(roads_1st);
    if (m_runSecondStage) fillHTTRoadTruthMatchHistograms_2nd(roads_2nd);

    fillHTTTrackTruthMatchHistograms_1st(tracks_1st);
    if (m_runSecondStage) fillHTTTrackTruthMatchHistograms_2nd(tracks_2nd);

    clearMonitorData();

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTPerformanceMonitorTool::makeEffHistograms()
{
    if (m_fast) return StatusCode::SUCCESS;

    if (!m_withPU) {
      makeCrudeEffHistograms_1st();
      if (m_runSecondStage) makeCrudeEffHistograms_2nd();
    }

    makeTruthMatchEffHistograms_1st();
    if (m_runSecondStage) makeTruthMatchEffHistograms_2nd();

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTPerformanceMonitorTool::finalize()
{
    ATH_CHECK(makeEffHistograms());
    ATH_CHECK(HTTMonitorBase::finalize());
    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTPerformanceMonitorTool::bookCrudeEffHistograms_1st()
{
    setHistDir(createDirName("/CrudeEff/EffHist_1st/RoadEff/"));

    //h_RoadEfficiency_1st_pt = new TGraphAsymmErrors(); h_RoadEfficiency_1st_pt->SetNameTitle("h_RoadEfficiency_1st_pt", "First Stage Road Efficiency as function of truth pt;p_{T} (GeV);"); ATH_CHECK(regGraph(getHistDir(), h_RoadEfficiency_1st_pt));
    //h_RoadEfficiency_1st_eta = new TGraphAsymmErrors(); h_RoadEfficiency_1st_eta->SetNameTitle("h_RoadEfficiency_1st_eta", "First Stage Road Efficiency as function of truth eta;#eta;"); ATH_CHECK(regGraph(getHistDir(), h_RoadEfficiency_1st_eta));
    //h_RoadEfficiency_1st_phi = new TGraphAsymmErrors(); h_RoadEfficiency_1st_phi->SetNameTitle("h_RoadEfficiency_1st_phi", "First Stage Road Efficiency as function of truth phi;#phi (rad);"); ATH_CHECK(regGraph(getHistDir(), h_RoadEfficiency_1st_phi));
    //h_RoadEfficiency_1st_d0 = new TGraphAsymmErrors(); h_RoadEfficiency_1st_d0->SetNameTitle("h_RoadEfficiency_1st_d0", "First Stage Road Efficiency as function of truth d0;d_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(), h_RoadEfficiency_1st_d0));
    //h_RoadEfficiency_1st_z0 = new TGraphAsymmErrors(); h_RoadEfficiency_1st_z0->SetNameTitle("h_RoadEfficiency_1st_z0", "First Stage Road Efficiency as function of truth z0;z_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(), h_RoadEfficiency_1st_z0));

    h_RoadEfficiency_1st_pt  = new TH1F("h_RoadEfficiency_1st_pt",  "First Stage Road Efficiency as function of truth pt;p_{T} (GeV);", getNXbinsPT(), m_xbinsPT);       ATH_CHECK(regHist(getHistDir(), h_RoadEfficiency_1st_pt));
    h_RoadEfficiency_1st_eta = new TH1F("h_RoadEfficiency_1st_eta", "First Stage Road Efficiency as function of truth eta;#eta;",       50, m_etamin, m_etamax);         ATH_CHECK(regHist(getHistDir(), h_RoadEfficiency_1st_eta));
    h_RoadEfficiency_1st_phi = new TH1F("h_RoadEfficiency_1st_phi", "First Stage Road Efficiency as function of truth phi;#phi (rad);", 50, m_phimin, m_phimax);         ATH_CHECK(regHist(getHistDir(), h_RoadEfficiency_1st_phi));
    h_RoadEfficiency_1st_d0  = new TH1F("h_RoadEfficiency_1st_d0", "First Stage Road Efficiency as function of truth d0;d_{0} (mm);",   50, m_d0Range[0], m_d0Range[1]); ATH_CHECK(regHist(getHistDir(), h_RoadEfficiency_1st_d0));
    h_RoadEfficiency_1st_z0  = new TH1F("h_RoadEfficiency_1st_z0", "First Stage Road Efficiency as function of truth z0;z_{0} (mm);",   50, m_z0Range[0], m_z0Range[1]); ATH_CHECK(regHist(getHistDir(), h_RoadEfficiency_1st_z0));

    setHistDir(createDirName("/CrudeEff/EffHist_1st/TrackEff/"));

    //h_TrackEfficiency_1st_pt = new TGraphAsymmErrors(); h_TrackEfficiency_1st_pt->SetNameTitle("h_TrackEfficiency_1st_pt", "First Stage Track Efficiency as function of truth pt;p_{T} (GeV);"); ATH_CHECK(regGraph(getHistDir(), h_TrackEfficiency_1st_pt));
    //h_TrackEfficiency_1st_eta = new TGraphAsymmErrors(); h_TrackEfficiency_1st_eta->SetNameTitle("h_TrackEfficiency_1st_eta", "First Stage Track Efficiency as function of truth eta;#eta;"); ATH_CHECK(regGraph(getHistDir(), h_TrackEfficiency_1st_eta));
    //h_TrackEfficiency_1st_phi = new TGraphAsymmErrors(); h_TrackEfficiency_1st_phi->SetNameTitle("h_TrackEfficiency_1st_phi", "First Stage Track Efficiency as function of truth phi;#phi (rad);"); ATH_CHECK(regGraph(getHistDir(), h_TrackEfficiency_1st_phi));
    //h_TrackEfficiency_1st_d0 = new TGraphAsymmErrors(); h_TrackEfficiency_1st_d0->SetNameTitle("h_TrackEfficiency_1st_d0", "First Stage Track Efficiency as function of truth d0;d_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(), h_TrackEfficiency_1st_d0));
    //h_TrackEfficiency_1st_z0 = new TGraphAsymmErrors(); h_TrackEfficiency_1st_z0->SetNameTitle("h_TrackEfficiency_1st_z0", "First Stage Track Efficiency as function of truth z0;z_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(), h_TrackEfficiency_1st_z0));

    h_TrackEfficiency_1st_pt  = new TH1F("h_TrackEfficiency_1st_pt",  "First Stage Track Efficiency as function of truth pt;p_{T} (GeV);", getNXbinsPT(), m_xbinsPT);       ATH_CHECK(regHist(getHistDir(), h_TrackEfficiency_1st_pt));
    h_TrackEfficiency_1st_eta = new TH1F("h_TrackEfficiency_1st_eta", "First Stage Track Efficiency as function of truth eta;#eta;",       50, m_etamin, m_etamax);         ATH_CHECK(regHist(getHistDir(), h_TrackEfficiency_1st_eta));
    h_TrackEfficiency_1st_phi = new TH1F("h_TrackEfficiency_1st_phi", "First Stage Track Efficiency as function of truth phi;#phi (rad);", 50, m_phimin, m_phimax);         ATH_CHECK(regHist(getHistDir(), h_TrackEfficiency_1st_phi));
    h_TrackEfficiency_1st_d0  = new TH1F("h_TrackEfficiency_1st_d0", "First Stage Track Efficiency as function of truth d0;d_{0} (mm);",   50, m_d0Range[0], m_d0Range[1]); ATH_CHECK(regHist(getHistDir(), h_TrackEfficiency_1st_d0));
    h_TrackEfficiency_1st_z0  = new TH1F("h_TrackEfficiency_1st_z0", "First Stage Track Efficiency as function of truth z0;z_{0} (mm);",   50, m_z0Range[0], m_z0Range[1]); ATH_CHECK(regHist(getHistDir(), h_TrackEfficiency_1st_z0));


    setHistDir(createDirName("/CrudeEff/EffHist_1st/FullEff/"));

    //h_FullEfficiency_1st_pt = new TGraphAsymmErrors(); h_FullEfficiency_1st_pt->SetNameTitle("h_FullEfficiency_1st_pt", "First Stage Full Efficiency as function of truth pt;p_{T} (GeV);"); ATH_CHECK(regGraph(getHistDir(), h_FullEfficiency_1st_pt));
    //h_FullEfficiency_1st_eta = new TGraphAsymmErrors(); h_FullEfficiency_1st_eta->SetNameTitle("h_FullEfficiency_1st_eta", "First Stage Full Efficiency as function of truth eta;#eta;"); ATH_CHECK(regGraph(getHistDir(), h_FullEfficiency_1st_eta));
    //h_FullEfficiency_1st_phi = new TGraphAsymmErrors(); h_FullEfficiency_1st_phi->SetNameTitle("h_FullEfficiency_1st_phi", "First Stage Full Efficiency as function of truth phi;#phi (rad);"); ATH_CHECK(regGraph(getHistDir(), h_FullEfficiency_1st_phi));
    //h_FullEfficiency_1st_d0 = new TGraphAsymmErrors(); h_FullEfficiency_1st_d0->SetNameTitle("h_FullEfficiency_1st_d0", "First Stage Full Efficiency as function of truth d0;d_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(), h_FullEfficiency_1st_d0));
    //h_FullEfficiency_1st_z0 = new TGraphAsymmErrors(); h_FullEfficiency_1st_z0->SetNameTitle("h_FullEfficiency_1st_z0", "First Stage Full Efficiency as function of truth z0;z_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(), h_FullEfficiency_1st_z0));

    h_FullEfficiency_1st_pt  = new TH1F("h_FullEfficiency_1st_pt",  "First Stage Full Efficiency as function of truth pt;p_{T} (GeV);", getNXbinsPT(), m_xbinsPT);       ATH_CHECK(regHist(getHistDir(), h_FullEfficiency_1st_pt));
    h_FullEfficiency_1st_eta = new TH1F("h_FullEfficiency_1st_eta", "First Stage Full Efficiency as function of truth eta;#eta;",       50, m_etamin, m_etamax);         ATH_CHECK(regHist(getHistDir(), h_FullEfficiency_1st_eta));
    h_FullEfficiency_1st_phi = new TH1F("h_FullEfficiency_1st_phi", "First Stage Full Efficiency as function of truth phi;#phi (rad);", 50, m_phimin, m_phimax);         ATH_CHECK(regHist(getHistDir(), h_FullEfficiency_1st_phi));
    h_FullEfficiency_1st_d0  = new TH1F("h_FullEfficiency_1st_d0", "First Stage Full Efficiency as function of truth d0;d_{0} (mm);",   50, m_d0Range[0], m_d0Range[1]); ATH_CHECK(regHist(getHistDir(), h_FullEfficiency_1st_d0));
    h_FullEfficiency_1st_z0  = new TH1F("h_FullEfficiency_1st_z0", "First Stage Full Efficiency as function of truth z0;z_{0} (mm);",   50, m_z0Range[0], m_z0Range[1]); ATH_CHECK(regHist(getHistDir(), h_FullEfficiency_1st_z0));

    setHistDir(createDirName("/CrudeEff/TruthTrackHist/"));

    h_Truth_Track_Leading_pt = new TH1F("h_Truth_Track_Leading_pt", "pt of leading truth track", getNXbinsPT(), m_xbinsPT); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_Leading_pt));
    h_Truth_Track_Leading_eta = new TH1F("h_Truth_Track_Leading_eta", "eta of leading truth track", 50, m_etamin, m_etamax); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_Leading_eta));
    h_Truth_Track_Leading_phi = new TH1F("h_Truth_Track_Leading_phi", "phi of leading truth track", 50, m_phimin, m_phimax); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_Leading_phi));
    h_Truth_Track_Leading_d0 = new TH1F("h_Truth_Track_Leading_d0", "d0 of leading truth track", 50, m_d0Range[0], m_d0Range[1]); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_Leading_d0));
    h_Truth_Track_Leading_z0 = new TH1F("h_Truth_Track_Leading_z0", "z0 of leading truth track", 50, m_z0Range[0], m_z0Range[1]); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_Leading_z0));


    setHistDir(createDirName("/CrudeEff/MatchedTrackHist_1st/withroad/"));

    h_Truth_Track_withroad_1st_pt = new TH1F("h_Truth_Track_withroad_1st_pt", "pt of truth track w/ first stage road", getNXbinsPT(), m_xbinsPT); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_withroad_1st_pt));
    h_Truth_Track_withroad_1st_eta = new TH1F("h_Truth_Track_withroad_1st_eta", "eta of truth track w/ first stage road", 50, m_etamin, m_etamax); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_withroad_1st_eta));
    h_Truth_Track_withroad_1st_phi = new TH1F("h_Truth_Track_withroad_1st_phi", "phi of truth track w/ first stage road", 50, m_phimin, m_phimax); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_withroad_1st_phi));
    h_Truth_Track_withroad_1st_d0 = new TH1F("h_Truth_Track_withroad_1st_d0", "d0 of truth track w/ first stage road", 50, m_d0Range[0], m_d0Range[1]); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_withroad_1st_d0));
    h_Truth_Track_withroad_1st_z0 = new TH1F("h_Truth_Track_withroad_1st_z0", "z0 of truth track w/ first stage road", 50, m_z0Range[0], m_z0Range[1]); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_withroad_1st_z0));


    setHistDir(createDirName("/CrudeEff/MatchedTrackHist_1st/withtrack/"));

    h_Truth_Track_withtrack_1st_pt = new TH1F("h_Truth_Track_withtrack_1st_pt", "pt of truth track w/ first stage track", getNXbinsPT(), m_xbinsPT); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_withtrack_1st_pt));
    h_Truth_Track_withtrack_1st_eta = new TH1F("h_Truth_Track_withtrack_1st_eta", "eta of truth track w/ first stage track", 50, m_etamin, m_etamax); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_withtrack_1st_eta));
    h_Truth_Track_withtrack_1st_phi = new TH1F("h_Truth_Track_withtrack_1st_phi", "phi of truth track w/ first stage track", 50, m_phimin, m_phimax); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_withtrack_1st_phi));
    h_Truth_Track_withtrack_1st_d0 = new TH1F("h_Truth_Track_withtrack_1st_d0", "d0 of truth track w/ first stage track", 50, m_d0Range[0], m_d0Range[1]); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_withtrack_1st_d0));
    h_Truth_Track_withtrack_1st_z0 = new TH1F("h_Truth_Track_withtrack_1st_z0", "z0 of truth track w/ first stage track", 50, m_z0Range[0], m_z0Range[1]); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_withtrack_1st_z0));


    clearHistDir();

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTPerformanceMonitorTool::bookCrudeEffHistograms_2nd()
{
    setHistDir(createDirName("/CrudeEff/EffHist_2nd/RoadEff/"));

    h_RoadEfficiency_2nd_pt = new TGraphAsymmErrors(); h_RoadEfficiency_2nd_pt->SetNameTitle("h_RoadEfficiency_2nd_pt", "Second Stage Road Efficiency as function of truth pt;p_{T} (GeV);"); ATH_CHECK(regGraph(getHistDir(), h_RoadEfficiency_2nd_pt));
    h_RoadEfficiency_2nd_eta = new TGraphAsymmErrors(); h_RoadEfficiency_2nd_eta->SetNameTitle("h_RoadEfficiency_2nd_eta", "Second Stage Road Efficiency as function of truth eta;#eta;"); ATH_CHECK(regGraph(getHistDir(), h_RoadEfficiency_2nd_eta));
    h_RoadEfficiency_2nd_phi = new TGraphAsymmErrors(); h_RoadEfficiency_2nd_phi->SetNameTitle("h_RoadEfficiency_2nd_phi", "Second Stage Road Efficiency as function of truth phi;#phi (rad);"); ATH_CHECK(regGraph(getHistDir(), h_RoadEfficiency_2nd_phi));
    h_RoadEfficiency_2nd_d0 = new TGraphAsymmErrors(); h_RoadEfficiency_2nd_d0->SetNameTitle("h_RoadEfficiency_2nd_d0", "Second Stage Road Efficiency as function of truth d0;d_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(), h_RoadEfficiency_2nd_d0));
    h_RoadEfficiency_2nd_z0 = new TGraphAsymmErrors(); h_RoadEfficiency_2nd_z0->SetNameTitle("h_RoadEfficiency_2nd_z0", "Second Stage Road Efficiency as function of truth z0;z_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(), h_RoadEfficiency_2nd_z0));


    setHistDir(createDirName("/CrudeEff/EffHist_2nd/TrackEff/"));

    h_TrackEfficiency_2nd_pt = new TGraphAsymmErrors(); h_TrackEfficiency_2nd_pt->SetNameTitle("h_TrackEfficiency_2nd_pt", "Second Stage Track Efficiency as function of truth pt;p_{T} (GeV);"); ATH_CHECK(regGraph(getHistDir(), h_TrackEfficiency_2nd_pt));
    h_TrackEfficiency_2nd_eta = new TGraphAsymmErrors(); h_TrackEfficiency_2nd_eta->SetNameTitle("h_TrackEfficiency_2nd_eta", "Second Stage Track Efficiency as function of truth eta;#eta;"); ATH_CHECK(regGraph(getHistDir(), h_TrackEfficiency_2nd_eta));
    h_TrackEfficiency_2nd_phi = new TGraphAsymmErrors(); h_TrackEfficiency_2nd_phi->SetNameTitle("h_TrackEfficiency_2nd_phi", "Second Stage Track Efficiency as function of truth phi;#phi (rad);"); ATH_CHECK(regGraph(getHistDir(), h_TrackEfficiency_2nd_phi));
    h_TrackEfficiency_2nd_d0 = new TGraphAsymmErrors(); h_TrackEfficiency_2nd_d0->SetNameTitle("h_TrackEfficiency_2nd_d0", "Second Stage Track Efficiency as function of truth d0;d_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(), h_TrackEfficiency_2nd_d0));
    h_TrackEfficiency_2nd_z0 = new TGraphAsymmErrors(); h_TrackEfficiency_2nd_z0->SetNameTitle("h_TrackEfficiency_2nd_z0", "Second Stage Track Efficiency as function of truth z0;z_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(), h_TrackEfficiency_2nd_z0));


    setHistDir(createDirName("/CrudeEff/EffHist_2nd/FullEff/"));

    h_FullEfficiency_2nd_pt = new TGraphAsymmErrors(); h_FullEfficiency_2nd_pt->SetNameTitle("h_FullEfficiency_2nd_pt", "Second Stage Full Efficiency as function of truth pt;p_{T} (GeV);"); ATH_CHECK(regGraph(getHistDir(), h_FullEfficiency_2nd_pt));
    h_FullEfficiency_2nd_eta = new TGraphAsymmErrors(); h_FullEfficiency_2nd_eta->SetNameTitle("h_FullEfficiency_2nd_eta", "Second Stage Full Efficiency as function of truth eta;#eta;"); ATH_CHECK(regGraph(getHistDir(), h_FullEfficiency_2nd_eta));
    h_FullEfficiency_2nd_phi = new TGraphAsymmErrors(); h_FullEfficiency_2nd_phi->SetNameTitle("h_FullEfficiency_2nd_phi", "Second Stage Full Efficiency as function of truth phi;#phi (rad);"); ATH_CHECK(regGraph(getHistDir(), h_FullEfficiency_2nd_phi));
    h_FullEfficiency_2nd_d0 = new TGraphAsymmErrors(); h_FullEfficiency_2nd_d0->SetNameTitle("h_FullEfficiency_2nd_d0", "Second Stage Full Efficiency as function of truth d0;d_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(), h_FullEfficiency_2nd_d0));
    h_FullEfficiency_2nd_z0 = new TGraphAsymmErrors(); h_FullEfficiency_2nd_z0->SetNameTitle("h_FullEfficiency_2nd_z0", "Second Stage Full Efficiency as function of truth z0;z_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(), h_FullEfficiency_2nd_z0));


    setHistDir(createDirName("/CrudeEff/MatchedTrackHist_2nd/withroad/"));

    h_Truth_Track_withroad_2nd_pt = new TH1F("h_Truth_Track_withroad_2nd_pt", "pt of truth track w/ second stage road", getNXbinsPT(), m_xbinsPT); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_withroad_2nd_pt));
    h_Truth_Track_withroad_2nd_eta = new TH1F("h_Truth_Track_withroad_2nd_eta", "eta of truth track w/ second stage road", 50, m_etamin, m_etamax); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_withroad_2nd_eta));
    h_Truth_Track_withroad_2nd_phi = new TH1F("h_Truth_Track_withroad_2nd_phi", "phi of truth track w/ second stage road", 50, m_phimin, m_phimax); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_withroad_2nd_phi));
    h_Truth_Track_withroad_2nd_d0 = new TH1F("h_Truth_Track_withroad_2nd_d0", "d0 of truth track w/ second stage road", 50, m_d0Range[0], m_d0Range[1]); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_withroad_2nd_d0));
    h_Truth_Track_withroad_2nd_z0 = new TH1F("h_Truth_Track_withroad_2nd_z0", "z0 of truth track w/ second stage road", 50, m_z0Range[0], m_z0Range[1]); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_withroad_2nd_z0));


    setHistDir(createDirName("/CrudeEff/MatchedTrackHist_2nd/withtrack/"));

    h_Truth_Track_withtrack_2nd_pt = new TH1F("h_Truth_Track_withtrack_2nd_pt", "pt of truth track w/ second stage track", getNXbinsPT(), m_xbinsPT); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_withtrack_2nd_pt));
    h_Truth_Track_withtrack_2nd_eta = new TH1F("h_Truth_Track_withtrack_2nd_eta", "eta of truth track w/ second stage track", 50, m_etamin, m_etamax); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_withtrack_2nd_eta));
    h_Truth_Track_withtrack_2nd_phi = new TH1F("h_Truth_Track_withtrack_2nd_phi", "phi of truth track w/ second stage track", 50, m_phimin, m_phimax); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_withtrack_2nd_phi));
    h_Truth_Track_withtrack_2nd_d0 = new TH1F("h_Truth_Track_withtrack_2nd_d0", "d0 of truth track w/ second stage track", 50, m_d0Range[0], m_d0Range[1]); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_withtrack_2nd_d0));
    h_Truth_Track_withtrack_2nd_z0 = new TH1F("h_Truth_Track_withtrack_2nd_z0", "z0 of truth track w/ second stage track", 50, m_z0Range[0], m_z0Range[1]); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_withtrack_2nd_z0));


    clearHistDir();

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTPerformanceMonitorTool::bookTruthMatchHistograms_1st()
{
    setHistDir(createDirName("/TruthMatchHist/TruthTrackHist/"));

    h_nTruth_Tracks_Full = new TH1I("h_nTruth_Tracks_Full", "number of truth tracks found per event;# truth tracks;# events", 50, -0.5, 50-0.5); ATH_CHECK(regHist(getHistDir(), h_nTruth_Tracks_Full));

    h_Truth_Track_Full_pt = new TH1F("h_Truth_Track_Full_pt", "pt of all truth tracks", getNXbinsPT(), m_xbinsPT); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_Full_pt));
    h_Truth_Track_Full_eta = new TH1F("h_Truth_Track_Full_eta", "eta of all truth tracks", 50, m_etamin, m_etamax); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_Full_eta));
    h_Truth_Track_Full_phi = new TH1F("h_Truth_Track_Full_phi", "phi of all truth tracks", 50, m_phimin, m_phimax); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_Full_phi));
    h_Truth_Track_Full_d0 = new TH1F("h_Truth_Track_Full_d0", "d0 of all truth tracks", 50, m_d0Range[0], m_d0Range[1]); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_Full_d0));
    h_Truth_Track_Full_z0 = new TH1F("h_Truth_Track_Full_z0", "z0 of all truth tracks", 50, m_z0Range[0], m_z0Range[1]); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_Full_z0));


    setHistDir(createDirName("/TruthMatchHist/OfflineTrackHist/TotalTrackHist/"));

    h_nOffline_Tracks_before_match = new TH1F("h_nOffline_Tracks_before_match", "number of offline tracks found per event w/o filtering;# offline tracks w/o filtering;# events", 100, -0.5, 5000-0.5); ATH_CHECK(regHist(getHistDir(), h_nOffline_Tracks_before_match));
    h_nOffline_Tracks_before_match_0GeV = new TH1F("h_nOffline_Tracks_before_match (HTT cuts)", "number of offline tracks found per event w/o filtering;# offline tracks w/o filtering;# events", 100, -0.5, 5000-0.5); ATH_CHECK(regHist(getHistDir(), h_nOffline_Tracks_before_match_0GeV));
    h_nOffline_Tracks_before_match_1GeV = new TH1F("h_nOffline_Tracks_before_match (HTT cuts >1GeV)", "number of offline tracks found per event w/o filtering;# offline tracks w/o filtering;# events", 100, -0.5, 2500-0.5); ATH_CHECK(regHist(getHistDir(), h_nOffline_Tracks_before_match_1GeV));
    h_nOffline_Tracks_before_match_2GeV = new TH1F("h_nOffline_Tracks_before_match (HTT cuts >2GeV)", "number of offline tracks found per event w/o filtering;# offline tracks w/o filtering;# events", 100, -0.5, 1000-0.5); ATH_CHECK(regHist(getHistDir(), h_nOffline_Tracks_before_match_2GeV));
    h_nOffline_Tracks = new TH1F("h_nOffline_Tracks", "number of offline tracks found per event;# offline tracks;# events", 100, -0.5, 100-0.5); ATH_CHECK(regHist(getHistDir(), h_nOffline_Tracks));

    h_Offline_Track_pt = new TH1F("h_Offline_Track_pt", "pt of all offline tracks", getNXbinsPT(), m_xbinsPT); ATH_CHECK(regHist(getHistDir(), h_Offline_Track_pt));
    h_Offline_Track_eta = new TH1F("h_Offline_Track_eta", "eta of all offline tracks", 50, m_etamin, m_etamax); ATH_CHECK(regHist(getHistDir(), h_Offline_Track_eta));
    h_Offline_Track_phi = new TH1F("h_Offline_Track_phi", "phi of all offline tracks", 50, m_phimin, m_phimax); ATH_CHECK(regHist(getHistDir(), h_Offline_Track_phi));
    h_Offline_Track_d0 = new TH1F("h_Offline_Track_d0", "d0 of all offline tracks",    50, m_d0Range[0], m_d0Range[1]); ATH_CHECK(regHist(getHistDir(), h_Offline_Track_d0));
    h_Offline_Track_z0 = new TH1F("h_Offline_Track_z0", "z0 of all offline tracks",    50, m_z0Range[0], m_z0Range[1]); ATH_CHECK(regHist(getHistDir(), h_Offline_Track_z0));


    setHistDir(createDirName("/TruthMatchHist/OfflineTrackHist/ResHist/no_truthmatch/"));

    h_Offline_vs_Truth_Res_no_truthmatch_qoverpt = new TH1F("h_Offline_vs_Truth_Res_no_truthmatch_qoverpt", "qoverpt resolution of truth track w/ offline track matched without truth matching", 100, (-1.)*ResHistLimit_ipt, ResHistLimit_ipt); ATH_CHECK(regHist(getHistDir(), h_Offline_vs_Truth_Res_no_truthmatch_qoverpt));
    h_Offline_vs_Truth_Res_no_truthmatch_pt = new TH1F("h_Offline_vs_Truth_Res_no_truthmatch_pt", "pt resolution of truth track w/ offline track matched without truth matching", 100, (-1.)*ResHistLimit_pt, ResHistLimit_pt); ATH_CHECK(regHist(getHistDir(), h_Offline_vs_Truth_Res_no_truthmatch_pt));
    h_Offline_vs_Truth_Res_no_truthmatch_eta = new TH1F("h_Offline_vs_Truth_Res_no_truthmatch_eta", "eta resolution of truth track w/ offline track matched without truth matching", 100, (-1.)*ResHistLimit_eta, ResHistLimit_eta); ATH_CHECK(regHist(getHistDir(), h_Offline_vs_Truth_Res_no_truthmatch_eta));
    h_Offline_vs_Truth_Res_no_truthmatch_phi = new TH1F("h_Offline_vs_Truth_Res_no_truthmatch_phi", "phi resolution of truth track w/ offline track matched without truth matching", 100, (-1.)*ResHistLimit_phi, ResHistLimit_phi); ATH_CHECK(regHist(getHistDir(), h_Offline_vs_Truth_Res_no_truthmatch_phi));
    h_Offline_vs_Truth_Res_no_truthmatch_d0 = new TH1F("h_Offline_vs_Truth_Res_no_truthmatch_d0", "d0 resolution of truth track w/ offline track matched without truth matching", 100, (-1.)*ResHistLimit_d0, ResHistLimit_d0); ATH_CHECK(regHist(getHistDir(), h_Offline_vs_Truth_Res_no_truthmatch_d0));
    h_Offline_vs_Truth_Res_no_truthmatch_z0 = new TH1F("h_Offline_vs_Truth_Res_no_truthmatch_z0", "z0 resolution of truth track w/ offline track matched without truth matching", 100, (-1.)*ResHistLimit_z0, ResHistLimit_z0); ATH_CHECK(regHist(getHistDir(), h_Offline_vs_Truth_Res_no_truthmatch_z0));
    h_Offline_vs_Truth_Res_no_truthmatch_z0_wide = new TH1F("h_Offline_vs_Truth_Res_no_truthmatch_z0_wide", "z0 resolution of truth track w/ offline track matched without truth matching (wide range)", 100, -25000, 25000); ATH_CHECK(regHist(getHistDir(), h_Offline_vs_Truth_Res_no_truthmatch_z0_wide));


    setHistDir(createDirName("/TruthMatchHist/OfflineTrackHist/EffHist/"));

    h_Offline_vs_Truth_eff_pt = new TGraphAsymmErrors(); h_Offline_vs_Truth_eff_pt->SetNameTitle("h_Offline_vs_Truth_eff_pt","Offline vs. Truth Efficiency as function of truth pt;p_{T} (GeV);"); ATH_CHECK(regGraph(getHistDir(), h_Offline_vs_Truth_eff_pt));
    h_Offline_vs_Truth_eff_eta = new TGraphAsymmErrors(); h_Offline_vs_Truth_eff_eta->SetNameTitle("h_Offline_vs_Truth_eff_eta","Offline vs. Truth Efficiency as function of truth eta;#eta;"); ATH_CHECK(regGraph(getHistDir(), h_Offline_vs_Truth_eff_eta));
    h_Offline_vs_Truth_eff_phi = new TGraphAsymmErrors(); h_Offline_vs_Truth_eff_phi->SetNameTitle("h_Offline_vs_Truth_eff_phi","Offline vs. Truth Efficiency as function of truth phi;#phi (rad);"); ATH_CHECK(regGraph(getHistDir(), h_Offline_vs_Truth_eff_phi));
    h_Offline_vs_Truth_eff_d0 = new TGraphAsymmErrors(); h_Offline_vs_Truth_eff_d0->SetNameTitle("h_Offline_vs_Truth_eff_d0","Offline vs. Truth Efficiency as function of truth d0;d_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(), h_Offline_vs_Truth_eff_d0));
    h_Offline_vs_Truth_eff_z0 = new TGraphAsymmErrors(); h_Offline_vs_Truth_eff_z0->SetNameTitle("h_Offline_vs_Truth_eff_z0","Offline vs. Truth Efficiency as function of truth z0;z_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(), h_Offline_vs_Truth_eff_z0));


    setHistDir(createDirName("/TruthMatchHist/OfflineTrackHist/MatchedTrackHist/"));

    h_Truth_Track_Offline_matched_pt = new TH1F("h_Truth_Track_Offline_matched_pt", "pt of truth track w/ offline track matched", getNXbinsPT(), m_xbinsPT); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_Offline_matched_pt));
    h_Truth_Track_Offline_matched_eta = new TH1F("h_Truth_Track_Offline_matched_eta", "eta of truth track w/ offline track matched", 50, m_etamin, m_etamax); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_Offline_matched_eta));
    h_Truth_Track_Offline_matched_phi = new TH1F("h_Truth_Track_Offline_matched_phi", "phi of truth track w/ offline track matched", 50, m_phimin, m_phimax); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_Offline_matched_phi));
    h_Truth_Track_Offline_matched_d0 = new TH1F("h_Truth_Track_Offline_matched_d0", "d0 of truth track w/ offline track matched",    50, m_d0Range[0], m_d0Range[1]); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_Offline_matched_d0));
    h_Truth_Track_Offline_matched_z0 = new TH1F("h_Truth_Track_Offline_matched_z0", "z0 of truth track w/ offline track matched",    50, m_z0Range[0], m_z0Range[1]); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_Offline_matched_z0));


    setHistDir(createDirName("/TruthMatchHist/OfflineTrackHist/ResHist/truthmatch/1D_Res/"));

    h_Offline_vs_Truth_Res_truthmatch_pt = new TH1F("h_Offline_vs_Truth_Res_truthmatch_pt", "pt resolution of truth track w/ offline track matched using truth matching",    100, (-1.)*ResHistLimit_pt2, ResHistLimit_pt2); ATH_CHECK(regHist(getHistDir(), h_Offline_vs_Truth_Res_truthmatch_pt));
    h_Offline_vs_Truth_Res_truthmatch_eta = new TH1F("h_Offline_vs_Truth_Res_truthmatch_eta", "eta resolution of truth track w/ offline track matched using truth matching", 100, (-1.)*ResHistLimit_eta2, ResHistLimit_eta2); ATH_CHECK(regHist(getHistDir(), h_Offline_vs_Truth_Res_truthmatch_eta));
    h_Offline_vs_Truth_Res_truthmatch_phi = new TH1F("h_Offline_vs_Truth_Res_truthmatch_phi", "phi resolution of truth track w/ offline track matched using truth matching", 100, (-1.)*ResHistLimit_phi2, ResHistLimit_phi2); ATH_CHECK(regHist(getHistDir(), h_Offline_vs_Truth_Res_truthmatch_phi));
    h_Offline_vs_Truth_Res_truthmatch_d0 = new TH1F("h_Offline_vs_Truth_Res_truthmatch_d0", "d0 resolution of truth track w/ offline track matched using truth matching",    100, (-1.)*ResHistLimit_d02, ResHistLimit_d02); ATH_CHECK(regHist(getHistDir(), h_Offline_vs_Truth_Res_truthmatch_d0));
    h_Offline_vs_Truth_Res_truthmatch_z0 = new TH1F("h_Offline_vs_Truth_Res_truthmatch_z0", "z0 resolution of truth track w/ offline track matched using truth matching",    100, (-1.)*ResHistLimit_z02, ResHistLimit_z02); ATH_CHECK(regHist(getHistDir(), h_Offline_vs_Truth_Res_truthmatch_z0));
    h_Offline_vs_Truth_Res_truthmatch_qoverpt = new TH1F("h_Offline_vs_Truth_Res_truthmatch_qoverpt","qoverpt resolution of truth track w/ offline track matched using truth matching", 100, (-1.)*ResHistLimit_ipt2, ResHistLimit_ipt2); ATH_CHECK(regHist(getHistDir(), h_Offline_vs_Truth_Res_truthmatch_qoverpt));


    setHistDir(createDirName("/TruthMatchHist/OfflineTrackHist/ResHist/truthmatch/2D_Res/vspt/"));

    h_Offline_vs_Truth_Res_truthmatch_pt_vspt = new TH2F("h_Offline_vs_Truth_Res_truthmatch_pt_vspt", "pt resolution of truth track w/ offline track matched vs pt using truth matching",    getNXbinsPT(), m_xbinsPT, 100, (-1.)*ResHistLimit_pt2, ResHistLimit_pt2); ATH_CHECK(regHist(getHistDir(), h_Offline_vs_Truth_Res_truthmatch_pt_vspt));
    h_Offline_vs_Truth_Res_truthmatch_eta_vspt = new TH2F("h_Offline_vs_Truth_Res_truthmatch_eta_vspt", "eta resolution of truth track w/ offline track matched vs pt using truth matching", getNXbinsPT(), m_xbinsPT, 100, (-1.)*ResHistLimit_eta2, ResHistLimit_eta2); ATH_CHECK(regHist(getHistDir(), h_Offline_vs_Truth_Res_truthmatch_eta_vspt));
    h_Offline_vs_Truth_Res_truthmatch_phi_vspt = new TH2F("h_Offline_vs_Truth_Res_truthmatch_phi_vspt", "phi resolution of truth track w/ offline track matched vs pt using truth matching", getNXbinsPT(), m_xbinsPT, 100, (-1.)*ResHistLimit_phi2, ResHistLimit_phi2); ATH_CHECK(regHist(getHistDir(), h_Offline_vs_Truth_Res_truthmatch_phi_vspt));
    h_Offline_vs_Truth_Res_truthmatch_d0_vspt = new TH2F("h_Offline_vs_Truth_Res_truthmatch_d0_vspt", "d0 resolution of truth track w/ offline track matched vs pt using truth matching",    getNXbinsPT(), m_xbinsPT, 100, (-1.)*ResHistLimit_d02, ResHistLimit_d02); ATH_CHECK(regHist(getHistDir(), h_Offline_vs_Truth_Res_truthmatch_d0_vspt));
    h_Offline_vs_Truth_Res_truthmatch_z0_vspt = new TH2F("h_Offline_vs_Truth_Res_truthmatch_z0_vspt", "z0 resolution of truth track w/ offline track matched vs pt using truth matching",    getNXbinsPT(), m_xbinsPT, 100, (-1.)*ResHistLimit_z02, ResHistLimit_z02); ATH_CHECK(regHist(getHistDir(), h_Offline_vs_Truth_Res_truthmatch_z0_vspt));
    h_Offline_vs_Truth_Res_truthmatch_qoverpt_vspt = new TH2F("h_Offline_vs_Truth_Res_truthmatch_qoverpt_vspt", "qoverpt resolution of truth track w/ offline track matched vs pt using truth matching", getNXbinsPT(), m_xbinsPT, 100, (-1.)*ResHistLimit_ipt2, ResHistLimit_ipt2); ATH_CHECK(regHist(getHistDir(), h_Offline_vs_Truth_Res_truthmatch_qoverpt_vspt));


    setHistDir(createDirName("/TruthMatchHist/OfflineTrackHist/ResHist/truthmatch/2D_Res/vseta/"));

    h_Offline_vs_Truth_Res_truthmatch_pt_vseta = new TH2F("h_Offline_vs_Truth_Res_truthmatch_pt_vseta", "pt resolution of truth track w/ offline track matched vs eta using truth matching",    50, m_etamin, m_etamax, 100, (-1.)*ResHistLimit_pt2, ResHistLimit_pt2); ATH_CHECK(regHist(getHistDir(), h_Offline_vs_Truth_Res_truthmatch_pt_vseta));
    h_Offline_vs_Truth_Res_truthmatch_eta_vseta = new TH2F("h_Offline_vs_Truth_Res_truthmatch_eta_vseta", "eta resolution of truth track w/ offline track matched vs eta using truth matching", 50, m_etamin, m_etamax, 100, (-1.)*ResHistLimit_eta2, ResHistLimit_eta2); ATH_CHECK(regHist(getHistDir(), h_Offline_vs_Truth_Res_truthmatch_eta_vseta));
    h_Offline_vs_Truth_Res_truthmatch_phi_vseta = new TH2F("h_Offline_vs_Truth_Res_truthmatch_phi_vseta", "phi resolution of truth track w/ offline track matched vs eta using truth matching", 50, m_etamin, m_etamax, 100, (-1.)*ResHistLimit_phi2, ResHistLimit_phi2); ATH_CHECK(regHist(getHistDir(), h_Offline_vs_Truth_Res_truthmatch_phi_vseta));
    h_Offline_vs_Truth_Res_truthmatch_d0_vseta = new TH2F("h_Offline_vs_Truth_Res_truthmatch_d0_vseta", "d0 resolution of truth track w/ offline track matched vs eta using truth matching",    50, m_etamin, m_etamax, 100, (-1.)*ResHistLimit_d02, ResHistLimit_d02); ATH_CHECK(regHist(getHistDir(), h_Offline_vs_Truth_Res_truthmatch_d0_vseta));
    h_Offline_vs_Truth_Res_truthmatch_z0_vseta = new TH2F("h_Offline_vs_Truth_Res_truthmatch_z0_vseta", "z0 resolution of truth track w/ offline track matched vs eta using truth matching",    50, m_etamin, m_etamax, 100, (-1.)*ResHistLimit_z02, ResHistLimit_z02); ATH_CHECK(regHist(getHistDir(), h_Offline_vs_Truth_Res_truthmatch_z0_vseta));
    h_Offline_vs_Truth_Res_truthmatch_qoverpt_vseta = new TH2F("h_Offline_vs_Truth_Res_truthmatch_qoverpt_vseta", "qoverpt resolution of truth track w/ offline track matched vs eta using truth matching", 1000, m_etamin, m_etamax, 100, (-1.)*ResHistLimit_ipt2, ResHistLimit_ipt2); ATH_CHECK(regHist(getHistDir(), h_Offline_vs_Truth_Res_truthmatch_qoverpt_vseta));


    setHistDir(createDirName("/TruthMatchHist/OfflineTrackHist/FakeRateHist/"));

    h_ratio_Offline_Truth = new TH1F("h_ratio_Offline_Truth", "# Offline Tracks / # Truth Tracks, per event;ratio(nOffline/nTruth);# events", 100, -0.5, 100-0.5); ATH_CHECK(regHist(getHistDir(), h_ratio_Offline_Truth));


    setHistDir(createDirName("/TruthMatchHist/RoadHist_1st/HTT_1st_vs_Truth/EffHist/"));

    //h_HTT_1st_vs_Truth_Road_eff_pt = new TGraphAsymmErrors(); h_HTT_1st_vs_Truth_Road_eff_pt->SetNameTitle("h_HTT_1st_vs_Truth_Road_eff_pt","First Stage HTT vs. Truth Road Efficiency as function of truth pt using truth matching;p_{T} (GeV);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_1st_vs_Truth_Road_eff_pt));
    //h_HTT_1st_vs_Truth_Road_eff_eta = new TGraphAsymmErrors(); h_HTT_1st_vs_Truth_Road_eff_eta->SetNameTitle("h_HTT_1st_vs_Truth_Road_eff_eta","First Stage HTT vs. Truth Road Efficiency as function of truth eta using truth matching;#eta;"); ATH_CHECK(regGraph(getHistDir(), h_HTT_1st_vs_Truth_Road_eff_eta));
    //h_HTT_1st_vs_Truth_Road_eff_phi = new TGraphAsymmErrors(); h_HTT_1st_vs_Truth_Road_eff_phi->SetNameTitle("h_HTT_1st_vs_Truth_Road_eff_phi","First Stage HTT vs. Truth Road Efficiency as function of truth phi using truth matching;#phi (rad);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_1st_vs_Truth_Road_eff_phi));
    //h_HTT_1st_vs_Truth_Road_eff_d0 = new TGraphAsymmErrors(); h_HTT_1st_vs_Truth_Road_eff_d0->SetNameTitle("h_HTT_1st_vs_Truth_Road_eff_d0","First Stage HTT vs. Truth Road Efficiency as function of truth d0 using truth matching;d_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_1st_vs_Truth_Road_eff_d0));
    //h_HTT_1st_vs_Truth_Road_eff_z0 = new TGraphAsymmErrors(); h_HTT_1st_vs_Truth_Road_eff_z0->SetNameTitle("h_HTT_1st_vs_Truth_Road_eff_z0","First Stage HTT vs. Truth Road Efficiency as function of truth z0 using truth matching;z_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_1st_vs_Truth_Road_eff_z0));


    h_HTT_1st_vs_Truth_Road_eff_pt  = new TH1F("h_HTT_1st_vs_Truth_Road_eff_pt",  "First Stage HTT vs. Truth Road Efficiency as function of truth pt using truth matching;p_{T} (GeV);", getNXbinsPT(), m_xbinsPT);       ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_Road_eff_pt));
    h_HTT_1st_vs_Truth_Road_eff_eta = new TH1F("h_HTT_1st_vs_Truth_Road_eff_eta", "First Stage HTT vs. Truth Road Efficiency as function of truth eta using truth matching;#eta;",       50, m_etamin, m_etamax);         ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_Road_eff_eta));
    h_HTT_1st_vs_Truth_Road_eff_phi = new TH1F("h_HTT_1st_vs_Truth_Road_eff_phi", "First Stage HTT vs. Truth Road Efficiency as function of truth phi using truth matching;#phi (rad);", 50, m_phimin, m_phimax);         ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_Road_eff_phi));
    h_HTT_1st_vs_Truth_Road_eff_d0  = new TH1F("h_HTT_1st_vs_Truth_Road_eff_d0", "First Stage HTT vs. Truth Road Efficiency as function of truth d0 using truth matching;d_{0} (mm);",   50, m_d0Range[0], m_d0Range[1]); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_Road_eff_d0));
    h_HTT_1st_vs_Truth_Road_eff_z0  = new TH1F("h_HTT_1st_vs_Truth_Road_eff_z0", "First Stage HTT vs. Truth Road Efficiency as function of truth z0 using truth matching;z_{0} (mm);",   50, m_z0Range[0], m_z0Range[1]); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_Road_eff_z0));

    setHistDir(createDirName("/TruthMatchHist/RoadHist_1st/HTT_1st_vs_Truth/MatchedTrackHist/"));

    h_Truth_Track_Road_1st_matched_pt = new TH1F("h_Truth_Track_Road_1st_matched_pt", "pt of truth track w/ first stage road matched using truth matching", getNXbinsPT(), m_xbinsPT); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_Road_1st_matched_pt));
    h_Truth_Track_Road_1st_matched_eta = new TH1F("h_Truth_Track_Road_1st_matched_eta", "eta of truth track w/ first stage road matched using truth matching", 50, m_etamin, m_etamax); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_Road_1st_matched_eta));
    h_Truth_Track_Road_1st_matched_phi = new TH1F("h_Truth_Track_Road_1st_matched_phi", "phi of truth track w/ first stage road matched using truth matching", 50, m_phimin, m_phimax); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_Road_1st_matched_phi));
    h_Truth_Track_Road_1st_matched_d0 = new TH1F("h_Truth_Track_Road_1st_matched_d0", "d0 of truth track w/ first stage road matched using truth matching", 50, m_d0Range[0], m_d0Range[1]); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_Road_1st_matched_d0));
    h_Truth_Track_Road_1st_matched_z0 = new TH1F("h_Truth_Track_Road_1st_matched_z0", "z0 of truth track w/ first stage road matched using truth matching", 50, m_z0Range[0], m_z0Range[1]); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_Road_1st_matched_z0));


    setHistDir(createDirName("/TruthMatchHist/RoadHist_1st/HTT_1st_vs_Offline/EffHist/"));

    h_HTT_1st_vs_Offline_Road_eff_pt = new TGraphAsymmErrors(); h_HTT_1st_vs_Offline_Road_eff_pt->SetNameTitle("h_HTT_1st_vs_Offline_Road_eff_pt","First Stage HTT vs. Offline Road Efficiency as function of truth pt using truth matching;p_{T} (GeV);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_1st_vs_Offline_Road_eff_pt));
    h_HTT_1st_vs_Offline_Road_eff_eta = new TGraphAsymmErrors(); h_HTT_1st_vs_Offline_Road_eff_eta->SetNameTitle("h_HTT_1st_vs_Offline_Road_eff_eta","First Stage HTT vs. Offline Road Efficiency as function of truth eta using truth matching;#eta;"); ATH_CHECK(regGraph(getHistDir(), h_HTT_1st_vs_Offline_Road_eff_eta));
    h_HTT_1st_vs_Offline_Road_eff_phi = new TGraphAsymmErrors(); h_HTT_1st_vs_Offline_Road_eff_phi->SetNameTitle("h_HTT_1st_vs_Offline_Road_eff_phi","First Stage HTT vs. Offline Road Efficiency as function of truth phi using truth matching;#phi (rad);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_1st_vs_Offline_Road_eff_phi));
    h_HTT_1st_vs_Offline_Road_eff_d0 = new TGraphAsymmErrors(); h_HTT_1st_vs_Offline_Road_eff_d0->SetNameTitle("h_HTT_1st_vs_Offline_Road_eff_d0","First Stage HTT vs. Offline Road Efficiency as function of truth d0 using truth matching;d_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_1st_vs_Offline_Road_eff_d0));
    h_HTT_1st_vs_Offline_Road_eff_z0 = new TGraphAsymmErrors(); h_HTT_1st_vs_Offline_Road_eff_z0->SetNameTitle("h_HTT_1st_vs_Offline_Road_eff_z0","First Stage HTT vs. Offline Road Efficiency as function of truth z0 using truth matching;z_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_1st_vs_Offline_Road_eff_z0));


    setHistDir(createDirName("/TruthMatchHist/TrackHist_1st/HTT_1st_vs_Truth/All/EffHist/"));

    h_HTT_1st_vs_Truth_eff_pt = new TGraphAsymmErrors(); h_HTT_1st_vs_Truth_eff_pt->SetNameTitle("h_HTT_1st_vs_Truth_eff_pt","First Stage HTT vs. Truth Efficiency as function of truth pt;p_{T} (GeV);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_1st_vs_Truth_eff_pt));
    h_HTT_1st_vs_Truth_eff_eta = new TGraphAsymmErrors(); h_HTT_1st_vs_Truth_eff_eta->SetNameTitle("h_HTT_1st_vs_Truth_eff_eta","First Stage HTT vs. Truth Efficiency as function of truth eta;#eta;"); ATH_CHECK(regGraph(getHistDir(), h_HTT_1st_vs_Truth_eff_eta));
    h_HTT_1st_vs_Truth_eff_phi = new TGraphAsymmErrors(); h_HTT_1st_vs_Truth_eff_phi->SetNameTitle("h_HTT_1st_vs_Truth_eff_phi","First Stage HTT vs. Truth Efficiency as function of truth phi;#phi (rad);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_1st_vs_Truth_eff_phi));
    h_HTT_1st_vs_Truth_eff_d0 = new TGraphAsymmErrors(); h_HTT_1st_vs_Truth_eff_d0->SetNameTitle("h_HTT_1st_vs_Truth_eff_d0","First Stage HTT vs. Truth Efficiency as function of truth d0;d_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_1st_vs_Truth_eff_d0));
    h_HTT_1st_vs_Truth_eff_z0 = new TGraphAsymmErrors(); h_HTT_1st_vs_Truth_eff_z0->SetNameTitle("h_HTT_1st_vs_Truth_eff_z0","First Stage HTT vs. Truth Efficiency as function of truth z0;z_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_1st_vs_Truth_eff_z0));


    setHistDir(createDirName("/TruthMatchHist/TrackHist_1st/HTT_1st_vs_Truth/All/MatchedTrackHist/"));

    h_Truth_Track_HTT_1st_matched_pt = new TH1F("h_Truth_Track_HTT_1st_matched_pt", "pt of truth track w/ first stage htt track matched", getNXbinsPT(), m_xbinsPT); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_HTT_1st_matched_pt));
    h_Truth_Track_HTT_1st_matched_eta = new TH1F("h_Truth_Track_HTT_1st_matched_eta", "eta of truth track w/ first stage htt track matched", 50, m_etamin, m_etamax); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_HTT_1st_matched_eta));
    h_Truth_Track_HTT_1st_matched_phi = new TH1F("h_Truth_Track_HTT_1st_matched_phi", "phi of truth track w/ first stage htt track matched", 50, m_phimin, m_phimax); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_HTT_1st_matched_phi));
    h_Truth_Track_HTT_1st_matched_d0 = new TH1F("h_Truth_Track_HTT_1st_matched_d0", "d0 of truth track w/ first stage htt track matched", 50, m_d0Range[0], m_d0Range[1]); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_HTT_1st_matched_d0));
    h_Truth_Track_HTT_1st_matched_z0 = new TH1F("h_Truth_Track_HTT_1st_matched_z0", "z0 of truth track w/ first stage htt track matched", 50, m_z0Range[0], m_z0Range[1]); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_HTT_1st_matched_z0));


    setHistDir(createDirName("/TruthMatchHist/TrackHist_1st/HTT_1st_vs_Truth/All/ResHist/1D_Res/"));

    h_HTT_1st_vs_Truth_Res_truthmatch_pt = new TH1F("h_HTT_1st_vs_Truth_Res_truthmatch_pt", "pt resolution of truth track w/ first stage htt track matched using truth matching",   100, (-1.)*ResHistLimit_pt, ResHistLimit_pt); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_Res_truthmatch_pt));
    h_HTT_1st_vs_Truth_Res_truthmatch_eta = new TH1F("h_HTT_1st_vs_Truth_Res_truthmatch_eta", "eta resolution of truth track w/ first stage htt track matched using truth matching",100, (-1.)*ResHistLimit_eta, ResHistLimit_eta); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_Res_truthmatch_eta));
    h_HTT_1st_vs_Truth_Res_truthmatch_phi = new TH1F("h_HTT_1st_vs_Truth_Res_truthmatch_phi", "phi resolution of truth track w/ first stage htt track matched using truth matching",100, (-1.)*ResHistLimit_phi, ResHistLimit_phi); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_Res_truthmatch_phi));
    h_HTT_1st_vs_Truth_Res_truthmatch_d0 = new TH1F("h_HTT_1st_vs_Truth_Res_truthmatch_d0", "d0 resolution of truth track w/ first stage htt track matched using truth matching",   100, (-1.)*ResHistLimit_d0, ResHistLimit_d0); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_Res_truthmatch_d0));
    h_HTT_1st_vs_Truth_Res_truthmatch_z0 = new TH1F("h_HTT_1st_vs_Truth_Res_truthmatch_z0", "z0 resolution of truth track w/ first stage htt track matched using truth matching",   100, (-1.)*ResHistLimit_z0, ResHistLimit_z0); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_Res_truthmatch_z0));
    h_HTT_1st_vs_Truth_Res_truthmatch_qoverpt = new TH1F("h_HTT_1st_vs_Truth_Res_truthmatch_qoverpt","qoverpt resolution of truth track w/ first stage htt track matched using truth matching", 100, (-1.)*ResHistLimit_ipt, ResHistLimit_ipt); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_Res_truthmatch_qoverpt));


    setHistDir(createDirName("/TruthMatchHist/TrackHist_1st/HTT_1st_vs_Truth/All/ResHist/2D_Res/vspt/"));

    h_HTT_1st_vs_Truth_Res_truthmatch_pt_vspt = new TH2F("h_HTT_1st_vs_Truth_Res_truthmatch_pt_vspt", "pt resolution of truth track w/ first stage htt track matched vs pt using truth matching",    getNXbinsPT(), m_xbinsPT, 100, (-1.)*ResHistLimit_pt, ResHistLimit_pt); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_Res_truthmatch_pt_vspt));
    h_HTT_1st_vs_Truth_Res_truthmatch_eta_vspt = new TH2F("h_HTT_1st_vs_Truth_Res_truthmatch_eta_vspt", "eta resolution of truth track w/ first stage htt track matched vs pt using truth matching", getNXbinsPT(), m_xbinsPT, 100, (-1.)*ResHistLimit_eta, ResHistLimit_eta); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_Res_truthmatch_eta_vspt));
    h_HTT_1st_vs_Truth_Res_truthmatch_phi_vspt = new TH2F("h_HTT_1st_vs_Truth_Res_truthmatch_phi_vspt", "phi resolution of truth track w/ first stage htt track matched vs pt using truth matching", getNXbinsPT(), m_xbinsPT, 100, (-1.)*ResHistLimit_phi, ResHistLimit_phi); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_Res_truthmatch_phi_vspt));
    h_HTT_1st_vs_Truth_Res_truthmatch_d0_vspt = new TH2F("h_HTT_1st_vs_Truth_Res_truthmatch_d0_vspt", "d0 resolution of truth track w/ first stage htt track matched vs pt using truth matching",    getNXbinsPT(), m_xbinsPT, 100, (-1.)*ResHistLimit_d0, ResHistLimit_d0); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_Res_truthmatch_d0_vspt));
    h_HTT_1st_vs_Truth_Res_truthmatch_z0_vspt = new TH2F("h_HTT_1st_vs_Truth_Res_truthmatch_z0_vspt", "z0 resolution of truth track w/ first stage htt track matched vs pt using truth matching",    getNXbinsPT(), m_xbinsPT, 100, (-1.)*ResHistLimit_z0, ResHistLimit_z0); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_Res_truthmatch_z0_vspt));
    h_HTT_1st_vs_Truth_Res_truthmatch_qoverpt_vspt = new TH2F("h_HTT_1st_vs_Truth_Res_truthmatch_qoverpt_vspt", "qoverpt resolution of truth track w/ first stage htt track matched vs pt using truth matching", getNXbinsPT(), m_xbinsPT, 100, (-1.)*ResHistLimit_ipt, ResHistLimit_ipt); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_Res_truthmatch_qoverpt_vspt));


    setHistDir(createDirName("/TruthMatchHist/TrackHist_1st/HTT_1st_vs_Truth/All/ResHist/2D_Res/vseta/"));

    h_HTT_1st_vs_Truth_Res_truthmatch_pt_vseta = new TH2F("h_HTT_1st_vs_Truth_Res_truthmatch_pt_vseta", "pt resolution of truth track w/ first stage htt track matched vs eta using truth matching",    50, m_etamin, m_etamax, 100, (-1.)*ResHistLimit_pt, ResHistLimit_pt); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_Res_truthmatch_pt_vseta));
    h_HTT_1st_vs_Truth_Res_truthmatch_eta_vseta = new TH2F("h_HTT_1st_vs_Truth_Res_truthmatch_eta_vseta", "eta resolution of truth track w/ first stage htt track matched vs eta using truth matching", 50, m_etamin, m_etamax, 100, (-1.)*ResHistLimit_eta, ResHistLimit_eta); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_Res_truthmatch_eta_vseta));
    h_HTT_1st_vs_Truth_Res_truthmatch_phi_vseta = new TH2F("h_HTT_1st_vs_Truth_Res_truthmatch_phi_vseta", "phi resolution of truth track w/ first stage htt track matched vs eta using truth matching", 50, m_etamin, m_etamax, 100, (-1.)*ResHistLimit_phi, ResHistLimit_phi); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_Res_truthmatch_phi_vseta));
    h_HTT_1st_vs_Truth_Res_truthmatch_d0_vseta = new TH2F("h_HTT_1st_vs_Truth_Res_truthmatch_d0_vseta", "d0 resolution of truth track w/ first stage htt track matched vs eta using truth matching",    50, m_etamin, m_etamax, 100, (-1.)*ResHistLimit_d0, ResHistLimit_d0); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_Res_truthmatch_d0_vseta));
    h_HTT_1st_vs_Truth_Res_truthmatch_z0_vseta = new TH2F("h_HTT_1st_vs_Truth_Res_truthmatch_z0_vseta", "z0 resolution of truth track w/ first stage htt track matched vs eta using truth matching",    50, m_etamin, m_etamax, 100, (-1.)*ResHistLimit_z0, ResHistLimit_z0); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_Res_truthmatch_z0_vseta));
    h_HTT_1st_vs_Truth_Res_truthmatch_qoverpt_vseta = new TH2F("h_HTT_1st_vs_Truth_Res_truthmatch_qoverpt_vseta", "qoverpt resolution of truth track w/ first stage htt track matched vs eta using truth matching", 50, m_etamin, m_etamax, 100, (-1.)*ResHistLimit_ipt, ResHistLimit_ipt); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_Res_truthmatch_qoverpt_vseta));


    setHistDir(createDirName("/TruthMatchHist/TrackHist_1st/HTT_1st_vs_Truth/passChi2/EffHist/"));

    h_HTT_1st_vs_Truth_passChi2_eff_pt = new TGraphAsymmErrors(); h_HTT_1st_vs_Truth_passChi2_eff_pt->SetNameTitle("h_HTT_1st_vs_Truth_passChi2_eff_pt","First Stage HTT vs. Truth Efficiency passing Chi^2 cut as function of truth pt;p_{T} (GeV);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_1st_vs_Truth_passChi2_eff_pt));
    h_HTT_1st_vs_Truth_passChi2_eff_eta = new TGraphAsymmErrors(); h_HTT_1st_vs_Truth_passChi2_eff_eta->SetNameTitle("h_HTT_1st_vs_Truth_passChi2_eff_eta","First Stage HTT vs. Truth Efficiency passing Chi^2 cut as function of truth eta;#eta;"); ATH_CHECK(regGraph(getHistDir(), h_HTT_1st_vs_Truth_passChi2_eff_eta));
    h_HTT_1st_vs_Truth_passChi2_eff_phi = new TGraphAsymmErrors(); h_HTT_1st_vs_Truth_passChi2_eff_phi->SetNameTitle("h_HTT_1st_vs_Truth_passChi2_eff_phi","First Stage HTT vs. Truth Efficiency passing Chi^2 cut as function of truth phi;#phi (rad);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_1st_vs_Truth_passChi2_eff_phi));
    h_HTT_1st_vs_Truth_passChi2_eff_d0 = new TGraphAsymmErrors(); h_HTT_1st_vs_Truth_passChi2_eff_d0->SetNameTitle("h_HTT_1st_vs_Truth_passChi2_eff_d0","First Stage HTT vs. Truth Efficiency passing Chi^2 cut as function of truth d0;d_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_1st_vs_Truth_passChi2_eff_d0));
    h_HTT_1st_vs_Truth_passChi2_eff_z0 = new TGraphAsymmErrors(); h_HTT_1st_vs_Truth_passChi2_eff_z0->SetNameTitle("h_HTT_1st_vs_Truth_passChi2_eff_z0","First Stage HTT vs. Truth Efficiency passing Chi^2 cut as function of truth z0;z_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_1st_vs_Truth_passChi2_eff_z0));


    setHistDir(createDirName("/TruthMatchHist/TrackHist_1st/HTT_1st_vs_Truth/passChi2/MatchedTrackHist/"));

    h_Truth_Track_HTT_1st_passChi2_matched_pt = new TH1F("h_Truth_Track_HTT_1st_passChi2_matched_pt", "pt of truth track w/ first stage htt track passing Chi^2 cut matched", getNXbinsPT(), m_xbinsPT); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_HTT_1st_passChi2_matched_pt));
    h_Truth_Track_HTT_1st_passChi2_matched_eta = new TH1F("h_Truth_Track_HTT_1st_passChi2_matched_eta", "eta of truth track w/ first stage htt track passing Chi^2 cut matched", 50, m_etamin, m_etamax); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_HTT_1st_passChi2_matched_eta));
    h_Truth_Track_HTT_1st_passChi2_matched_phi = new TH1F("h_Truth_Track_HTT_1st_passChi2_matched_phi", "phi of truth track w/ first stage htt track passing Chi^2 cut matched", 50, m_phimin, m_phimax); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_HTT_1st_passChi2_matched_phi));
    h_Truth_Track_HTT_1st_passChi2_matched_d0 = new TH1F("h_Truth_Track_HTT_1st_passChi2_matched_d0", "d0 of truth track w/ first stage htt track passing Chi^2 cut matched", 50, m_d0Range[0], m_d0Range[1]); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_HTT_1st_passChi2_matched_d0));
    h_Truth_Track_HTT_1st_passChi2_matched_z0 = new TH1F("h_Truth_Track_HTT_1st_passChi2_matched_z0", "z0 of truth track w/ first stage htt track passing Chi^2 cut matched", 50, m_z0Range[0], m_z0Range[1]); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_HTT_1st_passChi2_matched_z0));


    setHistDir(createDirName("/TruthMatchHist/TrackHist_1st/HTT_1st_vs_Truth/passChi2/ResHist/1D_Res/"));

    h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_pt = new TH1F("h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_pt", "pt resolution of truth track w/ first stage htt track passing Chi^2 cut matched using truth matching",   100, (-1.)*ResHistLimit_pt, ResHistLimit_pt); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_pt));
    h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_eta = new TH1F("h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_eta", "eta resolution of truth track w/ first stage htt track passing Chi^2 cut matched using truth matching",100, (-1.)*ResHistLimit_eta, ResHistLimit_eta); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_eta));
    h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_phi = new TH1F("h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_phi", "phi resolution of truth track w/ first stage htt track passing Chi^2 cut matched using truth matching",100, (-1.)*ResHistLimit_phi, ResHistLimit_phi); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_phi));
    h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_d0 = new TH1F("h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_d0", "d0 resolution of truth track w/ first stage htt track passing Chi^2 cut matched using truth matching",   100, (-1.)*ResHistLimit_d0, ResHistLimit_d0); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_d0));
    h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_z0 = new TH1F("h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_z0", "z0 resolution of truth track w/ first stage htt track passing Chi^2 cut matched using truth matching",   100, (-1.)*ResHistLimit_z0, ResHistLimit_z0); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_z0));
    h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_qoverpt = new TH1F("h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_qoverpt","qoverpt resolution of truth track w/ first stage htt track passing Chi^2 cut matched using truth matching", 100, (-1.)*ResHistLimit_ipt, ResHistLimit_ipt); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_qoverpt));


    setHistDir(createDirName("/TruthMatchHist/TrackHist_1st/HTT_1st_vs_Truth/passChi2/ResHist/2D_Res/vspt/"));

    h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_pt_vspt = new TH2F("h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_pt_vspt", "pt resolution of truth track w/ first stage htt track passing Chi^2 cut matched vs pt using truth matching",    getNXbinsPT(), m_xbinsPT, 100, (-1.)*ResHistLimit_pt, ResHistLimit_pt); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_pt_vspt));
    h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_eta_vspt = new TH2F("h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_eta_vspt", "eta resolution of truth track w/ first stage htt track passing Chi^2 cut matched vs pt using truth matching", getNXbinsPT(), m_xbinsPT, 100, (-1.)*ResHistLimit_eta, ResHistLimit_eta); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_eta_vspt));
    h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_phi_vspt = new TH2F("h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_phi_vspt", "phi resolution of truth track w/ first stage htt track passing Chi^2 cut matched vs pt using truth matching", getNXbinsPT(), m_xbinsPT, 100, (-1.)*ResHistLimit_phi, ResHistLimit_phi); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_phi_vspt));
    h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_d0_vspt = new TH2F("h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_d0_vspt", "d0 resolution of truth track w/ first stage htt track passing Chi^2 cut matched vs pt using truth matching", getNXbinsPT(), m_xbinsPT,    100, (-1.)*ResHistLimit_d0, ResHistLimit_d0); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_d0_vspt));
    h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_z0_vspt = new TH2F("h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_z0_vspt", "z0 resolution of truth track w/ first stage htt track passing Chi^2 cut matched vs pt using truth matching", getNXbinsPT(), m_xbinsPT,    100, (-1.)*ResHistLimit_z0, ResHistLimit_z0); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_z0_vspt));
    h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_qoverpt_vspt = new TH2F("h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_qoverpt_vspt", "qoverpt resolution of truth track w/ first stage htt track passing Chi^2 cut matched vs pt using truth matching", getNXbinsPT(), m_xbinsPT, 100, (-1.)*ResHistLimit_ipt, ResHistLimit_ipt); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_qoverpt_vspt));


    setHistDir(createDirName("/TruthMatchHist/TrackHist_1st/HTT_1st_vs_Truth/passChi2/ResHist/2D_Res/vseta/"));

    h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_pt_vseta = new TH2F("h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_pt_vseta", "pt resolution of truth track w/ first stage htt track passing Chi^2 cut matched vs eta using truth matching", 50, m_etamin, m_etamax,    100, (-1.)*ResHistLimit_pt, ResHistLimit_pt); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_pt_vseta));
    h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_eta_vseta = new TH2F("h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_eta_vseta", "eta resolution of truth track w/ first stage htt track passing Chi^2 cut matched vs eta using truth matching", 50, m_etamin, m_etamax, 100, (-1.)*ResHistLimit_eta, ResHistLimit_eta); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_eta_vseta));
    h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_phi_vseta = new TH2F("h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_phi_vseta", "phi resolution of truth track w/ first stage htt track passing Chi^2 cut matched vs eta using truth matching", 50, m_etamin, m_etamax, 100, (-1.)*ResHistLimit_phi, ResHistLimit_phi); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_phi_vseta));
    h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_d0_vseta = new TH2F("h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_d0_vseta", "d0 resolution of truth track w/ first stage htt track passing Chi^2 cut matched vs eta using truth matching", 50, m_etamin, m_etamax,    100, (-1.)*ResHistLimit_d0, ResHistLimit_d0); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_d0_vseta));
    h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_z0_vseta = new TH2F("h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_z0_vseta", "z0 resolution of truth track w/ first stage htt track passing Chi^2 cut matched vs eta using truth matching", 50, m_etamin, m_etamax,    100, (-1.)*ResHistLimit_z0, ResHistLimit_z0); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_z0_vseta));
    h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_qoverpt_vseta = new TH2F("h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_qoverpt_vseta", "qoverpt resolution of truth track w/ first stage htt track passing Chi^2 cut matched vs eta using truth matching", 50, m_etamin, m_etamax, 100, (-1.)*ResHistLimit_ipt, ResHistLimit_ipt); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_qoverpt_vseta));


    setHistDir(createDirName("/TruthMatchHist/TrackHist_1st/HTT_1st_vs_Truth/afterOR/EffHist/"));

    h_HTT_1st_vs_Truth_afterOR_eff_pt = new TGraphAsymmErrors(); h_HTT_1st_vs_Truth_afterOR_eff_pt->SetNameTitle("h_HTT_1st_vs_Truth_afterOR_eff_pt","First Stage HTT vs. Truth Efficiency after overlap removal as function of truth pt;p_{T} (GeV);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_1st_vs_Truth_afterOR_eff_pt));
    h_HTT_1st_vs_Truth_afterOR_eff_eta = new TGraphAsymmErrors(); h_HTT_1st_vs_Truth_afterOR_eff_eta->SetNameTitle("h_HTT_1st_vs_Truth_afterOR_eff_eta","First Stage HTT vs. Truth Efficiency after overlap removal as function of truth eta;#eta;"); ATH_CHECK(regGraph(getHistDir(), h_HTT_1st_vs_Truth_afterOR_eff_eta));
    h_HTT_1st_vs_Truth_afterOR_eff_phi = new TGraphAsymmErrors(); h_HTT_1st_vs_Truth_afterOR_eff_phi->SetNameTitle("h_HTT_1st_vs_Truth_afterOR_eff_phi","First Stage HTT vs. Truth Efficiency after overlap removal as function of truth phi;#phi (rad);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_1st_vs_Truth_afterOR_eff_phi));
    h_HTT_1st_vs_Truth_afterOR_eff_d0 = new TGraphAsymmErrors(); h_HTT_1st_vs_Truth_afterOR_eff_d0->SetNameTitle("h_HTT_1st_vs_Truth_afterOR_eff_d0","First Stage HTT vs. Truth Efficiency after overlap removal as function of truth d0;d_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_1st_vs_Truth_afterOR_eff_d0));
    h_HTT_1st_vs_Truth_afterOR_eff_z0 = new TGraphAsymmErrors(); h_HTT_1st_vs_Truth_afterOR_eff_z0->SetNameTitle("h_HTT_1st_vs_Truth_afterOR_eff_z0","First Stage HTT vs. Truth Efficiency after overlap removal as function of truth z0;z_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_1st_vs_Truth_afterOR_eff_z0));


    setHistDir(createDirName("/TruthMatchHist/TrackHist_1st/HTT_1st_vs_Truth/afterOR/MatchedTrackHist/"));

    h_Truth_Track_HTT_1st_afterOR_Nmatched = new TH1I("h_Truth_Track_HTT_1st_afterOR_Nmatched","N. first stage HTT track matched with truth/event", 100, 0.,100.); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_HTT_1st_afterOR_Nmatched));
    h_Truth_Track_HTT_1st_afterOR_matched_pt = new TH1F("h_Truth_Track_HTT_1st_afterOR_matched_pt", "pt of truth track w/ first stage htt track after overlap removal matched", getNXbinsPT(), m_xbinsPT); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_HTT_1st_afterOR_matched_pt));
    h_Truth_Track_HTT_1st_afterOR_matched_eta = new TH1F("h_Truth_Track_HTT_1st_afterOR_matched_eta", "eta of truth track w/ first stage htt track after overlap removal matched", 50, m_etamin, m_etamax); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_HTT_1st_afterOR_matched_eta));
    h_Truth_Track_HTT_1st_afterOR_matched_phi = new TH1F("h_Truth_Track_HTT_1st_afterOR_matched_phi", "phi of truth track w/ first stage htt track after overlap removal matched", 50, m_phimin, m_phimax); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_HTT_1st_afterOR_matched_phi));
    h_Truth_Track_HTT_1st_afterOR_matched_d0 = new TH1F("h_Truth_Track_HTT_1st_afterOR_matched_d0", "d0 of truth track w/ first stage htt track after overlap removal matched",    50, m_d0Range[0], m_d0Range[1]); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_HTT_1st_afterOR_matched_d0));
    h_Truth_Track_HTT_1st_afterOR_matched_z0 = new TH1F("h_Truth_Track_HTT_1st_afterOR_matched_z0", "z0 of truth track w/ first stage htt track after overlap removal matched",    50, m_z0Range[0], m_z0Range[1]); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_HTT_1st_afterOR_matched_z0));


    setHistDir(createDirName("/TruthMatchHist/TrackHist_1st/HTT_1st_vs_Truth/afterOR/ResHist/1D_Res/"));

    h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_pt = new TH1F("h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_pt", "pt resolution of truth track w/ first stage htt track after overlap removal matched using truth matching",   100, (-1.)*ResHistLimit_pt, ResHistLimit_pt); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_pt));
    h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_eta = new TH1F("h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_eta", "eta resolution of truth track w/ first stage htt track after overlap removal matched using truth matching",100, (-1.)*ResHistLimit_eta, ResHistLimit_eta); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_eta));
    h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_phi = new TH1F("h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_phi", "phi resolution of truth track w/ first stage htt track after overlap removal matched using truth matching",100, (-1.)*ResHistLimit_phi, ResHistLimit_phi); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_phi));
    h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_d0 = new TH1F("h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_d0", "d0 resolution of truth track w/ first stage htt track after overlap removal matched using truth matching",   100, (-1.)*ResHistLimit_d0, ResHistLimit_d0); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_d0));
    h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_z0 = new TH1F("h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_z0", "z0 resolution of truth track w/ first stage htt track after overlap removal matched using truth matching",   100, (-1.)*ResHistLimit_z0, ResHistLimit_z0); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_z0));
    h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_qoverpt = new TH1F("h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_qoverpt","qoverpt resolution of truth track w/ first stage htt track after overlap removal matched using truth matching", 100, (-1.)*ResHistLimit_ipt, ResHistLimit_ipt); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_qoverpt));


    setHistDir(createDirName("/TruthMatchHist/TrackHist_1st/HTT_1st_vs_Truth/afterOR/ResHist/2D_Res/vspt/"));

    h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_pt_vspt = new TH2F("h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_pt_vspt", "pt resolution of truth track w/ first stage htt track after overlap removal matched vs pt using truth matching",    getNXbinsPT(), m_xbinsPT, 100, (-1.)*ResHistLimit_pt, ResHistLimit_pt); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_pt_vspt));
    h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_eta_vspt = new TH2F("h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_eta_vspt", "eta resolution of truth track w/ first stage htt track after overlap removal matched vs pt using truth matching", getNXbinsPT(), m_xbinsPT, 100, (-1.)*ResHistLimit_eta, ResHistLimit_eta); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_eta_vspt));
    h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_phi_vspt = new TH2F("h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_phi_vspt", "phi resolution of truth track w/ first stage htt track after overlap removal matched vs pt using truth matching", getNXbinsPT(), m_xbinsPT, 100, (-1.)*ResHistLimit_phi, ResHistLimit_phi); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_phi_vspt));
    h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_d0_vspt = new TH2F("h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_d0_vspt", "d0 resolution of truth track w/ first stage htt track after overlap removal matched vs pt using truth matching",    getNXbinsPT(), m_xbinsPT, 100, (-1.)*ResHistLimit_d0, ResHistLimit_d0); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_d0_vspt));
    h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_z0_vspt = new TH2F("h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_z0_vspt", "z0 resolution of truth track w/ first stage htt track after overlap removal matched vs pt using truth matching",    getNXbinsPT(), m_xbinsPT, 100, (-1.)*ResHistLimit_z0, ResHistLimit_z0); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_z0_vspt));
    h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_qoverpt_vspt = new TH2F("h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_qoverpt_vspt", "qoverpt resolution of truth track w/ first stage htt track after overlap removal matched vs pt using truth matching", getNXbinsPT(), m_xbinsPT, 100, (-1.)*ResHistLimit_ipt, ResHistLimit_ipt); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_qoverpt_vspt));


    setHistDir(createDirName("/TruthMatchHist/TrackHist_1st/HTT_1st_vs_Truth/afterOR/ResHist/2D_Res/vseta/"));

    h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_pt_vseta = new TH2F("h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_pt_vseta", "pt resolution of truth track w/ first stage htt track after overlap removal matched vs eta using truth matching",    50, m_etamin, m_etamax, 100, (-1.)*ResHistLimit_pt, ResHistLimit_pt); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_pt_vseta));
    h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_eta_vseta = new TH2F("h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_eta_vseta", "eta resolution of truth track w/ first stage htt track after overlap removal matched vs eta using truth matching", 50, m_etamin, m_etamax, 100, (-1.)*ResHistLimit_eta, ResHistLimit_eta); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_eta_vseta));
    h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_phi_vseta = new TH2F("h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_phi_vseta", "phi resolution of truth track w/ first stage htt track after overlap removal matched vs eta using truth matching", 50, m_etamin, m_etamax, 100, (-1.)*ResHistLimit_phi, ResHistLimit_phi); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_phi_vseta));
    h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_d0_vseta = new TH2F("h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_d0_vseta", "d0 resolution of truth track w/ first stage htt track after overlap removal matched vs eta using truth matching",    50, m_etamin, m_etamax, 100, (-1.)*ResHistLimit_d0, ResHistLimit_d0); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_d0_vseta));
    h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_z0_vseta = new TH2F("h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_z0_vseta", "z0 resolution of truth track w/ first stage htt track after overlap removal matched vs eta using truth matching",    50, m_etamin, m_etamax, 100, (-1.)*ResHistLimit_z0, ResHistLimit_z0); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_z0_vseta));
    h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_qoverpt_vseta = new TH2F("h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_qoverpt_vseta", "qoverpt resolution of truth track w/ first stage htt track after overlap removal matched vs eta using truth matching", 50, m_etamin, m_etamax, 100, (-1.)*ResHistLimit_ipt, ResHistLimit_ipt); ATH_CHECK(regHist(getHistDir(), h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_qoverpt_vseta));


    setHistDir(createDirName("/TruthMatchHist/TrackHist_1st/HTT_1st_vs_Offline/All/EffHist/"));

    h_HTT_1st_vs_Offline_eff_pt = new TGraphAsymmErrors(); h_HTT_1st_vs_Offline_eff_pt->SetNameTitle("h_HTT_1st_vs_Offline_eff_pt","HTT vs. Offline Efficiency as function of truth pt;p_{T} (GeV);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_1st_vs_Offline_eff_pt));
    h_HTT_1st_vs_Offline_eff_eta = new TGraphAsymmErrors(); h_HTT_1st_vs_Offline_eff_eta->SetNameTitle("h_HTT_1st_vs_Offline_eff_eta","HTT vs. Offline Efficiency as function of truth eta;#eta;"); ATH_CHECK(regGraph(getHistDir(), h_HTT_1st_vs_Offline_eff_eta));
    h_HTT_1st_vs_Offline_eff_phi = new TGraphAsymmErrors(); h_HTT_1st_vs_Offline_eff_phi->SetNameTitle("h_HTT_1st_vs_Offline_eff_phi","HTT vs. Offline Efficiency as function of truth phi;#phi (rad);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_1st_vs_Offline_eff_phi));
    h_HTT_1st_vs_Offline_eff_d0 = new TGraphAsymmErrors(); h_HTT_1st_vs_Offline_eff_d0->SetNameTitle("h_HTT_1st_vs_Offline_eff_d0","HTT vs. Offline Efficiency as function of truth d0;d_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_1st_vs_Offline_eff_d0));
    h_HTT_1st_vs_Offline_eff_z0 = new TGraphAsymmErrors(); h_HTT_1st_vs_Offline_eff_z0->SetNameTitle("h_HTT_1st_vs_Offline_eff_z0","HTT vs. Offline Efficiency as function of truth z0;z_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_1st_vs_Offline_eff_z0));


    setHistDir(createDirName("/TruthMatchHist/TrackHist_1st/HTT_1st_vs_Offline/passChi2/EffHist/"));

    h_HTT_1st_vs_Offline_passChi2_eff_pt = new TGraphAsymmErrors(); h_HTT_1st_vs_Offline_passChi2_eff_pt->SetNameTitle("h_HTT_1st_vs_Offline_passChi2_eff_pt","HTT vs. Offline Efficiency passing Chi^2 cut as function of truth pt;p_{T} (GeV);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_1st_vs_Offline_passChi2_eff_pt));
    h_HTT_1st_vs_Offline_passChi2_eff_eta = new TGraphAsymmErrors(); h_HTT_1st_vs_Offline_passChi2_eff_eta->SetNameTitle("h_HTT_1st_vs_Offline_passChi2_eff_eta","HTT vs. Offline Efficiency passing Chi^2 cut as function of truth eta;#eta;"); ATH_CHECK(regGraph(getHistDir(), h_HTT_1st_vs_Offline_passChi2_eff_eta));
    h_HTT_1st_vs_Offline_passChi2_eff_phi = new TGraphAsymmErrors(); h_HTT_1st_vs_Offline_passChi2_eff_phi->SetNameTitle("h_HTT_1st_vs_Offline_passChi2_eff_phi","HTT vs. Offline Efficiency passing Chi^2 cut as function of truth phi;#phi (rad);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_1st_vs_Offline_passChi2_eff_phi));
    h_HTT_1st_vs_Offline_passChi2_eff_d0 = new TGraphAsymmErrors(); h_HTT_1st_vs_Offline_passChi2_eff_d0->SetNameTitle("h_HTT_1st_vs_Offline_passChi2_eff_d0","HTT vs. Offline Efficiency passing Chi^2 cut as function of truth d0;d_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_1st_vs_Offline_passChi2_eff_d0));
    h_HTT_1st_vs_Offline_passChi2_eff_z0 = new TGraphAsymmErrors(); h_HTT_1st_vs_Offline_passChi2_eff_z0->SetNameTitle("h_HTT_1st_vs_Offline_passChi2_eff_z0","HTT vs. Offline Efficiency passing Chi^2 cut as function of truth z0;z_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_1st_vs_Offline_passChi2_eff_z0));


    setHistDir(createDirName("/TruthMatchHist/TrackHist_1st/HTT_1st_vs_Offline/afterOR/EffHist/"));

    h_HTT_1st_vs_Offline_afterOR_eff_pt = new TGraphAsymmErrors(); h_HTT_1st_vs_Offline_afterOR_eff_pt->SetNameTitle("h_HTT_1st_vs_Offline_afterOR_eff_pt","HTT vs. Offline Efficiency after overlap removal as function of truth pt;p_{T} (GeV);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_1st_vs_Offline_afterOR_eff_pt));
    h_HTT_1st_vs_Offline_afterOR_eff_eta = new TGraphAsymmErrors(); h_HTT_1st_vs_Offline_afterOR_eff_eta->SetNameTitle("h_HTT_1st_vs_Offline_afterOR_eff_eta","HTT vs. Offline Efficiency after overlap removal as function of truth eta;#eta;"); ATH_CHECK(regGraph(getHistDir(), h_HTT_1st_vs_Offline_afterOR_eff_eta));
    h_HTT_1st_vs_Offline_afterOR_eff_phi = new TGraphAsymmErrors(); h_HTT_1st_vs_Offline_afterOR_eff_phi->SetNameTitle("h_HTT_1st_vs_Offline_afterOR_eff_phi","HTT vs. Offline Efficiency after overlap removal as function of truth phi;#phi (rad);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_1st_vs_Offline_afterOR_eff_phi));
    h_HTT_1st_vs_Offline_afterOR_eff_d0 = new TGraphAsymmErrors(); h_HTT_1st_vs_Offline_afterOR_eff_d0->SetNameTitle("h_HTT_1st_vs_Offline_afterOR_eff_d0","HTT vs. Offline Efficiency after overlap removal as function of truth d0;d_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_1st_vs_Offline_afterOR_eff_d0));
    h_HTT_1st_vs_Offline_afterOR_eff_z0 = new TGraphAsymmErrors(); h_HTT_1st_vs_Offline_afterOR_eff_z0->SetNameTitle("h_HTT_1st_vs_Offline_afterOR_eff_z0","HTT vs. Offline Efficiency after overlap removal as function of truth z0;z_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_1st_vs_Offline_afterOR_eff_z0));


    setHistDir(createDirName("/TruthMatchHist/FakeRateHist_1st/"));

    h_ratio_HTT_1st_Truth          = new TH1F("h_ratio_HTT_1st_Truth", "# First Stage HTT Tracks / # Truth Tracks, per event;ratio(nHTT/nTruth);# events", 100, -0.5, 100-0.5); ATH_CHECK(regHist(getHistDir(), h_ratio_HTT_1st_Truth));
    h_ratio_HTT_1st_passChi2_Truth = new TH1F("h_ratio_HTT_1st_passChi2_Truth", "# First Stage HTT Tracks passing Chi^2 cut / # Truth Tracks, per event;ratio(nHTT_1st_passChi2/nTruth);# events", 100, -0.5, 100-0.5); ATH_CHECK(regHist(getHistDir(), h_ratio_HTT_1st_passChi2_Truth));
    h_ratio_HTT_1st_afterOR_Truth  = new TH1F("h_ratio_HTT_1st_afterOR_Truth", "# First Stage HTT Tracks after overlap removal / # Truth Tracks, per event;ratio(nHTT_1st_afterOR/nTruth);# events", 100, -0.5, 100-0.5); ATH_CHECK(regHist(getHistDir(), h_ratio_HTT_1st_afterOR_Truth));

    h_ratio_HTT_1st_Offline = new TH1F("h_ratio_HTT_1st_Offline", "# First Stage HTT Tracks / # Offline Tracks, per event;ratio(nHTT/nOffline);# events", 100, -0.5, 100-0.5); ATH_CHECK(regHist(getHistDir(), h_ratio_HTT_1st_Offline));
    h_ratio_HTT_1st_passChi2_Offline = new TH1F("h_ratio_HTT_1st_passChi2_Offline", "# First Stage HTT Tracks passing Chi^2 cut / # Offline Tracks, per event;ratio(nHTT_1st_passChi2/nOffline);# events", 100, -0.5, 100-0.5); ATH_CHECK(regHist(getHistDir(), h_ratio_HTT_1st_passChi2_Offline));
    h_ratio_HTT_1st_afterOR_Offline = new TH1F("h_ratio_HTT_1st_afterOR_Offline", "# First Stage HTT Tracks after overlap removal / # Offline Tracks, per event;ratio(nHTT_1st_afterOR/nOffline);# events", 100, -0.5, 100-0.5); ATH_CHECK(regHist(getHistDir(), h_ratio_HTT_1st_afterOR_Offline));


    clearHistDir();

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTPerformanceMonitorTool::bookTruthMatchHistograms_2nd()
{
    setHistDir(createDirName("/TruthMatchHist/RoadHist_2nd/HTT_2nd_vs_Truth/EffHist/"));

    h_HTT_2nd_vs_Truth_Road_eff_pt = new TGraphAsymmErrors(); h_HTT_2nd_vs_Truth_Road_eff_pt->SetNameTitle("h_HTT_2nd_vs_Truth_Road_eff_pt","Second Stage HTT vs. Truth Road Efficiency as function of truth pt using truth matching;p_{T} (GeV);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_2nd_vs_Truth_Road_eff_pt));
    h_HTT_2nd_vs_Truth_Road_eff_eta = new TGraphAsymmErrors(); h_HTT_2nd_vs_Truth_Road_eff_eta->SetNameTitle("h_HTT_2nd_vs_Truth_Road_eff_eta","Second Stage HTT vs. Truth Road Efficiency as function of truth eta using truth matching;#eta;"); ATH_CHECK(regGraph(getHistDir(), h_HTT_2nd_vs_Truth_Road_eff_eta));
    h_HTT_2nd_vs_Truth_Road_eff_phi = new TGraphAsymmErrors(); h_HTT_2nd_vs_Truth_Road_eff_phi->SetNameTitle("h_HTT_2nd_vs_Truth_Road_eff_phi","Second Stage HTT vs. Truth Road Efficiency as function of truth phi using truth matching;#phi (rad);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_2nd_vs_Truth_Road_eff_phi));
    h_HTT_2nd_vs_Truth_Road_eff_d0 = new TGraphAsymmErrors(); h_HTT_2nd_vs_Truth_Road_eff_d0->SetNameTitle("h_HTT_2nd_vs_Truth_Road_eff_d0","Second Stage HTT vs. Truth Road Efficiency as function of truth d0 using truth matching;d_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_2nd_vs_Truth_Road_eff_d0));
    h_HTT_2nd_vs_Truth_Road_eff_z0 = new TGraphAsymmErrors(); h_HTT_2nd_vs_Truth_Road_eff_z0->SetNameTitle("h_HTT_2nd_vs_Truth_Road_eff_z0","Second Stage HTT vs. Truth Road Efficiency as function of truth z0 using truth matching;z_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_2nd_vs_Truth_Road_eff_z0));


    setHistDir(createDirName("/TruthMatchHist/RoadHist_2nd/HTT_2nd_vs_Truth/MatchedTrackHist/"));

    h_Truth_Track_Road_2nd_matched_pt = new TH1F("h_Truth_Track_Road_2nd_matched_pt", "pt of truth track w/ second stage road matched using truth matching", getNXbinsPT(), m_xbinsPT); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_Road_2nd_matched_pt));
    h_Truth_Track_Road_2nd_matched_eta = new TH1F("h_Truth_Track_Road_2nd_matched_eta", "eta of truth track w/ second stage road matched using truth matching", 50, m_etamin, m_etamax); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_Road_2nd_matched_eta));
    h_Truth_Track_Road_2nd_matched_phi = new TH1F("h_Truth_Track_Road_2nd_matched_phi", "phi of truth track w/ second stage road matched using truth matching", 50, m_phimin, m_phimax); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_Road_2nd_matched_phi));
    h_Truth_Track_Road_2nd_matched_d0 = new TH1F("h_Truth_Track_Road_2nd_matched_d0", "d0 of truth track w/ second stage road matched using truth matching", 50, m_d0Range[0], m_d0Range[1]); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_Road_2nd_matched_d0));
    h_Truth_Track_Road_2nd_matched_z0 = new TH1F("h_Truth_Track_Road_2nd_matched_z0", "z0 of truth track w/ second stage road matched using truth matching", 50, m_z0Range[0], m_z0Range[1]); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_Road_2nd_matched_z0));


    setHistDir(createDirName("/TruthMatchHist/RoadHist_2nd/HTT_2nd_vs_Offline/EffHist/"));

    h_HTT_2nd_vs_Offline_Road_eff_pt = new TGraphAsymmErrors(); h_HTT_2nd_vs_Offline_Road_eff_pt->SetNameTitle("h_HTT_2nd_vs_Offline_Road_eff_pt","Second Stage HTT vs. Offline Road Efficiency as function of truth pt using truth matching;p_{T} (GeV);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_2nd_vs_Offline_Road_eff_pt));
    h_HTT_2nd_vs_Offline_Road_eff_eta = new TGraphAsymmErrors(); h_HTT_2nd_vs_Offline_Road_eff_eta->SetNameTitle("h_HTT_2nd_vs_Offline_Road_eff_eta","Second Stage HTT vs. Offline Road Efficiency as function of truth eta using truth matching;#eta;"); ATH_CHECK(regGraph(getHistDir(), h_HTT_2nd_vs_Offline_Road_eff_eta));
    h_HTT_2nd_vs_Offline_Road_eff_phi = new TGraphAsymmErrors(); h_HTT_2nd_vs_Offline_Road_eff_phi->SetNameTitle("h_HTT_2nd_vs_Offline_Road_eff_phi","Second Stage HTT vs. Offline Road Efficiency as function of truth phi using truth matching;#phi (rad);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_2nd_vs_Offline_Road_eff_phi));
    h_HTT_2nd_vs_Offline_Road_eff_d0 = new TGraphAsymmErrors(); h_HTT_2nd_vs_Offline_Road_eff_d0->SetNameTitle("h_HTT_2nd_vs_Offline_Road_eff_d0","Second Stage HTT vs. Offline Road Efficiency as function of truth d0 using truth matching;d_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_2nd_vs_Offline_Road_eff_d0));
    h_HTT_2nd_vs_Offline_Road_eff_z0 = new TGraphAsymmErrors(); h_HTT_2nd_vs_Offline_Road_eff_z0->SetNameTitle("h_HTT_2nd_vs_Offline_Road_eff_z0","Second Stage HTT vs. Offline Road Efficiency as function of truth z0 using truth matching;z_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_2nd_vs_Offline_Road_eff_z0));


    setHistDir(createDirName("/TruthMatchHist/TrackHist_2nd/HTT_2nd_vs_Truth/All/EffHist/"));

    h_HTT_2nd_vs_Truth_eff_pt = new TGraphAsymmErrors(); h_HTT_2nd_vs_Truth_eff_pt->SetNameTitle("h_HTT_2nd_vs_Truth_eff_pt","Second Stage HTT vs. Truth Efficiency as function of truth pt;p_{T} (GeV);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_2nd_vs_Truth_eff_pt));
    h_HTT_2nd_vs_Truth_eff_eta = new TGraphAsymmErrors(); h_HTT_2nd_vs_Truth_eff_eta->SetNameTitle("h_HTT_2nd_vs_Truth_eff_eta","Second Stage HTT vs. Truth Efficiency as function of truth eta;#eta;"); ATH_CHECK(regGraph(getHistDir(), h_HTT_2nd_vs_Truth_eff_eta));
    h_HTT_2nd_vs_Truth_eff_phi = new TGraphAsymmErrors(); h_HTT_2nd_vs_Truth_eff_phi->SetNameTitle("h_HTT_2nd_vs_Truth_eff_phi","Second Stage HTT vs. Truth Efficiency as function of truth phi;#phi (rad);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_2nd_vs_Truth_eff_phi));
    h_HTT_2nd_vs_Truth_eff_d0 = new TGraphAsymmErrors(); h_HTT_2nd_vs_Truth_eff_d0->SetNameTitle("h_HTT_2nd_vs_Truth_eff_d0","Second Stage HTT vs. Truth Efficiency as function of truth d0;d_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_2nd_vs_Truth_eff_d0));
    h_HTT_2nd_vs_Truth_eff_z0 = new TGraphAsymmErrors(); h_HTT_2nd_vs_Truth_eff_z0->SetNameTitle("h_HTT_2nd_vs_Truth_eff_z0","Second Stage HTT vs. Truth Efficiency as function of truth z0;z_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_2nd_vs_Truth_eff_z0));


    setHistDir(createDirName("/TruthMatchHist/TrackHist_2nd/HTT_2nd_vs_Truth/All/MatchedTrackHist/"));

    h_Truth_Track_HTT_2nd_matched_pt = new TH1F("h_Truth_Track_HTT_2nd_matched_pt", "pt of truth track w/ second stage htt track matched", getNXbinsPT(), m_xbinsPT); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_HTT_2nd_matched_pt));
    h_Truth_Track_HTT_2nd_matched_eta = new TH1F("h_Truth_Track_HTT_2nd_matched_eta", "eta of truth track w/ second stage htt track matched", 50, m_etamin, m_etamax); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_HTT_2nd_matched_eta));
    h_Truth_Track_HTT_2nd_matched_phi = new TH1F("h_Truth_Track_HTT_2nd_matched_phi", "phi of truth track w/ second stage htt track matched", 50, m_phimin, m_phimax); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_HTT_2nd_matched_phi));
    h_Truth_Track_HTT_2nd_matched_d0 = new TH1F("h_Truth_Track_HTT_2nd_matched_d0", "d0 of truth track w/ second stage htt track matched", 50, m_d0Range[0], m_d0Range[1]); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_HTT_2nd_matched_d0));
    h_Truth_Track_HTT_2nd_matched_z0 = new TH1F("h_Truth_Track_HTT_2nd_matched_z0", "z0 of truth track w/ second stage htt track matched", 50, m_z0Range[0], m_z0Range[1]); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_HTT_2nd_matched_z0));


    setHistDir(createDirName("/TruthMatchHist/TrackHist_2nd/HTT_2nd_vs_Truth/All/ResHist/1D_Res/"));

    h_HTT_2nd_vs_Truth_Res_truthmatch_pt = new TH1F("h_HTT_2nd_vs_Truth_Res_truthmatch_pt", "pt resolution of truth track w/ second stage htt track matched using truth matching",   100, (-1.)*ResHistLimit_pt2, ResHistLimit_pt2); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_Res_truthmatch_pt));
    h_HTT_2nd_vs_Truth_Res_truthmatch_eta = new TH1F("h_HTT_2nd_vs_Truth_Res_truthmatch_eta", "eta resolution of truth track w/ second stage htt track matched using truth matching",100, (-1.)*ResHistLimit_eta2, ResHistLimit_eta2); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_Res_truthmatch_eta));
    h_HTT_2nd_vs_Truth_Res_truthmatch_phi = new TH1F("h_HTT_2nd_vs_Truth_Res_truthmatch_phi", "phi resolution of truth track w/ second stage htt track matched using truth matching",100, (-1.)*ResHistLimit_phi2, ResHistLimit_phi2); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_Res_truthmatch_phi));
    h_HTT_2nd_vs_Truth_Res_truthmatch_d0 = new TH1F("h_HTT_2nd_vs_Truth_Res_truthmatch_d0", "d0 resolution of truth track w/ second stage htt track matched using truth matching",   100, (-1.)*ResHistLimit_d02, ResHistLimit_d02); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_Res_truthmatch_d0));
    h_HTT_2nd_vs_Truth_Res_truthmatch_z0 = new TH1F("h_HTT_2nd_vs_Truth_Res_truthmatch_z0", "z0 resolution of truth track w/ second stage htt track matched using truth matching",   100, (-1.)*ResHistLimit_z02, ResHistLimit_z02); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_Res_truthmatch_z0));
    h_HTT_2nd_vs_Truth_Res_truthmatch_qoverpt = new TH1F("h_HTT_2nd_vs_Truth_Res_truthmatch_qoverpt","qoverpt resolution of truth track w/ second stage htt track matched using truth matching", 100, (-1.)*ResHistLimit_ipt2, ResHistLimit_ipt2); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_Res_truthmatch_qoverpt));


    setHistDir(createDirName("/TruthMatchHist/TrackHist_2nd/HTT_2nd_vs_Truth/All/ResHist/2D_Res/vspt/"));

    h_HTT_2nd_vs_Truth_Res_truthmatch_pt_vspt = new TH2F("h_HTT_2nd_vs_Truth_Res_truthmatch_pt_vspt", "pt resolution of truth track w/ second stage htt track matched vs pt using truth matching",    getNXbinsPT(), m_xbinsPT, 100, (-1.)*ResHistLimit_pt2, ResHistLimit_pt2); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_Res_truthmatch_pt_vspt));
    h_HTT_2nd_vs_Truth_Res_truthmatch_eta_vspt = new TH2F("h_HTT_2nd_vs_Truth_Res_truthmatch_eta_vspt", "eta resolution of truth track w/ second stage htt track matched vs pt using truth matching", getNXbinsPT(), m_xbinsPT, 100, (-1.)*ResHistLimit_eta2, ResHistLimit_eta2); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_Res_truthmatch_eta_vspt));
    h_HTT_2nd_vs_Truth_Res_truthmatch_phi_vspt = new TH2F("h_HTT_2nd_vs_Truth_Res_truthmatch_phi_vspt", "phi resolution of truth track w/ second stage htt track matched vs pt using truth matching", getNXbinsPT(), m_xbinsPT, 100, (-1.)*ResHistLimit_phi2, ResHistLimit_phi2); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_Res_truthmatch_phi_vspt));
    h_HTT_2nd_vs_Truth_Res_truthmatch_d0_vspt = new TH2F("h_HTT_2nd_vs_Truth_Res_truthmatch_d0_vspt", "d0 resolution of truth track w/ second stage htt track matched vs pt using truth matching",    getNXbinsPT(), m_xbinsPT, 100, (-1.)*ResHistLimit_d02, ResHistLimit_d02); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_Res_truthmatch_d0_vspt));
    h_HTT_2nd_vs_Truth_Res_truthmatch_z0_vspt = new TH2F("h_HTT_2nd_vs_Truth_Res_truthmatch_z0_vspt", "z0 resolution of truth track w/ second stage htt track matched vs pt using truth matching",    getNXbinsPT(), m_xbinsPT, 100, (-1.)*ResHistLimit_z02, ResHistLimit_z02); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_Res_truthmatch_z0_vspt));
    h_HTT_2nd_vs_Truth_Res_truthmatch_qoverpt_vspt = new TH2F("h_HTT_2nd_vs_Truth_Res_truthmatch_qoverpt_vspt", "qoverpt resolution of truth track w/ second stage htt track matched vs pt using truth matching", getNXbinsPT(), m_xbinsPT, 100, (-1.)*ResHistLimit_ipt2, ResHistLimit_ipt2); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_Res_truthmatch_qoverpt_vspt));


    setHistDir(createDirName("/TruthMatchHist/TrackHist_2nd/HTT_2nd_vs_Truth/All/ResHist/2D_Res/vseta/"));

    h_HTT_2nd_vs_Truth_Res_truthmatch_pt_vseta = new TH2F("h_HTT_2nd_vs_Truth_Res_truthmatch_pt_vseta", "pt resolution of truth track w/ second stage htt track matched vs eta using truth matching",    50, m_etamin, m_etamax, 100, (-1.)*ResHistLimit_pt2, ResHistLimit_pt2); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_Res_truthmatch_pt_vseta));
    h_HTT_2nd_vs_Truth_Res_truthmatch_eta_vseta = new TH2F("h_HTT_2nd_vs_Truth_Res_truthmatch_eta_vseta", "eta resolution of truth track w/ second stage htt track matched vs eta using truth matching", 50, m_etamin, m_etamax, 100, (-1.)*ResHistLimit_eta2, ResHistLimit_eta2); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_Res_truthmatch_eta_vseta));
    h_HTT_2nd_vs_Truth_Res_truthmatch_phi_vseta = new TH2F("h_HTT_2nd_vs_Truth_Res_truthmatch_phi_vseta", "phi resolution of truth track w/ second stage htt track matched vs eta using truth matching", 50, m_etamin, m_etamax, 100, (-1.)*ResHistLimit_phi2, ResHistLimit_phi2); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_Res_truthmatch_phi_vseta));
    h_HTT_2nd_vs_Truth_Res_truthmatch_d0_vseta = new TH2F("h_HTT_2nd_vs_Truth_Res_truthmatch_d0_vseta", "d0 resolution of truth track w/ second stage htt track matched vs eta using truth matching",    50, m_etamin, m_etamax, 100, (-1.)*ResHistLimit_d02, ResHistLimit_d02); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_Res_truthmatch_d0_vseta));
    h_HTT_2nd_vs_Truth_Res_truthmatch_z0_vseta = new TH2F("h_HTT_2nd_vs_Truth_Res_truthmatch_z0_vseta", "z0 resolution of truth track w/ second stage htt track matched vs eta using truth matching",    50, m_etamin, m_etamax, 100, (-1.)*ResHistLimit_z02, ResHistLimit_z02); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_Res_truthmatch_z0_vseta));
    h_HTT_2nd_vs_Truth_Res_truthmatch_qoverpt_vseta = new TH2F("h_HTT_2nd_vs_Truth_Res_truthmatch_qoverpt_vseta", "qoverpt resolution of truth track w/ second stage htt track matched vs eta using truth matching", 50, m_etamin, m_etamax, 100, (-1.)*ResHistLimit_ipt2, ResHistLimit_ipt2); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_Res_truthmatch_qoverpt_vseta));


    setHistDir(createDirName("/TruthMatchHist/TrackHist_2nd/HTT_2nd_vs_Truth/passChi2/EffHist/"));

    h_HTT_2nd_vs_Truth_passChi2_eff_pt = new TGraphAsymmErrors(); h_HTT_2nd_vs_Truth_passChi2_eff_pt->SetNameTitle("h_HTT_2nd_vs_Truth_passChi2_eff_pt","Second Stage HTT vs. Truth Efficiency passing Chi^2 cut as function of truth pt;p_{T} (GeV);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_2nd_vs_Truth_passChi2_eff_pt));
    h_HTT_2nd_vs_Truth_passChi2_eff_eta = new TGraphAsymmErrors(); h_HTT_2nd_vs_Truth_passChi2_eff_eta->SetNameTitle("h_HTT_2nd_vs_Truth_passChi2_eff_eta","Second Stage HTT vs. Truth Efficiency passing Chi^2 cut as function of truth eta;#eta;"); ATH_CHECK(regGraph(getHistDir(), h_HTT_2nd_vs_Truth_passChi2_eff_eta));
    h_HTT_2nd_vs_Truth_passChi2_eff_phi = new TGraphAsymmErrors(); h_HTT_2nd_vs_Truth_passChi2_eff_phi->SetNameTitle("h_HTT_2nd_vs_Truth_passChi2_eff_phi","Second Stage HTT vs. Truth Efficiency passing Chi^2 cut as function of truth phi;#phi (rad);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_2nd_vs_Truth_passChi2_eff_phi));
    h_HTT_2nd_vs_Truth_passChi2_eff_d0 = new TGraphAsymmErrors(); h_HTT_2nd_vs_Truth_passChi2_eff_d0->SetNameTitle("h_HTT_2nd_vs_Truth_passChi2_eff_d0","Second Stage HTT vs. Truth Efficiency passing Chi^2 cut as function of truth d0;d_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_2nd_vs_Truth_passChi2_eff_d0));
    h_HTT_2nd_vs_Truth_passChi2_eff_z0 = new TGraphAsymmErrors(); h_HTT_2nd_vs_Truth_passChi2_eff_z0->SetNameTitle("h_HTT_2nd_vs_Truth_passChi2_eff_z0","Second Stage HTT vs. Truth Efficiency passing Chi^2 cut as function of truth z0;z_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_2nd_vs_Truth_passChi2_eff_z0));


    setHistDir(createDirName("/TruthMatchHist/TrackHist_2nd/HTT_2nd_vs_Truth/passChi2/MatchedTrackHist/"));

    h_Truth_Track_HTT_2nd_passChi2_matched_pt = new TH1F("h_Truth_Track_HTT_2nd_passChi2_matched_pt", "pt of truth track w/ second stage htt track passing Chi^2 cut matched", getNXbinsPT(), m_xbinsPT); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_HTT_2nd_passChi2_matched_pt));
    h_Truth_Track_HTT_2nd_passChi2_matched_eta = new TH1F("h_Truth_Track_HTT_2nd_passChi2_matched_eta", "eta of truth track w/ second stage htt track passing Chi^2 cut matched", 50, m_etamin, m_etamax); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_HTT_2nd_passChi2_matched_eta));
    h_Truth_Track_HTT_2nd_passChi2_matched_phi = new TH1F("h_Truth_Track_HTT_2nd_passChi2_matched_phi", "phi of truth track w/ second stage htt track passing Chi^2 cut matched", 50, m_phimin, m_phimax); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_HTT_2nd_passChi2_matched_phi));
    h_Truth_Track_HTT_2nd_passChi2_matched_d0 = new TH1F("h_Truth_Track_HTT_2nd_passChi2_matched_d0", "d0 of truth track w/ second stage htt track passing Chi^2 cut matched", 50, m_d0Range[0], m_d0Range[1]); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_HTT_2nd_passChi2_matched_d0));
    h_Truth_Track_HTT_2nd_passChi2_matched_z0 = new TH1F("h_Truth_Track_HTT_2nd_passChi2_matched_z0", "z0 of truth track w/ second stage htt track passing Chi^2 cut matched", 50, m_z0Range[0], m_z0Range[1]); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_HTT_2nd_passChi2_matched_z0));


    setHistDir(createDirName("/TruthMatchHist/TrackHist_2nd/HTT_2nd_vs_Truth/passChi2/ResHist/1D_Res/"));

    h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_pt = new TH1F("h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_pt", "pt resolution of truth track w/ second stage htt track passing Chi^2 cut matched using truth matching",   100, (-1.)*ResHistLimit_pt2, ResHistLimit_pt2); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_pt));
    h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_eta = new TH1F("h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_eta", "eta resolution of truth track w/ second stage htt track passing Chi^2 cut matched using truth matching",100, (-1.)*ResHistLimit_eta2, ResHistLimit_eta2); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_eta));
    h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_phi = new TH1F("h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_phi", "phi resolution of truth track w/ second stage htt track passing Chi^2 cut matched using truth matching",100, (-1.)*ResHistLimit_phi2, ResHistLimit_phi2); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_phi));
    h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_d0 = new TH1F("h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_d0", "d0 resolution of truth track w/ second stage htt track passing Chi^2 cut matched using truth matching",   100, (-1.)*ResHistLimit_d02, ResHistLimit_d02); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_d0));
    h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_z0 = new TH1F("h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_z0", "z0 resolution of truth track w/ second stage htt track passing Chi^2 cut matched using truth matching",   100, (-1.)*ResHistLimit_z02, ResHistLimit_z02); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_z0));
    h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_qoverpt = new TH1F("h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_qoverpt","qoverpt resolution of truth track w/ second stage htt track passing Chi^2 cut matched using truth matching", 100, (-1.)*ResHistLimit_ipt2, ResHistLimit_ipt2); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_qoverpt));


    setHistDir(createDirName("/TruthMatchHist/TrackHist_2nd/HTT_2nd_vs_Truth/passChi2/ResHist/2D_Res/vspt/"));

    h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_pt_vspt = new TH2F("h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_pt_vspt", "pt resolution of truth track w/ second stage htt track passing Chi^2 cut matched vs pt using truth matching",    getNXbinsPT(), m_xbinsPT, 100, (-1.)*ResHistLimit_pt2, ResHistLimit_pt2); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_pt_vspt));
    h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_eta_vspt = new TH2F("h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_eta_vspt", "eta resolution of truth track w/ second stage htt track passing Chi^2 cut matched vs pt using truth matching", getNXbinsPT(), m_xbinsPT, 100, (-1.)*ResHistLimit_eta2, ResHistLimit_eta2); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_eta_vspt));
    h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_phi_vspt = new TH2F("h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_phi_vspt", "phi resolution of truth track w/ second stage htt track passing Chi^2 cut matched vs pt using truth matching", getNXbinsPT(), m_xbinsPT, 100, (-1.)*ResHistLimit_phi2, ResHistLimit_phi2); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_phi_vspt));
    h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_d0_vspt = new TH2F("h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_d0_vspt", "d0 resolution of truth track w/ second stage htt track passing Chi^2 cut matched vs pt using truth matching", getNXbinsPT(), m_xbinsPT,    100, (-1.)*ResHistLimit_d02, ResHistLimit_d02); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_d0_vspt));
    h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_z0_vspt = new TH2F("h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_z0_vspt", "z0 resolution of truth track w/ second stage htt track passing Chi^2 cut matched vs pt using truth matching", getNXbinsPT(), m_xbinsPT,    100, (-1.)*ResHistLimit_z02, ResHistLimit_z02); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_z0_vspt));
    h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_qoverpt_vspt = new TH2F("h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_qoverpt_vspt", "qoverpt resolution of truth track w/ second stage htt track passing Chi^2 cut matched vs pt using truth matching", getNXbinsPT(), m_xbinsPT, 100, (-1.)*ResHistLimit_ipt2, ResHistLimit_ipt2); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_qoverpt_vspt));


    setHistDir(createDirName("/TruthMatchHist/TrackHist_2nd/HTT_2nd_vs_Truth/passChi2/ResHist/2D_Res/vseta/"));

    h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_pt_vseta = new TH2F("h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_pt_vseta", "pt resolution of truth track w/ second stage htt track passing Chi^2 cut matched vs eta using truth matching", 50, m_etamin, m_etamax,    100, (-1.)*ResHistLimit_pt2, ResHistLimit_pt2); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_pt_vseta));
    h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_eta_vseta = new TH2F("h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_eta_vseta", "eta resolution of truth track w/ second stage htt track passing Chi^2 cut matched vs eta using truth matching", 50, m_etamin, m_etamax, 100, (-1.)*ResHistLimit_eta2, ResHistLimit_eta2); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_eta_vseta));
    h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_phi_vseta = new TH2F("h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_phi_vseta", "phi resolution of truth track w/ second stage htt track passing Chi^2 cut matched vs eta using truth matching", 50, m_etamin, m_etamax, 100, (-1.)*ResHistLimit_phi2, ResHistLimit_phi2); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_phi_vseta));
    h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_d0_vseta = new TH2F("h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_d0_vseta", "d0 resolution of truth track w/ second stage htt track passing Chi^2 cut matched vs eta using truth matching", 50, m_etamin, m_etamax,    100, (-1.)*ResHistLimit_d02, ResHistLimit_d02); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_d0_vseta));
    h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_z0_vseta = new TH2F("h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_z0_vseta", "z0 resolution of truth track w/ second stage htt track passing Chi^2 cut matched vs eta using truth matching", 50, m_etamin, m_etamax,    100, (-1.)*ResHistLimit_z02, ResHistLimit_z02); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_z0_vseta));
    h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_qoverpt_vseta = new TH2F("h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_qoverpt_vseta", "qoverpt resolution of truth track w/ second stage htt track passing Chi^2 cut matched vs eta using truth matching", 50, m_etamin, m_etamax, 100, (-1.)*ResHistLimit_ipt2, ResHistLimit_ipt2); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_qoverpt_vseta));


    setHistDir(createDirName("/TruthMatchHist/TrackHist_2nd/HTT_2nd_vs_Truth/afterOR/EffHist/"));

    h_HTT_2nd_vs_Truth_afterOR_eff_pt = new TGraphAsymmErrors(); h_HTT_2nd_vs_Truth_afterOR_eff_pt->SetNameTitle("h_HTT_2nd_vs_Truth_afterOR_eff_pt","Second Stage HTT vs. Truth Efficiency after overlap removal as function of truth pt;p_{T} (GeV);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_2nd_vs_Truth_afterOR_eff_pt));
    h_HTT_2nd_vs_Truth_afterOR_eff_eta = new TGraphAsymmErrors(); h_HTT_2nd_vs_Truth_afterOR_eff_eta->SetNameTitle("h_HTT_2nd_vs_Truth_afterOR_eff_eta","Second Stage HTT vs. Truth Efficiency after overlap removal as function of truth eta;#eta;"); ATH_CHECK(regGraph(getHistDir(), h_HTT_2nd_vs_Truth_afterOR_eff_eta));
    h_HTT_2nd_vs_Truth_afterOR_eff_phi = new TGraphAsymmErrors(); h_HTT_2nd_vs_Truth_afterOR_eff_phi->SetNameTitle("h_HTT_2nd_vs_Truth_afterOR_eff_phi","Second Stage HTT vs. Truth Efficiency after overlap removal as function of truth phi;#phi (rad);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_2nd_vs_Truth_afterOR_eff_phi));
    h_HTT_2nd_vs_Truth_afterOR_eff_d0 = new TGraphAsymmErrors(); h_HTT_2nd_vs_Truth_afterOR_eff_d0->SetNameTitle("h_HTT_2nd_vs_Truth_afterOR_eff_d0","Second Stage HTT vs. Truth Efficiency after overlap removal as function of truth d0;d_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_2nd_vs_Truth_afterOR_eff_d0));
    h_HTT_2nd_vs_Truth_afterOR_eff_z0 = new TGraphAsymmErrors(); h_HTT_2nd_vs_Truth_afterOR_eff_z0->SetNameTitle("h_HTT_2nd_vs_Truth_afterOR_eff_z0","Second Stage HTT vs. Truth Efficiency after overlap removal as function of truth z0;z_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_2nd_vs_Truth_afterOR_eff_z0));


    setHistDir(createDirName("/TruthMatchHist/TrackHist_2nd/HTT_2nd_vs_Truth/afterOR/MatchedTrackHist/"));

    h_Truth_Track_HTT_2nd_afterOR_Nmatched = new TH1I("h_Truth_Track_HTT_2nd_afterOR_Nmatched","N. Second Stage HTT track matched with truth/event", 100, 0.,100.); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_HTT_2nd_afterOR_Nmatched));
    h_Truth_Track_HTT_2nd_afterOR_matched_pt = new TH1F("h_Truth_Track_HTT_2nd_afterOR_matched_pt", "pt of truth track w/ second stage htt track after overlap removal matched", getNXbinsPT(), m_xbinsPT); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_HTT_2nd_afterOR_matched_pt));
    h_Truth_Track_HTT_2nd_afterOR_matched_eta = new TH1F("h_Truth_Track_HTT_2nd_afterOR_matched_eta", "eta of truth track w/ second stage htt track after overlap removal matched", 50, m_etamin, m_etamax); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_HTT_2nd_afterOR_matched_eta));
    h_Truth_Track_HTT_2nd_afterOR_matched_phi = new TH1F("h_Truth_Track_HTT_2nd_afterOR_matched_phi", "phi of truth track w/ second stage htt track after overlap removal matched", 50, m_phimin, m_phimax); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_HTT_2nd_afterOR_matched_phi));
    h_Truth_Track_HTT_2nd_afterOR_matched_d0 = new TH1F("h_Truth_Track_HTT_2nd_afterOR_matched_d0", "d0 of truth track w/ second stage htt track after overlap removal matched",    50, m_d0Range[0], m_d0Range[1]); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_HTT_2nd_afterOR_matched_d0));
    h_Truth_Track_HTT_2nd_afterOR_matched_z0 = new TH1F("h_Truth_Track_HTT_2nd_afterOR_matched_z0", "z0 of truth track w/ second stage htt track after overlap removal matched",    50, m_z0Range[0], m_z0Range[1]); ATH_CHECK(regHist(getHistDir(), h_Truth_Track_HTT_2nd_afterOR_matched_z0));


    setHistDir(createDirName("/TruthMatchHist/TrackHist_2nd/HTT_2nd_vs_Truth/afterOR/ResHist/1D_Res/"));

    h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_pt = new TH1F("h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_pt", "pt resolution of truth track w/ second stage htt track after overlap removal matched using truth matching",   100, (-1.)*ResHistLimit_pt2, ResHistLimit_pt2); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_pt));
    h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_eta = new TH1F("h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_eta", "eta resolution of truth track w/ second stage htt track after overlap removal matched using truth matching",100, (-1.)*ResHistLimit_eta2, ResHistLimit_eta2); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_eta));
    h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_phi = new TH1F("h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_phi", "phi resolution of truth track w/ second stage htt track after overlap removal matched using truth matching",100, (-1.)*ResHistLimit_phi2, ResHistLimit_phi2); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_phi));
    h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_d0 = new TH1F("h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_d0", "d0 resolution of truth track w/ second stage htt track after overlap removal matched using truth matching",   100, (-1.)*ResHistLimit_d02, ResHistLimit_d02); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_d0));
    h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_z0 = new TH1F("h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_z0", "z0 resolution of truth track w/ second stage htt track after overlap removal matched using truth matching",   100, (-1.)*ResHistLimit_z02, ResHistLimit_z02); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_z0));
    h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_qoverpt = new TH1F("h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_qoverpt","qoverpt resolution of truth track w/ second stage htt track after overlap removal matched using truth matching", 100, (-1.)*ResHistLimit_ipt2, ResHistLimit_ipt2); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_qoverpt));


    setHistDir(createDirName("/TruthMatchHist/TrackHist_2nd/HTT_2nd_vs_Truth/afterOR/ResHist/2D_Res/vspt/"));

    h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_pt_vspt = new TH2F("h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_pt_vspt", "pt resolution of truth track w/ second stage htt track after overlap removal matched vs pt using truth matching",    getNXbinsPT(), m_xbinsPT, 100, (-1.)*ResHistLimit_pt2, ResHistLimit_pt2); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_pt_vspt));
    h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_eta_vspt = new TH2F("h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_eta_vspt", "eta resolution of truth track w/ second stage htt track after overlap removal matched vs pt using truth matching", getNXbinsPT(), m_xbinsPT, 100, (-1.)*ResHistLimit_eta2, ResHistLimit_eta2); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_eta_vspt));
    h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_phi_vspt = new TH2F("h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_phi_vspt", "phi resolution of truth track w/ second stage htt track after overlap removal matched vs pt using truth matching", getNXbinsPT(), m_xbinsPT, 100, (-1.)*ResHistLimit_phi2, ResHistLimit_phi2); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_phi_vspt));
    h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_d0_vspt = new TH2F("h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_d0_vspt", "d0 resolution of truth track w/ second stage htt track after overlap removal matched vs pt using truth matching",    getNXbinsPT(), m_xbinsPT, 100, (-1.)*ResHistLimit_d02, ResHistLimit_d02); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_d0_vspt));
    h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_z0_vspt = new TH2F("h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_z0_vspt", "z0 resolution of truth track w/ second stage htt track after overlap removal matched vs pt using truth matching",    getNXbinsPT(), m_xbinsPT, 100, (-1.)*ResHistLimit_z02, ResHistLimit_z02); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_z0_vspt));
    h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_qoverpt_vspt = new TH2F("h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_qoverpt_vspt", "qoverpt resolution of truth track w/ second stage htt track after overlap removal matched vs pt using truth matching", getNXbinsPT(), m_xbinsPT, 100, (-1.)*ResHistLimit_ipt2, ResHistLimit_ipt2); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_qoverpt_vspt));


    setHistDir(createDirName("/TruthMatchHist/TrackHist_2nd/HTT_2nd_vs_Truth/afterOR/ResHist/2D_Res/vseta/"));

    h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_pt_vseta = new TH2F("h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_pt_vseta", "pt resolution of truth track w/ second stage htt track after overlap removal matched vs eta using truth matching",    50, m_etamin, m_etamax, 100, (-1.)*ResHistLimit_pt2, ResHistLimit_pt2); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_pt_vseta));
    h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_eta_vseta = new TH2F("h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_eta_vseta", "eta resolution of truth track w/ second stage htt track after overlap removal matched vs eta using truth matching", 50, m_etamin, m_etamax, 100, (-1.)*ResHistLimit_eta2, ResHistLimit_eta2); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_eta_vseta));
    h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_phi_vseta = new TH2F("h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_phi_vseta", "phi resolution of truth track w/ second stage htt track after overlap removal matched vs eta using truth matching", 50, m_etamin, m_etamax, 100, (-1.)*ResHistLimit_phi2, ResHistLimit_phi2); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_phi_vseta));
    h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_d0_vseta = new TH2F("h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_d0_vseta", "d0 resolution of truth track w/ second stage htt track after overlap removal matched vs eta using truth matching",    50, m_etamin, m_etamax, 100, (-1.)*ResHistLimit_d02, ResHistLimit_d02); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_d0_vseta));
    h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_z0_vseta = new TH2F("h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_z0_vseta", "z0 resolution of truth track w/ second stage htt track after overlap removal matched vs eta using truth matching",    50, m_etamin, m_etamax, 100, (-1.)*ResHistLimit_z02, ResHistLimit_z02); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_z0_vseta));
    h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_qoverpt_vseta = new TH2F("h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_qoverpt_vseta", "qoverpt resolution of truth track w/ second stage htt track after overlap removal matched vs eta using truth matching", 50, m_etamin, m_etamax, 100, (-1.)*ResHistLimit_ipt2, ResHistLimit_ipt2); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_qoverpt_vseta));


    setHistDir(createDirName("/TruthMatchHist/TrackHist_2nd/HTT_2nd_vs_Offline/All/EffHist/"));

    h_HTT_2nd_vs_Offline_eff_pt = new TGraphAsymmErrors(); h_HTT_2nd_vs_Offline_eff_pt->SetNameTitle("h_HTT_2nd_vs_Offline_eff_pt","Second Stage HTT vs. Offline Efficiency as function of truth pt;p_{T} (GeV);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_2nd_vs_Offline_eff_pt));
    h_HTT_2nd_vs_Offline_eff_eta = new TGraphAsymmErrors(); h_HTT_2nd_vs_Offline_eff_eta->SetNameTitle("h_HTT_2nd_vs_Offline_eff_eta","Second Stage HTT vs. Offline Efficiency as function of truth eta;#eta;"); ATH_CHECK(regGraph(getHistDir(), h_HTT_2nd_vs_Offline_eff_eta));
    h_HTT_2nd_vs_Offline_eff_phi = new TGraphAsymmErrors(); h_HTT_2nd_vs_Offline_eff_phi->SetNameTitle("h_HTT_2nd_vs_Offline_eff_phi","Second Stage HTT vs. Offline Efficiency as function of truth phi;#phi (rad);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_2nd_vs_Offline_eff_phi));
    h_HTT_2nd_vs_Offline_eff_d0 = new TGraphAsymmErrors(); h_HTT_2nd_vs_Offline_eff_d0->SetNameTitle("h_HTT_2nd_vs_Offline_eff_d0","Second Stage HTT vs. Offline Efficiency as function of truth d0;d_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_2nd_vs_Offline_eff_d0));
    h_HTT_2nd_vs_Offline_eff_z0 = new TGraphAsymmErrors(); h_HTT_2nd_vs_Offline_eff_z0->SetNameTitle("h_HTT_2nd_vs_Offline_eff_z0","Second Stage HTT vs. Offline Efficiency as function of truth z0;z_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_2nd_vs_Offline_eff_z0));


    setHistDir(createDirName("/TruthMatchHist/TrackHist_2nd/HTT_2nd_vs_Offline/passChi2/EffHist/"));

    h_HTT_2nd_vs_Offline_passChi2_eff_pt = new TGraphAsymmErrors(); h_HTT_2nd_vs_Offline_passChi2_eff_pt->SetNameTitle("h_HTT_2nd_vs_Offline_passChi2_eff_pt","Second Stage HTT vs. Offline Efficiency passing Chi^2 cut as function of truth pt;p_{T} (GeV);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_2nd_vs_Offline_passChi2_eff_pt));
    h_HTT_2nd_vs_Offline_passChi2_eff_eta = new TGraphAsymmErrors(); h_HTT_2nd_vs_Offline_passChi2_eff_eta->SetNameTitle("h_HTT_2nd_vs_Offline_passChi2_eff_eta","Second Stage HTT vs. Offline Efficiency passing Chi^2 cut as function of truth eta;#eta;"); ATH_CHECK(regGraph(getHistDir(), h_HTT_2nd_vs_Offline_passChi2_eff_eta));
    h_HTT_2nd_vs_Offline_passChi2_eff_phi = new TGraphAsymmErrors(); h_HTT_2nd_vs_Offline_passChi2_eff_phi->SetNameTitle("h_HTT_2nd_vs_Offline_passChi2_eff_phi","Second Stage HTT vs. Offline Efficiency passing Chi^2 cut as function of truth phi;#phi (rad);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_2nd_vs_Offline_passChi2_eff_phi));
    h_HTT_2nd_vs_Offline_passChi2_eff_d0 = new TGraphAsymmErrors(); h_HTT_2nd_vs_Offline_passChi2_eff_d0->SetNameTitle("h_HTT_2nd_vs_Offline_passChi2_eff_d0","Second Stage HTT vs. Offline Efficiency passing Chi^2 cut as function of truth d0;d_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_2nd_vs_Offline_passChi2_eff_d0));
    h_HTT_2nd_vs_Offline_passChi2_eff_z0 = new TGraphAsymmErrors(); h_HTT_2nd_vs_Offline_passChi2_eff_z0->SetNameTitle("h_HTT_2nd_vs_Offline_passChi2_eff_z0","Second Stage HTT vs. Offline Efficiency passing Chi^2 cut as function of truth z0;z_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_2nd_vs_Offline_passChi2_eff_z0));


    setHistDir(createDirName("/TruthMatchHist/TrackHist_2nd/HTT_2nd_vs_Offline/afterOR/EffHist/"));

    h_HTT_2nd_vs_Offline_afterOR_eff_pt = new TGraphAsymmErrors(); h_HTT_2nd_vs_Offline_afterOR_eff_pt->SetNameTitle("h_HTT_2nd_vs_Offline_afterOR_eff_pt","Second Stage HTT vs. Offline Efficiency after overlap removal as function of truth pt;p_{T} (GeV);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_2nd_vs_Offline_afterOR_eff_pt));
    h_HTT_2nd_vs_Offline_afterOR_eff_eta = new TGraphAsymmErrors(); h_HTT_2nd_vs_Offline_afterOR_eff_eta->SetNameTitle("h_HTT_2nd_vs_Offline_afterOR_eff_eta","Second Stage HTT vs. Offline Efficiency after overlap removal as function of truth eta;#eta;"); ATH_CHECK(regGraph(getHistDir(), h_HTT_2nd_vs_Offline_afterOR_eff_eta));
    h_HTT_2nd_vs_Offline_afterOR_eff_phi = new TGraphAsymmErrors(); h_HTT_2nd_vs_Offline_afterOR_eff_phi->SetNameTitle("h_HTT_2nd_vs_Offline_afterOR_eff_phi","Second Stage HTT vs. Offline Efficiency after overlap removal as function of truth phi;#phi (rad);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_2nd_vs_Offline_afterOR_eff_phi));
    h_HTT_2nd_vs_Offline_afterOR_eff_d0 = new TGraphAsymmErrors(); h_HTT_2nd_vs_Offline_afterOR_eff_d0->SetNameTitle("h_HTT_2nd_vs_Offline_afterOR_eff_d0","Second Stage HTT vs. Offline Efficiency after overlap removal as function of truth d0;d_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_2nd_vs_Offline_afterOR_eff_d0));
    h_HTT_2nd_vs_Offline_afterOR_eff_z0 = new TGraphAsymmErrors(); h_HTT_2nd_vs_Offline_afterOR_eff_z0->SetNameTitle("h_HTT_2nd_vs_Offline_afterOR_eff_z0","Second Stage HTT vs. Offline Efficiency after overlap removal as function of truth z0;z_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_2nd_vs_Offline_afterOR_eff_z0));


    setHistDir(createDirName("/TruthMatchHist/TrackHist_2nd/HTT_2nd_vs_HTT_1st/EffHist/"));

    h_HTT_2nd_vs_HTT_1st_eff_pt = new TGraphAsymmErrors(); h_HTT_2nd_vs_HTT_1st_eff_pt->SetNameTitle("h_HTT_2nd_vs_HTT_1st_eff_pt", "Second Stage vs. First Stage Track Efficiency as function of truth pt;p_{T} (GeV);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_2nd_vs_HTT_1st_eff_pt));
    h_HTT_2nd_vs_HTT_1st_eff_eta = new TGraphAsymmErrors(); h_HTT_2nd_vs_HTT_1st_eff_eta->SetNameTitle("h_HTT_2nd_vs_HTT_1st_eff_eta", "Second Stage vs. First Stage Track Efficiency as function of truth eta;#eta;"); ATH_CHECK(regGraph(getHistDir(), h_HTT_2nd_vs_HTT_1st_eff_eta));
    h_HTT_2nd_vs_HTT_1st_eff_phi = new TGraphAsymmErrors(); h_HTT_2nd_vs_HTT_1st_eff_phi->SetNameTitle("h_HTT_2nd_vs_HTT_1st_eff_phi", "Second Stage vs. First Stage Track Efficiency as function of truth phi;#phi (rad);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_2nd_vs_HTT_1st_eff_phi));
    h_HTT_2nd_vs_HTT_1st_eff_d0 = new TGraphAsymmErrors(); h_HTT_2nd_vs_HTT_1st_eff_d0->SetNameTitle("h_HTT_2nd_vs_HTT_1st_eff_d0", "Second Stage vs. First Stage Track Efficiency as function of truth d0;d_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_2nd_vs_HTT_1st_eff_d0));
    h_HTT_2nd_vs_HTT_1st_eff_z0 = new TGraphAsymmErrors(); h_HTT_2nd_vs_HTT_1st_eff_z0->SetNameTitle("h_HTT_2nd_vs_HTT_1st_eff_z0", "Second Stage vs. First Stage Track Efficiency as function of truth z0;z_{0} (mm);"); ATH_CHECK(regGraph(getHistDir(), h_HTT_2nd_vs_HTT_1st_eff_z0));


    setHistDir(createDirName("/TruthMatchHist/TrackHist_2nd/HTT_2nd_vs_HTT_1st/ResHist/1D_Res/"));

    h_HTT_2nd_vs_HTT_1st_Res_truthmatch_pt = new TH1F("h_HTT_2nd_vs_HTT_1st_Res_truthmatch_pt", "pt resolution of first stage htt track w/ second stage htt track matched using truth matching",   100, (-1.)*ResHistLimit_pt2, ResHistLimit_pt2); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_HTT_1st_Res_truthmatch_pt));
    h_HTT_2nd_vs_HTT_1st_Res_truthmatch_eta = new TH1F("h_HTT_2nd_vs_HTT_1st_Res_truthmatch_eta", "eta resolution of first stage htt track w/ second stage htt track matched using truth matching",100, (-1.)*ResHistLimit_eta2, ResHistLimit_eta2); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_HTT_1st_Res_truthmatch_eta));
    h_HTT_2nd_vs_HTT_1st_Res_truthmatch_phi = new TH1F("h_HTT_2nd_vs_HTT_1st_Res_truthmatch_phi", "phi resolution of first stage htt track w/ second stage htt track matched using truth matching",100, (-1.)*ResHistLimit_phi2, ResHistLimit_phi2); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_HTT_1st_Res_truthmatch_phi));
    h_HTT_2nd_vs_HTT_1st_Res_truthmatch_d0 = new TH1F("h_HTT_2nd_vs_HTT_1st_Res_truthmatch_d0", "d0 resolution of first stage htt track w/ second stage htt track matched using truth matching",   100, (-1.)*ResHistLimit_d02, ResHistLimit_d02); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_HTT_1st_Res_truthmatch_d0));
    h_HTT_2nd_vs_HTT_1st_Res_truthmatch_z0 = new TH1F("h_HTT_2nd_vs_HTT_1st_Res_truthmatch_z0", "z0 resolution of first stage htt track w/ second stage htt track matched using truth matching",   100, (-1.)*ResHistLimit_z02, ResHistLimit_z02); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_HTT_1st_Res_truthmatch_z0));
    h_HTT_2nd_vs_HTT_1st_Res_truthmatch_qoverpt = new TH1F("h_HTT_2nd_vs_HTT_1st_Res_truthmatch_qoverpt","qoverpt resolution of first stage htt track w/ second stage htt track matched using truth matching", 100, (-1.)*ResHistLimit_ipt2, ResHistLimit_ipt2); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_HTT_1st_Res_truthmatch_qoverpt));


    setHistDir(createDirName("/TruthMatchHist/TrackHist_2nd/HTT_2nd_vs_HTT_1st/ResHist/2D_Res/vspt/"));

    h_HTT_2nd_vs_HTT_1st_Res_truthmatch_pt_vspt = new TH2F("h_HTT_2nd_vs_HTT_1st_Res_truthmatch_pt_vspt", "pt resolution of first stage htt track w/ second stage htt track matched vs pt using truth matching",    getNXbinsPT(), m_xbinsPT, 100, (-1.)*ResHistLimit_pt2, ResHistLimit_pt2); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_HTT_1st_Res_truthmatch_pt_vspt));
    h_HTT_2nd_vs_HTT_1st_Res_truthmatch_eta_vspt = new TH2F("h_HTT_2nd_vs_HTT_1st_Res_truthmatch_eta_vspt", "eta resolution of first stage htt track w/ second stage htt track matched vs pt using truth matching", getNXbinsPT(), m_xbinsPT, 100, (-1.)*ResHistLimit_eta2, ResHistLimit_eta2); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_HTT_1st_Res_truthmatch_eta_vspt));
    h_HTT_2nd_vs_HTT_1st_Res_truthmatch_phi_vspt = new TH2F("h_HTT_2nd_vs_HTT_1st_Res_truthmatch_phi_vspt", "phi resolution of first stage htt track w/ second stage htt track matched vs pt using truth matching", getNXbinsPT(), m_xbinsPT, 100, (-1.)*ResHistLimit_phi2, ResHistLimit_phi2); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_HTT_1st_Res_truthmatch_phi_vspt));
    h_HTT_2nd_vs_HTT_1st_Res_truthmatch_d0_vspt = new TH2F("h_HTT_2nd_vs_HTT_1st_Res_truthmatch_d0_vspt", "d0 resolution of first stage htt track w/ second stage htt track matched vs pt using truth matching",    getNXbinsPT(), m_xbinsPT, 100, (-1.)*ResHistLimit_d02, ResHistLimit_d02); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_HTT_1st_Res_truthmatch_d0_vspt));
    h_HTT_2nd_vs_HTT_1st_Res_truthmatch_z0_vspt = new TH2F("h_HTT_2nd_vs_HTT_1st_Res_truthmatch_z0_vspt", "z0 resolution of first stage htt track w/ second stage htt track matched vs pt using truth matching",    getNXbinsPT(), m_xbinsPT, 100, (-1.)*ResHistLimit_z02, ResHistLimit_z02); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_HTT_1st_Res_truthmatch_z0_vspt));
    h_HTT_2nd_vs_HTT_1st_Res_truthmatch_qoverpt_vspt = new TH2F("h_HTT_2nd_vs_HTT_1st_Res_truthmatch_qoverpt_vspt", "qoverpt resolution of first stage htt track w/ second stage htt track matched vs pt using truth matching", getNXbinsPT(), m_xbinsPT, 100, (-1.)*ResHistLimit_ipt2, ResHistLimit_ipt2); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_HTT_1st_Res_truthmatch_qoverpt_vspt));


    setHistDir(createDirName("/TruthMatchHist/TrackHist_2nd/HTT_2nd_vs_HTT_1st/ResHist/2D_Res/vseta/"));

    h_HTT_2nd_vs_HTT_1st_Res_truthmatch_pt_vseta = new TH2F("h_HTT_2nd_vs_HTT_1st_Res_truthmatch_pt_vseta", "pt resolution of first stage htt track w/ second stage htt track matched vs eta using truth matching",    50, m_etamin, m_etamax, 100, (-1.)*ResHistLimit_pt2, ResHistLimit_pt2); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_HTT_1st_Res_truthmatch_pt_vseta));
    h_HTT_2nd_vs_HTT_1st_Res_truthmatch_eta_vseta = new TH2F("h_HTT_2nd_vs_HTT_1st_Res_truthmatch_eta_vseta", "eta resolution of first stage htt track w/ second stage htt track matched vs eta using truth matching", 50, m_etamin, m_etamax, 100, (-1.)*ResHistLimit_eta2, ResHistLimit_eta2); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_HTT_1st_Res_truthmatch_eta_vseta));
    h_HTT_2nd_vs_HTT_1st_Res_truthmatch_phi_vseta = new TH2F("h_HTT_2nd_vs_HTT_1st_Res_truthmatch_phi_vseta", "phi resolution of first stage htt track w/ second stage htt track matched vs eta using truth matching", 50, m_etamin, m_etamax, 100, (-1.)*ResHistLimit_phi2, ResHistLimit_phi2); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_HTT_1st_Res_truthmatch_phi_vseta));
    h_HTT_2nd_vs_HTT_1st_Res_truthmatch_d0_vseta = new TH2F("h_HTT_2nd_vs_HTT_1st_Res_truthmatch_d0_vseta", "d0 resolution of first stage htt track w/ second stage htt track matched vs eta using truth matching",    50, m_etamin, m_etamax, 100, (-1.)*ResHistLimit_d02, ResHistLimit_d02); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_HTT_1st_Res_truthmatch_d0_vseta));
    h_HTT_2nd_vs_HTT_1st_Res_truthmatch_z0_vseta = new TH2F("h_HTT_2nd_vs_HTT_1st_Res_truthmatch_z0_vseta", "z0 resolution of first stage htt track w/ second stage htt track matched vs eta using truth matching",    50, m_etamin, m_etamax, 100, (-1.)*ResHistLimit_z02, ResHistLimit_z02); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_HTT_1st_Res_truthmatch_z0_vseta));
    h_HTT_2nd_vs_HTT_1st_Res_truthmatch_qoverpt_vseta = new TH2F("h_HTT_2nd_vs_HTT_1st_Res_truthmatch_qoverpt_vseta", "qoverpt resolution of first stage htt track w/ second stage htt track matched vs eta using truth matching", 50, m_etamin, m_etamax, 100, (-1.)*ResHistLimit_ipt2, ResHistLimit_ipt2); ATH_CHECK(regHist(getHistDir(), h_HTT_2nd_vs_HTT_1st_Res_truthmatch_qoverpt_vseta));


    setHistDir(createDirName("/TruthMatchHist/FakeRateHist_2nd/"));

    h_ratio_HTT_2nd_Truth          = new TH1F("h_ratio_HTT_2nd_Truth", "# Second Stage HTT Tracks / # Truth Tracks, per event;ratio(nHTT_2nd/nTruth);# events", 100, -0.5, 100-0.5); ATH_CHECK(regHist(getHistDir(), h_ratio_HTT_2nd_Truth));
    h_ratio_HTT_2nd_passChi2_Truth = new TH1F("h_ratio_HTT_2nd_passChi2_Truth", "# Second Stage HTT Tracks passing Chi^2 cut / # Truth Tracks, per event;ratio(nHTT_2nd_passChi2/nTruth);# events", 100, -0.5, 100-0.5); ATH_CHECK(regHist(getHistDir(), h_ratio_HTT_2nd_passChi2_Truth));
    h_ratio_HTT_2nd_afterOR_Truth  = new TH1F("h_ratio_HTT_2nd_afterOR_Truth", "# Second Stage HTT Tracks after overlap removal / # Truth Tracks, per event;ratio(nHTT_2nd_afterOR/nTruth);# events", 100, -0.5, 100-0.5); ATH_CHECK(regHist(getHistDir(), h_ratio_HTT_2nd_afterOR_Truth));

    h_ratio_HTT_2nd_Offline          = new TH1F("h_ratio_HTT_2nd_Offline", "# Second Stage HTT Tracks / # Offline Tracks, per event;ratio(nHTT_2nd/nOffline);# events", 100, -0.5, 100-0.5); ATH_CHECK(regHist(getHistDir(), h_ratio_HTT_2nd_Offline));
    h_ratio_HTT_2nd_passChi2_Offline = new TH1F("h_ratio_HTT_2nd_passChi2_Offline", "# Second Stage HTT Tracks passing Chi^2 cut / # Offline Tracks, per event;ratio(nHTT_2nd_passChi2/nOffline);# events", 100, -0.5, 100-0.5); ATH_CHECK(regHist(getHistDir(), h_ratio_HTT_2nd_passChi2_Offline));
    h_ratio_HTT_2nd_afterOR_Offline  = new TH1F("h_ratio_HTT_2nd_afterOR_Offline", "# Second Stage HTT Tracks after overlap removal / # Offline Tracks, per event;ratio(nHTT_2nd_afterOR/nOffline);# events", 100, -0.5, 100-0.5); ATH_CHECK(regHist(getHistDir(), h_ratio_HTT_2nd_afterOR_Offline));


    clearHistDir();

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void HTTPerformanceMonitorTool::fillCrudeEffHistograms_1st(std::vector<HTTRoad*> const * roads_1st, std::vector<HTTTrack> const * tracks_1st)
{
    if (!roads_1st) {
        ATH_MSG_WARNING("Failed to retrieve first stage roads.");
        return;
    }
    if (!tracks_1st) {
        ATH_MSG_WARNING("Failed to retrieve first stage tracks.");
        return;
    }

    std::vector<HTTTruthTrack> const & truthtracks = m_logicEventInputHeader_1st->optional().getTruthTracks();

    h_Truth_Track_Leading_pt->Fill(truthtracks.front().getPt()*0.001);
    h_Truth_Track_Leading_eta->Fill(truthtracks.front().getEta());
    h_Truth_Track_Leading_phi->Fill(truthtracks.front().getPhi());
    h_Truth_Track_Leading_d0->Fill(truthtracks.front().getD0());
    h_Truth_Track_Leading_z0->Fill(truthtracks.front().getZ0());

    if (!roads_1st->empty()) {
        h_Truth_Track_withroad_1st_pt->Fill(truthtracks.front().getPt()*0.001); // GeV
        h_Truth_Track_withroad_1st_eta->Fill(truthtracks.front().getEta());
        h_Truth_Track_withroad_1st_phi->Fill(truthtracks.front().getPhi());
        h_Truth_Track_withroad_1st_d0->Fill(truthtracks.front().getD0());
        h_Truth_Track_withroad_1st_z0->Fill(truthtracks.front().getZ0());
    }

    int bestchi2ndof = std::numeric_limits<int>::max();
    for (HTTTrack const & t : *tracks_1st) {
        if (t.getChi2ndof() < bestchi2ndof) {
            bestchi2ndof = t.getChi2ndof();
        }
    }
    if (bestchi2ndof <= m_cut_chi2ndof) {
        h_Truth_Track_withtrack_1st_pt->Fill(truthtracks.front().getPt()*0.001);
        h_Truth_Track_withtrack_1st_eta->Fill(truthtracks.front().getEta());
        h_Truth_Track_withtrack_1st_phi->Fill(truthtracks.front().getPhi());
        h_Truth_Track_withtrack_1st_d0->Fill(truthtracks.front().getD0());
        h_Truth_Track_withtrack_1st_z0->Fill(truthtracks.front().getZ0());
    }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void HTTPerformanceMonitorTool::fillCrudeEffHistograms_2nd(std::vector<HTTRoad*> const * roads_2nd, std::vector<HTTTrack> const * tracks_2nd)
{
    if (!roads_2nd) {
        ATH_MSG_WARNING("Failed to retrieve second stage roads.");
        return;
    }
    if (!tracks_2nd) {
        ATH_MSG_WARNING("Failed to retrieve second stage tracks.");
        return;
    }

    std::vector<HTTTruthTrack> const & truthtracks = m_logicEventInputHeader_1st->optional().getTruthTracks();

    if (!roads_2nd->empty()) {
        h_Truth_Track_withroad_2nd_pt->Fill(truthtracks.front().getPt()*0.001); // GeV
        h_Truth_Track_withroad_2nd_eta->Fill(truthtracks.front().getEta());
        h_Truth_Track_withroad_2nd_phi->Fill(truthtracks.front().getPhi());
        h_Truth_Track_withroad_2nd_d0->Fill(truthtracks.front().getD0());
        h_Truth_Track_withroad_2nd_z0->Fill(truthtracks.front().getZ0());
    }

    int bestchi2ndof = std::numeric_limits<int>::max();
    for (HTTTrack const & t : *tracks_2nd) {
        if (t.getChi2ndof() < bestchi2ndof) {
            bestchi2ndof = t.getChi2ndof();
        }
    }
    if (bestchi2ndof <= m_cut_chi2ndof) {
        h_Truth_Track_withtrack_2nd_pt->Fill(truthtracks.front().getPt()*0.001);
        h_Truth_Track_withtrack_2nd_eta->Fill(truthtracks.front().getEta());
        h_Truth_Track_withtrack_2nd_phi->Fill(truthtracks.front().getPhi());
        h_Truth_Track_withtrack_2nd_d0->Fill(truthtracks.front().getD0());
        h_Truth_Track_withtrack_2nd_z0->Fill(truthtracks.front().getZ0());
    }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// TODO truth tracks in single muon wrappers have event index 0, so force everything to 0 here
void HTTPerformanceMonitorTool::fillHTTRoadTruthMatchHistograms_1st(std::vector<HTTRoad*> const * roads_1st)
{
    if (!roads_1st) {
        ATH_MSG_WARNING("Failed to retrieve first stage roads.");
        return;
    }


    // Create a lookup on the barcodes of all the roads, and also max frac
    //      value.first = roads with matching barcode, and fraction above m_cut_barcodefrac
    //      value.second = maximum barcode fraction among all roads for this barcode
    std::map<typename HTTMultiTruth::Barcode, std::pair<std::vector<HTTRoad const *>, float>> m;
    calculateTruth(roads_1st, m);

    std::vector<HTTTruthTrack> const & truthtracks = m_logicEventInputHeader_1st->optional().getTruthTracks();

    // Find roads matching each track
    for (auto const & t : truthtracks) {
        // No PDGid code associated with LLP sample type. If we want to run on one in particular,
        // should add it as a separate flag.
        if (m_evtSel->getSampleType() != SampleType::LLPs) {
            if (std::abs(t.getPDGCode()) != static_cast<int> (m_evtSel->getSampleType())) continue;
        }
        else {
            if ((m_evtSel->getLRTpdgID() != 0) && std::abs(t.getPDGCode()) != m_evtSel->getLRTpdgID()) {
                std::cout << "Skipping this truth track: wrong PDGID (" << t.getPDGCode() << ")" << std::endl;
                continue;
            }
        }
        if (!m_evtSel->passMatching(t)) continue;

        bool matched = false;
        auto it = m.find({ 0, t.getBarcode() });
        if (it != m.end() && !it->second.first.empty()) { // matched with roads
            matched = true;
            // ATH_MSG_INFO(" I am here for ROAD and truth track pT is: " << t.getPt()*0.001
            //           << " so q/pT is: " << t.getQ()/t.getPt()
            //           << " and d0 is: " << t.getD0());

            h_Truth_Track_Road_1st_matched_pt->Fill(t.getPt()*0.001);
            h_Truth_Track_Road_1st_matched_eta->Fill(t.getEta());
            h_Truth_Track_Road_1st_matched_phi->Fill(t.getPhi());
            h_Truth_Track_Road_1st_matched_d0->Fill(t.getD0());
            h_Truth_Track_Road_1st_matched_z0->Fill(t.getZ0());
        }
        if (!matched) ATH_MSG_DEBUG("First stage truth match road efficiency - Missed truth track: " << t.getPars());
    }

}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// TODO truth tracks in single muon wrappers have event index 0, so force everything to 0 here
void HTTPerformanceMonitorTool::fillHTTRoadTruthMatchHistograms_2nd(std::vector<HTTRoad*> const * roads_2nd)
{
    if (!roads_2nd) {
        ATH_MSG_WARNING("Failed to retrieve second stage roads.");
        return;
    }

    // Create a lookup on the barcodes of all the roads, and also max frac
    //      value.first = roads with matching barcode, and fraction above m_cut_barcodefrac
    //      value.second = maximum barcode fraction among all roads for this barcode
    std::map<typename HTTMultiTruth::Barcode, std::pair<std::vector<HTTRoad const *>, float>> m;
    calculateTruth(roads_2nd, m);

    std::vector<HTTTruthTrack> const & truthtracks = m_logicEventInputHeader_1st->optional().getTruthTracks();

    // Find roads matching each track
    for (auto const & t : truthtracks) {
        // No PDGid code associated with LLP sample type. If we want to run on one in particular,
        // should add it as a separate flag.
        if (m_evtSel->getSampleType() != SampleType::LLPs) {
            if (std::abs(t.getPDGCode()) != static_cast<int> (m_evtSel->getSampleType())) continue;
        }
        else {
            if ((m_evtSel->getLRTpdgID() != 0) && std::abs(t.getPDGCode()) != m_evtSel->getLRTpdgID()) {
                std::cout << "Skipping this truth track: wrong PDGID (" << t.getPDGCode() << ")" << std::endl;
                continue;
            }
        }
        if (!m_evtSel->passMatching(t)) continue;

        bool matched = false;
        auto it = m.find({ 0, t.getBarcode() });
        if (it != m.end() && !it->second.first.empty()) { // matched with roads
            matched = true;
            h_Truth_Track_Road_2nd_matched_pt->Fill(t.getPt()*0.001);
            h_Truth_Track_Road_2nd_matched_eta->Fill(t.getEta());
            h_Truth_Track_Road_2nd_matched_phi->Fill(t.getPhi());
            h_Truth_Track_Road_2nd_matched_d0->Fill(t.getD0());
            h_Truth_Track_Road_2nd_matched_z0->Fill(t.getZ0());
        }
        if (!matched) ATH_MSG_DEBUG("Second stage truth match road efficiency - Missed truth track: " << t.getPars());
    }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// TODO truth tracks in single muon wrappers have event index 0, so force everything to 0 here
void HTTPerformanceMonitorTool::fillHTTTrackTruthMatchHistograms_1st(std::vector<HTTTrack> const * tracks_1st)
{
    if (!tracks_1st) {
        ATH_MSG_WARNING("Failed to retrieve first stage tracks.");
        return;
    }

    std::map< long, std::vector<HTTTrack const *> > httmatchinfo;

    unsigned nTracks_1st_passChi2 = 0;
    unsigned nTracks_1st_afterOR = 0;

    std::cout << " m_cut_chi2ndof : " << m_cut_chi2ndof << std::endl;
    // std::ofstream myfile;
    // myfile.open ("truthroad_event_"+std::to_string( m_event )+".txt");

    // Information on HTT tracks relative to the hard-scattering events
    // are collected in a vector and later used to build matching maps
    for (HTTTrack const & htt_t : *tracks_1st)
    {
      // ATH_MSG_INFO(" I am here and truth track pT htt is: " << htt_t.getPt()*0.001
      //           << " so 1/pT is: " << 1./htt_t.getPt()
      //           << " and d0 is: " << htt_t.getD0());
        if (htt_t.getChi2ndof() <= m_cut_chi2ndof) nTracks_1st_passChi2++;
        if (htt_t.passedOR() == 1) nTracks_1st_afterOR++;

        if (m_evtSel->passMatching(htt_t) && htt_t.getBarcodeFrac() >= m_cut_barcodefrac)
            httmatchinfo[htt_t.getBarcode()].push_back(&htt_t);
    }

    // counters:
    unsigned n_HTTmatched_1st = 0;
    unsigned n_HTTmatched_1st_passChi2 = 0;
    unsigned n_HTTmatched_1st_afterOR = 0;


    std::vector<HTTTruthTrack*> filteredTruthTracks;
    std::vector<HTTTruthTrack> truth_tracks = m_logicEventInputHeader_1st->optional().getTruthTracks();

    m_nTruth_Tracks_Full = 0;
    m_bestHTTTrack_1st = nullptr;

    for (HTTTruthTrack & truth_t : truth_tracks) {
      // ATH_MSG_INFO(" I am here and truth track pT is: " << truth_t.getPt()*0.001
      //           << " so q/pT is: " << truth_t.getQ()/truth_t.getPt()
      //           << " and d0 is: " << truth_t.getD0());
        if (!m_evtSel->passMatching(truth_t)) continue;
        // if (truth_t.getBarcode() == 0) continue;
        // if (truth_t.getQ() == 0) continue;
        // if (!m_evtSel->passCuts(truth_t)) continue;

        filteredTruthTracks.push_back(&truth_t);

        m_nTruth_Tracks_Full++;
        //myfile << m_event << " " << truth_t.getQ()/truth_t.getPt() << " " << truth_t.getD0();

        h_Truth_Track_Full_pt->Fill(truth_t.getPt()*0.001);
        h_Truth_Track_Full_eta->Fill(truth_t.getEta());
        h_Truth_Track_Full_phi->Fill(truth_t.getPhi());
        h_Truth_Track_Full_d0->Fill(truth_t.getD0());
        h_Truth_Track_Full_z0->Fill(truth_t.getZ0());

        //========================
        // Offline vs Truth
        //========================
        if (truth_t.getBarcodeFracOffline() > m_cut_barcodefrac) {
            // Matched truth parameters
            h_Truth_Track_Offline_matched_pt->Fill(truth_t.getPt()*0.001);
            h_Truth_Track_Offline_matched_eta->Fill(truth_t.getEta());
            h_Truth_Track_Offline_matched_phi->Fill(truth_t.getPhi());
            h_Truth_Track_Offline_matched_d0->Fill(truth_t.getD0());
            h_Truth_Track_Offline_matched_z0->Fill(truth_t.getZ0());

            HTTOfflineTrack const * bestoffline = nullptr;
            for (HTTOfflineTrack const & offline_t : m_logicEventInputHeader_1st->optional().getOfflineTracks()) {
                if (offline_t.getBarcode() == truth_t.getBarcode() && offline_t.getBarcode() != -1 &&
                    fabs(offline_t.getBarcodeFrac() - truth_t.getBarcodeFracOffline()) < 0.01) {
                    bestoffline = &offline_t;
                    break;
                }
            }

            if (bestoffline) {
                h_Offline_vs_Truth_Res_truthmatch_pt->Fill((bestoffline->getPt() - truth_t.getPt()) * 0.001);
                h_Offline_vs_Truth_Res_truthmatch_eta->Fill(bestoffline->getEta() - truth_t.getEta());
                h_Offline_vs_Truth_Res_truthmatch_phi->Fill(bestoffline->getPhi() - truth_t.getPhi());
                h_Offline_vs_Truth_Res_truthmatch_d0->Fill(bestoffline->getD0() - truth_t.getD0());
                h_Offline_vs_Truth_Res_truthmatch_z0->Fill(bestoffline->getZ0() - truth_t.getZ0());
                h_Offline_vs_Truth_Res_truthmatch_qoverpt->Fill((abs(bestoffline->getQOverPt()) - abs(truth_t.getQOverPt())) * 1000.);

                h_Offline_vs_Truth_Res_truthmatch_pt_vspt->Fill(truth_t.getPt() * 0.001, (bestoffline->getPt() - truth_t.getPt()) * 0.001);
                h_Offline_vs_Truth_Res_truthmatch_eta_vspt->Fill(truth_t.getPt() * 0.001, bestoffline->getEta() - truth_t.getEta());
                h_Offline_vs_Truth_Res_truthmatch_phi_vspt->Fill(truth_t.getPt() * 0.001, bestoffline->getPhi() - truth_t.getPhi());
                h_Offline_vs_Truth_Res_truthmatch_d0_vspt->Fill(truth_t.getPt() * 0.001, bestoffline->getD0() - truth_t.getD0());
                h_Offline_vs_Truth_Res_truthmatch_z0_vspt->Fill(truth_t.getPt() * 0.001, bestoffline->getZ0() - truth_t.getZ0());
                h_Offline_vs_Truth_Res_truthmatch_qoverpt_vspt->Fill(truth_t.getPt() * 0.001, (abs(bestoffline->getQOverPt()) - abs(truth_t.getQOverPt())) * 1000.);

                h_Offline_vs_Truth_Res_truthmatch_pt_vseta->Fill(truth_t.getEta(), (bestoffline->getPt() - truth_t.getPt()) * 0.001);
                h_Offline_vs_Truth_Res_truthmatch_eta_vseta->Fill(truth_t.getEta(), bestoffline->getEta() - truth_t.getEta());
                h_Offline_vs_Truth_Res_truthmatch_phi_vseta->Fill(truth_t.getEta(), bestoffline->getPhi() - truth_t.getPhi());
                h_Offline_vs_Truth_Res_truthmatch_d0_vseta->Fill(truth_t.getEta(), bestoffline->getD0() - truth_t.getD0());
                h_Offline_vs_Truth_Res_truthmatch_z0_vseta->Fill(truth_t.getEta(), bestoffline->getZ0() - truth_t.getZ0());
                h_Offline_vs_Truth_Res_truthmatch_qoverpt_vseta->Fill(truth_t.getEta(), (abs(bestoffline->getQOverPt()) - abs(truth_t.getQOverPt())) * 1000.);
            }
        }

        //========================
        // HTT vs Truth
        //========================
        auto it_matched = httmatchinfo.find(truth_t.getBarcode());
        if (it_matched != httmatchinfo.end()) {
            n_HTTmatched_1st++;
            // matched truth parameters
            h_Truth_Track_HTT_1st_matched_pt->Fill(truth_t.getPt()*0.001);
            h_Truth_Track_HTT_1st_matched_eta->Fill(truth_t.getEta());
            h_Truth_Track_HTT_1st_matched_phi->Fill(truth_t.getPhi());
            h_Truth_Track_HTT_1st_matched_d0->Fill(truth_t.getD0());
            h_Truth_Track_HTT_1st_matched_z0->Fill(truth_t.getZ0());

            bool matched_passChi2 = false;
            bool matched_afterOR = false;

            HTTTrack const * besthtt = nullptr;
            HTTTrack const * besthtt_passChi2 = nullptr;
            HTTTrack const * besthtt_afterOR = nullptr;

            for (HTTTrack const * matched_t : it_matched->second) {
                if (!besthtt ||
                    (besthtt->getBarcodeFrac() < matched_t->getBarcodeFrac()) ||
                    (besthtt->getBarcodeFrac() == matched_t->getBarcodeFrac() && besthtt->getChi2ndof() > matched_t->getChi2ndof())) {
                    besthtt = matched_t;
                }

                if (matched_t->getChi2ndof() <= m_cut_chi2ndof) {
                    matched_passChi2 = true;
                    n_HTTmatched_1st_passChi2++;
                    if (!besthtt_passChi2 ||
                        (besthtt_passChi2->getBarcodeFrac() < matched_t->getBarcodeFrac()) ||
                        (besthtt_passChi2->getBarcodeFrac() == matched_t->getBarcodeFrac() && besthtt_passChi2->getChi2ndof() > matched_t->getChi2ndof())) {
                        besthtt_passChi2 = matched_t;
                    }
                }

                if (matched_t->passedOR() == 1) {
                    matched_afterOR = true;
                    n_HTTmatched_1st_afterOR++;
                    if (!besthtt_afterOR ||
                        (besthtt_afterOR->getBarcodeFrac() < matched_t->getBarcodeFrac()) ||
                        (besthtt_afterOR->getBarcodeFrac() == matched_t->getBarcodeFrac() && besthtt_afterOR->getChi2ndof() > matched_t->getChi2ndof())) {
                        besthtt_afterOR = matched_t;
                    }
                }
            }

            if (matched_passChi2) {
                // matched truth parameters
                h_Truth_Track_HTT_1st_passChi2_matched_pt->Fill(truth_t.getPt()*0.001);
                h_Truth_Track_HTT_1st_passChi2_matched_eta->Fill(truth_t.getEta());
                h_Truth_Track_HTT_1st_passChi2_matched_phi->Fill(truth_t.getPhi());
                h_Truth_Track_HTT_1st_passChi2_matched_d0->Fill(truth_t.getD0());
                h_Truth_Track_HTT_1st_passChi2_matched_z0->Fill(truth_t.getZ0());
            }

            if (matched_afterOR) {
                // matched truth parameters
                h_Truth_Track_HTT_1st_afterOR_matched_pt->Fill(truth_t.getPt()*0.001);
                h_Truth_Track_HTT_1st_afterOR_matched_eta->Fill(truth_t.getEta());
                h_Truth_Track_HTT_1st_afterOR_matched_phi->Fill(truth_t.getPhi());
                h_Truth_Track_HTT_1st_afterOR_matched_d0->Fill(truth_t.getD0());
                h_Truth_Track_HTT_1st_afterOR_matched_z0->Fill(truth_t.getZ0());
            }

            if (besthtt) {
                m_bestHTTTrack_1st = besthtt;

                h_HTT_1st_vs_Truth_Res_truthmatch_pt->Fill((besthtt->getPt() - truth_t.getPt()) * 0.001);
                h_HTT_1st_vs_Truth_Res_truthmatch_eta->Fill(besthtt->getEta() - truth_t.getEta());
                h_HTT_1st_vs_Truth_Res_truthmatch_phi->Fill(besthtt->getPhi() - truth_t.getPhi());
                h_HTT_1st_vs_Truth_Res_truthmatch_d0->Fill(besthtt->getD0() - truth_t.getD0());
                h_HTT_1st_vs_Truth_Res_truthmatch_z0->Fill(besthtt->getZ0() - truth_t.getZ0());
                h_HTT_1st_vs_Truth_Res_truthmatch_qoverpt->Fill((abs(besthtt->getQOverPt()) - abs(truth_t.getQOverPt())) * 1000.);

                h_HTT_1st_vs_Truth_Res_truthmatch_pt_vspt->Fill(truth_t.getPt() * 0.001, (besthtt->getPt() - truth_t.getPt()) * 0.001);
                h_HTT_1st_vs_Truth_Res_truthmatch_eta_vspt->Fill(truth_t.getPt() * 0.001, besthtt->getEta() - truth_t.getEta());
                h_HTT_1st_vs_Truth_Res_truthmatch_phi_vspt->Fill(truth_t.getPt() * 0.001, besthtt->getPhi() - truth_t.getPhi());
                h_HTT_1st_vs_Truth_Res_truthmatch_d0_vspt->Fill(truth_t.getPt() * 0.001, besthtt->getD0() - truth_t.getD0());
                h_HTT_1st_vs_Truth_Res_truthmatch_z0_vspt->Fill(truth_t.getPt() * 0.001, besthtt->getZ0() - truth_t.getZ0());
                h_HTT_1st_vs_Truth_Res_truthmatch_qoverpt_vspt->Fill(truth_t.getPt() * 0.001, (abs(besthtt->getQOverPt()) - abs(truth_t.getQOverPt())) * 1000.);

                h_HTT_1st_vs_Truth_Res_truthmatch_pt_vseta->Fill(truth_t.getEta(), (besthtt->getPt() - truth_t.getPt()) * 0.001);
                h_HTT_1st_vs_Truth_Res_truthmatch_eta_vseta->Fill(truth_t.getEta(), besthtt->getEta() - truth_t.getEta());
                h_HTT_1st_vs_Truth_Res_truthmatch_phi_vseta->Fill(truth_t.getEta(), besthtt->getPhi() - truth_t.getPhi());
                h_HTT_1st_vs_Truth_Res_truthmatch_d0_vseta->Fill(truth_t.getEta(), besthtt->getD0() - truth_t.getD0());
                h_HTT_1st_vs_Truth_Res_truthmatch_z0_vseta->Fill(truth_t.getEta(), besthtt->getZ0() - truth_t.getZ0());
                h_HTT_1st_vs_Truth_Res_truthmatch_qoverpt_vseta->Fill(truth_t.getEta(), (abs(besthtt->getQOverPt()) - abs(truth_t.getQOverPt())) * 1000.);
            }

            if (besthtt_passChi2) {
                h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_pt->Fill((besthtt_passChi2->getPt() - truth_t.getPt()) * 0.001);
                h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_eta->Fill(besthtt_passChi2->getEta() - truth_t.getEta());
                h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_phi->Fill(besthtt_passChi2->getPhi() - truth_t.getPhi());
                h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_d0->Fill(besthtt_passChi2->getD0() - truth_t.getD0());
                h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_z0->Fill(besthtt_passChi2->getZ0() - truth_t.getZ0());
                h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_qoverpt->Fill((abs(besthtt_passChi2->getQOverPt()) - abs(truth_t.getQOverPt())) * 1000.);

                h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_pt_vspt->Fill(truth_t.getPt() * 0.001, (besthtt_passChi2->getPt() - truth_t.getPt()) * 0.001);
                h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_eta_vspt->Fill(truth_t.getPt() * 0.001, besthtt_passChi2->getEta() - truth_t.getEta());
                h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_phi_vspt->Fill(truth_t.getPt() * 0.001, besthtt_passChi2->getPhi() - truth_t.getPhi());
                h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_d0_vspt->Fill(truth_t.getPt() * 0.001, besthtt_passChi2->getD0() - truth_t.getD0());
                h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_z0_vspt->Fill(truth_t.getPt() * 0.001, besthtt_passChi2->getZ0() - truth_t.getZ0());
                h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_qoverpt_vspt->Fill(truth_t.getPt() * 0.001, (abs(besthtt_passChi2->getQOverPt()) - abs(truth_t.getQOverPt())) * 1000.);

                h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_pt_vseta->Fill(truth_t.getEta(), (besthtt_passChi2->getPt() - truth_t.getPt()) * 0.001);
                h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_eta_vseta->Fill(truth_t.getEta(), besthtt_passChi2->getEta() - truth_t.getEta());
                h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_phi_vseta->Fill(truth_t.getEta(), besthtt_passChi2->getPhi() - truth_t.getPhi());
                h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_d0_vseta->Fill(truth_t.getEta(), besthtt_passChi2->getD0() - truth_t.getD0());
                h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_z0_vseta->Fill(truth_t.getEta(), besthtt_passChi2->getZ0() - truth_t.getZ0());
                h_HTT_1st_vs_Truth_passChi2_Res_truthmatch_qoverpt_vseta->Fill(truth_t.getEta(), (abs(besthtt_passChi2->getQOverPt()) - abs(truth_t.getQOverPt())) * 1000.);
            }

            if (besthtt_afterOR) {
                h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_pt->Fill((besthtt_afterOR->getPt() - truth_t.getPt()) * 0.001);
                h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_eta->Fill(besthtt_afterOR->getEta() - truth_t.getEta());
                h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_phi->Fill(besthtt_afterOR->getPhi() - truth_t.getPhi());
                h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_d0->Fill(besthtt_afterOR->getD0() - truth_t.getD0());
                h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_z0->Fill(besthtt_afterOR->getZ0() - truth_t.getZ0());
                h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_qoverpt->Fill((abs(besthtt_afterOR->getQOverPt()) - abs(truth_t.getQOverPt())) * 1000.);

                h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_pt_vspt->Fill(truth_t.getPt() * 0.001, (besthtt_afterOR->getPt() - truth_t.getPt()) * 0.001);
                h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_eta_vspt->Fill(truth_t.getPt() * 0.001, besthtt_afterOR->getEta() - truth_t.getEta());
                h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_phi_vspt->Fill(truth_t.getPt() * 0.001, besthtt_afterOR->getPhi() - truth_t.getPhi());
                h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_d0_vspt->Fill(truth_t.getPt() * 0.001, besthtt_afterOR->getD0() - truth_t.getD0());
                h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_z0_vspt->Fill(truth_t.getPt() * 0.001, besthtt_afterOR->getZ0() - truth_t.getZ0());
                h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_qoverpt_vspt->Fill(truth_t.getPt() * 0.001, (abs(besthtt_afterOR->getQOverPt()) - abs(truth_t.getQOverPt())) * 1000.);

                h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_pt_vseta->Fill(truth_t.getEta(), (besthtt_afterOR->getPt() - truth_t.getPt()) * 0.001);
                h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_eta_vseta->Fill(truth_t.getEta(), besthtt_afterOR->getEta() - truth_t.getEta());
                h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_phi_vseta->Fill(truth_t.getEta(), besthtt_afterOR->getPhi() - truth_t.getPhi());
                h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_d0_vseta->Fill(truth_t.getEta(), besthtt_afterOR->getD0() - truth_t.getD0());
                h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_z0_vseta->Fill(truth_t.getEta(), besthtt_afterOR->getZ0() - truth_t.getZ0());
                h_HTT_1st_vs_Truth_afterOR_Res_truthmatch_qoverpt_vseta->Fill(truth_t.getEta(), (abs(besthtt_afterOR->getQOverPt()) - abs(truth_t.getQOverPt())) * 1000.);
            }
        }
    }
    //myfile.close();
    m_event++;

    h_nTruth_Tracks_Full->Fill(m_nTruth_Tracks_Full);

    // count offline tracks
    unsigned nOffline_Tracks_before_match = 0;
    unsigned nOffline_Tracks_before_match_0GeV = 0;
    unsigned nOffline_Tracks_before_match_1GeV = 0;
    unsigned nOffline_Tracks_before_match_2GeV = 0;

    m_nOffline_Tracks = 0;

    for (HTTOfflineTrack const & offline_t : m_logicEventInputHeader_1st->optional().getOfflineTracks()) {

        nOffline_Tracks_before_match++;
        // Hacked selection
        if (offline_t.nHits() >=10 && m_evtSel->passD0(offline_t) && m_evtSel->passZ0(offline_t)) {
            nOffline_Tracks_before_match_0GeV++;
            if (abs(offline_t.getPt()) * 0.001 > 1) {
                nOffline_Tracks_before_match_1GeV++;
            }
            if (abs(offline_t.getPt()) * 0.001 > 2) {
                nOffline_Tracks_before_match_2GeV++;
            }
        }

        for (HTTTruthTrack const & truth_t : truth_tracks) {
            h_Offline_vs_Truth_Res_no_truthmatch_qoverpt->Fill((abs(offline_t.getQOverPt()) - abs(truth_t.getQOverPt())) * 1000.);
            h_Offline_vs_Truth_Res_no_truthmatch_pt->Fill((offline_t.getPt() - truth_t.getPt()) * 0.001);
            h_Offline_vs_Truth_Res_no_truthmatch_eta->Fill(offline_t.getEta() - truth_t.getEta());
            h_Offline_vs_Truth_Res_no_truthmatch_phi->Fill(offline_t.getPhi() - truth_t.getPhi());
            h_Offline_vs_Truth_Res_no_truthmatch_d0->Fill(offline_t.getD0() - truth_t.getD0());
            h_Offline_vs_Truth_Res_no_truthmatch_z0->Fill(offline_t.getZ0() - truth_t.getZ0());
            h_Offline_vs_Truth_Res_no_truthmatch_z0_wide->Fill(offline_t.getZ0() - truth_t.getZ0());
        }

        // matching with truth tracks in the region
        // this is used when full PU truth is stored.
        // Maybe include another parameter to define FullPUTruth?
        bool matched = true;

        if (m_evtSel->getSampleType() == SampleType::skipTruth) {
            matched = false;
            for (auto ftruth: filteredTruthTracks) {
                 if (ftruth->getBarcode() == offline_t.getBarcode() && offline_t.getBarcodeFrac() > m_cut_barcodefrac){
                    matched = true;
                    break;
                }
            }
        }
        else { // other cases count only tracks in the region
            matched = m_evtSel->passCuts(offline_t);
        }

        if (!matched) continue;

        m_nOffline_Tracks++;

        h_Offline_Track_pt->Fill(offline_t.getPt()*0.001);
        h_Offline_Track_eta->Fill(offline_t.getEta());
        h_Offline_Track_phi->Fill(offline_t.getPhi());
        h_Offline_Track_d0->Fill(offline_t.getD0());
        h_Offline_Track_z0->Fill(offline_t.getZ0());
    }

    h_nOffline_Tracks_before_match->Fill(nOffline_Tracks_before_match);
    h_nOffline_Tracks_before_match_0GeV->Fill(nOffline_Tracks_before_match_0GeV);
    h_nOffline_Tracks_before_match_1GeV->Fill(nOffline_Tracks_before_match_1GeV);
    h_nOffline_Tracks_before_match_2GeV->Fill(nOffline_Tracks_before_match_2GeV);
    h_nOffline_Tracks->Fill(m_nOffline_Tracks);

    //========================
    // Fake Rates
    //========================
    if (m_nTruth_Tracks_Full != 0) {
        h_ratio_HTT_1st_Truth->Fill((double)tracks_1st->size() / (double)m_nTruth_Tracks_Full);
        h_ratio_HTT_1st_passChi2_Truth->Fill((double)nTracks_1st_passChi2 / (double)m_nTruth_Tracks_Full);
        h_ratio_HTT_1st_afterOR_Truth->Fill((double)nTracks_1st_afterOR / (double)m_nTruth_Tracks_Full);
        h_ratio_Offline_Truth->Fill((double)m_nOffline_Tracks / (double)m_nTruth_Tracks_Full);
        std::cout << " nTracks_1st_passChi2 : " << nTracks_1st_passChi2 << std::endl;
    }

    if (m_nOffline_Tracks != 0) {
        h_ratio_HTT_1st_Offline->Fill((double)tracks_1st->size() / (double)m_nOffline_Tracks);
        h_ratio_HTT_1st_passChi2_Offline->Fill((double)nTracks_1st_passChi2 / (double)m_nOffline_Tracks);
        h_ratio_HTT_1st_afterOR_Offline->Fill((double)nTracks_1st_afterOR / (double)m_nOffline_Tracks);
    }

    //========================
    // some debug
    //========================
    ATH_MSG_DEBUG("Event " << m_nEvents << " truth:    " << truth_tracks.size() << ", passed " << m_nTruth_Tracks_Full);
    ATH_MSG_DEBUG("Event " << m_nEvents << " offline:  " << m_logicEventInputHeader_1st->optional().getOfflineTracks().size()
                           << ", passed " << m_nOffline_Tracks
                           << " and " << nOffline_Tracks_before_match << " before match");
    ATH_MSG_DEBUG("Event " << m_nEvents << " HTT tracks: " << tracks_1st->size() <<", passed " << nTracks_1st_afterOR
                           << ", matched " << n_HTTmatched_1st_afterOR );

    h_Truth_Track_HTT_1st_afterOR_Nmatched->Fill(n_HTTmatched_1st_afterOR);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// TODO truth tracks in single muon wrappers have event index 0, so force everything to 0 here
void HTTPerformanceMonitorTool::fillHTTTrackTruthMatchHistograms_2nd(std::vector<HTTTrack> const * tracks_2nd)
{
    if (!tracks_2nd) {
        ATH_MSG_WARNING("Failed to retrieve second stage tracks.");
        return;
    }

    std::map< long, std::vector<HTTTrack const *> > httmatchinfo;

    unsigned nTracks_2nd_passChi2 = 0;
    unsigned nTracks_2nd_afterOR = 0;

    // Information on HTT tracks relative to the hard-scattering events
    // are collected in a vector and later used to build matching maps
    for (HTTTrack const & htt_t : *tracks_2nd)
    {
        if (htt_t.getChi2ndof() <= m_cut_chi2ndof) nTracks_2nd_passChi2++;
        if (htt_t.passedOR() == 1) nTracks_2nd_afterOR++;

        if (m_evtSel->passMatching(htt_t) && htt_t.getBarcodeFrac() >= m_cut_barcodefrac)
            httmatchinfo[htt_t.getBarcode()].push_back(&htt_t);
    }

    // counters:
    unsigned n_HTTmatched_2nd = 0;
    unsigned n_HTTmatched_2nd_passChi2 = 0;
    unsigned n_HTTmatched_2nd_afterOR = 0;

    std::vector<HTTTruthTrack> truth_tracks = m_logicEventInputHeader_1st->optional().getTruthTracks();

    for (HTTTruthTrack & truth_t : truth_tracks) {
        if (!m_evtSel->passMatching(truth_t)) continue;

        //========================
        // HTT_2nd vs Truth
        //========================
        auto it_matched = httmatchinfo.find(truth_t.getBarcode());
        if (it_matched != httmatchinfo.end()) {
            n_HTTmatched_2nd++;
            // matched truth parameters
            h_Truth_Track_HTT_2nd_matched_pt->Fill(truth_t.getPt()*0.001);
            h_Truth_Track_HTT_2nd_matched_eta->Fill(truth_t.getEta());
            h_Truth_Track_HTT_2nd_matched_phi->Fill(truth_t.getPhi());
            h_Truth_Track_HTT_2nd_matched_d0->Fill(truth_t.getD0());
            h_Truth_Track_HTT_2nd_matched_z0->Fill(truth_t.getZ0());

            bool matched_passChi2 = false;
            bool matched_afterOR = false;

            HTTTrack const * besthtt = nullptr;
            HTTTrack const * besthtt_passChi2 = nullptr;
            HTTTrack const * besthtt_afterOR = nullptr;

            for (HTTTrack const * matched_t : it_matched->second) {
                if (!besthtt ||
                    (besthtt->getBarcodeFrac() < matched_t->getBarcodeFrac()) ||
                    (besthtt->getBarcodeFrac() == matched_t->getBarcodeFrac() && besthtt->getChi2ndof() > matched_t->getChi2ndof())) {
                    besthtt = matched_t;
                }

                if (matched_t->getChi2ndof() <= m_cut_chi2ndof) {
                    matched_passChi2 = true;
                    n_HTTmatched_2nd_passChi2++;
                    if (!besthtt_passChi2 ||
                        (besthtt_passChi2->getBarcodeFrac() < matched_t->getBarcodeFrac()) ||
                        (besthtt_passChi2->getBarcodeFrac() == matched_t->getBarcodeFrac() && besthtt_passChi2->getChi2ndof() > matched_t->getChi2ndof())) {
                        besthtt_passChi2 = matched_t;
                    }
                }

                if (matched_t->passedOR() == 1) {
                    matched_afterOR = true;
                    n_HTTmatched_2nd_afterOR++;
                    if (!besthtt_afterOR ||
                        (besthtt_afterOR->getBarcodeFrac() < matched_t->getBarcodeFrac()) ||
                        (besthtt_afterOR->getBarcodeFrac() == matched_t->getBarcodeFrac() && besthtt_afterOR->getChi2ndof() > matched_t->getChi2ndof())) {
                        besthtt_afterOR = matched_t;
                    }
                }
            }

            if (matched_passChi2) {
                // matched truth parameters
                h_Truth_Track_HTT_2nd_passChi2_matched_pt->Fill(truth_t.getPt()*0.001);
                h_Truth_Track_HTT_2nd_passChi2_matched_eta->Fill(truth_t.getEta());
                h_Truth_Track_HTT_2nd_passChi2_matched_phi->Fill(truth_t.getPhi());
                h_Truth_Track_HTT_2nd_passChi2_matched_d0->Fill(truth_t.getD0());
                h_Truth_Track_HTT_2nd_passChi2_matched_z0->Fill(truth_t.getZ0());
            }

            if (matched_afterOR) {
                // matched truth parameters
                h_Truth_Track_HTT_2nd_afterOR_matched_pt->Fill(truth_t.getPt()*0.001);
                h_Truth_Track_HTT_2nd_afterOR_matched_eta->Fill(truth_t.getEta());
                h_Truth_Track_HTT_2nd_afterOR_matched_phi->Fill(truth_t.getPhi());
                h_Truth_Track_HTT_2nd_afterOR_matched_d0->Fill(truth_t.getD0());
                h_Truth_Track_HTT_2nd_afterOR_matched_z0->Fill(truth_t.getZ0());
            }

            if (besthtt) {
                h_HTT_2nd_vs_Truth_Res_truthmatch_pt->Fill((besthtt->getPt() - truth_t.getPt()) * 0.001);
                h_HTT_2nd_vs_Truth_Res_truthmatch_eta->Fill(besthtt->getEta() - truth_t.getEta());
                h_HTT_2nd_vs_Truth_Res_truthmatch_phi->Fill(besthtt->getPhi() - truth_t.getPhi());
                h_HTT_2nd_vs_Truth_Res_truthmatch_d0->Fill(besthtt->getD0() - truth_t.getD0());
                h_HTT_2nd_vs_Truth_Res_truthmatch_z0->Fill(besthtt->getZ0() - truth_t.getZ0());
                h_HTT_2nd_vs_Truth_Res_truthmatch_qoverpt->Fill((abs(besthtt->getQOverPt()) - abs(truth_t.getQOverPt())) * 1000.);

                h_HTT_2nd_vs_Truth_Res_truthmatch_pt_vspt->Fill(truth_t.getPt() * 0.001, (besthtt->getPt() - truth_t.getPt()) * 0.001);
                h_HTT_2nd_vs_Truth_Res_truthmatch_eta_vspt->Fill(truth_t.getPt() * 0.001, besthtt->getEta() - truth_t.getEta());
                h_HTT_2nd_vs_Truth_Res_truthmatch_phi_vspt->Fill(truth_t.getPt() * 0.001, besthtt->getPhi() - truth_t.getPhi());
                h_HTT_2nd_vs_Truth_Res_truthmatch_d0_vspt->Fill(truth_t.getPt() * 0.001, besthtt->getD0() - truth_t.getD0());
                h_HTT_2nd_vs_Truth_Res_truthmatch_z0_vspt->Fill(truth_t.getPt() * 0.001, besthtt->getZ0() - truth_t.getZ0());
                h_HTT_2nd_vs_Truth_Res_truthmatch_qoverpt_vspt->Fill(truth_t.getPt() * 0.001, (abs(besthtt->getQOverPt()) - abs(truth_t.getQOverPt())) * 1000.);

                h_HTT_2nd_vs_Truth_Res_truthmatch_pt_vseta->Fill(truth_t.getEta(), (besthtt->getPt() - truth_t.getPt()) * 0.001);
                h_HTT_2nd_vs_Truth_Res_truthmatch_eta_vseta->Fill(truth_t.getEta(), besthtt->getEta() - truth_t.getEta());
                h_HTT_2nd_vs_Truth_Res_truthmatch_phi_vseta->Fill(truth_t.getEta(), besthtt->getPhi() - truth_t.getPhi());
                h_HTT_2nd_vs_Truth_Res_truthmatch_d0_vseta->Fill(truth_t.getEta(), besthtt->getD0() - truth_t.getD0());
                h_HTT_2nd_vs_Truth_Res_truthmatch_z0_vseta->Fill(truth_t.getEta(), besthtt->getZ0() - truth_t.getZ0());
                h_HTT_2nd_vs_Truth_Res_truthmatch_qoverpt_vseta->Fill(truth_t.getEta(), (abs(besthtt->getQOverPt()) - abs(truth_t.getQOverPt())) * 1000.);

                if (m_bestHTTTrack_1st) {
                    h_HTT_2nd_vs_HTT_1st_Res_truthmatch_pt->Fill((besthtt->getPt() - m_bestHTTTrack_1st->getPt()) * 0.001);
                    h_HTT_2nd_vs_HTT_1st_Res_truthmatch_eta->Fill(besthtt->getEta() - m_bestHTTTrack_1st->getEta());
                    h_HTT_2nd_vs_HTT_1st_Res_truthmatch_phi->Fill(besthtt->getPhi() - m_bestHTTTrack_1st->getPhi());
                    h_HTT_2nd_vs_HTT_1st_Res_truthmatch_d0->Fill(besthtt->getD0() - m_bestHTTTrack_1st->getD0());
                    h_HTT_2nd_vs_HTT_1st_Res_truthmatch_z0->Fill(besthtt->getZ0() - m_bestHTTTrack_1st->getZ0());
                    h_HTT_2nd_vs_HTT_1st_Res_truthmatch_qoverpt->Fill((abs(besthtt->getQOverPt()) - abs(m_bestHTTTrack_1st->getQOverPt())) * 1000.);

                    h_HTT_2nd_vs_HTT_1st_Res_truthmatch_pt_vspt->Fill(truth_t.getPt() * 0.001, (besthtt->getPt() - m_bestHTTTrack_1st->getPt()) * 0.001);
                    h_HTT_2nd_vs_HTT_1st_Res_truthmatch_eta_vspt->Fill(truth_t.getPt() * 0.001, besthtt->getEta() - m_bestHTTTrack_1st->getEta());
                    h_HTT_2nd_vs_HTT_1st_Res_truthmatch_phi_vspt->Fill(truth_t.getPt() * 0.001, besthtt->getPhi() - m_bestHTTTrack_1st->getPhi());
                    h_HTT_2nd_vs_HTT_1st_Res_truthmatch_d0_vspt->Fill(truth_t.getPt() * 0.001, besthtt->getD0() - m_bestHTTTrack_1st->getD0());
                    h_HTT_2nd_vs_HTT_1st_Res_truthmatch_z0_vspt->Fill(truth_t.getPt() * 0.001, besthtt->getZ0() - m_bestHTTTrack_1st->getZ0());
                    h_HTT_2nd_vs_HTT_1st_Res_truthmatch_qoverpt_vspt->Fill(truth_t.getPt() * 0.001, (abs(besthtt->getQOverPt()) - abs(m_bestHTTTrack_1st->getQOverPt())) * 1000.);

                    h_HTT_2nd_vs_HTT_1st_Res_truthmatch_pt_vseta->Fill(truth_t.getEta(), (besthtt->getPt() - m_bestHTTTrack_1st->getPt()) * 0.001);
                    h_HTT_2nd_vs_HTT_1st_Res_truthmatch_eta_vseta->Fill(truth_t.getEta(), besthtt->getEta() - m_bestHTTTrack_1st->getEta());
                    h_HTT_2nd_vs_HTT_1st_Res_truthmatch_phi_vseta->Fill(truth_t.getEta(), besthtt->getPhi() - m_bestHTTTrack_1st->getPhi());
                    h_HTT_2nd_vs_HTT_1st_Res_truthmatch_d0_vseta->Fill(truth_t.getEta(), besthtt->getD0() - m_bestHTTTrack_1st->getD0());
                    h_HTT_2nd_vs_HTT_1st_Res_truthmatch_z0_vseta->Fill(truth_t.getEta(), besthtt->getZ0() - m_bestHTTTrack_1st->getZ0());
                    h_HTT_2nd_vs_HTT_1st_Res_truthmatch_qoverpt_vseta->Fill(truth_t.getEta(), (abs(besthtt->getQOverPt()) - abs(m_bestHTTTrack_1st->getQOverPt())) * 1000.);
                }
            }

            if (besthtt_passChi2) {
                h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_pt->Fill((besthtt_passChi2->getPt() - truth_t.getPt()) * 0.001);
                h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_eta->Fill(besthtt_passChi2->getEta() - truth_t.getEta());
                h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_phi->Fill(besthtt_passChi2->getPhi() - truth_t.getPhi());
                h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_d0->Fill(besthtt_passChi2->getD0() - truth_t.getD0());
                h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_z0->Fill(besthtt_passChi2->getZ0() - truth_t.getZ0());
                h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_qoverpt->Fill((abs(besthtt_passChi2->getQOverPt()) - abs(truth_t.getQOverPt())) * 1000.);

                h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_pt_vspt->Fill(truth_t.getPt() * 0.001, (besthtt_passChi2->getPt() - truth_t.getPt()) * 0.001);
                h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_eta_vspt->Fill(truth_t.getPt() * 0.001, besthtt_passChi2->getEta() - truth_t.getEta());
                h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_phi_vspt->Fill(truth_t.getPt() * 0.001, besthtt_passChi2->getPhi() - truth_t.getPhi());
                h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_d0_vspt->Fill(truth_t.getPt() * 0.001, besthtt_passChi2->getD0() - truth_t.getD0());
                h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_z0_vspt->Fill(truth_t.getPt() * 0.001, besthtt_passChi2->getZ0() - truth_t.getZ0());
                h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_qoverpt_vspt->Fill(truth_t.getPt() * 0.001, (abs(besthtt_passChi2->getQOverPt()) - abs(truth_t.getQOverPt())) * 1000.);

                h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_pt_vseta->Fill(truth_t.getEta(), (besthtt_passChi2->getPt() - truth_t.getPt()) * 0.001);
                h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_eta_vseta->Fill(truth_t.getEta(), besthtt_passChi2->getEta() - truth_t.getEta());
                h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_phi_vseta->Fill(truth_t.getEta(), besthtt_passChi2->getPhi() - truth_t.getPhi());
                h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_d0_vseta->Fill(truth_t.getEta(), besthtt_passChi2->getD0() - truth_t.getD0());
                h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_z0_vseta->Fill(truth_t.getEta(), besthtt_passChi2->getZ0() - truth_t.getZ0());
                h_HTT_2nd_vs_Truth_passChi2_Res_truthmatch_qoverpt_vseta->Fill(truth_t.getEta(), (abs(besthtt_passChi2->getQOverPt()) - abs(truth_t.getQOverPt())) * 1000.);
            }

            if (besthtt_afterOR) {
                h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_pt->Fill((besthtt_afterOR->getPt() - truth_t.getPt()) * 0.001);
                h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_eta->Fill(besthtt_afterOR->getEta() - truth_t.getEta());
                h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_phi->Fill(besthtt_afterOR->getPhi() - truth_t.getPhi());
                h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_d0->Fill(besthtt_afterOR->getD0() - truth_t.getD0());
                h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_z0->Fill(besthtt_afterOR->getZ0() - truth_t.getZ0());
                h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_qoverpt->Fill((abs(besthtt_afterOR->getQOverPt()) - abs(truth_t.getQOverPt())) * 1000.);

                h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_pt_vspt->Fill(truth_t.getPt() * 0.001, (besthtt_afterOR->getPt() - truth_t.getPt()) * 0.001);
                h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_eta_vspt->Fill(truth_t.getPt() * 0.001, besthtt_afterOR->getEta() - truth_t.getEta());
                h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_phi_vspt->Fill(truth_t.getPt() * 0.001, besthtt_afterOR->getPhi() - truth_t.getPhi());
                h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_d0_vspt->Fill(truth_t.getPt() * 0.001, besthtt_afterOR->getD0() - truth_t.getD0());
                h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_z0_vspt->Fill(truth_t.getPt() * 0.001, besthtt_afterOR->getZ0() - truth_t.getZ0());
                h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_qoverpt_vspt->Fill(truth_t.getPt() * 0.001, (abs(besthtt_afterOR->getQOverPt()) - abs(truth_t.getQOverPt())) * 1000.);

                h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_pt_vseta->Fill(truth_t.getEta(), (besthtt_afterOR->getPt() - truth_t.getPt()) * 0.001);
                h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_eta_vseta->Fill(truth_t.getEta(), besthtt_afterOR->getEta() - truth_t.getEta());
                h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_phi_vseta->Fill(truth_t.getEta(), besthtt_afterOR->getPhi() - truth_t.getPhi());
                h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_d0_vseta->Fill(truth_t.getEta(), besthtt_afterOR->getD0() - truth_t.getD0());
                h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_z0_vseta->Fill(truth_t.getEta(), besthtt_afterOR->getZ0() - truth_t.getZ0());
                h_HTT_2nd_vs_Truth_afterOR_Res_truthmatch_qoverpt_vseta->Fill(truth_t.getEta(), (abs(besthtt_afterOR->getQOverPt()) - abs(truth_t.getQOverPt())) * 1000.);
            }
        }
    }

    //========================
    // Fake Rates
    //========================
    if (m_nTruth_Tracks_Full != 0) {
        h_ratio_HTT_2nd_Truth->Fill((double)tracks_2nd->size() / (double)m_nTruth_Tracks_Full);
        h_ratio_HTT_2nd_passChi2_Truth->Fill((double)nTracks_2nd_passChi2 / (double)m_nTruth_Tracks_Full);
        h_ratio_HTT_2nd_afterOR_Truth->Fill((double)nTracks_2nd_afterOR / (double)m_nTruth_Tracks_Full);
    }

    if (m_nOffline_Tracks != 0) {
        h_ratio_HTT_2nd_Offline->Fill((double)tracks_2nd->size() / (double)m_nOffline_Tracks);
        h_ratio_HTT_2nd_passChi2_Offline->Fill((double)nTracks_2nd_passChi2 / (double)m_nOffline_Tracks);
        h_ratio_HTT_2nd_afterOR_Offline->Fill((double)nTracks_2nd_afterOR / (double)m_nOffline_Tracks);
    }

    //========================
    // some debug
    //========================
    ATH_MSG_DEBUG("Event "<<m_nEvents<<" Second Stage HTT tracks: " << tracks_2nd->size() <<", passed "<< nTracks_2nd_afterOR
                            <<", matched "<<n_HTTmatched_2nd_afterOR );

    h_Truth_Track_HTT_2nd_afterOR_Nmatched->Fill(n_HTTmatched_2nd_afterOR);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void HTTPerformanceMonitorTool::makeCrudeEffHistograms_1st()
{
    getEffJMS(h_RoadEfficiency_1st_pt, h_Truth_Track_withroad_1st_pt, h_Truth_Track_Leading_pt);
    getEffJMS(h_RoadEfficiency_1st_eta, h_Truth_Track_withroad_1st_eta, h_Truth_Track_Leading_eta);
    getEffJMS(h_RoadEfficiency_1st_phi, h_Truth_Track_withroad_1st_phi, h_Truth_Track_Leading_phi);
    getEffJMS(h_RoadEfficiency_1st_d0, h_Truth_Track_withroad_1st_d0, h_Truth_Track_Leading_d0);
    getEffJMS(h_RoadEfficiency_1st_z0, h_Truth_Track_withroad_1st_z0, h_Truth_Track_Leading_z0);

    getEffJMS(h_TrackEfficiency_1st_pt, h_Truth_Track_withtrack_1st_pt, h_Truth_Track_withroad_1st_pt);
    getEffJMS(h_TrackEfficiency_1st_eta, h_Truth_Track_withtrack_1st_eta, h_Truth_Track_withroad_1st_eta);
    getEffJMS(h_TrackEfficiency_1st_phi, h_Truth_Track_withtrack_1st_phi, h_Truth_Track_withroad_1st_phi);
    getEffJMS(h_TrackEfficiency_1st_d0, h_Truth_Track_withtrack_1st_d0, h_Truth_Track_withroad_1st_d0);
    getEffJMS(h_TrackEfficiency_1st_z0, h_Truth_Track_withtrack_1st_z0, h_Truth_Track_withroad_1st_z0);

    getEffJMS(h_FullEfficiency_1st_pt, h_Truth_Track_withtrack_1st_pt, h_Truth_Track_Leading_pt);
    getEffJMS(h_FullEfficiency_1st_eta, h_Truth_Track_withtrack_1st_eta, h_Truth_Track_Leading_eta);
    getEffJMS(h_FullEfficiency_1st_phi, h_Truth_Track_withtrack_1st_phi, h_Truth_Track_Leading_phi);
    getEffJMS(h_FullEfficiency_1st_d0, h_Truth_Track_withtrack_1st_d0, h_Truth_Track_Leading_d0);
    getEffJMS(h_FullEfficiency_1st_z0, h_Truth_Track_withtrack_1st_z0, h_Truth_Track_Leading_z0);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void HTTPerformanceMonitorTool::makeCrudeEffHistograms_2nd()
{
    getEff(h_RoadEfficiency_2nd_pt, h_Truth_Track_withroad_2nd_pt, h_Truth_Track_Leading_pt);
    getEff(h_RoadEfficiency_2nd_eta, h_Truth_Track_withroad_2nd_eta, h_Truth_Track_Leading_eta);
    getEff(h_RoadEfficiency_2nd_phi, h_Truth_Track_withroad_2nd_phi, h_Truth_Track_Leading_phi);
    getEff(h_RoadEfficiency_2nd_d0, h_Truth_Track_withroad_2nd_d0, h_Truth_Track_Leading_d0);
    getEff(h_RoadEfficiency_2nd_z0, h_Truth_Track_withroad_2nd_z0, h_Truth_Track_Leading_z0);

    getEff(h_TrackEfficiency_2nd_pt, h_Truth_Track_withtrack_2nd_pt, h_Truth_Track_withroad_2nd_pt);
    getEff(h_TrackEfficiency_2nd_eta, h_Truth_Track_withtrack_2nd_eta, h_Truth_Track_withroad_2nd_eta);
    getEff(h_TrackEfficiency_2nd_phi, h_Truth_Track_withtrack_2nd_phi, h_Truth_Track_withroad_2nd_phi);
    getEff(h_TrackEfficiency_2nd_d0, h_Truth_Track_withtrack_2nd_d0, h_Truth_Track_withroad_2nd_d0);
    getEff(h_TrackEfficiency_2nd_z0, h_Truth_Track_withtrack_2nd_z0, h_Truth_Track_withroad_2nd_z0);

    getEff(h_FullEfficiency_2nd_pt, h_Truth_Track_withtrack_2nd_pt, h_Truth_Track_Leading_pt);
    getEff(h_FullEfficiency_2nd_eta, h_Truth_Track_withtrack_2nd_eta, h_Truth_Track_Leading_eta);
    getEff(h_FullEfficiency_2nd_phi, h_Truth_Track_withtrack_2nd_phi, h_Truth_Track_Leading_phi);
    getEff(h_FullEfficiency_2nd_d0, h_Truth_Track_withtrack_2nd_d0, h_Truth_Track_Leading_d0);
    getEff(h_FullEfficiency_2nd_z0, h_Truth_Track_withtrack_2nd_z0, h_Truth_Track_Leading_z0);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void HTTPerformanceMonitorTool::makeTruthMatchEffHistograms_1st()
{
    // HTT vs Truth Road Efficiency for first stage
    getEffJMS(h_HTT_1st_vs_Truth_Road_eff_pt, h_Truth_Track_Road_1st_matched_pt, h_Truth_Track_Full_pt);
    getEffJMS(h_HTT_1st_vs_Truth_Road_eff_eta, h_Truth_Track_Road_1st_matched_eta, h_Truth_Track_Full_eta);
    getEffJMS(h_HTT_1st_vs_Truth_Road_eff_phi, h_Truth_Track_Road_1st_matched_phi, h_Truth_Track_Full_phi);
    getEffJMS(h_HTT_1st_vs_Truth_Road_eff_d0, h_Truth_Track_Road_1st_matched_d0, h_Truth_Track_Full_d0);
    getEffJMS(h_HTT_1st_vs_Truth_Road_eff_z0, h_Truth_Track_Road_1st_matched_z0, h_Truth_Track_Full_z0);

    // HTT vs Offline Road Efficiency for first stage
    getEff(h_HTT_1st_vs_Offline_Road_eff_pt, h_Truth_Track_Road_1st_matched_pt, h_Truth_Track_Offline_matched_pt);
    getEff(h_HTT_1st_vs_Offline_Road_eff_eta, h_Truth_Track_Road_1st_matched_eta, h_Truth_Track_Offline_matched_eta);
    getEff(h_HTT_1st_vs_Offline_Road_eff_phi, h_Truth_Track_Road_1st_matched_phi, h_Truth_Track_Offline_matched_phi);
    getEff(h_HTT_1st_vs_Offline_Road_eff_d0, h_Truth_Track_Road_1st_matched_d0, h_Truth_Track_Offline_matched_d0);
    getEff(h_HTT_1st_vs_Offline_Road_eff_z0, h_Truth_Track_Road_1st_matched_z0, h_Truth_Track_Offline_matched_z0);

    // HTT vs Truth for first stage
    getEff(h_HTT_1st_vs_Truth_eff_pt, h_Truth_Track_HTT_1st_matched_pt, h_Truth_Track_Full_pt);
    getEff(h_HTT_1st_vs_Truth_eff_eta, h_Truth_Track_HTT_1st_matched_eta, h_Truth_Track_Full_eta);
    getEff(h_HTT_1st_vs_Truth_eff_phi, h_Truth_Track_HTT_1st_matched_phi, h_Truth_Track_Full_phi);
    getEff(h_HTT_1st_vs_Truth_eff_d0, h_Truth_Track_HTT_1st_matched_d0, h_Truth_Track_Full_d0);
    getEff(h_HTT_1st_vs_Truth_eff_z0, h_Truth_Track_HTT_1st_matched_z0, h_Truth_Track_Full_z0);

    // HTT vs Truth (passing Chi^2 cut) for first stage
    getEff(h_HTT_1st_vs_Truth_passChi2_eff_pt, h_Truth_Track_HTT_1st_passChi2_matched_pt, h_Truth_Track_Full_pt);
    getEff(h_HTT_1st_vs_Truth_passChi2_eff_eta, h_Truth_Track_HTT_1st_passChi2_matched_eta, h_Truth_Track_Full_eta);
    getEff(h_HTT_1st_vs_Truth_passChi2_eff_phi, h_Truth_Track_HTT_1st_passChi2_matched_phi, h_Truth_Track_Full_phi);
    getEff(h_HTT_1st_vs_Truth_passChi2_eff_d0, h_Truth_Track_HTT_1st_passChi2_matched_d0, h_Truth_Track_Full_d0);
    getEff(h_HTT_1st_vs_Truth_passChi2_eff_z0, h_Truth_Track_HTT_1st_passChi2_matched_z0, h_Truth_Track_Full_z0);

    // HTT vs Truth (after overlap removal) for first stage
    getEff(h_HTT_1st_vs_Truth_afterOR_eff_pt, h_Truth_Track_HTT_1st_afterOR_matched_pt, h_Truth_Track_Full_pt);
    getEff(h_HTT_1st_vs_Truth_afterOR_eff_eta, h_Truth_Track_HTT_1st_afterOR_matched_eta, h_Truth_Track_Full_eta);
    getEff(h_HTT_1st_vs_Truth_afterOR_eff_phi, h_Truth_Track_HTT_1st_afterOR_matched_phi, h_Truth_Track_Full_phi);
    getEff(h_HTT_1st_vs_Truth_afterOR_eff_d0, h_Truth_Track_HTT_1st_afterOR_matched_d0, h_Truth_Track_Full_d0);
    getEff(h_HTT_1st_vs_Truth_afterOR_eff_z0, h_Truth_Track_HTT_1st_afterOR_matched_z0, h_Truth_Track_Full_z0);

    // Offline vs Truth
    getEff(h_Offline_vs_Truth_eff_pt, h_Truth_Track_Offline_matched_pt, h_Truth_Track_Full_pt);
    getEff(h_Offline_vs_Truth_eff_eta, h_Truth_Track_Offline_matched_eta, h_Truth_Track_Full_eta);
    getEff(h_Offline_vs_Truth_eff_phi, h_Truth_Track_Offline_matched_phi, h_Truth_Track_Full_phi);
    getEff(h_Offline_vs_Truth_eff_d0, h_Truth_Track_Offline_matched_d0, h_Truth_Track_Full_d0);
    getEff(h_Offline_vs_Truth_eff_z0, h_Truth_Track_Offline_matched_z0, h_Truth_Track_Full_z0);

    // HTT vs Offline (All) for first stage
    getEff(h_HTT_1st_vs_Offline_eff_pt, h_Truth_Track_HTT_1st_matched_pt, h_Truth_Track_Offline_matched_pt);
    getEff(h_HTT_1st_vs_Offline_eff_eta, h_Truth_Track_HTT_1st_matched_eta, h_Truth_Track_Offline_matched_eta);
    getEff(h_HTT_1st_vs_Offline_eff_phi, h_Truth_Track_HTT_1st_matched_phi, h_Truth_Track_Offline_matched_phi);
    getEff(h_HTT_1st_vs_Offline_eff_d0, h_Truth_Track_HTT_1st_matched_d0, h_Truth_Track_Offline_matched_d0);
    getEff(h_HTT_1st_vs_Offline_eff_z0, h_Truth_Track_HTT_1st_matched_z0, h_Truth_Track_Offline_matched_z0);

    // HTT vs Offline (passing Chi^2 cut) for first stage
    getEff(h_HTT_1st_vs_Offline_passChi2_eff_pt, h_Truth_Track_HTT_1st_passChi2_matched_pt, h_Truth_Track_Offline_matched_pt);
    getEff(h_HTT_1st_vs_Offline_passChi2_eff_eta, h_Truth_Track_HTT_1st_passChi2_matched_eta, h_Truth_Track_Offline_matched_eta);
    getEff(h_HTT_1st_vs_Offline_passChi2_eff_phi, h_Truth_Track_HTT_1st_passChi2_matched_phi, h_Truth_Track_Offline_matched_phi);
    getEff(h_HTT_1st_vs_Offline_passChi2_eff_d0, h_Truth_Track_HTT_1st_passChi2_matched_d0, h_Truth_Track_Offline_matched_d0);
    getEff(h_HTT_1st_vs_Offline_passChi2_eff_z0, h_Truth_Track_HTT_1st_passChi2_matched_z0, h_Truth_Track_Offline_matched_z0);

    // HTT vs Offline (after overlap removal) for first stage
    getEff(h_HTT_1st_vs_Offline_afterOR_eff_pt, h_Truth_Track_HTT_1st_afterOR_matched_pt, h_Truth_Track_Offline_matched_pt);
    getEff(h_HTT_1st_vs_Offline_afterOR_eff_eta, h_Truth_Track_HTT_1st_afterOR_matched_eta, h_Truth_Track_Offline_matched_eta);
    getEff(h_HTT_1st_vs_Offline_afterOR_eff_phi, h_Truth_Track_HTT_1st_afterOR_matched_phi, h_Truth_Track_Offline_matched_phi);
    getEff(h_HTT_1st_vs_Offline_afterOR_eff_d0, h_Truth_Track_HTT_1st_afterOR_matched_d0, h_Truth_Track_Offline_matched_d0);
    getEff(h_HTT_1st_vs_Offline_afterOR_eff_z0, h_Truth_Track_HTT_1st_afterOR_matched_z0, h_Truth_Track_Offline_matched_z0);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void HTTPerformanceMonitorTool::makeTruthMatchEffHistograms_2nd()
{
    // HTT vs Truth Road Efficiency for second stage
    getEff(h_HTT_2nd_vs_Truth_Road_eff_pt, h_Truth_Track_Road_2nd_matched_pt, h_Truth_Track_Full_pt);
    getEff(h_HTT_2nd_vs_Truth_Road_eff_eta, h_Truth_Track_Road_2nd_matched_eta, h_Truth_Track_Full_eta);
    getEff(h_HTT_2nd_vs_Truth_Road_eff_phi, h_Truth_Track_Road_2nd_matched_phi, h_Truth_Track_Full_phi);
    getEff(h_HTT_2nd_vs_Truth_Road_eff_d0, h_Truth_Track_Road_2nd_matched_d0, h_Truth_Track_Full_d0);
    getEff(h_HTT_2nd_vs_Truth_Road_eff_z0, h_Truth_Track_Road_2nd_matched_z0, h_Truth_Track_Full_z0);

    // HTT vs Offline Road Efficiency for second stage
    getEff(h_HTT_2nd_vs_Offline_Road_eff_pt, h_Truth_Track_Road_2nd_matched_pt, h_Truth_Track_Offline_matched_pt);
    getEff(h_HTT_2nd_vs_Offline_Road_eff_eta, h_Truth_Track_Road_2nd_matched_eta, h_Truth_Track_Offline_matched_eta);
    getEff(h_HTT_2nd_vs_Offline_Road_eff_phi, h_Truth_Track_Road_2nd_matched_phi, h_Truth_Track_Offline_matched_phi);
    getEff(h_HTT_2nd_vs_Offline_Road_eff_d0, h_Truth_Track_Road_2nd_matched_d0, h_Truth_Track_Offline_matched_d0);
    getEff(h_HTT_2nd_vs_Offline_Road_eff_z0, h_Truth_Track_Road_2nd_matched_z0, h_Truth_Track_Offline_matched_z0);

    // HTT vs Truth for second stage
    getEff(h_HTT_2nd_vs_Truth_eff_pt, h_Truth_Track_HTT_2nd_matched_pt, h_Truth_Track_Full_pt);
    getEff(h_HTT_2nd_vs_Truth_eff_eta, h_Truth_Track_HTT_2nd_matched_eta, h_Truth_Track_Full_eta);
    getEff(h_HTT_2nd_vs_Truth_eff_phi, h_Truth_Track_HTT_2nd_matched_phi, h_Truth_Track_Full_phi);
    getEff(h_HTT_2nd_vs_Truth_eff_d0, h_Truth_Track_HTT_2nd_matched_d0, h_Truth_Track_Full_d0);
    getEff(h_HTT_2nd_vs_Truth_eff_z0, h_Truth_Track_HTT_2nd_matched_z0, h_Truth_Track_Full_z0);

    // HTT vs Truth (passing Chi^2 cut) for second stage
    getEff(h_HTT_2nd_vs_Truth_passChi2_eff_pt, h_Truth_Track_HTT_2nd_passChi2_matched_pt, h_Truth_Track_Full_pt);
    getEff(h_HTT_2nd_vs_Truth_passChi2_eff_eta, h_Truth_Track_HTT_2nd_passChi2_matched_eta, h_Truth_Track_Full_eta);
    getEff(h_HTT_2nd_vs_Truth_passChi2_eff_phi, h_Truth_Track_HTT_2nd_passChi2_matched_phi, h_Truth_Track_Full_phi);
    getEff(h_HTT_2nd_vs_Truth_passChi2_eff_d0, h_Truth_Track_HTT_2nd_passChi2_matched_d0, h_Truth_Track_Full_d0);
    getEff(h_HTT_2nd_vs_Truth_passChi2_eff_z0, h_Truth_Track_HTT_2nd_passChi2_matched_z0, h_Truth_Track_Full_z0);

    // HTT vs Truth (after overlap removal) for second stage
    getEff(h_HTT_2nd_vs_Truth_afterOR_eff_pt, h_Truth_Track_HTT_2nd_afterOR_matched_pt, h_Truth_Track_Full_pt);
    getEff(h_HTT_2nd_vs_Truth_afterOR_eff_eta, h_Truth_Track_HTT_2nd_afterOR_matched_eta, h_Truth_Track_Full_eta);
    getEff(h_HTT_2nd_vs_Truth_afterOR_eff_phi, h_Truth_Track_HTT_2nd_afterOR_matched_phi, h_Truth_Track_Full_phi);
    getEff(h_HTT_2nd_vs_Truth_afterOR_eff_d0, h_Truth_Track_HTT_2nd_afterOR_matched_d0, h_Truth_Track_Full_d0);
    getEff(h_HTT_2nd_vs_Truth_afterOR_eff_z0, h_Truth_Track_HTT_2nd_afterOR_matched_z0, h_Truth_Track_Full_z0);

    // HTT vs Offline (All) for second stage
    getEff(h_HTT_2nd_vs_Offline_eff_pt, h_Truth_Track_HTT_2nd_matched_pt, h_Truth_Track_Offline_matched_pt);
    getEff(h_HTT_2nd_vs_Offline_eff_eta, h_Truth_Track_HTT_2nd_matched_eta, h_Truth_Track_Offline_matched_eta);
    getEff(h_HTT_2nd_vs_Offline_eff_phi, h_Truth_Track_HTT_2nd_matched_phi, h_Truth_Track_Offline_matched_phi);
    getEff(h_HTT_2nd_vs_Offline_eff_d0, h_Truth_Track_HTT_2nd_matched_d0, h_Truth_Track_Offline_matched_d0);
    getEff(h_HTT_2nd_vs_Offline_eff_z0, h_Truth_Track_HTT_2nd_matched_z0, h_Truth_Track_Offline_matched_z0);

    // HTT vs Offline (passing Chi^2 cut) for second stage
    getEff(h_HTT_2nd_vs_Offline_passChi2_eff_pt, h_Truth_Track_HTT_2nd_passChi2_matched_pt, h_Truth_Track_Offline_matched_pt);
    getEff(h_HTT_2nd_vs_Offline_passChi2_eff_eta, h_Truth_Track_HTT_2nd_passChi2_matched_eta, h_Truth_Track_Offline_matched_eta);
    getEff(h_HTT_2nd_vs_Offline_passChi2_eff_phi, h_Truth_Track_HTT_2nd_passChi2_matched_phi, h_Truth_Track_Offline_matched_phi);
    getEff(h_HTT_2nd_vs_Offline_passChi2_eff_d0, h_Truth_Track_HTT_2nd_passChi2_matched_d0, h_Truth_Track_Offline_matched_d0);
    getEff(h_HTT_2nd_vs_Offline_passChi2_eff_z0, h_Truth_Track_HTT_2nd_passChi2_matched_z0, h_Truth_Track_Offline_matched_z0);

    // HTT vs Offline (after overlap removal) for second stage
    getEff(h_HTT_2nd_vs_Offline_afterOR_eff_pt, h_Truth_Track_HTT_2nd_afterOR_matched_pt, h_Truth_Track_Offline_matched_pt);
    getEff(h_HTT_2nd_vs_Offline_afterOR_eff_eta, h_Truth_Track_HTT_2nd_afterOR_matched_eta, h_Truth_Track_Offline_matched_eta);
    getEff(h_HTT_2nd_vs_Offline_afterOR_eff_phi, h_Truth_Track_HTT_2nd_afterOR_matched_phi, h_Truth_Track_Offline_matched_phi);
    getEff(h_HTT_2nd_vs_Offline_afterOR_eff_d0, h_Truth_Track_HTT_2nd_afterOR_matched_d0, h_Truth_Track_Offline_matched_d0);
    getEff(h_HTT_2nd_vs_Offline_afterOR_eff_z0, h_Truth_Track_HTT_2nd_afterOR_matched_z0, h_Truth_Track_Offline_matched_z0);

    // Second stage vs First stage track efficiency
    getEff(h_HTT_2nd_vs_HTT_1st_eff_pt, h_Truth_Track_HTT_2nd_matched_pt, h_Truth_Track_HTT_1st_matched_pt);
    getEff(h_HTT_2nd_vs_HTT_1st_eff_eta, h_Truth_Track_HTT_2nd_matched_eta, h_Truth_Track_HTT_1st_matched_eta);
    getEff(h_HTT_2nd_vs_HTT_1st_eff_phi, h_Truth_Track_HTT_2nd_matched_phi, h_Truth_Track_HTT_1st_matched_phi);
    getEff(h_HTT_2nd_vs_HTT_1st_eff_d0, h_Truth_Track_HTT_2nd_matched_d0, h_Truth_Track_HTT_1st_matched_d0);
    getEff(h_HTT_2nd_vs_HTT_1st_eff_z0, h_Truth_Track_HTT_2nd_matched_z0, h_Truth_Track_HTT_1st_matched_z0);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void HTTPerformanceMonitorTool::calculateTruth(std::vector<HTTRoad*> const * roads, std::map<typename HTTMultiTruth::Barcode, std::pair<std::vector<HTTRoad const *>, float>> & m)
{
    for (HTTRoad const * r : *roads) {
        HTTMultiTruth truth(r->getTruth());
        for (auto const & bw : truth) {
            HTTMultiTruth::Barcode code = bw.first;
            code.first = 0;
            auto & val = m[code];
            if (bw.second > m_cut_barcodefrac)
                val.first.push_back(r);
            if (bw.second > val.second)
                val.second = bw.second;
        }
    }
}
