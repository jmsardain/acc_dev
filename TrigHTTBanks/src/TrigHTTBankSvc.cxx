// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration


#include "TrigHTTBankSvc.h"


///////////////////////////////////////////////////////////////////////////////
// AthService Functions
///////////////////////////////////////////////////////////////////////////////


TrigHTTBankSvc::TrigHTTBankSvc(const std::string& name, ISvcLocator*svc) :
    AthService(name,svc),
    m_ccacheIO("HTTCCachePattIO", name),
    m_HTTMapping("TrigHTTMappingSvc", name)
{
    declareProperty("constants_1st",            m_path_constants_1st,               "Path of the 1st stage constants");
    declareProperty("constants_2nd",            m_path_constants_2nd,               "Path of the 2nd stage constants");
    declareProperty("constantsNoGuess_1st",     m_path_constants_1st_noguess,       "Path of the 1st stage constants without guessing");
    declareProperty("constantsNoGuess_2nd",     m_path_constants_2nd_noguess,       "Path of the 2nd stage constants without guessing");
    declareProperty("sectorBank_1st",           m_path_sectorbank_1st,              "Path of the first stage sector bank");
    declareProperty("sectorBank_2nd",           m_path_sectorbank_2nd,              "Path of the second stage sector bank");
    declareProperty("sectorSlices",             m_path_sectorslices,                "Path of the sector slices");
    declareProperty("patternBank",              m_path_patternbank,                 "Path of the pattern bank");
    declareProperty("BankType",                 m_bankTypes,                        "can be FILE or COOL");
    declareProperty("NCoords_1st",              m_ncoords_1st,                      "Number of 1st stage coordinates");
    declareProperty("NCoords_2nd",              m_ncoords_2nd,                      "Number of 2nd stage coordinates");
    // ECC - add in path for NN constants
    declareProperty("NNconstants",              m_path_NNconstants, "Path for NN constants");
}


StatusCode TrigHTTBankSvc::queryInterface(const InterfaceID& riid, void** ppvIf)
{
    if (interfaceID() == riid)
        *ppvIf = dynamic_cast<TrigHTTBankSvc*>(this);
    else if (ITrigHTTBankSvc::interfaceID() == riid)
        *ppvIf = dynamic_cast<ITrigHTTBankSvc*>(this);
    else
        return AthService::queryInterface(riid, ppvIf);

    addRef();
    return StatusCode::SUCCESS;
}


StatusCode TrigHTTBankSvc::checkInputs()
{
    // :(
    return StatusCode::SUCCESS;
}


StatusCode TrigHTTBankSvc::initialize()
{
    ATH_MSG_DEBUG("-------------- TrigHTTBankSvc -------------------");
    ATH_MSG_DEBUG("BankType Map    = " << m_bankTypes);
    ATH_MSG_DEBUG("Number of 1st stage coordinates = " << m_ncoords_1st);
    ATH_MSG_DEBUG("Number of 2nd stage coordinates = " << m_ncoords_2nd);
    ATH_MSG_DEBUG("-------------------------------------------------");

    ATH_CHECK(m_ccacheIO.retrieve());

    if (m_bankTypes.value()=="FILE")
    {
        ATH_CHECK(checkInputs());
    }
    else if (m_bankTypes.value() == "COOL")
    {
        ATH_MSG_ERROR("BANK TYPE COOL NOT YET SUPPORTED");
        return StatusCode::FAILURE;
    }
    else
    {
        ATH_MSG_ERROR("BANK TYPE " << m_bankTypes << "NOT YET SUPPORTED");
        return StatusCode::FAILURE;
    }

    // resize this to the appropriate one and to be safe, set to nullptr to start
    m_FitConstantBankNoGuess_1st.resize(m_HTTMapping->PlaneMap_1st()->getNLogiLayers());
    m_FitConstantBankNoGuess_2nd.resize(m_HTTMapping->PlaneMap_2nd()->getNLogiLayers());
    for (unsigned i = 0; i < m_FitConstantBankNoGuess_1st.size(); i++) m_FitConstantBankNoGuess_1st[i] = nullptr;
    for (unsigned i = 0; i < m_FitConstantBankNoGuess_2nd.size(); i++) m_FitConstantBankNoGuess_2nd[i] = nullptr;

    return StatusCode::SUCCESS;
}


