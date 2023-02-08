/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTTSGToRawHitsTool_H
#define HTTSGToRawHitsTool_H

#include "TrigHTTObjects/HTTHit.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "CLHEP/Geometry/Point3D.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IPartPropSvc.h"
#include "TrkToolInterfaces/ITruthToTrack.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTruthData/TrackTruthCollection.h"
#include "TrkTrackSummaryTool/TrackSummaryTool.h"
#include "TrkToolInterfaces/ITrackHoleSearchTool.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkParameters/TrackParameters.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "HepPDT/ParticleDataTable.hh"
#include "HepPDT/ParticleData.hh"

#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetPrepRawData/SiClusterContainer.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"

#include "TrigHTTInput/HTTEventInputHeaderToolI.h"
#include "TrigHTTObjects/HTTOptionalEventInfo.h"
#include "TrigHTTConfig/IHTTEventSelectionSvc.h"

class HTTEventInputHeader;
class HTTCluster;
class HTTTruthTrack;
class HTTOfflineTrack;
class IHTTEventSelectionSvc;


class HTTSGToRawHitsTool :  public extends<AthAlgTool, HTTEventInputHeaderToolI>  {

 public:  
  HTTSGToRawHitsTool(const std::string&, const std::string&, const IInterface*);
  virtual ~HTTSGToRawHitsTool() {;}

  virtual StatusCode initialize() override;
  virtual StatusCode finalize()   override;
  virtual StatusCode readData(HTTEventInputHeader* header, bool &last)  override;
  virtual StatusCode writeData(HTTEventInputHeader* header)  override; //not implmeneted yet

private:

  // JO configuration
  ToolHandle<Trk::ITruthToTrack>  m_truthToTrack; //!< tool to create track parameters from a gen particle
  ToolHandle<Trk::IExtrapolator>  m_extrapolator;
  ServiceHandle<IBeamCondSvc>     m_beamSpotSvc;
  ServiceHandle<IHTTEventSelectionSvc>  m_EvtSel;

  StringProperty         m_pixelClustersName;
  StringProperty         m_sctClustersName;
  StringProperty         m_tracksTruthName;
  StringProperty         m_offlineName;// {this, "OfflineName", "InDetTrackParticles", "name of offline tracks collection"};

  BooleanProperty        m_dumpHitsOnTracks;
  BooleanProperty        m_dumpSpacePoints;
  BooleanProperty        m_dumpTruthIntersections;
  BooleanProperty        m_filterHits;
  //  BooleanProperty        m_useOfflineTrackSelectorTool;
  BooleanProperty        m_readOfflineClusters;
  BooleanProperty        m_readTruthTracks; // if true the truth tracks are read
  BooleanProperty        m_readOfflineTracks;// {this, "GetOffline", false, "flag to enable the offline tracking save"};
  BooleanProperty        m_UseNominalOrigin; // if true truth values are always with respect to (0,0,0)
  DoubleProperty         m_maxEta;
  DoubleProperty         m_minPt;


  double m_minEtaHit=-6.;
  double m_maxEtaHit= 6.;
  double m_minPhiHit= CLHEP::pi *(-1);
  double m_maxPhiHit= CLHEP::pi;

  unsigned  m_event = 0;
  

  //internal pointers
  const AtlasDetectorID*   m_idHelper;
  StoreGateSvc*            m_storeGate;
  const PixelID*           m_pixelId;
  const SCT_ID*            m_sctId;

  const InDetDD::SiDetectorManager*     m_PIX_mgr;
  const InDetDD::SiDetectorManager*     m_SCT_mgr;
  const InDet::SiClusterContainer*      m_pixelContainer;
  const InDet::SiClusterContainer*      m_sctContainer;
  const HepPDT::ParticleDataTable*      m_particleDataTable;

  std::vector<const xAOD::TrackParticle*> trackParticleVec;

  typedef std::map<Identifier,int> HitIndexMap;
  bool IsHitInRegion(Amg::Vector3D& globalPos);
  
  StatusCode read_raw_silicon(  ) ; // dump raw silicon data to text file and populate hitIndexMap for rec. track processing
  StatusCode read_truth_tracks(HTTOptionalEventInfo& optional);
  StatusCode read_offline_tracks(HTTOptionalEventInfo& optional);
  StatusCode read_offline_clusters(HTTOptionalEventInfo& optional);

};



#endif // HTT_SGHINPUTTOOL_H
