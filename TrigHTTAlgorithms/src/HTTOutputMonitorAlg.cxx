/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTAlgorithms/HTTOutputMonitorAlg.h"

#include "TrigHTTAlgorithms/HTTDataFlowTool.h"
#include "TrigHTTInput/HTTEventOutputHeaderToolI.h"
#include "TrigHTTMonitor/HTTMonitorUnionTool.h"
#include "TrigHTTObjects/HTTLogicalEventInputHeader.h"
#include "TrigHTTObjects/HTTLogicalEventOutputHeader.h"


/////////////////////////////////////////////////////////////////////////////
HTTOutputMonitorAlg::HTTOutputMonitorAlg (std::string const & name, ISvcLocator* pSvcLocator) :
    AthAlgorithm(name, pSvcLocator),
    m_readOutputTool("HTTOutputHeaderTool/HTTOutputHeaderTool"),
    m_monitorUnionTool("HTTMonitorUnionTool/HTTMonitorUnionTool"),
    m_dataFlowTool("HTTDataFlowTool/HTTDataFlowTool")
{
    declareProperty("ReadOutputTool",               m_readOutputTool);
    declareProperty("MonitorUnionTool",             m_monitorUnionTool);
    declareProperty("DataFlowTool",                 m_dataFlowTool);
    declareProperty("RunSecondStage",               m_runSecondStage,           "flag to enable running the second stage fitting");
    declareProperty("histoPrintDetail",             m_histoPrintDetail,         "histogram printing detail, see HTTMonitorI");
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTOutputMonitorAlg::initialize()
{
    ATH_CHECK(m_readOutputTool.retrieve());
    ATH_CHECK(m_monitorUnionTool.retrieve());
    ATH_CHECK(m_dataFlowTool.retrieve());

    m_inputHeader_1st = new HTTLogicalEventInputHeader();
    m_inputHeader_2nd = new HTTLogicalEventInputHeader();
    m_outputHeader    = new HTTLogicalEventOutputHeader();

    ATH_CHECK(m_monitorUnionTool->passMonitorEvent(nullptr, m_inputHeader_1st, m_inputHeader_2nd, m_outputHeader));

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTOutputMonitorAlg::execute()
{
    bool last = false;
    ATH_CHECK(m_readOutputTool->readData(m_inputHeader_1st, m_inputHeader_2nd, m_outputHeader, last));
    if (last) return StatusCode::SUCCESS;

    ATH_CHECK(m_monitorUnionTool->fillHistograms());
    ATH_CHECK(m_dataFlowTool->getDataFlowInfo(m_outputHeader->getDataFlowInfo()));

    // // print some tracks for TMP test-vectors:
    // for (auto track : m_outputHeader->getHTTTracks_1st()) {
    //     std::cout << track << std::endl;
    // }
    // for (auto track : m_outputHeader->getHTTTracks_2nd()) {
    //     std::cout << track << std::endl;
    // }

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTOutputMonitorAlg::finalize()
{
    ATH_CHECK(m_monitorUnionTool->printHistograms(static_cast<HTTMonitorI::PrintDetail>(m_histoPrintDetail)));

    return StatusCode::SUCCESS;
}

