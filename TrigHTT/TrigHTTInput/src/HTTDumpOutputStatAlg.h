/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef HTT_DUMPOUTPUTSTATALG_H
#define HTT_DUMPOUTPUTSTATALG_H


#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrigHTTInput/HTTEventOutputHeaderToolI.h"

class TFile;
class TTree;
class TH2F;
class HTTLogicalEventInputHeader;
class HTTLogicalEventOutputHeader;

class HTTDumpOutputStatAlg : public AthAlgorithm {
public:
  HTTDumpOutputStatAlg (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~HTTDumpOutputStatAlg () {};
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  StatusCode BookHistograms();


private:
  ToolHandle<HTTEventOutputHeaderToolI>    m_readOutputTool;
  ToolHandle<HTTEventOutputHeaderToolI>    m_writeOutputTool;

  // histograms
  TH2F*   h_hits_r_vs_z;
};

#endif // HTT_DUMPOUTPUTSTATALG_H
