/*
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTMonitor/HTTOverlapRemovalMonitorTool.h"

#include "TrigHTTBanks/HTTSectorSlice.h"
#include "TrigHTTConfig/IHTTEventSelectionSvc.h"
#include "TrigHTTObjects/HTTTruthTrack.h"

static const InterfaceID IID_HTTOverlapRemovalMonitorTool("HTTOverlapRemovalMonitorTool", 1, 0);
const InterfaceID& HTTOverlapRemovalMonitorTool::interfaceID()
{
  return IID_HTTOverlapRemovalMonitorTool;
}

HTTOverlapRemovalMonitorTool::HTTOverlapRemovalMonitorTool(const std::string& algname, const std::string& name, const IInterface *ifc):
    HTTMonitorBase(algname,name,ifc),
    m_EvtSel("HTTEventSelectionSvc", name)
{
  declareInterface<HTTOverlapRemovalMonitorTool>(this);
  declareProperty("BarcodeFracCut",       m_cut_barcodefrac,      "cut on barcode fraction used in truth matching");
}

StatusCode HTTOverlapRemovalMonitorTool::initialize()
{
  ATH_MSG_INFO(HTTMonitorBase::initialize());

  ATH_CHECK(m_EvtSel.retrieve());

  return StatusCode::SUCCESS;
}

StatusCode HTTOverlapRemovalMonitorTool::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode HTTOverlapRemovalMonitorTool::bookTracksHistograms()
{
  setHistDir("/ORMONITOR/All_hist/");

  h_nTracks = new TH1F("h_nTracks", "Number of tracks per event; # of Tracks; Events", 2000, 0, 2000);
  ATH_CHECK(regHist(getHistDir(),h_nTracks));

  setHistDir("/ORMONITOR/OR_hist/");

  //  Book histrogram
  h_OR_nTracks = new TH1F("h_OR_nTracks", "Number of tracks per event post OR; # of Tracks; Events", 2000, 0, 2000);
  ATH_CHECK(regHist(getHistDir(), h_OR_nTracks));
  h_OR_chi2ndof = new TH1F("h_OR_chi2ndof", "Chi2ndof of tracks post OR; chi2; Events", 100, 0, 100);
  ATH_CHECK(regHist(getHistDir(), h_OR_chi2ndof));
  h_OR_pt = new TH1F("h_OR_pt", "pt of tracks post OR; pt; Events", getNXbinsPT(), m_xbinsPT);
  ATH_CHECK(regHist(getHistDir(), h_OR_pt));
  h_OR_phi = new TH1F("h_OR_phi", "phi of tracks post OR; phi; Events", 40, m_phimin, m_phimax);
  ATH_CHECK(regHist(getHistDir(), h_OR_phi));
  h_OR_eta = new TH1F("h_OR_eta", "eta of tracks post OR; eta; Events", 40, m_etamin, m_etamax);
  ATH_CHECK(regHist(getHistDir(), h_OR_eta));
  h_OR_d0 = new TH1F("h_OR_d0", "d0 of tracks post OR; d0; Events", 100, -2.5, 2.5);
  ATH_CHECK(regHist(getHistDir(), h_OR_d0));
  h_OR_z0 = new TH1F("h_OR_z0", "z0 of tracks post OR; z0; Evants", 100, -200., 200.);
  ATH_CHECK(regHist(getHistDir(), h_OR_z0));

  setHistDir("/ORMONITOR/bestTrack_hist/");

  h_bestTrack_nTracks = new TH1F("h_bestTrack_nTracks", "Number of best track per event", 2000, 0, 2000);
  ATH_CHECK(regHist(getHistDir(), h_bestTrack_nTracks));
  h_bestTrack_pt = new TH1F("h_bestTrack_pt", "best track track pt; pt; Events", getNXbinsPT(), m_xbinsPT);
  ATH_CHECK(regHist(getHistDir(), h_bestTrack_pt));
  h_bestTrack_phi = new TH1F("h_bestTrack_phi", "best track track phi; phi; Events", 40, m_phimin, m_phimax);
  ATH_CHECK(regHist(getHistDir(), h_bestTrack_phi));
  h_bestTrack_eta = new TH1F("h_bestTrack_eta", "best track track eta; eta; Events", 40, m_etamin, m_etamax);
  ATH_CHECK(regHist(getHistDir(), h_bestTrack_eta));
  h_bestTrack_d0 = new TH1F("h_bestTrack_d0", "best track track d0; d0; Events", 100, -2.5, 2.5);
  ATH_CHECK(regHist(getHistDir(), h_bestTrack_d0));
  h_bestTrack_z0 = new TH1F("h_bestTrack_z0", "best track track z0; z0; Events", 100, -200., 200.);
  ATH_CHECK(regHist(getHistDir(), h_bestTrack_z0));
  h_bestTrack_chi2ndof = new TH1F("h_bestTrack_chi2ndof", "best track chi2ndof; chi2; Evets", 100, 0, 100);
  ATH_CHECK(regHist(getHistDir(), h_bestTrack_chi2ndof));

  return StatusCode::SUCCESS;
}

StatusCode HTTOverlapRemovalMonitorTool::bookResolutionHistograms()
{
  setHistDir("/ORMONITOR/Res_hist/");

  //  OR Res
  h_OR_res_pt = new TH1F("h_OR_res_pt", "pt resolution of post OR track", 200, -10, 10);
  ATH_CHECK(regHist(getHistDir(), h_OR_res_pt));
  h_OR_res_phi = new TH1F("h_OR_res_phi", "phi resolution of post OR track", 100, -0.01, 0.01);
  ATH_CHECK(regHist(getHistDir(), h_OR_res_phi));
  h_OR_res_eta = new TH1F("h_OR_res_eta", "eta resolution of post OR track", 100, -0.01, 0.01);
  ATH_CHECK(regHist(getHistDir(), h_OR_res_eta));
  h_OR_res_d0 = new TH1F("h_OR_res_d0", "d0 resolution of post OR track", 100, -1, 1);
  ATH_CHECK(regHist(getHistDir(), h_OR_res_d0));
  h_OR_res_z0 = new TH1F("h_OR_res_z0", "z0 resolution of post OR track", 100, -2, 2);
  ATH_CHECK(regHist(getHistDir(), h_OR_res_z0));

  //  Best OR
  h_best_res_pt = new TH1F("h_best_res_pt", "pt resolution of post best track", 200, -10, 10);
  ATH_CHECK(regHist(getHistDir(), h_best_res_pt));
  h_best_res_phi = new TH1F("h_best_res_phi", "phi resolution of post best track", 100, -0.01, 0.01);
  ATH_CHECK(regHist(getHistDir(), h_best_res_phi));
  h_best_res_eta = new TH1F("h_best_res_eta", "eta resolution of post best track", 100, -0.01, 0.01);
  ATH_CHECK(regHist(getHistDir(), h_best_res_eta));
  h_best_res_d0 = new TH1F("h_best_res_d0", "d0 resolution of post best track", 100, -1, 1);
  ATH_CHECK(regHist(getHistDir(), h_best_res_d0));
  h_best_res_z0 = new TH1F("h_best_res_z0", "z0 resolution of post best track", 100, -2, 2);
  ATH_CHECK(regHist(getHistDir(), h_best_res_z0));

  return StatusCode::SUCCESS;
}

StatusCode HTTOverlapRemovalMonitorTool::bookEfficiencyHistograms()
{
  setHistDir("/ORMONITOR/Eff_hist/");

  //  OR eff
  h_OR_eff_pt = new TGraphAsymmErrors(); h_OR_eff_pt->SetNameTitle("h_OR_eff_pt","OR efficiencies as function of truth pt; pt; eff");
  ATH_CHECK(regGraph(getHistDir(), h_OR_eff_pt));
  h_OR_eff_phi = new TGraphAsymmErrors(); h_OR_eff_phi->SetNameTitle("h_OR_eff_phi", "OR efficiencies as function of truth phi; phi; eff");
  ATH_CHECK(regGraph(getHistDir(), h_OR_eff_phi));
  h_OR_eff_eta = new TGraphAsymmErrors(); h_OR_eff_eta->SetNameTitle("h_OR_eff_eta", "OR efficiencies as function of truth eta; eta; eff");
  ATH_CHECK(regGraph(getHistDir(), h_OR_eff_eta));
  h_OR_eff_d0 = new TGraphAsymmErrors(); h_OR_eff_d0->SetNameTitle("h_OR_eff_d0", "OR efficiencies as function of truth d0; d0; eff");
  ATH_CHECK(regGraph(getHistDir(), h_OR_eff_d0));
  h_OR_eff_z0 = new TGraphAsymmErrors(); h_OR_eff_z0->SetNameTitle("h_OR_eff_z0", "OR efficiencies as function of truth z0; z0; eff");
  ATH_CHECK(regGraph(getHistDir(), h_OR_eff_z0));

  //  Best eff
  h_best_eff_pt = new TGraphAsymmErrors(); h_best_eff_pt->SetNameTitle("h_best_eff_pt","best efficiencies as function of truth pt; pt; eff");
  ATH_CHECK(regGraph(getHistDir(), h_best_eff_pt));
  h_best_eff_phi = new TGraphAsymmErrors(); h_best_eff_phi->SetNameTitle("h_best_eff_phi", "best efficiencies as function of truth phi; phi; eff");
  ATH_CHECK(regGraph(getHistDir(), h_best_eff_phi));
  h_best_eff_eta = new TGraphAsymmErrors(); h_best_eff_eta->SetNameTitle("h_best_eff_eta", "best efficiencies as function of truth eta; eta; eff");
  ATH_CHECK(regGraph(getHistDir(), h_best_eff_eta));
  h_best_eff_d0 = new TGraphAsymmErrors(); h_best_eff_d0->SetNameTitle("h_best_eff_d0", "best efficiencies as function of truth d0; d0; eff");
  ATH_CHECK(regGraph(getHistDir(), h_best_eff_d0));
  h_best_eff_z0 = new TGraphAsymmErrors(); h_best_eff_z0->SetNameTitle("h_best_eff_z0", "best efficiencies as function of truth z0; z0; eff");
  ATH_CHECK(regGraph(getHistDir(), h_best_eff_z0));

  return StatusCode::SUCCESS;
}

StatusCode HTTOverlapRemovalMonitorTool::bookTruthHistogrmas()
{
  setHistDir("/ORMONITOR/truth_hist/");

  //  Truth with OR
  h_truth_with_OR_pt = new TH1F("h_truth_with_OR_pt", "truth track pt with OR; pt; Events", getNXbinsPT(), m_xbinsPT);
  ATH_CHECK(regHist(getHistDir(), h_truth_with_OR_pt));
  h_truth_with_OR_phi = new TH1F("h_truth_with_OR_phi", "truth track phi with OR; phi; Events", 40, m_phimin, m_phimax);
  ATH_CHECK(regHist(getHistDir(), h_truth_with_OR_phi));
  h_truth_with_OR_eta = new TH1F("h_truth_with_OR_eta", "truth track eta with OR; eta; Events", 40, m_etamin, m_etamax);
  ATH_CHECK(regHist(getHistDir(), h_truth_with_OR_eta));
  h_truth_with_OR_d0 = new TH1F("h_truth_with_OR_d0", "truth track d0 with OR; d0; Events", 100, -2.5, 2.5);
  ATH_CHECK(regHist(getHistDir(), h_truth_with_OR_d0));
  h_truth_with_OR_z0 = new TH1F("h_truth_with_OR_z0", "truth track z0 with OR; z0; Events", 100, -200, 200);
  ATH_CHECK(regHist(getHistDir(), h_truth_with_OR_z0));

  //  Truth with best (chi2 < 40)
  h_truth_with_best_pt = new TH1F("h_truth_with_best_pt", "truth track pt with best track; pt; Events", getNXbinsPT(), m_xbinsPT);
  ATH_CHECK(regHist(getHistDir(), h_truth_with_best_pt));
  h_truth_with_best_phi = new TH1F("h_truth_with_best_phi", "truth track phi with best track; phi; Events", 40, m_phimin, m_phimax);
  ATH_CHECK(regHist(getHistDir(), h_truth_with_best_phi));
  h_truth_with_best_eta = new TH1F("h_truth_with_best_eta", "truth track eta with best track; eta; Events", 40, m_etamin, m_etamax);
  ATH_CHECK(regHist(getHistDir(), h_truth_with_best_eta));
  h_truth_with_best_d0 = new TH1F("h_truth_with_best_d0", "truth track d0 with best track; d0; Events", 100, -2.5, 2.5);
  ATH_CHECK(regHist(getHistDir(), h_truth_with_best_d0));
  h_truth_with_best_z0 = new TH1F("h_truth_with_best_z0", "truth track z0 with best track; z0; Events", 100, -200, 200);
  ATH_CHECK(regHist(getHistDir(), h_truth_with_best_z0));

  //  All truth
  h_truth_pt = new TH1F("h_truth_pt", "truth track pt; pt; Events", getNXbinsPT(), m_xbinsPT);
  ATH_CHECK(regHist(getHistDir(), h_truth_pt));
  h_truth_phi = new TH1F("h_truth_phi", "truth track phi; phi; Events", 40, m_phimin, m_phimax);
  ATH_CHECK(regHist(getHistDir(), h_truth_phi));
  h_truth_eta = new TH1F("h_truth_eta", "truth track eta; eta; Evetns", 40, m_etamin, m_etamax);
  ATH_CHECK(regHist(getHistDir(), h_truth_eta));
  h_truth_d0 = new TH1F("h_truth_d0", "truth track d0; d0; Events", 100, -2.5, 2.5);
  ATH_CHECK(regHist(getHistDir(), h_truth_d0));
  h_truth_z0 = new TH1F("h_truth_z0", "truth track z0; z0; Events", 100, -200., 200.);
  ATH_CHECK(regHist(getHistDir(), h_truth_z0));

  return StatusCode::SUCCESS;
}

StatusCode HTTOverlapRemovalMonitorTool::bookHistograms()
{
  ATH_CHECK(bookTracksHistograms());
  //  Book the following histogrmas for single muon only
  if(m_EvtSel->getSampleType() == SampleType::singleMuons)
  {
    ATH_CHECK(bookEfficiencyHistograms());
    ATH_CHECK(bookResolutionHistograms());
    ATH_CHECK(bookTruthHistogrmas());
  }

  clearHistDir();

  return StatusCode::SUCCESS;
}

StatusCode HTTOverlapRemovalMonitorTool::fillAllTrackHistograms(std::vector<HTTTrack> const & tracks)
{
  ATH_MSG_DEBUG("Hist: nTracks is "<<tracks.size());
  h_nTracks->Fill(tracks.size());

  return StatusCode::SUCCESS;
}

StatusCode HTTOverlapRemovalMonitorTool::fillORHistograms(std::vector<HTTTrack> const & tracks)
{

  // Create variable to count the number of track pass OR
  int count_passedOR = 0;

  for(auto const & ORtrack : tracks)
  {
    //  If the track passed OR, fill its properties
    if(ORtrack.passedOR() == 1)
    {
      count_passedOR++;
      h_OR_chi2ndof->Fill(ORtrack.getChi2ndof());
      h_OR_pt->Fill(ORtrack.getPt()*0.001);
      h_OR_phi->Fill(ORtrack.getPhi());
      h_OR_eta->Fill(ORtrack.getEta());
      h_OR_d0->Fill(ORtrack.getD0());
      h_OR_z0->Fill(ORtrack.getZ0());
    }
  }

  h_OR_nTracks->Fill(count_passedOR);

  ATH_MSG_DEBUG("Number of track passed OR: "<<count_passedOR);
  ATH_MSG_DEBUG("Number of track failed OR: "<<tracks.size()-count_passedOR);

  return StatusCode::SUCCESS;
}

StatusCode HTTOverlapRemovalMonitorTool::fillbestTrackHistograms(std::vector<HTTTrack> const & tracks)
{
  //  Best track definition is an HTTTrack that has chi2<40
  int counter_bestTrack = 0;
  for(auto const & track : tracks)
  {
    if(track.getChi2ndof()<40)
    {
      counter_bestTrack++;
      h_bestTrack_pt->Fill(track.getPt()*0.001);
      h_bestTrack_phi->Fill(track.getPhi());
      h_bestTrack_eta->Fill(track.getEta());
      h_bestTrack_d0->Fill(track.getD0());
      h_bestTrack_z0->Fill(track.getZ0());
      h_bestTrack_chi2ndof->Fill(track.getChi2ndof());
    }
  }

  h_bestTrack_nTracks->Fill(counter_bestTrack);

  return StatusCode::SUCCESS;
}

StatusCode HTTOverlapRemovalMonitorTool::fillEfficiencyHistograms()
{
  h_OR_eff_pt->Divide(h_truth_with_OR_pt, h_truth_pt);//,"pois");
  h_OR_eff_phi->Divide(h_truth_with_OR_phi, h_truth_phi);//,"pois");
  h_OR_eff_eta->Divide(h_truth_with_OR_eta, h_truth_eta);//,"pois");
  h_OR_eff_d0->Divide(h_truth_with_OR_d0, h_truth_d0);//,"pois");
  h_OR_eff_z0->Divide(h_truth_with_OR_z0, h_truth_z0);//,"pois");

  h_best_eff_pt->Divide(h_truth_with_best_pt, h_truth_pt);//,"pois");
  h_best_eff_phi->Divide(h_truth_with_best_phi, h_truth_phi);//,"pois");
  h_best_eff_eta->Divide(h_truth_with_best_eta, h_truth_eta);//,"pois");
  h_best_eff_d0->Divide(h_truth_with_best_d0, h_truth_d0);//,"pois");
  h_best_eff_z0->Divide(h_truth_with_best_z0, h_truth_z0);//,"pois");

  return StatusCode::SUCCESS;
}

StatusCode HTTOverlapRemovalMonitorTool::fillORTruthMatchHistograms(std::vector<HTTTruthTrack> const & truth_tracks, std::vector<HTTTrack> const & tracks)
{
  //  Methods below are copied and modified from PerformanceMonitorTool
  HTTBarcodeMM httmatchinfo;

  for (HTTTrack const & htt_t : tracks)
  {
    if (htt_t.getEventIndex()==0)
    {
      // Information on HTT tracks relative to the hard-scattering events
      // are collected in a vector and later used to build matching maps
      if (htt_t.getBarcodeFrac()>m_cut_barcodefrac)
      {
        // We only consider tracks that passed OR
        if(htt_t.passedOR() == 1)
        {
          httmatchinfo.insert(std::pair<HTTMatchInfo, const HTTTrack*>(HTTMatchInfo(htt_t.getBarcode(),htt_t.getEventIndex()),&htt_t));
        }
      }
    }
  }

  for(auto truth_t : truth_tracks)
  {
    if(truth_t.getBarcode()>100000 || truth_t.getBarcode()==0) continue;
    if(truth_t.getEventIndex()!=0 && truth_t.getQ()==0) continue;
    if(!m_EvtSel->passCuts(truth_t)) continue;

    // Fill all truth tracks
    h_truth_pt->Fill(truth_t.getPt()*0.001);
    h_truth_phi->Fill(truth_t.getPhi());
    h_truth_eta->Fill(truth_t.getEta());
    h_truth_d0->Fill(truth_t.getD0());
    h_truth_z0->Fill(truth_t.getZ0());

    HTTMatchInfo reftruth(truth_t.getBarcode(),truth_t.getEventIndex());
    std::pair<HTTBarcodeMM::const_iterator,HTTBarcodeMM::const_iterator> mrange = httmatchinfo.equal_range(reftruth);
    if (mrange.first != mrange.second)
    {
      // Fill truth track with OR matched
      h_truth_with_OR_pt->Fill(truth_t.getPt()*0.001);
      h_truth_with_OR_phi->Fill(truth_t.getPhi());
      h_truth_with_OR_eta->Fill(truth_t.getEta());
      h_truth_with_OR_d0->Fill(truth_t.getD0());
      h_truth_with_OR_z0->Fill(truth_t.getZ0());


      const HTTTrack *bestORhtt(0x0);
      // Find the best HTT post OR based on BarcodeFrac()
      for(HTTBarcodeMM::const_iterator httI = mrange.first;httI!=mrange.second;++httI)
      {
        if (!bestORhtt)
        {
          bestORhtt = (*httI).second;
        } else if (bestORhtt->getBarcodeFrac()<(*httI).second->getBarcodeFrac())
        {
          bestORhtt = (*httI).second;
        }
      }

      // Fill the resolution of best HTT post OR
      if (bestORhtt) {
        h_OR_res_pt->Fill(truth_t.getPt()*0.001 - bestORhtt->getPt()*0.001);
        h_OR_res_phi->Fill(truth_t.getPhi() - bestORhtt->getPhi());
        h_OR_res_eta->Fill(truth_t.getEta() - bestORhtt->getEta());
        h_OR_res_d0->Fill(truth_t.getD0() - bestORhtt->getD0());
        h_OR_res_z0->Fill(truth_t.getZ0() - bestORhtt->getZ0());

      }
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode HTTOverlapRemovalMonitorTool::fillBestTruthMatchHistograms(std::vector<HTTTruthTrack> const & truth_tracks, std::vector<HTTTrack> const & tracks)
{
  //  Methods below are copied and modified from PerformanceMonitorTool
  HTTBarcodeMM httmatchinfo;

  for (HTTTrack const & htt_t : tracks)
  {
    if (htt_t.getEventIndex()==0)
    {
      // Information on HTT tracks relative to the hard-scattering events
      // are collected in a vector and later used to build matching maps
      if (htt_t.getBarcodeFrac()>m_cut_barcodefrac)
      {
        // We only consider tracks that chi2 < 40
        if(htt_t.getChi2ndof() <40 )
        {
          httmatchinfo.insert(std::pair<HTTMatchInfo, const HTTTrack*>(HTTMatchInfo(htt_t.getBarcode(),htt_t.getEventIndex()),&htt_t));
        }
      }
    }
  }

  for(auto truth_t : truth_tracks)
  {
    if(truth_t.getBarcode()>100000 || truth_t.getBarcode()==0) continue;
    if(truth_t.getEventIndex()!=0 && truth_t.getQ()==0) continue;
    if(!m_EvtSel->passCuts(truth_t)) continue;

    HTTMatchInfo reftruth(truth_t.getBarcode(),truth_t.getEventIndex());
    std::pair<HTTBarcodeMM::const_iterator,HTTBarcodeMM::const_iterator> mrange = httmatchinfo.equal_range(reftruth);
    if (mrange.first != mrange.second)
    {
      // Fill truth track with OR matched
      h_truth_with_best_pt->Fill(truth_t.getPt()*0.001);
      h_truth_with_best_phi->Fill(truth_t.getPhi());
      h_truth_with_best_eta->Fill(truth_t.getEta());
      h_truth_with_best_d0->Fill(truth_t.getD0());
      h_truth_with_best_z0->Fill(truth_t.getZ0());


      const HTTTrack *bestHtt(0x0);
      // Find the best HTT post OR based on BarcodeFrac()
      for(HTTBarcodeMM::const_iterator httI = mrange.first;httI!=mrange.second;++httI)
      {
        if (!bestHtt)
        {
          bestHtt = (*httI).second;
        } else if (bestHtt->getBarcodeFrac()<(*httI).second->getBarcodeFrac())
        {
          bestHtt = (*httI).second;
        }
      }

      // Fill the resolution of best HTT post OR
      if (bestHtt) {
        h_best_res_pt->Fill(truth_t.getPt()*0.001 - bestHtt->getPt()*0.001);
        h_best_res_phi->Fill(truth_t.getPhi() - bestHtt->getPhi());
        h_best_res_eta->Fill(truth_t.getEta() - bestHtt->getEta());
        h_best_res_d0->Fill(truth_t.getD0() - bestHtt->getD0());
        h_best_res_z0->Fill(truth_t.getZ0() - bestHtt->getZ0());

      }
    }
  }

  return StatusCode::SUCCESS;
}
