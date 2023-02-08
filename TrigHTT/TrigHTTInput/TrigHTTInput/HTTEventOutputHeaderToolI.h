/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTTEventOutputHeaderToolI_H
#define HTTEventOutputHeaderToolI_H

#include "GaudiKernel/IAlgTool.h"
#include "TFile.h"
#include "TTree.h"

class HTTLogicalEventOutputHeader;
class HTTLogicalEventInputHeader;

static const InterfaceID IID_HTTEventOutputHeaderToolI("HTTEventOutputHeaderToolI",1,0);

class HTTEventOutputHeaderToolI : virtual public ::IAlgTool 
{      
 public:

  DeclareInterfaceID( HTTEventOutputHeaderToolI, 1, 0);
  virtual ~HTTEventOutputHeaderToolI(){}
   
  virtual StatusCode readData(HTTLogicalEventInputHeader* INheader_1st, HTTLogicalEventInputHeader* INheader_2nd, HTTLogicalEventOutputHeader* OUTheader, bool &last) = 0;
  virtual StatusCode writeData(HTTLogicalEventInputHeader* INheader_1st, HTTLogicalEventInputHeader* INheader_2nd, HTTLogicalEventOutputHeader* OUTheader) = 0;
  
  virtual TTree* getEventTree() {return m_EventTree;};
  virtual  HTTLogicalEventInputHeader*  getLogicalEventInputHeader_1st() const  {return m_eventInputHeader_1st; };
  virtual  HTTLogicalEventInputHeader*  getLogicalEventInputHeader_2nd() const  {return m_eventInputHeader_2nd; };
  virtual  HTTLogicalEventOutputHeader* getLogicalEventOutputHeader() const  {return m_eventOutputHeader;};
 
 protected:
  
  HTTLogicalEventInputHeader  *m_eventInputHeader_1st;
  HTTLogicalEventInputHeader  *m_eventInputHeader_2nd;
  HTTLogicalEventOutputHeader *m_eventOutputHeader;

  TFile *m_infile = nullptr;
  TTree *m_EventTree = nullptr;

};


#endif // HTTEventOutputHeaderToolI_H
