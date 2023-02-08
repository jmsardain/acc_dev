/*
  Copyright (C) 2002-20121 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file HTTEventInputHeaderToolI.h
 *
 * This declares a basic interface for input tools which provide the HTTEventInputHeader data
 * to all HTT processing.
 */

#ifndef HTTEventInputHeaderToolI_H
#define HTTEventInputHeaderToolI_H

#include "GaudiKernel/IAlgTool.h"
#include "TFile.h"
#include "TTree.h"


class HTTEventInputHeader;

static const InterfaceID IID_HTTEventInputHeaderToolI("HTTEventInputHeaderToolI",1,0);

class HTTEventInputHeaderToolI : virtual public ::IAlgTool 
{      
 public:

  DeclareInterfaceID( HTTEventInputHeaderToolI, 1, 0);
  virtual ~HTTEventInputHeaderToolI(){}
   
  virtual StatusCode readData(HTTEventInputHeader* header, bool &last) = 0;
  virtual StatusCode writeData(HTTEventInputHeader* header) = 0;
  virtual HTTEventInputHeader*  getEventInputHeader()   {return m_eventHeader; };
  virtual TTree* getEventTree() {return m_EventTree;};

 protected:

  HTTEventInputHeader*        m_eventHeader;
  TFile *                     m_infile = nullptr;
  TTree *                     m_EventTree = nullptr;
  
};


#endif // HTTEventInputHeaderToolI_H
