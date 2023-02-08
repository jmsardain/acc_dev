/*
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
// inherited from https://gitlab.cern.ch/atlas/athena/blob/master/Trigger/TrigT1/TrigT1ResultByteStream/src/RoIBResultByteStreamTool.cxx

// Local includes:
#include "TrigHTTByteStream/TrigHTTTPByteStreamTool.h"
#include "TrigHTTByteStream/TrigHTTTPByteStreamStructure.h"

// Gaudi includes:
#include "GaudiKernel/MsgStream.h"

// TDAQ includes:
#include "eformat/SourceIdentifier.h"

// System includes
#include <exception>
#include <sstream>


TrigHTTTPByteStreamTool::TrigHTTTPByteStreamTool( const std::string& type, const std::string& name, const IInterface* parent )
: AthAlgTool( type, name, parent ) {}


StatusCode TrigHTTTPByteStreamTool::initialize() {
  ATH_MSG_DEBUG("Initialising TrigHTTTPByteStreamTool");

  std::ostringstream str;

  return StatusCode::SUCCESS;
}

StatusCode TrigHTTTPByteStreamTool::createTrackDetMap(const HTTTrack& track, unsigned int& detMap){
  unsigned int position=0;
  for(auto hit: track.getHTTHits()){
    unsigned int bit=hit.isStrip();
    detMap |= (bit<< position);
    position++;
  }
  return StatusCode::SUCCESS;
}

// Conversion from fragment to HTT Event Info - Event Header
StatusCode TrigHTTTPByteStreamTool::convertEventHeader( const std::vector<unsigned long int>& fragment, HTTEventInfo& event, dataFormat& dataType) {
  
  (void) dataType;

  ATH_MSG_DEBUG("----------------------------------------------------------------");
  ATH_MSG_DEBUG("--- Executing convertEventHeader() ");
  ATH_MSG_DEBUG("--- from 6/6 words of Event Header fragment to HTT Event Info");
  ATH_MSG_DEBUG("----------------------------------------------------------------");

  EVT_HDR_W1 infoword1;
  infoword1.fullword = fragment[0];
  ATH_MSG_DEBUG("---- word 1/6");
  ATH_MSG_DEBUG("   > FLAG: "  << std::hex << infoword1.fields.FLAG);
  if (infoword1.fields.DATA_TYPE == 0x1) {
    if (infoword1.fields.TRK_TYPE == 0x1 ) dataType = S1TRACK;
    else dataType = S2TRACK;
  } else dataType = RAWPIXEL; //in principle it is just any module-like data, from this until stripCluster
  ATH_MSG_DEBUG("   > Filled TRK_TYPE: "  << std::hex << infoword1.fields.TRK_TYPE);
  ATH_MSG_DEBUG("   > Filled DATA_TYPE: "  << std::hex << infoword1.fields.DATA_TYPE);
  ATH_MSG_DEBUG("   > ISGHIT: "  << std::hex << infoword1.fields.ISGHIT);
  ATH_MSG_DEBUG("   > SPARE: "  << std::hex << infoword1.fields.SPARE);
  ATH_MSG_DEBUG("   > NOTHING TO DO with L0ID: "  << std::hex << infoword1.fields.L0ID);

  EVT_HDR_W2 infoword2;
  infoword2.fullword = fragment[1];
  ATH_MSG_DEBUG("---- word 2/6");
  event.setBCID(infoword2.fields.BCID);
  ATH_MSG_DEBUG("   > Filled BCID: "  << std::hex << event.BCID());
  ATH_MSG_DEBUG("   > SPARE: "  << std::hex << infoword2.fields.SPARE);
  event.setRunNumber(infoword2.fields.RUNNUMBER);
  ATH_MSG_DEBUG("   > Filled RUNNUMBER: "  << std::hex << event.runNumber());

  EVT_HDR_W3 infoword3;
  infoword3.fullword = fragment[2];
  ATH_MSG_DEBUG("---- word 3/6");
  ATH_MSG_DEBUG("   > NOTHING TO DO WITH ROI: "  << std::hex << infoword3.fields.ROI);

  EVT_HDR_W4 infoword4;
  infoword4.fullword = fragment[3];
  ATH_MSG_DEBUG("---- word 4/6");
  ATH_MSG_DEBUG("   > NOTHING TO DO with EFPU_ID: "  << std::hex << infoword4.fields.EFPU_ID);
  ATH_MSG_DEBUG("   > NOTHING TO DO with EFPU_PID: "  << std::hex << infoword4.fields.EFPU_PID);
  ATH_MSG_DEBUG("   > NOTHING TO DO with TIME: "  << std::hex << infoword4.fields.TIME);

  EVT_HDR_W5 infoword5;
  infoword5.fullword = fragment[4];
  ATH_MSG_DEBUG("---- word 5/6");
  ATH_MSG_DEBUG("   > NOTHING TO DO with Connection_ID: "  << std::hex << infoword5.fields.Connection_ID);
  ATH_MSG_DEBUG("   > NOTHING TO DO with Transaction_ID: "  << std::hex << infoword5.fields.Transaction_ID);

  EVT_HDR_W6 infoword6;
  infoword6.fullword = fragment[5];
  ATH_MSG_DEBUG("---- word 6/6");
  ATH_MSG_DEBUG("   > NOTHING TO DO with STATUS: "  << std::hex << infoword6.fields.STATUS);
  ATH_MSG_DEBUG("   > NOTHING TO DO with CRC: "  << std::hex << infoword6.fields.CRC);

  ATH_MSG_DEBUG("---- Done!");
  ATH_MSG_DEBUG("----------------------------------------------------------------");

  return StatusCode::SUCCESS;
}


// Conversion from HTT Event Info to fragment  - Event Header
StatusCode TrigHTTTPByteStreamTool::convertEventHeader( const HTTEventInfo& event, dataFormat& dataType, std::vector<unsigned long int>& fragment) {

  ATH_MSG_DEBUG("----------------------------------------------------------------");
  ATH_MSG_DEBUG("--- Executing convertEventHeader() ");
  ATH_MSG_DEBUG("--- from HTT Event Info to 6/6 words of Event Header fragment");
  ATH_MSG_DEBUG("----------------------------------------------------------------");

  //Temporary data type
  // 0 = raw pixel; 1 = raw strip; 2 = pixel cluster; 3 = strip cluster; 4 = 1st stage track; 5 = 2d stage track

  //reset
  fragment.reserve(6);
  fragment.clear();
  EVT_HDR_W1 infoword1;
  GenMetaData consts;

  ATH_MSG_DEBUG("---- word 1/6");
  infoword1.fields.FLAG = consts.FLAG_EVT_HDR;
  ATH_MSG_DEBUG("   > FLAG: " << std::hex << infoword1.fields.FLAG );
  /// Temporary
  if (dataType == S2TRACK) infoword1.fields.TRK_TYPE = 0x2;
  else if (dataType == S1TRACK) infoword1.fields.TRK_TYPE = 0x1;
  else infoword1.fields.TRK_TYPE = 0x0; // 0x1 first stage, 0x2 second stage, added 0x0 when it is not a track
  ATH_MSG_DEBUG("   > Filled TRK_TYPE: "  << std::hex << infoword1.fields.TRK_TYPE  );

  if (dataType == S1TRACK || dataType == S2TRACK)  infoword1.fields.DATA_TYPE = 0x1; //  TRACKS=0x1 MODULES=0x0
  else  infoword1.fields.DATA_TYPE = 0x0; //  TRACKS=0x1 MODULES=0x0 , hardcoded
  ATH_MSG_DEBUG("   > Filled DATA_TYPE: "  << std::hex << infoword1.fields.DATA_TYPE );

  infoword1.fields.ISGHIT = 1; //check this info
  ATH_MSG_DEBUG("   > ISGHIT: "  << std::hex << infoword1.fields.ISGHIT << " - Not present in HTTEventInfo");
  infoword1.fields.SPARE = 2; //hardcoded
  ATH_MSG_DEBUG("   > SPARE: "  << std::hex << infoword1.fields.SPARE << " - Not present in HTTEventInfo");
  infoword1.fields.L0ID = event.eventNumber(); //Filling the event ID to distinguish events in HTTSim
  ATH_MSG_DEBUG("   > L0ID: "  << std::hex << infoword1.fields.L0ID << " Filling value of Event ID here");

  EVT_HDR_W2 infoword2;
  ATH_MSG_DEBUG("---- word 2/6");
  infoword2.fields.BCID = event.BCID();
  ATH_MSG_DEBUG("   > Filled BCID: "  << std::hex << infoword2.fields.BCID);
  infoword2.fields.SPARE = 2; //harcoded
  ATH_MSG_DEBUG("   > SPARE: "  << std::hex << infoword2.fields.SPARE << " - Not present in HTTEventInfo");
  infoword2.fields.RUNNUMBER = event.runNumber();
  ATH_MSG_DEBUG("   > Filled RUNNUMBER: "  << std::hex << infoword2.fields.RUNNUMBER);

  EVT_HDR_W3 infoword3;
  ATH_MSG_DEBUG("---- word 3/6");
  infoword3.fields.ROI = 4; //hardcoded
  ATH_MSG_DEBUG("   > ROI: "  << std::hex << infoword3.fields.ROI << " - Not present in HTTEventInfo");

  EVT_HDR_W4 infoword4;
  ATH_MSG_DEBUG("---- word 4/6");
  infoword4.fields.EFPU_ID = 22; //hardcoded
  ATH_MSG_DEBUG("   > EFPU_ID: "  << std::hex << infoword4.fields.EFPU_ID << " - Not present in HTTEventInfo");
  infoword4.fields.EFPU_PID = 33; //hardcoded
  ATH_MSG_DEBUG("   > EFPU_PID: "  << std::hex << infoword4.fields.EFPU_PID << " - Not present in HTTEventInfo");
  infoword4.fields.TIME = 1; //hardcoded
  ATH_MSG_DEBUG("   > TIME: "  << std::hex << infoword4.fields.TIME << " - Not present in HTTEventInfo");

  EVT_HDR_W5 infoword5;
  ATH_MSG_DEBUG("---- word 5/6");
  infoword5.fields.Connection_ID = 765; //hardcoded
  ATH_MSG_DEBUG("   > Connection_ID: "  << std::hex << infoword5.fields.Connection_ID << " - Not present in HTTEventInfo");
  infoword5.fields.Transaction_ID = 567; //hardcoded
  ATH_MSG_DEBUG("   > Transaction_ID: "  << std::hex << infoword5.fields.Transaction_ID << " - Not present in HTTEventInfo");

  EVT_HDR_W6 infoword6;
  ATH_MSG_DEBUG("---- word 6/6");
  infoword6.fields.STATUS = 3; //hardcoded
  ATH_MSG_DEBUG("   > STATUS: "  << std::hex << infoword6.fields.STATUS );
  infoword6.fields.CRC = 11; //hardcoded
  ATH_MSG_DEBUG("   > CRC: "  << std::hex << infoword6.fields.CRC );

  fragment[0]=infoword1.fullword;
  fragment[1]=infoword2.fullword;
  fragment[2]=infoword3.fullword;
  fragment[3]=infoword4.fullword;
  fragment[4]=infoword5.fullword;
  fragment[5]=infoword6.fullword;

  ATH_MSG_DEBUG("---- Done!");
  ATH_MSG_DEBUG("----------------------------------------------------------------");

  return StatusCode::SUCCESS;
}


// Conversion from fragment to HTT Event Info - event Footer
StatusCode TrigHTTTPByteStreamTool::convertEventFooter( const std::vector<unsigned long int>&fragment, HTTEventInfo& event) {

  (void) event;

  ATH_MSG_DEBUG("----------------------------------------------------------------");
  ATH_MSG_DEBUG("--- Executing convertEventFooter() ");
  ATH_MSG_DEBUG("--- from 3/3 words of Event Footer fragment to HTT Event Info");
  ATH_MSG_DEBUG("----------------------------------------------------------------");

  EVT_FTR_W1 infoword1;
  infoword1.fullword = fragment[0];
  ATH_MSG_DEBUG("---- word 1/3");
  ATH_MSG_DEBUG("   > NOTHING TO DO with HDR_CRC: "  << std::hex << infoword1.fields.HDR_CRC);
  ATH_MSG_DEBUG("   > NOTHING TO DO with META_COUNT: "  << std::hex << infoword1.fields.META_COUNT);
  ATH_MSG_DEBUG("   > NOTHING TO DO with SPARE: "  << std::hex << infoword1.fields.SPARE);
  ATH_MSG_DEBUG("   > FLAG: "  << std::hex << infoword1.fields.FLAG);

  EVT_FTR_W2 infoword2;
  infoword2.fullword = fragment[1];
  ATH_MSG_DEBUG("---- word 2/3");
  ATH_MSG_DEBUG("   > NOTHING TO DO with ERROR_FLAGS: "  << std::hex << infoword2.fields.ERROR_FLAGS);

  EVT_FTR_W3 infoword3;
  infoword3.fullword = fragment[2];
  ATH_MSG_DEBUG("---- word 3/3");
  ATH_MSG_DEBUG("   > NOTHING TO DO with CRC: "  << std::hex << infoword3.fields.CRC);
  ATH_MSG_DEBUG("   > NOTHING TO DO with WORD_COUNT: "  << std::hex << infoword3.fields.WORD_COUNT);

  ATH_MSG_DEBUG("---- Done!");
  ATH_MSG_DEBUG("----------------------------------------------------------------");

  return StatusCode::SUCCESS;
}

// Conversion from HTT Event Info to fragment - Event Footer

StatusCode TrigHTTTPByteStreamTool::convertEventFooter( const HTTEventInfo& event , std::vector<unsigned long int>& fragment) {

  (void) event;

  ATH_MSG_DEBUG("----------------------------------------------------------------");
  ATH_MSG_DEBUG("--- Executing convertEventFooter() ");
  ATH_MSG_DEBUG("--- from HTT Event Info to 3/3 words of Event Footer fragment");
  ATH_MSG_DEBUG("----------------------------------------------------------------");

  //reset
  fragment.reserve(3);
  fragment.clear();

  EVT_FTR_W1 infoword1;
  GenMetaData consts;
  ATH_MSG_DEBUG("---- word 1/3");
  infoword1.fields.HDR_CRC =213; //hardcoded
  ATH_MSG_DEBUG("   > HDR_CRC: "  << std::hex << infoword1.fields.HDR_CRC);
  infoword1.fields.META_COUNT = 99; //hardcoded
  ATH_MSG_DEBUG("   > META_COUNT: "  << std::hex << infoword1.fields.META_COUNT << " - Not present in HTTEventInfo");
  infoword1.fields.SPARE = 2; //hardcoded
  ATH_MSG_DEBUG("   > SPARE: "  << std::hex << infoword1.fields.SPARE << " - Not present in HTTEventInfo");
  infoword1.fields.FLAG = consts.FLAG_EVT_FTR;
  ATH_MSG_DEBUG("   > FLAG: "  << std::hex << infoword1.fields.FLAG << " - Not present in HTTEventInfo");

  EVT_FTR_W2 infoword2;
  ATH_MSG_DEBUG("---- word 2/3");
  infoword2.fields.ERROR_FLAGS =0000004; //hardcoded
  ATH_MSG_DEBUG("   > ERROR_FLAGS: "  << std::hex << infoword2.fields.ERROR_FLAGS << " - Not present in HTTEventInfo");

  EVT_FTR_W3 infoword3;
  ATH_MSG_DEBUG("---- word 3/3");
  infoword3.fields.CRC =6; //hardcoded
  ATH_MSG_DEBUG("   > CRC: "  << std::hex << infoword3.fields.CRC );
  infoword3.fields.WORD_COUNT = 42; //hardcoded
  ATH_MSG_DEBUG("   > WORD_COUNT: "  << std::hex << infoword3.fields.WORD_COUNT << " - Not present in HTTEventInfo");

  fragment[0]=infoword1.fullword;
  fragment[1]=infoword2.fullword;
  fragment[2]=infoword3.fullword;

  ATH_MSG_DEBUG("---- Done!");
  ATH_MSG_DEBUG("----------------------------------------------------------------");

  return StatusCode::SUCCESS;
}

// Conversion from fragment to HTT Event Info  - Module Header
StatusCode TrigHTTTPByteStreamTool::convertModuleHeader( const std::vector<unsigned long int>& fragment, HTTModuleInfo& moduleInfo) {

  ATH_MSG_DEBUG("----------------------------------------------------------------");
  ATH_MSG_DEBUG("--- Executing convertModuleHeader() ");
  ATH_MSG_DEBUG("--- from 2/2 words of Module Header fragment to HTT Module");
  ATH_MSG_DEBUG("----------------------------------------------------------------");

  M_HDR infoword1;
  infoword1.fullword = fragment[0];
  ATH_MSG_DEBUG("---- word 1/2");
  ATH_MSG_DEBUG("   > FLAG: "  << std::hex << infoword1.fields.FLAG);
  moduleInfo.type = infoword1.fields.TYPE;
  ATH_MSG_DEBUG("   > Filled TYPE: "  << std::hex << infoword1.fields.TYPE);
  moduleInfo.det = infoword1.fields.DET;
  ATH_MSG_DEBUG("   > Filled DET: "  << std::hex << infoword1.fields.DET);
  ATH_MSG_DEBUG("   > ROUTING: "  << std::hex << infoword1.fields.ROUTING);
  ATH_MSG_DEBUG("   > SPARE: "  << std::hex << infoword1.fields.SPARE);

  M_HDR2 infoword2;
  infoword2.fullword = fragment[1];
  ATH_MSG_DEBUG("---- word 2/2");
  moduleInfo.Id = infoword2.fields.MODID;
  ATH_MSG_DEBUG("   > Filled MODID: "  << std::hex << infoword2.fields.MODID);
  ATH_MSG_DEBUG("   > MODTYPE: "  << std::hex << infoword2.fields.MODTYPE);
  ATH_MSG_DEBUG("   > ORIENTATION: "  << std::hex << infoword2.fields.ORIENTATION);
  ATH_MSG_DEBUG("   > SPARE: "  << std::hex << infoword2.fields.SPARE);

  ATH_MSG_DEBUG("---- Done!");
  ATH_MSG_DEBUG("----------------------------------------------------------------");

  moduleInfo.det  = infoword1.fields.DET;
  moduleInfo.type = infoword1.fields.TYPE;
  moduleInfo.Id   = infoword2.fields.MODID;

  return StatusCode::SUCCESS;
}

// Conversion from HTT Event Info to fragment - Module Header

StatusCode TrigHTTTPByteStreamTool::convertModuleHeader( const HTTModuleInfo& moduleInfo , std::vector<unsigned long int>& fragment) {

  ATH_MSG_DEBUG("----------------------------------------------------------------");
  ATH_MSG_DEBUG("--- Executing convertModuleHeader() ");
  ATH_MSG_DEBUG("--- from HTT Module Info to 2/2 words of Module Header fragment");
  ATH_MSG_DEBUG("----------------------------------------------------------------");

  //reset
  fragment.reserve(2);
  fragment.clear();

  M_HDR infoword1;
  GenMetaData consts;

  ATH_MSG_DEBUG("---- word 1/2");
  infoword1.fields.FLAG = consts.FLAG_M_HDR;
  ATH_MSG_DEBUG("   > FLAG: "  << std::hex << infoword1.fields.FLAG << " - Not present in HTTModuleInfo");
  infoword1.fields.TYPE = moduleInfo.type;  // 0x0 raw, clustered 0x1, clusteredwraw 0x02, usused 0x3
  ATH_MSG_DEBUG("   > Filled TYPE: "  << std::hex << infoword1.fields.TYPE );
  infoword1.fields.DET = moduleInfo.det;// pixel 0x0, strip 0x1
  ATH_MSG_DEBUG("   > Filled DET: "  << std::hex << infoword1.fields.DET) ;
  infoword1.fields.ROUTING = 0xc; //nprm = 0xc, nambus = 0x4, ntfm_per_ssTP = 0x2, PRM_msb = 0x30, TFM_lsb = 0x31
  ATH_MSG_DEBUG("   > ROUTING: "  << std::hex << infoword1.fields.ROUTING << " - Not present in HTTModuleInfo");
  infoword1.fields.SPARE = 0; //hardcoded
  ATH_MSG_DEBUG("   > SPARE: "  << std::hex << infoword1.fields.SPARE << " - Not present in HTTModuleInfo");

  M_HDR2 infoword2;
  ATH_MSG_DEBUG("---- word 2/2");
  infoword2.fields.MODID =moduleInfo.Id;//1; //hardcoded
  ATH_MSG_DEBUG("   > Filled MODID: "  << std::hex << infoword2.fields.MODID );
  infoword2.fields.MODTYPE = 0; //hardcoded
  ATH_MSG_DEBUG("   > Filled MODTYPE: "  << std::hex << infoword2.fields.MODTYPE << " - Not present in HTTModuleInfo");
  infoword2.fields.ORIENTATION= 0; //hardcoded
  ATH_MSG_DEBUG("   > ORIENTATION: "  << std::hex << infoword2.fields.ORIENTATION << " - Not present in HTTModuleInfo");
  infoword2.fields.SPARE = 0; //hardcoded
  ATH_MSG_DEBUG("   > SPARE: "  << std::hex << infoword2.fields.SPARE << " - Not present in HTTModuleInfo");

  fragment[0]=infoword1.fullword;
  fragment[1]=infoword2.fullword;

  ATH_MSG_DEBUG("---- Done!");
  ATH_MSG_DEBUG("----------------------------------------------------------------");

  return StatusCode::SUCCESS;
}



// Conversion from fragment to HTT Event Info  - Raw Strip (HCC) Module Header
StatusCode TrigHTTTPByteStreamTool::convertRawStripModuleHeader( const unsigned long int&info, HTTEventInfo& event) {

  ATH_MSG_DEBUG("----------------------------------------------------------------");
  ATH_MSG_DEBUG("--- Executing convertRawStripModuleHeader() ");
  ATH_MSG_DEBUG("--- from 1/1 words of Raw Strip Module Header fragment to HTT Event Info");
  ATH_MSG_DEBUG("----------------------------------------------------------------");

  HCC_HDR infoword;
  infoword.fullword = info;
  ATH_MSG_DEBUG("---- word 1/1");
  ATH_MSG_DEBUG("   > Filled TYP: "  << std::hex << infoword.fields.TYP);
  ATH_MSG_DEBUG("   > FLAG: " << std::hex << infoword.fields.FLAG);
  ATH_MSG_DEBUG("   > Filled L0ID: " << std::hex << infoword.fields.L0ID);
  event.setBCID(infoword.fields.BCID);
  ATH_MSG_DEBUG("   > Filled setBCID: " << infoword.fields.BCID);

  return StatusCode::SUCCESS;
}


// Conversion from HTT Event Info to fragment - Module Header

StatusCode TrigHTTTPByteStreamTool::convertRawStripModuleHeader( const HTTEventInfo& event , unsigned long int& fragment) {

  ATH_MSG_DEBUG("----------------------------------------------------------------");
  ATH_MSG_DEBUG("--- Executing convertRawStripModuleHeader() ");
  ATH_MSG_DEBUG("--- from HTT Event Info to 1/1 words of Raw Strip Module Header fragment");
  ATH_MSG_DEBUG("----------------------------------------------------------------");

  HCC_HDR infoword;
  // GenMetaData consts; //unused, since the FLAG is not included
  ATH_MSG_DEBUG("---- word 1/1");
  infoword.fields.TYP =0x2; //hardcoded PR = 0x1 LP = 0x2
  ATH_MSG_DEBUG("   > Filled TYP: "  << std::hex << infoword.fields.TYP << " - Not present in HTTEventInfo?");
  infoword.fields.FLAG = 0; //hardcoded
  ATH_MSG_DEBUG("   > FLAG: "  << std::hex << infoword.fields.FLAG << " - Not present in HTTEventInfo");
  infoword.fields.L0ID = 12;
  ATH_MSG_DEBUG("   > Filled L0ID: "  << std::hex << infoword.fields.L0ID << " - Not present in HTTEventInfo");
  infoword.fields.BCID = event.BCID();
  ATH_MSG_DEBUG("   > Filled BCID: "  << std::hex << infoword.fields.BCID);

  fragment=infoword.fullword;


  ATH_MSG_DEBUG("---- Done!");
  ATH_MSG_DEBUG("----------------------------------------------------------------");

  return StatusCode::SUCCESS;
}

// Conversion from fragment to HTT Raw Hit - Raw Strip Module Data
StatusCode TrigHTTTPByteStreamTool::convertRawStripModuleData(const unsigned long int& fragment, HTTHit& raw)  {

  ATH_MSG_DEBUG("----------------------------------------------------------------");
  ATH_MSG_DEBUG("--- Executing convertRawStripModuleData() ");
  ATH_MSG_DEBUG("--- from word 1/1 of Raw Strip Module Data fragment to HTTHit");
  ATH_MSG_DEBUG("----------------------------------------------------------------");

  HCC_CLUSTER rawstripclusterword;
  rawstripclusterword.fullword = fragment;
  ATH_MSG_DEBUG("---- word 1/1");
  ATH_MSG_DEBUG("   > NOTHING TO DO with UNUSED: "  << std::hex << rawstripclusterword.fields.UNUSED);
  ATH_MSG_DEBUG("   > NOTHING TO DO with ABC: "  << std::hex << rawstripclusterword.fields.ABC);
  raw.setPhiIndex(rawstripclusterword.fields.COL);
  ATH_MSG_DEBUG("   > Filled setPhiIndex(): "    << std::hex << rawstripclusterword.fields.COL);
  ATH_MSG_DEBUG("   > NOTHING TO DO with NEXT: "    << std::hex << rawstripclusterword.fields.NEXT);

  ATH_MSG_DEBUG("---- Done!");
  ATH_MSG_DEBUG("----------------------------------------------------------------");

  return StatusCode::SUCCESS;
}


// Conversion from HTT Raw Hit to fragment - Raw Strip Module Data

StatusCode TrigHTTTPByteStreamTool::convertRawStripModuleData(const HTTHit& raw, unsigned long int& fragment)  {

  ATH_MSG_DEBUG("----------------------------------------------------------------");
  ATH_MSG_DEBUG("--- Executing convertRawStripModuleData() ");
  ATH_MSG_DEBUG("--- from HTThit to word 1/1 of Raw Strip Module Data fragment ");
  ATH_MSG_DEBUG("----------------------------------------------------------------");

  HCC_CLUSTER rawstripclusterword;
  ATH_MSG_DEBUG("---- word 1/1");
  rawstripclusterword.fields.UNUSED = 0;
  ATH_MSG_DEBUG("   > UNUSED: "  << std::hex << rawstripclusterword.fields.UNUSED << " - Always zero");
  rawstripclusterword.fields.ABC = 1; //hardcoded 1
  ATH_MSG_DEBUG("   > Filled ABC: "  << std::hex << rawstripclusterword.fields.ABC << " - Not included in HTTHit");
  rawstripclusterword.fields.COL = raw.getPhiIndex(); //hardcoded
  ATH_MSG_DEBUG("   > Filled COL: "  << std::hex << rawstripclusterword.fields.COL);
  rawstripclusterword.fields.NEXT = 1; //hardcoded 1
  ATH_MSG_DEBUG("   > Filled NEXT: "  << std::hex << rawstripclusterword.fields.NEXT << " - Not included in HTTHit");

  fragment = rawstripclusterword.fullword;

  ATH_MSG_DEBUG("---- Done!");
  ATH_MSG_DEBUG("----------------------------------------------------------------");

  return StatusCode::SUCCESS;
}


// Conversion from fragment to HTT Cluster - Pixel Cluster Module Data
StatusCode TrigHTTTPByteStreamTool::convertPixelClusterModuleData(const unsigned long int& fragment, HTTHit& hit)  {

  ATH_MSG_DEBUG("----------------------------------------------------------------");
  ATH_MSG_DEBUG("--- Executing convertPixelClusterModuleData() ");
  ATH_MSG_DEBUG("--- from word 1/1 of Pixel Cluster Module Block fragment to HTTCluster");
  ATH_MSG_DEBUG("----------------------------------------------------------------");

  PIXEL_CLUSTER pixelclusterword;
  pixelclusterword.fullword = fragment;

  ATH_MSG_DEBUG("---- word 1/1");
  hit.setEtaCoord(pixelclusterword.fields.ETA);
  ATH_MSG_DEBUG("   > Filled hit.setEtaCoord(eta): "  << std::hex << hit.getEtaCoord());
  hit.setPhiCoord(pixelclusterword.fields.PHI);
  ATH_MSG_DEBUG("   > Filled hit.setPhiCoord(phi): "  << std::hex << hit.getPhiCoord());
  hit.setEtaWidth(pixelclusterword.fields.ETASIZ);
  ATH_MSG_DEBUG("   > Filled hit.setEtaWidth(): "  << std::hex << hit.getEtaWidth());
  hit.setPhiWidth(pixelclusterword.fields.PHISIZ);
  ATH_MSG_DEBUG("   > Filled hit.setPhiWidth(): "  << std::hex << hit.getPhiWidth());
  ATH_MSG_DEBUG("   > LAST: "   << std::hex << pixelclusterword.fields.LAST);

  ATH_MSG_DEBUG("---- Done!");
  ATH_MSG_DEBUG("----------------------------------------------------------------");

  return StatusCode::SUCCESS;
}

// Conversion from HTT Cluster to fragment - Pixel Cluster Module Data

StatusCode TrigHTTTPByteStreamTool::convertPixelClusterModuleData(const HTTHit& hit, unsigned long int& fragment)  {

  ATH_MSG_DEBUG("----------------------------------------------------------------");
  ATH_MSG_DEBUG("--- Executing convertPixelClusterModuleData() ");
  ATH_MSG_DEBUG("--- from HTTHit to word 1/1 of Pixel Cluster Module Block fragment");
  ATH_MSG_DEBUG("----------------------------------------------------------------");
  ATH_MSG_DEBUG(hit);
  
  PIXEL_CLUSTER pixelclusterword;
  ATH_MSG_DEBUG("---- word 1/1");
  pixelclusterword.fields.ETA = hit.getEtaCoord();
  ATH_MSG_DEBUG("   > Filled ETA: "  << std::hex << pixelclusterword.fields.ETA << " - From hit.getEtaCoord()");
  pixelclusterword.fields.PHI = hit.getPhiCoord();
  ATH_MSG_DEBUG("   > Filled PHI: "  << std::hex << pixelclusterword.fields.PHI << " - From hit.getPhiCoord()");
  pixelclusterword.fields.ETASIZ = hit.getEtaWidth();
  ATH_MSG_DEBUG("   > Filled ETASIZ "  << std::hex << pixelclusterword.fields.ETASIZ << " - From hit.getEtaWidth()");
  pixelclusterword.fields.PHISIZ = hit.getPhiWidth();
  ATH_MSG_DEBUG("   > Filled PHISIZ: "  << std::hex << pixelclusterword.fields.PHISIZ << " - From hit.getPhiWidth()");
  pixelclusterword.fields.LAST = 1; //1 if last cluster in module, zero otherwise
  ATH_MSG_DEBUG("   > LAST: "  << std::hex << pixelclusterword.fields.LAST << " - NEED TO FIX LAST");

  fragment = pixelclusterword.fullword;

  ATH_MSG_DEBUG("---- Done!");
  ATH_MSG_DEBUG("----------------------------------------------------------------");

  return StatusCode::SUCCESS;
}

////// Pixel Cluster Footer
// Conversion from fragment to HTT Cluster - Pixel Cluster Footer
StatusCode TrigHTTTPByteStreamTool::convertPixelClusterFooter(const unsigned long int& fragment, HTTEventInfo& eventfooter)  {

  (void) eventfooter;

  ATH_MSG_DEBUG("----------------------------------------------------------------");
  ATH_MSG_DEBUG("--- Executing convertPixelClusterFooter() ");
  ATH_MSG_DEBUG("--- from word 1/1 of Pixel Cluster Footer fragment to HTTEventInfo");
  ATH_MSG_DEBUG("----------------------------------------------------------------");

  PIXEL_CL_FTR pixelclusterfword;
  pixelclusterfword.fullword = fragment;
  ATH_MSG_DEBUG("---- word 1/1");
  ATH_MSG_DEBUG("   > FLAG: "  << std::hex << pixelclusterfword.fields.FLAG);
  ATH_MSG_DEBUG("   > COUNT: "  << std::hex << pixelclusterfword.fields.COUNT);
  ATH_MSG_DEBUG("   > ERROR: "  << std::hex << pixelclusterfword.fields.ERROR);
  ATH_MSG_DEBUG("   > SPARE: "  << std::hex << pixelclusterfword.fields.SPARE);

  ATH_MSG_DEBUG("---- Done!");
  ATH_MSG_DEBUG("----------------------------------------------------------------");

  return StatusCode::SUCCESS;

}

// Conversion from HTT Event to fragment - Pixel Cluster Footer

StatusCode TrigHTTTPByteStreamTool::convertPixelClusterFooter(const HTTEventInfo& eventfooter, unsigned long int& fragment)  {

  (void) eventfooter;

  ATH_MSG_DEBUG("----------------------------------------------------------------");
  ATH_MSG_DEBUG("--- Executing convertPixelClusterFooter() ");
  ATH_MSG_DEBUG("--- from HTTCluster to word 1/1 of Pixel Cluster Footer fragment");
  ATH_MSG_DEBUG("----------------------------------------------------------------");

  PIXEL_CL_FTR pixelclusterfword;
  GenMetaData consts;
  ATH_MSG_DEBUG("---- word 1/1");
  pixelclusterfword.fields.FLAG = consts.FLAG_CL_FTR; //hardcoded
  ATH_MSG_DEBUG("   > FLAG: "  << std::hex << pixelclusterfword.fields.FLAG);
  pixelclusterfword.fields.COUNT = 2;
  ATH_MSG_DEBUG("   > COUNT: "  << std::hex << pixelclusterfword.fields.COUNT);
  pixelclusterfword.fields.ERROR = 0x0; //noerror = 0x0, front_end_error = 0x1. parse_error =0x2, unused = 0x3
  ATH_MSG_DEBUG("   > ERROR: "  << std::hex << pixelclusterfword.fields.ERROR);
  pixelclusterfword.fields.SPARE = 0;
  ATH_MSG_DEBUG("   > SPARE: "  << std::hex << pixelclusterfword.fields.SPARE );

  fragment = pixelclusterfword.fullword;

  ATH_MSG_DEBUG("---- Done!");
  ATH_MSG_DEBUG("----------------------------------------------------------------");

  return StatusCode::SUCCESS;

}

// Conversion from fragment to HTT Cluster - Strip Cluster Module Data
StatusCode TrigHTTTPByteStreamTool::convertStripClusterModuleData(const unsigned long int& fragment, HTTHit& hit)  {

  ATH_MSG_DEBUG("----------------------------------------------------------------");
  ATH_MSG_DEBUG("--- Executing convertStripClusterModuleData() ");
  ATH_MSG_DEBUG("--- from word 1/1 of Strip Cluster Module Block fragment to HTTCluster");
  ATH_MSG_DEBUG("----------------------------------------------------------------");

  STRIP_CLUSTER stripclusterword;
  stripclusterword.fullword = fragment;
  ATH_MSG_DEBUG("---- word 1/1");

  hit.setEtaCoord(stripclusterword.fields.ROW);
  ATH_MSG_DEBUG("   > Filled hit.setEtaCoord(raw): "  << std::hex << hit.getEtaCoord());
  hit.setPhiCoord(stripclusterword.fields.IDX);
  ATH_MSG_DEBUG("   > Filled hit.setPhiCoord(idx): "  << std::hex << hit.getPhiCoord());
  hit.setEtaWidth(stripclusterword.fields.LEN);
  ATH_MSG_DEBUG("   > Filled hit.setEtaWidth(): "    << std::hex << hit.getEtaWidth());
  ATH_MSG_DEBUG("   > LAST: "    << std::hex << stripclusterword.fields.LAST);


  ATH_MSG_DEBUG("---- Done!");
  ATH_MSG_DEBUG("----------------------------------------------------------------");

  return StatusCode::SUCCESS;
}

// Conversion from HTT Cluster to fragment - Strip Cluster Module Data
StatusCode TrigHTTTPByteStreamTool::convertStripClusterModuleData(const HTTHit& hit, unsigned long int& fragment)  {

  ATH_MSG_DEBUG("----------------------------------------------------------------");
  ATH_MSG_DEBUG("--- Executing convertStripClusterModuleData() ");
  ATH_MSG_DEBUG("--- from word 1/1 of Strip Cluster Module Block to HTTCluster fragment");
  ATH_MSG_DEBUG("----------------------------------------------------------------");


  STRIP_CLUSTER stripclusterword;
  ATH_MSG_DEBUG("---- word 1/1");
  stripclusterword.fields.ROW = hit.getPhiCoord();//phi
  ATH_MSG_DEBUG("   > Filled ROW: "  << std::hex << stripclusterword.fields.ROW << " - from hit.getPhiCoord()");
  stripclusterword.fields.IDX = hit.getEtaCoord();//eta
  ATH_MSG_DEBUG("   > Filled IDX: "  << std::hex << stripclusterword.fields.IDX << " - from hit.getEtaCoord()");
  stripclusterword.fields.LEN = hit.getEtaWidth();
  ATH_MSG_DEBUG("   > Filled LEN: "  << std::hex << stripclusterword.fields.LEN << " - from hit.getEtaWidth()");
  stripclusterword.fields.LAST = 1; //1 if last cluster in module, zero otherwise
  ATH_MSG_DEBUG("   > LAST: "  << std::hex << stripclusterword.fields.LAST << " - NEED TO FIX LAST");

  fragment=stripclusterword.fullword;

  ATH_MSG_DEBUG("---- Done!");
  ATH_MSG_DEBUG("----------------------------------------------------------------");

  return StatusCode::SUCCESS;
}


////// Strip Cluster Footer
// Conversion from fragment to HTT Cluster - Strip Cluster Footer
StatusCode TrigHTTTPByteStreamTool::convertStripClusterFooter(const unsigned long int& fragment, HTTEventInfo& eventfooter)  {

  (void) eventfooter;

  ATH_MSG_DEBUG("----------------------------------------------------------------");
  ATH_MSG_DEBUG("--- Executing convertStripClusterFooter() ");
  ATH_MSG_DEBUG("--- from word 1/1 of Strip Cluster Footer fragment to HTTEventInfo");
  ATH_MSG_DEBUG("----------------------------------------------------------------");

  STRIP_CL_FTR stripclusterfword;
  stripclusterfword.fullword = fragment;
  ATH_MSG_DEBUG("---- word 1/1");
  ATH_MSG_DEBUG("   > FLAG: "  << std::hex << stripclusterfword.fields.FLAG);
  ATH_MSG_DEBUG("   > COUNT: "  << std::hex << stripclusterfword.fields.COUNT);
  ATH_MSG_DEBUG("   > ERROR: "  << std::hex << stripclusterfword.fields.ERROR);

  ATH_MSG_DEBUG("---- Done!");
  ATH_MSG_DEBUG("----------------------------------------------------------------");

  return StatusCode::SUCCESS;

}

// Conversion from HTT Event to fragment - Strip Cluster Footer

StatusCode TrigHTTTPByteStreamTool::convertStripClusterFooter(const HTTEventInfo& eventfooter, unsigned long int& fragment)  {

  (void) eventfooter;

  ATH_MSG_DEBUG("----------------------------------------------------------------");
  ATH_MSG_DEBUG("--- Executing convertStripClusterFooter() ");
  ATH_MSG_DEBUG("--- from HTTCluster to word 1/1 of Strip Cluster Footer fragment");
  ATH_MSG_DEBUG("----------------------------------------------------------------");

  STRIP_CL_FTR stripclusterfword;
  GenMetaData consts;
  ATH_MSG_DEBUG("---- word 1/1");
  stripclusterfword.fields.FLAG = consts.FLAG_CL_FTR;
  ATH_MSG_DEBUG("   > FLAG: "  << std::hex << stripclusterfword.fields.FLAG << " - Not present in HTTEventInfo");
  stripclusterfword.fields.COUNT = 2;
  ATH_MSG_DEBUG("   > COUNT: "  << std::hex << stripclusterfword.fields.COUNT << " - Not present in HTTEventInfo");
  stripclusterfword.fields.ERROR = 0x0; //no error = 0x0, front end = 0x1, parse = 0x2, unused = 0x3
  ATH_MSG_DEBUG("   > ERROR: "  << std::hex << stripclusterfword.fields.ERROR << " - Not present in HTTEventInfo");

  fragment = stripclusterfword.fullword;

  ATH_MSG_DEBUG("---- Done!");
  ATH_MSG_DEBUG("----------------------------------------------------------------");

  return StatusCode::SUCCESS;

}



// Conversion from Track Block Data to HTT Track
StatusCode TrigHTTTPByteStreamTool::convertTrackBlockData(const std::vector<unsigned long int>& fragment, HTTTrack& track, bool& WITHSSID)  {

  ATH_MSG_DEBUG("----------------------------------------------------------------");
  ATH_MSG_DEBUG("--- Executing convertTrackBlockData() ");
  ATH_MSG_DEBUG("--- from 4/4 words Track Block Data fragment to HTTTrack");
  ATH_MSG_DEBUG("----------------------------------------------------------------");

  // Scale factors declared in include

  TRACK_W1 trackword1;
  trackword1.fullword = fragment[0];
  ATH_MSG_DEBUG("---- word 1/4");
  // All of this should change when Track has the things we want.
  ATH_MSG_DEBUG("   > FLAG: "  << std::hex << trackword1.fields.FLAG);
  track.setTrackStage(TrackStage(trackword1.fields.TYPE));
  ATH_MSG_DEBUG("   > TYPE: "  << std::hex << trackword1.fields.TYPE);
  WITHSSID = trackword1.fields.WITHSSID;
  ATH_MSG_DEBUG("   > WITHSSID: "  << std::hex << trackword1.fields.WITHSSID);
  ATH_MSG_DEBUG("   > SPARE: "  << std::hex << trackword1.fields.SPARE);

  TRACK_W2 trackword2;
  trackword2.fullword = fragment[1];
  ATH_MSG_DEBUG("---- word 2/4");
  ATH_MSG_DEBUG("   > NOTHING TO DO with DETMAP: "  << std::hex << trackword2.fields.DETMAP);
  track.setHitMap(trackword2.fields.HITMAP);
  ATH_MSG_DEBUG("   > Filled setHitMap(): "  << std::hex << trackword2.fields.HITMAP);
  track.setPatternID(trackword2.fields.ROADID);
  ATH_MSG_DEBUG("   > Filled ROADID: "  << std::hex << trackword2.fields.ROADID);
  ATH_MSG_DEBUG("   > NOTHING TO DO with PRM"  << std::hex << trackword2.fields.PRM);
  ATH_MSG_DEBUG("   > SPARE: "  << std::hex << trackword2.fields.SPARE);

  TRACK_W3 trackword3;
  trackword3.fullword = fragment[2];
  ATH_MSG_DEBUG("---- word 3/4");
  track.setFirstSectorID(trackword3.fields.STAGE1_SECTOR);
  ATH_MSG_DEBUG("   > Filled setSectorID(): "  << std::hex << track.getFirstSectorID());
  track.setSecondSectorID(trackword3.fields.STAGE2_SECTOR);
  ATH_MSG_DEBUG("   > Filled setSectorID(): "  << std::hex << trackword3.fields.STAGE2_SECTOR);
  track.setEta(trackword3.fields.ETA/sf_eta);
  ATH_MSG_DEBUG("   > Filled setEta(): "  << std::hex << track.getEta());
  track.setPhi(trackword3.fields.PHI/sf_phi);
  ATH_MSG_DEBUG("   > Filled setPhi(): "  << std::hex << track.getPhi());

  TRACK_W4 trackword4;
  trackword4.fullword = fragment[3];
  ATH_MSG_DEBUG("---- word 4/4");
  track.setQOverPt(trackword4.fields.QoverPt/sf_curv);
  ATH_MSG_DEBUG("   > Filled setQOverPt(): " << std::hex  << track.getQOverPt());
  track.setZ0(trackword4.fields.Z0/sf_z0);
  ATH_MSG_DEBUG("   > Filled setZ0(): "  << std::hex << track.getZ0());
  track.setD0(trackword4.fields.D0/sf_d0);
  ATH_MSG_DEBUG("   > Filled setD0(): "  << std::hex << track.getD0());
  track.setChi2(trackword4.fields.CHI2/sf_chi2);
  ATH_MSG_DEBUG("   > Filled setChi2(): "  << std::hex << track.getChi2());

  ATH_MSG_DEBUG("---- Done!");
  ATH_MSG_DEBUG("----------------------------------------------------------------");

  return StatusCode::SUCCESS;
}

// Conversion from HTT Track to Track Block Data
StatusCode TrigHTTTPByteStreamTool::convertTrackBlockData( const HTTTrack& track , const bool& WITHSSID, std::vector<unsigned long int>& fragment) {

  ATH_MSG_DEBUG("----------------------------------------------------------------");
  ATH_MSG_DEBUG("--- Executing convertTrackBlockData() ");
  ATH_MSG_DEBUG("--- from HTT Track to 4/4 words of Track Block Data fragment");
  ATH_MSG_DEBUG("----------------------------------------------------------------");

  //reset
  fragment.reserve(4);
  fragment.clear();

  // Scale factors declared in include
  TRACK_W1 trackword1;
  GenMetaData consts;

  //Building detmap for the track
  unsigned int detMap=0;
  ATH_CHECK(createTrackDetMap(track, detMap));

  ATH_MSG_DEBUG("---- word 1/4");
  trackword1.fields.FLAG = consts.FLAG_TRK_HDR;
  ATH_MSG_DEBUG("   > FLAG: "  << std::hex << trackword1.fields.FLAG << " - Not present in HTTTrack");
  trackword1.fields.WITHSSID = WITHSSID;
  ATH_MSG_DEBUG("   > WITHSSID: "  << std::hex << trackword1.fields.WITHSSID);
  trackword1.fields.TYPE = (unsigned int)(track.getTrackStage()); // 0x1 first stage, 0x2 second stage 
  ATH_MSG_DEBUG("   > Filled TYPE: "  << std::hex << trackword1.fields.TYPE);
  trackword1.fields.SPARE = 42;  //hardcoded
  ATH_MSG_DEBUG("   > Filled SPARE: "  << std::hex << trackword1.fields.SPARE << " - Not present in HTTTrack");

  TRACK_W2 trackword2;
  ATH_MSG_DEBUG("---- word 2/4");
  trackword2.fields.DETMAP = detMap; 
  ATH_MSG_DEBUG("   > Filled DETMAP: "  << std::hex << trackword2.fields.DETMAP << " - Built in the ByteStreamTool");
  trackword2.fields.HITMAP = track.getHitMap();
  ATH_MSG_DEBUG("   > Filled HITMAP: "  << std::hex << trackword2.fields.HITMAP);
  trackword2.fields.ROADID = track.getPatternID();
  ATH_MSG_DEBUG("   > Filled ROADID: "  << std::hex << trackword2.fields.ROADID);
  trackword2.fields.PRM = 88; //hardcoded
  ATH_MSG_DEBUG("   > Filled PRM: "  << std::hex << trackword2.fields.PRM << " -  Not present in HTTTrack");
  trackword2.fields.SPARE = 4;  //hardcoded
  ATH_MSG_DEBUG("   > Filled SPARE: "  << std::hex << trackword2.fields.SPARE << " - Not present in HTTTrack");

  TRACK_W3 trackword3;
  ATH_MSG_DEBUG("---- word 3/4");
  trackword3.fields.STAGE1_SECTOR = track.getFirstSectorID();
  ATH_MSG_DEBUG("   > Filled STAGE1_SECTOR: "  << std::hex << trackword3.fields.STAGE1_SECTOR);
  trackword3.fields.STAGE2_SECTOR = track.getSecondSectorID();
  ATH_MSG_DEBUG("   > Filled STAGE2_SECTOR: "  << std::hex << trackword3.fields.STAGE2_SECTOR);
  trackword3.fields.ETA = track.getEta()*sf_eta;
  ATH_MSG_DEBUG("   > Filled ETA: "  << std::hex << trackword3.fields.ETA);
  trackword3.fields.PHI = track.getPhi()*sf_phi;
  ATH_MSG_DEBUG("   > Filled PHI: "  << std::hex << trackword3.fields.PHI );

  TRACK_W4 trackword4;
  ATH_MSG_DEBUG("---- word 4/4");
  trackword4.fields.QoverPt = track.getQOverPt()*sf_curv;
  ATH_MSG_DEBUG("   > Filled QoverPt: "  << std::hex << trackword4.fields.QoverPt);
  trackword4.fields.Z0 = track.getZ0()*sf_z0;
  ATH_MSG_DEBUG("   > Filled Z0: "  << std::hex << trackword4.fields.Z0);
  trackword4.fields.D0 = track.getD0()*sf_d0;
  ATH_MSG_DEBUG("   > Filled D0: "  << std::hex << trackword4.fields.D0);
  trackword4.fields.CHI2 = track.getChi2()*sf_chi2;
  ATH_MSG_DEBUG("   > Filled CHI2: "  << std::hex << trackword4.fields.CHI2);

  fragment[0]=trackword1.fullword;
  fragment[1]=trackword2.fullword;
  fragment[2]=trackword3.fullword;
  fragment[3]=trackword4.fullword;

  ATH_MSG_DEBUG("---- Done!");
  ATH_MSG_DEBUG("----------------------------------------------------------------");

  return StatusCode::SUCCESS;
}

// Conversion from fragment to HTT Cluster - Track Pixel Cluster
StatusCode TrigHTTTPByteStreamTool::convertTrackPixelCluster(const unsigned long int& fragment, HTTHit& hit)  {

  ATH_MSG_DEBUG("----------------------------------------------------------------");
  ATH_MSG_DEBUG("--- Executing convertTrackPixelCluster() ");
  ATH_MSG_DEBUG("--- from word 1/1 of Track Pixel Cluster fragment to HTTHit");
  ATH_MSG_DEBUG("----------------------------------------------------------------");

  TRACK_PIXEL_CLUSTER pixelclusterword;
  pixelclusterword.fullword = fragment;
  ATH_MSG_DEBUG("---- word 1/1");

  ATH_MSG_DEBUG("   > NOTHING TO DO with MODID: "    << std::hex << pixelclusterword.fields.MODID);
  ATH_MSG_DEBUG("   > NOTHING TO DO with RAW_INCLUDED: "    << std::hex << pixelclusterword.fields.RAW_INCLUDED);
  ATH_MSG_DEBUG("   > NOTHING TO DO with SPARE: "    << std::hex << pixelclusterword.fields.SPARE);
  hit.setEtaCoord(pixelclusterword.fields.ETA);
  ATH_MSG_DEBUG("   > Filled hit.setEtaCoord(eta): "  << std::hex << hit.getEtaCoord());
  hit.setPhiCoord(pixelclusterword.fields.PHI);
  ATH_MSG_DEBUG("   > Filled hit.setPhiCoord(phi): "  << std::hex << hit.getPhiCoord());
  hit.setEtaWidth(pixelclusterword.fields.ETASIZ);
  ATH_MSG_DEBUG("   > Filled hit.setEtaWidth(): "   << std::hex << hit.getEtaWidth());
  hit.setPhiWidth(pixelclusterword.fields.PHISIZ);
  ATH_MSG_DEBUG("   > Filled hit.setPhiWidth(): "   << std::hex << hit.getPhiWidth());
  ATH_MSG_DEBUG("   > NOTHING TO DO with LAST: "    << std::hex << pixelclusterword.fields.LAST);



  ATH_MSG_DEBUG("---- Done!");
  ATH_MSG_DEBUG("----------------------------------------------------------------");

  return StatusCode::SUCCESS;
}

// Conversion from HTT Cluster to fragment - Track Pixel Cluster

StatusCode TrigHTTTPByteStreamTool::convertTrackPixelCluster(const HTTHit& hit, unsigned long int& fragment)  {

  ATH_MSG_DEBUG("----------------------------------------------------------------");
  ATH_MSG_DEBUG("--- Executing convertTrackPixelCluster() ");
  ATH_MSG_DEBUG("--- from HTTHit to word 1/1 of Track Pixel Cluster fragment");
  ATH_MSG_DEBUG("----------------------------------------------------------------");


  TRACK_PIXEL_CLUSTER pixelclusterword;
  ATH_MSG_DEBUG("---- word 1/1");
  pixelclusterword.fields.MODID = 1;
  ATH_MSG_DEBUG("   > Filled MODID: "  << std::hex << pixelclusterword.fields.MODID << " - Not present in HTTHit");
  pixelclusterword.fields.RAW_INCLUDED = 0;
  ATH_MSG_DEBUG("   > Filled RAW_INCLUDED: "  << std::hex << pixelclusterword.fields.RAW_INCLUDED << " - Not present in HTTHit");
  pixelclusterword.fields.SPARE = 1;
  ATH_MSG_DEBUG("   > Filled SPARE: "  << std::hex << pixelclusterword.fields.SPARE << " - Not Present in HTTHit");
  pixelclusterword.fields.ETA = hit.getEtaCoord();
  ATH_MSG_DEBUG("   > Filled ETA: "  << std::hex << pixelclusterword.fields.ETA << " - From hit.getEtaCoord()");
  pixelclusterword.fields.PHI = hit.getPhiCoord();
  ATH_MSG_DEBUG("   > Filled PHI: " << std::hex  << pixelclusterword.fields.PHI << " - From hit.getPhiCoord()");
  pixelclusterword.fields.ETASIZ = hit.getEtaWidth();
  ATH_MSG_DEBUG("   > Filled ETASIZ "  << std::hex << pixelclusterword.fields.ETASIZ << " - From hit.getEtaWidth()");
  pixelclusterword.fields.PHISIZ = hit.getPhiWidth();
  ATH_MSG_DEBUG("   > Filled PHISIZ: "  << std::hex << pixelclusterword.fields.PHISIZ << " - From hit.getPhiWidth()");
  pixelclusterword.fields.LAST = 1; //1 if last cluster in module, zero otherwise
  ATH_MSG_DEBUG("   > Filled LAST: " << pixelclusterword.fields.LAST << " - NEED TO FIX LAST");

  fragment = pixelclusterword.fullword;

  ATH_MSG_DEBUG("---- Done!");
  ATH_MSG_DEBUG("----------------------------------------------------------------");

  return StatusCode::SUCCESS;
}

// Conversion from fragment to HTT Cluster - Track Strip Cluster
StatusCode TrigHTTTPByteStreamTool::convertTrackStripCluster(const unsigned long int& fragment, HTTHit& hit)  {

  ATH_MSG_DEBUG("----------------------------------------------------------------");
  ATH_MSG_DEBUG("--- Executing convertTrackStripCluster() ");
  ATH_MSG_DEBUG("--- from word 1/1 of Track Strip Cluster fragment to HTTHit");
  ATH_MSG_DEBUG("----------------------------------------------------------------");

  TRACK_STRIP_CLUSTER stripclusterword;
  stripclusterword.fullword = fragment;
  ATH_MSG_DEBUG("---- word 1/1");
  ATH_MSG_DEBUG("   > NOTHING TO DO with MODID: "    << std::hex << stripclusterword.fields.MODID);
  hit.setEtaCoord(stripclusterword.fields.ROW);
  ATH_MSG_DEBUG("   > Filled hit.setPhiCoord(row): "  << std::hex << hit.getPhiCoord());
  hit.setPhiCoord(stripclusterword.fields.IDX);
  ATH_MSG_DEBUG("   > Filled hit.setPhiCoord(idx): "  << std::hex << hit.getEtaCoord());
  hit.setEtaWidth(stripclusterword.fields.LEN);
  ATH_MSG_DEBUG("   > Filled hit.setEtaWidth(len): "   << std::hex  << hit.getEtaWidth());
  ATH_MSG_DEBUG("   > NOTHING TO DO with LAST: "    << std::hex << stripclusterword.fields.LAST);


  ATH_MSG_DEBUG("---- Done!");
  ATH_MSG_DEBUG("----------------------------------------------------------------");

  return StatusCode::SUCCESS;
}

// Conversion from HTT Cluster to fragment - Track Strip Cluster
StatusCode TrigHTTTPByteStreamTool::convertTrackStripCluster(const HTTHit& hit, unsigned long int& fragment)  {

  ATH_MSG_DEBUG("----------------------------------------------------------------");
  ATH_MSG_DEBUG("--- Executing convertTrackStripCluster() ");
  ATH_MSG_DEBUG("--- from word 1/1 of Track Strip Cluster to HTTHit fragment");
  ATH_MSG_DEBUG("----------------------------------------------------------------");


  TRACK_STRIP_CLUSTER stripclusterword;
  ATH_MSG_DEBUG("---- word 1/1");
  ATH_MSG_DEBUG("   > NOTHING TO DO with MODID: "   << std::hex  << stripclusterword.fields.MODID);
  stripclusterword.fields.ROW = hit.getPhiCoord();
  ATH_MSG_DEBUG("   > Filled ROW: "  << std::hex << stripclusterword.fields.ROW << " - from hit.getPhiCoord()");
  stripclusterword.fields.IDX = hit.getEtaCoord();
  ATH_MSG_DEBUG("   > Filled IDX: " << std::hex  << stripclusterword.fields.IDX << " - from hit.getEtaCoord()");
  stripclusterword.fields.LEN = hit.getEtaWidth();
  ATH_MSG_DEBUG("   > Filled LEN: " << std::hex  << stripclusterword.fields.LEN << " - from hit.getEtaWidth()");
  stripclusterword.fields.LAST = 1; //1 if last cluster in module, zero otherwise
  ATH_MSG_DEBUG("   > Filled LAST: "  << std::hex << stripclusterword.fields.LAST << " - NEED TO FIX LAST");

  fragment=stripclusterword.fullword;
  ATH_MSG_DEBUG("---- Done!");
  ATH_MSG_DEBUG("----------------------------------------------------------------");

  return StatusCode::SUCCESS;
}

// Conversion from SSID fragment to HTT Track SSID information
StatusCode TrigHTTTPByteStreamTool::convertSSIDData(const unsigned long int& fragment, HTTHit& hit, HTT_SSID& ssid)  {

  ATH_MSG_DEBUG("----------------------------------------------------------------");
  ATH_MSG_DEBUG("--- Executing convertSSIDData() ");
  ATH_MSG_DEBUG("--- from 1/1 word SSID Block Data fragment to ssid info");
  ATH_MSG_DEBUG("----------------------------------------------------------------");

  SSID ssidword1;
  ssidword1.fullword = fragment;
  ATH_MSG_DEBUG("---- word 1/1");

  hit.setLayer(ssidword1.fields.LYR);
  ATH_MSG_DEBUG("   > Filled hit.setLayer(): "  << std::hex << hit.getLayer());
  ssid.setModule(ssidword1.fields.MODID);
  ATH_MSG_DEBUG("   > Filled MODID: "  << std::hex << ssidword1.fields.MODID);
  if (hit.isPixel()){
    ssid.setPixelX(ssidword1.fields.PHI);
  }
  else {
    ssid.setStripX(ssidword1.fields.PHI);
  }
  ATH_MSG_DEBUG("   > Filled PHI: "  << std::hex << ssidword1.fields.PHI);

  ATH_MSG_DEBUG("---- Done!");
  ATH_MSG_DEBUG("----------------------------------------------------------------");

  return StatusCode::SUCCESS;
}

// Conversion from HTT Track SSID information to SSID fragment
StatusCode TrigHTTTPByteStreamTool::convertSSIDData(const HTTHit& hit, const HTT_SSID& ssid, unsigned long int& fragment) {

  ATH_MSG_DEBUG("----------------------------------------------------------------");
  ATH_MSG_DEBUG("--- Executing convertSSIDData() ");
  ATH_MSG_DEBUG("--- from SSID Info to 1/1 words of SSID Block Data fragment");
  ATH_MSG_DEBUG("----------------------------------------------------------------");

  uint32_t moduleID = ssid.getModule();  // add this information in the DUMMY DATA      
  uint32_t x = 0;
  if (hit.isPixel()){
    x = ssid.getPixelX();
  }
  else {
    x = ssid.getStripX();
  }

  SSID ssidword1;

  ATH_MSG_DEBUG("---- word 1/1");
  ssidword1.fields.LYR = hit.getLayer(); //CHECK WHAT TO INITIALISE HERE
  ATH_MSG_DEBUG("   > Filled LYR: "  << std::hex << ssidword1.fields.LYR); // << " - Not present in HTTTrack");
  ssidword1.fields.MODID = moduleID; 
  ATH_MSG_DEBUG("   > Filled MODID: "  << std::hex << ssidword1.fields.MODID);
  ssidword1.fields.PHI = x; 
  ATH_MSG_DEBUG("   > Filled PHI: "  << std::hex << ssidword1.fields.PHI);

  ssidword1.fullword = fragment;

  ATH_MSG_DEBUG("---- Done!");
  ATH_MSG_DEBUG("----------------------------------------------------------------");

  return StatusCode::SUCCESS;
}
