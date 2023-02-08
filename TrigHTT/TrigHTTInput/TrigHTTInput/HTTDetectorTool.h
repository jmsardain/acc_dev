/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTTDETECTORTOOL_H
#define HTTDETECTORTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrigHTTObjects/HTTHit.h"
#include "TrigHTTObjects/HTTHit.h"
#include "TrigHTTMaps/ITrigHTTMappingSvc.h"

#include <string>
#include <vector>
#include <list>
#include <set>
#include <fstream>

#include "GaudiKernel/IPartPropSvc.h"
#include "TrkToolInterfaces/ITruthToTrack.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTruthData/TrackTruthCollection.h"
#include "TrkTrackSummaryTool/TrackSummaryTool.h"
#include "TrkToolInterfaces/ITrackHoleSearchTool.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

#include "InDetPrepRawData/SiClusterContainer.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"

class AtlasDetectorID;
class StoreGateSvc;
class ITruthParameters;
class TruthSelector;
class PixelID;
class SCT_ID;
class IBeamCondSvc;
class ITrigHTTMappingSvc;

namespace InDetDD {
  class SiDetectorManager;
}

/* This class interface the ID hits with the HTT simulation
    implemented in Athena. Original code */

class HTTDetectorTool : public ::AthAlgTool {
 public:

  HTTDetectorTool(const std::string&, const std::string&, const IInterface*);
  ~HTTDetectorTool() {;}

  /** @brief Query interface */
  static const InterfaceID& interfaceID( ) ; //needed in rel 20, not in rel21?

  StatusCode initialize();
  StatusCode finalize();
  void dumpGlobalToLocalModuleMap();

private:


  ServiceHandle<ITrigHTTMappingSvc>   m_HTTMapping;
  const  InDetDD::SiDetectorManager*  m_PIX_mgr;
  const PixelID*   m_pixelId;
  const SCT_ID*    m_sctId;

//// variable to create the module ID map
 std::string m_global2local_path;
 std::string m_sram_path_pix;
 std::string m_sram_path_sct;
 bool m_dumpAllModules;
//
};

#endif // HTTDETECTORTOOL_H
