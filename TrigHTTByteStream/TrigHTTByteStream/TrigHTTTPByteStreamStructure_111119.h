//  File https://gitlab.cern.ch/atlas-tdaq-ph2upgrades/atlas-tdaq-htt/tdaq-htt-firmware/system-simulation/blob/master/DataFormat/DataFormat.C
// From Elliot
// November 11 2019

//--------------------------------------------------------------------------------
// EVT_HDR_W1 word description
//--------------------------------------------------------------------------------
union EVT_HDR_W1 {
  struct  { 
    unsigned long int L0ID : 40;
    unsigned long int SPARE : 8;
    unsigned long int TRK_TYPE : 8;
    unsigned long int FLAG : 8;
  } fields;

  // access full word
  unsigned long int fullword;

  // bit descriptors
  const static unsigned long int FLAG_bits = 8;
  const static unsigned long int FLAG_firstbit = 63;
  const static unsigned long int FLAG_lastbit = 56;
  const static unsigned long int FLAG_FLAG = 0xab;
  void setFLAG(unsigned long int val) {
      if (val > (1UL<<FLAG_bits)){ throw("EVT_HDR_W1 setFLAG value out of range");}
      fields.FLAG = val;
  }
  unsigned long int getFLAG() {
      return fields.FLAG;
  }

  const static unsigned long int TRK_TYPE_bits = 8;
  const static unsigned long int TRK_TYPE_firstbit = 55;
  const static unsigned long int TRK_TYPE_lastbit = 48;
  const static unsigned long int TRK_TYPE_RHTT = 0x1;
  const static unsigned long int TRK_TYPE_GHTT = 0x2;
  void setTRK_TYPE(unsigned long int val) {
      if (val > (1UL<<TRK_TYPE_bits)){ throw("EVT_HDR_W1 setTRK_TYPE value out of range");}
      fields.TRK_TYPE = val;
  }
  unsigned long int getTRK_TYPE() {
      return fields.TRK_TYPE;
  }

  const static unsigned long int SPARE_bits = 8;
  const static unsigned long int SPARE_firstbit = 47;
  const static unsigned long int SPARE_lastbit = 40;
  void setSPARE(unsigned long int val) {
      if (val > (1UL<<SPARE_bits)){ throw("EVT_HDR_W1 setSPARE value out of range");}
      fields.SPARE = val;
  }
  unsigned long int getSPARE() {
      return fields.SPARE;
  }

  const static unsigned long int L0ID_bits = 40;
  const static unsigned long int L0ID_firstbit = 39;
  const static unsigned long int L0ID_lastbit = 0;
  void setL0ID(unsigned long int val) {
      if (val > (1UL<<L0ID_bits)){ throw("EVT_HDR_W1 setL0ID value out of range");}
      fields.L0ID = val;
  }
  unsigned long int getL0ID() {
      return fields.L0ID;
  }

};
//--------------------------------------------------------------------------------
// EVT_HDR_W2 word description
//--------------------------------------------------------------------------------
union EVT_HDR_W2 {
  struct  { 
    unsigned long int RUNNUMBER : 32;
    unsigned long int SPARE : 20;
    unsigned long int BCID : 12;
  } fields;

  // access full word
  unsigned long int fullword;

  // bit descriptors
  const static unsigned long int BCID_bits = 12;
  const static unsigned long int BCID_firstbit = 63;
  const static unsigned long int BCID_lastbit = 52;
  void setBCID(unsigned long int val) {
      if (val > (1UL<<BCID_bits)){ throw("EVT_HDR_W2 setBCID value out of range");}
      fields.BCID = val;
  }
  unsigned long int getBCID() {
      return fields.BCID;
  }

  const static unsigned long int SPARE_bits = 20;
  const static unsigned long int SPARE_firstbit = 51;
  const static unsigned long int SPARE_lastbit = 32;
  void setSPARE(unsigned long int val) {
      if (val > (1UL<<SPARE_bits)){ throw("EVT_HDR_W2 setSPARE value out of range");}
      fields.SPARE = val;
  }
  unsigned long int getSPARE() {
      return fields.SPARE;
  }

  const static unsigned long int RUNNUMBER_bits = 32;
  const static unsigned long int RUNNUMBER_firstbit = 31;
  const static unsigned long int RUNNUMBER_lastbit = 0;
  void setRUNNUMBER(unsigned long int val) {
      if (val > (1UL<<RUNNUMBER_bits)){ throw("EVT_HDR_W2 setRUNNUMBER value out of range");}
      fields.RUNNUMBER = val;
  }
  unsigned long int getRUNNUMBER() {
      return fields.RUNNUMBER;
  }

};
//--------------------------------------------------------------------------------
// EVT_HDR_W3 word description
//--------------------------------------------------------------------------------
union EVT_HDR_W3 {
  struct  { 
    unsigned long int ROI : 64;
  } fields;

  // access full word
  unsigned long int fullword;

  // bit descriptors
  const static unsigned long int ROI_bits = 64;
  const static unsigned long int ROI_firstbit = 63;
  const static unsigned long int ROI_lastbit = 0;
  void setROI(unsigned long int val) {
      fields.ROI = val;
  }
  unsigned long int getROI() {
      return fields.ROI;
  }

};
//--------------------------------------------------------------------------------
// EVT_HDR_W4 word description
//--------------------------------------------------------------------------------
union EVT_HDR_W4 {
  struct  { 
    unsigned long int TIME : 32;
    unsigned long int EFPU_PID : 12;
    unsigned long int EFPU_ID : 20;
  } fields;

  // access full word
  unsigned long int fullword;

