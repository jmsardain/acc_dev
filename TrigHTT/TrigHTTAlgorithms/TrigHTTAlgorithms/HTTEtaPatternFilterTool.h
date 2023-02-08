// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#ifndef HTTETAPATTERNFILTERTOOL_H
#define HTTETAPATTERNFILTERTOOL_H

/**
 * @file HTTEtaPatternFilterTool.h
 * @author Elliot Lipeles - lipeles@cern.ch
 * @date March 25th, 2021
 * @brief Implements road filtering using eta module patterns
 *
 * Declarations in this file:
 *      class HTTEtaPatternFilterTool : public AthAlgTool, virtual public HTTRoadFilterToolI
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


class HTTEtaPatternFilterTool : public AthAlgTool, virtual public HTTRoadFilterToolI
{
    public:

        ///////////////////////////////////////////////////////////////////////
        // AthAlgTool

        HTTEtaPatternFilterTool(const std::string&, const std::string&, const IInterface*);

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

        std::string m_pattern_file_path;  // path to pattern file
        unsigned m_threshold = 0; // Minimum number of hit layers to fire a road

        ///////////////////////////////////////////////////////////////////////////////
        // Utility Class

        class ModulesToPattern;
        struct ModuleId
        {
            SiliconTech siTech = SiliconTech::undefined;
            DetectorZone zone = DetectorZone::undefined;
            int etaModule = 0;
        };
        friend bool operator <(const ModuleId& lhs, const ModuleId& rhs);
        typedef std::vector<ModuleId> EtaPattern; // list of module ids in each layer

        ///////////////////////////////////////////////////////////////////////
        // Event Storage
        std::vector<HTTRoad_Hough> m_postfilter_roads;

        ///////////////////////////////////////////////////////////////////////
        // Convenience

        unsigned m_nLayers; // alias to m_HTTMapping->PlaneMap1stStage()->getNLogiLayers();

        // The below maps are created in initialize, with fixed keys. But the counters (values)
        // are reset every input road.
        std::map<EtaPattern, layer_bitmask_t> m_patternmap;
            // keys initialized from file
            // for each input road, the bitmask is reset
        std::vector<std::map<ModuleId, ModulesToPattern>> m_moduleHits;
            // inverses the above map, mapping (layer, moduleId) to patterns
            // note this stores pointers to m_patternmap, and will modify it
            // also stores a list of hits for each input road

        ///////////////////////////////////////////////////////////////////////
        // Metadata and Monitoring

        ///////////////////////////////////////////////////////////////////////
        // Helpers
        void readPatterns(std::string const & filepath);
        void buildMap();
        void resetCounters();
        void addHitsToMap(HTTRoad* r);
        void addRedundantPatterns(std::set<EtaPattern> & usedPatterns, EtaPattern const & currPatt, unsigned nExtra);
        HTTRoad_Hough buildRoad(std::pair<EtaPattern, layer_bitmask_t> const & patt, HTTRoad* origr) const;
        std::string to_string(EtaPattern patt) const;
        std::string to_string(HTTRoad road) const;
        std::string to_string(std::vector<unsigned> v) const;
};


#endif // HTTETAPATTERNFILTERTOOL_H
