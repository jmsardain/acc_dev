// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGHTT_VERSIONS_HTTTRACKTESTCONTAINER_V1_H
#define XAODTRIGHTT_VERSIONS_HTTTRACKTESTCONTAINER_V1_H

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTrigHTT/versions/HTTTrackTest_v1.h"

namespace xAOD {
   /// Define the CMMCPHits as a simple DataVector
   typedef DataVector< xAOD::HTTTrackTest_v1 > HTTTrackTestContainer_v1;
}

#endif // XAODTRIGLHTT_VERSIONS_HTTTRACKTESTCONTAINER_V1_H