  // bit descriptors
  const static unsigned long int EFPU_ID_bits = 20;
  const static unsigned long int EFPU_ID_firstbit = 63;
  const static unsigned long int EFPU_ID_lastbit = 44;
  void setEFPU_ID(unsigned long int val) {
      if (val > (1UL<<EFPU_ID_bits)){ throw("EVT_HDR_W4 setEFPU_ID value out of range");}
      fields.EFPU_ID = val;
  }
  unsigned long int getEFPU_ID() {
      return fields.EFPU_ID;
  }

  const static unsigned long int EFPU_PID_bits = 12;
  const static unsigned long int EFPU_PID_firstbit = 43;
  const static unsigned long int EFPU_PID_lastbit = 32;
  void setEFPU_PID(unsigned long int val) {
      if (val > (1UL<<EFPU_PID_bits)){ throw("EVT_HDR_W4 setEFPU_PID value out of range");}
      fields.EFPU_PID = val;
  }
  unsigned long int getEFPU_PID() {
      return fields.EFPU_PID;
  }

  const static unsigned long int TIME_bits = 32;
  const static unsigned long int TIME_firstbit = 31;
  const static unsigned long int TIME_lastbit = 0;
  void setTIME(unsigned long int val) {
      if (val > (1UL<<TIME_bits)){ throw("EVT_HDR_W4 setTIME value out of range");}
      fields.TIME = val;
  }
  unsigned long int getTIME() {
      return fields.TIME;
  }

};
//--------------------------------------------------------------------------------
// EVT_HDR_W5 word description
//--------------------------------------------------------------------------------
union EVT_HDR_W5 {
  struct  { 
    unsigned long int Transaction_ID : 32;
    unsigned long int Connection_ID : 32;
  } fields;

  // access full word
  unsigned long int fullword;

  // bit descriptors
  const static unsigned long int Connection_ID_bits = 32;
  const static unsigned long int Connection_ID_firstbit = 63;
  const static unsigned long int Connection_ID_lastbit = 32;
  void setConnection_ID(unsigned long int val) {
      if (val > (1UL<<Connection_ID_bits)){ throw("EVT_HDR_W5 setConnection_ID value out of range");}
      fields.Connection_ID = val;
  }
  unsigned long int getConnection_ID() {
      return fields.Connection_ID;
  }

  const static unsigned long int Transaction_ID_bits = 32;
  const static unsigned long int Transaction_ID_firstbit = 31;
  const static unsigned long int Transaction_ID_lastbit = 0;
  void setTransaction_ID(unsigned long int val) {
      if (val > (1UL<<Transaction_ID_bits)){ throw("EVT_HDR_W5 setTransaction_ID value out of range");}
      fields.Transaction_ID = val;
  }
  unsigned long int getTransaction_ID() {
      return fields.Transaction_ID;
  }

};
//--------------------------------------------------------------------------------
// EVT_HDR_W6 word description
//--------------------------------------------------------------------------------
union EVT_HDR_W6 {
  struct  { 
    unsigned long int CRC : 32;
    unsigned long int STATUS : 32;
  } fields;

  // access full word
  unsigned long int fullword;

  // bit descriptors
  const static unsigned long int STATUS_bits = 32;
  const static unsigned long int STATUS_firstbit = 63;
  const static unsigned long int STATUS_lastbit = 32;
  void setSTATUS(unsigned long int val) {
      if (val > (1UL<<STATUS_bits)){ throw("EVT_HDR_W6 setSTATUS value out of range");}
      fields.STATUS = val;
  }
  unsigned long int getSTATUS() {
      return fields.STATUS;
  }

  const static unsigned long int CRC_bits = 32;
  const static unsigned long int CRC_firstbit = 31;
  const static unsigned long int CRC_lastbit = 0;
  void setCRC(unsigned long int val) {
      if (val > (1UL<<CRC_bits)){ throw("EVT_HDR_W6 setCRC value out of range");}
      fields.CRC = val;
  }
  unsigned long int getCRC() {
      return fields.CRC;
  }

};
//--------------------------------------------------------------------------------
// EVT_FTR_W1 word description
//--------------------------------------------------------------------------------
union EVT_FTR_W1 {
  struct  { 
    unsigned long int HDR_CRC : 32;
    unsigned long int META_COUNT : 16;
    unsigned long int SPARE : 8;
    unsigned long int FLAG : 8;
  } fields;

  // access full word
  unsigned long int fullword;

  // bit descriptors
  const static unsigned long int FLAG_bits = 8;
  const static unsigned long int FLAG_firstbit = 63;
  const static unsigned long int FLAG_lastbit = 56;
  void setFLAG(unsigned long int val) {
      if (val > (1UL<<FLAG_bits)){ throw("EVT_FTR_W1 setFLAG value out of range");}
      fields.FLAG = val;
  }
  unsigned long int getFLAG() {
      return fields.FLAG;
  }

  const static unsigned long int SPARE_bits = 8;
  const static unsigned long int SPARE_firstbit = 55;
  const static unsigned long int SPARE_lastbit = 48;
  void setSPARE(unsigned long int val) {
      if (val > (1UL<<SPARE_bits)){ throw("EVT_FTR_W1 setSPARE value out of range");}
      fields.SPARE = val;
  }
  unsigned long int getSPARE() {
      return fields.SPARE;
  }

  const static unsigned long int META_COUNT_bits = 16;
  const static unsigned long int META_COUNT_firstbit = 47;
  const static unsigned long int META_COUNT_lastbit = 32;
  void setMETA_COUNT(unsigned long int val) {
      if (val > (1UL<<META_COUNT_bits)){ throw("EVT_FTR_W1 setMETA_COUNT value out of range");}
      fields.META_COUNT = val;
  }
  unsigned long int getMETA_COUNT() {
      return fields.META_COUNT;
  }

