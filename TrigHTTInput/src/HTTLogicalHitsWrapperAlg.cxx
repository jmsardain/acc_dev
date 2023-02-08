#include "TrigHTTInput/HTTLogicalHitsWrapperAlg.h"
#include "TrigHTTObjects/HTTEventInputHeader.h"
#include "TrigHTTObjects/HTTLogicalEventInputHeader.h"
#include "TrigHTTObjects/HTTLogicalEventOutputHeader.h"
#include "TrigHTTObjects/HTTTowerInputHeader.h"
#include "TrigHTTObjects/HTTCluster.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"

HTTLogicalHitsWrapperAlg::HTTLogicalHitsWrapperAlg (const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_hitInputTool("HTTSGToRawHitsTool/HTTSGToRawHitsTool"),
  m_writeOutputTool("HTTOutputHeaderTool/HTTOutputHeaderTool"),
  m_clusteringTool("HTTClusteringTool/HTTClusteringTool"),
  m_hitMapTool("HTT_RawToLogicalHitsTool/HTT_RawToLogicalHitsTool")
{
  declareProperty("InputTool",            m_hitInputTool);
  declareProperty("OutputTool",           m_writeOutputTool);
  declareProperty("RawToLogicalHitsTool", m_hitMapTool);
  declareProperty("Clustering",          m_Clustering=false, "flag to enable the clustering");
  declareProperty("Monitor",             m_Monitor=false, "flag to enable the monitor");
}


StatusCode HTTLogicalHitsWrapperAlg::initialize()
{
  ATH_MSG_INFO ( "HTTLogicalHitsWrapperAlg::initialize()");
  ATH_CHECK( m_hitInputTool.retrieve());
  ATH_CHECK( m_writeOutputTool.retrieve());
  ATH_CHECK( m_hitMapTool.retrieve());
  
  if (m_Clustering ) {
    ATH_CHECK( m_clusteringTool.retrieve());
    ATH_MSG_INFO ("Clustering is enabled");
  }

  if (m_Monitor)  ATH_CHECK( BookHistograms());
  

  ATH_MSG_DEBUG ("Finished initialize");
  return StatusCode::SUCCESS;
}

StatusCode HTTLogicalHitsWrapperAlg::BookHistograms(){

  h_hits_r_vs_z = new TH2F("h_hits_r_vs_z", "r/z ITK hit map; z[mm];r[mm]", 3500, 0., 3500., 1400, 0., 1400.);
  h_hits_barrelEC = new TH1F("h_hits_barrelEC", "barrelEC of hits", 10, 0., 10.);

  h_nMissedHit = new TH1F("h_nMissedHit", "n. of missing hits/event", 100, 0.,1000000.);
  h_nMissedHitFrac = new TH1F("h_nMissedHitFrac", "Fraction of missing hits/event", 100, 0.,1.);
  h_MissedHit_eta = new TH1F("h_MissedHit_eta", "eta of missing hits", 100, -10., 10.);
  h_MissedHit_phi = new TH1F("h_MissedHit_phi", "phi of missing hits", 100, 0., 100.);
  h_MissedHit_type = new TH1F("h_MissedHit_type", "type of missing hits", 10, 0., 10.);
  h_MissedHit_barrelEC = new TH1F("h_MissedHit_barrelEC", "barrelEC of missing hits", 10, 0., 10.);
  h_MissedHit_layer = new TH1F("h_MissedHit_layer", "layer of missing hits", 20, 0., 20.);

  return StatusCode::SUCCESS;
}


StatusCode HTTLogicalHitsWrapperAlg::execute()
{
  HTTEventInputHeader         m_eventHeader;
  HTTLogicalEventInputHeader  m_logicEventHeader_1st;
  HTTLogicalEventInputHeader  m_logicEventHeader_2nd;
  HTTLogicalEventOutputHeader m_logicEventOutputHeader;

  bool last = false;
  ATH_CHECK (m_hitInputTool->readData(&m_eventHeader, last));
  if (last) return StatusCode::SUCCESS;

  ATH_MSG_DEBUG (m_eventHeader);
    // ATH_MSG_DEBUG ("Header Event: "<< m_eventHeader->event());
  // ATH_MSG_DEBUG ("Found hits size ="<<m_eventHeader->nHits());

  // fill histograms
  for (auto hit: m_eventHeader.hits()) {
    float r= std::sqrt(hit.getX()*hit.getX() + hit.getY()*hit.getY());
    h_hits_barrelEC->  Fill(static_cast<int>(hit.getDetectorZone()));
    h_hits_r_vs_z->    Fill(hit.getZ(), r);
  }

  // Map hits:
  ATH_CHECK(m_hitMapTool->convert(1, m_eventHeader, m_logicEventHeader_1st));

  // clustering:
  std::vector<HTTHit> fulllist= m_eventHeader.hits();
  std::vector<HTTCluster> clusters;
  if (m_Clustering) {
    StatusCode sc = m_clusteringTool->DoClustering(m_logicEventHeader_1st, clusters);
    if (sc.isFailure()) {
      return sc;
    }
  }

  ATH_MSG_INFO ("Ending with " << clusters.size() << " clusters");

  //  ATH_MSG_DEBUG (*m_logicEventHeader);
  //  ATH_MSG_DEBUG ("LogicalHeader Event: "<< m_logicEventHeader->event());
  ATH_MSG_DEBUG (m_logicEventHeader_1st);

  // get unmapped hits for debuggin purpose
  if (m_Monitor) {
    ATH_MSG_DEBUG ("Filling histormas");
    std::vector<HTTHit> missing_hits;
    if (m_hitMapTool->getUnmapped(missing_hits).isFailure() ) {
      return StatusCode::FAILURE;
    }

    h_nMissedHit->Fill(missing_hits.size());
    h_nMissedHitFrac->Fill(float(missing_hits.size())/ float(fulllist.size()));
    for (auto hit: missing_hits){
      h_MissedHit_eta->     Fill(hit.getHTTEtaModule()) ;
      h_MissedHit_phi->     Fill(hit.getPhiModule());
      h_MissedHit_type->    Fill(static_cast<int>(hit.getDetType()));
      h_MissedHit_barrelEC->Fill(static_cast<int>(hit.getDetectorZone()));
      h_MissedHit_layer->   Fill(hit.getLayer());
    }
  }

  ATH_CHECK (m_writeOutputTool->writeData(&m_logicEventHeader_1st, &m_logicEventHeader_2nd, &m_logicEventOutputHeader));

  return StatusCode::SUCCESS;
}


StatusCode HTTLogicalHitsWrapperAlg::finalize()
{
  return StatusCode::SUCCESS;
}
