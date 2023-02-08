// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#ifndef HTTEXTRAPOLATOR_H
#define HTTEXTRAPOLATOR_H

/**
 * @file HTTExtrapolator.h
 * @author Chris Delitzsch - chris.malena.delitzsch@cern.ch
 * @date March 18, 2021
 * @brief Declares a class that finds the inputs for the second stage fitting
 */


#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include "TrigHTTObjects/HTTRoad.h"
#include "TrigHTTObjects/HTTHit.h"
#include "TrigHTTUtils/HTTTypes.h"
#include "TrigHTTBanks/HTTSectorBank.h"
#include "TrigHTTBanks/HTTFitConstantBank.h"
#include "TrigHTTAlgorithms/HTTRoadFinderToolI.h"
#include "TrigHTTObjects/HTTTrack.h"

#include <string>
#include <vector>
#include <map>

class ITrigHTTBankSvc;
class ITrigHTTMappingSvc;

/**
 * @class HTTExtrapolator
 * @brief Finds "roads" for second stage fitting
 *
 */
class HTTExtrapolator : public AthAlgTool
{
    public:

        ///////////////////////////////////////////////////////////////////////
        // AthAlgTool

        HTTExtrapolator(const std::string&, const std::string&, const IInterface*);

        virtual StatusCode initialize() override;
        virtual StatusCode finalize() override;

        ///////////////////////////////////////////////////////////////////////

        StatusCode getSecondStageFittingInputs(std::vector<const HTTHit*> const & phits_2nd, std::vector<HTTTrack> const & tracks_1st_passOR, std::vector<HTTRoad*> & roads_2nd);

        size_t getNConstantsExtrapolate() { return m_nConstants_extrapolate; }

    private:

        ///////////////////////////////////////////////////////////////////////
        // Handles (not owned)

        ServiceHandle<ITrigHTTBankSvc> m_HTTBankSvc;
        ServiceHandle<ITrigHTTMappingSvc> m_HTTMapping;

        HTTFitConstantBank* m_constantBank_2nd = nullptr;
        HTTPlaneMap const * m_pmap_1st;
        HTTPlaneMap const * m_pmap_2nd;

        HTTSectorBank* m_sectors_2nd = nullptr;

        std::vector<HTTRoad> m_roads_2nd;
        size_t m_nRoads_2nd = 0;
        size_t m_nConstants_extrapolate = 0;

        ///////////////////////////////////////////////////////////////////////
        // Properties

        size_t m_nLayers_2nd; // alias to m_HTTMapping->PlaneMap_2nd()->getNLogiLayers();
        unsigned int m_NCombMax = 8; //max number of second stage sectors being considered

        std::vector<size_t> layers_1st_to_2nd;
        std::vector<size_t> layers_2nd_to_1st;

        std::vector<std::vector<const HTTHit*>> getSectorHits(std::map<module_t, std::vector<const HTTHit*>> const & module_hit_map, sector_t sector) const;
        std::vector<std::vector<const HTTHit*>> getFirstStageHits(std::vector<HTTHit> const & track_hits_1st, std::vector<const HTTHit*> const & phits_2nd, layer_bitmask_t & masks_hit, layer_bitmask_t & masks_wildcard);
        std::vector<std::vector<const HTTHit*>> getHitsForRoad(std::vector<std::vector<const HTTHit*> > hits_1st, std::vector<std::vector<const HTTHit*>> sector_hits, std::vector<HTTHit> track_hits, layer_bitmask_t & masks_hits);

};

#endif // HTTSECTORMATCHTOOL_H