  const static unsigned long int HDR_CRC_bits = 32;
  const static unsigned long int HDR_CRC_firstbit = 31;
  const static unsigned long int HDR_CRC_lastbit = 0;
  void setHDR_CRC(unsigned long int val) {
      if (val > (1UL<<HDR_CRC_bits)){ throw("EVT_FTR_W1 setHDR_CRC value out of range");}
      fields.HDR_CRC = val;
  }
  unsigned long int getHDR_CRC() {
      return fields.HDR_CRC;
  }

};
//--------------------------------------------------------------------------------
// EVT_FTR_W2 word description
//--------------------------------------------------------------------------------
union EVT_FTR_W2 {
  struct  { 
    unsigned long int ERROR_FLAGS : 64;
  } fields;

  // access full word
  unsigned long int fullword;

  // bit descriptors
  const static unsigned long int ERROR_FLAGS_bits = 64;
  const static unsigned long int ERROR_FLAGS_firstbit = 63;
  const static unsigned long int ERROR_FLAGS_lastbit = 0;
  void setERROR_FLAGS(unsigned long int val) {
      fields.ERROR_FLAGS = val;
  }
  unsigned long int getERROR_FLAGS() {
      return fields.ERROR_FLAGS;
  }

};
//--------------------------------------------------------------------------------
// EVT_FTR_W3 word description
//--------------------------------------------------------------------------------
union EVT_FTR_W3 {
  struct  { 
    unsigned long int CRC : 32;
    unsigned long int WORD_COUNT : 32;
  } fields;

  // access full word
  unsigned long int fullword;

  // bit descriptors
  const static unsigned long int WORD_COUNT_bits = 32;
  const static unsigned long int WORD_COUNT_firstbit = 63;
  const static unsigned long int WORD_COUNT_lastbit = 32;
  void setWORD_COUNT(unsigned long int val) {
      if (val > (1UL<<WORD_COUNT_bits)){ throw("EVT_FTR_W3 setWORD_COUNT value out of range");}
      fields.WORD_COUNT = val;
  }
  unsigned long int getWORD_COUNT() {
      return fields.WORD_COUNT;
  }

  const static unsigned long int CRC_bits = 32;
  const static unsigned long int CRC_firstbit = 31;
  const static unsigned long int CRC_lastbit = 0;
  void setCRC(unsigned long int val) {
      if (val > (1UL<<CRC_bits)){ throw("EVT_FTR_W3 setCRC value out of range");}
      fields.CRC = val;
  }
  unsigned long int getCRC() {
      return fields.CRC;
  }

};
//--------------------------------------------------------------------------------
// M_HDR word description
//--------------------------------------------------------------------------------
union M_HDR {
  struct  { 
    unsigned long int ROUTING : 52;
    unsigned long int DET : 1;
    unsigned long int TYPE : 2;
    unsigned long int FLAG : 8;
  } fields;

  // access full word
  unsigned long int fullword;

  // bit descriptors
  const static unsigned long int FLAG_bits = 8;
  const static unsigned long int FLAG_firstbit = 63;
  const static unsigned long int FLAG_lastbit = 56;
  const static unsigned long int FLAG_FLAG = 0x55;
  void setFLAG(unsigned long int val) {
      if (val > (1UL<<FLAG_bits)){ throw("M_HDR setFLAG value out of range");}
      fields.FLAG = val;
  }
  unsigned long int getFLAG() {
      return fields.FLAG;
  }

  const static unsigned long int TYPE_bits = 2;
  const static unsigned long int TYPE_firstbit = 55;
  const static unsigned long int TYPE_lastbit = 54;
  const static unsigned long int TYPE_RAW = 0x0;
  const static unsigned long int TYPE_CLUSTERED = 0x1;
  const static unsigned long int TYPE_CLUSTEREDwRAW = 0x2;
  const static unsigned long int TYPE_UNUSED = 0x3;
  void setTYPE(unsigned long int val) {
      if (val > (1UL<<TYPE_bits)){ throw("M_HDR setTYPE value out of range");}
      fields.TYPE = val;
  }
  unsigned long int getTYPE() {
      return fields.TYPE;
  }

  const static unsigned long int DET_bits = 1;
  const static unsigned long int DET_firstbit = 53;
  const static unsigned long int DET_lastbit = 53;
  const static unsigned long int DET_PIXEL = 0x0;
  const static unsigned long int DET_STRIP = 0x1;
  void setDET(unsigned long int val) {
      if (val > (1UL<<DET_bits)){ throw("M_HDR setDET value out of range");}
      fields.DET = val;
  }
  unsigned long int getDET() {
      return fields.DET;
  }

  const static unsigned long int ROUTING_bits = 52;
  const static unsigned long int ROUTING_firstbit = 52;
  const static unsigned long int ROUTING_lastbit = 1;
  const static unsigned long int ROUTING_NPRM = 0xc;
  const static unsigned long int ROUTING_NAMBUS = 0x4;
  const static unsigned long int ROUTING_NTFM_per_SSTP = 0x2;
  const static unsigned long int ROUTING_TFM_lastbit = 0x30;
  void setROUTING(unsigned long int val) {
      if (val > (1UL<<ROUTING_bits)){ throw("M_HDR setROUTING value out of range");}
      fields.ROUTING = val;
  }
  unsigned long int getROUTING() {
      return fields.ROUTING;
  }

};
//--------------------------------------------------------------------------------
// M_HDR2 word description
//--------------------------------------------------------------------------------
union M_HDR2 {
  struct  { 
    unsigned int SPARE : 11;
    unsigned int ORIENTATION : 1;
    unsigned int MODTYPE : 2;
    unsigned int MODID : 18;
  } fields;

  // access full word
  unsigned int fullword;

  // bit descriptors
  const static unsigned int MODID_bits = 18;
  const static unsigned int MODID_firstbit = 31;
  const static unsigned int MODID_lastbit = 14;
  void setMODID(unsigned int val) {
      if (val > (1UL<<MODID_bits)){ throw("M_HDR2 setMODID value out of range");}
      fields.MODID = val;
  }
  unsigned int getMODID() {
      return fields.MODID;
  }

