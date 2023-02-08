/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTTPATTERNMATCHTOOL_H
#define HTTPATTERNMATCHTOOL_H

/**
 * @file HTTPatternMatchTool.h
 * @author Riley Xu - rixu@cern.ch
 * @date October 22nd, 2019
 * @brief Declares a class that does road finding using pattern matching
 */


#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include "TrigHTTObjects/HTTRoad.h"
#include "TrigHTTObjects/HTTHit.h"
#include "TrigHTTUtils/HTTTypes.h"
#include "TrigHTTAlgorithms/HTTRoadFinderToolI.h"

#include <string>
#include <vector>
#include <map>

class ITrigHTTBankSvc;
class ITrigHTTMappingSvc;
class HTTPatternBank;

/**
 * @class HTTPatternMatchTool
 * @brief Finds roads using pattern matching.
 *
 * This class does pattern matching by storing a layer bitmask for each PID.
 * It loops through each SSID and updates the bitmask for hit patterns. It also
 * keeps track of the hits so that subsequently creating a road is trivial.
 */
class HTTPatternMatchTool : public AthAlgTool, virtual public HTTRoadFinderToolI
{
    public:

        ///////////////////////////////////////////////////////////////////////
        // AthAlgTool

        HTTPatternMatchTool(const std::string&, const std::string&, const IInterface*);

        virtual StatusCode initialize() override;
        virtual StatusCode finalize() override;

        ///////////////////////////////////////////////////////////////////////
        // HTTRoadFinderToolI

        virtual StatusCode getRoads(const std::vector<const HTTHit*> & hits, std::vector<HTTRoad*> & roads) override;

    private:

        ///////////////////////////////////////////////////////////////////////
        // Handles (not owned)
        ServiceHandle<ITrigHTTBankSvc> m_HTTBankSvc;
        ServiceHandle<ITrigHTTMappingSvc> m_HTTMapping;
        HTTPatternBank const * m_bank;

        ///////////////////////////////////////////////////////////////////////
        // Properties

        int m_nLayers; // alias to m_HTTMapping->PlaneMap1stStage()->getNLogiLayers();
        int m_maxMissingLayers = 0; // max allowed number of missing layers for a pattern to still fire
        int m_region = 0; // hard-coded to 0 until we figure out what regions are
	int m_minHitLayers = 6; // min number of real hit layer (excluding WC)

        ///////////////////////////////////////////////////////////////////////
        // Metadata

        size_t m_nRoads = 0; // counter for number of roads returned so far

        ///////////////////////////////////////////////////////////////////////
        // Accumulators

        std::vector<HTTRoad> m_roads;
        std::vector<layer_bitmask_t> m_masks_hit; // Bitmask for hit layers for each pattern. size = nPatterns, index by PID. Does not include wildcards
        std::vector<layer_bitmask_t> m_masks_wc; // Bitmask for hit layers from wildcards
        std::map<pid_t, std::vector<const HTTHit*>> m_pattern_hits; // for each PID, store a list of hits in pattern
        std::map<pid_t, sector_t> m_pid2sector_map; // TODO this should just be a tree in the CCache file

        ///////////////////////////////////////////////////////////////////////
        // Helpers

        void processWildcards();
        void updateAccumulators(std::map<ssid_t, std::vector<const HTTHit*>> const & ssid_map);

};

#endif // HTTPATTERNMATCHTOOL_H
