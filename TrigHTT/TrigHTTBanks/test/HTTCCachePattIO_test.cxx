/**
 * @file HTTCCachePattIO_test.cxx
 * @author Riley Xu - rixu@cern.ch
 * @date 2019-06-04
 * @brief Unit tests for HTTCCachePattIO
 */


#undef NDEBUG
#include <cassert>
#include <string>
#include <iostream>

#include "TestTools/initGaudi.h"
#include "AthenaKernel/getMessageSvc.h"
#include "TrigHTTUtils/HTTMacros.h"
#include "TrigHTTBanks/HTTPatternBank.h"
#include "TrigHTTBanks/HTTCCachePattIO.h"

using namespace std;

string TESTBANKPATH_DC("root://eosuser.cern.ch//eos/atlas/atlascerngroupdisk/det-htt/HTTsim/ATLAS-P2-ITK-17-06-00/21.9.2/eta0103phi0305/PatternBanks/test/ccache.HTTSimExample.10k.DC21111122.root");
string TESTBANKPATH_NODC("root://eosuser.cern.ch//eos/atlas/atlascerngroupdisk/det-htt/HTTsim/ATLAS-P2-ITK-17-06-00/21.9.2/eta0103phi0305/PatternBanks/test/ccache.HTTSimExample.10k.noDC.root");
string TESTBANKPATH_NODC_LEGACY("root://eosuser.cern.ch//eos/atlas/atlascerngroupdisk/det-htt/HTTsim/ATLAS-P2-ITK-17-06-00/21.9.2/eta0103phi0305/PatternBanks/test/ccache.HTTSimExample.10k.noDC.legacy.root");

void test_read_ccache_nodc(ServiceHandle<HTTCCachePattIO> &io)
{
    HTTPatternBank bank = io->read(TESTBANKPATH_NODC);

    assert(!bank.getMetadata().hasDC);
    assert(bank.getMetadata().nPatterns == 10001);
    assert(bank.getNLayers() == 8);
}

void test_read_ccache_dc(ServiceHandle<HTTCCachePattIO> &io)
{
    HTTPatternBank bank = io->read(TESTBANKPATH_DC);

    assert(bank.getMetadata().hasDC);
    assert(bank.getMetadata().nPatterns == 10001);
    assert(bank.getNLayers() == 8);
}

void test_read_ccache_legacy(ServiceHandle<HTTCCachePattIO> &io)
{
    HTTPatternBank bank = io->read(TESTBANKPATH_NODC_LEGACY, true);

    assert(!bank.getMetadata().hasDC);
    assert(bank.getMetadata().nPatterns == 10001);
    assert(bank.getNLayers() == 8);
}

void test_write_ccache(ServiceHandle<HTTCCachePattIO> &io)
{
    string outfile("temp.TrigHTTCCachePattBankIOTest_TestWriteCCache.root");

    HTTPatternBank bank = io->read(TESTBANKPATH_DC);

    int err = io->write(outfile, bank);
    assert(!err);

    io->read(outfile);

    remove(outfile.c_str());
}

int main(int, char**)
{
    ISvcLocator* pSvcLoc;
    if (!Athena_test::initGaudi(pSvcLoc))
    {
        std::cerr << "Gaudi failed to initialize. Test failing." << std::endl;
        return 1;
    }

    //MsgStream msg(Athena::getMessageSvc(), "CCache_test");
    ServiceHandle<HTTCCachePattIO> io("HTTCCachePattIO", "HTTCCachePattIO_test");
    if (io.retrieve().isFailure()) return 1;

    test_read_ccache_nodc(io);
    test_read_ccache_dc(io);
    test_read_ccache_legacy(io);
    test_write_ccache(io);

    return 0;
}
