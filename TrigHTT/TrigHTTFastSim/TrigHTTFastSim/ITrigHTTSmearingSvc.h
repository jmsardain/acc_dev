
#ifndef ITRIGHTTSMEARINGSVC_H
#define ITRIGHTTSMEARINGSVC_H

#include "GaudiKernel/IInterface.h"

#include "xAODTracking/TrackParticle.h"

class ITrigHTTSmearingSvc: virtual public IService{

 public:
  
  // Interface
  static const InterfaceID& interfaceID();

  // Methods
  virtual void recoHTT(const xAOD::TrackParticle&, bool&) = 0;
  virtual void getHTTtrack(const xAOD::TrackParticle&, xAOD::TrackParticle&) = 0;

};

inline const InterfaceID& ITrigHTTSmearingSvc::interfaceID(){
  static const InterfaceID IID("ITrigHTTSmearingSvc",1,0);
  return IID;
}


#endif   // ITRIGHTTSMEARINGVC_H
