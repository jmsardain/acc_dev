/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGHTT_TRACKTEST_H
#define XAODTRIGHTT_TRACKTEST_H

// Local include(s):
#include "xAODTrigHTT/versions/HTTTrackTest_v1.h"
// clid -m "std::vector<MyObject>"

/// Namespace holding all the xAOD EDM classes
namespace xAOD {
   /// Define the latest version of the CMMRoI class
   typedef HTTTrackTest_v1 HTTTrackTest;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::HTTTrackTest , 127429023 , 1 )

#endif //XAODTRIGHTT_TRACKTEST_H
