#include "GaudiKernel/ITHistSvc.h"
#include <AsgTools/MessageCheck.h>
#include "TrigHTTFastSim/HTTFastSim.h"
#include <xAODCore/AuxContainerBase.h>
// Tracking
#include <xAODTracking/TrackParticleContainer.h>
#include <xAODTracking/TrackParticleAuxContainer.h>
#include <xAODTracking/TrackParticle.h>
// Event Info
#include <xAODEventInfo/EventInfo.h>

#include <xAODCore/ShallowCopy.h>

// ROOT
#include <TH1.h>

HTTFastSim :: HTTFastSim(const std::string& name, ISvcLocator *pSvcLocator)
  : AthAlgorithm (name, pSvcLocator),
    m_smearSvc("SmearingSvc_TDR",name)
{
  declareProperty("SmearSvc",m_smearSvc);

} 

StatusCode HTTFastSim :: initialize()
{

  ATH_MSG_INFO("HTTFastSim::initialize()");

  return StatusCode::SUCCESS;

}

StatusCode HTTFastSim :: execute()
{

  ATH_MSG_INFO("HTTFastSim::execute()");

  // Retrieve the eventInfo object from the event stor
  const xAOD::EventInfo *eventInfo = nullptr;
  ATH_CHECK(evtStore()->retrieve(eventInfo,"EventInfo"));

  ATH_MSG_INFO("in execute, runNumber="<<eventInfo->runNumber()<<",eventNumber="<<eventInfo->eventNumber());  

  // Retrieve track container (InDetTrackParticles)
  const xAOD::TrackParticleContainer *tracks = nullptr;
  ATH_CHECK(evtStore()->retrieve(tracks,"InDetTrackParticles"));

  xAOD::TrackParticleContainer* offTracks = new xAOD::TrackParticleContainer;
  xAOD::TrackParticleAuxContainer* offTracksAux = new xAOD::TrackParticleAuxContainer;
  offTracks->setStore( offTracksAux ); //gives it a new associated aux container

  ATH_CHECK( evtStore()->record(offTracks,"OffTracks") );
  ATH_CHECK( evtStore()->record(offTracksAux,"OffTracksAux.") );

  xAOD::TrackParticleContainer* HTTtracks = new xAOD::TrackParticleContainer;
  xAOD::TrackParticleAuxContainer* HTTtracksAux = new xAOD::TrackParticleAuxContainer;
  HTTtracks->setStore( HTTtracksAux ); //gives it a new associated aux container

  ATH_CHECK( evtStore()->record(HTTtracks,"httTracks") );
  ATH_CHECK( evtStore()->record(HTTtracksAux,"httTracksAux.") );

  // Loop over track container
  for(const xAOD::TrackParticle *track : *tracks){ 

    xAOD::TrackParticle *OffTrack = new xAOD::TrackParticle();
    offTracks->push_back(OffTrack);
    *OffTrack = *track;

    // Check if the track would be reconstructed by HTT
    bool testTrack = false;
    m_smearSvc->recoHTT(*track,testTrack);

    // If track is reconstructed by HTT smear the track and add it to the output xAOD collection
    if(testTrack){

      xAOD::TrackParticle *HTTtrack = new xAOD::TrackParticle();

      m_smearSvc->getHTTtrack(*track,*HTTtrack);

      HTTtracks->push_back(HTTtrack);
      
    }

  }

  // Retrieve HTT track container (InDetTrackParticles)
  const xAOD::TrackParticleContainer *test = nullptr;
  ATH_CHECK(evtStore()->retrieve(test,"httTracks"));

  return StatusCode::SUCCESS;

}

StatusCode HTTFastSim :: finalize()
{

  ATH_MSG_INFO("HTTFastSim::finalize()");

  return StatusCode::SUCCESS;

}
