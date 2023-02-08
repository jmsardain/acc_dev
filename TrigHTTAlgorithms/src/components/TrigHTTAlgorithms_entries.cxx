#include "GaudiKernel/DeclareFactoryEntries.h"

#include "TrigHTTAlgorithms/HTTDataFlowTool.h"
#include "TrigHTTAlgorithms/HTTEtaPatternFilterTool.h"
#include "TrigHTTAlgorithms/HTTExtrapolator.h"
#include "TrigHTTAlgorithms/HTTHough1DShiftTool.h"
#include "TrigHTTAlgorithms/HTTHoughMonitorTool.h"
#include "TrigHTTAlgorithms/HTTLLPHoughMonitorTool.h"
#include "TrigHTTAlgorithms/HTTHoughRootOutputTool.h"
#include "TrigHTTAlgorithms/HTTHoughTransform_d0phi0_Tool.h"
#include "TrigHTTAlgorithms/HTTHoughTransformTool.h"
#include "TrigHTTAlgorithms/HTTLLPDoubletHoughTransformTool.h"
#include "TrigHTTAlgorithms/HTTLLPRoadFilterTool.h"
#include "TrigHTTAlgorithms/HTTLogicalHitsProcessAlg.h"
#include "TrigHTTAlgorithms/HTTMapMakerAlg.h"
#include "TrigHTTAlgorithms/HTTNNTrackTool.h"
#include "TrigHTTAlgorithms/HTTOutputMonitorAlg.h"
#include "TrigHTTAlgorithms/HTTOverlapRemovalTool.h"
#include "TrigHTTAlgorithms/HTTOverlapRemovalValAlg.h"
#include "TrigHTTAlgorithms/HTTPatternMatchTool.h"
#include "TrigHTTAlgorithms/HTTPhiRoadFilterTool.h"
#include "TrigHTTAlgorithms/HTTRoadUnionTool.h"
#include "TrigHTTAlgorithms/HTTSectorMatchTool.h"
#include "TrigHTTAlgorithms/HTTTrackFitterTool.h"

DECLARE_COMPONENT( HTTDataFlowTool )
DECLARE_COMPONENT( HTTEtaPatternFilterTool )
DECLARE_COMPONENT( HTTExtrapolator )
DECLARE_COMPONENT( HTTHough1DShiftTool )
DECLARE_COMPONENT( HTTHoughMonitorTool )
DECLARE_COMPONENT( HTTLLPHoughMonitorTool )
DECLARE_COMPONENT( HTTHoughRootOutputTool )
DECLARE_COMPONENT( HTTHoughTransform_d0phi0_Tool )
DECLARE_COMPONENT( HTTHoughTransformTool )
DECLARE_COMPONENT( HTTLLPDoubletHoughTransformTool )
DECLARE_COMPONENT( HTTLLPRoadFilterTool )
DECLARE_COMPONENT( HTTLogicalHitsProcessAlg )
DECLARE_COMPONENT( HTTMapMakerAlg )
DECLARE_COMPONENT( HTTNNTrackTool )
DECLARE_COMPONENT( HTTOutputMonitorAlg )
DECLARE_COMPONENT( HTTOverlapRemovalTool )
DECLARE_COMPONENT( HTTOverlapRemovalValAlg )
DECLARE_COMPONENT( HTTPatternMatchTool )
DECLARE_COMPONENT( HTTPhiRoadFilterTool )
DECLARE_COMPONENT( HTTRoadUnionTool )
DECLARE_COMPONENT( HTTSectorMatchTool )
DECLARE_COMPONENT( HTTTrackFitterTool )

DECLARE_FACTORY_ENTRIES( TrigHTTAlgorithms )
{
    DECLARE_ALGORITHM( HTTLogicalHitsProcessAlg );
    DECLARE_ALGORITHM( HTTMapMakerAlg );
    DECLARE_ALGORITHM( HTTOutputMonitorAlg );
}