  const static unsigned int MODTYPE_bits = 2;
  const static unsigned int MODTYPE_firstbit = 13;
  const static unsigned int MODTYPE_lastbit = 12;
  void setMODTYPE(unsigned int val) {
      if (val > (1UL<<MODTYPE_bits)){ throw("M_HDR2 setMODTYPE value out of range");}
      fields.MODTYPE = val;
  }
  unsigned int getMODTYPE() {
      return fields.MODTYPE;
  }

  const static unsigned int ORIENTATION_bits = 1;
  const static unsigned int ORIENTATION_firstbit = 11;
  const static unsigned int ORIENTATION_lastbit = 11;
  void setORIENTATION(unsigned int val) {
      if (val > (1UL<<ORIENTATION_bits)){ throw("M_HDR2 setORIENTATION value out of range");}
      fields.ORIENTATION = val;
  }
  unsigned int getORIENTATION() {
      return fields.ORIENTATION;
  }

  const static unsigned int SPARE_bits = 11;
  const static unsigned int SPARE_firstbit = 10;
  const static unsigned int SPARE_lastbit = 0;
  void setSPARE(unsigned int val) {
      if (val > (1UL<<SPARE_bits)){ throw("M_HDR2 setSPARE value out of range");}
      fields.SPARE = val;
  }
  unsigned int getSPARE() {
      return fields.SPARE;
  }

};
//--------------------------------------------------------------------------------
// HCC_HDR word description
//--------------------------------------------------------------------------------
union HCC_HDR {
  struct  { 
    unsigned short int BCID : 4;
    unsigned short int L0ID : 7;
    unsigned short int FLAG : 1;
    unsigned short int TYP : 4;
  } fields;

  // access full word
  unsigned short int fullword;

  // bit descriptors
  const static unsigned short int TYP_bits = 4;
  const static unsigned short int TYP_firstbit = 15;
  const static unsigned short int TYP_lastbit = 12;
  const static unsigned short int TYP_PR = 0x1;
  const static unsigned short int TYP_LP = 0x2;
  void setTYP(unsigned short int val) {
      if (val > (1UL<<TYP_bits)){ throw("HCC_HDR setTYP value out of range");}
      fields.TYP = val;
  }
  unsigned short int getTYP() {
      return fields.TYP;
  }

  const static unsigned short int FLAG_bits = 1;
  const static unsigned short int FLAG_firstbit = 11;
  const static unsigned short int FLAG_lastbit = 11;
  void setFLAG(unsigned short int val) {
      if (val > (1UL<<FLAG_bits)){ throw("HCC_HDR setFLAG value out of range");}
      fields.FLAG = val;
  }
  unsigned short int getFLAG() {
      return fields.FLAG;
  }

  const static unsigned short int L0ID_bits = 7;
  const static unsigned short int L0ID_firstbit = 10;
  const static unsigned short int L0ID_lastbit = 4;
  void setL0ID(unsigned short int val) {
      if (val > (1UL<<L0ID_bits)){ throw("HCC_HDR setL0ID value out of range");}
      fields.L0ID = val;
  }
  unsigned short int getL0ID() {
      return fields.L0ID;
  }

  const static unsigned short int BCID_bits = 4;
  const static unsigned short int BCID_firstbit = 3;
  const static unsigned short int BCID_lastbit = 0;
  void setBCID(unsigned short int val) {
      if (val > (1UL<<BCID_bits)){ throw("HCC_HDR setBCID value out of range");}
      fields.BCID = val;
  }
  unsigned short int getBCID() {
      return fields.BCID;
  }

};
//--------------------------------------------------------------------------------
// HCC_CLUSTER word description
//--------------------------------------------------------------------------------
union HCC_CLUSTER {
  struct  { 
    unsigned short int NEXT : 3;
    unsigned short int COL : 8;
    unsigned short int ABC : 4;
    unsigned short int UNUSED : 1;
  } fields;

  // access full word
  unsigned short int fullword;

  // bit descriptors
  const static unsigned short int UNUSED_bits = 1;
  const static unsigned short int UNUSED_firstbit = 15;
  const static unsigned short int UNUSED_lastbit = 15;
  void setUNUSED(unsigned short int val) {
      if (val > (1UL<<UNUSED_bits)){ throw("HCC_CLUSTER setUNUSED value out of range");}
      fields.UNUSED = val;
  }
  unsigned short int getUNUSED() {
      return fields.UNUSED;
  }

  const static unsigned short int ABC_bits = 4;
  const static unsigned short int ABC_firstbit = 14;
  const static unsigned short int ABC_lastbit = 11;
  void setABC(unsigned short int val) {
      if (val > (1UL<<ABC_bits)){ throw("HCC_CLUSTER setABC value out of range");}
      fields.ABC = val;
  }
  unsigned short int getABC() {
      return fields.ABC;
  }

  const static unsigned short int COL_bits = 8;
  const static unsigned short int COL_firstbit = 10;
  const static unsigned short int COL_lastbit = 3;
  void setCOL(unsigned short int val) {
      if (val > (1UL<<COL_bits)){ throw("HCC_CLUSTER setCOL value out of range");}
      fields.COL = val;
  }
  unsigned short int getCOL() {
      return fields.COL;
  }

  const static unsigned short int NEXT_bits = 3;
  const static unsigned short int NEXT_firstbit = 2;
  const static unsigned short int NEXT_lastbit = 0;
  void setNEXT(unsigned short int val) {
      if (val > (1UL<<NEXT_bits)){ throw("HCC_CLUSTER setNEXT value out of range");}
      fields.NEXT = val;
  }
  unsigned short int getNEXT() {
      return fields.NEXT;
  }

};
#define HCC_LAST_CLUSTER  0x6fed;

