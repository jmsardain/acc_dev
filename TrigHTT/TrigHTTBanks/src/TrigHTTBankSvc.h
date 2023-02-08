// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#ifndef TRIGHTTBANKSVC_H
#define TRIGHTTBANKSVC_H

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IInterface.h"
#include "TrigHTTMaps/ITrigHTTMappingSvc.h"
#include "TrigHTTBanks/ITrigHTTBankSvc.h"
#include "TrigHTTBanks/HTTFitConstantBank.h"
#include "TrigHTTBanks/HTTSectorSlice.h"
#include "TrigHTTBanks/HTTSectorBank.h"
#include "TrigHTTBanks/HTTPatternBank.h"
#include "TrigHTTBanks/HTTCCachePattIO.h"


class TrigHTTBankSvc : public AthService, virtual public ITrigHTTBankSvc
{
    public:

        ///////////////////////////////////////////////////////////////////////
        // AthService

        TrigHTTBankSvc(const std::string& name, ISvcLocator* svc);
        virtual ~TrigHTTBankSvc() = default;

        virtual StatusCode initialize() override;
        virtual StatusCode finalize() override;

        static const InterfaceID& interfaceID();
        virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIf);

        ///////////////////////////////////////////////////////////////////////
        // ITrigHTTBankSvc

        virtual HTTFitConstantBank* FitConstantBank_1st(int missedPlane = -1) override;
        virtual HTTFitConstantBank* FitConstantBank_2nd(int missedPlane = -1) override;
        virtual HTTSectorBank* SectorBank_1st() override;
        virtual HTTSectorBank* SectorBank_2nd() override;
        virtual HTTSectorSlice* SectorSlice() override;
        virtual HTTPatternBank* PatternBank() override;

    private:

        ServiceHandle<HTTCCachePattIO> m_ccacheIO;
        ServiceHandle<ITrigHTTMappingSvc> m_HTTMapping;

        ///////////////////////////////////////////////////////////////////////
        // Configuration parameters
        int m_ncoords_1st = 9;
        int m_ncoords_2nd = 18;
        StringProperty m_bankTypes = std::string("FILE");
        std::string m_path_constants_1st;
        std::string m_path_constants_2nd;
        std::vector<std::string> m_path_constants_1st_noguess;
        std::vector<std::string> m_path_constants_2nd_noguess;
        std::string m_path_sectorbank_1st;
        std::string m_path_sectorbank_2nd;
        std::string m_path_sectorslices;
        std::string m_path_patternbank;
        std::string m_path_NNconstants;

        ///////////////////////////////////////////////////////////////////////
        // Storage pointers
        HTTFitConstantBank* m_FitConstantBank_1st = nullptr;
        HTTFitConstantBank* m_FitConstantBank_2nd = nullptr;
        std::vector<HTTFitConstantBank*> m_FitConstantBankNoGuess_1st;
        std::vector<HTTFitConstantBank*> m_FitConstantBankNoGuess_2nd;
        HTTSectorBank* m_SectorBank_1st = nullptr;
        HTTSectorBank* m_SectorBank_2nd = nullptr;
        HTTSectorSlice* m_SectorSlices = nullptr;
        HTTPatternBank* m_PatternBank = nullptr;

        ///////////////////////////////////////////////////////////////////////
        // Methods for lazy loading.
        // missedPlane = -1 means banks where we guess any missing hits
        bool LoadFitConstantBank_1st(int missedPlane = -1);
        bool LoadFitConstantBank_2nd(int missedPlane = -1);
        bool LoadSectorBank_1st();
        bool LoadSectorBank_2nd();
        bool LoadSectorSlice();
        bool LoadPatternBank();

        ///////////////////////////////////////////////////////////////////////
        // Helper
        StatusCode checkInputs();
};

inline const InterfaceID& TrigHTTBankSvc::interfaceID()
{
    static const InterfaceID IID_TrigHTTBankSvc("TrigHTTBankSvc", 1, 0);
    return IID_TrigHTTBankSvc;
}

#endif   // TRIGHTTBANKSVC_H
