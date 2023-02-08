/**
 * @file HTTIndexPattMergeAlgo.h
 * @author Riley Xu - rixu@cern.ch
 * @date 2019-10-28
 * @brief Algorithm to merge pattern banks in the indexed format.
 */


#include "TrigHTTBanks/HTTIndexPattTool_Reader.h"
#include "TrigHTTBanks/HTTIndexPattTool_Writer.h"
#include "TrigHTTBanks/HTTIndexPattMergeAlgo.h"


#include <vector>
#include <string>

using std::vector;
using std::string;



HTTIndexPattMergeAlgo::HTTIndexPattMergeAlgo(const std::string& name, ISvcLocator* pSvcLocator) :
    AthAlgorithm(name, pSvcLocator),
    m_reader("HTTIndexPattTool_Reader"),
    m_writer("HTTIndexPattTool_Writer")
{
    declareProperty("inputs", m_inputPaths);
}

StatusCode HTTIndexPattMergeAlgo::initialize()
{
    if (m_inputPaths.empty())
    {
        ATH_MSG_ERROR("Input paths empty");
        return StatusCode::FAILURE;
    }
    m_descriptions.resize(m_inputPaths.size());

    ATH_CHECK(m_reader.retrieve());
    ATH_CHECK(m_writer.retrieve());

    return StatusCode::SUCCESS;
}

StatusCode HTTIndexPattMergeAlgo::execute()
{
    // Failsafe to only run once (better way?)
    static int loop = 0;
    if (loop != 0) return StatusCode::SUCCESS;
    loop++;

    vector<size_t> coverages; // to be loaded by reads
    for (size_t i = 0; i < m_inputPaths.size(); i++)
    {
        if (i > 0) ATH_CHECK(m_reader->nextFile(m_inputPaths[i]));
        m_descriptions[i] = m_reader->getMetadata().description;

        std::vector<sector_t> const & sectors = m_reader->getSectors();
        for (sector_t sector : sectors)
        {
            vector<pattern_t> patts = m_reader->readPatterns(sector, -1, 0, &coverages);
            for (size_t iPatt = 0; iPatt < patts.size(); iPatt++)
                m_writer->addPattern(sector, patts[iPatt], coverages[iPatt]);
        }
    }

    return StatusCode::SUCCESS;
}


StatusCode HTTIndexPattMergeAlgo::finalize()
{
    // Make output bank description. Postpend the description from the first file to get
    // tag information, which should be the same among all files (other than nTrials).
    m_writer->setDescription(makeMergeDescription() + m_descriptions.front());

    // Write the pattern bank
    m_writer->write();

    return StatusCode::SUCCESS;
}


// Given a description string (which is usually set by the tag configuration),
// looks for a substring 'truth:<filepath>\n' to get the wrapper file from which
// the patterns were generated.
std::string getTruthFile(std::string const & desc)
{
    size_t i = desc.find("truth:");
    if (i == std::string::npos) return std::string();
    i += 6; // skip the 'truth:'

    size_t j = desc.find("\n", i);
    if (j == std::string::npos) return std::string();

    return desc.substr(i, j-i);
}


// Given a description string set by makeMergeDescription(), looks for the
// list specified by the 'merge:' key to get the list of files used to create
// the input merged bank.
std::string getMergeList(std::string const & desc)
{
    size_t i = desc.find("merge:\n");
    if (i == std::string::npos) return std::string();
    i += 7; // skip the 'merge:\n'

    size_t j = desc.find("\n", i);
    while (j != std::string::npos)
    {
        if (j+1 >= desc.size()) break;
        if (desc[j+1] != '\t') break;
        j = desc.find("\n", j + 1);
    }

    return desc.substr(i, j - i + 1);
}


// Utility function to make the description containing a list of all merged files
std::string HTTIndexPattMergeAlgo::makeMergeDescription() const
{
    std::string merge("merge:\n");
    for (size_t i = 0; i < m_inputPaths.size(); i++)
    {
        std::string const & desc = m_descriptions[i];
        std::string const & path = m_inputPaths[i];

        if (path.find("index.truth") != std::string::npos) // This file is a truth gen
            merge += "\t" + getTruthFile(desc) + "\n";
        else if (path.find("index.merge") != std::string::npos) // This file was already merged once
            merge += getMergeList(desc);
        else
            merge += "\t" + path + "\n";
    }

    return merge;
}



/*
 * Old method by opening all files at once and going through the sectors sequentially
 * Annoying to do now that the readers are tools and also debatable if faster

inline bool isDone(vector<vector<sector_t>> const & sectors, vector<size_t> const & isector)
{
    for (size_t i = 0; i < sectors.size(); i++)
        if (isector[i] < sectors[i].size()) return false;
    return true;
}

void merge()
{
    // Readers
    vector<vector<sector_t>> sectors; // sectors from each file
    vector<size_t> isector(m_inputPaths.size()); // next sector (index into sectors) to read for each file
    for (auto & r : m_readers) sectors.push_back(r->getSectors());

    // Do the read. Try to read the same sector from each file for better locality
    sector_t curr_sector = 0; // go through sectors sequentially
    vector<size_t> coverages; // to be loaded by reads

    while (!isDone(sectors, isector))
    {
        for (size_t iFile = 0; iFile < sectors.size(); iFile++)
        {
            if (isector[iFile] >= sectors[iFile].size()) continue;
            sector_t sector = sectors[iFile][isector[iFile]];
            if (sector <= curr_sector) // < should never happen but catch in case
            {
                vector<pattern_t> patts = m_readers[iFile]->readPatterns(sector, -1, 0, &coverages);
                for (size_t iPatt = 0; iPatt < patts.size(); iPatt++)
                    m_writer->addPattern(sector, patts[iPatt], coverages[iPatt]);
                isector[iFile]++;
            }
        }
        curr_sector++;
    }

}
 */
