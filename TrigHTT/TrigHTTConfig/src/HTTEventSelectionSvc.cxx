/*
    Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TMath.h"

#include "HTTEventSelectionSvc.h"
#include "TrigHTTConfig/HTTRegionSlices.h"
#include "TrigHTTObjects/HTTHit.h"
#include "TrigHTTObjects/HTTTrack.h"
#include "TrigHTTObjects/HTTOfflineTrack.h"
#include "TrigHTTObjects/HTTTruthTrack.h"
#include "TrigHTTObjects/HTTEventInputHeader.h"
#include "TrigHTTObjects/HTTLogicalEventInputHeader.h"
#include "TrigHTTUtils/HTTTypes.h"

HTTEventSelectionSvc::HTTEventSelectionSvc(const std::string& name, ISvcLocator* svc) :
  AthService(name, svc)
{
  declareProperty("regionID",   m_regionID, "current region under processing");
  declareProperty("regions",    m_regions_path = "", "path of the slices file");
  declareProperty("sampleType", m_sampleType = "singleMuons", "type of sample under processing (skipTruth, singleElectrons, singleMuons, singlePions, or LLPs)");
  declareProperty("withPU",     m_withPU, "flag to say if there is pile-up or not");
  declareProperty("doLRT",      m_LRT, "Change track selection to LRT quantities; hit selection unchanged");
  declareProperty("minLRTpT",   m_minLRTpT, "Minimum pT to use in LRT selection, in GeV");
  declareProperty("lrt_truthMatchPDGID", m_LRT_pdgID, "If we are running an LLP sample but want only some PDGID of output in the truth selection, set this");
  declareProperty("allowHighBarcode", m_allowHighBarcode, "Whether or not to allow barcodes over 200000 in truth matching");
}

StatusCode HTTEventSelectionSvc::initialize()
{
  if (!m_regions) createRegions();

  m_min = m_regions->getMin(m_regionID);
  m_max = m_regions->getMax(m_regionID);

  if (m_sampleType.value() == "skipTruth") m_st = SampleType::skipTruth;
  else if (m_sampleType.value() == "singleElectrons") m_st = SampleType::singleElectrons;
  else if (m_sampleType.value() == "singleMuons") m_st = SampleType::singleMuons;
  else if (m_sampleType.value() == "singlePions") m_st = SampleType::singlePions;
  else if (m_sampleType.value() == "LLPs") m_st = SampleType::LLPs;
  else {
    ATH_MSG_ERROR("initialize(): sampleType doesn't exist. ");
    return StatusCode::FAILURE;
  }

  // If we are doing large-radius tracks, we want to restrict the hits entering the
  // process as before, but we will accept any track as belonging to this region that has
  // most of its hits within the region, regardless of eta and phi.
  // We will also set a minimum pT of 5 GeV.
  m_trackmin = m_min;
  m_trackmax = m_max;
  double pTCutVal = 1./(1000.*m_minLRTpT);
  if (m_LRT) {
    m_trackmin.qOverPt = -1*pTCutVal;
    m_trackmin.d0 = -300;
    m_trackmin.z0 = -500;
    m_trackmax.qOverPt = pTCutVal;
    m_trackmax.d0 = 300;
    m_trackmax.z0 = 500;
  }

  // Should really be printing the full information, but it conflicts with
  // the requirement that log files be identical .....
  ATH_MSG_INFO("HTTEventSelectionSvc::initialize(): "       <<
               "regionID = "   << m_regionID                << ", "  <<
               "min = {"       << m_min                     << "}, " <<
               "max = {"       << m_max                     << "}, " <<
               "trackmin = "   << m_trackmin                << ", "  <<
               "trackmax = "   << m_trackmax                << ", "  <<
               "sampleType = " << std::string(m_sampleType) << ", "  <<
               "withPU = "     << static_cast<bool>(m_withPU));

  return StatusCode::SUCCESS;
}

StatusCode HTTEventSelectionSvc::finalize()
{
  if (m_regions) delete m_regions;
  ATH_MSG_INFO("HTTEventSelectionSvc::finalize()");

  return StatusCode::SUCCESS;
}

StatusCode HTTEventSelectionSvc::queryInterface(const InterfaceID& riid, void** ppvIf)
{
  if (interfaceID() == riid)
    *ppvIf = dynamic_cast<HTTEventSelectionSvc*> (this);
  else if (IHTTEventSelectionSvc::interfaceID() == riid)
    *ppvIf = dynamic_cast<IHTTEventSelectionSvc*> (this);
  else
    return AthService::queryInterface(riid, ppvIf);

  addRef();
  return StatusCode::SUCCESS;
}

bool HTTEventSelectionSvc::passCuts(const HTTHit& hit) const
{
  float eta = TMath::ASinH(hit.getGCotTheta());
  float phi = hit.getGPhi();

  if (eta >= m_min.eta && eta <= m_max.eta
      &&  phi >= m_min.phi && phi <= m_max.phi)
    return true;
  else
    return false;
}

bool HTTEventSelectionSvc::passCuts(const HTTTrack& track) const
{
  std::cout << " " << m_trackmin.qOverPt
            << " " << m_trackmin.eta
            << " " << m_trackmin.phi
            << " " << m_trackmin.d0
            << " " << m_trackmin.z0
            << std::endl;
  if (track.getQOverPt() >= m_trackmin.qOverPt && track.getQOverPt() <= m_trackmax.qOverPt
      && track.getEta() >= m_trackmin.eta && track.getEta() <= m_trackmax.eta
      && track.getPhi() >= m_trackmin.phi && track.getPhi() <= m_trackmax.phi
      && track.getD0() >= m_trackmin.d0 && track.getD0() <= m_trackmax.d0
      && track.getZ0() >= m_trackmin.z0 && track.getZ0() <= m_trackmax.z0)
    return true;
  else
    return false;
}

bool HTTEventSelectionSvc::passQOverPt(const HTTTrack& track) const
{
  if (track.getQOverPt() >= m_trackmin.qOverPt && track.getQOverPt() <= m_trackmax.qOverPt)
    return true;
  else
    return false;
}

bool HTTEventSelectionSvc::passEta(const HTTTrack& track) const
{
  if (track.getEta() >= m_trackmin.eta && track.getEta() <= m_trackmax.eta)
    return true;
  else
    return false;
}

bool HTTEventSelectionSvc::passPhi(const HTTTrack& track) const
{
  if (track.getPhi() >= m_trackmin.phi && track.getPhi() <= m_trackmax.phi)
    return true;
  else
    return false;
}

bool HTTEventSelectionSvc::passD0(const HTTTrack& track) const
{
  if (track.getD0() >= m_trackmin.d0 && track.getD0() <= m_trackmax.d0)
    return true;
  else
    return false;
}

bool HTTEventSelectionSvc::passZ0(const HTTTrack& track) const
{
  if (track.getZ0() >= m_trackmin.z0 && track.getZ0() <= m_trackmax.z0)
    return true;
  else
    return false;
}

bool HTTEventSelectionSvc::passCuts(const HTTOfflineTrack& offlineTrack) const
{
  if (offlineTrack.getQOverPt() >= m_trackmin.qOverPt && offlineTrack.getQOverPt() <= m_trackmax.qOverPt
      && offlineTrack.getEta() >= m_trackmin.eta && offlineTrack.getEta() <= m_trackmax.eta
      && offlineTrack.getPhi() >= m_trackmin.phi && offlineTrack.getPhi() <= m_trackmax.phi
      && offlineTrack.getD0() >= m_trackmin.d0 && offlineTrack.getD0() <= m_trackmax.d0
      && offlineTrack.getZ0() >= m_trackmin.z0 && offlineTrack.getZ0() <= m_trackmax.z0)
    return true;
  else
    return false;
}

bool HTTEventSelectionSvc::passQOverPt(const HTTOfflineTrack& offlineTrack) const
{
  if (offlineTrack.getQOverPt() >= m_trackmin.qOverPt && offlineTrack.getQOverPt() <= m_trackmax.qOverPt)
    return true;
  else
    return false;
}

bool HTTEventSelectionSvc::passEta(const HTTOfflineTrack& offlineTrack) const
{
  if (offlineTrack.getEta() >= m_trackmin.eta && offlineTrack.getEta() <= m_trackmax.eta)
    return true;
  else
    return false;
}

bool HTTEventSelectionSvc::passPhi(const HTTOfflineTrack& offlineTrack) const
{
  if (offlineTrack.getPhi() >= m_trackmin.phi && offlineTrack.getPhi() <= m_trackmax.phi)
    return true;
  else
    return false;
}

bool HTTEventSelectionSvc::passD0(const HTTOfflineTrack& offlineTrack) const
{
  if (offlineTrack.getD0() >= m_trackmin.d0 && offlineTrack.getD0() <= m_trackmax.d0)
    return true;
  else
    return false;
}

bool HTTEventSelectionSvc::passZ0(const HTTOfflineTrack& offlineTrack) const
{
  if (offlineTrack.getZ0() >= m_trackmin.z0 && offlineTrack.getZ0() <= m_trackmax.z0)
    return true;
  else
    return false;
}

bool HTTEventSelectionSvc::passCuts(const HTTTruthTrack& truthTrack) const
{
  //return (m_regions->inRegion(m_regionID, truthTrack));
  // Want a version that allows us to modify the selection parameters here.
  // If m_trackmin and m_trackmax are unmodified from the region definitions
  // this should return exactly the same.
  HTTTrackPars cur = truthTrack.getPars();
  for (unsigned i = 0; i < HTTTrackPars::NPARS; i++)
    {
      //if (cur[i] < m_trackmin[i]) return false;
      //if (cur[i] > m_trackmax[i]) return false;
      if (cur[i] < m_trackmin[i]) {
        if (abs(truthTrack.getPDGCode())==13) std::cout << "muon failed on criterion " << i << " falling below minimum: value was " << cur[i] << " and min was " << m_trackmin[i] << "; barcode was " << truthTrack.getBarcode() << std::endl;
        return false;
      }
      if (cur[i] > m_trackmax[i]) {
        if (abs(truthTrack.getPDGCode())==13) std::cout << "muon failed on criterion " << i << " falling above maximum: value was " << cur[i] << " and max was " << m_trackmax[i] << "; barcode was " << truthTrack.getBarcode() << std::endl;
        return false;
      }
    }

  ATH_MSG_DEBUG("Passing it. PDGid = " << truthTrack.getPDGCode() << "; barcode = " << truthTrack.getBarcode());
  return true;
}

bool HTTEventSelectionSvc::passMatching(HTTTrack const & track) const
{
  if (track.getBarcode() == 0 || track.getBarcode() == std::numeric_limits<long>::max()) return false;
  if (track.getEventIndex() == std::numeric_limits<long>::max()) return false;
  if (track.getQOverPt() == 0) return false;

  return true;
}

bool HTTEventSelectionSvc::passMatching(HTTTruthTrack const & truthTrack) const
{
  if (truthTrack.getBarcode() == 0 || truthTrack.getQ() == 0) return false;
  if ((!m_allowHighBarcode) && m_st != SampleType::skipTruth && truthTrack.getBarcode() > 200000) return false;
  if (!passCuts(truthTrack)) return false;

  return true;
}

bool HTTEventSelectionSvc::selectEvent(HTTEventInputHeader* eventHeader) const
{
  if (m_st == SampleType::skipTruth)
    return true;
  else if (m_st == SampleType::singleElectrons || m_st == SampleType::singleMuons || m_st == SampleType::singlePions){
    const auto& truthTracks = eventHeader->optional().getTruthTracks();
    return checkTruthTracks(truthTracks);
  }
  else if (m_st == SampleType::LLPs) {
    const auto& truthTracks = eventHeader->optional().getTruthTracks();
    // Maybe change this later - could be we want LRT selection in all cases but I suspect not
    if (m_LRT) return checkTruthTracksLRT(truthTracks);
    else return checkTruthTracks(truthTracks);
  }
  else {
    ATH_MSG_DEBUG("selectEvent(): Error with sampleType property");
    return false;
  }
}

bool HTTEventSelectionSvc::selectEvent(HTTLogicalEventInputHeader* eventHeader) const
{
  if (m_st == SampleType::skipTruth)
    return true;
  else if (m_st == SampleType::singleElectrons || m_st == SampleType::singleMuons || m_st == SampleType::singlePions){
    const auto& truthTracks = eventHeader->optional().getTruthTracks();
    return checkTruthTracks(truthTracks);
  }
  else if (m_st == SampleType::LLPs) {
    const auto& truthTracks = eventHeader->optional().getTruthTracks();
    // Maybe change this later - could be we want LRT selection in all cases but I suspect not
    if (m_LRT) return checkTruthTracksLRT(truthTracks);
    else return checkTruthTracks(truthTracks);
  }
  else {
    ATH_MSG_DEBUG("selectEvent(): Error with sampleType property");
    return false;
  }

}

const HTTRegionSlices* HTTEventSelectionSvc::getRegions()
{
  if (!m_regions) createRegions();
  return m_regions;
}

void HTTEventSelectionSvc::createRegions()
{
  if (!m_regions)
    {
      ATH_MSG_INFO("Creating the slices object");
      MsgStream cmsg(msgSvc(), "HTTRegionSlices");
      cmsg.setLevel(msg().level()); // cause AthMessaging is stupid and doesn't have this function
      m_regions = new HTTRegionSlices(cmsg, m_regions_path.value());
    }
}

bool HTTEventSelectionSvc::checkTruthTracks(const std::vector<HTTTruthTrack>& truthTracks) const
{

// find at least one track in the region
  bool good=false;
  for (auto track : truthTracks){
    if(m_regions->inRegion(m_regionID, track)){
        good=true;
        if (abs(track.getPDGCode()) != static_cast<int>(m_st)){
          ATH_MSG_WARNING("selectEvent(): TruthTrack PDGCode != sampleType");
          good=false;
        }
        else {
          ATH_MSG_DEBUG("selectEvent(): found one truth track, in region "
                  <<getRegionID() <<"; track pars: "<< truthTracks.front().getPars());
          break;
        }
    }
    else ATH_MSG_DEBUG("selectEvent(): found one truth track over "<<truthTracks.size()<<", out of region "
                  <<getRegionID() <<"; track pars: "<< truthTracks.front().getPars());
  }
  return good;

}

bool HTTEventSelectionSvc::checkTruthTracksLRT(const std::vector<HTTTruthTrack>& truthTracks) const
{
  ATH_MSG_DEBUG("selectEvent(): Checking truth tracks with LRT requirements");
  // Ideally we would make this flexible to use either of two conditions
  // 1) there is a truth track that passes our region criteria with our modified selections, or
  // 2) there is a truth track that passes all but the eta and phi requirements, but has
  // the majority of its associated hits in the region.

  // Check for 1)
  for (const auto& truthtrack : truthTracks ) {
    //std::cout << truthtrack.getPars() << "; pass/fail " << passCuts(truthtrack) << std::endl;
    // If we specified a PDG ID selection, it can go here - but by default this is false
    if ((m_LRT_pdgID != 0) && (m_LRT_pdgID != truthtrack.getPDGCode())) {
      if (passCuts(truthtrack)) std::cout << "Skipping an otherwise passing track due to wrong PDGID: " << truthtrack.getPDGCode() << std::endl;
      continue;
    }
    if (passCuts(truthtrack)) return true;
  }

  // Check for 2)
  // TODO someday.


  // Otherwise return false
  return false;
}
