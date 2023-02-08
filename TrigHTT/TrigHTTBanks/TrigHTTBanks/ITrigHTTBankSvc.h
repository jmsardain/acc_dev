// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#ifndef ITRIGHTTBANKSVC_H
#define ITRIGHTTBANKSVC_H

/**
 * This file defines an interface for a bank service. The service is in charge
 * of loading all constants, including the fit coefficients, pattern banks, etc.
 * Other classes can use this service to retrieve the banks.
 */


#include "GaudiKernel/IService.h"
#include "GaudiKernel/IInterface.h"

class HTTFitConstantBank;
class HTTPatternBank;
class HTTSectorSlice;
class HTTSectorBank;

class ITrigHTTBankSvc : virtual public IService
{
    public:
        virtual ~ITrigHTTBankSvc() = default;
        static const InterfaceID& interfaceID();

        virtual HTTFitConstantBank* FitConstantBank_1st(int missedPlane = -1) = 0;
        virtual HTTFitConstantBank* FitConstantBank_2nd(int missedPlane = -1) = 0;
        virtual HTTSectorSlice* SectorSlice() = 0;
        virtual HTTSectorBank* SectorBank_1st() = 0;
        virtual HTTSectorBank* SectorBank_2nd() = 0;
        virtual HTTPatternBank* PatternBank() = 0;
	//	std::string getNNpath(){return m_path_NNconstants;}
};

inline const InterfaceID& ITrigHTTBankSvc::interfaceID()
{
    static const InterfaceID IID("ITrigHTTBankSvc", 1, 0);
    return IID;
}


#endif   // ITRIGHTTBANKSVC_H
