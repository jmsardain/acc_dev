#ifndef HTT_LOGICALHITSWRAPPERALG_H
#define HTT_LOGICALHITSWRAPPERALG_H


#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthContainers/DataVector.h"

#include "HTTEventInputHeaderToolI.h"
#include "HTTEventOutputHeaderToolI.h"
#include "TrigHTTMaps/HTTClusteringToolI.h"
#include "HTT_RawToLogicalHitsTool.h"


class TFile;
class TTree;
class TH1F;
class TH2F;
class HTTEventInputHeader;
class HTTLogicalEventInputHeader;


class HTTLogicalHitsWrapperAlg : public AthAlgorithm {
public:
  HTTLogicalHitsWrapperAlg (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~HTTLogicalHitsWrapperAlg () {};
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  StatusCode BookHistograms();


private:
  // configuration parameters
  ToolHandle<HTTEventInputHeaderToolI>     m_hitInputTool;// { this, "HitInputTool", "HTT_SGHitInput/HTT_SGHitInput", "HitInput Tool" };
  ToolHandle<HTTEventOutputHeaderToolI>    m_writeOutputTool;
  ToolHandle<HTTClusteringToolI>           m_clusteringTool;// { this, "HTTClusteringFTKTool", "HTTClusteringFTKTool/HTTClusteringFTKTool", "FTKClusteringTool" };
  ToolHandle<HTT_RawToLogicalHitsTool>     m_hitMapTool;// { this, "MapTool", "HTT_SGHitInput/HTT_SGHitInput", "HitInput Tool" };
  //StringProperty  m_outpath ;//    {this, "OutFileName", "httsim_smartwrapper.root", "output path"};
  BooleanProperty m_Clustering;// this may become a tool
  BooleanProperty m_Monitor;// this may become a tool

  // internal variables
  int m_ntowers;


  // histograms
  TH1F*   h_hits_barrelEC;
  TH2F*   h_hits_r_vs_z;

  TH1F*   h_nMissedHit;
  TH1F*   h_nMissedHitFrac;
  TH1F*   h_MissedHit_eta ;
  TH1F*   h_MissedHit_phi;
  TH1F*   h_MissedHit_type;
  TH1F*   h_MissedHit_barrelEC;
  TH1F*   h_MissedHit_layer;

};

#endif // HTTSGRORAWHITSWRAPPERALG_h
