/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file HTTMonitorI.h
 * @author Yumeng Cao - yumeng.cao@cern.ch
 * @date Jan 14th, 2020
 * @Abstract interface for all monitor tools
 */

#ifndef HTTMONITORI_H
#define HTTMONITORI_H

#include "GaudiKernel/IAlgTool.h"
#include "TTree.h"

class HTTCluster;
class HTTEventInputHeader;
class HTTHit;
class HTTLogicalEventInputHeader;
class HTTLogicalEventOutputHeader;
class HTTRoad;
class HTTTrack;

static const InterfaceID IID_HTTMonitorI("HTTMonitorI",1,0);

class HTTMonitorI : virtual public IAlgTool {

	public:
		DeclareInterfaceID( HTTMonitorI, 1, 0);
		virtual ~HTTMonitorI() = default;

		enum PrintDetail
		{
			NONE,       // no print
			SUMMARY,    // misc summary statistics, non-histo counters
			CONCISE,    // (mean, entries, rms) of select histograms
			ALL 		// (mean, entries, rms) of all histograms
		};

		virtual StatusCode regTree(std::string const & dir, TTree* t) = 0;

		virtual StatusCode passMonitorEvent(HTTEventInputHeader* h,
											HTTLogicalEventInputHeader* h_in_1st, HTTLogicalEventInputHeader* h_in_2nd,
											HTTLogicalEventOutputHeader* h_out) = 0;

		virtual StatusCode passMonitorHits(std::vector<HTTHit> 		   const & hits, 	 std::string const & key) = 0;
		virtual StatusCode passMonitorClusters(std::vector<HTTCluster> const & clusters, std::string const & key) = 0;
		virtual StatusCode passMonitorRoads(std::vector<HTTRoad*> 	   const & roads, 	 std::string const & key) = 0;
		virtual StatusCode passMonitorTracks(std::vector<HTTTrack> 	   const & tracks, 	 std::string const & key) = 0;

		virtual StatusCode fillHistograms() = 0;

		virtual StatusCode printHistograms(PrintDetail detail = PrintDetail::SUMMARY) const = 0;

};

#endif // HTT_MONITORI_H
