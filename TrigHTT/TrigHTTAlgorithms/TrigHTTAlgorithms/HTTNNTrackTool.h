// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#ifndef HTTNNTRACKTOOL_H
#define HTTNNTRACKTOOL_H

/**
 * @file HTTNNTrackTool.h
 * @author Elliott Cheu
 * @date April 27 2021
 * @brief Utilize NN score to build track candidates
 *
 */

#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "TrigHTTObjects/HTTRoad.h"
#include "TrigHTTObjects/HTTHit.h"
#include "TrigHTTObjects/HTTTrack.h"
#include "TrigHTTObjects/HTTMultiTruth.h"
#include "TrigHTTObjects/HTTTruthTrack.h"
#include "TrigHTTUtils/HTTFunctions.h"
#include "TrigHTTBanks/HTTSectorBank.h"

#include "GaudiKernel/ITHistSvc.h"

#include "TrigHTTMaps/ITrigHTTMappingSvc.h"
#include "TrigHTTBanks/ITrigHTTBankSvc.h"
#include "TrigHTTMaps/HTTPlaneMap.h"

// ECC - NN libraries
// Class includes
#include "lwtnn/LightweightGraph.hh"
#include "lwtnn/parse_json.hh"


class ITrigHTTMappingSvc;

class HTTNNTrackTool : public AthAlgTool
{
  public:

        ///////////////////////////////////////////////////////////////////////
        // AthAlgTool

        HTTNNTrackTool(const std::string&, const std::string&, const IInterface*);

        virtual StatusCode initialize() override;
        virtual StatusCode finalize() override;
	    StatusCode getTracks(std::vector<HTTRoad*> &roads, std::vector<HTTTrack> &tracks, 
		                    //const std::vector<const HTTHit*>& allHits, 
                            //std::vector<const HTTHit*>& unusedHits, 
							const HTTNNMap *nnMap);
        static float getXScale() { return 1015;};
        static float getYScale() { return 1015;};
        static float getZScale() { return 3000;};

		// Flags
        DoubleProperty m_NNCut; // NN output value to cut on when selecting good tracks
        DoubleProperty m_chi2_scalefactor; // Scale factor to use in converting to a chi2

    private:

        ServiceHandle<ITrigHTTMappingSvc>   m_HTTMapping;
        ServiceHandle<ITHistSvc> m_tHistSvc;

        TTree *m_tree; // output tree
	std::vector<float> m_x; // x position of hit in road
        std::vector<float> m_y; // y pos
        std::vector<float> m_z; // z pos
	std::vector<float> m_barcodefrac; // truth barcode fraction for the hit
	std::vector<int> m_barcode; // truth barcode for the hit
	std::vector<int> m_eventindex; // event index for the hit
	std::vector<unsigned int> m_isPixel; // is hit pixel? if 0 it is strip
	std::vector<unsigned int> m_layer; // layer ID
	std::vector<unsigned int> m_isBarrel; // is hit in barrel? if 0 it is endcap
	std::vector<unsigned int> m_etawidth;
	std::vector<unsigned int> m_phiwidth;
	std::vector<unsigned int> m_etamodule;
	std::vector<unsigned int> m_phimodule;
	std::vector<unsigned int> m_ID; // ID hash for hit

	float m_phi; // phi pre-estimate from the 2d hough
	float m_invpt; // invpt pre-estimate from the 2d hough

	// quantities for the track matched to truth, not per hit
	float m_candidate_barcodefrac;
	float m_candidate_barcode;
	float m_candidate_eventindex;

	// track number in the event, since the request is to store this per road
	// naively vectors of vectors and one entry per event makes more sense but this was the
	// request from the ML people
	int m_tracknumber;

	// this is the tree index used to connect to the truth information
	int m_treeindex;

	// road number separates information from each road;
	int m_roadnumber;

	// Separate tree with truth track information
        TTree *m_truthtree; // output tree

	std::vector<float> m_truth_d0;
	std::vector<float> m_truth_z0;
	std::vector<float> m_truth_pt;
	std::vector<float> m_truth_eta;
	std::vector<float> m_truth_phi;
	std::vector<float> m_truth_pdg;
	std::vector<int> m_truth_q;
	std::vector<int> m_truth_barcode;
	std::vector<int> m_truth_eventindex;

	//////////////////////////////////////////////////////////////////
	// NN stuff
	int m_totalInputs;
        std::vector<const char*> m_input_node_names;
        std::vector<int64_t> m_input_node_dims;
        std::vector<const char*> m_output_node_names;

    void compute_truth(HTTTrack & newtrk) const;

};


#endif // HTTNNTRACKTOOL_H
