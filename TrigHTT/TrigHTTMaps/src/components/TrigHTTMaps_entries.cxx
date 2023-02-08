#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../TrigHTTMappingSvc.h"

DECLARE_SERVICE_FACTORY(TrigHTTMappingSvc)


#include "TrigHTTMaps/HTTSpacePointsTool.h"
#include "TrigHTTMaps/HTTSpacePointsTool_v2.h"
#include "TrigHTTMaps/HTTHitFilteringTool.h"
#include "TrigHTTMaps/HTTClusteringTool.h"
#include "TrigHTTMaps/HTTClusteringOfflineTool.h"

DECLARE_COMPONENT( HTTClusteringTool )
DECLARE_COMPONENT( HTTClusteringOfflineTool )
DECLARE_COMPONENT(HTTSpacePointsTool)
DECLARE_COMPONENT(HTTSpacePointsTool_v2)
DECLARE_COMPONENT(HTTHitFilteringTool)

// DECLARE_FACTORY_ENTRIES(TrigHTTMaps) {
//    DECLARE_SERVICE   ( TrigHTTMappingSvc )
// }

// for rel 22:
//DECLARE_COMPONENT(TrigHTTMappingSvc)
