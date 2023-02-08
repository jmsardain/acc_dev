/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTT_RAWTOLOGICALHITSTOOL_H
#define HTT_RAWTOLOGICALHITSTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include "TrigHTTObjects/HTTHit.h"
#include "TrigHTTMaps/ITrigHTTMappingSvc.h"
#include "TrigHTTConfig/IHTTEventSelectionSvc.h"

//TO DO: moe into HTTMaps

// Forward declaration
class ITrigHTTMappingSvc;
class HTTEventInputHeader;
class HTTLogicalEventInputHeader;


class HTT_RawToLogicalHitsTool : public AthAlgTool
{
 public:

  HTT_RawToLogicalHitsTool(const std::string&, const std::string&, const IInterface*);
  ~HTT_RawToLogicalHitsTool() {;}

  /** @brief Query interface */
  static const InterfaceID& interfaceID( ) ; //needed in rel 20, not in rel21?

  StatusCode initialize();
  StatusCode finalize();
  StatusCode convert(unsigned stage, const HTTEventInputHeader& header, HTTLogicalEventInputHeader& logicheader);
  StatusCode getUnmapped(std::vector<HTTHit>& missing_hits);

  const HTTPlaneMap* getPlaneMap_1st();

  // for histograms used in debugging
  std::vector<int> m_missing_hit_codes;
private:
  // JO configuration
  ServiceHandle<ITrigHTTMappingSvc>     m_HTTMapping;
  ServiceHandle<IHTTEventSelectionSvc>  m_EvtSel;
  IntegerProperty                       m_saveOptional; // =0 no optional saved, =1 saved in region, =2 save all 
  IntegerArrayProperty                  m_towersToMap; // {this, "TowersToMap", {}, "Which Towers to map, goes from 0 to 96!"};

  // internal members

  std::vector<int> m_towers;
  std::vector<HTTHit> m_missing_hits;// vector to save hits not mapped, debugging only
  //  std::set<int> evt_logical_layers; // set to count when an event has found atleast 8 layers
};

#endif // HTT_RAWTOLOGICALHITSTOOL_H
