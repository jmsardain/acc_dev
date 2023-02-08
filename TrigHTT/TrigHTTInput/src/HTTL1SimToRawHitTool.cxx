/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTInput/HTTL1SimToRawHitTool.h"
#include "TrigHTTInput/L1SimEvent.h"
#include "TrigHTTInput/L1SimFitResult.h"
#include "TrigHTTObjects/HTTEventInfo.h"
#include "TrigHTTInput/TrigHTTInputUtils.h"
#include "TrigHTTObjects/HTTEventInputHeader.h"

#include "TFile.h"
#include "TTree.h"
// FrameWork includes
#include "GaudiKernel/IToolSvc.h"


HTTL1SimToRawHitTool::HTTL1SimToRawHitTool( const std::string& type, 
					      const std::string& name, 
					      const IInterface* parent ) :
  base_class(type, name, parent )
{
  declareProperty( "inputFileName", m_inputFileName );  
}


StatusCode HTTL1SimToRawHitTool::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");
  ATH_MSG_INFO ("Reading input file: "  << m_inputFileName);
  m_inputFile = TFile::Open(m_inputFileName.value().c_str(),"READ");
  if ( m_inputFile->IsZombie()) {
    ATH_MSG_ERROR("File "<<m_inputFileName<<" not found. Exit");
    return StatusCode::FAILURE;
  }

  
  m_EventTree = (TTree*) m_inputFile->Get("FitResults");
  int nevents = m_EventTree->GetEntries();
  if (nevents == -1 ){
    ATH_MSG_ERROR("File "<<m_inputFileName<<": Tree is empty");
    return StatusCode::FAILURE;
  }

  if (m_EventTree->GetBranchStatus("event") == 0){
    ATH_MSG_ERROR("File "<<m_inputFileName<<": Tree is error state");
    return StatusCode::FAILURE;
  }

  TBranch *branch  = m_EventTree->GetBranch("event");
  m_event = new L1SimEvent();
  branch->SetAddress(&m_event);
  branch->Print();
  m_eventId = 0; 
 
  return StatusCode::SUCCESS;
}

StatusCode HTTL1SimToRawHitTool::finalize()
{
  ATH_MSG_DEBUG("Processed " << m_eventId <<" events");
  return StatusCode::SUCCESS;
}

StatusCode HTTL1SimToRawHitTool::writeData(HTTEventInputHeader* header)  {
  m_eventHeader= header; // not tested
  m_EventTree->Fill();
  return StatusCode::SUCCESS;
}


StatusCode HTTL1SimToRawHitTool::readData(HTTEventInputHeader* header, bool &last)
{
  m_eventHeader=header; //take the external pointer
  last=false;
  HTTEventInfo event_info;
  event_info.setRunNumber(0);

  m_EventTree->GetEntry(m_eventId);
  
  event_info.setEventNumber(m_eventId);
  m_eventHeader->newEvent(event_info);//this also reset all varaibles

  std::vector<FitResult> allfitlist;
  m_event->FitResults(allfitlist);

  // dump
  std::cout<<" ----------------------"<<std::endl;
  std::cout<<"L1SIM: Event "<<m_eventId<<" with "<< allfitlist.size()<<" fit results"<<std::endl;
  int nfit=0;
  for (auto fit: allfitlist){
    if (fit.isGood()){
      //std::cout <<fit<<std::endl;
      if (dumpFitResult(fit, std::cout) != StatusCode::SUCCESS) return StatusCode::FAILURE;
    }
    nfit++;
  }
  m_eventId++;
  return StatusCode::SUCCESS;
}



StatusCode HTTL1SimToRawHitTool::dumpFitResult(const FitResult& f, std::ostream& s){

  s<< std::fixed
   <<"L1SIM: "
   <<"Fit Result: " << ((f.fitstat() == fit_status::good ) ? "Good " : "Bad ") 
   <<" curv=" << std::setprecision(3) << f.curv()
   <<" eta="       << std::setprecision(2) << f.eta()
   <<" phi="       << std::setprecision(2) << f.phi()
   <<" z0="        << std::setprecision(2) << f.z0()
   <<" d0="        << std::setprecision(2) << f.d0()
   <<" chi2="      << std::setprecision(2) << f.chi2ndf()
    // <<" deltaR="    << std::setprecision(4) << f.deltaR()
    //   <<" barcodem="  << std::setw(3) << f.barcodem()
    //   <<" nmuons="    << std::setw(6) << f.nmuons()
   <<" passHW="     <<  f.passHW()
    //   <<" patType="    <<  f.patternType()
   <<" pattern="    << f.pattern()
   <<" sector="      << f.setID()
   <<" nhitlayers=" <<  f.nhitlayers()
    //   <<" event="   << std::setw(5) << f.eventId()
   <<"\n";
  s<<"L1SIM:      Hits: ";
  for (auto hit: f.hitComb()) { s<<" "<<(hit);}
  s <<"\n";

  //  s<<"      Miss layers: ";
  //  for (auto lay: f.layMiss()) { s<<" "<<(lay);}
  //s <<"\n";


  return StatusCode::SUCCESS;

}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 


