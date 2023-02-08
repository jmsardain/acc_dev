/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGHTTBYTESTREAM_TRIGHTTBYTESTREAMDECODERALG_H
#define TRIGHTTBYTESTREAM_TRIGHTTBYTESTREAMDECODERALG_H

/** @class HTTByteStreamDecoderAlg
 *  @brief Algorithm creating HTT results from ByteStream representation
 **/


// Trigger includes
#include "TrigHTTByteStream/TrigHTTTPByteStreamTool.h"
#include "TrigHTTObjects/HTTTrack.h"
#include "TrigHTTInput/HTTEventInputHeaderToolI.h"
#include "TrigHTTInput/HTTEventOutputHeaderToolI.h"
#include "xAODTrigHTT/HTTTrackTestContainer.h"
#include "TrigHTTUtils/HTTTypes.h"
#include "TrigHTTMaps/ITrigHTTMappingSvc.h"

// Athena includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "GaudiKernel/ToolHandle.h"

#include <bits/stdc++.h>
#include <vector>

class HTTHit;
class HTTTrack;
class ITrigHTTMappingSvc;
class HTTEventInputHeader;
class HTTLogicalEventInputHeader;
class HTTLogicalEventOutputHeader;

#define NHTTFILES 7
#define HCC_LAST_CLUSTER                0x6fed;

class HTTByteStreamDecoderAlg : public AthAlgorithm {
public:

  HTTByteStreamDecoderAlg(const std::string& name, ISvcLocator* svcLoc);
  virtual ~HTTByteStreamDecoderAlg(){};

  StatusCode initialize() ;
  StatusCode finalize() ;
  StatusCode execute();

 private:
  // ------------------------- Properties --------------------------------------
  /// StoreGate key for the output RoIBResult
  /* SG::WriteHandleKey<ROIB::RoIBResult> m_roibResultWHKey {this, "RoIBResultWHKey", "RoIBResult", */
  /*                                                         "Key of the output RoIBResult object"}; */
  SG::ReadHandleKey<xAOD::HTTTrackTestContainer> m_trackKey;//{this,"HTTTrackKey","HTTTrackKey", "SG key of the HTTTrackContainer"};
  BooleanProperty m_doTestVectors;
  BooleanProperty m_doFakeDecoding;
  BooleanProperty m_doDummyTestVectors;
  IntegerProperty m_tvType;

  // ------------------------- Tool/Service handles ----------------------------

  /**
   * @brief Tool to retrieve data from file
   **/
  ToolHandle<HTTEventInputHeaderToolI>     m_inputHeaderTool;// { this, "HitInputTool", "HTT_SGHitInput/HTT_SGHitInput", "HitInput Tool" };


/**
   * @brief Tool to retrieve data from file
   **/
  
  ToolHandle<HTTEventOutputHeaderToolI>    m_outputHeaderTool;
 
  /**
   * @brief Tool performing the decoding work
   **/
  ToolHandle<TrigHTTTPByteStreamTool> m_decoderTool;// {this, "TPDecoderTool", "TPByteStreamTool","Tool performing the decoding work for TP"};

  /**
   * @brief Svc to retrieve data online
   **/
  ServiceHandle<IROBDataProviderSvc>  m_robDataProviderSvc;

 /**
   * @brief Svc to retrieve HTT mapping
   **/
  ServiceHandle<ITrigHTTMappingSvc>    m_HTTMapping;
  
  // ------------------------- Other private members ---------------------------
  /**
   * @brief Creates string for Event header
   **/
  StatusCode addEventHeader(const HTTEventInputHeader&  eventHeader, std::stringstream& eventstring, dataFormat& dataType);

  /**
   * @brief Creates string for Event Footer
   **/
  StatusCode addEventFooter(const HTTEventInputHeader&  eventHeader, std::stringstream& eventstring);

  /**
   * @brief Creates string for TP board input
   **/
  StatusCode decode_RAW(std::stringstream& eventstring);

  /**
   * @brief Creates string for PRM board input
   **/
  StatusCode decode_CLUSTER(std::stringstream& eventstring);

  /**
   * @brief Creates string for PRM board extensions (inner data)
   **/
  StatusCode addSSIDINFO(const HTTTrack& track, std::stringstream& ssidstring);

  /**
   * @brief Creates string for PRM board output
   **/
  StatusCode decode_TRACK(std::stringstream& eventstring, dataFormat& dataType, bool& WITHSSID, bool& passOR);

  /**
   * @brief Creates dummy data to test BS decoding
   **/
  StatusCode createDummyData( );

  /**
   * @brief Creates hitmap and detmap for the tracks 
   **/
  StatusCode createTrackHitMap(const HTTTrack& track, unsigned int& hitMap);


  std::ofstream m_outfiles[NHTTFILES]; // name of output files for TV production
  int m_event_counter;

  
  // internal pointers
  HTTEventInputHeader           *m_inputHeader;
  HTTLogicalEventInputHeader    *m_logInputHeader_1st;
  HTTLogicalEventInputHeader    *m_logInputHeader_2nd;
  HTTLogicalEventOutputHeader   *m_logOutputHeader;
};

#endif //TRIGHTTBYTESTREAM_TRIGHTTBYTESTREAMDECODERALG_H
