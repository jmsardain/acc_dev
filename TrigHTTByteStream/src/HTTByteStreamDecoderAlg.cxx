/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "HTTByteStreamDecoderAlg.h"

//#include "TrigHTTObjects/HTT_SSID.h"
#include "TrigHTTObjects/HTTEventInfo.h"
#include "TrigHTTObjects/HTTEventInputHeader.h"
#include "TrigHTTObjects/HTTHit.h"
#include "TrigHTTObjects/HTTLogicalEventInputHeader.h"
#include "TrigHTTObjects/HTTLogicalEventOutputHeader.h"
#include "TrigHTTObjects/HTTTrack.h"
#include "TrigHTTMaps/HTTSSMap.h"

#include "xAODTrigHTT/HTTTrackTest.h"

#include <iostream>
#include <fstream>

// =============================================================================
// Standard constructor
// =============================================================================

HTTByteStreamDecoderAlg::HTTByteStreamDecoderAlg(const std::string& name, ISvcLocator* svcLoc):
  AthAlgorithm(name, svcLoc),
  m_inputHeaderTool("HTTInputHeaderTool/HTTInputHeaderTool"),
  m_outputHeaderTool("HTTOutputHeaderTool/HTTOutputHeaderTool"),
  m_decoderTool("HTT_TPDecoderTool/HTT_TPDecoderTool"),
  m_robDataProviderSvc("ROBDataProviderSvc", name),
  m_HTTMapping("TrigHTTMappingSvc", name)
{
  declareProperty("InputTool",          m_inputHeaderTool);
  declareProperty("OutputTool",         m_outputHeaderTool);
  declareProperty("TPDecoderTool",      m_decoderTool);
  declareProperty("TvType",             m_tvType=-1, "flag to select the type of test-vector: -1=all, 0=raw data, 1=cluster, 2=track v1, 3=track v1 ssid, 4=track v1 OR, 5=track v2, 6=track v2 ssid");
  declareProperty("DoTestVectors",      m_doTestVectors=true, "flag to enable the test vectors");
  declareProperty("DoDummyTestVectors", m_doDummyTestVectors=true, "flag to enable the dummy test vectors");
  declareProperty("DoFakeDecoding",     m_doFakeDecoding=false, "flag to enable the fake decoding");
  declareProperty("HTTTrackKey",        m_trackKey="HTTTracks", "SG key of the HTTTrackContainer");
}

