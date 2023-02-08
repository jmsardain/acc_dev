/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTInput/HTTSGToRawHitsTool.h"
#include "TrigHTTInput/TrigHTTInputUtils.h"


#include "TrigHTTObjects/HTTEventInputHeader.h"
#include "TrigHTTObjects/HTTEventInfo.h"
#include "TrigHTTObjects/HTTOfflineTrack.h"
#include "TrigHTTObjects/HTTOfflineHit.h"
#include "TrigHTTObjects/HTTTruthTrack.h"
#include "TrigHTTConfig/IHTTEventSelectionSvc.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/TriggerInfo.h"
#include "EventInfo/EventID.h"

#include "IdDictDetDescr/IdDictManager.h"
#include "InDetPrepRawData/SiClusterContainer.h"
#include "InDetPrepRawData/SiClusterCollection.h"
#include "InDetRawData/InDetRawDataCollection.h"
#include "InDetRawData/InDetRawDataContainer.h"
#include "InDetRawData/InDetRawDataCLASS_DEF.h"
#include "InDetSimData/InDetSimDataCollection.h"
#include "InDetSimData/SCT_SimHelper.h"
#include "InDetSimData/PixelSimHelper.h"
#include "InDetReadoutGeometry/SiDetectorDesign.h"

////////////////////////////////////////////////////////////#include "InDetReadoutGeometry/SiCellId.h"
#include "ReadoutGeometryBase/SiCellId.h"

////////////////////////////////////////////////////////////#include "InDetReadoutGeometry/SiReadoutCellId.h"
#include "ReadoutGeometryBase/SiReadoutCellId.h"


#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"
#include "GeneratorObjects/McEventCollection.h"
#include "GeneratorObjects/xAODTruthParticleLink.h"

#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"


HTTSGToRawHitsTool::HTTSGToRawHitsTool(const std::string& algname, const std::string &name, const IInterface *ifc) :
  base_class(algname,name,ifc),
  m_truthToTrack( "Trk::TruthToTrack/InDetTruthToTrack" ),
  m_extrapolator( "Trk::Extrapolator/AtlasExtrapolator" ),
  m_beamSpotSvc( "BeamCondSvc" , name ),
  m_EvtSel("HTTEventSelectionSvc", name),
  m_maxEta(3.3),
  m_minPt(.8*CLHEP::GeV),
  m_idHelper(0)
{
  declareProperty("maxEta",                   m_maxEta);
  declareProperty("minPt",                    m_minPt);
  declareProperty("pixelClustersName",        m_pixelClustersName="PixelClusters", "name of pixel collection");
  declareProperty("SCT_ClustersName",         m_sctClustersName=  "SCT_Clusters", "name of SCT collection");
  //  declareProperty("tracksTruthName",          m_tracksTruthName= "Trk::TruthToTrack/InDetTruthToTrack", "name of truth track colleciotn");
  declareProperty("OfflineName",              m_offlineName = "InDetTrackParticles", "name of offline tracks collection");
  declareProperty("dumpHitsOnTracks",         m_dumpHitsOnTracks=false, "dump hits of tracks if true");
  declareProperty("dumpTruthIntersections",   m_dumpTruthIntersections=false, "dump truth intersection if true");
  declareProperty("filterHits",               m_filterHits=false, "filter hits based on region");
  declareProperty("TruthToTrackTool"        , m_truthToTrack);
  declareProperty("Extrapolator"            , m_extrapolator);
  declareProperty("BeamCondSvc"             , m_beamSpotSvc);
  //  declareProperty("useOfflineTrackSelectorTool" , m_useOfflineTrackSelectorTool);
  declareProperty("ReadOfflineClusters",  m_readOfflineClusters=true, "flag to enable the offline cluster save");
  declareProperty("ReadTruthTracks",  m_readTruthTracks=true, "flag to enable the truth tracking save");
  declareProperty("ReadOfflineTracks",m_readOfflineTracks = true, "flag to enable the offline tracking save");
  declareProperty("UseNominalOrigin", m_UseNominalOrigin=false);
}




// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTSGToRawHitsTool::initialize(){

  ATH_MSG_DEBUG ("HTTSGToRawHitsTool::initialize()");

  ATH_CHECK(service("StoreGateSvc", m_storeGate));

  ATH_CHECK(m_truthToTrack.retrieve());
  ATH_MSG_INFO ( m_truthToTrack << " retrieved" );

  ATH_CHECK (m_extrapolator.retrieve());
  ATH_MSG_INFO (m_extrapolator << " retrieved");

  ATH_CHECK (m_beamSpotSvc.retrieve());
  ATH_MSG_INFO ( m_beamSpotSvc << " retrieved");

  ATH_CHECK(m_EvtSel.retrieve());
  ATH_MSG_INFO ( m_EvtSel << " retrieved: eta=("<<m_EvtSel->getMin().eta <<","<< m_EvtSel->getMax().eta
                          <<"),   phi=("<<m_EvtSel->getMin().phi <<","<< m_EvtSel->getMax().phi<<")");


  //m_minEtaHit=m_EvtSel->getMin().eta - 0.1;
  //m_maxEtaHit=m_EvtSel->getMax().eta + 0.1;
  m_minEtaHit=-4.;
  m_maxEtaHit=4.;
  m_minPhiHit=m_EvtSel->getMin().phi - 0.2;
  m_maxPhiHit=m_EvtSel->getMax().phi + 0.2;

  if (m_filterHits)
    ATH_MSG_INFO ("Selecting hits in eta ("<< m_minEtaHit<<","<< m_maxEtaHit
                             <<"), phi ("<< m_minPhiHit <<","<< m_maxPhiHit<<")"); 

  IPartPropSvc* partPropSvc = 0;
  ATH_CHECK(service("PartPropSvc", partPropSvc));
  m_particleDataTable = partPropSvc->PDT();
  ATH_MSG_INFO ("PartPropSvc retrieved and filled particleDataTable");

  const IdDictManager* idDictMgr( 0 );
  ATH_CHECK( detStore()->retrieve(idDictMgr, "IdDict") );

  // ID helpers
  //m_idHelper = new AtlasDetectorID;
  // why this is crashing?
  ATH_CHECK(detStore()->retrieve(m_idHelper, "AtlasID"));
  /*if (idDictMgr->initializeHelper(*m_idHelper)) {
    ATH_MSG_ERROR ("Unable to initialize ID IdHelper" );
    return StatusCode::FAILURE;
    }*/

  ATH_MSG_INFO (" m_idHelper initialized");

  ATH_CHECK( detStore()->retrieve(m_PIX_mgr, "Pixel") );
  ATH_CHECK( detStore()->retrieve(m_pixelId, "PixelID") );
  ATH_CHECK( detStore()->retrieve(m_SCT_mgr, "SCT") );
  ATH_CHECK( detStore()->retrieve(m_sctId,   "SCT_ID") );


  m_eventHeader = new HTTEventInputHeader();
  m_event=0;
  ATH_MSG_INFO ( "End of Tool initialize");
  return StatusCode::SUCCESS;
}


StatusCode HTTSGToRawHitsTool::finalize(){
  ATH_MSG_INFO ("finalized: processed "<<m_event<<" events");
  //if( m_idHelper ) { delete m_idHelper; }
  return StatusCode::SUCCESS;
}

//NoTHING TO DO
StatusCode HTTSGToRawHitsTool::writeData(HTTEventInputHeader* header)  {
  m_eventHeader= &*header;//copy object
  return StatusCode::SUCCESS;
}

