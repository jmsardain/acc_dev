/*
 * HTTPattGenToolI.cxx: See HTTPattGenToolI.h
 *
 * Created: August 9th, 2019
 * Author: Riley Xu
 * Email: rixu@cern.ch
 */

#include "TrigHTTBankGen/HTTPattGenToolI.h"

using std::vector;


/*****************************************************************************/
/* Constructors                                                              */
/*****************************************************************************/

HTTPattGenToolI::HTTPattGenToolI(const std::string& algname, const std::string &name, const IInterface *ifc) :
    AthAlgTool(algname, name, ifc),
    m_HTTMapping("TrigHTTMappingSvc", name),
    m_HTTBankSvc("TrigHTTBankSvc", name),
    m_rndmSvc("AtRanluxGenSvc", name)
{
    declareProperty("WCmax", m_maxWCs, "maximum number of wildcards allowed in a pattern (default: 2)");
    declareProperty("WCplanes", m_WCAllowed, "boolean vector for which layers are allowed to have a wildcard (default: all true)");
    declareProperty("rndStreamName", m_rndStreamName, "default: RANDOM");
}


StatusCode HTTPattGenToolI::initialize()
{
    // Input checks
    ATH_CHECK(m_HTTMapping.retrieve());
    ATH_CHECK(m_HTTBankSvc.retrieve());
    m_nLayers = m_HTTMapping->PlaneMap_1st()->getNLogiLayers();
    m_sectorBank = m_HTTBankSvc->SectorBank_1st();

    bool ok = false;
    if (m_nLayers == 0)
        ATH_MSG_ERROR("layers not set");
    else if (m_maxWCs > m_nLayers)
        ATH_MSG_ERROR("max WCs > layers");
    else if (m_WCAllowed.size() != m_nLayers && !m_WCAllowed.empty())
        ATH_MSG_ERROR("WCplanes size()=" << m_WCAllowed.size() << " but expected layers=" << m_nLayers);
    else if (!m_sectorBank)
        ATH_MSG_ERROR("sector bank is null");
    else
        ok = true;
    if (!ok) return StatusCode::FAILURE;

    // Initializations
    if (m_WCAllowed.empty()) m_WCAllowed.resize(m_nLayers, true);
    m_rndFlat = new CLHEP::RandFlat(m_rndmSvc->GetEngine(m_rndStreamName));
    m_rndGauss = new CLHEP::RandGauss(m_rndmSvc->GetEngine(m_rndStreamName));

    return StatusCode::SUCCESS;
}


/*****************************************************************************/
/* Configuration                                                             */
/*****************************************************************************/

void HTTPattGenToolI::setWCAllowed(std::vector<bool> const & list)
{
    if (list.size() != m_nLayers)
         ATH_MSG_ERROR("setWCAllowed() wrong number of layers");
    else m_WCAllowed = list;
}

/*****************************************************************************/
/* Core                                                                      */
/*****************************************************************************/

// This wrapper calls nextCandidates() and selects a pattern from among them.
ErrorStatus HTTPattGenToolI::next()
{
    vector<HTTPattern> pattCands;
    ErrorStatus es = nextCandidates(pattCands, m_pattWCs);
    m_iter++;

    if (es == ES_EOF)
        return ES_EOF;
    else if (es == ES_EMPTY || pattCands.empty())
        return ES_EMPTY;
    else if (es == ES_CONSTRAINT || m_pattWCs > m_maxWCs)
        return ES_CONSTRAINT;
    else if (es == ES_FAIL)
        return ES_FAIL;
    else
    {
        m_pattern = selectPattern(pattCands);
        return ES_OK;
    }
}


HTTPattern const & HTTPattGenToolI::selectPattern(vector<HTTPattern> const & pattCands) const
{
    // TODO only imported random selection per JAAA's comment from FTKPattGenRoot.
    // Also, the 7of8 distance filter from GoodTrack() in FTK was not copied (should?).
    size_t ind = (size_t)(pattCands.size() * getRandom());
    if (ind >= pattCands.size()) ind = pattCands.size() - 1;
    return pattCands[ind];
}
