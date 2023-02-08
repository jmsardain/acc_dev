#include "TrigHTTInput/HTTRawHitsWrapperAlg.h"
#include "TrigHTTObjects/HTTEventInputHeader.h"
#include "TrigHTTObjects/HTTTowerInputHeader.h"

#include "TFile.h"
#include "TTree.h"
#include "TH2F.h"
#include "TObjectTable.h"



HTTRawHitsWrapperAlg::HTTRawHitsWrapperAlg (const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_readOutputTool("HTTSGToRawHitsTool/HTTSGToRawHitsTool"),
  m_writeOutputTool("HTTInputHeaderTool/HTTInputHeaderTool")
{
  declareProperty("InputTool",    m_readOutputTool);
  declareProperty("OutputTool",   m_writeOutputTool);
}
 

StatusCode HTTRawHitsWrapperAlg::initialize()
{
  ATH_MSG_INFO ( "HTTRawHitsWrapperAlg::initialize()");
  ATH_CHECK(m_readOutputTool.retrieve());
  ATH_CHECK(m_writeOutputTool.retrieve());

  m_tot_hits       =0;
  m_tot_truth      =0;
  m_tot_oftracks   =0;
  
  // eventually add some histograms for monitoring
  //ATH_CHECK(BookHistograms());
  ATH_MSG_DEBUG ( "Finished initialize");
  return StatusCode::SUCCESS;
}

StatusCode HTTRawHitsWrapperAlg::BookHistograms(){
  return StatusCode::SUCCESS;
}

StatusCode HTTRawHitsWrapperAlg::execute() {
  ATH_MSG_DEBUG ("Running on event ");   
  HTTEventInputHeader  m_eventHeader;
  bool last=false;
  ATH_CHECK(m_readOutputTool->readData(&m_eventHeader, last));
   if (last) return StatusCode::SUCCESS;
   
  ATH_MSG_DEBUG (m_eventHeader);

  m_tot_hits       +=m_eventHeader.nHits();
  m_tot_truth      +=m_eventHeader.optional().nTruthTracks();
  m_tot_oftracks   +=m_eventHeader.optional().nOfflineTracks();

 ATH_CHECK (m_writeOutputTool->writeData(&m_eventHeader) );
 
  //gObjectTable->Print();// write down  statistcs of memory leak
  return StatusCode::SUCCESS;
}


StatusCode HTTRawHitsWrapperAlg::finalize()
{ 
  ATH_MSG_INFO("Finalized: n.hits ="<<m_tot_hits
              <<" n.truth="<<m_tot_truth
              <<" n.offline tracks="<<m_tot_oftracks);

  return StatusCode::SUCCESS;
}