/** This function get from the SG the inner detector raw hits
  and prepares them for HTT simulation */
StatusCode HTTSGToRawHitsTool::readData(HTTEventInputHeader* header, bool &last)
{
  last=false;
  const EventInfo* eventInfo(0);
  ATH_CHECK(m_storeGate->retrieve(eventInfo));

  const EventID* eventID(eventInfo->event_ID());
  ATH_MSG_INFO ("entered execution for run " << eventID->run_number() << "   event " << eventID->event_number());

  //Filled to variable / start event
  HTTEventInfo event_info;
  event_info.setRunNumber(eventID->run_number());
  event_info.setEventNumber(eventID->event_number());
  event_info.setLB(eventID->lumi_block());
  event_info.setBCID(eventID->bunch_crossing_id());
  event_info.setaverageInteractionsPerCrossing(eventInfo->averageInteractionsPerCrossing());
  event_info.setactualInteractionsPerCrossing(eventInfo->actualInteractionsPerCrossing());

  const TriggerInfo* triggerInfo(eventInfo->trigger_info());
  event_info.setextendedLevel1ID(triggerInfo->extendedLevel1ID() );
  event_info.setlevel1TriggerType(triggerInfo->level1TriggerType () );
  event_info.setlevel1TriggerInfo(triggerInfo->level1TriggerInfo ());

  m_eventHeader->newEvent(event_info);//this also reset all varaibles in the header

  // get pixel and sct cluster containers
  if( m_storeGate->retrieve(m_pixelContainer, m_pixelClustersName).isFailure() ) {
    ATH_MSG_WARNING ( "unable to retrieve the PixelCluster container " << m_pixelClustersName);
  }
  if( m_storeGate->retrieve(m_sctContainer, m_sctClustersName).isFailure() ) {
    ATH_MSG_WARNING ( "unable to retrieve the SCT_Cluster container " << m_sctClustersName);
  }

  //HitIndexMap hitIndexMap; // keep running index event-unique to each hit
  //HitIndexMap pixelClusterIndexMap;

  // dump raw silicon data
  ATH_MSG_INFO ("Dump raw silicon data");
  ATH_CHECK(read_raw_silicon());
  ATH_MSG_INFO ("Found list of hits, size = " << m_eventHeader->nHits());

  HTTOptionalEventInfo optional;

  if (m_readOfflineClusters) {
    ATH_CHECK( read_offline_clusters(optional));
    ATH_MSG_INFO ("Saved " << optional.nOfflineClusters() << " offline clusters");
  }

  if (m_readOfflineTracks){
    if (read_offline_tracks(optional) != StatusCode::SUCCESS){
      ATH_MSG_ERROR ("Error in reading offline tracks");
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO ("Saved " << optional.nOfflineTracks() << " offline tracks");
  }

  if (m_readTruthTracks) {
    if (read_truth_tracks(optional) != StatusCode::SUCCESS){
      ATH_MSG_ERROR ("Error in reading truth tracks");
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO ("Saved " << optional.nTruthTracks() << " truth tracks");
  }  

  m_eventHeader->setOptional(optional);
  ATH_MSG_DEBUG ( *m_eventHeader);

  *header= *m_eventHeader; 
  m_event++;
  ATH_MSG_DEBUG ( "End of read()");
  return StatusCode::SUCCESS;
}


StatusCode 
HTTSGToRawHitsTool::read_offline_tracks(HTTOptionalEventInfo& optional)
{
  const xAOD::TrackParticleContainer *offlineTracks = nullptr;
  if (m_storeGate->retrieve(offlineTracks, m_offlineName).isFailure()) {
    ATH_MSG_ERROR("Failed to retrieve Offline Tracks " << m_offlineName);
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO ("read Offline tracks, size = "<< offlineTracks->size());
  optional.reserveOfflineTracks(offlineTracks->size());
  auto track_it   = offlineTracks->begin();
  auto last_track = offlineTracks->end();
  trackParticleVec.clear();
  trackParticleVec.reserve(offlineTracks->size());
  
  for (int iTrk = 0 ; track_it != last_track; track_it++, iTrk++){
    const xAOD::TrackParticle* offlineTrk = *track_it;
    const Trk::Track* track = offlineTrk->track();
    trackParticleVec.push_back(offlineTrk);

    HTTOfflineTrack tmpOfflineTrack;
    tmpOfflineTrack.setQOverPt(offlineTrk->qOverP());
    tmpOfflineTrack.setEta(offlineTrk->eta());
    tmpOfflineTrack.setPhi(offlineTrk->phi());
    tmpOfflineTrack.setD0(offlineTrk->d0());
    tmpOfflineTrack.setZ0(offlineTrk->z0());

    // Find the corresponding truth track to store the barcode.
    const xAOD::TruthParticle* associatedTruth = nullptr;
    typedef ElementLink<xAOD::TruthParticleContainer> Link_t;
    if (offlineTrk->isAvailable<Link_t>("truthParticleLink")) {
      static SG::AuxElement::ConstAccessor<Link_t> linkAcc("truthParticleLink");
      const Link_t& link = linkAcc(*offlineTrk);
      if (link.isValid()) {
        associatedTruth = *link;
      }
    }
    if (associatedTruth) {
      tmpOfflineTrack.setBarcode(associatedTruth->barcode());
    }

    float bestMatch(std::numeric_limits<float>::quiet_NaN());
    if (offlineTrk->isAvailable<float>("truthMatchProbability")) {
      bestMatch = offlineTrk->auxdata<float>("truthMatchProbability");
    }

    if (not std::isnan(bestMatch)) tmpOfflineTrack.setBarcodeFrac(bestMatch);

    const DataVector<const Trk::TrackStateOnSurface>* trackStates = track->trackStateOnSurfaces();
    if(!trackStates)    {
      ATH_MSG_ERROR("trackStatesOnSurface troubles");
      return StatusCode::FAILURE;
    }
    auto it = trackStates->begin();
    auto it_end = trackStates->end();
    if (!(*it)) {
      ATH_MSG_WARNING("TrackStateOnSurface == Null" << std::endl);
      continue;
    }
    for (; it!=it_end; it++) {
      const Trk::TrackStateOnSurface* tsos = *it;
      if (tsos == 0) continue;
      HTTOfflineHit tmpOfflineHit;
      if (tsos->type(Trk::TrackStateOnSurface::Measurement))
        tmpOfflineHit.setHitType(OfflineHitType::Measurement);
      else if (tsos->type(Trk::TrackStateOnSurface::InertMaterial))
        tmpOfflineHit.setHitType(OfflineHitType::InertMaterial);
      else if (tsos->type(Trk::TrackStateOnSurface::BremPoint))
        tmpOfflineHit.setHitType(OfflineHitType::BremPoint);
      else if (tsos->type(Trk::TrackStateOnSurface::Perigee))
        tmpOfflineHit.setHitType(OfflineHitType::Perigee);
      else if (tsos->type(Trk::TrackStateOnSurface::Outlier))
        tmpOfflineHit.setHitType(OfflineHitType::Outlier);
      else if (tsos->type(Trk::TrackStateOnSurface::Hole))
        tmpOfflineHit.setHitType(OfflineHitType::Hole);
      else
        tmpOfflineHit.setHitType(OfflineHitType::Other);
      
      const Trk::MeasurementBase* measurement = (*it)->measurementOnTrack();
      const Trk::RIO_OnTrack* hit = dynamic_cast <const Trk::RIO_OnTrack*>(measurement);
      
      if (hit != nullptr) {
        const Identifier& hitId = hit->identify();
        if (m_idHelper->is_pixel(hitId)) {
      	 tmpOfflineHit.setIsPixel(true);
      	 tmpOfflineHit.setIsBarrel(m_pixelId->is_barrel(hitId));	  
      	 const InDetDD::SiDetectorElement* sielement = m_PIX_mgr->getDetectorElement(hitId);
      	 tmpOfflineHit.setClusterID(sielement->identifyHash());
      	 tmpOfflineHit.setTrackNumber(iTrk);
      	 tmpOfflineHit.setLayer(m_pixelId->layer_disk(hitId));
      	 tmpOfflineHit.setLocX((float)measurement->localParameters()[Trk::locX]);
      	 tmpOfflineHit.setLocY((float)measurement->localParameters()[Trk::locY]);
      	}
        else if (m_idHelper->is_sct(hitId)) {
      	 tmpOfflineHit.setIsPixel(false);
      	 tmpOfflineHit.setIsBarrel(m_sctId->is_barrel(hitId));
      	 const InDetDD::SiDetectorElement* sielement = m_SCT_mgr->getDetectorElement(hitId);
      	 tmpOfflineHit.setClusterID(sielement->identifyHash());
      	 tmpOfflineHit.setTrackNumber(iTrk);
      	 tmpOfflineHit.setLayer(m_pixelId->layer_disk(hitId));
      	 tmpOfflineHit.setLocX(((float)measurement->localParameters()[Trk::locX]));
      	 tmpOfflineHit.setLocY(-99999.9);
      	}
      }
      tmpOfflineTrack.addHit(tmpOfflineHit);
    }
    optional.addOfflineTrack(tmpOfflineTrack);
  }//end of loop over tracks

  return StatusCode::SUCCESS;
}


bool HTTSGToRawHitsTool::IsHitInRegion(Amg::Vector3D& globalPos){
  if (globalPos.eta()<=m_maxEtaHit && globalPos.eta()>=m_minEtaHit
    && globalPos.phi()<=m_maxPhiHit && globalPos.phi()>=m_minPhiHit)
    return true;

  return false;

}


// dump silicon channels with geant matching information.
StatusCode 
HTTSGToRawHitsTool::read_raw_silicon( ) // const cannot make variables push back to DataInput
{
  ATH_MSG_INFO ("read silicon hits");
  unsigned int hitIndex = 0u;
  unsigned int missedHits = 0;
  //unsigned int pixelClusterIndex=0;
  const DataHandle<PixelRDO_Container> pixel_rdocontainer_iter;
  const InDetSimDataCollection* pixelSimDataMap(0);
  const bool have_pixel_sdo = m_storeGate->retrieve(pixelSimDataMap, "PixelSDO_Map").isSuccess();
  if (!have_pixel_sdo) {
    ATH_MSG_WARNING ( "Missing Pixel SDO Map");
  }


/********** DO PIXELS **********/
  ATH_MSG_INFO("Doing Pixel  retrieveal");
  if( m_storeGate->retrieve(pixel_rdocontainer_iter, "PixelRDOs").isSuccess()  ) {
    m_eventHeader->reserveHits(pixel_rdocontainer_iter->size());
    pixel_rdocontainer_iter->clID(); // anything to dereference the DataHandle
    for (auto iColl = pixel_rdocontainer_iter->begin(), fColl = pixel_rdocontainer_iter->end(); iColl != fColl; ++iColl) {
      const InDetRawDataCollection<PixelRDORawData>* pixel_rdoCollection(*iColl);
      if( !pixel_rdoCollection ) { continue; }
      //const int size = pixel_rdoCollection->size();
      // ATH_MSG_DEBUG ( "Pixel InDetRawDataCollection found with " << size << " RDOs");
      // loop on all RDOs
      for (auto iRDO = pixel_rdoCollection->begin(), fRDO = pixel_rdoCollection->end(); iRDO!=fRDO; ++iRDO) {
        Identifier rdoId = (*iRDO)->identify();
        // get the det element from the det element collection
        const InDetDD::SiDetectorElement* sielement = m_PIX_mgr->getDetectorElement(rdoId); assert( sielement);

        Amg::Vector2D LocalPos = sielement->localPositionOfCell(rdoId);
        Amg::Vector3D globalPos = sielement->globalPosition(LocalPos);
        InDetDD::SiCellId cellID = sielement->cellIdFromIdentifier (rdoId);
        const int nCells = sielement->numberOfConnectedCells(cellID);
        
       
        // push back the hit information  to DataInput for HitList
        HTTHit tmpSGhit;
        tmpSGhit.setHitType(HitType::unmapped);
        tmpSGhit.setDetType(SiliconTech::pixel);
        tmpSGhit.setIdentifierHash(sielement->identifyHash());

        int barrel_ec = m_pixelId->barrel_ec(rdoId);
        if (barrel_ec == 0)
          tmpSGhit.setDetectorZone(DetectorZone::barrel);
        else if (barrel_ec == 2)
          tmpSGhit.setDetectorZone(DetectorZone::posEndcap);
        else if (barrel_ec == -2)
          tmpSGhit.setDetectorZone(DetectorZone::negEndcap);

        tmpSGhit.setLayerDisk( m_pixelId->layer_disk(rdoId));
        tmpSGhit.setPhiModule(m_pixelId->phi_module(rdoId));
        tmpSGhit.setEtaModule(m_pixelId->eta_module(rdoId));
        tmpSGhit.setPhiIndex(m_pixelId->phi_index(rdoId));
        tmpSGhit.setEtaIndex(m_pixelId->eta_index(rdoId));
        tmpSGhit.setEtaWidth(0);
        tmpSGhit.setPhiWidth(0);
        tmpSGhit.setX(globalPos[Amg::x]);
        tmpSGhit.setY(globalPos[Amg::y]);
        tmpSGhit.setZ(globalPos[Amg::z]);
        tmpSGhit.setToT((*iRDO)->getToT());

        if (m_filterHits){
          if (!IsHitInRegion(globalPos)){
          //if (!m_EvtSel->passCuts(tmpSGhit)) {
            //float eta = TMath::ASinH(tmpSGhit.getGCotTheta());
            //float phi = tmpSGhit.getGPhi();
            ATH_MSG_INFO ("This PIXEL hit does not pass the filter: "//eta="<<eta<<", phi="<<phi
                  <<" - global: eta="<<globalPos.eta()<<", phi="<<globalPos.phi());  
            missedHits++;        
            continue;
        }}


       

        // if there is simulation truth available, try to retrieve the "most likely" barcode for this pixel.
        const HepMC::GenParticle* best_parent = 0;
        TrigHTTInputUtils::ParentBitmask parent_mask;
        HepMcParticleLink::ExtendedBarCode best_extcode;
        if( have_pixel_sdo && pixelSimDataMap ) {
          auto iter(pixelSimDataMap->find(rdoId));
          // this might be the ganged pixel copy.
          if( nCells>1 && iter == pixelSimDataMap->end() ) {
            InDetDD::SiReadoutCellId SiRC( m_pixelId->phi_index(rdoId), m_pixelId->eta_index(rdoId) );
            for( int ii=0; ii<nCells && iter == pixelSimDataMap->end(); ++ii ) {
              iter = pixelSimDataMap->find(sielement->identifierFromCellId(sielement->design().connectedCell(SiRC,ii)));
            }
          } // end search for correct ganged pixel
          // if SDO found for this pixel, associate the particle. otherwise leave unassociated.
          if( iter != pixelSimDataMap->end() )  {
            const InDetSimData& sdo( iter->second );
            const std::vector<InDetSimData::Deposit>& deposits( sdo.getdeposits() );
            for (auto iDep=deposits.begin(), fDep=deposits.end(); iDep!=fDep; ++iDep ) {
              const HepMcParticleLink& particleLink( iDep->first );
              //const InDetSimData::Deposit::second_type qdep( iDep->second ); // energy(charge) contributed by this particle
              // RDO's without SDO's are delta rays or detector noise.
              if( !particleLink.isValid() ) { continue; }
              const HepMC::GenParticle* particle( particleLink );
              const float genEta=particle->momentum().pseudoRapidity();
              const float genPt=particle->momentum().perp(); // MeV
              // reject unstable particles
              if( particle->status()%1000!=1 ) { continue; }
              // reject secondaries and low pT (<400 MeV) pileup
              if( particle->barcode()>10000000 || particle->barcode()==0 ) { continue; }
              // reject far forward particles
              if( fabs(genEta)>m_maxEta ) { continue; }
              // "best_parent" is the highest pt particle
              if( !best_parent || best_parent->momentum().perp()<genPt ) {
                best_parent = particle;
                best_extcode = HepMcParticleLink::ExtendedBarCode( particleLink.barcode() , particleLink.eventIndex() );
              }
              parent_mask |= TrigHTTInputUtils::construct_truth_bitmap( particle );
              // check SDO
            } // end for each contributing particle
          } // end if truth found for this pixel
        } // end if pixel truth available
        


        tmpSGhit.setEventIndex((long)(best_parent ? best_extcode.eventIndex() : std::numeric_limits<long>::max()));
        tmpSGhit.setBarcode((long)(best_parent ? best_extcode.barcode() : std::numeric_limits<long>::max()));
        tmpSGhit.setBarcodePt( static_cast<unsigned long>(std::ceil(best_parent ? best_parent->momentum().perp() : 0.)) );
        tmpSGhit.setParentageMask(parent_mask.to_ulong());

        // Add truth
        HTTMultiTruth mt;
        HTTMultiTruth::Barcode uniquecode(tmpSGhit.getEventIndex(),tmpSGhit.getBarcode());
        mt.maximize(uniquecode,tmpSGhit.getBarcodePt());
        tmpSGhit.setTruth(mt);


        // finalizd hit
        m_eventHeader->addHit(tmpSGhit);
        ++hitIndex;

      } // end for each RDO in the collection
    } // for each pixel RDO collection
  } // dump raw pixel data

  const InDetSimDataCollection* sctSimDataMap(0);
  const bool have_sct_sdo = m_storeGate->retrieve(sctSimDataMap, "SCT_SDO_Map").isSuccess();
  if (!have_sct_sdo) {
    ATH_MSG_WARNING ( "Missing SCT SDO Map");
  }
  
  /****** DO STRIPS ***********/
  ATH_MSG_INFO("Doing STRIP  retrieveal");
  const DataHandle<SCT_RDO_Container> sct_rdocontainer_iter;
  if( m_storeGate->retrieve(sct_rdocontainer_iter, "SCT_RDOs").isSuccess() ) {
    sct_rdocontainer_iter->clID(); // anything to dereference the DataHandle
    for (auto iColl = sct_rdocontainer_iter->begin(), fColl = sct_rdocontainer_iter->end(); iColl!=fColl; ++iColl ) {
      const InDetRawDataCollection<SCT_RDORawData>* SCT_Collection(*iColl);
      if( !SCT_Collection ) { continue; }
      //      const int size = SCT_Collection->size();
      //      ATH_MSG_DEBUG ( "SCT InDetRawDataCollection found with " << size << " RDOs");
      for( DataVector<SCT_RDORawData>::const_iterator iRDO=SCT_Collection->begin(), fRDO=SCT_Collection->end(); iRDO!=fRDO; ++iRDO ) {
        const Identifier rdoId = (*iRDO)->identify();
        // get the det element from the det element collection
        const InDetDD::SiDetectorElement* sielement = m_SCT_mgr->getDetectorElement(rdoId);
        Amg::Vector2D LocalPos = sielement->localPositionOfCell(rdoId);
        //const Amg::Vector3D globalPos( sielement->globalPosition(LocalPos) );
        std::pair<Amg::Vector3D, Amg::Vector3D> endsOfStrip = sielement->endsOfStrip(LocalPos);
       
        HTTHit tmpSGhit;
        tmpSGhit.setHitType(HitType::unmapped);
        tmpSGhit.setDetType(SiliconTech::strip);
        tmpSGhit.setIdentifierHash(sielement->identifyHash());

        int barrel_ec = m_sctId->barrel_ec(rdoId);
        if (barrel_ec == 0)
          tmpSGhit.setDetectorZone(DetectorZone::barrel);
        else if (barrel_ec == 2)
          tmpSGhit.setDetectorZone(DetectorZone::posEndcap);
        else if (barrel_ec == -2)
          tmpSGhit.setDetectorZone(DetectorZone::negEndcap);

        tmpSGhit.setLayerDisk(m_sctId->layer_disk(rdoId));
        tmpSGhit.setPhiModule(m_sctId->phi_module(rdoId));
        tmpSGhit.setEtaModule(m_sctId->eta_module(rdoId));
        tmpSGhit.setPhiIndex(m_sctId->strip(rdoId));
        tmpSGhit.setEtaIndex(m_sctId->row(rdoId));
        tmpSGhit.setSide(m_sctId->side(rdoId));
        tmpSGhit.setEtaWidth((*iRDO)->getGroupSize());
        tmpSGhit.setPhiWidth(0);
        tmpSGhit.setX(0.5*(endsOfStrip.first.x() + endsOfStrip.second.x()));
        tmpSGhit.setY(0.5*(endsOfStrip.first.y() + endsOfStrip.second.y()));
        tmpSGhit.setZ(0.5*(endsOfStrip.first.z() + endsOfStrip.second.z()));
        
        if (m_filterHits){
          Amg::Vector3D globalPos = sielement->globalPosition(LocalPos);
          if (!IsHitInRegion(globalPos)){
         // if (!m_EvtSel->passCuts(tmpSGhit)) {
            //float eta = TMath::ASinH(tmpSGhit.getGCotTheta());
            //float phi = tmpSGhit.getGPhi(); 
            ATH_MSG_INFO ("This STRIP hit does not pass the filter: "//eta="<<eta<<", phi="<<phi
                  <<" - global: eta="<<globalPos.eta()<<", phi="<<globalPos.phi());
            missedHits++;
          continue;
          } 
        }
        
         // if there is simulation truth available, try to retrieve the
        // "most likely" barcode for this strip.
        const HepMC::GenParticle* best_parent = 0;
        TrigHTTInputUtils::ParentBitmask parent_mask;
        HepMcParticleLink::ExtendedBarCode best_extcode;
        if (have_sct_sdo && sctSimDataMap) {
          auto iter( sctSimDataMap->find(rdoId) );
          // if SDO found for this pixel, associate the particle
          if( iter!=sctSimDataMap->end() )  {
            const InDetSimData& sdo( iter->second );
            const std::vector<InDetSimData::Deposit>& deposits( sdo.getdeposits() );
            for (auto iDep=deposits.begin(), fDep=deposits.end(); iDep!=fDep; ++iDep) {
              const HepMcParticleLink& particleLink( iDep->first );
              // const InDetSimData::Deposit::second_type qdep( iDep->second ); // energy(charge) contributed by this particle
              // RDO's without SDO's are delta rays or detector noise.
              if( !particleLink.isValid() ) { continue; }
              const HepMC::GenParticle* particle( particleLink );
              const float genEta=particle->momentum().pseudoRapidity();
              const float genPt=particle->momentum().perp(); // MeV
              // reject unstable particles
              if( particle->status()%1000!=1 ) { continue; }
              // reject secondaries and low pt (<400 MeV) pileup truth
              if( particle->barcode()>10000000 || particle->barcode()==0 ) { continue; }
              // reject far forward particles
              if( fabs(genEta)>m_maxEta ) { continue; }
              // "best_parent" is the highest pt particle
              if( !best_parent || best_parent->momentum().perp()<genPt ) {
                best_parent = particle;
                best_extcode = HepMcParticleLink::ExtendedBarCode( particleLink.barcode() , particleLink.eventIndex() );
              }
              parent_mask |= TrigHTTInputUtils::construct_truth_bitmap( particle );
            } // end for each contributing particle
          } // end if truth found for this strip
        } // end if sct truth available
        // push back the hit information  to DataInput for HitList , copy from RawInput.cxx


        tmpSGhit.setEventIndex((long)(best_parent ? best_extcode.eventIndex() : std::numeric_limits<long>::max()));
        tmpSGhit.setBarcode((long)(best_parent ? best_extcode.barcode(): std::numeric_limits<long>::max()));
        tmpSGhit.setBarcodePt( static_cast<unsigned long>(std::ceil(best_parent ? best_parent->momentum().perp() : 0.)) );
        tmpSGhit.setParentageMask(parent_mask.to_ulong());
        // Add truth
        HTTMultiTruth mt;
        HTTMultiTruth::Barcode uniquecode(tmpSGhit.getEventIndex(),tmpSGhit.getBarcode());
        mt.maximize(uniquecode,tmpSGhit.getBarcodePt());
        tmpSGhit.setTruth(mt);

        m_eventHeader->addHit(tmpSGhit);
          // is this needed ? [rdoId] = hitIndex;
        ++hitIndex;
      } // end for each RDO in the strip collection
    } // end for each strip RDO collection
    // dump all RDO's and SDO's for a given event, for debugging purposes
  } // end dump raw SCT data

  ATH_MSG_INFO("Found "<<hitIndex<<" hits and "<< missedHits <<" missed hits, because of filter");

// COMMENT THIS BECAUSE SEEMS NOT TO BE USED 
   /*Comment byt Nick Style:
   "trying to recreate the cluster truth info, s
   o that one could see which HepMc::GenParticle was the dominant contributor, 
   in order to do the truth matching. Since you are (as far as I understand?) 
   just forwarding the truth match from the offline tracks then I would not see why 
   this is needed at this stage.
   */

  /*
  // Dump pixel clusters. They're in m_pixelContainer
  m_pixelContainer->clID(); // anything to dereference the DataHandle
  for (auto iColl = m_pixelContainer->begin(), fColl = m_pixelContainer->end(); iColl != fColl; ++iColl ) {
    const InDet::SiClusterCollection* pixelClusterCollection(*iColl);
    if( !pixelClusterCollection ) {
      ATH_MSG_DEBUG ( "pixelClusterCollection not available!");
      continue;
    }
    //const int size = pixelClusterCollection->size();
    //    ATH_MSG_DEBUG ( "PixelClusterCollection found with " << size << " clusters");

    for (auto iCluster = pixelClusterCollection->begin(), fCluster = pixelClusterCollection->end(); iCluster!=fCluster; ++iCluster ) {
      //needed ? Identifier theId = (*iCluster)->identify();

      // if there is simulation truth available, try to retrieve the "most likely" barcode for this pixel cluster.
      const HepMC::GenParticle* best_parent = 0;
      TrigHTTInputUtils::ParentBitmask parent_mask;
      HepMcParticleLink::ExtendedBarCode best_extcode;
      if (have_pixel_sdo && pixelSimDataMap) {
        for (auto rdoIter = (*iCluster)->rdoList().begin(); rdoIter != (*iCluster)->rdoList().end(); rdoIter++) {
          const InDetDD::SiDetectorElement* sielement = m_PIX_mgr->getDetectorElement(*rdoIter);
          assert( sielement);
          // const InDetDD::SiLocalPosition rawPos = sielement->rawLocalPositionOfCell(*rdoIter);
          InDetDD::SiCellId cellID = sielement->cellIdFromIdentifier (*rdoIter);

          const int nCells = sielement->numberOfConnectedCells(cellID);
          auto iter(pixelSimDataMap->find(*rdoIter));
          // this might be the ganged pixel copy.
          if( nCells>1 && iter == pixelSimDataMap->end() ) {
            InDetDD::SiReadoutCellId SiRC(m_pixelId->phi_index(*rdoIter), m_pixelId->eta_index(*rdoIter));
            for( int ii=0; ii<nCells && iter == pixelSimDataMap->end(); ++ii ) {
              iter = pixelSimDataMap->find(sielement->identifierFromCellId(sielement->design().connectedCell(SiRC,ii)));
            }
          } // end search for correct ganged pixel
          // if SDO found for this pixel, associate the particle. otherwise leave unassociated.
          if (iter!=pixelSimDataMap->end()) {
            const InDetSimData& sdo(iter->second);
            const std::vector<InDetSimData::Deposit>& deposits( sdo.getdeposits() );
            for (auto iDep=deposits.begin(), fDep=deposits.end(); iDep!=fDep; ++iDep) {
              const HepMcParticleLink& particleLink( iDep->first );
              //const InDetSimData::Deposit::second_type qdep( iDep->second ); // energy(charge) contributed by this particle
              // RDO's without SDO's are delta rays or detector noise.
              if( !particleLink.isValid() ) { continue; }
              const HepMC::GenParticle* particle( particleLink );
              const float genEta=particle->momentum().pseudoRapidity();
              const float genPt=particle->momentum().perp(); // MeV
              // reject unstable particles
              if( particle->status()%1000!=1 ) { continue; }
              // reject secondaries and low pT (<400 MeV) pileup
              if( particle->barcode()>10000000 || particle->barcode()==0 ) { continue; }
              // reject far forward particles
              if( fabs(genEta)>m_maxEta ) { continue; }
              // "best_parent" is the highest pt particle
              if( !best_parent || best_parent->momentum().perp()<genPt ) {
                best_parent = particle;
                best_extcode = HepMcParticleLink::ExtendedBarCode( particleLink.barcode() , particleLink.eventIndex() );
              }
              parent_mask |= TrigHTTInputUtils::construct_truth_bitmap( particle );
            } // loop over deposits
          } // if found SDO of pixel
        } // loop over pixels in cluster
      } // if we have pixel sdo's available

      // is this needed? pixelClusterIndexMap[theId] = pixelClusterIndex;
      pixelClusterIndex++;
    } // End loop over pixel clusters
  } // End loop over pixel cluster collection
*/

  return StatusCode::SUCCESS;
}


StatusCode
HTTSGToRawHitsTool::read_offline_clusters(HTTOptionalEventInfo& optional)
{

  //Lets do the Pixel clusters first
  //Loopover the pixel clusters and convert them into a HTTCluster for storage
  // Dump pixel clusters. They're in m_pixelContainer

  const InDetSimDataCollection* pixelSimDataMap(0);
  const bool have_pixel_sdo = m_storeGate->retrieve(pixelSimDataMap, "PixelSDO_Map").isSuccess();
  if (!have_pixel_sdo) {
    ATH_MSG_WARNING ( "Missing Pixel SDO Map");
  }

  m_pixelContainer->clID(); // anything to dereference the DataHandle
  optional.reserveOfflineClusters(m_pixelContainer->size());
  for (auto iColl=m_pixelContainer->begin(), fColl=m_pixelContainer->end(); iColl!=fColl; ++iColl ) {
    const InDet::SiClusterCollection* pixelClusterCollection(*iColl);
    if( !pixelClusterCollection ) {
      ATH_MSG_DEBUG ( "pixelClusterCollection not available!");
      continue;
    }
    const int size = pixelClusterCollection->size();
    ATH_MSG_DEBUG ( "PixelClusterCollection found with " << size << " clusters");
    for (auto iCluster=pixelClusterCollection->begin(), fCluster=pixelClusterCollection->end(); iCluster!=fCluster; ++iCluster) {

      //TODO: This bloc appears multiple times. Turn to a function!
      // if there is simulation truth available, try to retrieve the "most likely" barcode for this pixel cluster.
      const HepMC::GenParticle* best_parent = 0;
      TrigHTTInputUtils::ParentBitmask parent_mask;
      HepMcParticleLink::ExtendedBarCode best_extcode;
      if (have_pixel_sdo && pixelSimDataMap) {
        for (auto rdoIter = (*iCluster)->rdoList().begin(); rdoIter != (*iCluster)->rdoList().end(); rdoIter++) {
          const InDetDD::SiDetectorElement* sielement = m_PIX_mgr->getDetectorElement(*rdoIter);
          assert( sielement);
          // const InDetDD::SiLocalPosition rawPos = sielement->rawLocalPositionOfCell(*rdoIter);
          InDetDD::SiCellId cellID = sielement->cellIdFromIdentifier (*rdoIter);
          const int nCells = sielement->numberOfConnectedCells(cellID);
          auto iter( pixelSimDataMap->find(*rdoIter) );
          // this might be the ganged pixel copy.
          if( nCells>1 && iter == pixelSimDataMap->end() ) {
            InDetDD::SiReadoutCellId SiRC( m_pixelId->phi_index(*rdoIter), m_pixelId->eta_index(*rdoIter) );
            for( int ii=0; ii<nCells && iter == pixelSimDataMap->end(); ++ii ) {
              iter = pixelSimDataMap->find(sielement->identifierFromCellId(sielement->design().connectedCell(SiRC,ii)));
            }
          } // end search for correct ganged pixel
          // if SDO found for this pixel, associate the particle. otherwise leave unassociated.
          if( iter != pixelSimDataMap->end() )  {
            const InDetSimData& sdo(iter->second);
            const std::vector<InDetSimData::Deposit>& deposits( sdo.getdeposits() );
            for( std::vector<InDetSimData::Deposit>::const_iterator iDep=deposits.begin(), fDep=deposits.end(); iDep!=fDep; ++iDep ) {
              const HepMcParticleLink& particleLink( iDep->first );
              //const InDetSimData::Deposit::second_type qdep( iDep->second ); // energy(charge) contributed by this particle
              // RDO's without SDO's are delta rays or detector noise.
              if( !particleLink.isValid() ) { continue; }
              const HepMC::GenParticle* particle( particleLink );
              const float genEta=particle->momentum().pseudoRapidity();
              const float genPt=particle->momentum().perp(); // MeV
              // reject unstable particles
              if( particle->status()%1000!=1 ) { continue; }
              // reject secondaries and low pT (<400 MeV) pileup
              if( particle->barcode()>10000000 || particle->barcode()==0 ) { continue; }
              // reject far forward particles
              if( fabs(genEta)>m_maxEta ) { continue; }
              // "best_parent" is the highest pt particle
              if( !best_parent || best_parent->momentum().perp()<genPt ) {
                best_parent = particle;
                best_extcode = HepMcParticleLink::ExtendedBarCode( particleLink.barcode() , particleLink.eventIndex() );
              }
              parent_mask |= TrigHTTInputUtils::construct_truth_bitmap( particle );
            } // loop over deposits
          } // if found SDO of pixel
        } // loop over pixels in cluster
      } // if we have pixel sdo's available


      Identifier theID = (*iCluster)->identify();
      //cluster object to be written out
      HTTCluster clusterOut;
      //Rawhit object to represent the cluster
      HTTHit clusterEquiv;
      //Lets get the information of this pixel cluster
      const InDetDD::SiDetectorElement* sielement = m_PIX_mgr->getDetectorElement(theID);
      assert( sielement);
      const InDetDD::SiLocalPosition localPos = sielement->localPositionOfCell(theID);
      const Amg::Vector3D globalPos( sielement->globalPosition(localPos) );
      clusterEquiv.setHitType(HitType::clustered);
      clusterEquiv.setX(globalPos.x());
      clusterEquiv.setY(globalPos.y());
      clusterEquiv.setZ(globalPos.z());
      clusterEquiv.setDetType(SiliconTech::pixel);
      clusterEquiv.setIdentifierHash(sielement->identifyHash());

      int barrel_ec = m_pixelId->barrel_ec(theID);
      if (barrel_ec == 0)
        clusterEquiv.setDetectorZone(DetectorZone::barrel);
      else if (barrel_ec == 2)
        clusterEquiv.setDetectorZone(DetectorZone::posEndcap);
      else if (barrel_ec == -2)
        clusterEquiv.setDetectorZone(DetectorZone::negEndcap);

      clusterEquiv.setLayerDisk(m_pixelId->layer_disk(theID));
      clusterEquiv.setPhiModule(m_pixelId->phi_module(theID));
      clusterEquiv.setEtaModule(m_pixelId->eta_module(theID));
      clusterEquiv.setPhiIndex(m_pixelId->phi_index(theID));
      clusterEquiv.setEtaIndex(m_pixelId->eta_index(theID));
      //Col/Row -> eta/phi order??
      clusterEquiv.setPhiWidth((*iCluster)->width().colRow()[1]);
      clusterEquiv.setEtaWidth((*iCluster)->width().colRow()[0]);
      //Save the truth here as the MultiTruth object is only transient
      clusterEquiv.setEventIndex((long)(best_parent ? best_extcode.eventIndex() : std::numeric_limits<long>::max()));
      clusterEquiv.setBarcode((long)(best_parent ? best_extcode.barcode() : std::numeric_limits<long>::max()));
      clusterEquiv.setBarcodePt( static_cast<unsigned long>(std::ceil(best_parent ? best_parent->momentum().perp() : 0.)) );
      clusterEquiv.setParentageMask(parent_mask.to_ulong());
      clusterOut.setClusterEquiv(clusterEquiv);
      optional.addOfflineCluster(clusterOut);
    }
  }

  // PIXEL CLUSTERS
  //Now lets do the strip clusters
  //Loopover the pixel clusters and convert them into a HTTCluster for storage
  // Dump pixel clusters. They're in m_pixelContainer

  const InDetSimDataCollection* sctSimDataMap(0);
  const bool have_sct_sdo = m_storeGate->retrieve(sctSimDataMap, "SCT_SDO_Map").isSuccess();
  if (!have_sct_sdo) {
    ATH_MSG_WARNING ( "Missing SCT SDO Map");
  }

  const DataHandle<SCT_RDO_Container> sct_rdocontainer_iter;
  if( m_storeGate->retrieve(sct_rdocontainer_iter, "SCT_RDOs").isSuccess() ) {
    sct_rdocontainer_iter->clID(); // anything to dereference the DataHandle
    for (auto iColl = sct_rdocontainer_iter->begin(), fColl = sct_rdocontainer_iter->end(); iColl != fColl; ++iColl ) {
      const InDetRawDataCollection<SCT_RDORawData>* SCT_Collection(*iColl);
      if( !SCT_Collection ) { continue; }
      //      const int size = SCT_Collection->size();
      //      ATH_MSG_DEBUG ( "SCT InDetRawDataCollection found with " << size << " RDOs");
      for (auto iRDO = SCT_Collection->begin(), fRDO = SCT_Collection->end(); iRDO != fRDO; ++iRDO ) {
        const Identifier rdoId = (*iRDO)->identify();
        // get the det element from the det element collection
        const InDetDD::SiDetectorElement* sielement = m_SCT_mgr->getDetectorElement(rdoId);
        const InDetDD::SiDetectorDesign& design = dynamic_cast<const InDetDD::SiDetectorDesign&>(sielement->design());
        const InDetDD::SiLocalPosition localPos = design.localPositionOfCell(m_sctId->strip(rdoId));
        const Amg::Vector3D gPos = sielement->globalPosition(localPos);
        // if there is simulation truth available, try to retrieve the
        // "most likely" barcode for this strip.
        const HepMC::GenParticle* best_parent = 0;
        TrigHTTInputUtils::ParentBitmask parent_mask;
        HepMcParticleLink::ExtendedBarCode best_extcode;
        if( have_sct_sdo && sctSimDataMap ) {
          auto iter(sctSimDataMap->find(rdoId));
          // if SDO found for this pixel, associate the particle
          if (iter!=sctSimDataMap->end()) {
            const InDetSimData& sdo(iter->second);
            const std::vector<InDetSimData::Deposit>& deposits( sdo.getdeposits() );
            for (auto iDep=deposits.begin(), fDep=deposits.end(); iDep!=fDep; ++iDep ) {
              const HepMcParticleLink& particleLink( iDep->first );
              // const InDetSimData::Deposit::second_type qdep( iDep->second ); // energy(charge) contributed by this particle
              // RDO's without SDO's are delta rays or detector noise.
              if( !particleLink.isValid() ) { continue; }
              const HepMC::GenParticle* particle( particleLink );
              const float genEta=particle->momentum().pseudoRapidity();
              const float genPt=particle->momentum().perp(); // MeV
              // reject unstable particles
              if( particle->status()%1000!=1 ) { continue; }
              // reject secondaries and low pt (<400 MeV) pileup truth
              if( particle->barcode()>10000000 || particle->barcode()==0 ) { continue; }
              // reject far forward particles
              if( fabs(genEta)>m_maxEta ) { continue; }
              // "best_parent" is the highest pt particle
              if( !best_parent || best_parent->momentum().perp()<genPt ) {
                best_parent = particle;
                best_extcode = HepMcParticleLink::ExtendedBarCode( particleLink.barcode() , particleLink.eventIndex() );
              }
              parent_mask |= TrigHTTInputUtils::construct_truth_bitmap( particle );
            } // end for each contributing particle
          } // end if truth found for this strip
        } // end if sct truth available
        // push back the hit information  to DataInput for HitList , copy from RawInput.cxx
        HTTCluster clusterOut;
        HTTHit clusterEquiv;
        clusterEquiv.setHitType(HitType::clustered);
        clusterEquiv.setX(gPos.x());
        clusterEquiv.setY(gPos.y());
        clusterEquiv.setZ(gPos.z());
        clusterEquiv.setDetType(SiliconTech::strip);
        clusterEquiv.setIdentifierHash(sielement->identifyHash());

        int barrel_ec = m_sctId->barrel_ec(rdoId);
        if (barrel_ec == 0)
          clusterEquiv.setDetectorZone(DetectorZone::barrel);
        else if (barrel_ec == 2)
          clusterEquiv.setDetectorZone(DetectorZone::posEndcap);
        else if (barrel_ec == -2)
          clusterEquiv.setDetectorZone(DetectorZone::negEndcap);

        clusterEquiv.setLayerDisk(m_sctId->layer_disk(rdoId));
        clusterEquiv.setPhiModule(m_sctId->phi_module(rdoId));
        clusterEquiv.setEtaModule(m_sctId->eta_module(rdoId));
        clusterEquiv.setPhiIndex(m_sctId->strip(rdoId));
        clusterEquiv.setEtaIndex(m_sctId->row(rdoId));
        clusterEquiv.setSide(m_sctId->side(rdoId));
        //I think this is the strip "cluster" width
        clusterEquiv.setPhiWidth((*iRDO)->getGroupSize());
        //Save the truth here as the MultiTruth object is only transient
        clusterEquiv.setEventIndex((long)(best_parent ? best_extcode.eventIndex() : std::numeric_limits<long>::max()));
        clusterEquiv.setBarcode((long)(best_parent ? best_extcode.barcode(): std::numeric_limits<long>::max()));
        clusterEquiv.setBarcodePt( static_cast<unsigned long>(std::ceil(best_parent ? best_parent->momentum().perp() : 0.)) );
        clusterEquiv.setParentageMask(parent_mask.to_ulong());
        clusterOut.setClusterEquiv(clusterEquiv);
        optional.addOfflineCluster(clusterOut);
      } // end for each RDO in the strip collection
    } // end for each strip RDO collection
    // dump all RDO's and SDO's for a given event, for debugging purposes
  } // end dump raw SCT data

  return StatusCode::SUCCESS;
}

StatusCode HTTSGToRawHitsTool::read_truth_tracks(HTTOptionalEventInfo& optional)
{

  // retrieve truth tracks from athena
  const McEventCollection* SimTracks = 0;
  if (m_storeGate->retrieve(SimTracks, "TruthEvent").isFailure()) {
    std::string key = "G4Truth";
    if (m_storeGate->retrieve(SimTracks, key).isFailure()) {
      key = "";
      if (m_storeGate->retrieve(SimTracks,key).isFailure()) {
        ATH_MSG_ERROR ( "could not find the McEventCollection truth tracks" );
        return StatusCode::FAILURE;
      }
    }
  }

  // Retrieve the links between HepMC and xAOD::TruthParticle
  const xAODTruthParticleLinkVector* truthParticleLinkVec = 0;
  if (m_storeGate->retrieve(truthParticleLinkVec, "xAODTruthLinks").isFailure()) {
    ATH_MSG_ERROR ( "Cannot retrieve TruthParticleLinkVector" );
  }

  ATH_MSG_INFO("Dump truth tracks, size " << SimTracks->size() << " " << (SimTracks ? SimTracks->size() : 0u));

  // dump each truth track
  optional.reserveOfflineClusters(SimTracks->size());
  //for( unsigned int ievt=0, fevt=(SimTracks ? SimTracks->size() : 0u); ievt!=fevt; ++ievt ) {
  for (unsigned ievt = 0; ievt < SimTracks->size(); ++ievt) {

    const HepMC::GenEvent* genEvent = SimTracks->at(ievt);
    // retrieve the primary interaction vertex here. for now, use the dummy origin.
    HepGeom::Point3D<double> primaryVtx(0.,0.,0.);
    // the event should have signal process vertex unless it was generated as single particles.
    // if it exists, use it for the primary vertex.
    if (genEvent->signal_process_vertex()) {
      primaryVtx.set(genEvent->signal_process_vertex()->point3d().x(),
                     genEvent->signal_process_vertex()->point3d().y(),
                     genEvent->signal_process_vertex()->point3d().z());
      ATH_MSG_DEBUG ( "using signal process vertex for eventIndex " << ievt << ":"
                      << primaryVtx.x() << "\t" << primaryVtx.y()  << "\t" << primaryVtx.z() );
    }

    for (auto it = genEvent->particles_begin(), ft = genEvent->particles_end(); it != ft; ++it) {

      const HepMC::GenParticle* const particle(*it);
      const int pdgcode = particle->pdg_id();

      // reject generated particles without a production vertex.
      if (!particle->production_vertex()) {
        // ATH_MSG_INFO ("Production vertex not found for this particle, ignored...");
        continue;
      }

      // reject neutral or unstable particles
      const HepPDT::ParticleData* pd = m_particleDataTable->particle(abs(pdgcode));

      if (!pd) {
        // ATH_MSG_INFO ("ParticleData not avialable for this particle, ignored...");
        continue;
      }

      float charge = pd->charge();
      if (pdgcode < 0) charge *= -1.; // since we took absolute value above

      if (std::abs(charge)<0.5) {
        // ATH_MSG_INFO ("This is neutral, ignored");
        continue;
      }

      if (particle->status()%1000!=1) {
        // ATH_MSG_INFO ("Particle status "<< particle->status() <<", igneored");
        continue;
      }

      // truth-to-track tool
      const Amg::Vector3D momentum(particle->momentum().px(), particle->momentum().py(), particle->momentum().pz());
      const Amg::Vector3D position(particle->production_vertex()->position().x(), particle->production_vertex()->position().y(), particle->production_vertex()->position().z());
      const Trk::CurvilinearParameters cParameters(position, momentum, charge);

      Trk::PerigeeSurface persf;
      if (m_UseNominalOrigin) {
        Amg::Vector3D origin(0,0,0);
        persf = Trk::PerigeeSurface(origin);
      }
      else {
        persf = m_beamSpotSvc->beamPos();
      }

      const Trk::TrackParameters* tP = m_extrapolator->extrapolate(cParameters, persf, Trk::anyDirection, false);

      const double track_truth_p = (tP && fabs(tP->parameters()[Trk::qOverP]) > 1.e-8) ?
                                    tP->charge()/tP->parameters()[Trk::qOverP] : 10E7;
      const double track_truth_phi = tP ? tP->parameters()[Trk::phi] : 999.;
      const double track_truth_d0 = tP ? tP->parameters()[Trk::d0] : 999.;
      const double track_truth_z0 = tP ? tP->parameters()[Trk::z0] : 999.;
      
      // const double track_truth_x0 = tP ? tP->position().x() : 999.;
      // const double track_truth_y0 = tP ? tP->position().y() : 999.;
      // const double track_truth_z0 = tP ? tP->position().z() : 999.;
      
      const double track_truth_q = tP ? tP->charge() : 0.;

      const double track_truth_sinphi = tP ? std::sin(tP->parameters()[Trk::phi]) : -1.;
      const double track_truth_cosphi = tP ? std::cos(tP->parameters()[Trk::phi]) : -1.;
      const double track_truth_sintheta = tP ? std::sin(tP->parameters()[Trk::theta]) : -1.;
      const double track_truth_costheta = tP ? std::cos(tP->parameters()[Trk::theta]) : -1.;

      double truth_d0corr = track_truth_d0 - (primaryVtx.y()*cos(track_truth_phi) - primaryVtx.x()*sin(track_truth_phi));
      double truth_zvertex = 0.;

      delete tP;
      
      const HepGeom::Point3D<double> startVertex(particle->production_vertex()->point3d().x(), particle->production_vertex()->point3d().y(), particle->production_vertex()->point3d().z());
      // categorize particle (prompt, secondary, etc.) based on InDetPerformanceRTT/detector paper criteria.
      bool isPrimary = true;
      if (std::abs(truth_d0corr)>2.) isPrimary = false;
      if (particle->barcode() > 10000000 || particle->barcode() == 0) isPrimary = false;

      if (isPrimary && particle->production_vertex()) {
        const HepGeom::Point3D<double> startVertex(particle->production_vertex()->point3d().x(), particle->production_vertex()->point3d().y(), particle->production_vertex()->point3d().z());
        if (std::abs(startVertex.z() - truth_zvertex) > 100.) isPrimary = false;
        if (particle->end_vertex()) {
          HepGeom::Point3D<double> endVertex(particle->end_vertex()->point3d().x(), particle->end_vertex()->point3d().y(), particle->end_vertex()->point3d().z());
          if (endVertex.perp() < 400. && std::abs(endVertex.z()) < 2300.) isPrimary=false;
        }
      } else
        isPrimary = false;

      HepMcParticleLink::ExtendedBarCode extBarcode2(particle->barcode(), ievt);
      
      // Retrieve the corresponding xAOD::TruthParticle to get offline track matching info
      float bestMatch = 0.;
      const xAOD::TruthParticle* truthParticle = nullptr;
      for (const auto& entry : *truthParticleLinkVec) {
        if (entry->first.isValid() && entry->second.isValid() && entry->first.cptr()->barcode() == particle->barcode()) {
          truthParticle = *entry->second;
          if (particle->barcode() != truthParticle->barcode() ||
              particle->pdg_id() != truthParticle->pdgId() ||
              particle->status() != truthParticle->status())
            truthParticle = nullptr;
        }
      }
      if (truthParticle == nullptr)
        ATH_MSG_WARNING ( "This HepMC::GenParticle has no matched xAOD::TruthParticle." );
      else{
        
        for (const auto& trackParticle: trackParticleVec) { // Inner loop over track particles
          const xAOD::TruthParticle* associatedTruth = nullptr;
          typedef ElementLink<xAOD::TruthParticleContainer> Link_t;
          if (trackParticle->isAvailable<Link_t>("truthParticleLink")) {
            static SG::AuxElement::ConstAccessor<Link_t> linkAcc("truthParticleLink");
            const Link_t& link = linkAcc(*trackParticle);
            if (link.isValid()) associatedTruth = *link;
          }
          if (associatedTruth && associatedTruth == truthParticle) {  
            float prob(std::numeric_limits<float>::quiet_NaN());
            if (trackParticle->isAvailable<float>("truthMatchProbability"))
              prob = trackParticle->auxdata<float>("truthMatchProbability");
            if (not std::isnan(prob)) bestMatch = std::max(prob, bestMatch);
          }
        }
      }      
      
      HTTTruthTrack tmpSGTrack;

      tmpSGTrack.setD0(track_truth_d0);
      tmpSGTrack.setZ0(track_truth_z0);

      tmpSGTrack.setVtxX(primaryVtx.x());
      tmpSGTrack.setVtxY(primaryVtx.y());
      tmpSGTrack.setVtxZ(primaryVtx.z());
      tmpSGTrack.setPX(track_truth_p*track_truth_cosphi*track_truth_sintheta);
      tmpSGTrack.setPY(track_truth_p*track_truth_sinphi*track_truth_sintheta);
      tmpSGTrack.setPZ(track_truth_p*track_truth_costheta);

      tmpSGTrack.setQ(track_truth_q);
      
      tmpSGTrack.setPDGCode(pdgcode);
      tmpSGTrack.setBarcode(extBarcode2.barcode());
      tmpSGTrack.setEventIndex(extBarcode2.eventIndex());
      tmpSGTrack.setBarcodeFracOffline(bestMatch);

      ATH_MSG_DEBUG(tmpSGTrack.getPars());
      optional.addTruthTrack(tmpSGTrack);
      
    } // end for each GenParticle in this GenEvent

  } // end for each GenEvent
  
  return StatusCode::SUCCESS;
}
