/**
 * @file HTTIndexPattTool_Base.cxx
 * @brief See header.
 * @author Riley Xu - rixu@cern.ch
 * @date 2019-06-14
 */

#include "TrigHTTMaps/HTTPlaneMap.h"
#include "TrigHTTBanks/HTTIndexPattTool_Base.h"

#include <numeric>


HTTIndexPattTool_Base::HTTIndexPattTool_Base(std::string const & type, std::string const & name, IInterface const * parent) :
    AthAlgTool(type, name, parent),
    m_HTTMapSvc("TrigHTTMappingSvc", name)
{
    declareProperty("bank_path", m_filepath, "Input or output pattern bank filepath");
}


// @return true if inputs ok
bool HTTIndexPattTool_Base::checkInputs()
{
    if (m_filepath.empty())
        ATH_MSG_ERROR("initialize() empty filepath");
    else if (m_option.empty())
        ATH_MSG_ERROR("initialize() developer error, file open option empty");
    else
        return true;
    return false;
}


// Check inputs and opens the file
StatusCode HTTIndexPattTool_Base::initialize()
{
    if (!checkInputs()) return StatusCode::FAILURE;
    ATH_CHECK(m_HTTMapSvc.retrieve());

    m_nLayers = m_HTTMapSvc->PlaneMap_1st()->getNLogiLayers();

    m_file.reset(TFile::Open(m_filepath.c_str(), m_option.c_str()));
    if (!m_file || m_file->IsZombie())
    {
        ATH_MSG_ERROR("initialize() unable to open file");
        return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("Opened " << m_filepath);

    return StatusCode::SUCCESS;
}


// Calculate the metadata from sectors and indexData and populates m_metadata.
void HTTIndexPattTool_Base::calculateMetadata(std::vector<SectorIndexData> const & indexData)
{
    m_metadata = HTTBankMetadata(); // reset in case
    m_metadata.isValid = true;
    for (SectorIndexData const & dat : indexData) updateMetadata(dat);
}

// Updates m_metadata sector-by-sector
void HTTIndexPattTool_Base::updateMetadata(SectorIndexData const & indexData)
{
    m_metadata.nSectors++;
    if (indexData.coverages.front() > m_metadata.maxCoverage)
        m_metadata.maxCoverage = indexData.coverages.front();
    if (indexData.coverages.back() < m_metadata.minCoverage)
        m_metadata.minCoverage = indexData.coverages.back();
    m_metadata.nPatterns +=
        std::accumulate(indexData.nPatterns.begin(), indexData.nPatterns.end(), 0);
}

