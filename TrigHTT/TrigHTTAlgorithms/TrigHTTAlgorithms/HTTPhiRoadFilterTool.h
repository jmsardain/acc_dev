// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#ifndef HTTPHIROADFILTERTOOL_H
#define HTTPHIROADFILTERTOOL_H

/**
 * @file HTTPhiRoadFilterTool.h
 * @author Elliot Lipeles - lipeles@cern.ch
 * @date March 28th, 2021
 * @brief Implements road filtering using eta module patterns
 *
 * Declarations in this file:
 *      class HTTPhiRoadFilterTool : public AthAlgTool, virtual public HTTRoadFilterToolI
 *
 */

#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "TrigHTTUtils/HTTTypes.h"
#include "TrigHTTUtils/HTTVectors.h"
#include "TrigHTTObjects/HTTRoad.h"
#include "TrigHTTObjects/HTTHit.h"
#include "TrigHTTObjects/HTTTrackPars.h"
#include "TrigHTTAlgorithms/HTTRoadFilterToolI.h"

#include "TFile.h"

#include <string>
#include <vector>
#include <map>
#include <boost/dynamic_bitset_fwd.hpp>

class ITrigHTTBankSvc;
class ITrigHTTMappingSvc;


class HTTPhiRoadFilterTool : public AthAlgTool, virtual public HTTRoadFilterToolI
{
    public:

        ///////////////////////////////////////////////////////////////////////
        // AthAlgTool

        HTTPhiRoadFilterTool(const std::string&, const std::string&, const IInterface*);

        virtual StatusCode initialize() override;
        virtual StatusCode finalize() override;

        ///////////////////////////////////////////////////////////////////////
        // HTTRoadFilterToolI

        virtual StatusCode filterRoads(const std::vector<HTTRoad*> & prefilter_roads, std::vector<HTTRoad*> & postfilter_roads) override;

    private:

        ///////////////////////////////////////////////////////////////////////
        // Handles
        ServiceHandle<ITrigHTTMappingSvc> m_HTTMapping;

        ///////////////////////////////////////////////////////////////////////
        // Properties
        UnsignedIntegerProperty m_threshold = 0; // Minimum number of hit layers to fire a road
        FloatArrayProperty m_window; // Distance from nominal track to keep hit, by layer.
        FloatProperty m_ptscaling = 0.0;
  
        ///////////////////////////////////////////////////////////////////////////////
        // Utility Class

        //////////////////////////////////////////////////////////////////////
        // Event Storage
        std::vector<HTTRoad> m_postfilter_roads;
  
        ///////////////////////////////////////////////////////////////////////
        // Convenience
        unsigned m_nLayers; // alias to m_HTTMapping->PlaneMap1stStage()->getNLogiLayers();
  
        ///////////////////////////////////////////////////////////////////////
        // Metadata and Monitoring

        unsigned m_event = 0;
        std::string m_name; // Gets the instance name from the full gaudi name

        ///////////////////////////////////////////////////////////////////////
        // Helpers
        HTTRoad_Hough buildRoad(HTTRoad* origr) const;
};


#endif // HTTPHIROADFILTERTOOL_H
