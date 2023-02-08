/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTMonitor/HTTEventMonitorTool.h"

#include "TrigHTTObjects/HTTLogicalEventInputHeader.h"
#include "TrigHTTObjects/HTTLogicalEventOutputHeader.h"


static const InterfaceID IID_HTTEventMonitorTool("HTTEventMonitorTool", 1, 0);
const InterfaceID& HTTEventMonitorTool::interfaceID()
{ return IID_HTTEventMonitorTool; }


/////////////////////////////////////////////////////////////////////////////
HTTEventMonitorTool::HTTEventMonitorTool(std::string const & algname, std::string const & name, IInterface const * ifc) :
    HTTMonitorBase(algname,name,ifc)
{
    declareInterface<HTTEventMonitorTool>(this);
    declareProperty("fastMon",              m_fast,                 "only do fast monitoring");
    declareProperty("OutputMon",            m_outputMonitor,        "do monitoring on output data (HTTLogicalEventInputHeader, HTTLogicalEventOutputHeader)");
    declareProperty("Clustering",           m_clustering,           "flag to enable the clustering");
    declareProperty("Spacepoints",          m_doSpacepoints,        "flag to enable spacepoints");
    declareProperty("RunSecondStage",       m_runSecondStage,       "flag to enable running the second stage fitting");
    declareProperty("BarcodeFracCut",       m_cut_barcodefrac,      "cut on barcode fraction used in truth matching");
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTEventMonitorTool::initialize()
{
    ATH_MSG_INFO("HTTEventMonitorTool::initialize()");
    ATH_CHECK(HTTMonitorBase::initialize());
    ATH_CHECK(retrieveHistograms());
    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTEventMonitorTool::retrieveHistograms()
{
    if (!m_fast) {
        setHistDir(createDirName("/TruthMatchHist/RoadHist_1st/HTT_1st_vs_Truth/MatchedTrackHist/"));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_Truth_Track_Road_1st_matched_pt", h_Truth_Track_Road_1st_matched_pt));

        if (m_runSecondStage) {
            setHistDir(createDirName("/TruthMatchHist/RoadHist_2nd/HTT_2nd_vs_Truth/MatchedTrackHist/"));
            ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_Truth_Track_Road_2nd_matched_pt", h_Truth_Track_Road_2nd_matched_pt));
        }

        setHistDir(createDirName("/TruthMatchHist/TruthTrackHist/"));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_Truth_Track_Full_pt", h_Truth_Track_Full_pt));

        setHistDir(createDirName("/TruthMatchHist/OfflineTrackHist/MatchedTrackHist/"));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_Truth_Track_Offline_matched_pt", h_Truth_Track_Offline_matched_pt));
    }

    setHistDir(createDirName("/HTTTrackHist_1st/ORTrackHist/TotalTrackHist/"));
    ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_nTracks_1st_afterOR", h_nTracks_1st_afterOR));

    if (!m_fast) {
        setHistDir(createDirName("/TruthMatchHist/TrackHist_1st/HTT_1st_vs_Truth/"));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "All/MatchedTrackHist/h_Truth_Track_HTT_1st_matched_pt", h_Truth_Track_HTT_1st_matched_pt));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "passChi2/MatchedTrackHist/h_Truth_Track_HTT_1st_passChi2_matched_pt", h_Truth_Track_HTT_1st_passChi2_matched_pt));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "afterOR/MatchedTrackHist/h_Truth_Track_HTT_1st_afterOR_matched_pt", h_Truth_Track_HTT_1st_afterOR_matched_pt));
    }

    if (m_runSecondStage) {
        setHistDir(createDirName("/HTTTrackHist_2nd/ORTrackHist/TotalTrackHist/"));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_nTracks_2nd_afterOR", h_nTracks_2nd_afterOR));

        if (!m_fast) {
            setHistDir(createDirName("/TruthMatchHist/TrackHist_2nd/HTT_2nd_vs_Truth/"));
            ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "All/MatchedTrackHist/h_Truth_Track_HTT_2nd_matched_pt", h_Truth_Track_HTT_2nd_matched_pt));
            ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "passChi2/MatchedTrackHist/h_Truth_Track_HTT_2nd_passChi2_matched_pt", h_Truth_Track_HTT_2nd_passChi2_matched_pt));
            ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "afterOR/MatchedTrackHist/h_Truth_Track_HTT_2nd_afterOR_matched_pt", h_Truth_Track_HTT_2nd_afterOR_matched_pt));
        }
    }

    setHistDir(createDirName("/HTTHitHist_1st/AllLayerHist/"));
    ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_nHits_1st", h_nHits_1st));
    ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_nLayers_hit_1st", h_nLayers_hit_1st));

    if (m_runSecondStage) {
        setHistDir(createDirName("/HTTHitHist_2nd/AllLayerHist/"));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_nHits_2nd", h_nHits_2nd));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_nLayers_hit_2nd", h_nLayers_hit_2nd));
    }

    setHistDir(createDirName("/HTTRoadHist_1st/AllRoadHist/"));
    ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_nRoads_1st", h_nRoads_1st));
    if (!m_withPU) ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_nRoads_1st_uHits", h_nRoads_1st_uHits));
    ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_nRoads_1st_matched", h_nRoads_1st_matched));
    ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_nRoads_1st_dupeBarcode", h_nRoads_1st_dupeBarcode));
    ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_nRoads_1st_fakeBarcode", h_nRoads_1st_fakeBarcode));
    ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_nRoadHitCombos_1st", h_nRoadHitCombos_1st));

    setHistDir(createDirName("/HTTRoadHist_1st/PerRoadHist/"));
    ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_nHits_road_1st", h_nHits_road_1st));
    ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_nLayers_road_1st", h_nLayers_road_1st));
    ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_hitLayers_road_1st", h_hitLayers_road_1st));
    ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_wcLayers_road_1st", h_wcLayers_road_1st));
    ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_matchedLayers_road_1st", h_matchedLayers_road_1st));

    if (m_runSecondStage) {
        setHistDir(createDirName("/HTTRoadHist_2nd/AllRoadHist/"));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_nRoads_2nd", h_nRoads_2nd));
        if (!m_withPU) ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_nRoads_2nd_uHits", h_nRoads_2nd_uHits));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_nRoads_2nd_matched", h_nRoads_2nd_matched));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_nRoads_2nd_dupeBarcode", h_nRoads_2nd_dupeBarcode));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_nRoads_2nd_fakeBarcode", h_nRoads_2nd_fakeBarcode));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_nRoadHitCombos_2nd", h_nRoadHitCombos_2nd));

        setHistDir(createDirName("/HTTRoadHist_2nd/PerRoadHist/"));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_nHits_road_2nd", h_nHits_road_2nd));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_nLayers_road_2nd", h_nLayers_road_2nd));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_hitLayers_road_2nd", h_hitLayers_road_2nd));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_wcLayers_road_2nd", h_wcLayers_road_2nd));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_matchedLayers_road_2nd", h_matchedLayers_road_2nd));
    }

    setHistDir(createDirName("/HTTTrackHist_1st/"));
    ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "AllTrackHist/TotalTrackHist/h_nTracks_1st", h_nTracks_1st));
    ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "GoodChi2TrackHist/TotalTrackHist/h_nTracks_1st_passChi2", h_nTracks_1st_passChi2));
    ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "AllTrackHist/TotalTrackHist/h_nTracks_1st_per_patt", h_nTracks_1st_per_patt));
    ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "GoodChi2TrackHist/TotalTrackHist/h_nTracks_1st_per_patt_passChi2", h_nTracks_1st_per_patt_passChi2));
    ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "ORTrackHist/TotalTrackHist/h_nTracks_1st_per_patt_afterOR", h_nTracks_1st_per_patt_afterOR));
    ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "AllTrackHist/TotalTrackHist/h_nConstants_1st", h_nConstants_1st));
    ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "GoodChi2TrackHist/TotalTrackHist/h_nConstants_1st_passChi2", h_nConstants_1st_passChi2));
    ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "ORTrackHist/TotalTrackHist/h_nConstants_1st_afterOR", h_nConstants_1st_afterOR));
    ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "AllTrackHist/TotalTrackHist/h_Track_1st_pt", h_Track_1st_pt));
    ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "AllTrackHist/TotalTrackHist/h_Track_1st_eta", h_Track_1st_eta));
    ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "AllTrackHist/TotalTrackHist/h_Track_1st_phi", h_Track_1st_phi));
    ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "AllTrackHist/TotalTrackHist/h_Track_1st_d0", h_Track_1st_d0));
    ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "AllTrackHist/TotalTrackHist/h_Track_1st_z0", h_Track_1st_z0));
    ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "AllTrackHist/TotalTrackHist/h_Track_1st_chi2", h_Track_1st_chi2));
    ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "AllTrackHist/TotalTrackHist/h_Track_1st_chi2ndof", h_Track_1st_chi2ndof));
    ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "AllTrackHist/TotalTrackHist/h_Track_1st_nmissing", h_Track_1st_nmissing));

    if (m_runSecondStage) {
        setHistDir(createDirName("/HTTTrackHist_2nd/"));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "AllTrackHist/TotalTrackHist/h_nTracks_2nd", h_nTracks_2nd));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "GoodChi2TrackHist/TotalTrackHist/h_nTracks_2nd_passChi2", h_nTracks_2nd_passChi2));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "AllTrackHist/TotalTrackHist/h_nTracks_2nd_per_patt", h_nTracks_2nd_per_patt));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "GoodChi2TrackHist/TotalTrackHist/h_nTracks_2nd_per_patt_passChi2", h_nTracks_2nd_per_patt_passChi2));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "ORTrackHist/TotalTrackHist/h_nTracks_2nd_per_patt_afterOR", h_nTracks_2nd_per_patt_afterOR));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "AllTrackHist/TotalTrackHist/h_nConstants_2nd", h_nConstants_2nd));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "GoodChi2TrackHist/TotalTrackHist/h_nConstants_2nd_passChi2", h_nConstants_2nd_passChi2));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "ORTrackHist/TotalTrackHist/h_nConstants_2nd_afterOR", h_nConstants_2nd_afterOR));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "AllTrackHist/TotalTrackHist/h_Track_2nd_pt", h_Track_2nd_pt));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "AllTrackHist/TotalTrackHist/h_Track_2nd_eta", h_Track_2nd_eta));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "AllTrackHist/TotalTrackHist/h_Track_2nd_phi", h_Track_2nd_phi));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "AllTrackHist/TotalTrackHist/h_Track_2nd_d0", h_Track_2nd_d0));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "AllTrackHist/TotalTrackHist/h_Track_2nd_z0", h_Track_2nd_z0));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "AllTrackHist/TotalTrackHist/h_Track_2nd_chi2", h_Track_2nd_chi2));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "AllTrackHist/TotalTrackHist/h_Track_2nd_chi2ndof", h_Track_2nd_chi2ndof));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "AllTrackHist/TotalTrackHist/h_Track_2nd_nmissing", h_Track_2nd_nmissing));
    }

    if (!m_fast) {
        setHistDir(createDirName("/TruthMatchHist/TruthTrackHist/"));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_nTruth_Tracks_Full", h_nTruth_Tracks_Full));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_Truth_Track_Full_eta", h_Truth_Track_Full_eta));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_Truth_Track_Full_phi", h_Truth_Track_Full_phi));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_Truth_Track_Full_d0", h_Truth_Track_Full_d0));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_Truth_Track_Full_z0", h_Truth_Track_Full_z0));
    }

    if (!m_fast) {
        setHistDir("/HTTOfflineClusterHist/AllLayerHist/");
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_nClustersOff", h_nClustersOff));
    }

    if (!m_fast && !m_outputMonitor && m_clustering) {
        setHistDir(createDirName("/HTTClusterHist_1st/AllLayerHist/"));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_nClusters_1st", h_nClusters_1st));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_Cluster_1st_eta", h_Cluster_1st_eta));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_Cluster_1st_phi", h_Cluster_1st_phi));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_Cluster_1st_etaWidth", h_Cluster_1st_etaWidth));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_Cluster_1st_phiWidth", h_Cluster_1st_phiWidth));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_Cluster_1st_barrelEC", h_Cluster_1st_barrelEC));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_Cluster_1st_layer", h_Cluster_1st_layer));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_Cluster_1st_nSubregions", h_Cluster_1st_nSubregions));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_nClusters_1st_subregion", h_nClusters_1st_subregion));

        setHistDir(createDirName("/HTTClusterHist_1st/PerLayerHist/"));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_nClusters_1st_layer", h_nClusters_1st_layer));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_nClusters_1st_PerSubregion_layer", h_nClusters_1st_PerSubregion_layer));

        if (m_runSecondStage) {
            setHistDir(createDirName("/HTTClusterHist_2nd/AllLayerHist/"));
            ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_nClusters_2nd", h_nClusters_2nd));
            ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_Cluster_2nd_eta", h_Cluster_2nd_eta));
            ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_Cluster_2nd_phi", h_Cluster_2nd_phi));
            ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_Cluster_2nd_etaWidth", h_Cluster_2nd_etaWidth));
            ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_Cluster_2nd_phiWidth", h_Cluster_2nd_phiWidth));
            ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_Cluster_2nd_barrelEC", h_Cluster_2nd_barrelEC));
            ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_Cluster_2nd_layer", h_Cluster_2nd_layer));
            ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_Cluster_2nd_nSubregions", h_Cluster_2nd_nSubregions));
            ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_nClusters_2nd_subregion", h_nClusters_2nd_subregion));

            setHistDir(createDirName("/HTTClusterHist_2nd/PerLayerHist/"));
            ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_nClusters_2nd_layer", h_nClusters_2nd_layer));
            ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_nClusters_2nd_PerSubregion_layer", h_nClusters_2nd_PerSubregion_layer));
        }
    }

    if (!m_fast && !m_outputMonitor && m_doSpacepoints) {
        setHistDir(createDirName("/HTTSpacepointHist_1st/"));
        ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_nSpacepoints_1st", h_nSpacepoints_1st));
        if (m_runSecondStage) {
            setHistDir(createDirName("/HTTSpacepointHist_2nd/"));
            ATH_CHECK(m_tHistSvc->getHist(getHistDir() + "h_nSpacepoints_2nd", h_nSpacepoints_2nd));
        }
    }
    
    clearHistDir();

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTEventMonitorTool::fillHistograms()
{
    // Use event selection to pass or reject event based on truth info, sample type etc
    ATH_CHECK(selectEvent());

    m_nEvents++;

    calculateStatistics();
    clearMonitorData();

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTEventMonitorTool::printHistograms(PrintDetail detail) const
{
    // Summary
    if (detail >= SUMMARY)
    {
        ATH_MSG_INFO("-----------------------------------------------------------------------");
        ATH_MSG_INFO("Summary Statistics:");
        ATH_MSG_INFO("\tEvents with first stage roads=" << m_nEvents_with_road_1st << "/" << m_nEvents
                << ", miss=" << 100.0 * (m_nEvents - m_nEvents_with_road_1st) / m_nEvents << "%");
        if (!m_fast)
        {
            ATH_MSG_INFO("\tFirst stage road matched to truth eff=" << m_nTruthTracks_with_road_1st << "/" << m_nTruthTracks
                    << ", miss=" << 100.0 * (m_nTruthTracks - m_nTruthTracks_with_road_1st) / m_nTruthTracks << "%");
            ATH_MSG_INFO("\t\t[1-2 GeV]=" << 100. * h_Truth_Track_Road_1st_matched_pt->Integral(1, 1) / h_Truth_Track_Full_pt->Integral(1, 1)
                       << "% [2-4 Gev]=" << 100. * h_Truth_Track_Road_1st_matched_pt->Integral(2, 3) / h_Truth_Track_Full_pt->Integral(2, 3)
                       << "% [4+ GeV]="  << 100. * h_Truth_Track_Road_1st_matched_pt->Integral(4, -1) / h_Truth_Track_Full_pt->Integral(4, -1) << "%");
        }
        ATH_MSG_INFO("\tFirst stage road Ave=" << (double)m_nRoads_1st / m_nEvents << ", exclude zero=" << (double)m_nRoads_1st / m_nEvents_with_road_1st);
        ATH_MSG_INFO("\tFirst stage road hit combinations Ave=" << (double)m_nCombos_1st / m_nEvents << ", exclude zero=" << (double)m_nCombos_1st / m_nEvents_with_road_1st);

        if (m_runSecondStage) {
            ATH_MSG_INFO(" ");

            ATH_MSG_INFO("\tEvents with second stage roads=" << m_nEvents_with_road_2nd << "/" << m_nEvents
                    << ", miss=" << 100.0 * (m_nEvents - m_nEvents_with_road_2nd) / m_nEvents << "%");
            if (!m_fast)
            {
                ATH_MSG_INFO("\tSecond stage road matched to truth eff=" << m_nTruthTracks_with_road_2nd << "/" << m_nTruthTracks
                        << ", miss=" << 100.0 * (m_nTruthTracks - m_nTruthTracks_with_road_2nd) / m_nTruthTracks << "%");
                ATH_MSG_INFO("\t\t[1-2 GeV]=" << 100. * h_Truth_Track_Road_2nd_matched_pt->Integral(1, 1) / h_Truth_Track_Full_pt->Integral(1, 1)
                           << "% [2-4 Gev]=" << 100. * h_Truth_Track_Road_2nd_matched_pt->Integral(2, 3) / h_Truth_Track_Full_pt->Integral(2, 3)
                           << "% [4+ GeV]="  << 100. * h_Truth_Track_Road_2nd_matched_pt->Integral(4, -1) / h_Truth_Track_Full_pt->Integral(4, -1) << "%");
            }
            ATH_MSG_INFO("\tSecond stage road Ave=" << (double)m_nRoads_2nd / m_nEvents << ", exclude zero=" << (double)m_nRoads_2nd / m_nEvents_with_road_2nd);
            ATH_MSG_INFO("\tSecond stage road hit combinations Ave=" << (double)m_nCombos_2nd / m_nEvents << ", exclude zero=" << (double)m_nCombos_2nd / m_nEvents_with_road_2nd);
        }

        ATH_MSG_INFO(" ");

        ATH_MSG_INFO("\tFirst Stage Tracks Final Ave=" << h_nTracks_1st_afterOR->GetMean()
                   << ", Events=" << m_nEvents - h_nTracks_1st_afterOR->GetBinContent(1) << "/" << m_nEvents
                   << "(" << 100 * (1 - (double)(h_nTracks_1st_afterOR->GetBinContent(1)) / h_nTracks_1st_afterOR->GetEntries()) << "%)");
        if (!m_fast)
        {
            unsigned n_all = h_Truth_Track_HTT_1st_matched_pt->GetEntries();
            unsigned n_chi = h_Truth_Track_HTT_1st_passChi2_matched_pt->GetEntries();
            unsigned n_htt = h_Truth_Track_HTT_1st_afterOR_matched_pt->GetEntries();
            unsigned n_off = h_Truth_Track_Offline_matched_pt->GetEntries();
            ATH_MSG_INFO("\tFirst stage track matched to truth: all=" << n_all << ", passChi2=" << n_chi << ", final=" << n_htt << "/" << m_nTruthTracks
                    << ", miss=" << 100.0 * (m_nTruthTracks - n_htt) / m_nTruthTracks << "%");
            ATH_MSG_INFO("\twrt offline=" << n_htt << "/" << n_off << "(" << 100.0*n_htt/n_off << "%)");
        }

        if (m_runSecondStage) {
            ATH_MSG_INFO(" ");

            ATH_MSG_INFO("\tSecond Stage Tracks Final Ave=" << h_nTracks_2nd_afterOR->GetMean()
                       << ", Events=" << m_nEvents - h_nTracks_2nd_afterOR->GetBinContent(1) << "/" << m_nEvents
                       << "(" << 100 * (1 - (double)(h_nTracks_2nd_afterOR->GetBinContent(1)) / h_nTracks_2nd_afterOR->GetEntries()) << "%)");
            if (!m_fast)
            {
                unsigned n_all_2nd = h_Truth_Track_HTT_2nd_matched_pt->GetEntries();
                unsigned n_chi_2nd = h_Truth_Track_HTT_2nd_passChi2_matched_pt->GetEntries();
                unsigned n_htt_2nd = h_Truth_Track_HTT_2nd_afterOR_matched_pt->GetEntries();
                unsigned n_off   = h_Truth_Track_Offline_matched_pt->GetEntries();
                ATH_MSG_INFO("\tSecond stage track matched to truth: all=" << n_all_2nd << ", passChi2=" << n_chi_2nd << ", final=" << n_htt_2nd << "/" << m_nTruthTracks
                        << ", miss=" << 100.0 * (m_nTruthTracks - n_htt_2nd) / m_nTruthTracks << "%");
                ATH_MSG_INFO("\twrt offline=" << n_htt_2nd << "/" << n_off << "(" << 100.0*n_htt_2nd/n_off << "%)");
            }
        }

        ATH_MSG_INFO("-----------------------------------------------------------------------");
    }

    ATH_MSG_INFO(" ");

    ATH_MSG_INFO("----------------------------------FIRST STAGE HITS----------------------------------");
    if (detail >= CONCISE)  printHist(h_nHits_1st);
    if (detail >= CONCISE)  printHist(h_nLayers_hit_1st);

    if (m_runSecondStage) {
        ATH_MSG_INFO("----------------------------------SECOND STAGE HITS----------------------------------");
        if (detail >= CONCISE)  printHist(h_nHits_2nd);
        if (detail >= CONCISE)  printHist(h_nLayers_hit_2nd);
    }

    ATH_MSG_INFO("---------------------------------FIRST STAGE ROADS----------------------------------");
    if (detail >= CONCISE)  printHist(h_nRoads_1st);
    if (detail >= CONCISE && !m_withPU)  printHist(h_nRoads_1st_uHits);
    if (detail >= CONCISE)  printHist(h_nRoads_1st_matched);
    if (detail >= ALL)      printHist(h_nRoads_1st_dupeBarcode);
    if (detail >= ALL)      printHist(h_nRoads_1st_fakeBarcode);
    if (detail >= CONCISE)  printHist(h_nRoadHitCombos_1st);

    if (!m_fast)
    {
        if (detail >= CONCISE)  printHist(h_nHits_road_1st);
        if (detail >= CONCISE)  printHist(h_nLayers_road_1st);
        if (detail >= CONCISE)  printHist(h_hitLayers_road_1st);
        if (detail >= ALL)      printHist(h_wcLayers_road_1st);
        if (detail >= ALL)      printHist(h_matchedLayers_road_1st);
    }

    if (m_runSecondStage) {
        ATH_MSG_INFO("---------------------------------SECOND STAGE ROADS----------------------------------");
        if (detail >= CONCISE)  printHist(h_nRoads_2nd);
        if (detail >= CONCISE && !m_withPU)  printHist(h_nRoads_2nd_uHits);
        if (detail >= CONCISE)  printHist(h_nRoads_2nd_matched);
        if (detail >= ALL)      printHist(h_nRoads_2nd_dupeBarcode);
        if (detail >= ALL)      printHist(h_nRoads_2nd_fakeBarcode);
        if (detail >= CONCISE)  printHist(h_nRoadHitCombos_2nd);

        if (!m_fast)
        {
            if (detail >= CONCISE)  printHist(h_nHits_road_2nd);
            if (detail >= CONCISE)  printHist(h_nLayers_road_2nd);
            if (detail >= CONCISE)  printHist(h_hitLayers_road_2nd);
            if (detail >= ALL)      printHist(h_wcLayers_road_2nd);
            if (detail >= ALL)      printHist(h_matchedLayers_road_2nd);
        }
    }

    ATH_MSG_INFO("-------------------------------FIRST STAGE HTT TRACKS-------------------------------");
    if (detail >= CONCISE)  printHist(h_nTracks_1st);
    if (detail >= CONCISE)  printHist(h_nTracks_1st_passChi2);
    if (detail >= CONCISE)  printHist(h_nTracks_1st_afterOR);
    if (detail >= ALL)      printHist(h_nTracks_1st_per_patt);
    if (detail >= ALL)      printHist(h_nTracks_1st_per_patt_passChi2);
    if (detail >= ALL)      printHist(h_nTracks_1st_per_patt_afterOR);
    if (detail >= ALL)      printHist(h_nConstants_1st);
    if (detail >= ALL)      printHist(h_nConstants_1st_passChi2);
    if (detail >= ALL)      printHist(h_nConstants_1st_afterOR);
    if (detail >= ALL)      printHist(h_Track_1st_pt);
    if (detail >= ALL)      printHist(h_Track_1st_eta);
    if (detail >= ALL)      printHist(h_Track_1st_phi);
    if (detail >= ALL)      printHist(h_Track_1st_d0);
    if (detail >= ALL)      printHist(h_Track_1st_z0);
    if (detail >= CONCISE)  printHist(h_Track_1st_chi2);
    if (detail >= CONCISE)  printHist(h_Track_1st_chi2ndof);
    if (detail >= CONCISE)  printHist(h_Track_1st_nmissing);

    if (m_runSecondStage) {
        ATH_MSG_INFO("-------------------------------SECOND STAGE HTT TRACKS-------------------------------");
        if (detail >= CONCISE)  printHist(h_nTracks_2nd);
        if (detail >= CONCISE)  printHist(h_nTracks_2nd_passChi2);
        if (detail >= CONCISE)  printHist(h_nTracks_2nd_afterOR);
        if (detail >= ALL)      printHist(h_nTracks_2nd_per_patt);
        if (detail >= ALL)      printHist(h_nTracks_2nd_per_patt_passChi2);
        if (detail >= ALL)      printHist(h_nTracks_2nd_per_patt_afterOR);
        if (detail >= ALL)      printHist(h_nConstants_2nd);
        if (detail >= ALL)      printHist(h_nConstants_2nd_passChi2);
        if (detail >= ALL)      printHist(h_nConstants_2nd_afterOR);
        if (detail >= ALL)      printHist(h_Track_2nd_pt);
        if (detail >= ALL)      printHist(h_Track_2nd_eta);
        if (detail >= ALL)      printHist(h_Track_2nd_phi);
        if (detail >= ALL)      printHist(h_Track_2nd_d0);
        if (detail >= ALL)      printHist(h_Track_2nd_z0);
        if (detail >= CONCISE)  printHist(h_Track_2nd_chi2);
        if (detail >= CONCISE)  printHist(h_Track_2nd_chi2ndof);
        if (detail >= CONCISE)  printHist(h_Track_2nd_nmissing);
    }

    if (!m_fast)
    {
        ATH_MSG_INFO("------------------------------TRUTH TRACKS------------------------------");
        if (detail >= ALL) printHist(h_nTruth_Tracks_Full);
        if (detail >= ALL) printHist(h_Truth_Track_Full_pt);
        if (detail >= ALL) printHist(h_Truth_Track_Full_eta);
        if (detail >= ALL) printHist(h_Truth_Track_Full_phi);
        if (detail >= ALL) printHist(h_Truth_Track_Full_d0);
        if (detail >= ALL) printHist(h_Truth_Track_Full_z0);
    }

    if (!m_fast) {
        ATH_MSG_INFO("--------------------------------OFFLINE CLUSTERS--------------------------------");
        if (detail >= CONCISE)  printHist(h_nClustersOff);
    }

    if (!m_fast && !m_outputMonitor && m_clustering)
    {
        ATH_MSG_INFO("--------------------------------FIRST STAGE CLUSTERS--------------------------------");
        if (detail >= CONCISE)  printHist(h_nClusters_1st);
        if (detail >= ALL)      printHist(h_Cluster_1st_eta);
        if (detail >= ALL)      printHist(h_Cluster_1st_phi);
        if (detail >= ALL)      printHist(h_Cluster_1st_etaWidth);
        if (detail >= ALL)      printHist(h_Cluster_1st_phiWidth);
        if (detail >= ALL)      printHist(h_Cluster_1st_barrelEC);
        if (detail >= CONCISE)  printHist(h_Cluster_1st_layer);
        if (detail >= ALL)      printHist(h_Cluster_1st_nSubregions);
        if (detail >= ALL)      printHist(h_nClusters_1st_subregion, true);
        if (detail >= ALL)      printHist(h_nClusters_1st_layer, true);
        if (detail >= ALL)      printHist(h_nClusters_1st_PerSubregion_layer, true);
    }

    if (!m_fast && !m_outputMonitor && m_clustering && m_runSecondStage)
    {
        ATH_MSG_INFO("--------------------------------SECOND STAGE CLUSTERS--------------------------------");
        if (detail >= CONCISE)  printHist(h_nClusters_2nd);
        if (detail >= ALL)      printHist(h_Cluster_2nd_eta);
        if (detail >= ALL)      printHist(h_Cluster_2nd_phi);
        if (detail >= ALL)      printHist(h_Cluster_2nd_etaWidth);
        if (detail >= ALL)      printHist(h_Cluster_2nd_phiWidth);
        if (detail >= ALL)      printHist(h_Cluster_2nd_barrelEC);
        if (detail >= CONCISE)  printHist(h_Cluster_2nd_layer);
        if (detail >= ALL)      printHist(h_Cluster_2nd_nSubregions);
        if (detail >= ALL)      printHist(h_nClusters_2nd_subregion, true);
        if (detail >= ALL)      printHist(h_nClusters_2nd_layer, true);
        if (detail >= ALL)      printHist(h_nClusters_2nd_PerSubregion_layer, true);
    }

    if (!m_fast && !m_outputMonitor && m_doSpacepoints)
    {
        ATH_MSG_INFO("------------------------------SPACEPOINTS-------------------------------");
        if (detail >= CONCISE)  printHist(h_nSpacepoints_1st);
        if (m_runSecondStage) {
            if (detail >= CONCISE)  printHist(h_nSpacepoints_2nd);
        }
    }

    // TODO every other histogram X(. Add as needed I guess

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTEventMonitorTool::finalize()
{
    ATH_MSG_INFO("HTTEventMonitorTool::finalize()");
    ATH_CHECK(HTTMonitorBase::finalize());
    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void HTTEventMonitorTool::calculateStatistics()
{
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

    // Create a lookup on the barcodes of all the roads, and also max frac
    //      value.first = roads with matching barcode, and fraction above m_cut_barcodefrac
    //      value.second = maximum barcode fraction among all roads for this barcode
    std::map<typename HTTMultiTruth::Barcode, std::pair<std::vector<HTTRoad const *>, float>> m_1st, m_2nd;

    if (!roads_1st->empty()) m_nEvents_with_road_1st++;
    m_nRoads_1st += roads_1st->size();
    for (HTTRoad const * r : *roads_1st) m_nCombos_1st += r->getNHitCombos();
    calculateTruth(roads_1st, m_1st);

    if (m_runSecondStage) {
        if (!roads_2nd->empty()) m_nEvents_with_road_2nd++;
        m_nRoads_2nd += roads_2nd->size();
        for (HTTRoad const * r : *roads_2nd) m_nCombos_2nd += r->getNHitCombos();
        calculateTruth(roads_2nd, m_2nd);
    }

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
        m_nTruthTracks++;

        auto it_1st = m_1st.find({ 0, t.getBarcode() });
        if (it_1st != m_1st.end() && !it_1st->second.first.empty()) {
            m_nTruthTracks_with_road_1st++; // matched with roads
        }

        if (m_runSecondStage) {
            auto it_2nd = m_2nd.find({ 0, t.getBarcode() });
            if (it_2nd != m_2nd.end() && !it_2nd->second.first.empty()) {
                m_nTruthTracks_with_road_2nd++; // matched with roads
            }
        }
    }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void HTTEventMonitorTool::calculateTruth(std::vector<HTTRoad*> const * roads, std::map<typename HTTMultiTruth::Barcode, std::pair<std::vector<HTTRoad const *>, float>> & m)
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


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void HTTEventMonitorTool::printHist(TH1* h, bool profile) const
{
    if (!profile)
        ATH_MSG_INFO(h->GetTitle() << ": " << printVerbose(h));
    else
        ATH_MSG_INFO(h->GetTitle() << ": " << printProfile(h));
}

