#include "TrigHTTInput/HTTReadLogicalHitsAlg.h"
#include "TrigHTTObjects/HTTTowerInputHeader.h"
#include "TrigHTTObjects/HTTLogicalEventInputHeader.h"
#include "TrigHTTObjects/HTTLogicalEventOutputHeader.h"


#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"

HTTReadLogicalHitsAlg::HTTReadLogicalHitsAlg (const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_readOutputTool("HTTOutputHeaderTool/HTTOutputHeaderTool")
{
  declareProperty("InputTool",    m_readOutputTool);
}


StatusCode HTTReadLogicalHitsAlg::initialize()
{
  ATH_MSG_INFO ( "HTTReadLogicalHitsAlg::initialize()");
  ATH_CHECK( m_readOutputTool.retrieve());
  m_event=0;
 
  ATH_MSG_INFO ( "End of initialize");
  return StatusCode::SUCCESS;
}


StatusCode HTTReadLogicalHitsAlg::execute() 
{
  HTTLogicalEventInputHeader  m_eventInputHeader_1st;
  HTTLogicalEventInputHeader  m_eventInputHeader_2nd;
  HTTLogicalEventOutputHeader m_eventOutputHeader;
  bool last = false;
  ATH_CHECK(m_readOutputTool->readData(&m_eventInputHeader_1st, &m_eventInputHeader_2nd, &m_eventOutputHeader, last) );
  if (last) return StatusCode::SUCCESS;
  ATH_MSG_DEBUG (m_eventInputHeader_1st);
  m_event++;

  return StatusCode::SUCCESS;
}


StatusCode HTTReadLogicalHitsAlg::finalize()
{
 ATH_MSG_INFO ( "finalized");
 return StatusCode::SUCCESS;
}
