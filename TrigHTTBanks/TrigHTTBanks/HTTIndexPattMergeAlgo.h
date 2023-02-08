/**
 * @file HTTIndexPattMergeAlgo.h
 * @author Riley Xu - rixu@cern.ch
 * @date 2019-10-28
 * @brief Algorithm to merge pattern banks in the indexed format.
 */

#include "AthenaBaseComps/AthAlgorithm.h"

#include <string>

class HTTIndexPattTool_ReaderI;
class HTTIndexPattTool_Writer;

/*
 * This algorithm merges indexed pattern banks stored in separate ROOT files
 * maxEvents should be set to 1. The reader and writer tools
 * are currently hard-coded to the default-named ones.
 *
 * Usage (job options):
 *      reader = HTTIndexPattTool_Reader()
 *      reader.layers = layers
 *      reader.bank_path = input_banks[0]
 *
 *      merge = HTTIndexPattMergeAlgo()
 *      merge.inputs = input_banks
 *
 *      writer = HTTIndexPattTool_Writer()
 *      writer.layers = layers
 *      writer.bank_path = output_bank
 *
 */
class HTTIndexPattMergeAlgo : public AthAlgorithm
{
    public:
        HTTIndexPattMergeAlgo(const std::string& name, ISvcLocator* pSvcLocator);
        StatusCode initialize() override;
        StatusCode execute() override;
        StatusCode finalize() override;

    private:
        std::vector<std::string> m_inputPaths;
        std::vector<std::string> m_descriptions; // Copy the description metadata from the input files
        ToolHandle<HTTIndexPattTool_ReaderI> m_reader;
        ToolHandle<HTTIndexPattTool_Writer> m_writer;

        std::string makeMergeDescription() const;
};
