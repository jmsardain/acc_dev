/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTTInputHeaderTool_H
#define HTTInputHeaderTool_H

/**
 * @file HTT_ReadRawHitsTool.h
 *
 * This class reads input data from a ROOT file (wrapper file)
 */

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigHTTInput/HTTEventInputHeaderToolI.h"
#include "TFile.h"
#include "TTree.h"

class HTTEventInputHeader;

class HTTInputHeaderTool : public extends<AthAlgTool, HTTEventInputHeaderToolI>  {
  
 public:
  
  HTTInputHeaderTool(const std::string&, const std::string&, const IInterface*);
  virtual ~HTTInputHeaderTool() {;}
    
  virtual StatusCode initialize() override; 
  virtual StatusCode finalize()   override;
  virtual StatusCode readData(HTTEventInputHeader* header, bool &last)  override;
  virtual StatusCode writeData(HTTEventInputHeader* header)  override; //not implmeneted yet
  
  
 private:
  // JO configuration
  StringArrayProperty m_inpath;
  StringProperty m_rwoption;
  
  //internal pointers  
  unsigned  m_event = 0;
  unsigned  m_totevent=0;
  unsigned  m_file = 0;
 
  StatusCode openFile(std::string const & path);

};

#endif // HTT_READRAWHINPUTTOOL_H
