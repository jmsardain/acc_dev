// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#ifndef HTTMatrixGenAlgo_h
#define HTTMatrixGenAlgo_h

/**
 * @file HTTMatrixGenAlgo.h
 * @author Rewrite by Riley Xu - riley.xu@cern.ch after FTK code
 * @date May 8th, 2020
 * @brief Algorithm to generate matrix files, to be used for sector and constant generation.
 *
 * This algorithm uses muon truth tracks to generate matrix files. The matrix files
 * contain the sector definitions (list of modules) and an accumulation of the tracks'
 * hits and track parameters. These are later used by ConstGenAlgo to generate fit constants.
 *
 * For each sector, we store the information in the MatrixAccumulator struct. Each track
 * found in the sector has its parameters and hit coordinates added/appended to the struct.
 */


#include "GaudiKernel/ITHistSvc.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "TrigHTTUtils/HTTTypes.h"
#include "TrigHTTConfig/IHTTEventSelectionSvc.h"
#include "TrigHTTMaps/ITrigHTTMappingSvc.h"
#include "TrigHTTMaps/HTTPlaneMap.h"
#include "TrigHTTMaps/HTTRegionMap.h"
#include "TrigHTTInput/HTTSGToRawHitsTool.h"
#include "TrigHTTInput/HTTEventInputHeaderToolI.h"
#include "TrigHTTInput/HTT_RawToLogicalHitsTool.h"
#include "TrigHTTMaps/HTTClusteringToolI.h"
#include "TrigHTTObjects/HTTEventInputHeader.h"
#include "TrigHTTObjects/HTTTruthTrack.h"
#include "TrigHTTBankGen/HTTMatrixIO.h"

#include "TTree.h"
#include "TFile.h"
#include "TH1I.h"

#include <string>
#include <vector>


class ITHistSvc;
class TH1F;
class TH2F;
class ITrigHTTMappingSvc;
class HTTRoadFinderToolI;
class HTTHoughTransformTool;


class HTTMatrixGenAlgo : public AthAlgorithm
{
    public:
        HTTMatrixGenAlgo(const std::string& name, ISvcLocator* pSvcLocator);
        virtual ~HTTMatrixGenAlgo() = default;

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

        ServiceHandle<ITrigHTTMappingSvc>    m_HTTMapping;
        ServiceHandle<IHTTEventSelectionSvc> m_EvtSel;
        ServiceHandle<ITHistSvc>             m_tHistSvc;

        ToolHandle<HTTSGToRawHitsTool>       m_hitInputTool; // input handler
	ToolHandle<HTT_RawToLogicalHitsTool> m_hitMapTool;
        ToolHandle<HTTClusteringToolI>       m_clusteringTool;// { this, "HTTClusteringFTKTool", "HTTClusteringFTKTool/HTTClusteringFTKTool", "HTTClusteringFTKTool" };
	ToolHandle<HTTRoadFinderToolI>       m_roadFinderTool;
	const HTTPlaneMap* m_pmap = nullptr; // alias to m_HTTMapping->PlaneMap();

        ///////////////////////////////////////////////////////////////////////
        // Configuration

        int m_nRegions = 0;
        int m_nLayers = 0;
        int m_nDim = 0;
        int m_nDim2 = 0; // m_nDim ^ 2

        float m_PT_THRESHOLD = 0;
        float m_D0_THRESHOLD = 1;
        int m_TRAIN_PDG = 0;
        int m_MaxWC;

        HTTTrackPars m_sliceMin = 0;
        HTTTrackPars m_sliceMax = 0;
        HTTTrackParsI m_nBins;

        bool m_doClustering = true;
        int m_ideal_geom = 0;
	bool m_single = false;
	bool m_doHoughConstants = false; // If true will do the matrix for the delta global phis method


        ///////////////////////////////////////////////////////////////////////
        // Meta Data

        size_t m_nTracks = 0; // Total number of truth tracks encountered
        size_t m_nTracksUsed = 0; // Total number of tracks after filtering, i.e. used to fit constants

        ///////////////////////////////////////////////////////////////////////
        // Helper Functions

        enum selectHit_returnCode { SH_FAILURE, SH_KEEP_OLD, SH_KEEP_NEW };

        StatusCode bookHistograms();
        std::vector<HTTHit> getLogicalHits(HTTEventInputHeader & header) const;
        std::vector<HTTTruthTrack> filterTrainingTracks(std::vector<HTTTruthTrack> const & truth_tracks) const;
        std::map<int, std::vector<HTTHit>> makeBarcodeMap(std::vector<HTTHit> const & hits, std::vector<HTTTruthTrack> const & tracks) const;
        selectHit_returnCode selectHit(HTTHit const & old_hit, HTTHit const & new_hit) const;
        bool filterSectorHits(std::vector<HTTHit> const & all_hits, std::vector<HTTHit> & sector_hits, HTTTruthTrack const & t) const;
        int getRegion(std::vector<HTTHit> const & hits) const;
	StatusCode makeAccumulator(std::vector<HTTHit> const & sector_hits, HTTTruthTrack const & track, std::pair<std::vector<module_t>, HTTMatrixAccumulator> & accumulator) const;
        std::vector<TTree*> createMatrixTrees();
        void fillMatrixTrees(std::vector<TTree*> const & matrixTrees);
        void writeSliceTree();

        ///////////////////////////////////////////////////////////////////////
        // Histograms

        // These are ordered as in HTTTrackPars, phi, curvature, d0, z0, eta
        TH1I* h_trainingTrack[HTTTrackPars::NPARS];
        TH1I* h_sectorPars[HTTTrackPars::NPARS];
        TH1I* h_SHfailure[HTTTrackPars::NPARS];
        TH1I* h_3hitsInLayer[HTTTrackPars::NPARS];
        TH1I* h_notEnoughHits[HTTTrackPars::NPARS];

        TH1I* h_trackQoP_okHits;
        TH1I* h_trackQoP_okRegion;

        TH1I* h_nHit;
};

#endif // HTTMatrixGenAlgo_h
