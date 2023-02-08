#ifndef HTT_READLOGICALHITS_H
#define HTT_READLOGICALHITS_H


#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigHTTInput/HTTEventOutputHeaderToolI.h"
#include "HTT_RawToLogicalHitsTool.h"

class TFile;
class TTree;
class TH1F;

class HTTLogicalEventInputHeader;

class HTTReadLogicalHitsAlg : public AthAlgorithm {
public:
  HTTReadLogicalHitsAlg (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~HTTReadLogicalHitsAlg () {};
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:

  ToolHandle<HTTEventOutputHeaderToolI>    m_readOutputTool;
  
  unsigned int m_event;
  unsigned int m_totevent;
  
};

#endif // HTTREADLOGICALHITS_h
