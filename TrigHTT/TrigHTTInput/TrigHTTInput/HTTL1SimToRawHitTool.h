/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHTTINPUT_HTTL1SimToRawHitTool_H
#define TRIGHTTINPUT_HTTL1SimToRawHitTool_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

// AthenaBaseComps includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigHTTInput/HTTEventInputHeaderToolI.h"
#include "TrigHTTObjects/HTTHit.h"
#include "TrigHTTInput/L1SimEvent.h"

// Forward declaration
class TFile;
class TTree;


class HTTL1SimToRawHitTool: public extends<AthAlgTool, HTTEventInputHeaderToolI>
{ 
 public: 

  HTTL1SimToRawHitTool( const std::string& type,
	     const std::string& name, 
	     const IInterface* parent );

  virtual ~HTTL1SimToRawHitTool(){}; 


  virtual StatusCode initialize() override;
  virtual StatusCode finalize()   override;
  virtual StatusCode readData(HTTEventInputHeader* header, bool &last)  override;
  virtual StatusCode writeData(HTTEventInputHeader* header)  override; //not implmeneted yet
  
 private: 

  /// Default constructor: 
  HTTL1SimToRawHitTool();

  StatusCode dumpFitResult(const FitResult& fit, std::ostream& s);

  StringProperty  m_inputFileName;

  TFile *m_inputFile;
  TTree *m_EventTree;

  L1SimEvent* m_event;
  Long64_t m_eventId;

};


#endif //> !TRIGHTTINPUT_HTTL1SimToRawHitTool_H