//--------------------------------------------------------------------------------
// PIXEL_CLUSTER word description
//--------------------------------------------------------------------------------
union PIXEL_CLUSTER {
  struct  { 
    unsigned int ETA : 13;
    unsigned int ETASIZ : 3;
    unsigned int PHI : 13;
    unsigned int PHISIZ : 2;
    unsigned int LAST : 1;
  } fields;

  // access full word
  unsigned int fullword;

  // bit descriptors
  const static unsigned int LAST_bits = 1;
  const static unsigned int LAST_firstbit = 31;
  const static unsigned int LAST_lastbit = 31;
  void setLAST(unsigned int val) {
      if (val > (1UL<<LAST_bits)){ throw("PIXEL_CLUSTER setLAST value out of range");}
      fields.LAST = val;
  }
  unsigned int getLAST() {
      return fields.LAST;
  }

  const static unsigned int PHISIZ_bits = 2;
  const static unsigned int PHISIZ_firstbit = 30;
  const static unsigned int PHISIZ_lastbit = 29;
  void setPHISIZ(unsigned int val) {
      if (val > (1UL<<PHISIZ_bits)){ throw("PIXEL_CLUSTER setPHISIZ value out of range");}
      fields.PHISIZ = val;
  }
  unsigned int getPHISIZ() {
      return fields.PHISIZ;
  }

  const static unsigned int PHI_bits = 13;
  const static unsigned int PHI_firstbit = 28;
  const static unsigned int PHI_lastbit = 16;
  void setPHI(unsigned int val) {
      if (val > (1UL<<PHI_bits)){ throw("PIXEL_CLUSTER setPHI value out of range");}
      fields.PHI = val;
  }
  unsigned int getPHI() {
      return fields.PHI;
  }

  const static unsigned int ETASIZ_bits = 3;
  const static unsigned int ETASIZ_firstbit = 15;
  const static unsigned int ETASIZ_lastbit = 13;
  void setETASIZ(unsigned int val) {
      if (val > (1UL<<ETASIZ_bits)){ throw("PIXEL_CLUSTER setETASIZ value out of range");}
      fields.ETASIZ = val;
  }
  unsigned int getETASIZ() {
      return fields.ETASIZ;
  }

  const static unsigned int ETA_bits = 13;
  const static unsigned int ETA_firstbit = 12;
  const static unsigned int ETA_lastbit = 0;
  void setETA(unsigned int val) {
      if (val > (1UL<<ETA_bits)){ throw("PIXEL_CLUSTER setETA value out of range");}
      fields.ETA = val;
  }
  unsigned int getETA() {
      return fields.ETA;
  }

};
//--------------------------------------------------------------------------------
// PIXEL_CL_FTR word description
//--------------------------------------------------------------------------------
union PIXEL_CL_FTR {
  struct  { 
    unsigned int SPARE : 14;
    unsigned int ERROR : 2;
    unsigned int COUNT : 8;
    unsigned int FLAG : 8;
  } fields;

  // access full word
  unsigned int fullword;

  // bit descriptors
  const static unsigned int FLAG_bits = 8;
  const static unsigned int FLAG_firstbit = 31;
  const static unsigned int FLAG_lastbit = 24;
  const static unsigned int FLAG_FLAG = 0x77;
  void setFLAG(unsigned int val) {
      if (val > (1UL<<FLAG_bits)){ throw("PIXEL_CL_FTR setFLAG value out of range");}
      fields.FLAG = val;
  }
  unsigned int getFLAG() {
      return fields.FLAG;
  }

  const static unsigned int COUNT_bits = 8;
  const static unsigned int COUNT_firstbit = 23;
  const static unsigned int COUNT_lastbit = 16;
  void setCOUNT(unsigned int val) {
      if (val > (1UL<<COUNT_bits)){ throw("PIXEL_CL_FTR setCOUNT value out of range");}
      fields.COUNT = val;
  }
  unsigned int getCOUNT() {
      return fields.COUNT;
  }

  const static unsigned int ERROR_bits = 2;
  const static unsigned int ERROR_firstbit = 15;
  const static unsigned int ERROR_lastbit = 14;
  const static unsigned int ERROR_NO_ERROR = 0x0;
  const static unsigned int ERROR_FRONT_END_ERROR = 0x1;
  const static unsigned int ERROR_PARSE_ERROR = 0x2;
  const static unsigned int ERROR_UNUSED = 0x3;
  void setERROR(unsigned int val) {
      if (val > (1UL<<ERROR_bits)){ throw("PIXEL_CL_FTR setERROR value out of range");}
      fields.ERROR = val;
  }
  unsigned int getERROR() {
      return fields.ERROR;
  }

  const static unsigned int SPARE_bits = 14;
  const static unsigned int SPARE_firstbit = 13;
  const static unsigned int SPARE_lastbit = 0;
  void setSPARE(unsigned int val) {
      if (val > (1UL<<SPARE_bits)){ throw("PIXEL_CL_FTR setSPARE value out of range");}
      fields.SPARE = val;
  }
  unsigned int getSPARE() {
      return fields.SPARE;
  }

};
//--------------------------------------------------------------------------------
// STRIP_CLUSTER word description
//--------------------------------------------------------------------------------
union STRIP_CLUSTER {
  struct  { 
    unsigned short int IDX : 11;
    unsigned short int LEN : 3;
    unsigned short int ROW : 1;
    unsigned short int LAST : 1;
  } fields;

  // access full word
  unsigned short int fullword;

  // bit descriptors
  const static unsigned short int LAST_bits = 1;
  const static unsigned short int LAST_firstbit = 15;
  const static unsigned short int LAST_lastbit = 15;
  void setLAST(unsigned short int val) {
      if (val > (1UL<<LAST_bits)){ throw("STRIP_CLUSTER setLAST value out of range");}
      fields.LAST = val;
  }
  unsigned short int getLAST() {
      return fields.LAST;
  }

