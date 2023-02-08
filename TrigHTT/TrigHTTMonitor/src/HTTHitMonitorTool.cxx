/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTMonitor/HTTHitMonitorTool.h"

#include "TrigHTTMaps/HTTPlaneMap.h"
#include "TrigHTTObjects/HTTHit.h"
#include "TrigHTTObjects/HTTLogicalEventInputHeader.h"


static const InterfaceID IID_HTTHitMonitorTool("HTTHitMonitorTool", 1, 0);
const InterfaceID& HTTHitMonitorTool::interfaceID()
{ return IID_HTTHitMonitorTool; }


/////////////////////////////////////////////////////////////////////////////
HTTHitMonitorTool::HTTHitMonitorTool(std::string const & algname, std::string const & name, IInterface const * ifc) :
    HTTMonitorBase(algname,name,ifc)
{
    declareInterface<HTTHitMonitorTool>(this);
    declareProperty("fastMon",              m_fast,                 "only do fast monitoring");
    declareProperty("OutputMon",            m_outputMonitor,        "do monitoring on output data (HTTLogicalEventInputHeader, HTTLogicalEventOutputHeader)");
    declareProperty("RunSecondStage",       m_runSecondStage,       "flag to enable running the second stage fitting");
    declareProperty("canExtendHistRanges",  m_extend,               "extend x-axes of some histograms");
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTHitMonitorTool::initialize()
{
    ATH_MSG_INFO("HTTHitMonitorTool::initialize()");
    ATH_CHECK(HTTMonitorBase::initialize());
    ATH_CHECK(bookHistograms());
    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTHitMonitorTool::bookHistograms()
{
    ATH_CHECK(bookHTTHitHistograms_1st());
    if (m_runSecondStage) ATH_CHECK(bookHTTHitHistograms_2nd());
    if (!m_outputMonitor && !m_fast) {
        ATH_CHECK(bookMissedHitHistograms_1st());
        if (m_runSecondStage) ATH_CHECK(bookMissedHitHistograms_2nd());
    }
    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTHitMonitorTool::fillHistograms()
{
    // Use event selection to pass or reject event based on truth info, sample type etc
    ATH_CHECK(selectEvent());

    std::vector<HTTHit> const * hits_1st_mapped = &(m_logicEventInputHeader_1st->towers().at(0).hits());
    std::vector<HTTHit> const * hits_2nd_mapped = m_runSecondStage ? &(m_logicEventInputHeader_2nd->towers().at(0).hits()) : nullptr;

    fillHTTHitHistograms_1st(hits_1st_mapped);
    if (m_runSecondStage) fillHTTHitHistograms_2nd(hits_2nd_mapped);

    if (!m_fast && m_eventInputHeader) {
        fillMissedHitHistograms_1st(getMonitorHits("Hits_1st_Miss"), hits_1st_mapped);
        if (m_runSecondStage) fillMissedHitHistograms_2nd(getMonitorHits("Hits_2nd_Miss"), hits_2nd_mapped);
    }

    clearMonitorData();

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTHitMonitorTool::finalize()
{
    ATH_MSG_INFO("HTTHitMonitorTool::finalize()");
    ATH_CHECK(HTTMonitorBase::finalize());
    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTHitMonitorTool::bookHTTHitHistograms_1st()
{
    HTTPlaneMap const * pmap_1st = m_HTTMapping->PlaneMap_1st();


    setHistDir("/HTTHitHist_1st/AllLayerHist/");

    h_nHits_1st = new TH1I("h_nHits_1st", "number of first stage hits / event", 5000, -0.5, 5000-0.5);
    if (m_extend) h_nHits_1st->GetXaxis()->SetCanExtend(true);
    h_nLayers_hit_1st = new TH1I("h_nLayers_hit_1st", "number of first stage hit layers / event", m_nLayers_1st + 1, -0.5, m_nLayers_1st + 0.5);
    h_hit_1st_Layer = new TH1I("h_hit_1st_Layer", "Layer of mapped first stage hits", m_nLayers_1st, -0.5, m_nLayers_1st - 0.5);

    ATH_CHECK(regHist(getHistDir(), h_nHits_1st));
    ATH_CHECK(regHist(getHistDir(), h_nLayers_hit_1st));
    ATH_CHECK(regHist(getHistDir(), h_hit_1st_Layer));


    setHistDir("/HTTHitHist_1st/PerLayerHist/");

    h_nHits_1st_layer = new TH2I(
            "h_nHits_1st_layer",
            "number of mapped/clustered first stage hits in each layer per event;layer;# processing hits / event",
            m_nLayers_1st, -0.5, m_nLayers_1st-0.5, 2000, -0.5, 2000-0.5
    );
    ATH_CHECK(regHist(getHistDir(), h_nHits_1st_layer));

    h_Hit_1st_coord_layer.resize(m_nLayers_1st);

    for (unsigned i = 0; i < m_nLayers_1st; i++)
    {
        std::string name = pmap_1st->layerName(i, 0); // assume first section

        int xbin = pmap_1st->isPixel(i) ? 800 : 5;
        int ybin = pmap_1st->isPixel(i) ? 900 : 1500;
        double xmax = xbin - 0.5;
        double ymax = ybin - 0.5;

        h_Hit_1st_coord_layer[i] = new TH2I(
                ("h_Hit_1st_coord_layer_" + std::to_string(i)).c_str(),
                ("The row/col positions of first stage hits in layer " + name + ";eta index; phi Index").c_str(),
                xbin, -0.5, xmax, ybin, -0.5, ymax
        );

        ATH_CHECK(regHist(getHistDir(), h_Hit_1st_coord_layer[i]));
    }


    clearHistDir();

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTHitMonitorTool::bookHTTHitHistograms_2nd()
{
    HTTPlaneMap const * pmap_2nd = m_HTTMapping->PlaneMap_2nd();


    setHistDir("/HTTHitHist_2nd/AllLayerHist/");

    h_nHits_2nd = new TH1I("h_nHits_2nd", "number of second stage hits / event", 5000, -0.5, 5000-0.5);
    if (m_extend) h_nHits_2nd->GetXaxis()->SetCanExtend(true);
    h_nLayers_hit_2nd = new TH1I("h_nLayers_hit_2nd", "number of second stage hit layers / event", m_nLayers_2nd + 1, -0.5, m_nLayers_2nd + 0.5);
    h_hit_2nd_Layer = new TH1I("h_hit_2nd_Layer", "Layer of mapped second stage hits", m_nLayers_2nd, -0.5, m_nLayers_2nd - 0.5);

    ATH_CHECK(regHist(getHistDir(), h_nHits_2nd));
    ATH_CHECK(regHist(getHistDir(), h_nLayers_hit_2nd));
    ATH_CHECK(regHist(getHistDir(), h_hit_2nd_Layer));


    setHistDir("/HTTHitHist_2nd/PerLayerHist/");

    h_nHits_2nd_layer = new TH2I(
            "h_nHits_2nd_layer",
            "number of mapped/clustered second stage hits in each layer per event;layer;# processing hits / event",
            m_nLayers_2nd, -0.5, m_nLayers_2nd-0.5, 2000, -0.5, 2000-0.5
    );
    ATH_CHECK(regHist(getHistDir(), h_nHits_2nd_layer));

    h_Hit_2nd_coord_layer.resize(m_nLayers_2nd);

    for (unsigned i = 0; i < m_nLayers_2nd; i++)
    {
        std::string name = pmap_2nd->layerName(i, 0); // assume first section

        int xbin = pmap_2nd->isPixel(i) ? 800 : 5;
        int ybin = pmap_2nd->isPixel(i) ? 900 : 1500;
        double xmax = xbin - 0.5;
        double ymax = ybin - 0.5;

        h_Hit_2nd_coord_layer[i] = new TH2I(
                ("h_Hit_2nd_coord_layer_" + std::to_string(i)).c_str(),
                ("The row/col positions of second stage hits in layer " + name+";eta index; phi Index").c_str(),
                xbin, -0.5, xmax, ybin, -0.5, ymax
        );

        ATH_CHECK(regHist(getHistDir(), h_Hit_2nd_coord_layer[i]));
    }


    clearHistDir();

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTHitMonitorTool::bookMissedHitHistograms_1st()
{
    setHistDir(createDirName("/MissedHitHist_1st/"));

    h_nMissedHits_1st = new TH1I("h_nMissedHits_1st", "n. of missing first stage hits/event", 150, -0.5, 150-0.5); ATH_CHECK(regHist(getHistDir(), h_nMissedHits_1st));
    h_nMissedHits_1st_Frac = new TH1F("h_nMissedHits_1st_Frac", "Fraction of missing first stage hits/event", 100, 0., 1.); ATH_CHECK(regHist(getHistDir(), h_nMissedHits_1st_Frac));
    h_nMissedHits_1st_vs_truth_pt = new TH2F("h_nMissedHits_1st_vs_truth_pt", "Number of missing first stage hits vs truth q*pt", 100, -400., 400., 50,-0.5,50.-0.5); ATH_CHECK(regHist(getHistDir(), h_nMissedHits_1st_vs_truth_pt));
    h_nMissedHits_1st_vs_truth_eta = new TH2F("h_nMissedHits_1st_vs_truth_eta", "Number of missing first stage hits vs truth eta", 50, m_etamin, m_etamax, 50,-0.5,50-0.5); ATH_CHECK(regHist(getHistDir(), h_nMissedHits_1st_vs_truth_eta));
    h_nMissedHits_1st_vs_truth_phi = new TH2F("h_nMissedHits_1st_vs_truth_phi", "Number of missing first stage hits vs truth phi", 50, m_phimin, m_phimax, 50,-0.5,50-0.5); ATH_CHECK(regHist(getHistDir(), h_nMissedHits_1st_vs_truth_phi));
    h_nMissedHits_1st_vs_truth_d0  = new TH2F("h_nMissedHits_1st_vs_truth_d0", "Number of missing first stage hits vs truth d0", 50, m_d0Range[0], m_d0Range[1], 50,-0.5,50-0.5); ATH_CHECK(regHist(getHistDir(), h_nMissedHits_1st_vs_truth_d0));
    h_nMissedHits_1st_vs_truth_z0  = new TH2F("h_nMissedHits_1st_vs_truth_z0", "Number of missing first stage hits vs truth z0", 50, m_z0Range[0], m_z0Range[1], 50,-0.5,50-0.5); ATH_CHECK(regHist(getHistDir(), h_nMissedHits_1st_vs_truth_z0));

    h_MissedHit_1st_eta = new TH1I("h_MissedHit_1st_eta", "eta of missing first stage hits", 100, -100, 100); ATH_CHECK(regHist(getHistDir(), h_MissedHit_1st_eta));
    h_MissedHit_1st_phi = new TH1I("h_MissedHit_1st_phi", "phi of missing first stage hits", 100, 0, 100); ATH_CHECK(regHist(getHistDir(), h_MissedHit_1st_phi));
    h_MissedHit_1st_type = new TH1I("h_MissedHit_1st_type", "type of missing first stage hits", 2, -0.5, 2-0.5); ATH_CHECK(regHist(getHistDir(), h_MissedHit_1st_type));
    h_MissedHit_1st_barrelEC = new TH1I("h_MissedHit_1st_barrelEC", "barrelEC of missing first stage hits", 2, -0.5, 2-0.5); ATH_CHECK(regHist(getHistDir(), h_MissedHit_1st_barrelEC));
    // h_MissedHit_1st_layer = new TH1I("h_MissedHit_1st_layer", "layer of missing first stage hits", m_nLayers_1st, -0.5, m_nLayers_1st-0.5); ATH_CHECK(regHist(getHistDir(), h_MissedHit_1st_layer));
    h_MissedHit_1st_physLayer = new TH1I("h_MissedHit_1st_physLayer", "ITK layer of missing first stage hits", 600, -0.5, 600-0.5); ATH_CHECK(regHist(getHistDir(), h_MissedHit_1st_physLayer));


    clearHistDir();

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTHitMonitorTool::bookMissedHitHistograms_2nd()
{
    setHistDir(createDirName("/MissedHitHist_2nd/"));

    h_nMissedHits_2nd = new TH1I("h_nMissedHits_2nd", "n. of missing second stage hits/event", 150, -0.5, 150-0.5); ATH_CHECK(regHist(getHistDir(), h_nMissedHits_2nd));
    h_nMissedHits_2nd_Frac = new TH1F("h_nMissedHits_2nd_Frac", "Fraction of missing second stage hits/event", 100, 0., 1.); ATH_CHECK(regHist(getHistDir(), h_nMissedHits_2nd_Frac));
    h_nMissedHits_2nd_vs_truth_pt = new TH2F("h_nMissedHits_2nd_vs_truth_pt", "Number of missing second stage hits vs truth q*pt", 100, -400., 400., 50,-0.5,50.-0.5); ATH_CHECK(regHist(getHistDir(), h_nMissedHits_2nd_vs_truth_pt));
    h_nMissedHits_2nd_vs_truth_eta = new TH2F("h_nMissedHits_2nd_vs_truth_eta", "Number of missing second stage hits vs truth eta", 50, m_etamin, m_etamax, 50,-0.5,50-0.5); ATH_CHECK(regHist(getHistDir(), h_nMissedHits_2nd_vs_truth_eta));
    h_nMissedHits_2nd_vs_truth_phi = new TH2F("h_nMissedHits_2nd_vs_truth_phi", "Number of missing second stage hits vs truth phi", 50, m_phimin, m_phimax, 50,-0.5,50-0.5); ATH_CHECK(regHist(getHistDir(), h_nMissedHits_2nd_vs_truth_phi));
    h_nMissedHits_2nd_vs_truth_d0  = new TH2F("h_nMissedHits_2nd_vs_truth_d0", "Number of missing second stage hits vs truth d0", 50, m_d0Range[0], m_d0Range[1], 50,-0.5,50-0.5); ATH_CHECK(regHist(getHistDir(), h_nMissedHits_2nd_vs_truth_d0));
    h_nMissedHits_2nd_vs_truth_z0  = new TH2F("h_nMissedHits_2nd_vs_truth_z0", "Number of missing second stage hits vs truth z0", 50, m_z0Range[0], m_z0Range[1], 50,-0.5,50-0.5); ATH_CHECK(regHist(getHistDir(), h_nMissedHits_2nd_vs_truth_z0));

    h_MissedHit_2nd_eta = new TH1I("h_MissedHit_2nd_eta", "eta of second stage missing hits", 100, -100, 100); ATH_CHECK(regHist(getHistDir(), h_MissedHit_2nd_eta));
    h_MissedHit_2nd_phi = new TH1I("h_MissedHit_2nd_phi", "phi of second stage missing hits", 100, 0, 100); ATH_CHECK(regHist(getHistDir(), h_MissedHit_2nd_phi));
    h_MissedHit_2nd_type = new TH1I("h_MissedHit_2nd_type", "type of missing second stage hits", 2, -0.5, 2-0.5); ATH_CHECK(regHist(getHistDir(), h_MissedHit_2nd_type));
    h_MissedHit_2nd_barrelEC = new TH1I("h_MissedHit_2nd_barrelEC", "barrelEC of missing second stage hits", 2, -0.5, 2-0.5); ATH_CHECK(regHist(getHistDir(), h_MissedHit_2nd_barrelEC));
    // h_MissedHit_2nd_layer = new TH1I("h_MissedHit_2nd_layer", "layer of missing second stage hits", m_nLayers_1st, -0.5, m_nLayers_1st-0.5); ATH_CHECK(regHist(getHistDir(), h_MissedHit_2nd_layer));
    h_MissedHit_2nd_physLayer = new TH1I("h_MissedHit_2nd_physLayer", "ITK layer of missing second stage hits", 600, -0.5, 600-0.5); ATH_CHECK(regHist(getHistDir(), h_MissedHit_2nd_physLayer));


    clearHistDir();

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void HTTHitMonitorTool::fillHTTHitHistograms_1st(std::vector<HTTHit> const * hits_1st_mapped)
{
    if (!hits_1st_mapped) {
        ATH_MSG_WARNING("Failed to retrieve first stage mapped hits.");
        return;
    }

    h_nHits_1st->Fill(hits_1st_mapped->size());

    std::vector<bool> layers_hit(m_nLayers_1st);
    for (HTTHit const & h : *hits_1st_mapped) layers_hit[h.getLayer()] = true;
    int layers = 0;
    for (bool hit : layers_hit) if (hit) layers++;
    h_nLayers_hit_1st->Fill(layers);
    for (HTTHit const & h : *hits_1st_mapped) h_hit_1st_Layer->Fill(h.getLayer());

    std::vector<size_t> nHits_layer;
    nHits_layer.resize(m_nLayers_1st);

    for (HTTHit const & hit : *hits_1st_mapped) {
        nHits_layer[hit.getLayer()]++;
        h_Hit_1st_coord_layer[hit.getLayer()]->Fill(hit.getEtaIndex(), hit.getPhiIndex());
    }

    for (size_t layer = 0; layer < m_nLayers_1st; layer++) {
        h_nHits_1st_layer->Fill(layer, nHits_layer[layer]);
    }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void HTTHitMonitorTool::fillHTTHitHistograms_2nd(std::vector<HTTHit> const * hits_2nd_mapped)
{
    if (!hits_2nd_mapped) {
        ATH_MSG_WARNING("Failed to retrieve second stage mapped hits.");
        return;
    }

    h_nHits_2nd->Fill(hits_2nd_mapped->size());

    std::vector<bool> layers_hit(m_nLayers_2nd);
    for (HTTHit const & h : *hits_2nd_mapped) layers_hit[h.getLayer()] = true;
    int layers = 0;
    for (bool hit : layers_hit) if (hit) layers++;
    h_nLayers_hit_2nd->Fill(layers);
    for (HTTHit const & h : *hits_2nd_mapped) h_hit_2nd_Layer->Fill(h.getLayer());

    std::vector<size_t> nHits_layer;
    nHits_layer.resize(m_nLayers_2nd);

    for (HTTHit const & hit : *hits_2nd_mapped) {
        nHits_layer[hit.getLayer()]++;
        h_Hit_2nd_coord_layer[hit.getLayer()]->Fill(hit.getEtaIndex(), hit.getPhiIndex());
    }

    for (size_t layer = 0; layer < m_nLayers_2nd; layer++) {
        h_nHits_2nd_layer->Fill(layer, nHits_layer[layer]);
    }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void HTTHitMonitorTool::fillMissedHitHistograms_1st(std::vector<HTTHit> const * hits_1st_miss, std::vector<HTTHit> const * hits_1st_mapped)
{
    if (!hits_1st_miss || !hits_1st_mapped) {
        ATH_MSG_WARNING("Failed to retrieve first stage missed/mapped hits.");
        return;
    }

    h_nMissedHits_1st->Fill(hits_1st_miss->size());
    h_nMissedHits_1st_Frac->Fill(float(hits_1st_miss->size()) / float(hits_1st_miss->size() + hits_1st_mapped->size()));
    if (hits_1st_miss->size() > 0 && !m_withPU) {
        int nmiss = hits_1st_miss->size();
        std::vector<HTTTruthTrack> truth_tracks = m_logicEventInputHeader_1st->optional().getTruthTracks();
        if (truth_tracks.size() == 1) {
            for (auto truth_t: truth_tracks) {
                h_nMissedHits_1st_vs_truth_pt->Fill(truth_t.getQ() * truth_t.getPt() * 0.001, nmiss);
                h_nMissedHits_1st_vs_truth_eta->Fill(truth_t.getEta(), nmiss);
                h_nMissedHits_1st_vs_truth_phi->Fill(truth_t.getPhi(), nmiss);
                h_nMissedHits_1st_vs_truth_d0->Fill(truth_t.getD0(), nmiss);
                h_nMissedHits_1st_vs_truth_z0->Fill(truth_t.getZ0(), nmiss);
            }
        }
    }

    for (HTTHit const & hit: *hits_1st_miss) {
        h_MissedHit_1st_eta->     Fill(hit.getHTTEtaModule()) ;
        h_MissedHit_1st_phi->     Fill(hit.getPhiModule());
        h_MissedHit_1st_type->    Fill(static_cast<int>(hit.getDetType()));
        h_MissedHit_1st_barrelEC->Fill(static_cast<int>(hit.getDetectorZone()));
        // ERROR: no layer if unmapped:
        // h_MissedHit_layer->   Fill(hit.getLayer());
        //isPixel (1/0) 2. isEndcap (1/0) 3. physical layer
         /* 1 2
            1 0
            0 2
            0 0
            */
        h_MissedHit_1st_physLayer-> Fill(getUniqueLayerId(hit));
        ATH_MSG_DEBUG("Missed hit: pixel=" << hit.isPixel() << " Barrel=" << hit.isBarrel() << " layer=" << hit.getPhysLayer());
    }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void HTTHitMonitorTool::fillMissedHitHistograms_2nd(std::vector<HTTHit> const * hits_2nd_miss, std::vector<HTTHit> const * hits_2nd_mapped)
{
    if (!hits_2nd_miss || !hits_2nd_mapped) {
        ATH_MSG_WARNING("Failed to retrieve second stage missed/mapped hits.");
        return;
    }

    h_nMissedHits_2nd->Fill(hits_2nd_miss->size());
    h_nMissedHits_2nd_Frac->Fill(float(hits_2nd_miss->size()) / float(hits_2nd_miss->size() + hits_2nd_mapped->size()));

    if (hits_2nd_miss->size() > 0 && !m_withPU) {
        int nmiss = hits_2nd_miss->size();
        std::vector<HTTTruthTrack> truth_tracks = m_logicEventInputHeader_1st->optional().getTruthTracks();
        if (truth_tracks.size() == 1) {
            for (auto truth_t: truth_tracks) {
                h_nMissedHits_2nd_vs_truth_pt->Fill(truth_t.getQ()*truth_t.getPt()*0.001, nmiss);
                h_nMissedHits_2nd_vs_truth_eta->Fill(truth_t.getEta(), nmiss);
                h_nMissedHits_2nd_vs_truth_phi->Fill(truth_t.getPhi(), nmiss);
                h_nMissedHits_2nd_vs_truth_d0->Fill(truth_t.getD0(), nmiss);
                h_nMissedHits_2nd_vs_truth_z0->Fill(truth_t.getZ0(), nmiss);
            }
        }
    }

    for (HTTHit const & hit: *hits_2nd_miss) {
        h_MissedHit_2nd_eta->     Fill(hit.getHTTEtaModule()) ;
        h_MissedHit_2nd_phi->     Fill(hit.getPhiModule());
        h_MissedHit_2nd_type->    Fill(static_cast<int>(hit.getDetType()));
        h_MissedHit_2nd_barrelEC->Fill(static_cast<int>(hit.getDetectorZone()));
        // ERROR: no layer if unmapped:
        // h_MissedHit_2nd_layer->   Fill(hit.getLayer());
        //isPixel (1/0) 2. isEndcap (1/0) 3. physical layer
         /* 1 2
            1 0
            0 2
            0 0
            */
        h_MissedHit_2nd_physLayer-> Fill(getUniqueLayerId(hit));
        ATH_MSG_DEBUG("Missed second stage hit: pixel=" << hit.isPixel() << " Barrel=" << hit.isBarrel() << " layer=" << hit.getPhysLayer());
    }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// to be moved to MapSvc?
int HTTHitMonitorTool::getUniqueLayerId(HTTHit const & hit)
{
    // isPixel (1/0) 2. isEndcap (1/0) 3. physical layer
         /* 1 2
            1 0
            0 2
            0 0
            */
    return (hit.isPixel() * 100 + (hit.isBarrel() + 2) * 100 + hit.getPhysLayer());
}

