// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#ifndef HTTMatrixReductionAlgo_h
#define HTTMatrixReductionAlgo_h

/**
 * @file HTTMatrixReductionAlgo.h
 * @author Rewrite by Riley Xu - riley.xu@cern.ch after FTK code
 * @date May 21st, 2020
 * @brief Algorithm to reduce matrix files from 2nd stage to 1st stage.
 *
 * This algorithm simply copies the sector matrices from the input file,
 * but only keeping 1st stage layers/coordinates. In python pseudocode,
 *          layer_filter = [ is1stStage(layer_2nd) for layer_2nd in range(# 2nd stage layers) ]
 *          modules_1st = modules_2nd[layer_filter]
 * and similarly for the coordinate-length members.
 *
 * It is questionable if this step is even needed. I imagine the constant generation can just
 * read in the 2nd stage matrices and do this step by itself. (Once we combine the plane maps
 * the filtering should be just a built-in function for the pmap class).
 */


#include "GaudiKernel/ITHistSvc.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "TrigHTTMaps/ITrigHTTMappingSvc.h"
#include "TrigHTTBankGen/HTTMatrixAccumulator.h"

class TTree;

#include <string>
#include <vector>


class HTTMatrixReductionAlgo: public AthAlgorithm
{
    public:

        HTTMatrixReductionAlgo (const std::string& name, ISvcLocator* pSvcLocator);
        virtual ~HTTMatrixReductionAlgo() = default;
        StatusCode initialize() override;
        StatusCode execute() override;
        StatusCode finalize() override;

    private:

        ///////////////////////////////////////////////////////////////////////
        // Handles

        ServiceHandle<ITrigHTTMappingSvc> m_HTTMapping;
        ServiceHandle<ITHistSvc> m_tHistSvc;
        HTTPlaneMap const * m_pmap_1st;
        HTTPlaneMap const * m_pmap_2nd;

        ///////////////////////////////////////////////////////////////////////
        // Configuration

        std::string m_filePath;

        bool m_allregion = false;
        int m_region = 0;
        int m_nRegions = 96;
        int m_region_start;
        int m_region_end;

        ///////////////////////////////////////////////////////////////////////
        // Data

        std::vector<AccumulateMap> m_sector_cum; // 1st stage accumulate maps, size nRegion.

        ///////////////////////////////////////////////////////////////////////
        // Helper Functions

        StatusCode copySliceTree(TFile *file);
        int getInversion(size_t layer_2nd);
        std::pair<std::vector<size_t>, std::vector<size_t>> matchStages();
        std::pair<std::vector<module_t>, HTTMatrixAccumulator> reduce(
                std::vector<module_t> const & modules_2nd, HTTMatrixAccumulator const & acc_2nd,
                std::vector<size_t> const & layers_1st_to_2nd, std::vector<size_t> const & coords_1st_to_2nd);
        void extract_1stStage(TFile *file);
};




#endif // HTTMatrixReductionAlgo_h

