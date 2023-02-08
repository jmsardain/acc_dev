/**
 * @file HTTIndexPattIO_test.cxx
 * @brief Unit tests for HTTIndexPattIO
 * @author Riley Xu - rixu@cern.ch
 * @date 2019-06-14
 */

#undef NDEBUG
#include <cassert>
#include <string>
#include <iostream>
#include <numeric>

#include "TestTools/initGaudi.h"
#include "TrigHTTBanks/HTTIndexPattTool_Base.h"
#include "TrigHTTBanks/HTTIndexPattTool_Reader.h"
#include "TrigHTTBanks/HTTIndexPattTool_Writer.h"

using namespace std;


// TESTBANK and TESTBANK_LEGACY both have the same content
void test_read_base(HTTIndexPattTool_ReaderI *reader, bool legacy)
{
    auto sectors = reader->getSectors();
    assert(sectors.size() == 13487);
    assert(sectors.front() == 0);

    // Expected values for below
    vector<UInt_t> nPatterns = {2, 4, 5, 22, 41, 128, 282, 724, 1943, 7146};
    vector<UInt_t> coverages = {11, 9, 8, 7, 6, 5, 4, 3, 2, 1};
    vector<Int_t>  SSIDs0;
    if (legacy) SSIDs0 = {6300004, 7100003, 6200004};
    else SSIDs0 = {6300004, 5600004, 5400004};

    // getSectorIndex
    SectorIndexData data;
    assert(reader->getSectorIndex(sectors.front(), data) == ES_OK);
    assert(data.nPatterns == nPatterns);
    assert(data.coverages == coverages);
    assert(data.firstDataEntry == 0);
    assert(data.SSIDs[0].size() == 69);
    for (size_t i = 0; i < SSIDs0.size(); i++)
        HTT_EQUAL_THROW(data.SSIDs[0][i], SSIDs0[i]);

    // readPatterns
    auto patterns = reader->readPatterns(sectors.front());
    HTT_EQUAL_THROW(patterns.size(), (size_t)std::accumulate(nPatterns.begin(), nPatterns.end(), 0));

    // firstDataEntry
    assert(reader->getSectorIndex(1, data) == ES_OK);
    assert(data.firstDataEntry == 10);

    // readPatterns, mincov
    patterns = reader->readPatterns(1, -1, 12);
    vector<pattern_t> patt_exp = {
        {7100081, 5006120, 5806320, 5806320, 6006320, 5906320, 4506420, 4506420},
        {4800081, 3306120, 3906320, 3906320, 4306320, 4206320, 3806420, 3806420}
    };
    assert(patterns == patt_exp);
}

void test_read()
{
    ToolHandle<HTTIndexPattTool_ReaderI> reader("HTTIndexPattTool_Reader");
    if (reader.retrieve().isFailure()) throw "Couldn't retrieve reader";
    test_read_base(dynamic_cast<HTTIndexPattTool_ReaderI*>(reader.get()), false);
    cout << "test_read() passed" << endl;
}

void test_read_legacy()
{
    ToolHandle<HTTIndexPattTool_ReaderI> reader("HTTIndexPattTool_ReaderLegacy");
    if (reader.retrieve().isFailure()) throw "Couldn't retrieve reader";
    test_read_base(dynamic_cast<HTTIndexPattTool_ReaderI*>(reader.get()), true);
    cout << "test_read_legacy() passed" << endl;
}

void test_write()
{
    ToolHandle<HTTIndexPattTool_ReaderI> reader("HTTIndexPattTool_Reader");
    ToolHandle<HTTIndexPattTool_Writer> writer("HTTIndexPattTool_Writer");

    auto sectors = reader->getSectors();
    for (size_t i = 0; i < sectors.size() && i < 10; i++)
    {
        vector<size_t> coverages;
        auto patterns = reader->readPatterns(sectors[i], -1, 0, &coverages);
        for (size_t i = 0; i < patterns.size(); i++)
            writer->addPattern(sectors[i], patterns[i], coverages[i]);
    }
    writer->write();

    cout << "test_write() passed" << endl;
    //remove(outfile.c_str());
}

int main(int, char**)
{
    ISvcLocator* pSvcLoc;
    if (!Athena_test::initGaudi("HTTIndexPattIO_test_config.txt", pSvcLoc))
    {
        std::cerr << "Gaudi failed to initialize. Test failing." << std::endl;
        return 1;
    }

    test_read();
    test_read_legacy();
    test_write();

    return 0;
}