  const static unsigned short int ROW_bits = 1;
  const static unsigned short int ROW_firstbit = 14;
  const static unsigned short int ROW_lastbit = 14;
  void setROW(unsigned short int val) {
      if (val > (1UL<<ROW_bits)){ throw("STRIP_CLUSTER setROW value out of range");}
      fields.ROW = val;
  }
  unsigned short int getROW() {
      return fields.ROW;
  }

  const static unsigned short int LEN_bits = 3;
  const static unsigned short int LEN_firstbit = 13;
  const static unsigned short int LEN_lastbit = 11;
  void setLEN(unsigned short int val) {
      if (val > (1UL<<LEN_bits)){ throw("STRIP_CLUSTER setLEN value out of range");}
      fields.LEN = val;
  }
  unsigned short int getLEN() {
      return fields.LEN;
  }

  const static unsigned short int IDX_bits = 11;
  const static unsigned short int IDX_firstbit = 10;
  const static unsigned short int IDX_lastbit = 0;
  void setIDX(unsigned short int val) {
      if (val > (1UL<<IDX_bits)){ throw("STRIP_CLUSTER setIDX value out of range");}
      fields.IDX = val;
  }
  unsigned short int getIDX() {
      return fields.IDX;
  }

};
//--------------------------------------------------------------------------------
// STRIP_CL_FTR word description
//--------------------------------------------------------------------------------
union STRIP_CL_FTR {
  struct  { 
    unsigned short int ERROR : 2;
    unsigned short int COUNT : 6;
    unsigned short int FLAG : 8;
  } fields;

  // access full word
  unsigned short int fullword;

  // bit descriptors
  const static unsigned short int FLAG_bits = 8;
  const static unsigned short int FLAG_firstbit = 15;
  const static unsigned short int FLAG_lastbit = 8;
  const static unsigned short int FLAG_FLAG = 0x77;
  void setFLAG(unsigned short int val) {
      if (val > (1UL<<FLAG_bits)){ throw("STRIP_CL_FTR setFLAG value out of range");}
      fields.FLAG = val;
  }
  unsigned short int getFLAG() {
      return fields.FLAG;
  }

  const static unsigned short int COUNT_bits = 6;
  const static unsigned short int COUNT_firstbit = 7;
  const static unsigned short int COUNT_lastbit = 2;
  void setCOUNT(unsigned short int val) {
      if (val > (1UL<<COUNT_bits)){ throw("STRIP_CL_FTR setCOUNT value out of range");}
      fields.COUNT = val;
  }
  unsigned short int getCOUNT() {
      return fields.COUNT;
  }

  const static unsigned short int ERROR_bits = 2;
  const static unsigned short int ERROR_firstbit = 1;
  const static unsigned short int ERROR_lastbit = 0;
  const static unsigned short int ERROR_NO_ERROR = 0x0;
  const static unsigned short int ERROR_FRONT_END_ERROR = 0x1;
  const static unsigned short int ERROR_PARSE_ERROR = 0x2;
  const static unsigned short int ERROR_UNUSED = 0x3;
  void setERROR(unsigned short int val) {
      if (val > (1UL<<ERROR_bits)){ throw("STRIP_CL_FTR setERROR value out of range");}
      fields.ERROR = val;
  }
  unsigned short int getERROR() {
      return fields.ERROR;
  }

};
//--------------------------------------------------------------------------------
// TRACK_W1 word description
//--------------------------------------------------------------------------------
union TRACK_W1 {
  struct  { 
    unsigned long int DETMAP : 13;
    unsigned long int HITMAP : 13;
    unsigned long int ROADID : 24;
    unsigned long int PRM : 10;
    unsigned long int TYPE : 4;
  } fields;

  // access full word
  unsigned long int fullword;

  // bit descriptors
  const static unsigned long int TYPE_bits = 4;
  const static unsigned long int TYPE_firstbit = 63;
  const static unsigned long int TYPE_lastbit = 60;
  const static unsigned long int TYPE_FIRST_STAGE = 0x1;
  const static unsigned long int TYPE_SECOND_STAGE = 0x2;
  void setTYPE(unsigned long int val) {
      if (val > (1UL<<TYPE_bits)){ throw("TRACK_W1 setTYPE value out of range");}
      fields.TYPE = val;
  }
  unsigned long int getTYPE() {
      return fields.TYPE;
  }

  const static unsigned long int PRM_bits = 10;
  const static unsigned long int PRM_firstbit = 59;
  const static unsigned long int PRM_lastbit = 50;
  void setPRM(unsigned long int val) {
      if (val > (1UL<<PRM_bits)){ throw("TRACK_W1 setPRM value out of range");}
      fields.PRM = val;
  }
  unsigned long int getPRM() {
      return fields.PRM;
  }

  const static unsigned long int ROADID_bits = 24;
  const static unsigned long int ROADID_firstbit = 49;
  const static unsigned long int ROADID_lastbit = 26;
  void setROADID(unsigned long int val) {
      if (val > (1UL<<ROADID_bits)){ throw("TRACK_W1 setROADID value out of range");}
      fields.ROADID = val;
  }
  unsigned long int getROADID() {
      return fields.ROADID;
  }

  const static unsigned long int HITMAP_bits = 13;
  const static unsigned long int HITMAP_firstbit = 25;
  const static unsigned long int HITMAP_lastbit = 13;
  void setHITMAP(unsigned long int val) {
      if (val > (1UL<<HITMAP_bits)){ throw("TRACK_W1 setHITMAP value out of range");}
      fields.HITMAP = val;
  }
  unsigned long int getHITMAP() {
      return fields.HITMAP;
  }

