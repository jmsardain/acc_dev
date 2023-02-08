/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTTTRACKFITTERTOOL_H
#define HTTTRACKFITTERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include "TrigHTTMaps/ITrigHTTMappingSvc.h"
#include "TrigHTTBanks/ITrigHTTBankSvc.h"

#include "TrigHTTUtils/HTTMacros.h"
#include "TrigHTTAlgorithms/TrackFitter.h"
#include "TrigHTTBanks/HTTSectorBank.h"

#include <string>
#include <vector>
#include <ostream>

class ITrigHTTMappingSvc;
class ITrigHTTBankSvc;
class HTTSectorBank;
class TrackFitter;

/////////////////////////////////////////////////////////////////////////////
class HTTTrackFitterTool: public AthAlgTool {
public:
  HTTTrackFitterTool (const std::string&, const std::string&, const IInterface*);
  ~HTTTrackFitterTool() {}

  static const InterfaceID& interfaceID();

  StatusCode initialize();
  StatusCode finalize();

  StatusCode getTracks(std::vector<HTTRoad*> const & roads, std::vector<HTTTrack> & tracks);
  StatusCode getMissingHitsCheckTracks(std::vector<HTTTrack> & tracks_guessed);

  StatusCode getNFits(int & n)            { n = m_tfpobj->getNFits();             return StatusCode::SUCCESS; }
  StatusCode getNFitsMajority(int & n)    { n = m_tfpobj->getNFitsMajority();     return StatusCode::SUCCESS; }
  StatusCode getNFitsMajoritySCI(int & n) { n = m_tfpobj->getNFitsMajoritySCI();  return StatusCode::SUCCESS; }
  StatusCode getNFitsMajorityPix(int & n) { n = m_tfpobj->getNFitsMajorityPix();  return StatusCode::SUCCESS; }
  StatusCode getNFitsRecovery(int & n)    { n = m_tfpobj->getNFitsRecovery();     return StatusCode::SUCCESS; }

private:
  TrackFitter* m_tfpobj; // instance of the TrackFitter object

  ServiceHandle<ITrigHTTMappingSvc>   m_HTTMapping;
  ServiceHandle<ITrigHTTBankSvc>   m_HTTBank;

  // steer recovery parameters
  FloatProperty m_chi2dof_recovery_min;
  FloatProperty m_chi2dof_recovery_max;

  BooleanProperty m_do2ndStage;
  BooleanProperty m_doDeltaGPhis;

  IntegerProperty m_DoMajority;
  IntegerProperty m_MaxNhitsPerPlane;
  IntegerProperty m_NoRecoveryNHits;

  int m_FitRemoved;

  bool m_GuessHits;
  bool m_DoMissingHitsChecks;
  int  m_IdealCoordFitType;

};

#endif // HTTTrackFitterTool_h
