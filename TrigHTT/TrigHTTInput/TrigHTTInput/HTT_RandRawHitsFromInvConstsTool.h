/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTT_RANDRAWHITSFROMINVCONSTSTOOL_H
#define HTT_RANDRAWHITSFROMINVCONSTSTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include "TrigHTTObjects/HTTHit.h"
#include "TrigHTTObjects/HTTOptionalEventInfo.h"
#include "TrigHTTObjects/HTTTrack.h"
#include "TrigHTTObjects/HTTTruthTrack.h"
#include "TrigHTTObjects/HTTTrackPars.h"
#include "TrigHTTObjects/HTTEventInputHeader.h"

#include "TrigHTTInput/HTTEventInputHeaderToolI.h"

#include "TrigHTTBanks/HTTFitConstantBank.h"
#include "TrigHTTBanks/HTTSectorSlice.h"
#include "TrigHTTBanks/HTTSectorBank.h"
#include "TrigHTTBanks/ITrigHTTBankSvc.h"

#include "TrigHTTMaps/HTTPlaneMap.h"
#include "TrigHTTMaps/ITrigHTTMappingSvc.h"

#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGauss.h"
#include "CLHEP/Random/RandPoisson.h"

#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"

//class ITrigHTTBankSvc;
//class ITrigHTTMappingSvc;
class HTTTruthTrack;
class HTTEventInputHeader;

using std::vector;

class HTT_RandRawHitsFromInvConstsTool :  public extends<AthAlgTool, HTTEventInputHeaderToolI>  {

 public:
  HTT_RandRawHitsFromInvConstsTool(const std::string&, const std::string&, const IInterface*);
  virtual ~HTT_RandRawHitsFromInvConstsTool() {;}

  virtual StatusCode initialize() override;
  virtual StatusCode finalize()   override;
  virtual StatusCode readData(HTTEventInputHeader* header, bool &last)  override;
  virtual StatusCode writeData(HTTEventInputHeader* header) override;
  StatusCode readData(HTTEventInputHeader* header, bool &last, bool doReset);
  
private:
  ServiceHandle<ITrigHTTBankSvc> m_HTTBankSvc;
  ServiceHandle<ITrigHTTMappingSvc> m_HTTMappingSvc;

  const InDetDD::SiDetectorManager*     m_PIX_mgr;
  const InDetDD::SiDetectorManager*     m_SCT_mgr;


  HTTFitConstantBank *m_constBank; // not owned
  HTTSectorSlice *m_sectorSlice; // not owned
  const HTTPlaneMap *m_pmap; // not owned
  HTTSectorBank *m_sectorBank; // now owned

  // Slice parameters
  //    These are auto-assigned from the sector slice file, if not set.
  //    Only manually set to be more restrictive than those in file.
  HTTTrackPars m_sliceMin;
  HTTTrackPars m_sliceMax;

  HTTEventInputHeader *m_eventHeader;

  IntegerProperty  m_nTracks; // number of random tracks
  BooleanProperty  m_PoissonFluctuate; // choose whether to poisson fluctuate around m_nTracks or keep it constant
  BooleanProperty  m_1stStageTracks; // choose whether to generate 1st or 2nd stage tracks
  IntegerProperty  m_nMaxWC; // max number of missed planes before we accept a track and stop

  HTTTrackPars getRandomTrackPars() const;

  HTTTruthTrack getTruthTrack(HTTTrackPars &pars, int &tracknum) const;
  sector_t getRandomSector(HTTTrackPars &pars) const;
  std::vector<HTTHit> getRawHits(const HTTTrackPars &pars, const int &tracknum) const;

};



#endif // HTT_SGHINPUTTOOL_H