  const static unsigned long int DETMAP_bits = 13;
  const static unsigned long int DETMAP_firstbit = 12;
  const static unsigned long int DETMAP_lastbit = 0;
  void setDETMAP(unsigned long int val) {
      if (val > (1UL<<DETMAP_bits)){ throw("TRACK_W1 setDETMAP value out of range");}
      fields.DETMAP = val;
  }
  unsigned long int getDETMAP() {
      return fields.DETMAP;
  }

};
//--------------------------------------------------------------------------------
// TRACK_W2 word description
//--------------------------------------------------------------------------------
union TRACK_W2 {
  struct  { 
    unsigned long int PHI : 16;
    unsigned long int ETA : 16;
    unsigned long int STAGE2_SECTOR : 16;
    unsigned long int STAGE1_SECTOR : 16;
  } fields;

  // access full word
  unsigned long int fullword;

  // bit descriptors
  const static unsigned long int STAGE1_SECTOR_bits = 16;
  const static unsigned long int STAGE1_SECTOR_firstbit = 63;
  const static unsigned long int STAGE1_SECTOR_lastbit = 48;
  void setSTAGE1_SECTOR(unsigned long int val) {
      if (val > (1UL<<STAGE1_SECTOR_bits)){ throw("TRACK_W2 setSTAGE1_SECTOR value out of range");}
      fields.STAGE1_SECTOR = val;
  }
  unsigned long int getSTAGE1_SECTOR() {
      return fields.STAGE1_SECTOR;
  }

  const static unsigned long int STAGE2_SECTOR_bits = 16;
  const static unsigned long int STAGE2_SECTOR_firstbit = 47;
  const static unsigned long int STAGE2_SECTOR_lastbit = 32;
  void setSTAGE2_SECTOR(unsigned long int val) {
      if (val > (1UL<<STAGE2_SECTOR_bits)){ throw("TRACK_W2 setSTAGE2_SECTOR value out of range");}
      fields.STAGE2_SECTOR = val;
  }
  unsigned long int getSTAGE2_SECTOR() {
      return fields.STAGE2_SECTOR;
  }

  const static unsigned long int ETA_bits = 16;
  const static unsigned long int ETA_firstbit = 31;
  const static unsigned long int ETA_lastbit = 16;
  void setETA(unsigned long int val) {
      if (val > (1UL<<ETA_bits)){ throw("TRACK_W2 setETA value out of range");}
      fields.ETA = val;
  }
  unsigned long int getETA() {
      return fields.ETA;
  }

  const static unsigned long int PHI_bits = 16;
  const static unsigned long int PHI_firstbit = 15;
  const static unsigned long int PHI_lastbit = 0;
  void setPHI(unsigned long int val) {
      if (val > (1UL<<PHI_bits)){ throw("TRACK_W2 setPHI value out of range");}
      fields.PHI = val;
  }
  unsigned long int getPHI() {
      return fields.PHI;
  }

};
//--------------------------------------------------------------------------------
// TRACK_W3 word description
//--------------------------------------------------------------------------------
union TRACK_W3 {
  struct  { 
    unsigned long int CHI2 : 16;
    unsigned long int D0 : 16;
    unsigned long int Z0 : 16;
    unsigned long int QoverPt : 16;
  } fields;

  // access full word
  unsigned long int fullword;

  // bit descriptors
  const static unsigned long int QoverPt_bits = 16;
  const static unsigned long int QoverPt_firstbit = 63;
  const static unsigned long int QoverPt_lastbit = 48;
  void setQoverPt(unsigned long int val) {
      if (val > (1UL<<QoverPt_bits)){ throw("TRACK_W3 setQoverPt value out of range");}
      fields.QoverPt = val;
  }
  unsigned long int getQoverPt() {
      return fields.QoverPt;
  }

  const static unsigned long int Z0_bits = 16;
  const static unsigned long int Z0_firstbit = 47;
  const static unsigned long int Z0_lastbit = 32;
  void setZ0(unsigned long int val) {
      if (val > (1UL<<Z0_bits)){ throw("TRACK_W3 setZ0 value out of range");}
      fields.Z0 = val;
  }
  unsigned long int getZ0() {
      return fields.Z0;
  }

  const static unsigned long int D0_bits = 16;
  const static unsigned long int D0_firstbit = 31;
  const static unsigned long int D0_lastbit = 16;
  void setD0(unsigned long int val) {
      if (val > (1UL<<D0_bits)){ throw("TRACK_W3 setD0 value out of range");}
      fields.D0 = val;
  }
  unsigned long int getD0() {
      return fields.D0;
  }

  const static unsigned long int CHI2_bits = 16;
  const static unsigned long int CHI2_firstbit = 15;
  const static unsigned long int CHI2_lastbit = 0;
  void setCHI2(unsigned long int val) {
      if (val > (1UL<<CHI2_bits)){ throw("TRACK_W3 setCHI2 value out of range");}
      fields.CHI2 = val;
  }
  unsigned long int getCHI2() {
      return fields.CHI2;
  }

};
//--------------------------------------------------------------------------------
// TRACK_PIXEL_CLUSTER word description
//--------------------------------------------------------------------------------
union TRACK_PIXEL_CLUSTER {
  struct  { 
    unsigned long int LAST : 1;
    unsigned long int PHISIZ : 2;
    unsigned long int ETASIZ : 3;
    unsigned long int PHI : 13;
    unsigned long int ETA : 13;
    unsigned long int SPARE : 13;
    unsigned long int RAW_INCLUDED : 1;
    unsigned long int MODID : 18;
  } fields;

  // access full word
  unsigned long int fullword;

  // bit descriptors
  const static unsigned long int MODID_bits = 18;
  const static unsigned long int MODID_firstbit = 63;
  const static unsigned long int MODID_lastbit = 46;
  void setMODID(unsigned long int val) {
      if (val > (1UL<<MODID_bits)){ throw("TRACK_PIXEL_CLUSTER setMODID value out of range");}
      fields.MODID = val;
  }
  unsigned long int getMODID() {
      return fields.MODID;
  }

