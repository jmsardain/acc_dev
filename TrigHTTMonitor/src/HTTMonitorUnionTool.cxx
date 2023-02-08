/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTMonitor/HTTMonitorUnionTool.h"

#include "TrigHTTMonitor/HTTEventMonitorTool.h"


/////////////////////////////////////////////////////////////////////////////
HTTMonitorUnionTool::HTTMonitorUnionTool(std::string const & algname, std::string const & name, IInterface const * ifc) :
    HTTMonitorBase(algname,name,ifc),
    m_monitorTools(this)
{
    declareInterface<HTTMonitorI>(this);
    declareProperty("MonitorTools",         m_monitorTools,         "Array of HTTMonitorTools");
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTMonitorUnionTool::initialize()
{
    ATH_MSG_INFO("HTTMonitorUnionTool::initialize()");
    ATH_CHECK(m_monitorTools.retrieve());

    // Error checking
    if (m_monitorTools.empty()) {
        ATH_MSG_FATAL("No tools are loaded by HTTMonitorUnionTool.");
        return StatusCode::FAILURE;
    }

    ATH_MSG_INFO("Running with " << m_monitorTools.size() << " monitor tool(s).");

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTMonitorUnionTool::regTree(std::string const & dir, TTree* t)
{
    for (auto & monitorTool : m_monitorTools) {
        if (auto ptr = dynamic_cast<HTTEventMonitorTool*>(monitorTool.get())) {
            return ptr->regTree(dir, t);
        }
    }
    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTMonitorUnionTool::passMonitorEvent(HTTEventInputHeader* h,
                                                 HTTLogicalEventInputHeader* h_in_1st, HTTLogicalEventInputHeader* h_in_2nd,
                                                 HTTLogicalEventOutputHeader* h_out)
{
    for (auto & monitorTool : m_monitorTools) {
        ATH_CHECK(dynamic_cast<HTTMonitorI*>(monitorTool.get())->passMonitorEvent(h, h_in_1st, h_in_2nd, h_out));
    }
    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTMonitorUnionTool::passMonitorHits(std::vector<HTTHit> const & hits, std::string const & key)
{
    for (auto & monitorTool : m_monitorTools) {
        ATH_CHECK(dynamic_cast<HTTMonitorI*>(monitorTool.get())->passMonitorHits(hits, key));
    }
    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTMonitorUnionTool::passMonitorClusters(std::vector<HTTCluster> const & clusters, std::string const & key)
{
    for (auto & monitorTool : m_monitorTools) {
        ATH_CHECK(dynamic_cast<HTTMonitorI*>(monitorTool.get())->passMonitorClusters(clusters, key));
    }
    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTMonitorUnionTool::passMonitorRoads(std::vector<HTTRoad*> const & roads, std::string const & key)
{
    for (auto & monitorTool : m_monitorTools) {
        ATH_CHECK(dynamic_cast<HTTMonitorI*>(monitorTool.get())->passMonitorRoads(roads, key));
    }
    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTMonitorUnionTool::passMonitorTracks(std::vector<HTTTrack> const & tracks, std::string const & key)
{
    for (auto & monitorTool : m_monitorTools) {
        ATH_CHECK(dynamic_cast<HTTMonitorI*>(monitorTool.get())->passMonitorTracks(tracks, key));
    }
    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTMonitorUnionTool::fillHistograms()
{
    for (auto & monitorTool : m_monitorTools) {
        ATH_CHECK(dynamic_cast<HTTMonitorI*>(monitorTool.get())->fillHistograms());
    }
    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTMonitorUnionTool::printHistograms(PrintDetail detail) const
{
    for (auto & monitorTool : m_monitorTools) {
        if (auto ptr = dynamic_cast<HTTEventMonitorTool*>(monitorTool.get())) {
            return ptr->printHistograms(detail);
        }
    }
    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTMonitorUnionTool::finalize()
{
    ATH_MSG_INFO("HTTMonitorUnionTool::finalize()");
    return HTTMonitorBase::finalize();
}

