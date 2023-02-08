/**
 * @file HTTRoadUnionTool.cxx
 * @author Riley Xu - riley.xu@cern.ch
 * @date November 20th, 2020
 * @brief See header file.
 */


#include "TrigHTTAlgorithms/HTTRoadUnionTool.h"


HTTRoadUnionTool::HTTRoadUnionTool(const std::string& algname, const std::string &name, const IInterface *ifc) :
    AthAlgTool(algname, name, ifc),
    m_tools(this)
{
    declareInterface<HTTRoadFinderToolI>(this);
    declareProperty("tools", m_tools, "Array of HTTRoadFinderTools");
}


StatusCode HTTRoadUnionTool::initialize()
{
    // Retrieve
    ATH_MSG_INFO("Using " << m_tools.size() << " tools");
    ATH_CHECK(m_tools.retrieve());

    // Error checking
    bool ok = false;
    if (m_tools.empty())
        ATH_MSG_FATAL("initialize() Tool list empty");
    else
        ok = true;
    if (!ok) return StatusCode::FAILURE;

    return StatusCode::SUCCESS;
}


StatusCode HTTRoadUnionTool::finalize()
{
    return StatusCode::SUCCESS;
}


StatusCode HTTRoadUnionTool::getRoads(const std::vector<const HTTHit*> & hits, std::vector<HTTRoad*> & roads)
{

    roads.clear();
    for (auto const & tool : m_tools)
    {
        std::vector<HTTRoad*> r;
        ATH_CHECK(tool->getRoads(hits, r));
        roads.insert(roads.end(), std::make_move_iterator(r.begin()), std::make_move_iterator(r.end()));
    }

    return StatusCode::SUCCESS;
}
