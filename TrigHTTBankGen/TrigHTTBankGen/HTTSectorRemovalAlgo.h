// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#ifndef HTTSectorRemovalAlgo_h
#define HTTSectorRemovalAlgo_h

/**
 * @file HTTSectorRemovalAlgo.h
 * @author Riley Xu - riley.xu@cern.ch
 * @date Aug 4th, 2020
 * @brief Algorithm to handle duplicate removal of sectors in a matrix file.
 *
 * Wildcard sectors may occasionally be extraneous, in the sense that a typical
 * 13/13 track misses a hit from random chance (?) and not because of a gap in
 * the detector or a dead module, which is what we want to use wildcards for.
 * During matrix generation these tracks will generate a new sector that really
 * isn't different from the original. These 'duplicates' can lead to exponential
 * number of roads.
 *
 * This algorithm doesn't actually remove the entries from the file, but just
 * outputs a list of sectors (entry numbers) to skip.
 *
 */


#include "AthenaBaseComps/AthAlgorithm.h"
#include "TrigHTTMaps/ITrigHTTMappingSvc.h"
#include "TrigHTTBankGen/HTTMatrixIO.h"

#include <string>
#include <vector>

class TTree;

class HTTSectorRemovalAlgo : public AthAlgorithm
{
    public:
        HTTSectorRemovalAlgo(const std::string& name, ISvcLocator* pSvcLocator);
        virtual ~HTTSectorRemovalAlgo() = default;
        StatusCode initialize() override;
        StatusCode execute() override;
        StatusCode finalize() override;

        // Main methods for doing the removal
        enum class Method
        {
            discardWc,      // keep non-wildcard sectors over wildcard ones
            preferWc,         // keep wildcard sectors over non-wildcard ones
            coverageThresh  // keep wildcard sectors using a coverage threshold
        };

    private:

        ///////////////////////////////////////////////////////////////////////
        // Objects

        // For each layer, store a map from module ids to sectors.
        std::vector<std::map<module_t, std::vector<sector_t>>> m_moduleMap;

        // Condensed information from the matrices (if we read the full MatrixAccumulator
        // it won't fit in memory). Index by sector.
        struct SectorInfo
        {
            size_t coverage;
            std::vector<module_t> modules;
            // HTTTrackPArs pars; maybe in future iterations can also compare pars? constants?
        };
        std::vector<SectorInfo> m_sectorInfo;

        // List of sectors to be removed
        std::vector<bool> m_removeList;

        ///////////////////////////////////////////////////////////////////////
        // Handles

        ServiceHandle<ITrigHTTMappingSvc> m_HTTMapping;

        ///////////////////////////////////////////////////////////////////////
        // Configuration

        std::string m_inpath;
        std::string m_outpath = "remove_list.txt";
        int m_nLayers = 0;
        int m_nCoords = 0;
        int m_region = 0;

        std::string m_sMethod = "discard_wildcards"; // String for athena property, cast to m_method below.
        Method m_method = Method::discardWc; // Main configuration selector.

        ///////////////////////////////////////////////////////////////////////
        // Helper Functions

        void readTree(TTree *tree);
        void makeRemovalList();
        std::vector<sector_t> getMatches(sector_t s1);
        void compareMatches(sector_t s1, std::vector<sector_t> const & matches);
};


#endif // HTTSectorRemovalAlgo_h

