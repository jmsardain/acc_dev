/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTTSPACEPOINTSTOOLV2_H
#define HTTSPACEPOINTSTOOLV2_H

#include <array>
#include <vector>
#include <map>

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigHTTObjects/HTTCluster.h"
#include "TrigHTTObjects/HTTHit.h"
#include "TrigHTTMaps/HTTSpacePointsToolI.h"

class TH1I;

class HTTSpacePointsTool_v2 : public extends<AthAlgTool, HTTSpacePointsToolI> {
 public:
    HTTSpacePointsTool_v2(const std::string &, const std::string &, const IInterface *);
    virtual ~HTTSpacePointsTool_v2() {}

    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    virtual StatusCode DoSpacePoints(HTTLogicalEventInputHeader &, std::vector<HTTCluster> &) override;

 private:
    StatusCode fillMaps(std::vector<HTTHit>& hits);
    StatusCode makeSpacePoints(HTTTowerInputHeader &tower, std::vector<HTTCluster> &spacepoints);
    void calcPosition(HTTHit &hit_in, HTTHit &hit_out, float &x, float &y, float &z);
    bool searchForMatch(HTTHit& hit_in,std::vector<HTTHit>& hits_outer,HTTTowerInputHeader &tower, std::vector<HTTCluster> &spacepoints);
    void addSpacePoints(HTTHit& hit_in, HTTHit& hit_out ,HTTTowerInputHeader &tower, std::vector<HTTCluster> &spacepoints);

    //----------------------
    // Working Memory
    std::map<std::vector<int>,std::pair<std::vector<HTTHit>,std::vector<HTTHit>>> m_map;
    std::vector<HTTHit> m_pixel;

    FloatProperty m_phiwindow = 0.008; // radians

    BooleanProperty m_duplicate = false;
    BooleanProperty m_filter = false;
    BooleanProperty m_filterClose = false;

    // self monitoring
    unsigned m_inputhits = 0;
    unsigned m_spacepts = 0;
    unsigned m_filteredhits = 0;
    unsigned m_diffmodule = 0;
    TH1I*    m_spacepts_per_hit;

};

#endif // HTTSPACEPOINTSTOOLV2_H
