/**
 * @file HTTIndexPattConvertAlgo.cxx
 * @author Riley Xu - rixu@cern.ch
 * @date 2019-10-28
 * @brief Algorithm to convert legacy indexed banks (FTK-style) to the new format.
 */

#include "AthenaBaseComps/AthAlgorithm.h"

#include <string>

class HTTIndexPattTool_ReaderI;
class HTTIndexPattTool_Writer;

/*
 * This algorithm converts FTK indexed banks to HTT indexed banks.
 * maxEvents should be set to 1. The reader and writer tools
 * are currently hard-coded to the default-named ones.
 *
 * Usage (job options):
 *      reader = HTTIndexPattTool_ReaderLegacy()
 *      reader.layers = layers
 *      reader.bank_path = input_bank
 *
 *      writer = HTTIndexPattTool_Writer()
 *      writer.layers = layers
 *      writer.bank_path = output_bank
 */
class HTTIndexPattConvertAlgo : public AthAlgorithm
{
    public:
        HTTIndexPattConvertAlgo(const std::string& name, ISvcLocator* pSvcLocator);
        StatusCode initialize() override;
        StatusCode execute() override;
        StatusCode finalize() override;

    private:
        ToolHandle<HTTIndexPattTool_ReaderI> m_reader;
        ToolHandle<HTTIndexPattTool_Writer> m_writer;
};