// =============================================================================
// Implementation of AthReentrantAlgorithm::initialize
// =============================================================================
StatusCode HTTByteStreamDecoderAlg::initialize() {
  ATH_MSG_DEBUG("Initialising " << name());

  m_event_counter=0;

  // ROBDataProviderSvc needed to retrieve data from Storage Handler
  ATH_CHECK(m_robDataProviderSvc.retrieve());
  ATH_CHECK(m_HTTMapping.retrieve());

  if (m_tvType<=0  && ! m_doDummyTestVectors){
    // reading from file: input header with input hits
    ATH_CHECK(m_inputHeaderTool.retrieve());
    ATH_CHECK(m_outputHeaderTool.retrieve());

    m_inputHeader        = m_inputHeaderTool->getEventInputHeader();
    m_logInputHeader_1st = m_outputHeaderTool->getLogicalEventInputHeader_1st();
    m_logInputHeader_2nd = m_outputHeaderTool->getLogicalEventInputHeader_2nd();
    m_logOutputHeader    = m_outputHeaderTool->getLogicalEventOutputHeader();
  }
  else ATH_CHECK(createDummyData());// created just once


  // decoder tool
  ATH_CHECK(m_decoderTool.retrieve());
  ATH_MSG_DEBUG("Setting HTT_TPdecoderTool tool");
  ATH_CHECK(m_decoderTool->initialize());

  //  ATH_CHECK(m_trackKey.initialize(!m_trackKey.key().empty()));// this only works for rel 22
  if (!m_trackKey.key().empty())
    ATH_CHECK(m_trackKey.initialize());

  ATH_MSG_INFO("Running with tv type="<<m_tvType);

  // open utput files here:
  std::string outpath[NHTTFILES];
  outpath[0] =  "AMTP_IN.txt";
  outpath[1] =  "PRM_IN.txt";
  outpath[2] =  "PRM_OUT.txt";
  outpath[3] =  "PRM_EXT.txt";
  outpath[4] =  "AMTP_OUT.txt"; 
  outpath[5] =  "TFM_OUT.txt";
  outpath[6] =  "TFM_EXT.txt";

  ATH_MSG_INFO("Creating output files: ");
  for (int i=0; i<NHTTFILES;i++){
    ATH_MSG_INFO(outpath[i]<<"....");
    m_outfiles[i].open(outpath[i], std::ofstream::out);
    // add (....,  std::ofstream::out| std::ios::binary); to open in binary format
  }

  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of AthReentrantAlgorithm::finalize
// =============================================================================
StatusCode HTTByteStreamDecoderAlg::finalize() {
  ATH_MSG_DEBUG("Finalising " << name() <<" after "<<m_event_counter<<" events" );
  ATH_CHECK(m_robDataProviderSvc.release());
  ATH_MSG_INFO("Closing "<< NHTTFILES <<" output files");
  for (int i=0; i<NHTTFILES;i++){
    m_outfiles[i].close();
  }

  if (m_doDummyTestVectors){
    if (m_inputHeader) delete m_inputHeader;
    if (m_logInputHeader_1st) delete m_logInputHeader_1st;
    if (m_logInputHeader_2nd) delete m_logInputHeader_2nd;
    if (m_logOutputHeader) delete m_logOutputHeader;
  }

  return StatusCode::SUCCESS;
}


// =============================================================================
// Implementation of AthReentrantAlgorithm::execute
// =============================================================================
StatusCode HTTByteStreamDecoderAlg::execute()  {
  if (!m_doDummyTestVectors)  m_event_counter++;
  ATH_MSG_DEBUG("Executing " << name() <<" on event "<<m_event_counter);
  ATH_MSG_DEBUG("Test Vector activities start here");

// first load the data, once per event
  if (!m_doDummyTestVectors){
    bool last=false;
    ATH_CHECK(m_inputHeaderTool->readData(m_inputHeader, last));
    ATH_CHECK(m_outputHeaderTool->readData(m_logInputHeader_1st, m_logInputHeader_2nd, m_logOutputHeader, last));
    if (last) return StatusCode::SUCCESS;
  }

  //  ATH_MSG_DEBUG(*m_inputHeader);  // commenting out the DEBUGs (to check later)
  //  ATH_MSG_DEBUG(*m_logInputHeader);
  //  ATH_MSG_DEBUG(*m_logOutputHeader);

  // TEST: retrieve tracks from SG:
  if (! m_trackKey.key().empty()) {
    ATH_MSG_DEBUG("Found tracks key "<< m_trackKey.key());
    SG::ReadHandle<xAOD::HTTTrackTestContainer> inputTracks(m_trackKey);
    if (inputTracks.isValid()) {
      ATH_MSG_DEBUG("Retrieving tracks from key "<< m_trackKey.key());
      ATH_MSG_DEBUG("Found "<<inputTracks->size()<<" tracks");
      for(const xAOD::HTTTrackTest* xtrack : *inputTracks ){
        ATH_MSG_DEBUG("Retrieved track with pt ="<<xtrack->pt());
        // add here the convertion
      }
    }
  }


  std::stringstream eventstring;
  dataFormat dataType = RAWSTRIP;
  bool WITHSSID = false;
  bool passOR = false;
  if (m_tvType<=0) {
    ATH_CHECK(decode_RAW(eventstring));
    m_outfiles[0]<<eventstring.str();
  }
  if (m_tvType<0 || m_tvType==1){
    ATH_CHECK(decode_CLUSTER(eventstring));
    m_outfiles[1]<<eventstring.str();
  }
  if (m_tvType<0 || m_tvType==2) {
    dataType = S1TRACK;
    ATH_CHECK(decode_TRACK(eventstring, dataType, WITHSSID, passOR));
    m_outfiles[2]<<eventstring.str();
  }
  if (m_tvType<0 || m_tvType==3){
    dataType = S1TRACK;
    WITHSSID = false; //replacing true with false (debug the SSID crash with sim events later)
    ATH_CHECK(decode_TRACK(eventstring, dataType, WITHSSID, passOR));
    m_outfiles[3]<<eventstring.str();
  }
  if (m_tvType<0 || m_tvType==4){
    dataType = S1TRACK;
    passOR = true;
    ATH_CHECK(decode_TRACK(eventstring, dataType, WITHSSID, passOR)); 
    m_outfiles[4]<<eventstring.str();
  }
  if (m_tvType<0 || m_tvType==5){
    dataType = S2TRACK;
    ATH_CHECK(decode_TRACK(eventstring, dataType, WITHSSID, passOR));
    m_outfiles[5]<<eventstring.str();
  }
  if (m_tvType<0 || m_tvType==6){
    dataType = S2TRACK;
    WITHSSID = false; //replacing true with false (debug the SSID crash with sim events later)   
    ATH_CHECK(decode_TRACK(eventstring, dataType, WITHSSID, passOR));
    m_outfiles[6]<<eventstring.str();
  }


  return StatusCode::SUCCESS;

}

StatusCode HTTByteStreamDecoderAlg::addEventHeader(const HTTEventInputHeader&  eventHeader, std::stringstream& eventstring, dataFormat& dataType){
  HTTEventInfo eventInfo= eventHeader.event() ;
  std::vector<unsigned long int> newEventHeaderWordTV(6,0);
  ATH_CHECK(m_decoderTool->convertEventHeader(eventInfo, dataType, newEventHeaderWordTV));
  std::string flagString = "1 ";
  for (int i = 0; i< 6; i++){
    if ( i == 0 ) flagString = "1 ";
    else flagString = "0 ";
    eventstring << flagString << std::setw(16) << std::setfill('0') << std::hex << newEventHeaderWordTV[i] << std::endl;
  }
  return StatusCode::SUCCESS;
}


StatusCode HTTByteStreamDecoderAlg::addEventFooter(const HTTEventInputHeader&  eventHeader, std::stringstream& eventstring){
  std::string flagString = "1 ";
  HTTEventInfo eventInfo= eventHeader.event() ;
  std::vector<unsigned long int> newEventFooterWordTV = {0, 0, 0};
  ATH_CHECK(m_decoderTool->convertEventFooter(eventInfo, newEventFooterWordTV));
  for (int i = 0; i< 3; i++){
    if ( i == 0 ) flagString = "1 ";
    else flagString = "0 ";
    eventstring << flagString << std::setw(16) << std::setfill('0') << std::hex << newEventFooterWordTV[i] << std::endl;
  }
  return StatusCode::SUCCESS;
}

StatusCode HTTByteStreamDecoderAlg::createTrackHitMap(const HTTTrack& track, unsigned int& hitMap){
    unsigned int position=0;
    for(auto& hit : track.getHTTHits()){ //assume all the 8 hits are there
      if(hit.isMapped()){
	hitMap |= (1<< position);
	position++;
      }
    }
    return StatusCode::SUCCESS;
}

StatusCode HTTByteStreamDecoderAlg::decode_RAW(std::stringstream& eventstring){
  ATH_MSG_DEBUG("Entering decode_RAW: TP IN");

  eventstring.str(std::string());
  std::string flagString = "1 ";


  // only strips added in raw format; pixels to come
  dataFormat dataType = RAWSTRIP;
  ATH_CHECK(addEventHeader(*m_inputHeader, eventstring, dataType)); //event header is the same for RAWSTRIP or RAWPIXEL
  ATH_MSG_DEBUG("Event Header added");
  HTTEventInfo eventInfo= m_inputHeader->event() ;

  // Hits
  // group hits per module? which order? we may add a util function to order the hits as in the hardware (in HTTUtils)
  std::map <unsigned int, std::vector<HTTHit> > hits_per_module;
  std::vector<HTTHit> fulllist= m_inputHeader->hits();
  for (auto hit: fulllist){
    auto moduleId= hit.getHTTIdentifierHash();
    hits_per_module[moduleId].push_back(hit);
  }

  for (auto const& moduleHits: hits_per_module){
    auto moduleId = moduleHits.first;
    auto hits = moduleHits.second;
    HTTModuleInfo mod;
    mod.Id= moduleId;
    mod.type= 0x0; //0x0 raw, clustered 0x1, clusteredwraw 0x02, usused 0x3
    mod.det=hits.front().isStrip();

    //// Module header
    std::vector<unsigned long int> newModuleHeaderWordTV;
    ATH_CHECK(m_decoderTool->convertModuleHeader(mod, newModuleHeaderWordTV));
    for (int i = 0; i< 2; i++){
      if ( i == 0 ) {
	flagString = "1 ";
	eventstring << flagString << std::setw(16) << std::setfill('0') << std::hex << newModuleHeaderWordTV[i] << std::endl;
      }
      else {
	flagString = "0 ";
	eventstring << flagString << std::setw(8) << std::setfill('0') << std::hex << newModuleHeaderWordTV[i];
      }
    }

    if (mod.det == true) { // strip
      ATH_MSG_DEBUG("Module "<< moduleId<<": containing "<< hits.size()<<" HCC clusters");

      /// This is only for Strips, Pixel are not there yet
      //// HCC Header
      unsigned long int newHCCHeaderWordTV = 0;
      ATH_CHECK(m_decoderTool->convertRawStripModuleHeader(eventInfo, newHCCHeaderWordTV));//change this
      eventstring << std::setw(4) << std::setfill('0') << std::hex << newHCCHeaderWordTV;

      //// HCC Clusters
      flagString = "0 ";
      int count_hitS = 0;
      for (auto &hit : hits){
          unsigned long int newRawStripWordTV = 0;
          ATH_CHECK(m_decoderTool->convertRawStripModuleData(hit, newRawStripWordTV));
	if ((count_hitS) % 4 == 0) eventstring << std::setw(4) << std::setfill('0') << std::hex << newRawStripWordTV << std::endl;
	else if ((count_hitS+3) % 4 == 0) eventstring << flagString << std::setw(4) << std::setfill('0') << std::hex << newRawStripWordTV;
	else eventstring << std::setw(4) << std::setfill('0') << std::hex << newRawStripWordTV;
	count_hitS++;
      }

      if ((count_hitS+3) % 4 == 0){
	eventstring << flagString << std::setw(4) << std::setfill('0') << std::hex << HCC_LAST_CLUSTER;
	eventstring << std::setw(12) << std::setfill('0') << std::hex << 0 << std::endl;
      }
      if ((count_hitS+2) % 4 == 0){
	eventstring << std::setw(4) << std::setfill('0') << std::hex << HCC_LAST_CLUSTER;
	eventstring << std::setw(8) << std::setfill('0') << std::hex << 0 << std::endl;
      }
      if ((count_hitS+1) % 4 == 0){
	eventstring << std::setw(4) << std::setfill('0') << std::hex << HCC_LAST_CLUSTER;
	eventstring << std::setw(4) << std::setfill('0') << std::hex << 0 << std::endl;
      }
      if ((count_hitS) % 4 == 0){
	eventstring << std::setw(4) << std::setfill('0') << std::hex << HCC_LAST_CLUSTER;
      }
    }
    
    else { 
      ATH_MSG_DEBUG("Module "<< moduleId<<": containing "<< hits.size()<<" pixel clusters: decoder still not available");
      eventstring << std::setw(8) << std::setfill('0') << std::hex << 0 << std::endl;
    }
  }

  //// Event Footer
  ATH_CHECK(addEventFooter(*m_inputHeader, eventstring));

  return StatusCode::SUCCESS;
}


StatusCode HTTByteStreamDecoderAlg::decode_CLUSTER(std::stringstream& eventstring){
  ATH_MSG_DEBUG("Entering decode_CLUSTER: PRM IN");
  eventstring.str(std::string());//reset
  std::string flagString = "1 ";

  HTTEventInfo eventInfo= m_logInputHeader_1st->event() ;
  dataFormat dataType = PIXELCLUSTER;

  ATH_CHECK(addEventHeader(*m_inputHeader, eventstring, dataType)); //event header is the same for RAWSTRIP or RAWPIXEL

  auto tower=  m_logInputHeader_1st->getTower(0);

  // Hits
  std::map <unsigned int, std::vector<HTTHit> > hits_per_module;
  // loop over hits:
  for (auto &hit:  tower->hits()){
    auto moduleId= hit.getHTTIdentifierHash();
    hits_per_module[moduleId].push_back(hit);
  }

  for (auto const& moduleHits: hits_per_module){
    auto moduleId = moduleHits.first;
    auto hits = moduleHits.second;
    HTTModuleInfo mod;
    mod.Id= moduleId;
    mod.type= 0x1; //cluster //0x0 raw, clustered 0x1, clusteredwraw 0x02, usused 0x3
    mod.det=hits.front().isPixel(); // det 0x0 pix 0x1 strip

    // Module Header
    std::vector<unsigned long int> newModuleHeaderWordTV = {0, 0};
    ATH_CHECK(m_decoderTool->convertModuleHeader(mod, newModuleHeaderWordTV));
    for (int i = 0; i< 2; i++){
      if ( i == 0 ){
	flagString = "1 ";
	eventstring << flagString << std::setw(16) << std::setfill('0') << std::hex << newModuleHeaderWordTV[i] << std::endl;
      }
      else {
	flagString = "0 ";
	eventstring << flagString << std::setw(8) << std::setfill('0') << std::hex << newModuleHeaderWordTV[i];
      }
    }

    if(mod.det == 0x0) {
      // Pixel Cluster data
      ATH_MSG_DEBUG("Module "<< moduleId << ":Pixel clusters ");
      int count_hitP = 0;
      flagString = "0 ";
      // looping over all the clusters
      for (auto &hit : hits){
	unsigned long int newPixelWordTV = 0;
	ATH_CHECK(m_decoderTool->convertPixelClusterModuleData(hit, newPixelWordTV));
	if ((count_hitP) % 2 == 0) eventstring << std::setw(8) << std::setfill('0') << std::hex << newPixelWordTV << std::endl;
	else eventstring << flagString << std::setw(8) << std::setfill('0') << std::hex << newPixelWordTV;
	count_hitP++;
      }

      // Pixel Cluster footer
      unsigned long int newPixelFooterWordTV = 0;
      ATH_CHECK(m_decoderTool->convertPixelClusterFooter(eventInfo, newPixelFooterWordTV));
      if((count_hitP+1) % 2 == 0) {
	eventstring << flagString << std::setw(8) << std::setfill('0') << std::hex << newPixelFooterWordTV ;
	eventstring << std::setw(8) << std::setfill('0') << std::hex << 0 << std::endl;
      }
      else {
	eventstring << std::setw(8) << std::setfill('0') << std::hex << newPixelFooterWordTV << std::endl;
      }
    }
    else {
      // Strip Cluster data
      ATH_MSG_DEBUG("Module "<< moduleId << ":Strip clusters ");
      int count_hitS = 0;
      flagString = "0 ";
      // looping over all the clusters
      for (auto &hit : hits){
	unsigned long int newStripWordTV =0;
	ATH_CHECK(m_decoderTool->convertStripClusterModuleData(hit, newStripWordTV));
	if ((count_hitS+3) % 4 == 0) eventstring << std::setw(4) << std::setfill('0') << std::hex << newStripWordTV << std::endl;
	else if ((count_hitS+2) % 4 == 0) eventstring << flagString << std::setw(4) << std::setfill('0') << std::hex << newStripWordTV;
	else eventstring << std::setw(4) << std::setfill('0') << std::hex << newStripWordTV;
	count_hitS++;
      }

      //Strip Cluster footer
      unsigned long int newStripFooterWordTV = 0;
      ATH_CHECK(m_decoderTool->convertStripClusterFooter(eventInfo, newStripFooterWordTV));
      if((count_hitS+3) % 4 == 0) {
	eventstring << std::setw(4) << std::setfill('0') << std::hex << newStripFooterWordTV << std::endl;
      }
      if((count_hitS+2) % 4 == 0) {
	eventstring << flagString << std::setw(4) << std::setfill('0') << std::hex << newStripFooterWordTV;
	eventstring << std::setw(12) << std::setfill('0') << std::hex << 0 << std::endl;
      }
      if((count_hitS+1) % 4 == 0) {
	eventstring << std::setw(4) << std::setfill('0') << std::hex << newStripFooterWordTV;
	eventstring << std::setw(8) << std::setfill('0') << std::hex << 0 << std::endl;
      }
      if((count_hitS) % 4 == 0) {
	eventstring << std::setw(4) << std::setfill('0') << std::hex << newStripFooterWordTV;
	eventstring << std::setw(4) << std::setfill('0') << std::hex << 0 << std::endl;
      }
    }
  }

  // Event footer
  ATH_CHECK(addEventFooter(*m_inputHeader, eventstring));

  return StatusCode::SUCCESS;
}



StatusCode HTTByteStreamDecoderAlg::decode_TRACK(std::stringstream& eventstring, dataFormat& dataType, bool& WITHSSID, bool& passOR){
  if (dataType == S1TRACK) {
    if(WITHSSID == false && passOR == false) ATH_MSG_DEBUG("Entering decode_TRACK: PRM OUT");
    if(WITHSSID == true && passOR == false) ATH_MSG_DEBUG("Entering decode_TRACK: PRM EXT");
    if(WITHSSID == false && passOR == true) ATH_MSG_DEBUG("Entering decode_TRACK: AMTP OUT");
  }
  if(dataType == S2TRACK) {
    if(WITHSSID == false && passOR == false) ATH_MSG_DEBUG("Entering decode_TRACK: TFM OUT");
    if(WITHSSID == true && passOR == false) ATH_MSG_DEBUG("Entering decode_TRACK: TFM EXT");
  }

  eventstring.str(std::string());//reset
  std::string flagString = "1 ";

  HTTEventInfo eventInfo = m_logInputHeader_1st->event() ;
  ATH_CHECK(addEventHeader(*m_inputHeader, eventstring, dataType)); //event header is the same for RAWSTRIP or RAWPIXEL

  // get the tracks from the event
  if (m_doDummyTestVectors) {
    ATH_MSG_DEBUG("Track test vector dummy - two track blocks with the same content");
  }

  int ntracks_1st_all = 0;
  int ntracks_1st_passedOR = 0;
  std::vector<HTTTrack> tracks_1st;
  if (passOR) {
    for (auto track : m_logOutputHeader->getHTTTracks_1st()) {
      ntracks_1st_all++;
      if (track.passedOR() == 1) {
        tracks_1st.push_back(track);
        ntracks_1st_passedOR++;
      }
    }
    ATH_MSG_DEBUG("Number of tracks after OR = " << ntracks_1st_passedOR << " out of " << ntracks_1st_all);  
  }
  else {
    for (auto track : m_logOutputHeader->getHTTTracks_1st()) {
      tracks_1st.push_back(track);
    }
  }

  int ntrack_1st = 0;
  for (auto track: tracks_1st) {
    unsigned int hitMap_toset=0;
    ATH_CHECK(createTrackHitMap(track, hitMap_toset));
    track.setHitMap(hitMap_toset);
    // for (auto track : m_logOutputHeader->getHTTTracks_1st()) {
    std::vector<unsigned long int> newTrackWordTV = {0, 0, 0, 0};
    ATH_CHECK(m_decoderTool->convertTrackBlockData(track, WITHSSID, newTrackWordTV));
    if (ntrack_1st == 0)
      ATH_MSG_DEBUG("Track Block 1 - 1 pixel cluster, 7 strip clusters, zero padding");
    else
      ATH_MSG_DEBUG("Track Block 2 - 4 pixel clusters, 4 strip clusters");

    // track header words
    for (int i = 0; i < 4; i++) {
      if (i == 0) flagString = "1 ";
      else flagString = "0 ";
      eventstring << flagString << std::setw(16) << std::setfill('0')
		              << std::hex << newTrackWordTV[i] << std::endl;
    }

    std::stringstream hitstring; // collect all words from track hits
    int nbits = 0;
    flagString = "0 ";
    for (auto & hit : track.getHTTHits()) { // loop over the track hits
      if (hit.isPixel()) {
        // Track Pixel Cluster
      	unsigned long int newTrackPixelWordTV = 0;
      	ATH_CHECK(m_decoderTool->convertTrackPixelCluster(hit, newTrackPixelWordTV));
      	hitstring << flagString << std::setw(16) << std::setfill('0')
		              << std::hex << newTrackPixelWordTV;
      	nbits += 64;
      }
      else {
        //Track Strip cluster
        unsigned long int newTrackStripWordTV = 0;
      	ATH_CHECK(m_decoderTool->convertTrackStripCluster(hit, newTrackStripWordTV));
      	nbits += 32;
        if ((nbits % 64) == 0) {
          hitstring << std::setw(8) << std::setfill('0') << std::hex << newTrackStripWordTV;
        }
        else {
          hitstring << flagString << std::setw(8) << std::setfill('0') << std::hex << newTrackStripWordTV;
        }
      }

      if (nbits != 0 && nbits % 64 == 0 ) hitstring << std::endl; // end of 64 bits word
    }

    // add zero padding when have odd strip hits
    if (nbits % 64 == 32) {
      hitstring << std::setw(8) << std::setfill('0') << 0 << std::endl;
    }

    eventstring << hitstring.str();

    if (WITHSSID) {
      std::stringstream ssidstring;
      ATH_CHECK(addSSIDINFO(track, ssidstring)); // Adding SSID info
      eventstring << ssidstring.str();
    }

    ntrack_1st++;
  } //end of loop over tracks

  // Event footer
  ATH_CHECK(addEventFooter(*m_inputHeader, eventstring));

  return StatusCode::SUCCESS;
}


StatusCode HTTByteStreamDecoderAlg::addSSIDINFO(const HTTTrack& track, std::stringstream& ssidstring){
  ATH_MSG_DEBUG("Entering addSSIDINFO");
  ssidstring.str(std::string());//reset
  std::string flagString = "0 ";

  int nhits=0;
  // Get hits and SSID for the track
  for (HTTHit const& hit : track.getHTTHits()){ // loop over the track hits
    if(hit.isMapped()){
      if(hit.getHitType() != HitType::guessed){ //check for missed hits which are labelled HitType::mapped
	    // // retrieve the SSID
	      ssid_t ssid = m_HTTMapping->SSMap()->getSSID(hit);
	      if (ssid != SSID_ERROR){
	        HTT_SSID const& bitfield(ssid); // 
	        // Build here the SSID string
	        unsigned long int newSSIDWordTV = 0;
	        ATH_CHECK(m_decoderTool->convertSSIDData(hit, bitfield, newSSIDWordTV));
	        if (nhits % 4 == 0 ) { ssidstring << flagString << std::setw(4) << std::setfill('0') << std::hex << newSSIDWordTV; }
	        else { ssidstring << std::setw(4) << std::setfill('0') << std::hex << newSSIDWordTV; }
	        nhits+=1;
	      }
	      else {
	        ATH_MSG_WARNING("SSID info not found");
	      }
	      if (nhits!=0 && nhits % 4 == 0 ) ssidstring<<std::endl; //end of 64 bits word
      }
    }
  }

  // zero padding when needed
  if ((nhits+3) % 4 == 0) ssidstring << std::setw(12) << std::setfill('0') << std::hex << 0 << std::endl;
  if ((nhits+2) % 4 == 0) ssidstring << std::setw(8) << std::setfill('0') << std::hex << 0 << std::endl;
  if ((nhits+1) % 4 == 0) ssidstring << std::setw(4) << std::setfill('0') << std::hex << 0 << std::endl;

  return StatusCode::SUCCESS;
}


StatusCode HTTByteStreamDecoderAlg::createDummyData() {
  ATH_MSG_DEBUG("Entering createDummyData");

  m_inputHeader        = new HTTEventInputHeader();
  m_logInputHeader_1st = new HTTLogicalEventInputHeader();
  m_logInputHeader_2nd = new HTTLogicalEventInputHeader();
  m_logOutputHeader    = new HTTLogicalEventOutputHeader();

  //Creating an eventInfo
  m_event_counter++;
  unsigned long RunNumber = 00000000;
  unsigned long eventNumber = m_event_counter;
  int bcid = 666;

  HTTEventInfo tv_info (RunNumber,  eventNumber, bcid);
  m_inputHeader->newEvent(tv_info);
  m_logInputHeader_1st->newEvent(tv_info);
  m_logInputHeader_2nd->newEvent(tv_info);

  // TP INPUT
  //Creating a raw hit
  HTTHit tv_rawhit;
  tv_rawhit.setHitType(HitType::unmapped);
  tv_rawhit.setDetType(SiliconTech::strip);
  tv_rawhit.setEtaIndex(1);
  tv_rawhit.setPhiIndex(2);
  tv_rawhit.setEtaWidth(3);
  tv_rawhit.setPhiWidth(2);
  tv_rawhit.setIdentifierHash(1);// this is the mdduleID

  // Fill the InputHeader
  m_inputHeader->addHit(tv_rawhit);

  // PRM INPUT
  //TP Creating pixel & strip HTTHit
  HTTHit tv_Pcluster1;
  tv_Pcluster1.setDetType(SiliconTech::pixel);
  tv_Pcluster1.setHitType(HitType::clustered);
  tv_Pcluster1.setPhiCoord(2);
  tv_Pcluster1.setEtaCoord(1);
  tv_Pcluster1.setEtaWidth(3);
  tv_Pcluster1.setPhiWidth(2);
  tv_Pcluster1.setLayer(5);
  tv_Pcluster1.setIdentifierHash(1);

  HTTHit tv_Pcluster2;
  tv_Pcluster2.setDetType(SiliconTech::pixel);
  tv_Pcluster2.setHitType(HitType::clustered);
  tv_Pcluster2.setPhiCoord(2);
  tv_Pcluster2.setEtaCoord(1);
  tv_Pcluster2.setEtaWidth(3);
  tv_Pcluster2.setPhiWidth(2);
  tv_Pcluster2.setLayer(5);
  tv_Pcluster2.setIdentifierHash(1);

  HTTHit tv_Pcluster3;
  tv_Pcluster3.setDetType(SiliconTech::pixel);
  tv_Pcluster3.setHitType(HitType::clustered);
  tv_Pcluster3.setPhiCoord(2);
  tv_Pcluster3.setEtaCoord(1);
  tv_Pcluster3.setEtaWidth(3);
  tv_Pcluster3.setPhiWidth(2);
  tv_Pcluster3.setLayer(5);
  tv_Pcluster3.setIdentifierHash(2);

  HTTHit tv_Pcluster4;
  tv_Pcluster4.setDetType(SiliconTech::pixel);
  tv_Pcluster4.setHitType(HitType::clustered);
  tv_Pcluster4.setPhiCoord(2);
  tv_Pcluster4.setEtaCoord(1);
  tv_Pcluster4.setEtaWidth(3);
  tv_Pcluster4.setPhiWidth(2);
  tv_Pcluster4.setLayer(5);
  tv_Pcluster4.setIdentifierHash(3);

  HTTHit tv_Pcluster5;
  tv_Pcluster5.setDetType(SiliconTech::pixel);
  tv_Pcluster5.setHitType(HitType::clustered);
  tv_Pcluster5.setPhiCoord(2);
  tv_Pcluster5.setEtaCoord(1);
  tv_Pcluster5.setEtaWidth(3);
  tv_Pcluster5.setPhiWidth(2);
  tv_Pcluster5.setLayer(5);
  tv_Pcluster5.setIdentifierHash(4);

  HTTHit tv_Scluster1;
  tv_Scluster1.setDetType(SiliconTech::strip);
  tv_Scluster1.setHitType(HitType::clustered);
  tv_Scluster1.setPhiCoord(2);
  tv_Scluster1.setEtaCoord(1);
  tv_Scluster1.setEtaWidth(3);
  tv_Scluster1.setPhiWidth(2);
  tv_Scluster1.setLayer(5);
  tv_Scluster1.setIdentifierHash(5);

  HTTHit tv_Scluster2;
  tv_Scluster2.setDetType(SiliconTech::strip);
  tv_Scluster2.setHitType(HitType::clustered);
  tv_Scluster2.setPhiCoord(2);
  tv_Scluster2.setEtaCoord(1);
  tv_Scluster2.setEtaWidth(3);
  tv_Scluster2.setPhiWidth(2);
  tv_Scluster2.setLayer(5);
  tv_Scluster2.setIdentifierHash(5);

  HTTHit tv_Scluster3;
  tv_Scluster3.setDetType(SiliconTech::strip);
  tv_Scluster3.setHitType(HitType::clustered);
  tv_Scluster3.setPhiCoord(2);
  tv_Scluster3.setEtaCoord(1);
  tv_Scluster3.setEtaWidth(3);
  tv_Scluster3.setPhiWidth(2);
  tv_Scluster3.setLayer(5);
  tv_Scluster3.setIdentifierHash(6);

  HTTHit tv_Scluster4;
  tv_Scluster4.setDetType(SiliconTech::strip);
  tv_Scluster4.setHitType(HitType::clustered);
  tv_Scluster4.setPhiCoord(2);
  tv_Scluster4.setEtaCoord(1);
  tv_Scluster4.setEtaWidth(3);
  tv_Scluster4.setPhiWidth(2);
  tv_Scluster4.setLayer(5);
  tv_Scluster4.setIdentifierHash(7);

  HTTHit tv_Scluster5;
  tv_Scluster5.setDetType(SiliconTech::strip);
  tv_Scluster5.setHitType(HitType::clustered);
  tv_Scluster5.setPhiCoord(2);
  tv_Scluster5.setEtaCoord(1);
  tv_Scluster5.setEtaWidth(3);
  tv_Scluster5.setPhiWidth(2);
  tv_Scluster5.setLayer(5);
  tv_Scluster5.setIdentifierHash(8);

  // add the clusters to the event
  HTTTowerInputHeader tower(1);
  tower.addHit(tv_Pcluster1);
  tower.addHit(tv_Pcluster2);
  tower.addHit(tv_Pcluster3);
  tower.addHit(tv_Pcluster4);
  tower.addHit(tv_Pcluster5);
  tower.addHit(tv_Scluster1);
  tower.addHit(tv_Scluster2);
  tower.addHit(tv_Scluster3);
  tower.addHit(tv_Scluster4);
  tower.addHit(tv_Scluster5);
  m_logInputHeader_1st->reserveTowers((size_t)1);
  m_logInputHeader_2nd->reserveTowers((size_t)1);
  m_logInputHeader_1st->addTower(tower);
  m_logInputHeader_2nd->addTower(tower);

  // ----- PRM OUT
  //creating 2 HTTHits, one for strip, one for pixel
  HTTHit tv_strip_hit;
  tv_strip_hit.setDetType(SiliconTech::strip);
  tv_strip_hit.setHitType(HitType::mapped);
  tv_strip_hit.setPhiCoord(0);
  tv_strip_hit.setEtaCoord(1);
  tv_strip_hit.setIdentifierHash(1);// global moduleID
  tv_strip_hit.setLayer(5);
  tv_strip_hit.setSection(0);
  tv_strip_hit.setEtaWidth(1);


  HTTHit tv_pix_hit;
  tv_pix_hit.setDetType(SiliconTech::pixel);
  tv_pix_hit.setHitType(HitType::mapped);
  tv_pix_hit.setPhiCoord(2);
  tv_pix_hit.setEtaCoord(1);
  tv_pix_hit.setIdentifierHash(2);// global moduleID
  tv_pix_hit.setLayer(0);
  tv_pix_hit.setSection(0);
  tv_pix_hit.setEtaWidth(3);
  tv_pix_hit.setPhiWidth(2);


  // 8 HTTHits --> SSID --> track
  //Creating first track (Track Block 1 - 1 pixel cluster, 7 strip clusters)
  HTTTrack tv_track;
  tv_track.setTrackStage(TrackStage::FIRST);
  tv_track.setNLayers(8);
  tv_track.setTrackID(666);
  tv_track.setEta(0.7);
  tv_track.setPhi(1.5);
  tv_track.setQOverPt(0.01);
  tv_track.setD0(0.3);
  tv_track.setZ0(0.1);
  tv_track.setChi2(2.);
  tv_track.setFirstSectorID(267);
  tv_track.setSecondSectorID(0);
  // add one pix HTTHit
  //std::cout<<"Adding 0"<<std::endl;
  tv_track.setHTTHit(0,tv_pix_hit);
  // add 7 strip hits
  for (int i=1; i<8;i++){
    //std::cout<<"Adding "<<i<<std::endl;
    tv_track.setHTTHit(i,tv_strip_hit);
  }

  unsigned int hitMap1=0;
  ATH_CHECK(createTrackHitMap(tv_track, hitMap1));
  tv_track.setHitMap(hitMap1);

  //Creating second track (Track Block 2 - 4 pixel clusters, 4 strip clusters)
  // same parameters, excepts the hits
  HTTTrack tv_track2;
  tv_track2.setTrackStage(TrackStage::FIRST);
  tv_track2.setNLayers(8);
  tv_track2.setTrackID(666);
  tv_track2.setEta(0.7);
  tv_track2.setPhi(1.5);
  tv_track2.setQOverPt(0.01);
  tv_track2.setD0(0.3);
  tv_track2.setZ0(0.1);
  tv_track2.setChi2(2.);
  tv_track2.setFirstSectorID(267);
  tv_track2.setSecondSectorID(0);
  // add 4 pix HTTHits
  for (int i=0; i<4;i++){
     //std::cout<<"Adding "<<i<<std::endl;
     tv_track2.setHTTHit(i,tv_pix_hit);
  }
  // add 4 strip hits
  for (int i=4; i<8;i++) {
    //    std::cout<<"Adding "<<i<<std::endl;
     tv_track2.setHTTHit(i,tv_strip_hit);
  }
  unsigned int hitMap2=0;
  ATH_CHECK(createTrackHitMap(tv_track2, hitMap2));
  tv_track2.setHitMap(hitMap2);

  std::vector<HTTTrack> allHTTtracks;
  allHTTtracks.clear();
  allHTTtracks.push_back(tv_track);
  allHTTtracks.push_back(tv_track2);

  // add the tracks to the event
   m_logOutputHeader->addHTTTracks_1st(allHTTtracks);
   ATH_MSG_DEBUG("Track Size = " << m_logOutputHeader->nHTTTracks_1st());

   return StatusCode::SUCCESS;
}
