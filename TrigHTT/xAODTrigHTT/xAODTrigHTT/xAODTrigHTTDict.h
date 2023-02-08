// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


// to declare  DataVector<T>

#ifndef XAODTRIGHTT_XAODTRIGHTTDICT_H
#define XAODTRIGHTT_XAODTRIGHTTDICT_H

// STL include(s):
#include <vector>

// EDM include(s):
#include "AthLinks/DataLink.h"
#include "AthLinks/ElementLink.h"

// Local include(s):
#include "xAODTrigHTT/HTTTrackTestContainer.h"
#include "xAODTrigHTT/versions/HTTTrackTest_v1.h"
#include "xAODTrigHTT/versions/HTTTrackTestContainer_v1.h"
#include "xAODTrigHTT/versions/HTTTrackTestAuxContainer_v1.h"


namespace{
  struct GCCXML_DUMMY_INSTANTIATION_XAODTRIGHTT {
    //container
    xAOD::HTTTrackTestContainer_v1 TrackTestContainer_v1_a1;
    ElementLink< xAOD::HTTTrackTestContainer_v1 > TrackContainer_v1_a2;
    std::vector< ElementLink< xAOD::HTTTrackTestContainer_v1 > > TrackContainer_v1_a3;
    std::vector< std::vector< ElementLink< xAOD::HTTTrackTestContainer_v1 > > > TrackContainer_v1_a4;

  };
}

#endif // XAODTRIGHTT_XAODTRIGHTTDICT_H
