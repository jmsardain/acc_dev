/*
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTAlgorithms/HTTOverlapRemovalValAlg.h"

#include "TrigHTTBanks/ITrigHTTBankSvc.h"
#include "TrigHTTConfig/IHTTEventSelectionSvc.h"
#include "TrigHTTObjects/HTTLogicalEventInputHeader.h"
#include "TrigHTTObjects/HTTLogicalEventOutputHeader.h"
#include "TrigHTTObjects/HTTTrack.h"
#include "TrigHTTObjects/HTTTruthTrack.h"


HTTOverlapRemovalValAlg::HTTOverlapRemovalValAlg(const std::string& name, ISvcLocator* pSvcLocator):
    AthAlgorithm(name,pSvcLocator),
    m_readOutputTool("HTTOutputHeaderTool/HTTOutputHeaderTool"),
    m_OverlapRemovalTool("HTTOverlapRemovalTool/HTTOverlapRemovalTool"),
    m_OverlapRemovalMonitorTool("HTTOverlapRemovalMonitorTool/HTTOverlapRemovalMonitorTool"),
    m_HTTBankSvc("TrigHTTBankSvc", name),
    m_EvtSel("HTTEventSelectionSvc",name)
{
  declareProperty("InputTool",              m_readOutputTool);
  declareProperty("OverlapRemoval",         m_OverlapRemovalTool);
  declareProperty("OverlapRemovalMonitor",  m_OverlapRemovalMonitorTool);
}


StatusCode HTTOverlapRemovalValAlg::initialize()
{
  ATH_MSG_INFO("HTTOverlapRemovalValAlg::initialize()");

  //  Retrieve all tools
  ATH_CHECK(m_readOutputTool.retrieve());
  ATH_CHECK(m_OverlapRemovalTool.retrieve());
  ATH_CHECK(m_OverlapRemovalMonitorTool.retrieve());
  ATH_CHECK(m_HTTBankSvc.retrieve());
  ATH_CHECK(m_EvtSel.retrieve());

  //  new ROOT object pointers
  m_inputHeader_1st = new HTTLogicalEventInputHeader();
  m_inputHeader_2nd = new HTTLogicalEventInputHeader();
  m_outputHeader    = new HTTLogicalEventOutputHeader();

  ATH_CHECK(m_OverlapRemovalMonitorTool->bookHistograms());
  ATH_MSG_DEBUG("Finished initialization");
  return StatusCode::SUCCESS;
}


StatusCode HTTOverlapRemovalValAlg::execute()
{
  ATH_MSG_DEBUG("Running on event ");

  //  Get event
  bool last = false;
  ATH_CHECK(m_readOutputTool->readData(m_inputHeader_1st, m_inputHeader_2nd, m_outputHeader, last));
  if (last) return StatusCode::SUCCESS;

  // Get tracks and truth tracks from the output header
  std::vector<HTTTruthTrack> truth_tracks = m_inputHeader_1st->optional().getTruthTracks();
  std::vector<HTTTrack> tracks_1st = m_outputHeader->getHTTTracks_1st();

  //  Run OR tools
  if(m_OverlapRemovalTool->runOverlapRemoval(tracks_1st) == StatusCode::SUCCESS)
  {
    ATH_MSG_DEBUG("Overlap removal done");
  }
  else
  {
    ATH_MSG_FATAL("Failure of OverlapRemoval");
    return StatusCode::FAILURE;
  }

  //  Fill histograms
  ATH_CHECK(m_OverlapRemovalMonitorTool->fillAllTrackHistograms(tracks_1st));
  ATH_CHECK(m_OverlapRemovalMonitorTool->fillORHistograms(tracks_1st));
  ATH_CHECK(m_OverlapRemovalMonitorTool->fillbestTrackHistograms(tracks_1st));

  //  Fill truth matching and resolution plots for single muon sample only
  if(m_EvtSel->getSampleType() == SampleType::singleMuons)
  {
    ATH_CHECK(m_OverlapRemovalMonitorTool->fillORTruthMatchHistograms(truth_tracks, tracks_1st));
    ATH_CHECK(m_OverlapRemovalMonitorTool->fillBestTruthMatchHistograms(truth_tracks, tracks_1st));
  }

  return StatusCode::SUCCESS;
}

StatusCode HTTOverlapRemovalValAlg::finalize()
{
  //  Finalize efficiency plots
  if(m_EvtSel->getSampleType() == SampleType::singleMuons)
  {
    ATH_CHECK(m_OverlapRemovalMonitorTool->fillEfficiencyHistograms());
  }

  return StatusCode::SUCCESS;
}
