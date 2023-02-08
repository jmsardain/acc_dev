/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTMonitor/HTTMonitorBase.h"

#include "TrigHTTBanks/HTTSectorSlice.h"
#include "TrigHTTMaps/HTTPlaneMap.h"
#include "TrigHTTUtils/HTTFunctions.h"

#include "TProfile.h"

static const InterfaceID IID_HTTMonitorBase("HTTMonitorBase", 1, 0);
const InterfaceID& HTTMonitorBase::interfaceID()
{ return IID_HTTMonitorBase; }


/////////////////////////////////////////////////////////////////////////////
HTTMonitorBase::HTTMonitorBase(std::string const & algname, std::string const & name, IInterface const * ifc) :
    AthAlgTool(algname,name,ifc),
    m_tHistSvc("THistSvc", name),
    m_HTTMapping("TrigHTTMappingSvc", name),
    m_evtSel("HTTEventSelectionSvc", name)
{
    declareInterface<HTTMonitorBase>(this);
    declareProperty("CheckHistRange",       m_checkHistRange,       "Check all the histogram ranges");
    declareProperty("eventSelector",        m_evtSel,               "Event selector tool to use");
    declareProperty("outputTag",            m_outputtag = "",       "Extra string to use in output folder names - default none");
    declareProperty("d0Range",              m_d0Range,              "d0 range to use in histogram axes");
    declareProperty("z0Range",              m_z0Range,              "z0 range to use in histogram axes");
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTMonitorBase::initialize()
{
    ATH_CHECK(m_tHistSvc.retrieve());
    ATH_CHECK(m_HTTMapping.retrieve());
    ATH_CHECK(m_evtSel.retrieve());

    TDirectory *theDirectory = gDirectory;
    theDirectory->cd();
    TH1::StatOverflows(kTRUE); // added here, should work, otherwise we have to add on regHist() for each single hitsogrma

    // TODO change these to region slice?
    m_etamin = m_evtSel->getMin().eta-0.1;
    m_etamax = m_evtSel->getMax().eta+0.1;
    m_phimin = m_evtSel->getMin().phi-0.1;
    m_phimax = m_evtSel->getMax().phi+0.1;

    ATH_CHECK(m_d0Range.size() == 2);
    ATH_CHECK(m_z0Range.size() == 2);

    m_nLayers_1st = m_HTTMapping->PlaneMap_1st()->getNLogiLayers();
    m_nLayers_2nd = m_HTTMapping->PlaneMap_2nd()->getNLogiLayers();

    m_withPU = m_evtSel->checkPU();

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTMonitorBase::passMonitorEvent(HTTEventInputHeader* h,
                                            HTTLogicalEventInputHeader* h_in_1st, HTTLogicalEventInputHeader* h_in_2nd,
                                            HTTLogicalEventOutputHeader* h_out)
{
    m_eventInputHeader          = h;
    m_logicEventInputHeader_1st = h_in_1st;
    m_logicEventInputHeader_2nd = h_in_2nd;
    m_logicEventOutputHeader    = h_out;

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTMonitorBase::passMonitorHits(std::vector<HTTHit> const & hits, std::string const & key)
{
    if (m_monitorHits.find(key) != m_monitorHits.end()) m_monitorHits.erase(key);
    m_monitorHits.insert({key, &hits});
    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTMonitorBase::passMonitorClusters(std::vector<HTTCluster> const & clusters, std::string const & key)
{
    if (m_monitorClusters.find(key) != m_monitorClusters.end()) m_monitorClusters.erase(key);
    m_monitorClusters.insert({key, &clusters});
    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTMonitorBase::passMonitorRoads(std::vector<HTTRoad*> const & roads, std::string const & key)
{
    if (m_monitorRoads.find(key) != m_monitorRoads.end()) m_monitorRoads.erase(key);
    m_monitorRoads.insert({key, &roads});
    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTMonitorBase::passMonitorTracks(std::vector<HTTTrack> const & tracks, std::string const & key)
{
    if (m_monitorTracks.find(key) != m_monitorTracks.end()) m_monitorTracks.erase(key);
    m_monitorTracks.insert({key, &tracks});
    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTMonitorBase::printHistograms(PrintDetail detail) const
{
    detail = ALL;

    if (detail == ALL) {

        ATH_MSG_INFO("=====================================");
        ATH_MSG_INFO("Now Print all the histograms.");

        std::string current_dir = "";

        for (std::string hist : m_tHistSvc->getHists()) {

            std::string this_dir = hist.substr(0, hist.find_last_of("/"));

            if (this_dir.compare(current_dir)) {
                current_dir = this_dir;
                ATH_MSG_INFO("****************************************************************************************************");
                ATH_MSG_INFO("Current directory: " << current_dir);
                ATH_MSG_INFO("****************************************************************************************************");
            }

            TH1* h;

            if (m_tHistSvc->getHist(hist, h).isFailure())
                ATH_MSG_ERROR("Histogram with directory " << hist << " does not exist.");
            else
                ATH_MSG_INFO(h->GetTitle() << ": " << printVerbose(h));
        }

        ATH_MSG_INFO("============================================");
        ATH_MSG_INFO("Finished Printing all the histograms.");
        ATH_MSG_INFO("============================================");

    }

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTMonitorBase::finalize()
{
    if (m_checkHistRange) checkHistRange();
    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTMonitorBase::selectEvent() const
{
    if (m_eventInputHeader) {
        if (!m_evtSel->selectEvent(m_eventInputHeader)) {
            return StatusCode::SUCCESS;
        }
    }
    else if (!m_evtSel->selectEvent(m_logicEventInputHeader_1st)) {
        return StatusCode::SUCCESS;
    }
    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
std::string HTTMonitorBase::createDirName(std::string const & parent_string) const
{
    if (m_outputtag.value().empty()) return parent_string;

    std::istringstream initial(parent_string);
    std::stringstream ss;
    std::string subdir;

    while (getline(initial, subdir, '/')) {
        if (subdir.empty()) continue;
        bool parentdir = ss.str().empty();
        ss << "/" << subdir;
        if (parentdir) ss << m_outputtag.value();
    }
    ss << "/";

    return ss.str();
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
std::vector<HTTHit> const * HTTMonitorBase::getMonitorHits(std::string const & key) const
{
    if (m_monitorHits.find(key) == m_monitorHits.end()) {
        ATH_MSG_ERROR("HTTHit data with key " << key << " not found for the monitor.");
        return nullptr;
    }

    return m_monitorHits.find(key)->second;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
std::vector<HTTCluster> const * HTTMonitorBase::getMonitorClusters(std::string const & key) const
{
    if (m_monitorClusters.find(key) == m_monitorClusters.end()) {
        ATH_MSG_ERROR("HTTCluster data with key " << key << " not found for the monitor.");
        return nullptr;
    }

    return m_monitorClusters.find(key)->second;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
std::vector<HTTRoad*> const * HTTMonitorBase::getMonitorRoads(std::string const & key) const
{
    if (m_monitorRoads.find(key) == m_monitorRoads.end()) {
        ATH_MSG_ERROR("HTTRoad data with key " << key << " not found for the monitor.");
        return nullptr;
    }

    return m_monitorRoads.find(key)->second;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
std::vector<HTTTrack> const * HTTMonitorBase::getMonitorTracks(std::string const & key) const
{
    if (m_monitorTracks.find(key) == m_monitorTracks.end()) {
        ATH_MSG_ERROR("HTTTrack data with key " << key << " not found for the monitor.");
        return nullptr;
    }

    return m_monitorTracks.find(key)->second;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void HTTMonitorBase::clearMonitorData()
{
    m_monitorRoads.clear();
    m_monitorTracks.clear();
    m_monitorClusters.clear();
    m_monitorHits.clear();
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
std::string HTTMonitorBase::printVerbose(TH1 const * h) const
{
    std::stringstream ss;
    ss << "Entries=" << h->GetEntries();

    if (h->GetDimension() == 1) {
        ss << ", Mean=" << h->GetMean() << ", RMS=" << h->GetRMS() << ", RMS95=" << rms95(h);
    }

    if (h->GetDimension() >= 2) {
        ss << ", X Mean=" << h->GetMean(1) << ", X RMS=" << h->GetRMS(1)
           << ", Y Mean=" << h->GetMean(2) << ", Y RMS=" << h->GetRMS(2);
        if (h->GetDimension() == 3) {
            ss << ", Z Mean=" << h->GetMean(3) << ", Z RMS=" << h->GetRMS(3);
        }
    }

    return ss.str();
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
std::string HTTMonitorBase::printProfile(TH1* h) const
{
    if (TH2* h2 = dynamic_cast<TH2*>(h))
    {
        std::stringstream ss;
        TProfile *p = h2->ProfileX("_pfx", 1, -1, "s"); // std dev instead of error on the mean
        ss << "Total Entries:" << h2->GetEntries();
        for (int i = 1; i <= p->GetNbinsX(); i++)
            ss << "\n\tBin=" << i << ", Entries=" << p->GetBinEntries(i) << ", Mean=" << p->GetBinContent(i) << ", RMS=" << p->GetBinError(i);
        return ss.str();
    }

    return printVerbose(h);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void HTTMonitorBase::getEff(TGraphAsymmErrors* h_eff, TH1 const * h_pass, TH1 const * h_total) const
{
    if (h_pass->GetNbinsX() != h_total->GetNbinsX()) return;
    if (h_pass->GetXaxis()->GetXmin() != h_total->GetXaxis()->GetXmin()) return;
    if (h_pass->GetXaxis()->GetXmax() != h_total->GetXaxis()->GetXmax()) return;

    bool hasOverflow = false;
    TH1* h_pass_tmp = dynamic_cast<TH1*>(h_pass->Clone());
    for (int i = 0; i <= h_total->GetNbinsX() + 1; i++) {
        if (h_pass_tmp->GetBinContent(i) > h_total->GetBinContent(i)) {
            h_pass_tmp->SetBinContent(i, h_total->GetBinContent(i));
            hasOverflow = true;
        }
    }
    if (hasOverflow) {
        std::stringstream ss;
        ss << h_pass->GetName() << " has at least 1 bin greater than that of " << h_total->GetName() << "";
        ATH_MSG_WARNING(ss.str());
    }

    gROOT->ProcessLine("gErrorIgnoreLevel = kError;");
    //h_eff->Divide(h_pass_tmp, h_total);
    h_eff->Divide(h_pass_tmp, h_total, "B");
    gROOT->ProcessLine("gErrorIgnoreLevel = kUnset;");

    return;
}

// Jad Mathieu Sardain * * * * * * * * * * * * * * * * * * * * * * * * * * *
void HTTMonitorBase::getEffJMS(TH1F* h_eff, TH1F const * h_pass, TH1F const * h_total) const
{
  if (h_pass->GetNbinsX() != h_total->GetNbinsX()) return;
  if (h_pass->GetXaxis()->GetXmin() != h_total->GetXaxis()->GetXmin()) return;
  if (h_pass->GetXaxis()->GetXmax() != h_total->GetXaxis()->GetXmax()) return;

  bool hasOverflow = false;
  TH1F* h_pass_tmp = dynamic_cast<TH1F*>(h_pass->Clone());
  for (int i = 1; i < h_total->GetNbinsX() + 1; i++) {
      if (h_pass_tmp->GetBinContent(i) > h_total->GetBinContent(i)) {
          h_pass_tmp->SetBinContent(i, h_total->GetBinContent(i));
          hasOverflow = true;
      }
  }
  if (hasOverflow) {
      std::stringstream ss;
      ss << h_pass->GetName() << " has at least 1 bin greater than that of " << h_total->GetName() << "";
      ATH_MSG_WARNING(ss.str());
  }

  //gROOT->ProcessLine("gErrorIgnoreLevel = kError;");
  //h_eff->Divide(h_pass_tmp, h_total);
  h_eff->Divide(h_pass_tmp, h_total, 1., 1., "B");
  //gROOT->ProcessLine("gErrorIgnoreLevel = kUnset;");
  for (int i = 1; i < h_eff->GetNbinsX() + 1; i++) {
    //if (isinf(h_eff->GetBinError(i)) || isnan(h_eff->GetBinError(i)) ){
    if (h_eff->GetBinError(i) >= 1 ){
      //double eff    = float(h_pass_tmp->GetBinContent(i)) / float(h_total->GetBinContent(i));
      double efferr = (1. / h_total->GetBinContent(i)) * std::sqrt( float(h_pass_tmp->GetBinContent(i)) * (1. - float( h_pass_tmp->GetBinContent(i) ) / float(h_total->GetBinContent(i))));
      /*std::cout << " " << h_total->GetBinContent(i)
                << " " << h_pass_tmp->GetBinContent(i)
                << " " << h_pass_tmp->GetBinContent(i) /  h_total->GetBinContent(i)
                << " " << (1. / h_total->GetBinContent(i))
                << " " << 1 - float( h_pass_tmp->GetBinContent(i) ) / float (h_total->GetBinContent(i))
                << " " << TMath::Sqrt( float(h_pass_tmp->GetBinContent(i)) * (1 - float( h_pass_tmp->GetBinContent(i) ) / float (h_total->GetBinContent(i))))
                << " " << std::endl;*/
      //
      //std::cout << " " << typeid(h_total->GetBinContent(i)).name()
      //          << " " << typeid(h_pass_tmp->GetBinContent(i)).name()
      //          << std::endl;
      //h_eff->SetBinContent(i , eff);
      h_eff->SetBinError(i , efferr);
      //std::cout << " " << efferr << std::endl;
    }
  }
  return;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void HTTMonitorBase::checkHistRange() const
{
    ATH_MSG_DEBUG("Now check if the histogram ranges are set correctly.");

    bool all_checked = true;

    for (std::string hist : m_tHistSvc->getHists()) {

        // Do not check pT histograms
        int hist_length = hist.length();
        if (hist_length > 3 && hist.substr(hist_length - 3, hist_length -1) == "_pt") continue;;

        TH1* h;
        bool wrong_range = false;
        std::stringstream ss;

        if (m_tHistSvc->getHist(hist, h).isFailure()) {
            ATH_MSG_ERROR("Histogram with directory " << hist << " does not exist.");
        }
        else {
            unsigned underflow_x = 0, overflow_x = 0, underflow_y = 0, overflow_y = 0, underflow_z = 0, overflow_z = 0;

            if (h->GetDimension() == 1) {
                underflow_x = h->GetBinContent(0);
                overflow_x  = h->GetBinContent(h->GetNbinsX() + 1);
            }

            if (h->GetDimension() == 2) {
                for (int biny = 0; biny <= h->GetNbinsY() + 1; biny++) {
                    if (h->GetBinContent(0, biny) > 0)
                        underflow_x += h->GetBinContent(0, biny);
                    if (h->GetBinContent(h->GetNbinsX() + 1, biny) > 0)
                        overflow_x  += h->GetBinContent(h->GetNbinsX() + 1, biny);
                }

                for (int binx = 0; binx <= h->GetNbinsX() + 1; binx++) {
                    if (h->GetBinContent(binx, 0) > 0)
                        underflow_y += h->GetBinContent(binx, 0);
                    if (h->GetBinContent(binx, h->GetNbinsY() + 1) > 0)
                        overflow_y  += h->GetBinContent(binx, h->GetNbinsY() + 1);
                }
            }

            if (h->GetDimension() == 3) {
                for (int biny = 0; biny <= h->GetNbinsY() + 1; biny++) {
                    for (int binz = 0; binz <= h->GetNbinsZ() + 1; binz++) {
                        if (h->GetBinContent(0, biny, binz) > 0)
                            underflow_x += h->GetBinContent(0, biny, binz);
                        if (h->GetBinContent(h->GetNbinsX() + 1, biny, binz) > 0)
                            overflow_x  += h->GetBinContent(h->GetNbinsX() + 1, biny, binz);
                    }
                }

                for (int binx = 0; binx <= h->GetNbinsX() + 1; binx++) {
                    for (int binz = 0; binz <= h->GetNbinsZ() + 1; binz++) {
                        if (h->GetBinContent(binx, 0, binz) > 0)
                            underflow_y += h->GetBinContent(binx, 0, binz);
                        if (h->GetBinContent(binx, h->GetNbinsY() + 1, binz) > 0)
                            overflow_y  += h->GetBinContent(binx, h->GetNbinsY() + 1, binz);
                    }
                }

                for (int binx = 0; binx <= h->GetNbinsX() + 1; binx++) {
                    for (int biny = 0; biny <= h->GetNbinsY() + 1; biny++) {
                        if (h->GetBinContent(binx, biny, 0) > 0)
                            underflow_z += h->GetBinContent(binx, biny, 0);
                        if (h->GetBinContent(binx, biny, h->GetNbinsZ() + 1) > 0)
                            overflow_z  += h->GetBinContent(binx, biny, h->GetNbinsZ() + 1);
                    }
                }
            }

            if (std::string(h->ClassName()) == "TH1I" || std::string(h->ClassName()) == "TH2I" || std::string(h->ClassName()) == "TH3I") {
                if (underflow_x > 0 || overflow_x > 0 || underflow_y > 0 || overflow_y > 0 || underflow_z > 0 || overflow_z > 0) {
                    all_checked = false;
                    wrong_range = true;
                    ss << "Wrong range " << h->GetName() << ": ";
                    if (underflow_x > 0) {
                        if (h->GetDimension() == 1)
                            ss << "Underflow: " << underflow_x << ". ";
                        else
                            ss << "X Underflow: " << underflow_x << ". ";
                    }
                    if (overflow_x > 0) {
                        if (h->GetDimension() == 1)
                            ss << "Overflow: "  << overflow_x  << ". ";
                        else
                            ss << "X Overflow: "  << overflow_x  << ". ";
                    }
                    if (underflow_y > 0)
                        ss << "Y Underflow: " << underflow_y << ". ";
                    if (overflow_y > 0)
                        ss << "Y Overflow: "  << overflow_y  << ". ";
                    if (underflow_z > 0)
                        ss << "Z Underflow: " << underflow_z << ". ";
                    if (overflow_z > 0)
                        ss << "Z Overflow: "  << overflow_z  << ". ";
                }
            }
            else {
                if (h->GetDimension() > 1) continue;
                if (underflow_x / h->Integral() > 0.05 || overflow_x / h->Integral() > 0.05) {
                    all_checked = false;
                    wrong_range = true;
                    ss << "Wrong range " << h->GetName() << ": ";
                    if (underflow_x / h->Integral() > 0.05)
                        ss << "Underflow: " << 100.0 * underflow_x / h->Integral() << "%. ";
                    if (overflow_x / h->Integral() > 0.05)
                        ss << "Overflow: "  << 100.0 * overflow_x / h->Integral() << "%. ";
                }
            }
        }

        if (wrong_range) ATH_MSG_WARNING(ss.str());

    }

    if (all_checked) ATH_MSG_DEBUG("All histogram ranges are set correctly.");
}
