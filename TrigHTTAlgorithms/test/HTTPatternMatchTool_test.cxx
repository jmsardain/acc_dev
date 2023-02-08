/**
 * @file HTTPatternMatchTool_test.cxx
 * @brief Unit tests for HTTPatternMatchTool
 * @author Riley Xu - riley.xu@cern.ch
 * @date 2019-11-15
 */

#undef NDEBUG
#include <cassert>
#include <string>
#include <iostream>
#include <numeric>

#include "TestTools/initGaudi.h"
#include "TrigHTTAlgorithms/HTTPatternMatchTool.h"

using namespace std;


void test()
{
    ToolHandle<HTTPatternMatchTool> pm("HTTPatternMatchTool");
    if (pm.retrieve().isFailure()) throw "Couldn't retrieve PM";

    // Not sure how to unit test effectively :(
    // Need HTTHits
}


int main(int, char**)
{
    ISvcLocator* pSvcLoc;
    if (!Athena_test::initGaudi("HTTPatternMatchTool_test_config.txt", pSvcLoc))
    {
        std::cerr << "Gaudi failed to initialize. Test failing." << std::endl;
        return 1;
    }

    test();

    return 0;
}
