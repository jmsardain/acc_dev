#ifndef HTTFastSim_h
#define HTTFastSim_h

//#include <AnaAlgorithm/AnaAlgorithm.h>
//#include "GaudiKernel/IAlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

#include "TrigHTTFastSim/ITrigHTTSmearingSvc.h"

#include "TH1.h"
#include "TTree.h"
#include <vector>

class HTTFastSim : public AthAlgorithm
{
 public:
  HTTFastSim(const std::string& name, ISvcLocator* pSvcLocator);

  virtual StatusCode initialize() override;
  virtual StatusCode execute () override;
  virtual StatusCode finalize () override;

 private:
  ServiceHandle<ITrigHTTSmearingSvc> m_smearSvc;

};

#endif
