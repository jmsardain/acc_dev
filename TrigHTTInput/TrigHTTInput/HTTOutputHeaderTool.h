/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTT_READOUTPUTHEADERTOOL_H
#define HTT_READOUTPUTHEADERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigHTTInput/HTTEventOutputHeaderToolI.h"
#include <numeric>

class HTTLogicalEventInputHeader;
class HTTLogicalEventOutputHeader;

class HTTOutputHeaderTool : public extends<AthAlgTool, HTTEventOutputHeaderToolI>  {

public:

  HTTOutputHeaderTool(std::string const &, std::string const &, IInterface const *);
  virtual ~HTTOutputHeaderTool() {}

  virtual StatusCode initialize() override; 
  virtual StatusCode finalize()   override;

  virtual StatusCode readData(HTTLogicalEventInputHeader* INheader_1st, HTTLogicalEventInputHeader* INheader_2nd, HTTLogicalEventOutputHeader* OUTheader, bool &last) override;
  virtual StatusCode writeData(HTTLogicalEventInputHeader* INheader_1st, HTTLogicalEventInputHeader* INheader_2nd, HTTLogicalEventOutputHeader* OUTheader)            override;
  
  std::string fileName() { return std::accumulate(m_inpath.value().begin(), m_inpath.value().end(), std::string{}); }

private:
  // JO configuration
  StringArrayProperty   m_inpath;
  StringProperty        m_rwoption        = std::string("READ");
  BooleanProperty       m_runSecondStage  = false;

  // internal counters  
  unsigned  m_event    = 0;
  unsigned  m_totevent = 0;
  unsigned  m_file     = 0;

  std::string branchNameIn_1st  = "HTTLogicalEventInputHeader_1st";
  std::string branchNameIn_2nd  = "HTTLogicalEventInputHeader_2nd";
  std::string branchNameOut     = "HTTLogicalEventOutputHeader";

  StatusCode openFile(std::string const & path);

};

#endif // HTT_READOUTPUTHEADERTOOL_H
