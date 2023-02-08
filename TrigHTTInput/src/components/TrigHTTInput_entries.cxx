#include "GaudiKernel/DeclareFactoryEntries.h"

#include "TrigHTTInput/HTTRawHitsWrapperAlg.h"
#include "TrigHTTInput/HTTRawNtupleWrapperAlg.h"
#include "TrigHTTInput/HTTReadLogicalHitsAlg.h"
#include "TrigHTTInput/HTTLogicalHitsWrapperAlg.h"

#include "TrigHTTInput/HTTSGToRawHitsTool.h"
#include "TrigHTTInput/HTT_ReadRawRandomHitsTool.h"
#include "TrigHTTInput/HTTInputHeaderTool.h"
#include "TrigHTTInput/HTT_RawToLogicalHitsTool.h"
#include "TrigHTTInput/HTTL1SimToRawHitTool.h"
#include "TrigHTTInput/HTT_RandRawHitsFromInvConstsTool.h"
#include "TrigHTTInput/HTTDetectorTool.h"
#include "TrigHTTInput/HTTDumpCondAlgo.h"
#include "TrigHTTInput/HTTOutputHeaderTool.h"
#include "../HTTDumpOutputStatAlg.h"


DECLARE_COMPONENT( HTTDetectorTool )
DECLARE_COMPONENT( HTTSGToRawHitsTool )
DECLARE_COMPONENT( HTT_RandRawHitsFromInvConstsTool )
DECLARE_COMPONENT( HTT_ReadRawRandomHitsTool )
DECLARE_COMPONENT( HTTInputHeaderTool )
DECLARE_COMPONENT( HTT_RawToLogicalHitsTool )
DECLARE_COMPONENT( HTTL1SimToRawHitTool )
DECLARE_COMPONENT( HTTOutputHeaderTool )

DECLARE_COMPONENT( HTTDumpCondAlgo )
DECLARE_COMPONENT( HTTRawHitsWrapperAlg )
DECLARE_COMPONENT( HTTRawNtupleWrapperAlg )
DECLARE_COMPONENT( HTTReadLogicalHitsAlg)
DECLARE_COMPONENT( HTTLogicalHitsWrapperAlg)
DECLARE_COMPONENT( HTTDumpOutputStatAlg )

 DECLARE_FACTORY_ENTRIES( TrigHTTInput )
{
   DECLARE_ALGORITHM( HTTDumpCondAlgo );
   DECLARE_ALGORITHM( HTTRawHitsWrapperAlg );
   DECLARE_ALGORITHM( HTTRawNtupleWrapperAlg );
   DECLARE_ALGORITHM( HTTReadLogicalHitsAlg);
   DECLARE_ALGORITHM( HTTLogicalHitsWrapperAlg );
   DECLARE_ALGORITHM( HTTDumpOutputStatAlg );
}


