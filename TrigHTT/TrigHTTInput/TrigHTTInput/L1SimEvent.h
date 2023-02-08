/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1SimEvent_H_
#define L1SimEvent_H_

#include "TObject.h"
#include "TrigHTTInput/L1SimFitResult.h"


class L1SimEvent : public TObject {

 public:
  L1SimEvent() {_eventId=-1;};
  L1SimEvent(int eventid) {_eventId=eventid;};

  virtual ~L1SimEvent() {Reset();};
  void Reset() { _fitlist.clear(); _eventId=-1;};
  //set 
  void SetEventId(int& eventId){_eventId=eventId;};
  void SetFitList(std::vector<FitResult>& fitlist) {_fitlist=fitlist;};

  //get
  int nFitResults() {return _fitlist.size();};
  int EventId()     {return _eventId;};

  //get passing references
  void EventId(int& id)                            { id = _eventId;};
  void FitResults(std::vector<FitResult> &fitlist) { fitlist= _fitlist;};
  
 private:
  int _eventId;
  std::vector <FitResult> _fitlist;// need to save obj, not pointers

  ClassDef(L1SimEvent,1) 
};


#endif
