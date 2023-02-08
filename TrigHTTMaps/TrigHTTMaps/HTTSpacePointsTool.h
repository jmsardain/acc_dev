/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTTSPACEPOINTSTOOL_H
#define HTTSPACEPOINTSTOOL_H

#include <array>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigHTTObjects/HTTCluster.h"
#include "TrigHTTObjects/HTTHit.h"
#include "TrigHTTMaps/HTTSpacePointsToolI.h"

class HTTSpacePointsTool : public extends<AthAlgTool, HTTSpacePointsToolI> {
 public:
    HTTSpacePointsTool(const std::string &, const std::string &, const IInterface *);
    virtual ~HTTSpacePointsTool() {}

    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    virtual StatusCode DoSpacePoints(HTTLogicalEventInputHeader &, std::vector<HTTCluster> &) override;

 private:
    void SpacePointFinder(const std::vector<HTTHit> &, std::vector<std::array<int, 2>> &);

    BooleanProperty m_duplicate = false;
    BooleanProperty m_filter = false;
    BooleanProperty m_filterClose = false;
};

#endif // HTTSPACEPOINTSTOOL_H