StatusCode TrigHTTBankSvc::finalize()
{
    ATH_MSG_INFO("TrigHTTBankSvc::finalize()");

    if (m_FitConstantBank_1st) delete m_FitConstantBank_1st;
    if (m_FitConstantBank_2nd) delete m_FitConstantBank_2nd;
    for (auto bank : m_FitConstantBankNoGuess_1st) if (bank) delete bank;
    for (auto bank : m_FitConstantBankNoGuess_2nd) if (bank) delete bank;
    if (m_SectorBank_1st) delete m_SectorBank_1st;
    if (m_SectorBank_2nd) delete m_SectorBank_2nd;
    if (m_SectorSlices) delete m_SectorSlices;
    if (m_PatternBank) delete m_PatternBank;

    return StatusCode::SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
// Load Functions
///////////////////////////////////////////////////////////////////////////////

bool TrigHTTBankSvc::LoadFitConstantBank_1st(int missedPlane)
{
    MsgStream m(msgSvc(), "HTTFitConstantBank_1st");
    m.setLevel(msg().level()); // cause AthMessaging is stupid and doesn't have this function // in r22!

    if (missedPlane == -1)
      m_FitConstantBank_1st = new HTTFitConstantBank(m, m_HTTMapping->PlaneMap_1st(), m_ncoords_1st, m_path_constants_1st, true);
    else
      m_FitConstantBankNoGuess_1st[missedPlane] = new HTTFitConstantBank(m, m_HTTMapping->PlaneMap_1st(), m_ncoords_1st, m_path_constants_1st_noguess[missedPlane], true, missedPlane); // no check on the plane number

    return true;
}


bool TrigHTTBankSvc::LoadFitConstantBank_2nd(int missedPlane)
{
    MsgStream m(msgSvc(), "HTTFitConstantBank_2nd");
    m.setLevel(msg().level()); // cause AthMessaging is stupid and doesn't have this function // in r22!
    if (missedPlane == -1)
      m_FitConstantBank_2nd = new HTTFitConstantBank(m, m_HTTMapping->PlaneMap_2nd(), m_ncoords_2nd, m_path_constants_2nd, false);
    else
      m_FitConstantBankNoGuess_2nd[missedPlane] = new HTTFitConstantBank(m, m_HTTMapping->PlaneMap_2nd(), m_ncoords_2nd, m_path_constants_2nd_noguess[missedPlane], false, missedPlane); // no check on the plane number

    return true;
}


bool TrigHTTBankSvc::LoadSectorBank_1st()
{
    MsgStream m(msgSvc(), "HTTSectorBank_1st");
    m.setLevel(msg().level()); // cause AthMessaging is stupid and doesn't have this function // in r22!
    m_SectorBank_1st = new HTTSectorBank(m, m_path_sectorbank_1st);
    return true;
}


bool TrigHTTBankSvc::LoadSectorBank_2nd()
{
  MsgStream m(msgSvc(), "HTTSectorBank_2nd");
  m.setLevel(msg().level()); // cause AthMessaging is stupid and doesn't have this function // in r22!                                                                                                    
  m_SectorBank_2nd = new HTTSectorBank(m, m_path_sectorbank_2nd);
  return true;
}


bool TrigHTTBankSvc::LoadSectorSlice()
{
    MsgStream m(msgSvc(), "HTTSectorSlice");
    m.setLevel(msg().level()); // cause AthMessaging is stupid and doesn't have this function // in r22!
    m_SectorSlices = new HTTSectorSlice(m, m_path_sectorslices);
    return true;
}


bool TrigHTTBankSvc::LoadPatternBank()
{
    m_PatternBank = new HTTPatternBank(m_ccacheIO->read(m_path_patternbank));
    return true;
}


///////////////////////////////////////////////////////////////////////////////
// ITrigHTTBankSvc Functions
///////////////////////////////////////////////////////////////////////////////


HTTFitConstantBank* TrigHTTBankSvc::FitConstantBank_1st(int missedPlane)
{
    if (missedPlane == -1) {
        // Don't need to check the return actually, will return nullptr anyways
        if (!m_FitConstantBank_1st) LoadFitConstantBank_1st();

        return m_FitConstantBank_1st;
    }
    else {
        // Don't need to check the return actually, will return nullptr anyways
        if (!m_FitConstantBankNoGuess_1st[missedPlane]) LoadFitConstantBank_1st(missedPlane); 

        return m_FitConstantBankNoGuess_1st[missedPlane];
    }
}


HTTFitConstantBank* TrigHTTBankSvc::FitConstantBank_2nd(int missedPlane)
{
    if (missedPlane == -1) {
        // Don't need to check the return actually, will return nullptr anyways
        if (!m_FitConstantBank_2nd) LoadFitConstantBank_2nd();

        return m_FitConstantBank_2nd;
    }
    else {
        // Don't need to check the return actually, will return nullptr anyways
        if (!m_FitConstantBankNoGuess_2nd[missedPlane]) LoadFitConstantBank_2nd(missedPlane);

        return m_FitConstantBankNoGuess_2nd[missedPlane];
    }
}


HTTSectorBank* TrigHTTBankSvc::SectorBank_1st()
{
    // Don't need to check the return actually, will return nullptr anyways
    if (!m_SectorBank_1st) LoadSectorBank_1st();

    return m_SectorBank_1st;
}

HTTSectorBank* TrigHTTBankSvc::SectorBank_2nd()
{
    // Don't need to check the return actually, will return nullptr anyways
    if (!m_SectorBank_2nd) LoadSectorBank_2nd();

    return m_SectorBank_2nd;
}


HTTSectorSlice* TrigHTTBankSvc::SectorSlice()
{
    // Don't need to check the return actually, will return nullptr anyways
    if (!m_SectorSlices) LoadSectorSlice();

    return m_SectorSlices;
}


HTTPatternBank* TrigHTTBankSvc::PatternBank()
{
    // Don't need to check the return actually, will return nullptr anyways
    if (!m_PatternBank) LoadPatternBank();

    return m_PatternBank;
}