  const static unsigned long int RAW_INCLUDED_bits = 1;
  const static unsigned long int RAW_INCLUDED_firstbit = 45;
  const static unsigned long int RAW_INCLUDED_lastbit = 45;
  void setRAW_INCLUDED(unsigned long int val) {
      if (val > (1UL<<RAW_INCLUDED_bits)){ throw("TRACK_PIXEL_CLUSTER setRAW_INCLUDED value out of range");}
      fields.RAW_INCLUDED = val;
  }
  unsigned long int getRAW_INCLUDED() {
      return fields.RAW_INCLUDED;
  }

  const static unsigned long int SPARE_bits = 13;
  const static unsigned long int SPARE_firstbit = 44;
  const static unsigned long int SPARE_lastbit = 32;
  void setSPARE(unsigned long int val) {
      if (val > (1UL<<SPARE_bits)){ throw("TRACK_PIXEL_CLUSTER setSPARE value out of range");}
      fields.SPARE = val;
  }
  unsigned long int getSPARE() {
      return fields.SPARE;
  }

  const static unsigned long int ETA_bits = 13;
  const static unsigned long int ETA_firstbit = 31;
  const static unsigned long int ETA_lastbit = 19;
  void setETA(unsigned long int val) {
      if (val > (1UL<<ETA_bits)){ throw("TRACK_PIXEL_CLUSTER setETA value out of range");}
      fields.ETA = val;
  }
  unsigned long int getETA() {
      return fields.ETA;
  }

  const static unsigned long int PHI_bits = 13;
  const static unsigned long int PHI_firstbit = 18;
  const static unsigned long int PHI_lastbit = 6;
  void setPHI(unsigned long int val) {
      if (val > (1UL<<PHI_bits)){ throw("TRACK_PIXEL_CLUSTER setPHI value out of range");}
      fields.PHI = val;
  }
  unsigned long int getPHI() {
      return fields.PHI;
  }

  const static unsigned long int ETASIZ_bits = 3;
  const static unsigned long int ETASIZ_firstbit = 5;
  const static unsigned long int ETASIZ_lastbit = 3;
  void setETASIZ(unsigned long int val) {
      if (val > (1UL<<ETASIZ_bits)){ throw("TRACK_PIXEL_CLUSTER setETASIZ value out of range");}
      fields.ETASIZ = val;
  }
  unsigned long int getETASIZ() {
      return fields.ETASIZ;
  }

  const static unsigned long int PHISIZ_bits = 2;
  const static unsigned long int PHISIZ_firstbit = 2;
  const static unsigned long int PHISIZ_lastbit = 1;
  void setPHISIZ(unsigned long int val) {
      if (val > (1UL<<PHISIZ_bits)){ throw("TRACK_PIXEL_CLUSTER setPHISIZ value out of range");}
      fields.PHISIZ = val;
  }
  unsigned long int getPHISIZ() {
      return fields.PHISIZ;
  }

  const static unsigned long int LAST_bits = 1;
  const static unsigned long int LAST_firstbit = 0;
  const static unsigned long int LAST_lastbit = 0;
  void setLAST(unsigned long int val) {
      if (val > (1UL<<LAST_bits)){ throw("TRACK_PIXEL_CLUSTER setLAST value out of range");}
      fields.LAST = val;
  }
  unsigned long int getLAST() {
      return fields.LAST;
  }

};
//--------------------------------------------------------------------------------
// TRACK_STRIP_CLUSTER word description
//--------------------------------------------------------------------------------
union TRACK_STRIP_CLUSTER {
  struct  { 
    unsigned int LAST : 1;
    unsigned int LEN : 3;
    unsigned int IDX : 11;
    unsigned int ROW : 1;
    unsigned int MODID : 16;
  } fields;

  // access full word
  unsigned int fullword;

  // bit descriptors
  const static unsigned int MODID_bits = 16;
  const static unsigned int MODID_firstbit = 31;
  const static unsigned int MODID_lastbit = 16;
  void setMODID(unsigned int val) {
      if (val > (1UL<<MODID_bits)){ throw("TRACK_STRIP_CLUSTER setMODID value out of range");}
      fields.MODID = val;
  }
  unsigned int getMODID() {
      return fields.MODID;
  }

  const static unsigned int ROW_bits = 1;
  const static unsigned int ROW_firstbit = 15;
  const static unsigned int ROW_lastbit = 15;
  void setROW(unsigned int val) {
      if (val > (1UL<<ROW_bits)){ throw("TRACK_STRIP_CLUSTER setROW value out of range");}
      fields.ROW = val;
  }
  unsigned int getROW() {
      return fields.ROW;
  }

  const static unsigned int IDX_bits = 11;
  const static unsigned int IDX_firstbit = 14;
  const static unsigned int IDX_lastbit = 4;
  void setIDX(unsigned int val) {
      if (val > (1UL<<IDX_bits)){ throw("TRACK_STRIP_CLUSTER setIDX value out of range");}
      fields.IDX = val;
  }
  unsigned int getIDX() {
      return fields.IDX;
  }

  const static unsigned int LEN_bits = 3;
  const static unsigned int LEN_firstbit = 3;
  const static unsigned int LEN_lastbit = 1;
  void setLEN(unsigned int val) {
      if (val > (1UL<<LEN_bits)){ throw("TRACK_STRIP_CLUSTER setLEN value out of range");}
      fields.LEN = val;
  }
  unsigned int getLEN() {
      return fields.LEN;
  }

  const static unsigned int LAST_bits = 1;
  const static unsigned int LAST_firstbit = 0;
  const static unsigned int LAST_lastbit = 0;
  void setLAST(unsigned int val) {
      if (val > (1UL<<LAST_bits)){ throw("TRACK_STRIP_CLUSTER setLAST value out of range");}
      fields.LAST = val;
  }
  unsigned int getLAST() {
      return fields.LAST;
  }

};
