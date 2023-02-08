
#ifndef TRIGHTTSMEARINGSVC_TDR_H
#define TRIGHTTSMEARINGSVC_TDR_H

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IInterface.h" 
#include "TrigHTTFastSim/ITrigHTTSmearingSvc.h"

#include "AthenaKernel/IAtRndmGenSvc.h"

// xAOD
#include "xAODTracking/TrackParticle.h"

// ROOT
#include "TF1.h"
#include "TF2.h"

class ISvcLocator;

namespace CLHEP {
  class HepRandomEngine;
}

class TrigHTTSmearingSvc_TDR: virtual public ITrigHTTSmearingSvc, public AthService
{
 public:
  
  TrigHTTSmearingSvc_TDR(const std::string& name, ISvcLocator* svc);
  
  virtual ~TrigHTTSmearingSvc_TDR() {};

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  static const InterfaceID& interfaceID();
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIf); 

  virtual void recoHTT(const xAOD::TrackParticle&, bool&) override;
  virtual void getHTTtrack(const xAOD::TrackParticle&, xAOD::TrackParticle&) override;

 private:
  
  ServiceHandle<IAtRndmGenSvc> m_rndmSvc;
  CLHEP::HepRandomEngine* m_randomEngine = nullptr;
  std::string m_randomStreamName;

};

inline const InterfaceID& TrigHTTSmearingSvc_TDR::interfaceID()
{
  static const InterfaceID IID_TrigHTTSmearingSvc_TDR("TrigHTTSmearingSvc_TDR",1,0); 
  return IID_TrigHTTSmearingSvc_TDR;
}

#endif 
