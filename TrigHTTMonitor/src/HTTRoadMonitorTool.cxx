/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTMonitor/HTTRoadMonitorTool.h"

#include "TrigHTTObjects/HTTLogicalEventInputHeader.h"
#include "TrigHTTObjects/HTTLogicalEventOutputHeader.h"
#include "TrigHTTUtils/HTTHash.h"


static const InterfaceID IID_HTTRoadMonitorTool("HTTRoadMonitorTool", 1, 0);
const InterfaceID& HTTRoadMonitorTool::interfaceID()
{ return IID_HTTRoadMonitorTool; }


/////////////////////////////////////////////////////////////////////////////
HTTRoadMonitorTool::HTTRoadMonitorTool(std::string const & algname, std::string const & name, IInterface const * ifc) :
    HTTMonitorBase(algname,name,ifc)
{
    declareInterface<HTTRoadMonitorTool>(this);
    declareProperty("fastMon",              m_fast,                 "only do fast monitoring");
    declareProperty("RunSecondStage",       m_runSecondStage,       "flag to enable running the second stage fitting");
    declareProperty("canExtendHistRanges",  m_extend,               "extend x-axes of some histograms");
    declareProperty("BarcodeFracCut",       m_cut_barcodefrac,      "cut on barcode fraction used in truth matching");
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTRoadMonitorTool::initialize()
{
    ATH_MSG_INFO("HTTRoadMonitorTool::initialize()");
    ATH_CHECK(HTTMonitorBase::initialize());
    ATH_CHECK(bookHistograms());
    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTRoadMonitorTool::bookHistograms()
{
    ATH_CHECK(bookHTTRoadHistograms_1st());
    if (m_runSecondStage) ATH_CHECK(bookHTTRoadHistograms_2nd());
    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTRoadMonitorTool::fillHistograms()
{
    // Use event selection to pass or reject event based on truth info, sample type etc
    ATH_CHECK(selectEvent());

    std::vector<HTTRoad*> const * roads_1st = nullptr;
    std::vector<HTTRoad*> const * roads_2nd = nullptr;

    std::vector<HTTRoad*> proads_1st, proads_2nd;

    if (!m_logicEventOutputHeader) {
        roads_1st = getMonitorRoads("Roads_1st");
        if (m_runSecondStage) roads_2nd = getMonitorRoads("Roads_2nd");
    }
    else {
        m_logicEventOutputHeader->getHTTRoads_1st(proads_1st);
        roads_1st = &proads_1st;
        if (m_runSecondStage) {
            m_logicEventOutputHeader->getHTTRoads_2nd(proads_2nd);
            roads_2nd = &proads_2nd;
        }
    }

    fillHTTRoadHistograms_1st(roads_1st);
    if (m_runSecondStage) fillHTTRoadHistograms_2nd(roads_2nd);

    clearMonitorData();

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTRoadMonitorTool::finalize()
{
    ATH_MSG_INFO("HTTRoadMonitorTool::finalize()");
    ATH_CHECK(HTTMonitorBase::finalize());
    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTRoadMonitorTool::bookHTTRoadHistograms_1st()
{
    setHistDir(createDirName("/HTTRoadHist_1st/AllRoadHist/"));

    h_nRoads_1st = new TH1I("h_nRoads_1st", "number of first stage roads found per event;# roads;# events", 1000, -0.5, 1000-0.5);
    if (!m_withPU) h_nRoads_1st_uHits = new TH1I("h_nRoads_1st_uHits", "number of first stage roads with unique hits found per event;# roads;# events", 100, -0.5, 100-0.5);
    h_nRoads_1st_matched = new TH1I("h_nRoads_1st_matched", "number of first stage roads per matched truth track;# roads;# truth tracks", 100, -0.5, 100-0.5);
    h_nRoads_1st_dupeBarcode = new TH1I("h_nRoads_1st_dupeBarcode", "number of first stage roads with the same barcode;# roads;# barcodes", 500, -0.5, 500-0.5);
    h_nRoads_1st_fakeBarcode = new TH1I("h_nRoads_1st_fakeBarcode", "number of first stage roads with barcode < 0;# roads;# barcodes", 100, -0.5, 100-0.5);
    h_nRoadHitCombos_1st = new TH1I("h_nRoadHitCombos_1st", "total number of first stage hit combinations per event;# roads;# events", 2500, -0.5, 2500-0.5);
    h_nRoads_1st_vs_truth_pt  = new TH2F("h_nRoads_1st_vs_truth_pt", "Number of first stage roads vs truth q*pt", 100, -400. , 400., 50,-0.5,50-0.5); ATH_CHECK(regHist(getHistDir(), h_nRoads_1st_vs_truth_pt));
    h_nRoads_1st_vs_truth_eta = new TH2F("h_nRoads_1st_vs_truth_eta", "Number of first stage roads vs truth eta", 50, m_etamin, m_etamax, 50,-0.5,50-0.5); ATH_CHECK(regHist(getHistDir(), h_nRoads_1st_vs_truth_eta));
    h_nRoads_1st_vs_truth_phi = new TH2F("h_nRoads_1st_vs_truth_phi", "Number of first stage roads vs truth phi", 50, m_phimin, m_phimax, 50,-0.5,50-0.5); ATH_CHECK(regHist(getHistDir(), h_nRoads_1st_vs_truth_phi));
    h_nRoads_1st_vs_truth_d0 = new TH2F("h_nRoads_1st_vs_truth_d0", "Number of first stage roads vs truth d0", 50, m_d0Range[0], m_d0Range[1], 50,-0.5,50-0.5); ATH_CHECK(regHist(getHistDir(), h_nRoads_1st_vs_truth_d0));
    h_nRoads_1st_vs_truth_z0 = new TH2F("h_nRoads_1st_vs_truth_z0", "Number of first stage roads vs truth z0", 50, m_z0Range[0], m_z0Range[1], 50,-0.5,50-0.5); ATH_CHECK(regHist(getHistDir(), h_nRoads_1st_vs_truth_z0));
    h_road_1st_highestBarcodeFrac = new TH1I("h_road_1st_highestBarcodeFrac", "highest first stage barcode fraction per truth track;barcode fraction;# truth tracks", 10, 0, 1.01);
    if (m_extend) h_nRoads_1st->GetXaxis()->SetCanExtend(true);
    if (m_extend) h_nRoadHitCombos_1st->GetXaxis()->SetCanExtend(true);
    ATH_CHECK(regHist(getHistDir(), h_nRoads_1st));
    if (!m_withPU) ATH_CHECK(regHist(getHistDir(), h_nRoads_1st_uHits));
    ATH_CHECK(regHist(getHistDir(), h_nRoads_1st_matched));
    ATH_CHECK(regHist(getHistDir(), h_nRoads_1st_dupeBarcode));
    ATH_CHECK(regHist(getHistDir(), h_nRoads_1st_fakeBarcode));
    ATH_CHECK(regHist(getHistDir(), h_nRoadHitCombos_1st));
    ATH_CHECK(regHist(getHistDir(), h_road_1st_highestBarcodeFrac));


    setHistDir(createDirName("/HTTRoadHist_1st/PerRoadHist/"));

    h_nHits_road_1st = new TH1I("h_nHits_road_1st", "number of hits per first stage road;# hits;# roads", 50, -0.5, 50-0.5); ATH_CHECK(regHist(getHistDir(), h_nHits_road_1st));
    h_nLayers_road_1st = new TH1I("h_nLayers_road_1st", "number of layers hit per first stage road;# layers;# roads", m_nLayers_1st+1, -0.5, m_nLayers_1st+0.5); ATH_CHECK(regHist(getHistDir(), h_nLayers_road_1st));

    h_hitLayers_road_1st = new TH1I("h_hitLayers_road_1st", "layer ids with hits, per first stage road;layer ID;# roads", m_nLayers_1st, -0.5, m_nLayers_1st-0.5); ATH_CHECK(regHist(getHistDir(), h_hitLayers_road_1st));
    h_wcLayers_road_1st = new TH1I("h_wcLayers_road_1st", "layer ids with wildcards, per first stage road;layer ID;# roads", m_nLayers_1st, -0.5, m_nLayers_1st-0.5); ATH_CHECK(regHist(getHistDir(), h_wcLayers_road_1st));
    h_matchedLayers_road_1st = new TH1I("h_matchedLayers_road_1st", "layer ids with matches (hits + WCs), per first stage road;layer ID;# roads", m_nLayers_1st, -0.5, m_nLayers_1st-0.5); ATH_CHECK(regHist(getHistDir(), h_matchedLayers_road_1st));

    h_Categories_road_1st = new TH1I("h_Categories_road_1st","type of road for all first stage roads; type; # roads", roadtype::miss0wc0, roadtype::moremiss, roadtype::miss0wc0+1);
    TAxis* raxis = h_Categories_road_1st->GetXaxis();
    raxis->SetBinLabel(roadtype::moremiss, "moremiss");
    raxis->SetBinLabel(roadtype::miss2wc2, "6/8+2WC");
    raxis->SetBinLabel(roadtype::miss2wc1, "6/8+WC");
    raxis->SetBinLabel(roadtype::miss1wc0, "7/8");
    raxis->SetBinLabel(roadtype::miss1wc1, "7/8+WC");
    raxis->SetBinLabel(roadtype::miss0wc0, "8/8");
    ATH_CHECK(regHist(getHistDir(), h_Categories_road_1st));


    setHistDir(createDirName("/HTTRoadHist_1st/BestRoadHist/"));

    h_nLayers_best_road_1st = new TH1I("h_nLayers_best_road_1st", "maximum number of layers hit in the first stage roads per event;# layers;# roads", m_nLayers_1st+1, -0.5, m_nLayers_1st+0.5); ATH_CHECK(regHist(getHistDir(), h_nLayers_best_road_1st));
    h_hitLayers_best_road_1st = new TH1I("h_hitLayers_best_road_1st", "layer ids with hits, first stage roads with most hit layers per event;layer ID;# roads", m_nLayers_1st, -0.5, m_nLayers_1st-0.5); ATH_CHECK(regHist(getHistDir(), h_hitLayers_best_road_1st));
    h_wcLayers_best_road_1st = new TH1I("h_wcLayers_best_road_1st", "layer ids with wildcards, first stage roads with most hit layers per event;layer ID;# roads", m_nLayers_1st, -0.5, m_nLayers_1st-0.5); ATH_CHECK(regHist(getHistDir(), h_wcLayers_best_road_1st));
    h_matchedLayers_best_road_1st = new TH1I("h_matchedLayers_best_road_1st", "layer ids with matches (hits + WCs), first stage roads with most hit layers per event;layer ID;# roads", m_nLayers_1st, -0.5, m_nLayers_1st-0.5); ATH_CHECK(regHist(getHistDir(), h_matchedLayers_best_road_1st));

    h_Categories_best_road_1st = new TH1I("h_Categories_best_road_1st","type of road for best first stage roads; road type; # roads",roadtype::miss0wc0,roadtype::moremiss,roadtype::miss0wc0+1);
    TAxis* braxis = h_Categories_best_road_1st->GetXaxis();
    braxis->SetBinLabel(roadtype::moremiss, "moremiss");
    braxis->SetBinLabel(roadtype::miss2wc2, "6/8+2WC");
    braxis->SetBinLabel(roadtype::miss2wc1, "6/8+WC");
    braxis->SetBinLabel(roadtype::miss1wc0, "7/8");
    braxis->SetBinLabel(roadtype::miss1wc1, "7/8+WC");
    braxis->SetBinLabel(roadtype::miss0wc0, "8/8");
    ATH_CHECK(regHist(getHistDir(), h_Categories_best_road_1st));

    if (!m_withPU) {
        setHistDir(createDirName("/HTTRoadHist_1st/MultHist/"));

        h_pattMult_1st = new TH1I("h_pattMult_1st", "first stage pattern multiplicity (# of first stage roads fired by each pattern);multiplicity;# patterns", 50, -0.5, 50-0.5); ATH_CHECK(regHist(getHistDir(), h_pattMult_1st));
        h_sectMult_1st = new TH1I("h_sectMult_1st", "first stage sector multiplicity (# of first stage roads fired in each sector);multiplicity;# sectors", 100, -0.5, 100-0.5); ATH_CHECK(regHist(getHistDir(), h_sectMult_1st));
    }


    clearHistDir();

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTRoadMonitorTool::bookHTTRoadHistograms_2nd()
{
    setHistDir(createDirName("/HTTRoadHist_2nd/AllRoadHist/"));

    h_nRoads_2nd = new TH1I("h_nRoads_2nd", "number of second stage roads found per event;# roads;# events", 1000, -0.5, 1000-0.5);
    if (!m_withPU) h_nRoads_2nd_uHits = new TH1I("h_nRoads_2nd_uHits", "number of second stage roads with unique hits found per event;# roads;# events", 100, -0.5, 100-0.5);
    h_nRoads_2nd_matched = new TH1I("h_nRoads_2nd_matched", "number of second stage roads per matched truth track;# roads;# truth tracks", 100, -0.5, 100-0.5);
    h_nRoads_2nd_dupeBarcode = new TH1I("h_nRoads_2nd_dupeBarcode", "number of second stage roads with the same barcode;# roads;# barcodes", 500, -0.5, 500-0.5);
    h_nRoads_2nd_fakeBarcode = new TH1I("h_nRoads_2nd_fakeBarcode", "number of second stage roads with barcode < 0;# roads;# barcodes", 100, -0.5, 100-0.5);
    h_nRoadHitCombos_2nd = new TH1I("h_nRoadHitCombos_2nd", "total number of second stage hit combinations per event;# roads;# events", 2500, -0.5, 2500-0.5);
    h_nRoads_2nd_vs_truth_pt  = new TH2F("h_nRoads_2nd_vs_truth_pt", "Number of second stage roads vs truth q*pt", 100, -400. , 400., 50,-0.5,50-0.5); ATH_CHECK(regHist(getHistDir(), h_nRoads_2nd_vs_truth_pt));
    h_nRoads_2nd_vs_truth_eta = new TH2F("h_nRoads_2nd_vs_truth_eta", "Number of second stage roads vs truth eta", 50, m_etamin, m_etamax, 50,-0.5,50-0.5); ATH_CHECK(regHist(getHistDir(), h_nRoads_2nd_vs_truth_eta));
    h_nRoads_2nd_vs_truth_phi = new TH2F("h_nRoads_2nd_vs_truth_phi", "Number of second stage roads vs truth phi", 50, m_phimin, m_phimax, 50,-0.5,50-0.5); ATH_CHECK(regHist(getHistDir(), h_nRoads_2nd_vs_truth_phi));
    h_nRoads_2nd_vs_truth_d0 = new TH2F("h_nRoads_2nd_vs_truth_d0", "Number of second stage roads vs truth d0", 50, m_d0Range[0], m_d0Range[1], 50,-0.5,50-0.5); ATH_CHECK(regHist(getHistDir(), h_nRoads_2nd_vs_truth_d0));
    h_nRoads_2nd_vs_truth_z0 = new TH2F("h_nRoads_2nd_vs_truth_z0", "Number of second stage roads vs truth z0", 50, m_z0Range[0], m_z0Range[1], 50,-0.5,50-0.5); ATH_CHECK(regHist(getHistDir(), h_nRoads_2nd_vs_truth_z0));
    h_road_2nd_highestBarcodeFrac = new TH1I("h_road_2nd_highestBarcodeFrac", "highest second stage barcode fraction per truth track;barcode fraction;# truth tracks", 10, 0, 1.01);
    if (m_extend) h_nRoads_2nd->GetXaxis()->SetCanExtend(true);
    if (m_extend) h_nRoadHitCombos_2nd->GetXaxis()->SetCanExtend(true);
    ATH_CHECK(regHist(getHistDir(), h_nRoads_2nd));
    if (!m_withPU) ATH_CHECK(regHist(getHistDir(), h_nRoads_2nd_uHits));
    ATH_CHECK(regHist(getHistDir(), h_nRoads_2nd_matched));
    ATH_CHECK(regHist(getHistDir(), h_nRoads_2nd_dupeBarcode));
    ATH_CHECK(regHist(getHistDir(), h_nRoads_2nd_fakeBarcode));
    ATH_CHECK(regHist(getHistDir(), h_nRoadHitCombos_2nd));
    ATH_CHECK(regHist(getHistDir(), h_road_2nd_highestBarcodeFrac));


    setHistDir(createDirName("/HTTRoadHist_2nd/PerRoadHist/"));

    h_nHits_road_2nd = new TH1I("h_nHits_road_2nd", "number of hits per second stage road;# hits;# roads", 50, -0.5, 50-0.5); ATH_CHECK(regHist(getHistDir(), h_nHits_road_2nd));
    h_nLayers_road_2nd = new TH1I("h_nLayers_road_2nd", "number of layers hit per second stage road;# layers;# roads", m_nLayers_2nd+1, -0.5, m_nLayers_2nd+0.5); ATH_CHECK(regHist(getHistDir(), h_nLayers_road_2nd));

    h_hitLayers_road_2nd = new TH1I("h_hitLayers_road_2nd", "layer ids with hits, per second stage road;layer ID;# roads", m_nLayers_2nd, -0.5, m_nLayers_2nd-0.5); ATH_CHECK(regHist(getHistDir(), h_hitLayers_road_2nd));
    h_wcLayers_road_2nd = new TH1I("h_wcLayers_road_2nd", "layer ids with wildcards, per second stage road;layer ID;# roads", m_nLayers_2nd, -0.5, m_nLayers_2nd-0.5); ATH_CHECK(regHist(getHistDir(), h_wcLayers_road_2nd));
    h_matchedLayers_road_2nd = new TH1I("h_matchedLayers_road_2nd", "layer ids with matches (hits + WCs), per second stage road;layer ID;# roads", m_nLayers_2nd, -0.5, m_nLayers_2nd-0.5); ATH_CHECK(regHist(getHistDir(), h_matchedLayers_road_2nd));

    h_Categories_road_2nd = new TH1I("h_Categories_road_2nd","type of road for all second stage roads; type; # roads", roadtype::miss0wc0, roadtype::moremiss, roadtype::miss0wc0+1);
    TAxis* raxis = h_Categories_road_2nd->GetXaxis();
    raxis->SetBinLabel(roadtype::moremiss, "moremiss");
    raxis->SetBinLabel(roadtype::miss2wc2, "11/13+2WC");
    raxis->SetBinLabel(roadtype::miss2wc1, "11/13+WC");
    raxis->SetBinLabel(roadtype::miss1wc0, "12/13");
    raxis->SetBinLabel(roadtype::miss1wc1, "12/13+WC");
    raxis->SetBinLabel(roadtype::miss0wc0, "13/13");
    ATH_CHECK(regHist(getHistDir(), h_Categories_road_2nd));


    setHistDir(createDirName("/HTTRoadHist_2nd/BestRoadHist/"));

    h_nLayers_best_road_2nd = new TH1I("h_nLayers_best_road_2nd", "maximum number of layers hit in the second stage roads per event;# layers;# roads", m_nLayers_2nd+1, -0.5, m_nLayers_2nd+0.5); ATH_CHECK(regHist(getHistDir(), h_nLayers_best_road_2nd));
    h_hitLayers_best_road_2nd = new TH1I("h_hitLayers_best_road_2nd", "layer ids with hits, second stage roads with most hit layers per event;layer ID;# roads", m_nLayers_2nd, -0.5, m_nLayers_2nd-0.5); ATH_CHECK(regHist(getHistDir(), h_hitLayers_best_road_2nd));
    h_wcLayers_best_road_2nd = new TH1I("h_wcLayers_best_road_2nd", "layer ids with wildcards, second stage roads with most hit layers per event;layer ID;# roads", m_nLayers_2nd, -0.5, m_nLayers_2nd-0.5); ATH_CHECK(regHist(getHistDir(), h_wcLayers_best_road_2nd));
    h_matchedLayers_best_road_2nd = new TH1I("h_matchedLayers_best_road_2nd", "layer ids with matches (hits + WCs), second stage roads with most hit layers per event;layer ID;# roads", m_nLayers_2nd, -0.5, m_nLayers_2nd-0.5); ATH_CHECK(regHist(getHistDir(), h_matchedLayers_best_road_2nd));

    h_Categories_best_road_2nd = new TH1I("h_Categories_best_road_2nd", "type of road for best second stage roads; road type; # roads", roadtype::miss0wc0, roadtype::moremiss, roadtype::miss0wc0+1);
    TAxis* braxis = h_Categories_best_road_2nd->GetXaxis();
    braxis->SetBinLabel(roadtype::moremiss, "moremiss");
    braxis->SetBinLabel(roadtype::miss2wc2, "11/13+2WC");
    braxis->SetBinLabel(roadtype::miss2wc1, "11/13+WC");
    braxis->SetBinLabel(roadtype::miss1wc0, "12/13");
    braxis->SetBinLabel(roadtype::miss1wc1, "12/13+WC");
    braxis->SetBinLabel(roadtype::miss0wc0, "13/13");
    ATH_CHECK(regHist(getHistDir(), h_Categories_best_road_2nd));

    if (!m_withPU) {
        setHistDir(createDirName("/HTTRoadHist_2nd/MultHist/"));

        h_pattMult_2nd = new TH1I("h_pattMult_2nd", "second stage pattern multiplicity (# of second stage roads fired by each pattern);multiplicity;# patterns", 50, -0.5, 50-0.5); ATH_CHECK(regHist(getHistDir(), h_pattMult_2nd));
        h_sectMult_2nd = new TH1I("h_sectMult_2nd", "second stage sector multiplicity (# of second stage roads fired in each sector);multiplicity;# sectors", 100, -0.5, 100-0.5); ATH_CHECK(regHist(getHistDir(), h_sectMult_2nd));
    }


    clearHistDir();

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void HTTRoadMonitorTool::fillHTTRoadHistograms_1st(std::vector<HTTRoad*> const * roads_1st)
{
    if (!roads_1st) {
        ATH_MSG_WARNING("Failed to retrieve first stage roads.");
        return;
    }

    // Number of roads
    h_nRoads_1st->Fill(roads_1st->size());

    // Road combos and max layer
    unsigned maxLayers = 0;
    size_t nCombos = 0;
    for (HTTRoad const * r : *roads_1st)
    {
        if (r->getNHitLayers() > maxLayers) maxLayers = r->getNHitLayers();
        nCombos += r->getNHitCombos();
    }
    h_nRoadHitCombos_1st->Fill(nCombos);
    h_nLayers_best_road_1st->Fill(maxLayers);

    ///////////////////////////////////////////////////////
    // Slow monitoring
    ///////////////////////////////////////////////////////
    if (m_fast) return;

    typedef std::unordered_set<std::unordered_set<HTTHit const *>, HTT::set_hash_fast<std::unordered_set<HTTHit const *>>> RoadHitsSet;
    RoadHitsSet roadHitsSet; // count unique roads based on hits

    std::unordered_map<pid_t, size_t> pattMult; // number of time each pattern has been found in a road
    std::unordered_map<sector_t, size_t> sectMult; // number of time each sector has been found in a road

    for (HTTRoad const * r : *roads_1st)
    {
        // # hits
        h_nHits_road_1st->Fill(r->getNHits());

        // # layers
        unsigned nLayers = __builtin_popcount(r->getHitLayers());
        unsigned nLayersWC = __builtin_popcount(r->getWCLayers());
        h_nLayers_road_1st->Fill(nLayers);

        // categories
        roadtype category = roadtype::noroad;
        if (nLayers == m_nLayers_1st) category = roadtype::miss0wc0;
        else if (nLayers == m_nLayers_1st - 1) {
            if (nLayersWC == 0) category = roadtype::miss1wc0;
            else category = roadtype::miss1wc1;
        }
        else if (nLayers == m_nLayers_1st - 2) {
            if (nLayersWC == 1) category = roadtype::miss2wc1;
            else if (nLayersWC == 2) category = roadtype::miss2wc2;
        }
        else {
            category = roadtype::moremiss;
            ATH_MSG_WARNING("Found first stage road with " << nLayers << " and " << nLayersWC <<" WCs");
        }
        ATH_MSG_DEBUG("First stage road with " << nLayers << " layers and " << nLayersWC << " WCs: category=" << category);
        h_Categories_road_1st->Fill(category);
        if (nLayers == maxLayers) h_Categories_best_road_1st->Fill(category);

        // enumerated layers
        fillBitmask(h_hitLayers_road_1st, r->getHitLayers(), m_nLayers_1st);
        fillBitmask(h_wcLayers_road_1st, r->getWCLayers(), m_nLayers_1st);
        fillBitmask(h_matchedLayers_road_1st, r->getHitLayers() | r->getWCLayers(), m_nLayers_1st);
        if (nLayers == maxLayers)
        {
            fillBitmask(h_hitLayers_best_road_1st, r->getHitLayers(), m_nLayers_1st);
            fillBitmask(h_wcLayers_best_road_1st, r->getWCLayers(), m_nLayers_1st);
            fillBitmask(h_matchedLayers_best_road_1st, r->getHitLayers() | r->getWCLayers(), m_nLayers_1st);
        }

        // multiplicities (only do these with no PU, otherwise can get huge and slow)
        if (!m_withPU)
        {
            roadHitsSet.insert(r->getHits_flat());
            pattMult[r->getPID()]++;
            sectMult[r->getSector()]++;
        }
    }

    // Fill a few histograms used only with no PU
    if (!m_withPU)
    {
        h_nRoads_1st_uHits->Fill(roadHitsSet.size());
        for (auto const & p_m : pattMult) h_pattMult_1st->Fill(p_m.second);
        for (auto const & s_m : sectMult) h_sectMult_1st->Fill(s_m.second);
    }

    // Create a lookup on the barcodes of all the roads, and also max frac
    //      value.first = roads with matching barcode, and fraction above m_cut_barcodefrac
    //      value.second = maximum barcode fraction among all roads for this barcode
    std::map<typename HTTMultiTruth::Barcode, std::pair<std::vector<HTTRoad const *>, float>> m;
    calculateTruth(roads_1st, m);

    // Find number of "duplicates" based on barcode
    for (auto const & br : m)
    {
        if (br.first.second >= 0)
            h_nRoads_1st_dupeBarcode->Fill(br.second.first.size());
        else
            h_nRoads_1st_fakeBarcode->Fill(br.second.first.size());
    }

    std::vector<HTTTruthTrack> const & truthtracks = m_logicEventInputHeader_1st->optional().getTruthTracks();

    // count roads per track paarameters
    int nRoads = roads_1st->size();
    for (auto truth_t: truthtracks) {
        h_nRoads_1st_vs_truth_pt->Fill(truth_t.getQ() * truth_t.getPt()*0.001, nRoads);
        h_nRoads_1st_vs_truth_eta->Fill(truth_t.getEta(), nRoads);
        h_nRoads_1st_vs_truth_phi->Fill(truth_t.getPhi(), nRoads);
        h_nRoads_1st_vs_truth_d0->Fill(truth_t.getD0(), nRoads);
        h_nRoads_1st_vs_truth_z0->Fill(truth_t.getZ0(), nRoads);
    }

    // Find roads matching each track
    for (auto const & t : truthtracks)
    {
        // No PDGid code associated with LLP sample type. If we want to run on one in particular,
        // should add it as a separate flag.
        if (m_evtSel->getSampleType() != SampleType::LLPs) {
            if (std::abs(t.getPDGCode()) != static_cast<int> (m_evtSel->getSampleType())) continue;
        }
        else {
            if ((m_evtSel->getLRTpdgID() !=0) && std::abs(t.getPDGCode()) != m_evtSel->getLRTpdgID()) {
                std::cout << "Skipping this truth track: wrong PDGID (" << t.getPDGCode() << ")" << std::endl;
                continue;
            }
        }
        if (!m_evtSel->passMatching(t)) continue;

        // TODO truth tracks in single muon wrappers have event index 0, so force everything to 0 here
        auto it = m.find({ 0, t.getBarcode() });
        if (it != m.end()) {
            h_road_1st_highestBarcodeFrac->Fill(it->second.second);
            if (!it->second.first.empty()) h_nRoads_1st_matched->Fill(it->second.first.size());
        }
        else {
            h_road_1st_highestBarcodeFrac->Fill(0);
        }
    }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void HTTRoadMonitorTool::fillHTTRoadHistograms_2nd(std::vector<HTTRoad*> const * roads_2nd)
{
    if (!roads_2nd) {
        ATH_MSG_WARNING("Failed to retrieve second stage roads.");
        return;
    }

    // Number of roads
    h_nRoads_2nd->Fill(roads_2nd->size());

    // Road combos and max layer
    unsigned maxLayers = 0;
    size_t nCombos = 0;
    for (HTTRoad const * r : *roads_2nd)
    {
        if (r->getNHitLayers() > maxLayers) maxLayers = r->getNHitLayers();
        nCombos += r->getNHitCombos();
    }
    h_nRoadHitCombos_2nd->Fill(nCombos);
    h_nLayers_best_road_2nd->Fill(maxLayers);

    ///////////////////////////////////////////////////////
    // Slow monitoring
    ///////////////////////////////////////////////////////
    if (m_fast) return;

    typedef std::unordered_set<std::unordered_set<HTTHit const *>, HTT::set_hash_fast<std::unordered_set<HTTHit const *>>> RoadHitsSet;
    RoadHitsSet roadHitsSet; // count unique roads based on hits

    std::unordered_map<pid_t, size_t> pattMult; // number of time each pattern has been found in a road
    std::unordered_map<sector_t, size_t> sectMult; // number of time each sector has been found in a road

    for (HTTRoad const * r : *roads_2nd)
    {
        // # hits
        h_nHits_road_2nd->Fill(r->getNHits());

        // # layers
        unsigned nLayers = __builtin_popcount(r->getHitLayers());
        unsigned nLayersWC = __builtin_popcount(r->getWCLayers());
        h_nLayers_road_2nd->Fill(nLayers);

        // categories
        roadtype category = roadtype::noroad;
        if (nLayers == m_nLayers_2nd) category= roadtype::miss0wc0;
        else if (nLayers == m_nLayers_2nd - 1) {
            if (nLayersWC == 0) category = roadtype::miss1wc0;
            else category = roadtype::miss1wc1;
        }
        else if (nLayers == m_nLayers_2nd - 2) {
            if (nLayersWC == 1) category = roadtype::miss2wc1;
            else if (nLayersWC == 2) category = roadtype::miss2wc2;
        }
        else {
            category = roadtype::moremiss;
            ATH_MSG_WARNING("Found second stage road with " << nLayers << " and "<< nLayersWC << " WCs");
        }
        ATH_MSG_DEBUG("Second stage road with " << nLayers << " layers and "<< nLayersWC << " WCs: category = " << category);
        h_Categories_road_2nd->Fill(category);
        if (nLayers == maxLayers) h_Categories_best_road_2nd->Fill(category);

        // enumerated layers
        fillBitmask(h_hitLayers_road_2nd, r->getHitLayers(), m_nLayers_2nd);
        fillBitmask(h_wcLayers_road_2nd, r->getWCLayers(), m_nLayers_2nd);
        fillBitmask(h_matchedLayers_road_2nd, r->getHitLayers() | r->getWCLayers(), m_nLayers_2nd);
        if (nLayers == maxLayers)
        {
            fillBitmask(h_hitLayers_best_road_2nd, r->getHitLayers(), m_nLayers_2nd);
            fillBitmask(h_wcLayers_best_road_2nd, r->getWCLayers(), m_nLayers_2nd);
            fillBitmask(h_matchedLayers_best_road_2nd, r->getHitLayers() | r->getWCLayers(), m_nLayers_2nd);
        }

        // multiplicities (only do these with no PU, otherwise can get huge and slow)
        if (!m_withPU)
        {
            roadHitsSet.insert(r->getHits_flat());
            pattMult[r->getPID()]++;
            sectMult[r->getSector()]++;
        }
    }

    // Fill a few histograms used only with no PU
    if (!m_withPU)
    {
        h_nRoads_2nd_uHits->Fill(roadHitsSet.size());
        for (auto const & p_m : pattMult) h_pattMult_2nd->Fill(p_m.second);
        for (auto const & s_m : sectMult) h_sectMult_2nd->Fill(s_m.second);
    }

    // Create a lookup on the barcodes of all the roads, and also max frac
    //      value.first = roads with matching barcode, and fraction above m_cut_barcodefrac
    //      value.second = maximum barcode fraction among all roads for this barcode
    std::map<typename HTTMultiTruth::Barcode, std::pair<std::vector<HTTRoad const *>, float>> m;
    calculateTruth(roads_2nd, m);

    // Find number of "duplicates" based on barcode
    for (auto const & br : m)
    {
        if (br.first.second >= 0)
            h_nRoads_2nd_dupeBarcode->Fill(br.second.first.size());
        else
            h_nRoads_2nd_fakeBarcode->Fill(br.second.first.size());
    }

    std::vector<HTTTruthTrack> const & truthtracks = m_logicEventInputHeader_1st->optional().getTruthTracks();

    // count roads per track paarameters
    int nRoads = roads_2nd->size();
    for (auto truth_t: truthtracks) {
        h_nRoads_2nd_vs_truth_pt->Fill(truth_t.getQ() *truth_t.getPt()*0.001, nRoads);
        h_nRoads_2nd_vs_truth_eta->Fill(truth_t.getEta(), nRoads);
        h_nRoads_2nd_vs_truth_phi->Fill(truth_t.getPhi(), nRoads);
        h_nRoads_2nd_vs_truth_d0->Fill(truth_t.getD0(), nRoads);
        h_nRoads_2nd_vs_truth_z0->Fill(truth_t.getZ0(), nRoads);
    }

    // Find roads matching each track
    for (auto const & t : truthtracks)
    {
        // No PDGid code associated with LLP sample type. If we want to run on one in particular,
        // should add it as a separate flag.
        if (m_evtSel->getSampleType() != SampleType::LLPs) {
            if (std::abs(t.getPDGCode()) != static_cast<int> (m_evtSel->getSampleType())) continue;
        }
        else {
            if ((m_evtSel->getLRTpdgID() !=0) && std::abs(t.getPDGCode()) != m_evtSel->getLRTpdgID()) {
                std::cout << "Skipping this truth track: wrong PDGID (" << t.getPDGCode() << ")" << std::endl;
                continue;
            }
        }
        if (!m_evtSel->passMatching(t)) continue;

        // TODO truth tracks in single muon wrappers have event index 0, so force everything to 0 here
        auto it = m.find({ 0, t.getBarcode() });
        if (it != m.end()) {
            h_road_2nd_highestBarcodeFrac->Fill(it->second.second);
            if (!it->second.first.empty()) h_nRoads_2nd_matched->Fill(it->second.first.size());
        }
        else {
            h_road_2nd_highestBarcodeFrac->Fill(0);
        }
    }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void HTTRoadMonitorTool::calculateTruth(std::vector<HTTRoad*> const * roads, std::map<typename HTTMultiTruth::Barcode, std::pair<std::vector<HTTRoad const *>, float>> & m)
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
