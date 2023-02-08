/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTAlgorithms/HTTTrackFitterTool.h"
#include "TrigHTTMaps/HTTPlaneMap.h"
#include "TrigHTTMaps/HTTRegionMap.h"
#include "TrigHTTMaps/HTTSSMap.h"
#include "TrigHTTUtils/HTTFunctions.h"

#include "GaudiKernel/MsgStream.h"

#include <sstream>
#include <iostream>
#include <fstream>

static const InterfaceID IID_HTTTrackFitterTool("HTTTrackFitterTool", 1, 0);
const InterfaceID& HTTTrackFitterTool::interfaceID()
{ return IID_HTTTrackFitterTool; }


/////////////////////////////////////////////////////////////////////////////
HTTTrackFitterTool::HTTTrackFitterTool(const std::string& algname, const std::string& name, const IInterface *ifc) :
  AthAlgTool(algname,name,ifc),
  m_tfpobj(0x0),
  m_HTTMapping("TrigHTTMappingSvc", name),
  m_HTTBank("TrigHTTBankSvc", name),
  m_chi2dof_recovery_min(40.),
  m_chi2dof_recovery_max(1e30),
  m_do2ndStage(false),
  m_doDeltaGPhis(false),
  m_DoMajority(1),
  m_MaxNhitsPerPlane(-1),
  m_NoRecoveryNHits(-1),
  m_GuessHits(true),
  m_DoMissingHitsChecks(false),
  m_IdealCoordFitType(0)
{
  declareProperty("chi2dofrecoverymin",m_chi2dof_recovery_min,"min chi^2 cut for attempting recovery fits");
  declareProperty("chi2dofrecoverymax",m_chi2dof_recovery_max,"max chi^2 for still attempting recovery fits");
  declareProperty("doMajority",m_DoMajority);
  declareProperty("maxHitsPerPlane", m_MaxNhitsPerPlane);
  declareProperty("nHits_noRecovery",m_NoRecoveryNHits);
  declareProperty("Do2ndStageTrackFit",m_do2ndStage);
  declareProperty("GuessHits", m_GuessHits, "If True then we Guess hits, if False then we use separate banks and don't guess");
  declareProperty("DoMissingHitsChecks", m_DoMissingHitsChecks, "If True and we guess hits, when we have 8/8 we also drop hits and guess them to compare to true positions");
  declareProperty("IdealCoordFitType", m_IdealCoordFitType, "Fit types, potentially using idealized coordinates");
  declareProperty("DoDeltaGPhis", m_doDeltaGPhis, "If True will do the fit by the delta global phis method");
  declareInterface<HTTTrackFitterTool>(this);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTTrackFitterTool::initialize()
{
    ATH_MSG_DEBUG("HTTTrackFitterTool::initialize()");

    ATH_CHECK(m_HTTMapping.retrieve());
    ATH_CHECK(m_HTTBank.retrieve());

    HTTFitConstantBank* nominalbank;
    std::vector<HTTFitConstantBank*> bankvec;

    if(!m_do2ndStage){
      nominalbank = m_HTTBank->FitConstantBank_1st();
      if (!m_GuessHits) {
        for (unsigned int iplane = 0; iplane < m_HTTMapping->PlaneMap_1st()->getNLogiLayers(); iplane++) {
	  HTTFitConstantBank* bank = m_HTTBank->FitConstantBank_1st(iplane);
	  bankvec.push_back(bank);
        }
      }
    }
    else {
      nominalbank = m_HTTBank->FitConstantBank_2nd();
      if (!m_GuessHits) {
        for (unsigned int iplane = 0; iplane < m_HTTMapping->PlaneMap_2nd()->getNLogiLayers(); iplane++) {
          HTTFitConstantBank* bank = m_HTTBank->FitConstantBank_2nd(iplane);
          bankvec.push_back(bank);
        }
      }
    }

    MsgStream m(msgSvc(), "TrackFitter");
    m.setLevel(msg().level()); // cause AthMessaging is stupid and doesn't have this function // in r22!
    m_tfpobj = new TrackFitter(m, nominalbank, bankvec, m_GuessHits);

    if(!m_do2ndStage)
      m_tfpobj->setPlaneMap(m_HTTMapping->PlaneMap_1st());
    else
      m_tfpobj->setPlaneMap(m_HTTMapping->PlaneMap_2nd());

    // set parameter object to TrackFitter
    m_tfpobj->setChi2DofRecoveryMin(m_chi2dof_recovery_min);
    m_tfpobj->setChi2DofRecoveryMax(m_chi2dof_recovery_max);

    m_tfpobj->setDoMajority(m_DoMajority);

    m_tfpobj->setRequireFirst(0);
    m_tfpobj->setDoSecondStage(m_do2ndStage);

    m_tfpobj->setNorecoveryNhits(m_NoRecoveryNHits);

    if (!m_GuessHits && m_DoMissingHitsChecks) ATH_MSG_WARNING("We can't do missing hits check if we don't guess hits");
    m_tfpobj->setDoMissingHitsCheck(m_DoMissingHitsChecks);

    m_tfpobj->setIdealCoordFitType(static_cast<TrackCorrType>(m_IdealCoordFitType));
    m_tfpobj->setDoDeltaGPhis(m_doDeltaGPhis);

    m_tfpobj->init();
    return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTTrackFitterTool::getTracks(const std::vector<HTTRoad*>& roads, std::vector<HTTTrack>& tracks) {
    // elaborate the next event

    int status = m_tfpobj->fitTracks(roads,tracks);
    if (status != FITTRACKS_OK) return StatusCode::FAILURE;

    if (msgLvl(MSG::DEBUG))
    {
        ATH_MSG_DEBUG("getTracks() returning " << tracks.size() << " tracks:");
        for (auto & t : tracks)
            ATH_MSG_DEBUG("\tchi2 = " << t.getChi2() << " and phi = " << t.getPhi()
                    << " and eta = " << t.getEta() << " and d0 = " << t.getD0()
                    << " and z0 = " << t.getZ0() << "and pt = " << t.getPt());
    }

    return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTTrackFitterTool::getMissingHitsCheckTracks(std::vector<HTTTrack>& tracks_guessed)
{
    // only call this after we first fit the tracks!
    tracks_guessed = m_tfpobj->getMissingHitsCheckTracks();
    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTTrackFitterTool::finalize()
{
    ATH_MSG_DEBUG("finalize()");
    delete m_tfpobj;
    return StatusCode::SUCCESS;
}
