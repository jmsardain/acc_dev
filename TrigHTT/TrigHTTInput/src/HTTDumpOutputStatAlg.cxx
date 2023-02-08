#include "HTTDumpOutputStatAlg.h"

#include "TrigHTTObjects/HTTLogicalEventInputHeader.h"
#include "TrigHTTObjects/HTTLogicalEventOutputHeader.h"
#include "TrigHTTObjects/HTTTrack.h"

#include "TFile.h"
#include "TTree.h"
#include "TH2F.h"


HTTDumpOutputStatAlg::HTTDumpOutputStatAlg (const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_readOutputTool("HTTOutputHeaderTool/HTTOutputHeaderTool"),
  m_writeOutputTool("HTTOutputHeaderTool/HTTOutputHeaderTool")
{
  declareProperty("InputTool",    m_readOutputTool);
  declareProperty("OutputTool",   m_writeOutputTool);
}


StatusCode HTTDumpOutputStatAlg::initialize()
{
  ATH_MSG_INFO ( "HTTDumpOutputStatAlg::initialize()");  
  ATH_CHECK( m_readOutputTool.retrieve());
  ATH_CHECK( m_writeOutputTool.retrieve());

  // eventually add some histograms for monitoring
  ATH_CHECK( BookHistograms());
  
   // TO DO: must register the histograms to the TFile in the Tool
  ATH_MSG_DEBUG ( "Finished initialize");
  return StatusCode::SUCCESS;
}


StatusCode HTTDumpOutputStatAlg::BookHistograms(){
  h_hits_r_vs_z = new TH2F("h_hits_r_vs_z", "r/z ITK hit map; z[mm];r[mm]", 3500, 0., 3500., 450, 0., 450.);
  return StatusCode::SUCCESS;
}


StatusCode HTTDumpOutputStatAlg::execute() {
  ATH_MSG_DEBUG ("Running on event ");   
  
  HTTLogicalEventInputHeader  m_eventInputHeader_1st;
  HTTLogicalEventInputHeader  m_eventInputHeader_2nd;
  HTTLogicalEventOutputHeader m_eventOutputHeader;

  bool last=false;
  ATH_CHECK (m_readOutputTool->readData(&m_eventInputHeader_1st, &m_eventInputHeader_2nd, &m_eventOutputHeader, last));
   if (last) return StatusCode::SUCCESS;

  //ATH_MSG_DEBUG (m_eventInputHeader);
  //ATH_MSG_DEBUG (m_eventOutputHeader);

  // fill histograms
  for (auto track : m_eventOutputHeader.getHTTTracks_1st()) {
    ATH_MSG_DEBUG (track);
    //float r= std::sqrt(hit.getX()*hit.getX() + hit.getY()*hit.getY());
    //h_hits_r_vs_z->Fill(hit.getZ(), r);
  }

  for (auto track : m_eventOutputHeader.getHTTTracks_2nd()) {
    ATH_MSG_DEBUG (track);
  }

  ATH_CHECK (m_writeOutputTool->writeData(&m_eventInputHeader_1st, &m_eventInputHeader_2nd, &m_eventOutputHeader) );

  return StatusCode::SUCCESS;
}


StatusCode HTTDumpOutputStatAlg::finalize()
{
  return StatusCode::SUCCESS;
}
