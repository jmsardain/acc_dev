/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTInput/HTT_RandRawHitsFromInvConstsTool.h"
#include "TrigHTTMaps/HTTSSMap.h"


HTT_RandRawHitsFromInvConstsTool::HTT_RandRawHitsFromInvConstsTool(const std::string& algname, const std::string &name, const IInterface *ifc) :
  base_class(algname,name,ifc),
  m_HTTBankSvc("TrigHTTBankSvc", name),
  m_HTTMappingSvc("TrigHTTMappingSvc", name),
  m_nTracks(1),
  m_PoissonFluctuate(false),
  m_1stStageTracks(true),
  m_nMaxWC(2)
{
  declareProperty("NTracks", m_nTracks);
  declareProperty("PoissonFluctuate", m_PoissonFluctuate);
  declareProperty("Generate1stStageTracks",m_1stStageTracks); // if not 1st stage then it's second stage
  declareProperty("NMaxWC",m_nMaxWC); // max WCs, ie max number of missed planes before we accept a track and stop
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTT_RandRawHitsFromInvConstsTool::initialize()
{
  ATH_CHECK(m_HTTBankSvc.retrieve());
  ATH_CHECK(m_HTTMappingSvc.retrieve());

  // ATH_CHECK( detStore()->retrieve(m_PIX_mgr, "Pixel") );
  // ATH_CHECK( detStore()->retrieve(m_SCT_mgr, "SCT") );



  m_sectorSlice = m_HTTBankSvc->SectorSlice();
  std::pair<HTTTrackPars, HTTTrackPars> bnds = m_sectorSlice->getBoundaries();

  for (unsigned i = 0; i < HTTTrackPars::NPARS; i++)
    {
      if (std::isnan(m_sliceMin[i])) m_sliceMin[i] = bnds.first[i];
      if (std::isnan(m_sliceMax[i])) m_sliceMax[i] = bnds.second[i];
    }
  ATH_MSG_INFO("Slice parameters: " << m_sliceMin << "; " << m_sliceMax);

  if (m_1stStageTracks) {
    m_pmap = m_HTTMappingSvc->PlaneMap_1st();
    m_constBank = m_HTTBankSvc->FitConstantBank_1st();
  }
  else {
    m_pmap = m_HTTMappingSvc->PlaneMap_2nd();
    m_constBank = m_HTTBankSvc->FitConstantBank_2nd();
  }

  m_constBank->prepareInvFitConstants();

  return StatusCode::SUCCESS;
}


StatusCode HTT_RandRawHitsFromInvConstsTool::finalize()
{
  ATH_MSG_INFO ("finalized");
  return StatusCode::SUCCESS;
}

StatusCode HTT_RandRawHitsFromInvConstsTool::writeData(HTTEventInputHeader* header) {
  m_eventHeader=header; //take the external pointer
  // then do something with it!
  return StatusCode::SUCCESS; 
}


/** This function get from the SG the inner detector raw hits
  and prepares them for HTT simulation */
StatusCode HTT_RandRawHitsFromInvConstsTool::readData(HTTEventInputHeader* header, bool &last)
{
  return readData(header,last,false); // by default don't reset
}


/** This function get from the SG the inner detector raw hits
  and prepares them for HTT simulation */
StatusCode HTT_RandRawHitsFromInvConstsTool::readData(HTTEventInputHeader* header, bool &last, bool doReset)
{
  last=false;
  m_eventHeader=header; //take the external pointer
  HTTOptionalEventInfo optional;
  if (doReset) {
    m_eventHeader->reset(); //reset things and use new optional event info
  }
  else {
    optional = m_eventHeader->optional(); // keep the old optional event header!
  }
    

  int ntrack = m_nTracks;
  if (m_PoissonFluctuate) ntrack = CLHEP::RandPoisson::shoot(ntrack);
  for (int itrack = 1; itrack <= ntrack; itrack++) { // don't want barcode to start counting at zero
    HTTTrackPars m_genPars = getRandomTrackPars();
    vector<HTTHit> rawhits = getRawHits(m_genPars, itrack);
    ATH_MSG_DEBUG("Adding " << rawhits.size() << " hits from our track");
    for (auto rawhit : rawhits) m_eventHeader->addHit(rawhit);
    HTTTruthTrack truthtrack = getTruthTrack(m_genPars, itrack);
    optional.addTruthTrack(truthtrack);
  }

  m_eventHeader->setOptional(optional);
  ATH_MSG_DEBUG ( *m_eventHeader);
  ATH_MSG_DEBUG ( "End of execute()");
  return StatusCode::SUCCESS;
}

// Get random track using the sector parameters.
// FTK Inherited.
HTTTrackPars HTT_RandRawHitsFromInvConstsTool::getRandomTrackPars() const
{

  // JAA TODO: Maybe we want to have options to preferentially have tracks with lower pt or other such parameters

    HTTTrackPars pars;

    pars.qOverPt = (m_sliceMax.qOverPt - m_sliceMin.qOverPt) * CLHEP::RandFlat::shoot() + m_sliceMin.qOverPt;
    pars.eta = (m_sliceMax.eta - m_sliceMin.eta) * CLHEP::RandFlat::shoot() + m_sliceMin.eta;
    pars.z0  = (m_sliceMax.z0  - m_sliceMin.z0)  * CLHEP::RandFlat::shoot() + m_sliceMin.z0;
    pars.phi = (m_sliceMax.phi - m_sliceMin.phi) * CLHEP::RandFlat::shoot() + m_sliceMin.phi;
    pars.phi = remainder(pars.phi, 2.*M_PI); // Convert phi to the format used in the constants calculations
    pars.d0=(m_sliceMax.d0 - m_sliceMin.d0) * CLHEP::RandFlat::shoot() + m_sliceMin.d0;
    ATH_MSG_DEBUG("Threw a random track! And q/pt = " << pars.qOverPt << " and eta = " << pars.eta << " and z0 " << pars.z0 << " and phi = " << pars.phi << " and d0 = " << pars.d0);

    return pars;
}

HTTTruthTrack HTT_RandRawHitsFromInvConstsTool::getTruthTrack(HTTTrackPars &pars, int &tracknum) const
{
  HTTTruthTrack track;
  
  track.setD0(pars.d0);
  track.setZ0(0.);

  double sinh_eta = sinh(pars.eta); // this is pz / pt
  double pt = 1. / fabs(pars.qOverPt);
  double pz = pt*sinh_eta;

  track.setVtxX(0);
  track.setVtxY(0);
  track.setVtxZ(pars.z0);
  track.setPX(pt*cos(pars.phi)); // do we need abs value of phi?
  track.setPY(pt*sin(pars.phi)); // do we need abs value of phi?
  track.setPZ(pz);
  track.setQ(pars.qOverPt > 0 ? 1 : -1);
  track.setPDGCode(0);
  track.setBarcode(tracknum);
  track.setEventIndex(0);
  track.setBarcodeFracOffline(0.);

  ATH_MSG_DEBUG("Making a fake truth track with pt = " << track.getQ()*track.getPt() << " and d0 = " << track.getD0() << " and px = " << track.getPX() << " and py = " << track.getPY() << " and phi = " << track.getPhi() << " and eta = " << track.getEta());

  return track;

}

// not used right now. we can pick a random sector, but it's unlikely to have enough hits. may still be useful?
sector_t HTT_RandRawHitsFromInvConstsTool::getRandomSector(HTTTrackPars &pars) const
{
    // Find sectors that contain this track using sector slices, then we just randomly use one
    vector<int> sectors = m_sectorSlice->searchSectors(pars);
    size_t nsector = sectors.size();
    ATH_MSG_DEBUG("Number of sector found = " << nsector);
    if (nsector == 0) return SECTOR_NOTFOUND;

    // probably a better way to pick a single random sector using CLHEP, not sure how, but this should work
    unsigned int randvalue = (unsigned int)(nsector*CLHEP::RandFlat::shoot());
    if (randvalue == nsector) randvalue = randvalue - 1; // protection against rare case

    ATH_MSG_DEBUG("Picking sector = " << randvalue);
    sector_t sector = sectors[randvalue];
    ATH_MSG_DEBUG("Returning sector " << sector);
    return sector;
}

std::vector<HTTHit> HTT_RandRawHitsFromInvConstsTool::getRawHits(const HTTTrackPars &pars, const int &tracknum) const
{

  std::vector<HTTHit> rawhits;
  HTTHit rawhit;
  HTTTrack track;
  track.setNLayers(m_pmap->getNLogiLayers());

  // Find sectors that contain this track using sector slices, then we just randomly use one
  vector<int> sectors = m_sectorSlice->searchSectors(pars);

  track.setPars(pars);

  //Removed "RAW" from these as this no longer exists... Does this entire class need to exist now?
  track.setPhi(track.getPhi(), false);
  track.setD0(track.getD0());
  track.setZ0(track.getZ0());

  static vector<double> constr(m_constBank->getNConstr()); // filled with 0
  for (unsigned int i=0; i<constr.size(); ++i)
    constr[i] = CLHEP::RandGauss::shoot();

  for (auto sector: sectors) {
    rawhits.clear();
    int nwc = 0;
    m_constBank->invlinfit(sector, track, &constr[0]); // sets track.setCoord
    for (size_t layer = 0; layer < m_pmap->getNLogiLayers(); layer++) {
      SiliconTech Tech = m_pmap->getDetType(layer);

      // Comment from FTK: TODO
      // potential BUG with HWMODEID=2, the section number and module number are not known
      int section = 0;

      // Find x coordinate
      double x = track.getPhiCoord(layer);
      double x_width = m_HTTMappingSvc->SSMap()->getModulePhiWidth(layer,section);
      if (x < 0 || x >= x_width) // no good
	{
	  nwc++;
	  //	  ATH_MSG_DEBUG("x: " << x << " xwidth: " << x_width);
	  continue;
	}

      rawhit.setDetType(SiliconTech::strip);

      // JAA TODO we need the RDO ID, this isn't necessarily the same thing!
      const Identifier globalModuleIdentifier(m_HTTBankSvc->SectorBank_1st()->getModules(sector)[layer]);

      // Find y coordinate, if needed
      double y_width = 0, y = 0;
      if (Tech == SiliconTech::pixel) // is pixel
	{
	  rawhit.setDetType(SiliconTech::pixel);
	  y = track.getEtaCoord(layer);
	  y_width = m_HTTMappingSvc->SSMap()->getModuleEtaWidth(layer,section);
	  if (y < 0 || y >= y_width) // no good
	    {
	      nwc++;
	      continue;
	    }
	  
	  // JAA TODO WE CANNOT USE THIS SINCE WE DON'T HAVE GEOMETRIES LOADED! WHAT TO DO?
	  // const InDetDD::SiDetectorElement* sielement = m_PIX_mgr->getDetectorElement(globalModuleIdentifier);
	  // Amg::Vector2D LocalPos = sielement->localPositionOfCell(globalModuleIdentifier);
	  // Amg::Vector3D globalPos = sielement->globalPosition(LocalPos);
	  // rawhit.setX(globalPos[Amg::x]);
	  // rawhit.setY(globalPos[Amg::y]);
	  // rawhit.setZ(globalPos[Amg::z]);
	}
      else { //is strip
	// 	  // JAA TODO WE CANNOT USE THIS SINCE WE DON'T HAVE GEOMETRIES LOADED! WHAT TO DO?
	// const InDetDD::SiDetectorElement* sielement = m_SCT_mgr->getDetectorElement(globalModuleIdentifier);
	// Amg::Vector2D LocalPos = sielement->localPositionOfCell(globalModuleIdentifier);
	// std::pair<Amg::Vector3D, Amg::Vector3D> endsOfStrip = sielement->endsOfStrip(LocalPos);
	// rawhit.setX(0.5*(endsOfStrip.first.x() + endsOfStrip.second.x()));
	// rawhit.setY(0.5*(endsOfStrip.first.y() + endsOfStrip.second.y()));
	// rawhit.setZ(0.5*(endsOfStrip.first.z() + endsOfStrip.second.z()));
      }
    
      // Find ssid
      module_t globalModuleId = m_HTTBankSvc->SectorBank_1st()->getModules(sector)[layer];
      rawhit.setIdentifierHash(globalModuleId);

      // Find ssid
      rawhit.setHitType(HitType::mapped);
      rawhit.setDetType(Tech);
      if (Tech == SiliconTech::strip) {
	rawhit.setIdentifierHash(globalModuleId >> 2);
	rawhit.setEtaIndex(globalModuleId & 0b11);
      }
      else {
	rawhit.setIdentifierHash(globalModuleId);
      }
      rawhit.setLayer(layer);
      rawhit.setSection(section); // is this right?
      rawhit.setPhiCoord(x);
      if (Tech == SiliconTech::pixel) rawhit.setEtaCoord(y);

      rawhit.setBarcode(tracknum); // kludge index, should work
      rawhit.setEventIndex(0);
      rawhit.setBarcodePt(fabs(1./pars.qOverPt));
      rawhit.setParentageMask(0);
      rawhit.setDetectorZone(DetectorZone::barrel); // doesn't work for endcap right now!
      rawhits.push_back(rawhit);

    }
    if (nwc <= m_nMaxWC)
      return rawhits;
  }
  // if we got here we didn't find anything matching within the number of necessary WCs, so return nothing
  rawhits.clear();
  return rawhits;
}
