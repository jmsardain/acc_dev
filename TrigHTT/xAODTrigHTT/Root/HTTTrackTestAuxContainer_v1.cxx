/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMMCPHitsAuxContainer_v1.cxx 579924 2014-01-24 10:26:51Z krasznaa $

// Local include(s):
#include "xAODTrigHTT/versions/HTTTrackTestAuxContainer_v1.h"

namespace xAOD {

  HTTTrackTestAuxContainer_v1::HTTTrackTestAuxContainer_v1() :
    AuxContainerBase()
  {
    AUX_VARIABLE( pt );
 
  }

} // namespace xAOD
