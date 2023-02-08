/*
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
* @file src/HTTOverlapRemovalValAlg.h
* @author Zhaoyuan.Cui@cern.ch
* @date Dec. 3, 2020
* @brief Main algorithm for Overlap removal validation framework.
*/
#ifndef HTTOVERLAPREMOVALVALALG_H
#define HTTOVERLAPREMOVALVALALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
//#include "GaudiKernel/ITHistSvc.h"

#include "TrigHTTAlgorithms/HTTOverlapRemovalTool.h"
#include "TrigHTTInput/HTTEventOutputHeaderToolI.h"
#include "TrigHTTMonitor/HTTOverlapRemovalMonitorTool.h"

class ITrigHTTBankSvc;
class IHTTEventSelectionSvc;
class HTTLogicalEventOutputHeader;
class HTTLogicalEventInputHeader;

/**
* @class HTTOverlapRemovalAlg
* @brief Algorithm of Overlap Removal validation.
*
* This algorithm mainly uses HTTOverlapRemovalTool and HTTOverlapRemovalMonitorTool
* to validate the overlap removal algorithm. It also uses HTTEventOutputHeaderToolI to read
* HTTLogicalEvent track branch.
*/

class HTTOverlapRemovalValAlg : public AthAlgorithm
{
public:
  //  AthAlgorithm basics
  HTTOverlapRemovalValAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~HTTOverlapRemovalValAlg(){}

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();


private:
  //  Tools
  ToolHandle<HTTEventOutputHeaderToolI>    m_readOutputTool;

  ToolHandle<HTTOverlapRemovalTool>         m_OverlapRemovalTool;
  ToolHandle<HTTOverlapRemovalMonitorTool>  m_OverlapRemovalMonitorTool;
  ServiceHandle<ITrigHTTBankSvc>            m_HTTBankSvc;
  ServiceHandle<IHTTEventSelectionSvc>      m_EvtSel;

  //  ROOT pointer
  HTTLogicalEventInputHeader*   m_inputHeader_1st;
  HTTLogicalEventInputHeader*   m_inputHeader_2nd;
  HTTLogicalEventOutputHeader*  m_outputHeader;
};

#endif  // HTTOVERLAPREMOVALVALALG_H
