/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTAlgorithms/HTTLLPHoughMonitorTool.h"

#include "TrigHTTAlgorithms/HTTLLPDoubletHoughTransformTool.h"
#include "TrigHTTAlgorithms/HTTRoadUnionTool.h"
#include "TrigHTTObjects/HTTLogicalEventInputHeader.h"
#include "TrigHTTObjects/HTTLogicalEventOutputHeader.h"
#include "TrigHTTObjects/HTTRoad.h"
#include "TrigHTTObjects/HTTTruthTrack.h"
#include <iostream>
#include <fstream>
static const InterfaceID IID_HTTLLPHoughMonitorTool("HTTLLPHoughMonitorTool", 1, 0);
const InterfaceID& HTTLLPHoughMonitorTool::interfaceID()
{ return IID_HTTLLPHoughMonitorTool; }

//unsigned nHitLayers(std::vector<HTTHit> const & hits);

///////////////////////////////////////////////////////////////////////////////
// Initialize
///////////////////////////////////////////////////////////////////////////////
HTTLLPHoughMonitorTool::HTTLLPHoughMonitorTool(const std::string& algname, const std::string& name, const IInterface *ifc) :
    HTTMonitorBase(algname,name,ifc)
{
    declareInterface<HTTLLPHoughMonitorTool>(this);
}


StatusCode HTTLLPHoughMonitorTool::initialize()
{
    ATH_CHECK(HTTMonitorBase::initialize());
    ATH_CHECK(bookHistograms());
    outFile.open("truthtracks.txt");
    outFile << "event,barcode,charge,pt,d0" <<std::endl;
    return StatusCode::SUCCESS;
}


StatusCode HTTLLPHoughMonitorTool::bookHistograms()
{
    setHistDir("/HoughJMS/");

    h_res_withroad_pt_matched_all = new TH1D("h_res_withroad_pt_matched_all", "Road Resolution as function of truth q/p_{T};q/p_{T};", 100, -0.001, 0.001);
    h_res_withroad_phi_matched_all = new TH1D("h_res_withroad_phi_matched_all", "Road Resolution as function of truth d_{0};d_{0};", 100, -5, 5);


    ATH_CHECK(regHist(getHistDir(), h_res_withroad_pt_matched_all));
    ATH_CHECK(regHist(getHistDir(), h_res_withroad_phi_matched_all));

    return StatusCode::SUCCESS;
}


void HTTLLPHoughMonitorTool::setTool(const IAlgTool* rf)
{
    auto tool = dynamic_cast<const HTTLLPDoubletHoughTransformTool*>(rf);
    m_LLPhoughTools.push_back(tool);
    // if (auto uni = dynamic_cast<const HTTRoadUnionTool*>(rf))
    // {
    //   ATH_MSG_INFO(" IN THIS CONDITION ");
    //     for (auto & tool : uni->tools())
    //         if (auto ptr = dynamic_cast<const HTTLLPDoubletHoughTransformTool*>(tool.get()))
    //             m_LLPhoughTools.push_back(ptr);
    // }
    // else if (auto tool = dynamic_cast<const HTTLLPDoubletHoughTransformTool*>(rf))
    // {
    //   ATH_MSG_INFO(" IN THAT CONDITION ");
    //
    //     m_LLPhoughTools.push_back(tool);
    // }
}


///////////////////////////////////////////////////////////////////////////////
// Filling
///////////////////////////////////////////////////////////////////////////////

StatusCode HTTLLPHoughMonitorTool::fillHistograms()
{
    ATH_CHECK(fillRoadHistograms(getMonitorRoads("Roads_1st")));
    clearMonitorData();

    return StatusCode::SUCCESS;
}

StatusCode HTTLLPHoughMonitorTool::fillRoadHistograms(std::vector<HTTRoad*> const * roads)
{
    std::vector<HTTTruthTrack> const & truthtracks = m_logicEventInputHeader_1st->optional().getTruthTracks();
    std::vector<HTTHit> const & hits = m_logicEventInputHeader_1st->towers().front().hits(); // not used here
    HTTTruthTrack const & truth_t = truthtracks.front();

    bool passed = true;
    outFile << m_event << ","
            << truth_t.getBarcode()  << ","
            << truth_t.getQ() << ","
            << truth_t.getPt() << ","
            << truth_t.getD0()
            << std::endl;

    if (truth_t.getBarcode() == 0 || truth_t.getQ() == 0) passed=false;
    if (truth_t.getBarcode() > 200000)  passed=false;
    if (roads->size() >  30) passed = false;

    if (passed){
    // ATH_MSG_INFO(" roads size in hough monitor is: " << roads->size());
    // ATH_MSG_INFO(" m_LLPhoughTools size in hough monitor is: " << m_LLPhoughTools.size());
    // Crude efficiency on single-particle events (no slicing)
    if (!m_withPU && m_LLPhoughTools.size() == 1){
    //if (!m_withPU && roads->size() > 0){

      //ATH_MSG_INFO("I am here in HOUGH MONITOR" );
      //ATH_MSG_INFO(" truth_t.getPt() " << truth_t.getPt() << " road size = " << roads->size());
      //ATH_MSG_INFO(" truth_t.getD0() " << truth_t.getD0() << " truth_t.getQ() / truth_t.getPt() " << truth_t.getQ() / truth_t.getPt());
      for (HTTRoad* road : *roads){
        //ATH_MSG_INFO("I am here in HOUGH MONITOR in roads" );
        HTTRoad_Hough const * road_hough = dynamic_cast<HTTRoad_Hough*>(road);
        if (!road_hough) break;

        // Resolution
        ATH_MSG_INFO(" HOUGH MONITOR road q/pT : " << road_hough->getY()
                  << " truth q/pT:  " << truth_t.getQ()/truth_t.getPt()
                  << " and road  d0 is: " << road_hough->getX()
                  << " truth d0" << truth_t.getD0()
                  << " truth_t.getBarcode() " << truth_t.getBarcode());

        h_res_withroad_pt_matched_all->Fill((truth_t.getQ() / truth_t.getPt()) - road_hough->getY());
        h_res_withroad_phi_matched_all->Fill(truth_t.getD0() - road_hough->getX());

      }
    }
  }
  m_event++;
    return StatusCode::SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////////////////

StatusCode HTTLLPHoughMonitorTool::finalize()
{
    ATH_MSG_INFO("Misses: " << m_nMiss);
    ATH_MSG_INFO("\tNot enough hit layers: " << m_nHitsBelowThreshold);

    return StatusCode::SUCCESS;
}
