/*
  void attachTruth(std::vector<HTTHit> &);  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTTCLUSTERINGTOOL_H
#define HTTCLUSTERINGTOOL_H

/*
 * httClustering
 * ---------------
 *
 * Routines to perform clustering in the pixels, based on TrigHTTSim
 *
 */

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigHTTMaps/HTTClusteringToolI.h"
#include "TrigHTTUtils/HTTMacros.h"
#include "TrigHTTObjects/HTTHit.h"
#include "TrigHTTObjects/HTTCluster.h"

void attachTruth(std::vector<HTTHit> &);
bool updatePixelCluster(HTTCluster &currentCluster, HTTHit &incomingHit, bool newCluster);
bool updateStripCluster(HTTCluster &currentCluster, HTTHit &incomingHit, bool newCluster);

class HTTClusteringTool : public extends <AthAlgTool,HTTClusteringToolI> {
public:

  HTTClusteringTool(const std::string&, const std::string&, const IInterface*);

  virtual ~HTTClusteringTool() {;}

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  virtual StatusCode DoClustering(HTTLogicalEventInputHeader &, std::vector<HTTCluster> &) const override;

 private:

  //HTT pixel clustering using the HTTSim objects
  void SortedClustering(std::vector<std::vector<HTTHit> > sorted_hits, std::vector<HTTCluster> &) const;
  void Clustering(std::vector<HTTHit>, std::vector<HTTCluster> &) const ;
};

#endif // HTTCLUSTERINGTOOL_H
