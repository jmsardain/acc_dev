/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTTCLUSTERMONITORTOOL_H
#define HTTCLUSTERMONITORTOOL_H

#include "TrigHTTMonitor/HTTMonitorBase.h"

/////////////////////////////////////////////////////////////////////////////
class HTTClusterMonitorTool: public HTTMonitorBase
{
public:

  HTTClusterMonitorTool (std::string const & algname, std::string const & name, IInterface const * ifc);
  ~HTTClusterMonitorTool() {}

  static const InterfaceID& interfaceID();

  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms() override;
  virtual StatusCode finalize() override;

protected:

  virtual StatusCode bookHistograms() override;

private:

  // Book histograms
  StatusCode bookHTTOfflineClusterHistograms();
  StatusCode bookHTTClusterHistograms_1st();
  StatusCode bookHTTClusterHistograms_2nd();
  StatusCode bookHTTSpacepointHistograms_1st();
  StatusCode bookHTTSpacepointHistograms_2nd();

  // Fill histograms
  void fillHTTOfflineClusterHistograms();
  void fillHTTClusterHistograms_1st(std::vector<HTTCluster> const * clusters_1st);
  void fillHTTClusterHistograms_2nd(std::vector<HTTCluster> const * clusters_2nd);
  void fillHTTSpacepointHistograms_1st(std::vector<HTTCluster> const * spacepoints_1st);
  void fillHTTSpacepointHistograms_2nd(std::vector<HTTCluster> const * spacepoints_2nd);

  // usefull tool
  int getUniqueLayerId(HTTHit const & hit);

  // Flags
  BooleanProperty m_clustering = false;
  BooleanProperty m_doSpacepoints = false;
  BooleanProperty m_runSecondStage = false;

  //====================
  // Histograms
  //====================

  // Offline Cluster Histograms
  TH1I*   h_nClustersOff;
  TH2I*   h_ClusterOff_coord;

  TH2F*   h_nClustersOff_vs_truth_pt;
  TH2F*   h_nClustersOff_vs_truth_eta;
  TH2F*   h_nClustersOff_vs_truth_phi;
  TH2F*   h_nClustersOff_vs_truth_d0;
  TH2F*   h_nClustersOff_vs_truth_z0;

  std::vector<TH2I*>  h_ClusterOff_coord_layer;

  // First Stage Cluster Histograms
  TH1I*   h_nClusters_1st;
  TH1I*   h_Cluster_1st_eta;
  TH1I*   h_Cluster_1st_phi;
  TH1I*   h_Cluster_1st_etaWidth;
  TH1I*   h_Cluster_1st_phiWidth;
  TH1I*   h_Cluster_1st_barrelEC;
  TH1I*   h_Cluster_1st_layer;
  TH1I*   h_Cluster_1st_physLayer;
  TH2I*   h_Cluster_1st_coord;
  TH1I*   h_Cluster_1st_nSubregions;
  TH2I*   h_nClusters_1st_subregion;            // x: subregion, y: nclusters

  TH2F*   h_nClusters_1st_vs_truth_pt;
  TH2F*   h_nClusters_1st_vs_truth_eta;
  TH2F*   h_nClusters_1st_vs_truth_phi;
  TH2F*   h_nClusters_1st_vs_truth_d0;
  TH2F*   h_nClusters_1st_vs_truth_z0;

  TH2I*   h_nClusters_1st_layer;                // x: layer,     y: nclusters
  TH2I*   h_nClusters_1st_PerSubregion_layer;   // x: layer,     y: nclusters
  TH2I*   h_Cluster_1st_etaWidth_layer;         // x: layer,     y: eta width
  TH2I*   h_Cluster_1st_phiWidth_layer;         // x: layer,     y: phi width

  std::vector<TH2I*>  h_Cluster_1st_coord_layer;

  // Second Stage Cluster Histograms
  TH1I*   h_nClusters_2nd;
  TH1I*   h_Cluster_2nd_eta;
  TH1I*   h_Cluster_2nd_phi;
  TH1I*   h_Cluster_2nd_etaWidth;
  TH1I*   h_Cluster_2nd_phiWidth;
  TH1I*   h_Cluster_2nd_barrelEC;
  TH1I*   h_Cluster_2nd_layer;
  TH1I*   h_Cluster_2nd_physLayer;
  TH2I*   h_Cluster_2nd_coord;
  TH1I*   h_Cluster_2nd_nSubregions;
  TH2I*   h_nClusters_2nd_subregion;            // x: subregion, y: nclusters

  TH2F*   h_nClusters_2nd_vs_truth_pt;
  TH2F*   h_nClusters_2nd_vs_truth_eta;
  TH2F*   h_nClusters_2nd_vs_truth_phi;
  TH2F*   h_nClusters_2nd_vs_truth_d0;
  TH2F*   h_nClusters_2nd_vs_truth_z0;

  TH2I*   h_nClusters_2nd_layer;                // x: layer,     y: nclusters
  TH2I*   h_nClusters_2nd_PerSubregion_layer;   // x: layer,     y: nclusters
  TH2I*   h_Cluster_2nd_etaWidth_layer;         // x: layer,     y: eta width
  TH2I*   h_Cluster_2nd_phiWidth_layer;         // x: layer,     y: phi width

  std::vector<TH2I*>  h_Cluster_2nd_coord_layer;

  // First Stage Spacepoint Histograms
  TH1I*   h_nSpacepoints_1st;
  TH1I*   h_Spacepoint_1st_nHits;
  TH1F*   h_Spacepoint_1st_globalResidual;
  TH2F*   h_Spacepoint_1st_angle_layer;
  TH2F*   h_Spacepoint_1st_distance_layer;
  TH2I*   h_Spacepoint_1st_globalXY;
  TH2I*   h_Spacepoint_1st_globalRZ;

  // Second Stage Spacepoint Histograms
  TH1I*   h_nSpacepoints_2nd;
  TH1I*   h_Spacepoint_2nd_nHits;
  TH1F*   h_Spacepoint_2nd_globalResidual;
  TH2F*   h_Spacepoint_2nd_angle_layer;
  TH2F*   h_Spacepoint_2nd_distance_layer;
  TH2I*   h_Spacepoint_2nd_globalXY;
  TH2I*   h_Spacepoint_2nd_globalRZ;
};

#endif // HTTClusterMonitorTool_h
