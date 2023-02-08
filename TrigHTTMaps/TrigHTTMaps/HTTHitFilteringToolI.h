#ifndef HTTHITFILTERINGTOOLI_H
#define HTTHITFILTERINGTOOLI_H

/**
 * @file HTTHitFilteringToolI.h
 * @author Will Kalderon - willam.kalderon@cern.ch, Julian Wollrath - wollrath@cern.ch
 * @date 2021
 * @brief Declares an abstract class that implements an interface for hit/cluster filtering.
 * This class is implemented in
 *      HTTHitFilteringTool.h
 */

#include "GaudiKernel/IAlgTool.h"
#include "TrigHTTObjects/HTTCluster.h"
#include "TrigHTTObjects/HTTLogicalEventInputHeader.h"
#include "TrigHTTMaps/HTTPlaneMap.h"

class HTTHitFilteringToolI : virtual public ::IAlgTool {
 public:
  DeclareInterfaceID(HTTHitFilteringToolI, 1, 0);
  virtual ~HTTHitFilteringToolI() = default;
  
  virtual StatusCode DoRandomRemoval(HTTLogicalEventInputHeader &, bool) = 0;
  virtual StatusCode GetPairedStripPhysLayers(const HTTPlaneMap*, std::vector<int> &) = 0;
  virtual StatusCode DoHitFiltering(HTTLogicalEventInputHeader &,
                                    std::vector<int>, std::vector<int>,
                                    std::vector<HTTCluster> &) = 0;

};

#endif // HTTHITFILTERINGTOOLI_H
