/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file HTTMonitorBase.h
 * @author Yumeng Cao - yumeng.cao@cern.ch
 * @date Jan 14th, 2020
 * @base class for all monitor tools
 */

#ifndef HTTMONITORBASE_H
#define HTTMONITORBASE_H


#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrigHTTMonitor/HTTMonitorI.h"

#include "TrigHTTConfig/IHTTEventSelectionSvc.h"
#include "TrigHTTMaps/ITrigHTTMappingSvc.h"
#include "TrigHTTUtils/HTTTypes.h"

#include "TGraphAsymmErrors.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"

#include <fstream>


class HTTCluster;
class HTTEventInputHeader;
class HTTHit;
class HTTLogicalEventInputHeader;
class HTTLogicalEventOutputHeader;
class HTTRoad;
class HTTTrack;


/////////////////////////////////////////////////////////////////////////////
class HTTMonitorBase: public AthAlgTool, virtual public HTTMonitorI
{

	public:

		HTTMonitorBase (std::string const &, std::string const &, IInterface const *);
		virtual ~HTTMonitorBase() {}

		static const InterfaceID& interfaceID();

		virtual StatusCode initialize() override;

		virtual StatusCode regTree(std::string const & dir, TTree* t) override { return m_tHistSvc->regTree(dir+t->GetName(), t); }

		virtual StatusCode passMonitorEvent(HTTEventInputHeader* h,
											HTTLogicalEventInputHeader* h_in_1st, HTTLogicalEventInputHeader* h_in_2nd,
											HTTLogicalEventOutputHeader* h_out) override;

		virtual StatusCode passMonitorHits    (std::vector<HTTHit> 	   const & hits, 	 std::string const & key) override;
		virtual StatusCode passMonitorClusters(std::vector<HTTCluster> const & clusters, std::string const & key) override;
		virtual StatusCode passMonitorRoads   (std::vector<HTTRoad*>   const & roads, 	 std::string const & key) override;
		virtual StatusCode passMonitorTracks  (std::vector<HTTTrack>   const & tracks, 	 std::string const & key) override;

		virtual StatusCode fillHistograms() override { m_nEvents++; return StatusCode::SUCCESS; }

        virtual StatusCode printHistograms(PrintDetail detail) const override;

        virtual StatusCode finalize() override;

	protected:

        virtual StatusCode bookHistograms()    	{ return StatusCode::SUCCESS; }
        virtual StatusCode retrieveHistograms() { return StatusCode::SUCCESS; }
        virtual StatusCode makeEffHistograms() 	{ return StatusCode::SUCCESS; }

        StatusCode selectEvent() const;

		void setHistDir(std::string const & dir) { m_dir = "/MONITOROUT" + dir; }
		std::string const & getHistDir() { return m_dir; }
		void clearHistDir() { m_dir = ""; }

		// For when you have mulitple performance tools
		std::string createDirName(std::string const & parent_string) const;

		int getNXbinsPT() const { return sizeof(m_xbinsPT) / sizeof(m_xbinsPT[0]) - 1; }

        StatusCode regGraph(std::string const & dir, TGraph* g)	{ return m_tHistSvc->regGraph(dir + g->GetName(), g); }
				StatusCode regHist (std::string const & dir, TH1*    h) { return m_tHistSvc->regHist (dir + h->GetName(), h); }
        StatusCode regHist (std::string const & dir, TH2*    h)	{ return m_tHistSvc->regHist (dir + h->GetName(), h); }
        StatusCode regHist (std::string const & dir, TH3*    h)	{ return m_tHistSvc->regHist (dir + h->GetName(), h); }

        std::vector<HTTHit>     const * getMonitorHits    (std::string const & key) const;
        std::vector<HTTCluster> const * getMonitorClusters(std::string const & key)	const;
        std::vector<HTTRoad*>   const * getMonitorRoads   (std::string const & key) const;
        std::vector<HTTTrack>   const * getMonitorTracks  (std::string const & key) const;

        void clearMonitorData();

        std::string printVerbose(TH1 const * h) const;
        std::string printProfile(TH1* h) const;

        // Wrapper function to guarantee efficiency plots are produced correctly. Force h_pass <= h_total for each bin.
				void getEff(TGraphAsymmErrors* h_eff, TH1 const * h_pass, TH1 const * h_total) const;
				void getEffJMS(TH1F* h_eff, TH1F const * h_pass, TH1F const * h_total) const;

        void checkHistRange() const;

		ServiceHandle<ITHistSvc>		        m_tHistSvc;
		ServiceHandle<ITrigHTTMappingSvc>	    m_HTTMapping;
		ServiceHandle<IHTTEventSelectionSvc>    m_evtSel;

		BooleanProperty m_extend = true; // extend x-axis of some histograms
		BooleanProperty	m_checkHistRange = false;
        StringProperty m_outputtag;

        FloatProperty m_cut_barcodefrac = 0.5;
        FloatProperty m_cut_chi2ndof = 40.;

        // Configurable objects for histogram ranges
        std::vector<double> m_d0Range = {-3.5,3.5};
        std::vector<double> m_z0Range = {-250,250};

		// internal pointers
		HTTEventInputHeader*			m_eventInputHeader;
		HTTLogicalEventInputHeader*		m_logicEventInputHeader_1st;
		HTTLogicalEventInputHeader*		m_logicEventInputHeader_2nd;
		HTTLogicalEventOutputHeader*	m_logicEventOutputHeader;

		// histogram directory
		std::string m_dir;

		// pT binning used to make pT plots.
		Double_t const m_xbinsPT[17] = {1., 2., 3., 4., 5., 6., 7., 8., 9., 10., 20., 30., 40., 50., 100., 200., 400.}; //need 16 to go with these edges

		// ranges for eta/phi set automatically based on sector boundaries
		float m_etamin;
		float m_etamax;
		float m_phimin;
		float m_phimax;

		// Resolution histogram limits
		const float ResHistLimit_ipt = 0.1;
		const float ResHistLimit_pt  = 1.;
		const float ResHistLimit_eta = 0.05;
		const float ResHistLimit_phi = 0.05;
		const float ResHistLimit_d0  = 4.;
		const float ResHistLimit_z0  = 6.;

		const float ResHistLimit_ipt2 = 0.1;
		const float ResHistLimit_pt2 = 1.;
		const float ResHistLimit_eta2 = 0.01;
		const float ResHistLimit_phi2 = 0.01;
		const float ResHistLimit_d02 = 0.5;
		const float ResHistLimit_z02 = 1.;

		size_t m_nEvents 	 = 0;
		size_t m_nLayers_1st = 0;
		size_t m_nLayers_2nd = 0;

		bool m_withPU = false;

		// Unordered maps used to pass data to monitor tools
		std::unordered_map< std::string, std::vector<HTTHit>	 const * >	m_monitorHits;
		std::unordered_map< std::string, std::vector<HTTCluster> const * >	m_monitorClusters;
		std::unordered_map< std::string, std::vector<HTTRoad*>	 const * >	m_monitorRoads;
		std::unordered_map< std::string, std::vector<HTTTrack>	 const * >	m_monitorTracks;
};

#endif // HTT_MONITORBASE_H
