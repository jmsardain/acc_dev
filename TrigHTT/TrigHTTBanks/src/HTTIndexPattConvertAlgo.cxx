/**
 * @file HTTIndexPattConvertAlgo.cxx
 * @author Riley Xu - rixu@cern.ch
 * @date 2019-07-09
 * @brief Algorithm to convert legacy indexed banks (FTK-style) to the new format.
 */

#include "TrigHTTBanks/HTTIndexPattConvertAlgo.h"
#include "TrigHTTBanks/HTTIndexPattTool_Reader.h"
#include "TrigHTTBanks/HTTIndexPattTool_Writer.h"
#include "TrigHTTUtils/HTTMacros.h"


HTTIndexPattConvertAlgo::HTTIndexPattConvertAlgo(const std::string& name, ISvcLocator* pSvcLocator) :
    AthAlgorithm(name, pSvcLocator),
    m_reader("HTTIndexPattTool_ReaderLegacy"),
    m_writer("HTTIndexPattTool_Writer")
{
}

StatusCode HTTIndexPattConvertAlgo::initialize()
{
    ATH_CHECK(m_reader.retrieve());
    ATH_CHECK(m_writer.retrieve());
    return StatusCode::SUCCESS;
}

StatusCode HTTIndexPattConvertAlgo::execute()
{
    // Failsafe to only run once (better way?)
    static int loop = 0;
    if (loop != 0) return StatusCode::SUCCESS;
    loop++;

    auto sectors = m_reader->getSectors();
    for (auto sector : sectors)
    {
        vector<size_t> coverages;
        auto patterns = m_reader->readPatterns(sector, -1, 0, &coverages);
        for (size_t i = 0; i < patterns.size(); i++)
            m_writer->addPattern(sector, patterns[i], coverages[i]);
    }

    return StatusCode::SUCCESS;
}

StatusCode HTTIndexPattConvertAlgo::finalize()
{
    m_writer->write();

    return StatusCode::SUCCESS;
}
