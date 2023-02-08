// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#ifndef HTTMatrixMergeAlgo_h
#define HTTMatrixMergeAlgo_h

/**
 * @file HTTMatrixMergeAlgo.h
 * @author Rewrite by Riley Xu - riley.xu@cern.ch after FTK code
 * @date May 8th, 2020
 * @brief Algorithm to merge matrix files, to be used for sector and constant generation.
 *
 * This algorithm merges multiple matrix files. See HTTMatrixGenAlgo.h for information
 * on the matrix file. This class uses the same map structure to accumulate the data.
 *
 * This algorithm doesn't run on events; the functinality takes place in initialize()
 * and finalize().
 */


#include "GaudiKernel/ITHistSvc.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "TrigHTTBankGen/HTTMatrixIO.h"

#include "TTree.h"
#include "TH1I.h"
#include "TH1F.h"

#include <string>
#include <vector>

class ITHistSvc;


class HTTMatrixMergeAlgo : public AthAlgorithm
{
    public:
        HTTMatrixMergeAlgo(const std::string& name, ISvcLocator* pSvcLocator);
        virtual ~HTTMatrixMergeAlgo() = default;
        StatusCode initialize() override;
        StatusCode execute() override;
        StatusCode finalize() override;

    private:

        ///////////////////////////////////////////////////////////////////////
        // Objects

        // Main logic. For each sector, store a struct that accumulates the track parameters, hit coordinates, etc.
        std::vector<AccumulateMap> m_sector_cum; // Index by region

        ///////////////////////////////////////////////////////////////////////
        // Handles

        ServiceHandle<ITHistSvc> m_tHistSvc;

        ///////////////////////////////////////////////////////////////////////
        // Configuration

        std::vector<std::string> m_fpath;

        BooleanProperty m_Monitor;
        bool m_allregion = false;

        int m_region = 0;
        int m_nRegions = 96;
        int m_region_start;
        int m_region_end;

        int m_nFiles = 0;
        int m_nLayers = 0;
        int m_nDim;
        int m_nDim2;

        ///////////////////////////////////////////////////////////////////////
        // Meta Data

        // Histograms
        TH1I* h_nSector;
        TH1I* h_nHit;
        TH1F* h_c;
        TH1F* h_d;
        TH1F* h_phi;
        TH1F* h_coto;
        TH1F* h_z;

        ///////////////////////////////////////////////////////////////////////
        // Helper Functions

        StatusCode bookHistograms();
        StatusCode copySliceTree(TFile *file);
        void readFiles();
        void readTree(TTree *matrix_tree, int region);
        std::vector<TTree*> createMatrixTrees();
        void fillMatrixTrees(std::vector<TTree*> const & matrixTrees);
};


#endif // HTTMatrixMergeAlgo_h

