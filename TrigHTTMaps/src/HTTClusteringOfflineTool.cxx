/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTMaps/HTTClusteringOfflineTool.h"

HTTClusteringOfflineTool::HTTClusteringOfflineTool(const std::string& algname, const std::string &name, const IInterface *ifc) :
  base_class(algname, name, ifc)
{
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTClusteringOfflineTool::initialize(){

  ATH_MSG_INFO ("HTTClusteringOfflineTool::initialize()");
  return StatusCode::SUCCESS;

}


StatusCode HTTClusteringOfflineTool::finalize(){
  ATH_MSG_INFO ("HTTClusteringOfflineTool::finalize()");
  return StatusCode::SUCCESS;
}

StatusCode HTTClusteringOfflineTool::DoClustering(HTTLogicalEventInputHeader &header, std::vector<HTTCluster> &clusters) const
{

    clusters = header.optional().getOfflineClusters();
    //fill the multitruth
    for(auto& cluster:clusters){
        HTTHit clusterEquiv = cluster.getClusterEquiv();
        HTTMultiTruth mt;
        HTTMultiTruth::Barcode uniquecode(clusterEquiv.getEventIndex(),clusterEquiv.getBarcode());
        mt.maximize(uniquecode,clusterEquiv.getBarcodePt());
        clusterEquiv.setTruth(mt);
        cluster.setClusterEquiv(clusterEquiv);
    }
    return StatusCode::SUCCESS;
}
