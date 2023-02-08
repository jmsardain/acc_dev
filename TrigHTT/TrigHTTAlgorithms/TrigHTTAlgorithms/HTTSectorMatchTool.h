// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#ifndef HTTSECTORMATCHTOOL_H
#define HTTSECTORMATCHTOOL_H

/**
 * @file HTTSectorMatchTool.h
 * @author Riley Xu - rixu@cern.ch
 * @date April 2nd, 2020
 * @brief Declares a class that does road finding using sectors as patterns
 */


#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include "TrigHTTObjects/HTTRoad.h"
#include "TrigHTTObjects/HTTHit.h"
#include "TrigHTTUtils/HTTTypes.h"
#include "TrigHTTBanks/HTTSectorBank.h"
#include "TrigHTTAlgorithms/HTTRoadFinderToolI.h"

#include <string>
#include <vector>
#include <map>

class ITrigHTTBankSvc;
class ITrigHTTMappingSvc;
class HTTPatternBank;

/**
 * @class HTTSectorMatchTool
 * @brief Finds roads using sector matching.
 *
 * This class does sector matching by storing a layer bitmask for each sector.
 * It loops through each hit's module and updates the bitmask for hit sectors. It also
 * keeps track of the hits so that subsequently creating a road is trivial.
 */
class HTTSectorMatchTool : public AthAlgTool, virtual public HTTRoadFinderToolI
{
    public:

        ///////////////////////////////////////////////////////////////////////
        // AthAlgTool

        HTTSectorMatchTool(const std::string&, const std::string&, const IInterface*);

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
        HTTSectorBank *m_sectors = nullptr; // alias to m_HTTBankSvc->SectorBank();

        ///////////////////////////////////////////////////////////////////////
        // Properties

        size_t m_nLayers; // alias to m_HTTMapping->PlaneMap1stStage()->getNLogiLayers();
        int m_maxMissingLayers = 0; // max allowed number of missing layers for a pattern to still fire

        ///////////////////////////////////////////////////////////////////////
        // Convenience

        std::vector<layer_bitmask_t> m_masks_wc; // Bitmask for hit layers from wildcards

        ///////////////////////////////////////////////////////////////////////
        // Event Storage

        std::vector<HTTRoad> m_roads;

        ///////////////////////////////////////////////////////////////////////
        // Metadata

        size_t m_nRoads = 0; // counter for number of roads returned so far

        ///////////////////////////////////////////////////////////////////////
        // Helpers

        void processWildcards();
        std::vector<layer_bitmask_t> createBitmasks(std::map<module_t, std::vector<const HTTHit*>> const & module_hit_map) const;
        std::vector<std::vector<const HTTHit*>> getSectorHits(std::map<module_t, std::vector<const HTTHit*>> const & module_hit_map, sector_t sector) const;
};

#endif // HTTSECTORMATCHTOOL_H
