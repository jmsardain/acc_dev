// TrigHTT
#include "TrigHTTSmearingSvc_TDR.h"
#include "TrigHTTFastSim/HTTSmearingFunctionsTDRold2017.h"

#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandGauss.h"

#include <AsgTools/MessageCheck.h>

// xAOD
#include <xAODCore/AuxContainerBase.h>
#include <xAODTracking/TrackParticleContainer.h>
#include <xAODTracking/TrackParticle.h>
#include <xAODEventInfo/EventInfo.h>
#include <xAODCore/ShallowCopy.h>

// ROOT
#include <TF1.h>
#include <TF2.h>
#include <TFile.h>

TrigHTTSmearingSvc_TDR::TrigHTTSmearingSvc_TDR(const std::string& name, ISvcLocator* svc):
  AthService(name, svc),
  m_rndmSvc("AtRndmGenSvc", name)
{
  
}

StatusCode TrigHTTSmearingSvc_TDR::queryInterface(const InterfaceID& riid, void** ppvIf){
  if(interfaceID() == riid){
    *ppvIf = dynamic_cast<TrigHTTSmearingSvc_TDR*>(this);
  }else if(ITrigHTTSmearingSvc::interfaceID() == riid){
    *ppvIf = dynamic_cast<ITrigHTTSmearingSvc*>(this);
  }else{
    return AthService::queryInterface(riid, ppvIf);
  }
  
  addRef();
  return StatusCode::SUCCESS;
  
}

StatusCode TrigHTTSmearingSvc_TDR::initialize(){

  ATH_MSG_INFO("TrigHTTSmearingSvc_TDR::initialize()");

  ATH_MSG_INFO("Implementing L1Track smearing functions used for the TDR. Found here https://twiki.cern.ch/twiki/bin/viewauth/Atlas/L1TrackSmearing");

  // Random number generator
  ATH_CHECK(m_rndmSvc.retrieve());
  m_randomEngine = m_rndmSvc->GetEngine(m_randomStreamName);
  if (!m_randomEngine) {
    ATH_MSG_FATAL("Failed to get random engine: " << m_randomStreamName);
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;

}

StatusCode TrigHTTSmearingSvc_TDR::finalize(){

  ATH_MSG_INFO("TrigHTTSmearingSvc_TDR::finalize()");

  return StatusCode::SUCCESS;

}

// Tells us if input offline track will be reconstructed by HTT
void TrigHTTSmearingSvc_TDR::recoHTT(const xAOD::TrackParticle& original_track, bool& isValid){

  if((original_track.pt()*0.001)<4){

    isValid = false;
    
  }else{
  
    isValid = true;

  }

}

void TrigHTTSmearingSvc_TDR::getHTTtrack(const xAOD::TrackParticle& original_track, xAOD::TrackParticle& HTT_track){
  
  HTT_track = original_track;

  // Mean paramteres values (parameterized only as a function of eta)                                      
  double track_mean_ipt = L1TTmean_ript_vs_eta(original_track.eta());
  double track_mean_eta = L1TTmean_reta_vs_eta(original_track.eta());
  double track_mean_phi = L1TTmean_rphi_vs_eta(original_track.eta());
  double track_mean_d0 = L1TTmean_rd0_vs_eta(original_track.eta());
  double track_mean_z0 = L1TTmean_rzed_vs_eta(original_track.eta());
  // Resolution (parameterized as a function of pt and eta)                                                
  double track_sigma_ipt = L1TTsigma_ript(original_track.pt()*0.001,original_track.eta());
  double track_sigma_eta = L1TTsigma_reta(original_track.pt()*0.001,original_track.eta());
  double track_sigma_phi = L1TTsigma_rphi(original_track.pt()*0.001,original_track.eta());
  double track_sigma_d0 = L1TTsigma_rd0(original_track.pt()*0.001,original_track.eta());
  double track_sigma_z0 = L1TTsigma_rzed(original_track.pt()*0.001,original_track.eta());
  
  double track_pt_smear = 1/(CLHEP::RandGauss::shoot(m_randomEngine,track_mean_ipt,track_sigma_ipt)+1/(original_track.pt()));
  double track_eta_smear = original_track.eta() + CLHEP::RandGauss::shoot(m_randomEngine,track_mean_eta,track_sigma_eta);
  double track_phi_smear = original_track.phi() + CLHEP::RandGauss::shoot(m_randomEngine,track_mean_phi,track_sigma_phi);
  double track_d0_smear = original_track.d0() + CLHEP::RandGauss::shoot(m_randomEngine,track_mean_d0,track_sigma_d0);
  double track_z0_smear = original_track.z0() + CLHEP::RandGauss::shoot(m_randomEngine,track_mean_z0,track_sigma_z0);
  
  double track_px_smear = track_pt_smear * TMath::Cos(track_phi_smear);
  double track_py_smear = track_pt_smear * TMath::Sin(track_phi_smear);
  
  double track_pz_smear = original_track.p4().Pz();
  double track_q_smear = original_track.charge();
  
  double  track_p_smear = TMath::Sqrt(TMath::Power(track_px_smear,2)+TMath::Power(track_py_smear,2)+TMath::Power(track_pz_smear,2));
  
  HTT_track.setDefiningParameters(track_d0_smear,track_z0_smear,track_phi_smear,track_eta_smear,track_q_smear/track_p_smear);
    
}
