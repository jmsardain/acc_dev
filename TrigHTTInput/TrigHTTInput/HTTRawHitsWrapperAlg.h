/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef HTT_RAWHITSWRAPPERALG_H
#define HTT_RAWHITSWRAPPERALG_H


#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrigHTTInput/HTTEventInputHeaderToolI.h"

class TFile;
class TTree;
class TH2F;
class HTTEventInputHeader;

class HTTRawHitsWrapperAlg : public AthAlgorithm {
public:
  HTTRawHitsWrapperAlg (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~HTTRawHitsWrapperAlg () {};
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  StatusCode BookHistograms();


private:
  // configuration parameters  
  ToolHandle<HTTEventInputHeaderToolI>    m_readOutputTool;// { this, "HitInputTool", "HTT_SGHitInput/HTT_SGHitInput", "HitInput Tool" };
  ToolHandle<HTTEventInputHeaderToolI>    m_writeOutputTool;

  // some debug counters
  unsigned int m_tot_hits=0;
  unsigned int m_tot_truth=0;
  unsigned int m_tot_oftracks=0;
};

#endif // HTTRAWHITSWRAPPERALG_h
