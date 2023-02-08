/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTInput/HTTDumpCondAlgo.h"

#include <sstream>
#include <iostream>
#include <fstream>

/////////////////////////////////////////////////////////////////////////////
HTTDumpCondAlgo::HTTDumpCondAlgo(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator), 
  m_detectorTool("HTTDetectorTool/HTTDetectorTool"),
  m_DumpBadModules(false),
  m_DumpModuleIDMap(false),
  m_DumpGlobalToLocalMap(false),
  m_DumpIDMap(false),
  m_DumpModulePositions(false)
{
  declareProperty("DumpBadModules", m_DumpBadModules, "If true enable dump of bad modules for HTT");
  declareProperty("DumpModuleIDMap", m_DumpModuleIDMap, "If true dumps the map of the modules in each tower");
  declareProperty("DumpGlobalToLocalMap",m_DumpGlobalToLocalMap, "True if you want to produce the Global-to-Local map");
  declareProperty("DumpIDMap",m_DumpIDMap);
  declareProperty("DumpModulePositions",m_DumpModulePositions,"To dump the corner positions of the modules");
}

HTTDumpCondAlgo::~HTTDumpCondAlgo()
{
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTDumpCondAlgo::initialize(){
  ATH_MSG_INFO( "HTTDumpCondAlgo::initialize()" );

  // select how the input is obtained
  if (m_DumpBadModules || m_DumpGlobalToLocalMap) {
    // Use the SG to retrieve the hits, this also means other Athena tools can be used
    ATH_CHECK(m_detectorTool.retrieve());
    ATH_MSG_INFO ("Setting HTTDetectorTool tool" );
  }
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTDumpCondAlgo::execute() {
  
  ATH_MSG_INFO ("HTTDumpCondAlgo::execute() start" );
  
  if (m_DumpBadModules) {
    // These are not available yet
    ATH_MSG_WARNING( "HTTDumpCondAlgo DumpBadModules not available yet");
    //    m_detectorTool->makeBadModuleMap(); //Dump bad SS map
    //m_detectorTool->dumpDeadModuleSummary(); //Dump bad module map
    m_DumpBadModules = false; // in case you run on more than 1 event, only dump once
  }

  if (m_DumpGlobalToLocalMap) {
    m_detectorTool->dumpGlobalToLocalModuleMap(); // Dump the map of the module of each tower
    m_DumpGlobalToLocalMap=false;
  }

  if (m_DumpIDMap) {
    // This is not available yet
    ATH_MSG_WARNING( "HTTDumpCondAlgo DumpIDMap not available yet" );
    //m_detectorTool->dumpIDMap();
    m_DumpIDMap = false; // in case you run on more than 1 event, only dump once
  }

  if(m_DumpModulePositions) {
    //m_detectorTool->dumpModulePositions();
     m_DumpModulePositions=false;
  }

  return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTDumpCondAlgo::finalize() {
  
  return StatusCode::SUCCESS;
}
