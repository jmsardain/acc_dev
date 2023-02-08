/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTMonitor/HTTClusterMonitorTool.h"

#include "TrigHTTMaps/HTTRegionMap.h"
#include "TrigHTTMaps/HTTPlaneMap.h"
#include "TrigHTTObjects/HTTCluster.h"
#include "TrigHTTObjects/HTTLogicalEventInputHeader.h"
#include "TrigHTTUtils/HTTFunctions.h"
#include "TrigHTTUtils/HTTVectors.h"


static const InterfaceID IID_HTTClusterMonitorTool("HTTClusterMonitorTool", 1, 0);
const InterfaceID& HTTClusterMonitorTool::interfaceID()
{ return IID_HTTClusterMonitorTool; }


/////////////////////////////////////////////////////////////////////////////
HTTClusterMonitorTool::HTTClusterMonitorTool(std::string const & algname, std::string const & name, IInterface const * ifc) :
    HTTMonitorBase(algname,name,ifc)
{
    declareInterface<HTTClusterMonitorTool>(this);
    declareProperty("Clustering",           m_clustering,           "flag to enable the clustering");
    declareProperty("Spacepoints",          m_doSpacepoints,        "flag to enable spacepoints");
    declareProperty("RunSecondStage",       m_runSecondStage,       "flag to enable running the second stage fitting");
    declareProperty("canExtendHistRanges",  m_extend,               "extend x-axes of some histograms");
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTClusterMonitorTool::initialize()
{
    ATH_MSG_INFO("HTTClusterMonitorTool::initialize()");
    ATH_CHECK(HTTMonitorBase::initialize());
    ATH_CHECK(bookHistograms());
    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTClusterMonitorTool::bookHistograms()
{
    ATH_CHECK(bookHTTOfflineClusterHistograms());

    if (m_clustering) {
        ATH_CHECK(bookHTTClusterHistograms_1st());
        if (m_runSecondStage) ATH_CHECK(bookHTTClusterHistograms_2nd());
    }

    if (m_doSpacepoints) {
        ATH_CHECK(bookHTTSpacepointHistograms_1st());
        if (m_runSecondStage) ATH_CHECK(bookHTTSpacepointHistograms_2nd());
    }

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTClusterMonitorTool::fillHistograms()
{
    // Use event selection to pass or reject event based on truth info, sample type etc
    ATH_CHECK(selectEvent());

    fillHTTOfflineClusterHistograms();

    if (m_clustering) {
        fillHTTClusterHistograms_1st(getMonitorClusters("Clusters_1st"));
        if (m_runSecondStage) fillHTTClusterHistograms_2nd(getMonitorClusters("Clusters_2nd"));
        
    }

    if (m_doSpacepoints) {
        fillHTTSpacepointHistograms_1st(getMonitorClusters("Spacepoints_1st"));
        if (m_runSecondStage) fillHTTSpacepointHistograms_2nd(getMonitorClusters("Spacepoints_2nd"));
    }

    clearMonitorData();

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTClusterMonitorTool::finalize()
{
    ATH_MSG_INFO("HTTClusterMonitorTool::finalize()");
    ATH_CHECK(HTTMonitorBase::finalize());
    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTClusterMonitorTool::bookHTTOfflineClusterHistograms()
{
    HTTPlaneMap const * pmap_2nd = m_HTTMapping->PlaneMap_2nd();


    setHistDir(createDirName("/HTTOfflineClusterHist/AllLayerHist/"));

    h_nClustersOff = new TH1I("h_nClustersOff", "number of offline clusters found per event;# clusters;# events", 2500, -0.5, 2500-0.5);
    if (m_extend) h_nClustersOff->GetXaxis()->SetCanExtend(true);
    ATH_CHECK(regHist(getHistDir(), h_nClustersOff));

    h_ClusterOff_coord = new TH2I("h_ClusterOff_coord", "The row/col positions of offline clusters", 2000, -0.5, 2000 - 0.5, 1000, -0.5, 1000 - 0.5); ATH_CHECK(regHist(getHistDir(), h_ClusterOff_coord));

    h_nClustersOff_vs_truth_pt  = new TH2F("h_nClustersOff_vs_truth_pt", "Number of offline clusters vs truth q*pt", 100, -400. , 400., 50,-0.5,50-0.5); ATH_CHECK(regHist(getHistDir(), h_nClustersOff_vs_truth_pt));
    h_nClustersOff_vs_truth_eta = new TH2F("h_nClustersOff_vs_truth_eta", "Number of offline clusters vs truth eta", 50, m_etamin, m_etamax, 50,-0.5,50-0.5); ATH_CHECK(regHist(getHistDir(), h_nClustersOff_vs_truth_eta));
    h_nClustersOff_vs_truth_phi = new TH2F("h_nClustersOff_vs_truth_phi", "Number of offline clusters vs truth phi", 50, m_phimin, m_phimax, 50,-0.5,50-0.5); ATH_CHECK(regHist(getHistDir(), h_nClustersOff_vs_truth_phi));
    h_nClustersOff_vs_truth_d0  = new TH2F("h_nClustersOff_vs_truth_d0", "Number of offline clusters vs truth d0", 50, m_d0Range[0], m_d0Range[1], 50,-0.5,50-0.5); ATH_CHECK(regHist(getHistDir(), h_nClustersOff_vs_truth_d0));
    h_nClustersOff_vs_truth_z0  = new TH2F("h_nClustersOff_vs_truth_z0", "Number of offline clusters vs truth z0", 50, m_z0Range[0], m_z0Range[1], 50,-0.5,50-0.5); ATH_CHECK(regHist(getHistDir(), h_nClustersOff_vs_truth_z0));


    setHistDir(createDirName("/HTTOfflineClusterHist/PerLayerHist/"));

    h_ClusterOff_coord_layer.resize(m_nLayers_2nd);

    for (unsigned i = 0; i < m_nLayers_2nd; i++)
    {
        std::string name = pmap_2nd->layerName(i, 0); // assume first section

        int xbin = pmap_2nd->isPixel(i) ? 800 : 5;
        int ybin = pmap_2nd->isPixel(i) ? 900 : 1500;
        double xmax = xbin - 0.5;
        double ymax = ybin - 0.5;

        h_ClusterOff_coord_layer[i] = new TH2I(
                ("h_ClusterOff_coord_layer_" + std::to_string(i)).c_str(),
                ("The row/col positions of offline clusters in layer " + name + ";eta index; phi Index").c_str(),
                xbin, -0.5, xmax, ybin, -0.5, ymax
        );

        ATH_CHECK(regHist(getHistDir(), h_ClusterOff_coord_layer[i]));
    }   


    clearHistDir();

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTClusterMonitorTool::bookHTTClusterHistograms_1st()
{
    HTTPlaneMap const * pmap_1st = m_HTTMapping->PlaneMap_1st();
    unsigned nSubregions = m_HTTMapping->SubRegionMap()->getNRegions();


    setHistDir(createDirName("/HTTClusterHist_1st/AllLayerHist/"));

    h_nClusters_1st = new TH1I("h_nClusters_1st", "number of first stage clusters found per event;# clusters;# events", 2500, -0.5, 2500-0.5);
    if (m_extend) h_nClusters_1st->GetXaxis()->SetCanExtend(true);
    ATH_CHECK(regHist(getHistDir(), h_nClusters_1st));

    h_Cluster_1st_eta = new TH1I("h_Cluster_1st_eta", "eta strip of first stage clusters", 1500, -0.5, 1500 - 0.5); ATH_CHECK(regHist(getHistDir(), h_Cluster_1st_eta));
    h_Cluster_1st_phi = new TH1I("h_Cluster_1st_phi", "phi side of first stage clusters", 1500, -0.5, 1500 - 0.5); ATH_CHECK(regHist(getHistDir(), h_Cluster_1st_phi));
    h_Cluster_1st_etaWidth = new TH1I("h_Cluster_1st_etaWidth", "eta width of first stage clusters", 50, -0.5, 50 - 0.5); ATH_CHECK(regHist(getHistDir(), h_Cluster_1st_etaWidth));
    h_Cluster_1st_phiWidth = new TH1I("h_Cluster_1st_phiWidth", "phi width of first stage clusters", 100, -0.5, 150 - 0.5); ATH_CHECK(regHist(getHistDir(), h_Cluster_1st_phiWidth));
    h_Cluster_1st_barrelEC = new TH1I("h_Cluster_1st_barrelEC", "barrelEC of first stage clusters", 2, -0.5, 2-0.5); ATH_CHECK(regHist(getHistDir(), h_Cluster_1st_barrelEC));
    h_Cluster_1st_layer = new TH1I("h_Cluster_1st_layer", "layer of first stage clusters", m_nLayers_1st, -0.5, m_nLayers_1st-0.5); ATH_CHECK(regHist(getHistDir(), h_Cluster_1st_layer));
    h_Cluster_1st_physLayer = new TH1I("h_Cluster_1st_physLayer", "ITK layer of first stage cluster centroids", 600, -0.5, 600-0.5); ATH_CHECK(regHist(getHistDir(), h_Cluster_1st_physLayer));
    h_Cluster_1st_coord = new TH2I("h_Cluster_1st_coord", "The row/col positions of first stage clusters", 800, -0.5, 800 - 0.5, 1500, -0.5, 1500 - 0.5); ATH_CHECK(regHist(getHistDir(), h_Cluster_1st_coord));
    h_Cluster_1st_nSubregions = new TH1I("h_Cluster_1st_nSubregions", "number of subregions each first stage cluster is in;# subregions;# clusters", nSubregions, 0.5, nSubregions+0.5); // exclude 0
    ATH_CHECK(regHist(getHistDir(), h_Cluster_1st_nSubregions));

    h_nClusters_1st_subregion = new TH2I(
            "h_nClusters_1st_subregion",
            "number of first stage clusters in each subregion per event;subregion;# clusters / event",
            nSubregions, -0.5, nSubregions-0.5, 500, 0, 5000
    );
    ATH_CHECK(regHist(getHistDir(), h_nClusters_1st_subregion));

    h_nClusters_1st_vs_truth_pt = new TH2F("h_nClusters_1st_vs_truth_pt", "Number of first stage first stage clusters vs truth q*pt", 100, -400. , 400., 50,-0.5,50-0.5); ATH_CHECK(regHist(getHistDir(), h_nClusters_1st_vs_truth_pt));
    h_nClusters_1st_vs_truth_eta = new TH2F("h_nClusters_1st_vs_truth_eta", "Number of first stage first stage clusters vs truth eta", 50, m_etamin, m_etamax, 50,-0.5,50-0.5); ATH_CHECK(regHist(getHistDir(), h_nClusters_1st_vs_truth_eta));
    h_nClusters_1st_vs_truth_phi = new TH2F("h_nClusters_1st_vs_truth_phi", "Number of first stage first stage clusters vs truth phi", 50, m_phimin, m_phimax, 50,-0.5,50-0.5); ATH_CHECK(regHist(getHistDir(), h_nClusters_1st_vs_truth_phi));
    h_nClusters_1st_vs_truth_d0 = new TH2F("h_nClusters_1st_vs_truth_d0", "Number of first stage first stage clusters vs truth d0", 50, m_d0Range[0], m_d0Range[1], 50,-0.5,50-0.5); ATH_CHECK(regHist(getHistDir(), h_nClusters_1st_vs_truth_d0));
    h_nClusters_1st_vs_truth_z0 = new TH2F("h_nClusters_1st_vs_truth_z0", "Number of first stage first stage clusters vs truth z0", 50, m_z0Range[0], m_z0Range[1], 50,-0.5,50-0.5); ATH_CHECK(regHist(getHistDir(), h_nClusters_1st_vs_truth_z0));


    setHistDir(createDirName("/HTTClusterHist_1st/PerLayerHist/"));

    // repeated here because needed as general ---> FP
    h_nClusters_1st_layer = new TH2I(
            "h_nClusters_1st_layer",
            "number of first stage clusters in each layer per event;layer;# clusters / event",
            m_nLayers_1st, -0.5, m_nLayers_1st-0.5, 200, 0, 2000
    );
    h_nClusters_1st_PerSubregion_layer = new TH2I(
            "h_nClusters_1st_PerSubregion_layer",
            "Number of first stage clusters per subregion in each layer per event;layer;# clusters / event / subregion",
            m_nLayers_1st, -0.5, m_nLayers_1st-0.5, 500, -0.5, 500-0.5
    );
    h_Cluster_1st_etaWidth_layer = new TH2I(
            "h_Cluster_1st_etaWidth_layer",
            "eta width of first stage clusters in each layer per event;layer;eta width / event",
            m_nLayers_1st, -0.5, m_nLayers_1st-0.5, 20, -0.5, 20 - 0.5
    );
    h_Cluster_1st_phiWidth_layer = new TH2I(
            "h_Cluster_1st_phiWidth_layer",
            "phi width of first stage clusters in each layer per event;layer;phi width / event",
            m_nLayers_1st, -0.5, m_nLayers_1st-0.5, 20, -0.5, 20 - 0.5
    );
    ATH_CHECK(regHist(getHistDir(), h_nClusters_1st_layer));
    ATH_CHECK(regHist(getHistDir(), h_nClusters_1st_PerSubregion_layer));
    ATH_CHECK(regHist(getHistDir(), h_Cluster_1st_etaWidth_layer));
    ATH_CHECK(regHist(getHistDir(), h_Cluster_1st_phiWidth_layer));

    h_Cluster_1st_coord_layer.resize(m_nLayers_1st);

    for (unsigned i = 0; i < m_nLayers_1st; i++)
    {
        std::string name = pmap_1st->layerName(i, 0); // assume first section

        int xbin = pmap_1st->isPixel(i) ? 800 : 5;
        int ybin = pmap_1st->isPixel(i) ? 900 : 1500;
        double xmax = xbin - 0.5;
        double ymax = ybin - 0.5;

        h_Cluster_1st_coord_layer[i] = new TH2I(
                ("h_Cluster_1st_coord_layer_" + std::to_string(i)).c_str(),
                ("The row/col positions of first stage clusters in layer " + name+";eta index; phi Index").c_str(),
                xbin, -0.5, xmax, ybin, -0.5, ymax
        );

        ATH_CHECK(regHist(getHistDir(), h_Cluster_1st_coord_layer[i]));
    }


    clearHistDir();

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTClusterMonitorTool::bookHTTClusterHistograms_2nd()
{
    HTTPlaneMap const * pmap_2nd = m_HTTMapping->PlaneMap_2nd();
    unsigned nSubregions = m_HTTMapping->SubRegionMap()->getNRegions();


    setHistDir(createDirName("/HTTClusterHist_2nd/AllLayerHist/"));

    h_nClusters_2nd = new TH1I("h_nClusters_2nd", "number of second stage clusters found per event;# clusters;# events", 2500, -0.5, 2500-0.5);
    if (m_extend) h_nClusters_2nd->GetXaxis()->SetCanExtend(true);
    ATH_CHECK(regHist(getHistDir(), h_nClusters_2nd));

    h_Cluster_2nd_eta = new TH1I("h_Cluster_2nd_eta", "eta strip of second stage clusters", 1500, -0.5, 1500 - 0.5); ATH_CHECK(regHist(getHistDir(), h_Cluster_2nd_eta));
    h_Cluster_2nd_phi = new TH1I("h_Cluster_2nd_phi", "phi side of second stage clusters", 1500, -0.5, 1500 - 0.5); ATH_CHECK(regHist(getHistDir(), h_Cluster_2nd_phi));
    h_Cluster_2nd_etaWidth = new TH1I("h_Cluster_2nd_etaWidth", "eta width of second stage clusters", 50, -0.5, 50 - 0.5); ATH_CHECK(regHist(getHistDir(), h_Cluster_2nd_etaWidth));
    h_Cluster_2nd_phiWidth = new TH1I("h_Cluster_2nd_phiWidth", "phi width of second stage clusters", 100, -0.5, 150 - 0.5); ATH_CHECK(regHist(getHistDir(), h_Cluster_2nd_phiWidth));
    h_Cluster_2nd_barrelEC = new TH1I("h_Cluster_2nd_barrelEC", "barrelEC of second stage clusters", 2, -0.5, 2-0.5); ATH_CHECK(regHist(getHistDir(), h_Cluster_2nd_barrelEC));
    h_Cluster_2nd_layer = new TH1I("h_Cluster_2nd_layer", "layer of second stage clusters", m_nLayers_2nd, -0.5, m_nLayers_2nd-0.5); ATH_CHECK(regHist(getHistDir(), h_Cluster_2nd_layer));
    h_Cluster_2nd_physLayer = new TH1I("h_Cluster_2nd_physLayer", "ITK layer of second stage cluster centroids", 600, -0.5, 600-0.5); ATH_CHECK(regHist(getHistDir(), h_Cluster_2nd_physLayer));
    h_Cluster_2nd_coord = new TH2I("h_Cluster_2nd_coord", "The row/col positions of second stage clusters", 800, -0.5, 800 - 0.5, 1500, -0.5, 1500 - 0.5); ATH_CHECK(regHist(getHistDir(), h_Cluster_2nd_coord));
    h_Cluster_2nd_nSubregions = new TH1I("h_Cluster_2nd_nSubregions", "number of subregions each second stage cluster is in;# subregions;# clusters", nSubregions, 0.5, nSubregions+0.5); // exclude 0
    ATH_CHECK(regHist(getHistDir(), h_Cluster_2nd_nSubregions));

    h_nClusters_2nd_subregion = new TH2I(
            "h_nClusters_2nd_subregion",
            "number of second stage clusters in each subregion per event;subregion;# clusters / event",
            nSubregions, -0.5, nSubregions-0.5, 500, 0, 5000
    );
    ATH_CHECK(regHist(getHistDir(), h_nClusters_2nd_subregion));

    h_nClusters_2nd_vs_truth_pt = new TH2F("h_nClusters_2nd_vs_truth_pt", "Number of second stage clusters vs truth q*pt", 100, -400. , 400., 50,-0.5,50-0.5); ATH_CHECK(regHist(getHistDir(), h_nClusters_2nd_vs_truth_pt));
    h_nClusters_2nd_vs_truth_eta = new TH2F("h_nClusters_2nd_vs_truth_eta", "Number of second stage clusters vs truth eta", 50, m_etamin, m_etamax, 50,-0.5,50-0.5); ATH_CHECK(regHist(getHistDir(), h_nClusters_2nd_vs_truth_eta));
    h_nClusters_2nd_vs_truth_phi = new TH2F("h_nClusters_2nd_vs_truth_phi", "Number of second stage clusters vs truth phi", 50, m_phimin, m_phimax, 50,-0.5,50-0.5); ATH_CHECK(regHist(getHistDir(), h_nClusters_2nd_vs_truth_phi));
    h_nClusters_2nd_vs_truth_d0 = new TH2F("h_nClusters_2nd_vs_truth_d0", "Number of second stage clusters vs truth d0", 50, m_d0Range[0], m_d0Range[1], 50,-0.5,50-0.5); ATH_CHECK(regHist(getHistDir(), h_nClusters_2nd_vs_truth_d0));
    h_nClusters_2nd_vs_truth_z0 = new TH2F("h_nClusters_2nd_vs_truth_z0", "Number of second stage clusters vs truth z0", 50, m_z0Range[0], m_z0Range[1], 50,-0.5,50-0.5); ATH_CHECK(regHist(getHistDir(), h_nClusters_2nd_vs_truth_z0));


    setHistDir(createDirName("/HTTClusterHist_2nd/PerLayerHist/"));

    // repeated here because needed as general ---> FP
    h_nClusters_2nd_layer = new TH2I(
            "h_nClusters_2nd_layer",
            "number of second stage clusters in each layer per event;layer;# clusters / event",
            m_nLayers_2nd, -0.5, m_nLayers_2nd-0.5, 200, 0, 2000
    );
    h_nClusters_2nd_PerSubregion_layer = new TH2I(
            "h_nClusters_2nd_PerSubregion_layer",
            "Number of second stage clusters per subregion in each layer per event;layer;# clusters / event / subregion",
            m_nLayers_2nd, -0.5, m_nLayers_2nd-0.5, 500, -0.5, 500-0.5
    );
    h_Cluster_2nd_etaWidth_layer = new TH2I(
            "h_Cluster_2nd_etaWidth_layer",
            "eta width of second stage clusters in each layer per event;layer;eta width / event",
            m_nLayers_2nd, -0.5, m_nLayers_2nd-0.5, 20, -0.5, 20 - 0.5
    );
    h_Cluster_2nd_phiWidth_layer = new TH2I(
            "h_Cluster_2nd_phiWidth_layer",
            "phi width of second stage clusters in each layer per event;layer;phi width / event",
            m_nLayers_2nd, -0.5, m_nLayers_2nd-0.5, 20, -0.5, 20 - 0.5
    );
    ATH_CHECK(regHist(getHistDir(), h_nClusters_2nd_layer));
    ATH_CHECK(regHist(getHistDir(), h_nClusters_2nd_PerSubregion_layer));
    ATH_CHECK(regHist(getHistDir(), h_Cluster_2nd_etaWidth_layer));
    ATH_CHECK(regHist(getHistDir(), h_Cluster_2nd_phiWidth_layer));

    h_Cluster_2nd_coord_layer.resize(m_nLayers_2nd);

    for (unsigned i = 0; i < m_nLayers_2nd; i++)
    {
        std::string name = pmap_2nd->layerName(i, 0); // assume first section

        int xbin = pmap_2nd->isPixel(i) ? 800 : 5;
        int ybin = pmap_2nd->isPixel(i) ? 900 : 1500;
        double xmax = xbin - 0.5;
        double ymax = ybin - 0.5;

        h_Cluster_2nd_coord_layer[i] = new TH2I(
                ("h_Cluster_2nd_coord_layer_" + std::to_string(i)).c_str(),
                ("The row/col positions of second stage clusters in layer " + name + ";eta index; phi Index").c_str(),
                xbin, -0.5, xmax, ybin, -0.5, ymax
        );

        ATH_CHECK(regHist(getHistDir(), h_Cluster_2nd_coord_layer[i]));
    }


    clearHistDir();

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTClusterMonitorTool::bookHTTSpacepointHistograms_1st()
{
    setHistDir(createDirName("/HTTSpacepointHist_1st/"));

    h_nSpacepoints_1st = new TH1I("h_nSpacepoints_1st", "number of first stage spacepoints per event;# spacepoints;# events", h_nClusters_1st->GetNbinsX(), h_nClusters_1st->GetXaxis()->GetBinLowEdge(1), h_nClusters_1st->GetXaxis()->GetBinLowEdge(h_nClusters_1st->GetNbinsX()+1)); // want the same binning as h_nClusters_1st so can combine histograms
    h_Spacepoint_1st_nHits = new TH1I("h_Spacepoint_1st_nHits", "number of hits in first stage spacepoint;# hits;# spacepoints", 3, -0.5, 3-0.5);
    h_Spacepoint_1st_globalResidual = new TH1F("h_Spacepoint_1st_globalResidual", "absolute value of global residual between first stage spacepoints and hits;residual [mm];# spacepoints / 125 #mum", 200, 0, 25);
    h_Spacepoint_1st_angle_layer = new TH2F("h_Spacepoint_1st_angle_layer", "Angle between clusters forming first stage spacepoint in each layer;opening angle between vectors to spacepoint hits [rad];# spacepoints / 0.15 mrad", 4, -0.5, 4-0.5, 60, 0., 0.009);
    h_Spacepoint_1st_distance_layer = new TH2F("h_Spacepoint_1st_distance_layer", "Absolute distance between clusters forming first stage spacepoint in each layer;(global) distance [mm];# spacepoints / 0.5 mm", 4, -0.5, 4-0.5, 200, 0., 100.);

    h_Spacepoint_1st_globalXY = new TH2I("h_Spacepoint_1st_globalXY", "The global x/y positions of spacepoints;x (mm);y (mm)", 600, -1200, 1200, 600, -1200, 1200);
    h_Spacepoint_1st_globalRZ = new TH2I("h_Spacepoint_1st_globalRZ", "The global r/z positions of spacepoints;z (mm);r (mm)", 300, 0, 1200, 1500, -3000, 3000);

    if(m_extend) h_nSpacepoints_1st->SetCanExtend(true);
    ATH_CHECK(regHist(getHistDir(), h_nSpacepoints_1st));
    ATH_CHECK(regHist(getHistDir(), h_Spacepoint_1st_nHits));
    ATH_CHECK(regHist(getHistDir(), h_Spacepoint_1st_globalResidual));
    ATH_CHECK(regHist(getHistDir(), h_Spacepoint_1st_angle_layer));
    ATH_CHECK(regHist(getHistDir(), h_Spacepoint_1st_distance_layer));
    ATH_CHECK(regHist(getHistDir(), h_Spacepoint_1st_globalXY));
    ATH_CHECK(regHist(getHistDir(), h_Spacepoint_1st_globalRZ));


    clearHistDir();

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTClusterMonitorTool::bookHTTSpacepointHistograms_2nd()
{
    setHistDir(createDirName("/HTTSpacepointHist_2nd/"));

    h_nSpacepoints_2nd = new TH1I("h_nSpacepoints_2nd", "number of first stage spacepoints per event;# spacepoints;# events", h_nClusters_2nd->GetNbinsX(), h_nClusters_2nd->GetXaxis()->GetBinLowEdge(1), h_nClusters_2nd->GetXaxis()->GetBinLowEdge(h_nClusters_2nd->GetNbinsX()+1)); // want the same binning as h_nClusters_2nd so can combine histograms
    h_Spacepoint_2nd_nHits = new TH1I("h_Spacepoint_2nd_nHits", "number of hits in first stage spacepoint;# hits;# spacepoints", 3, -0.5, 3-0.5);
    h_Spacepoint_2nd_globalResidual = new TH1F("h_Spacepoint_2nd_globalResidual", "absolute value of global residual between first stage spacepoints and hits;residual [mm];# spacepoints / 125 #mum", 200, 0, 25);
    h_Spacepoint_2nd_angle_layer = new TH2F("h_Spacepoint_2nd_angle_layer", "Angle between clusters forming first stage spacepoint in each layer;opening angle between vectors to spacepoint hits [rad];# spacepoints / 0.15 mrad", 4, -0.5, 4-0.5, 60, 0., 0.009);
    h_Spacepoint_2nd_distance_layer = new TH2F("h_Spacepoint_2nd_distance_layer", "Absolute distance between clusters forming first stage spacepoint in each layer;(global) distance [mm];# spacepoints / 0.5 mm", 4, -0.5, 4-0.5, 200, 0., 100.);

    h_Spacepoint_2nd_globalXY = new TH2I("h_Spacepoint_2nd_globalXY", "The global x/y positions of spacepoints;x (mm);y (mm)", 600, -1200, 1200, 600, -1200, 1200);
    h_Spacepoint_2nd_globalRZ = new TH2I("h_Spacepoint_2nd_globalRZ", "The global r/z positions of spacepoints;z (mm);r (mm)", 300, 0, 1200, 1500, -3000, 3000);

    if(m_extend) h_nSpacepoints_2nd->SetCanExtend(true);
    ATH_CHECK(regHist(getHistDir(), h_nSpacepoints_2nd));
    ATH_CHECK(regHist(getHistDir(), h_Spacepoint_2nd_nHits));
    ATH_CHECK(regHist(getHistDir(), h_Spacepoint_2nd_globalResidual));
    ATH_CHECK(regHist(getHistDir(), h_Spacepoint_2nd_angle_layer));
    ATH_CHECK(regHist(getHistDir(), h_Spacepoint_2nd_distance_layer));
    ATH_CHECK(regHist(getHistDir(), h_Spacepoint_2nd_globalXY));
    ATH_CHECK(regHist(getHistDir(), h_Spacepoint_2nd_globalRZ));


    clearHistDir();

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void HTTClusterMonitorTool::fillHTTOfflineClusterHistograms()
{
    unsigned nClustersOff = 0;
    for (HTTCluster const & cluster : m_logicEventInputHeader_1st->optional().getOfflineClusters())
    {
        HTTHit ClusterEquiv = cluster.getClusterEquiv();
        // hack: the offlien clusters appear as clustered, but they are not mapped
        ClusterEquiv.setHitType(HitType::unmapped);
        m_HTTMapping->PlaneMap_2nd()->map(ClusterEquiv);
        ClusterEquiv.setHitType(HitType::clustered);
        // if (ClusterEquiv.getLayer() == -1) continue;
        if (!m_HTTMapping->RegionMap_2nd()->isInRegion(0, ClusterEquiv)) continue;
        nClustersOff++;
        h_ClusterOff_coord->Fill(ClusterEquiv.getPhiIndex(), ClusterEquiv.getEtaIndex());
        h_ClusterOff_coord_layer[ClusterEquiv.getLayer()]->Fill(ClusterEquiv.getEtaIndex(), ClusterEquiv.getPhiIndex());
    }
    h_nClustersOff->Fill(nClustersOff);

    if (!m_withPU) {
        std::vector<HTTTruthTrack> truth_tracks = m_logicEventInputHeader_1st->optional().getTruthTracks();
        if (truth_tracks.size() == 1) {
            for (auto truth_t : truth_tracks) {
                h_nClustersOff_vs_truth_pt->Fill(truth_t.getQ() *truth_t.getPt()*0.001, nClustersOff);
                h_nClustersOff_vs_truth_eta->Fill(truth_t.getEta(), nClustersOff);
                h_nClustersOff_vs_truth_phi->Fill(truth_t.getPhi(), nClustersOff);
                h_nClustersOff_vs_truth_d0->Fill(truth_t.getD0(), nClustersOff);
                h_nClustersOff_vs_truth_z0->Fill(truth_t.getZ0(), nClustersOff);
            }
        }
    }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void HTTClusterMonitorTool::fillHTTClusterHistograms_1st(std::vector<HTTCluster> const * clusters_1st)
{
    if (!clusters_1st) {
        ATH_MSG_WARNING("Failed to retrieve first stage clusters.");
        return;
    }

    std::vector<size_t> nClusters_layer;
    nClusters_layer.resize(m_nLayers_1st);

    h_nClusters_1st->Fill(clusters_1st->size());

    vector2D<size_t> counts(m_HTTMapping->SubRegionMap()->getNRegions(), m_HTTMapping->PlaneMap_1st()->getNLogiLayers());
    for (HTTCluster const & cluster : *clusters_1st)
    {
        HTTHit const & ClusterEquiv = cluster.getClusterEquiv();
        h_Cluster_1st_eta->Fill(ClusterEquiv.getEtaIndex());
        h_Cluster_1st_phi->Fill(ClusterEquiv.getPhiIndex());
        h_Cluster_1st_etaWidth->Fill(ClusterEquiv.getEtaWidth());
        h_Cluster_1st_phiWidth->Fill(ClusterEquiv.getPhiWidth());
        h_Cluster_1st_barrelEC->Fill(static_cast<int>(ClusterEquiv.getDetectorZone()));
        h_Cluster_1st_layer->Fill(ClusterEquiv.getLayer());
        h_Cluster_1st_physLayer-> Fill(getUniqueLayerId(ClusterEquiv));
        h_Cluster_1st_coord->Fill(ClusterEquiv.getEtaCoord(),ClusterEquiv.getPhiCoord());

        int layer = ClusterEquiv.getLayer();
        nClusters_layer[layer]++;
        h_Cluster_1st_coord_layer[layer]->Fill(ClusterEquiv.getEtaCoord(), ClusterEquiv.getPhiCoord());
        h_Cluster_1st_etaWidth_layer->Fill(layer, ClusterEquiv.getEtaWidth());
        h_Cluster_1st_phiWidth_layer->Fill(layer, ClusterEquiv.getPhiWidth());

        std::vector<uint32_t> regions = m_HTTMapping->SubRegionMap()->getRegions(ClusterEquiv);
        for (uint32_t region : regions) counts(region, layer)++;
        h_Cluster_1st_nSubregions->Fill(regions.size());
    }

    for (unsigned region = 0; region < counts.size(0); region++)
    {
        size_t count_subregion = 0;
        for (unsigned layer = 0; layer < counts.size(1); layer++)
        {
            h_nClusters_1st_PerSubregion_layer->Fill(layer, counts(region, layer));
            count_subregion += counts(region, layer);
        }
        h_nClusters_1st_subregion->Fill(region, count_subregion);
    }

    for (size_t layer = 0; layer < m_nLayers_1st; layer++) {
        h_nClusters_1st_layer->Fill(layer, nClusters_layer[layer]);
    }

    if (!m_withPU){
        int nClusters_1st = clusters_1st->size();
        std::vector<HTTTruthTrack> truth_tracks = m_logicEventInputHeader_1st->optional().getTruthTracks();
        if (truth_tracks.size() == 1) {
            for (auto truth_t : truth_tracks) {
                h_nClusters_1st_vs_truth_pt->Fill(truth_t.getQ() * truth_t.getPt() * 0.001, nClusters_1st);
                h_nClusters_1st_vs_truth_eta->Fill(truth_t.getEta(), nClusters_1st);
                h_nClusters_1st_vs_truth_phi->Fill(truth_t.getPhi(), nClusters_1st);
                h_nClusters_1st_vs_truth_d0->Fill(truth_t.getD0(), nClusters_1st);
                h_nClusters_1st_vs_truth_z0->Fill(truth_t.getZ0(), nClusters_1st);
            }
        }
    }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void HTTClusterMonitorTool::fillHTTClusterHistograms_2nd(std::vector<HTTCluster> const * clusters_2nd)
{
    if (!clusters_2nd) {
        ATH_MSG_WARNING("Failed to retrieve second stage clusters.");
        return;
    }

    std::vector<size_t> nClusters_layer;
    nClusters_layer.resize(m_nLayers_2nd);

    h_nClusters_2nd->Fill(clusters_2nd->size());

    vector2D<size_t> counts(m_HTTMapping->SubRegionMap()->getNRegions(), m_HTTMapping->PlaneMap_2nd()->getNLogiLayers());
    for (HTTCluster const & cluster : *clusters_2nd)
    {
        HTTHit const & ClusterEquiv = cluster.getClusterEquiv();
        h_Cluster_2nd_eta->Fill(ClusterEquiv.getEtaIndex());
        h_Cluster_2nd_phi->Fill(ClusterEquiv.getPhiIndex());
        h_Cluster_2nd_etaWidth->Fill(ClusterEquiv.getEtaWidth());
        h_Cluster_2nd_phiWidth->Fill(ClusterEquiv.getPhiWidth());
        h_Cluster_2nd_barrelEC->Fill(static_cast<int>(ClusterEquiv.getDetectorZone()));
        h_Cluster_2nd_layer->Fill(ClusterEquiv.getLayer());
        h_Cluster_2nd_physLayer-> Fill(getUniqueLayerId(ClusterEquiv));
        h_Cluster_2nd_coord->Fill(ClusterEquiv.getEtaCoord(),ClusterEquiv.getPhiCoord());

        int layer = ClusterEquiv.getLayer();
        nClusters_layer[layer]++;
        h_Cluster_2nd_coord_layer[layer]->Fill(ClusterEquiv.getEtaCoord(), ClusterEquiv.getPhiCoord());
        h_Cluster_2nd_etaWidth_layer->Fill(layer, ClusterEquiv.getEtaWidth());
        h_Cluster_2nd_phiWidth_layer->Fill(layer, ClusterEquiv.getPhiWidth());

        std::vector<uint32_t> regions = m_HTTMapping->SubRegionMap()->getRegions(ClusterEquiv);
        for (uint32_t region : regions) counts(region, layer)++;
        h_Cluster_2nd_nSubregions->Fill(regions.size());
    }

    for (unsigned region = 0; region < counts.size(0); region++)
    {
        size_t count_subregion = 0;
        for (unsigned layer = 0; layer < counts.size(1); layer++)
        {
            h_nClusters_2nd_PerSubregion_layer->Fill(layer, counts(region, layer));
            count_subregion += counts(region, layer);
        }
        h_nClusters_2nd_subregion->Fill(region, count_subregion);
    }

    for (size_t layer = 0; layer < m_nLayers_2nd; layer++) {
        h_nClusters_2nd_layer->Fill(layer, nClusters_layer[layer]);
    }

    if (!m_withPU) {
        int nClusters_2nd = clusters_2nd->size();
        std::vector<HTTTruthTrack> truth_tracks = m_logicEventInputHeader_1st->optional().getTruthTracks();
        if (truth_tracks.size() == 1) {
            for (auto truth_t : truth_tracks) {
                h_nClusters_2nd_vs_truth_pt->Fill(truth_t.getQ() * truth_t.getPt() * 0.001, nClusters_2nd);
                h_nClusters_2nd_vs_truth_eta->Fill(truth_t.getEta(), nClusters_2nd);
                h_nClusters_2nd_vs_truth_phi->Fill(truth_t.getPhi(), nClusters_2nd);
                h_nClusters_2nd_vs_truth_d0->Fill(truth_t.getD0(), nClusters_2nd);
                h_nClusters_2nd_vs_truth_z0->Fill(truth_t.getZ0(), nClusters_2nd);
            }
        }
    }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void HTTClusterMonitorTool::fillHTTSpacepointHistograms_1st(std::vector<HTTCluster> const * spacepoints_1st)
{
    if (!spacepoints_1st) {
        ATH_MSG_WARNING("Failed to retrieve first stage spacepoints.");
        return;
    }

    size_t nSpacepoints = 0;

    for (auto sp: *spacepoints_1st) {
        float res = -999.;
        HTTHit hit = sp.getClusterEquiv();

        // calculate residual
        if (hit.getHitType() == HitType::spacepoint) {
            // global coordinates of the cluster on the inner layer of the stave
            float a[] = {sp.getHitList().at(0).getX(), sp.getHitList().at(0).getY(), sp.getHitList().at(0).getZ()};
            // global coordinates of the spacepoint
            float h[] = {hit.getX(), hit.getY(), hit.getZ()};

            res = eucnorm(a[0] - h[0], a[1] - h[1], a[2] - h[2]);
        }

        h_Spacepoint_1st_nHits->Fill(sp.getHitList().size());
        h_Spacepoint_1st_globalResidual->Fill(res);

        h_Spacepoint_1st_globalXY->Fill(hit.getX(), hit.getY());
        h_Spacepoint_1st_globalRZ->Fill(hit.getZ(), hit.getR());

        // TODO this layering indexing is incorrect, especially for the endcaps
        for (int i = 0; i < 4; ++i) {
            if (hit.isPixel()) {
                ++nSpacepoints;
                break;
            }
            if (sp.getHitList().size() < 2) {
                ++nSpacepoints;
                break;
            }
            // check if this is an actual spacepoints
            if (hit.getHitType() != HitType::spacepoint) {
                ++nSpacepoints;
                break;
            }
            if ((int)((float)hit.getPhysLayer() / 2.) != i) continue;

            // global coordinates of the two clusters forming the spacepoint
            float a[] = {sp.getHitList().at(0).getX(), sp.getHitList().at(0).getY(), sp.getHitList().at(0).getZ()};
            float b[] = {sp.getHitList().at(1).getX(), sp.getHitList().at(1).getY(), sp.getHitList().at(1).getZ()};
            // opening angle between the two vectors pointing to the two clusters
            double alpha = eucangle(a[0], a[1], a[2], b[0], b[1], b[2]);
            // distance between the two clusters
            double dl = eucnorm(a[0] - b[0], a[1] - b[1], a[2] - b[2]);

            h_Spacepoint_1st_angle_layer->Fill(i, fabs(alpha));
            h_Spacepoint_1st_distance_layer->Fill(i, fabs(dl));
            ++nSpacepoints;
        }
    }

    h_nSpacepoints_1st->Fill(nSpacepoints);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void HTTClusterMonitorTool::fillHTTSpacepointHistograms_2nd(std::vector<HTTCluster> const * spacepoints_2nd)
{
    if (!spacepoints_2nd) {
        ATH_MSG_WARNING("Failed to retrieve second stage spacepoints.");
        return;
    }

    size_t nSpacepoints = 0;

    for (auto sp: *spacepoints_2nd) {
        float res = -999.;
        HTTHit hit = sp.getClusterEquiv();

        // calculate residual
        if (hit.getHitType() == HitType::spacepoint) {
            // global coordinates of the cluster on the inner layer of the stave
            float a[] = {sp.getHitList().at(0).getX(), sp.getHitList().at(0).getY(), sp.getHitList().at(0).getZ()};
            // global coordinates of the spacepoint
            float h[] = {hit.getX(), hit.getY(), hit.getZ()};

            res = eucnorm(a[0] - h[0], a[1] - h[1], a[2] - h[2]);
        }

        h_Spacepoint_2nd_nHits->Fill(sp.getHitList().size());
        h_Spacepoint_2nd_globalResidual->Fill(res);

        h_Spacepoint_2nd_globalXY->Fill(hit.getX(), hit.getY());
        h_Spacepoint_2nd_globalRZ->Fill(hit.getZ(), hit.getR());

        // TODO this layering indexing is incorrect, especially for the endcaps
        for (int i = 0; i < 4; ++i) {
            if (hit.isPixel()) {
                ++nSpacepoints;
                break;
            }
            if (sp.getHitList().size() < 2) {
                ++nSpacepoints;
                break;
            }
            // check if this is an actual spacepoints
            if (hit.getHitType() != HitType::spacepoint) {
                ++nSpacepoints;
                break;
            }
            if ((int)((float)hit.getPhysLayer() / 2.) != i) continue;

            // global coordinates of the two clusters forming the spacepoint
            float a[] = {sp.getHitList().at(0).getX(), sp.getHitList().at(0).getY(), sp.getHitList().at(0).getZ()};
            float b[] = {sp.getHitList().at(1).getX(), sp.getHitList().at(1).getY(), sp.getHitList().at(1).getZ()};
            // opening angle between the two vectors pointing to the two clusters
            double alpha = eucangle(a[0], a[1], a[2], b[0], b[1], b[2]);
            // distance between the two clusters
            double dl = eucnorm(a[0] - b[0], a[1] - b[1], a[2] - b[2]);

            h_Spacepoint_2nd_angle_layer->Fill(i, fabs(alpha));
            h_Spacepoint_2nd_distance_layer->Fill(i, fabs(dl));
            ++nSpacepoints;
        }
    }

    h_nSpacepoints_2nd->Fill(nSpacepoints);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// to be moved to MapSvc?
int HTTClusterMonitorTool::getUniqueLayerId(HTTHit const & hit)
{
    // isPixel (1/0) 2. isEndcap (1/0) 3. physical layer
         /* 1 2
            1 0
            0 2
            0 0
            */
    return (hit.isPixel() * 100 + (hit.isBarrel() + 2) * 100 + hit.getPhysLayer());
}
