// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration


#ifndef ITRIGHTTMAPPINGSVC_H
#define ITRIGHTTMAPPINGSVC_H

#include "GaudiKernel/IService.h"
#include "GaudiKernel/IInterface.h"
#include "TrigHTTMaps/HTTNNMap.h" // ECC - can't seem to use class forward declaration

// Forward declarations
class HTTRegionMap;
class HTTPlaneMap;
class HTTSSMap;
class HTTConnectionMap;
class HTTNNMap;


class ITrigHTTMappingSvc: virtual public IService
{
    public:

        static const InterfaceID& interfaceID();

        virtual const HTTPlaneMap* PlaneMap_1st() const = 0;
        virtual const HTTPlaneMap* PlaneMap_2nd() const = 0;
        virtual const HTTRegionMap* RegionMap_1st() const = 0;
        virtual const HTTRegionMap* RegionMap_2nd() const = 0;
        virtual const HTTRegionMap* SubRegionMap() const = 0;
        virtual const HTTSSMap* SSMap() const = 0;
        virtual const HTTConnectionMap* ConnectionMap() const = 0;
        virtual const HTTNNMap* NNMap() const = 0;
};


inline const InterfaceID& ITrigHTTMappingSvc::interfaceID()
{
    static const InterfaceID IID("ITrigHTTMappingSvc", 1, 0);
    return IID;
}


#endif   // ITRIGHTTMAPPINGSVC_H
