// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGHTT_VERSIONS_CMMCPHITSAUXCONTAINER_V1_H
#define XAODTRIGHTT_VERSIONS_CMMCPHITSAUXCONTAINER_V1_H

// STL include(s):
#include <vector>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD{

  class HTTTrackTestAuxContainer_v1 : public AuxContainerBase {
  public:
    // Default constructor
    HTTTrackTestAuxContainer_v1();

  private:
    std::vector<float> pt;
    
  }; // class HTTTrackTestContainer_v1  
} // namespace xAOD

#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::HTTTrackTestAuxContainer_v1 , xAOD::AuxContainerBase );

#endif // XAODTRIGHTT_VERSIONS_CMMCPHITSAUXCONTAINER_V1_H
