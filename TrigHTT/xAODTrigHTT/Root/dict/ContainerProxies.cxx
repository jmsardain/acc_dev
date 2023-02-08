/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */
 
// $Id: ContainerProxies.cxx 693681 2015-09-08 12:18:40Z amazurov $

//For proper ROOT I/O, decalre  the container with ADD_NS_DV_PROXY

// xAOD include(s):
#include "xAODCore/AddDVProxy.h"



// Local include(s):
#include "xAODTrigHTT/versions/HTTTrackTestContainer_v1.h"

ADD_NS_DV_PROXY( xAOD , HTTTrackTestContainer_v1 );
