/**
 * @file HTTSSMap_test.cxx
 * @brief Unit tests for HTTSSMap
 * @author Riley Xu - rixu@cern.ch
 * @date 2020-01-15
 */

#undef NDEBUG
#include <cassert>
#include <string>
#include <iostream>
#include <numeric>

#include "TestTools/initGaudi.h"
#include "AthenaKernel/getMessageSvc.h"
#include "TrigHTTMaps/HTTSSMap.h"

using namespace std;

static const string pmap_path("../htt_configuration/map_files/step3_01eta03_03phi05.pmap");
static const string rmap_path("../htt_configuration/map_files/rmaps/eta0103phi0305_ATLAS-P2-ITK-23-00-01.rmap");
static const string ssmap_path("../htt_configuration/ss_files/HTT_TestBankGenV2.ss");

void test(HTTSSMap & ssmap)
{
    // Hard-coded values from step3_01eta03_03phi05.pmap and HTT_TestBankGen.ss.
    // Make sure to change these if those files are changed
    assert(ssmap.getSSPhiWidth(0, 0) == 33);
    //assert(ssmap.getModulePhiWidth(0, 0) == 773);
    assert(ssmap.getSSEtaWidth(0, 0) == 402);
    //assert(ssmap.getModuleEtaWidth(0, 0) == 805);
    /*
    for (size_t layer = 1; layer < 8; layer++)
    {
        assert(ssmap.getSSPhiWidth(layer, 0) == 40);
        assert(ssmap.getModulePhiWidth(layer, 0) == 1280);
    }
    */
}


int main(int, char**)
{
    ISvcLocator* pSvcLoc;
    if (!Athena_test::initGaudi(pSvcLoc))
    {
        std::cerr << "Gaudi failed to initialize. Test failing." << std::endl;
        return 1;
    }
    IMessageSvc* msgSvc = Athena::getMessageSvc();
    MsgStream pmsg(msgSvc, "HTTPlaneMap");
    MsgStream rmsg(msgSvc, "HTTRegionMap");
    MsgStream ssmsg(msgSvc, "HTTSSMap");
    pmsg.setLevel(MSG::WARNING);
    rmsg.setLevel(MSG::WARNING);
    ssmsg.setLevel(MSG::DEBUG);

    HTTPlaneMap pmap(pmsg, pmap_path, 0, 1);
    HTTRegionMap rmap(rmsg, &pmap, rmap_path);
    HTTSSMap ssmap(ssmsg, &rmap, ssmap_path);

    test(ssmap);

    return 0;
}
