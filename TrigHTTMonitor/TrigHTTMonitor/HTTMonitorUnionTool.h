/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTTMONITORUNIONTOOL_H
#define HTTMONITORUNIONTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrigHTTMonitor/HTTMonitorBase.h"

class HTTHit;

/////////////////////////////////////////////////////////////////////////////
class HTTMonitorUnionTool: public HTTMonitorBase
{
	public:

		HTTMonitorUnionTool(std::string const &, std::string const &, IInterface const *);

		virtual StatusCode initialize() override;

		virtual StatusCode regTree(std::string const & dir, TTree* t) override;

		virtual StatusCode passMonitorEvent(HTTEventInputHeader* h,
											HTTLogicalEventInputHeader* h_in_1st, HTTLogicalEventInputHeader* h_in_2nd,
											HTTLogicalEventOutputHeader* h_out) override;

		virtual StatusCode passMonitorHits    (std::vector<HTTHit> 	   const & hits, 	 std::string const & key) override;
		virtual StatusCode passMonitorClusters(std::vector<HTTCluster> const & clusters, std::string const & key) override;
		virtual StatusCode passMonitorRoads   (std::vector<HTTRoad*>   const & roads, 	 std::string const & key) override;
		virtual StatusCode passMonitorTracks  (std::vector<HTTTrack>   const & tracks, 	 std::string const & key) override;

		virtual StatusCode fillHistograms() override;

		virtual StatusCode printHistograms(PrintDetail detail) const override;

        virtual StatusCode finalize() override;

        ToolHandleArray<HTTMonitorI> const & monitorTools() const { return m_monitorTools; }

    private:

    	ToolHandleArray<HTTMonitorI> m_monitorTools;
};

#endif
