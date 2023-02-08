/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTTHITFILTERINGTOOL_H
#define HTTHITFILTERINGTOOL_H

/*
 * httHitFiltering
 * ---------------
 *
 * Routines to filter hits/clusters, based on TrigHTTSim
 *
 */

#include "AthenaBaseComps/AthAlgTool.h"

#include "TrigHTTObjects/HTTLogicalEventInputHeader.h"
#include "TrigHTTObjects/HTTCluster.h"
#include "TrigHTTObjects/HTTHit.h"

#include "TrigHTTMaps/HTTHitFilteringToolI.h"

#include "TRandom3.h"
#include <unordered_map>


class ITrigHTTMappingSvc;

class HTTHitFilteringTool : public extends<AthAlgTool, HTTHitFilteringToolI> {
 public:
    HTTHitFilteringTool(const std::string &, const std::string &, const IInterface *);

    virtual ~HTTHitFilteringTool() { ; }

    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    virtual StatusCode DoRandomRemoval(HTTLogicalEventInputHeader &, bool) override;
    virtual StatusCode GetPairedStripPhysLayers(const HTTPlaneMap*, std::vector<int> &) override;
    virtual StatusCode DoHitFiltering(HTTLogicalEventInputHeader &,
                                      std::vector<int>, std::vector<int>,
                                      std::vector<HTTCluster> &) override;

 private:
    void FilterHits(std::vector<HTTHit> &, std::vector<int> &, std::vector<int> &, std::vector<HTTHit> &);

    bool check_hit_stub(HTTHit, HTTHit, float, float);
    void fill_cut_values(HTTHit, float &, float &);

    // configuration
    BooleanProperty m_doRandomRemoval;
    float m_rndPixelHitRmFrac;
    float m_rndStripHitRmFrac;
    float m_rndPixelClustRmFrac;
    float m_rndStripClustRmFrac;

    BooleanProperty m_doStubs;
    std::string m_stubCutsFile;
    float m_barrelStubDphiCut;
    float m_endcapStubDphiCut;
    BooleanProperty m_useNstrips;

    TRandom3 m_random;
    std::unordered_map<std::string, std::unordered_map<int, std::unordered_map<int, std::pair<float,float>>>> m_stubCutMap;

};

#endif // HTTHITFILTERINGTOOL_H
