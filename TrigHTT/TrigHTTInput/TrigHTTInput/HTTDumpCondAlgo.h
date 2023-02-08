/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTTDumpCondAlgo_h
#define HTTDumpCondAlgo_h

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "TrigHTTInput/HTTDetectorTool.h" 
#include "GaudiKernel/ToolHandle.h"

#include <string>

/////////////////////////////////////////////////////////////////////////////
class HTTDumpCondAlgo: public AthAlgorithm {
public:
  HTTDumpCondAlgo (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~HTTDumpCondAlgo ();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:

  ToolHandle<HTTDetectorTool> m_detectorTool; // detector status handler

  // options
  bool m_DumpBadModules;
  bool m_DumpModuleIDMap;
  bool m_DumpGlobalToLocalMap;

  bool m_DumpIDMap;

  bool m_DumpModulePositions;

};

#endif // HTTDumpCondAlgo_h
