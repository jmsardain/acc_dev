#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../TrigHTTBankSvc.h"
#include "TrigHTTBanks/HTTIndexPattTool_Reader.h"
#include "TrigHTTBanks/HTTIndexPattTool_Writer.h"
#include "TrigHTTBanks/HTTIndexPattConvertAlgo.h"
#include "TrigHTTBanks/HTTIndexPattMergeAlgo.h"
#include "TrigHTTBanks/HTTCCachePattIO.h"


DECLARE_COMPONENT( HTTIndexPattTool_Reader )
DECLARE_COMPONENT( HTTIndexPattTool_ReaderLegacy )
DECLARE_COMPONENT( HTTIndexPattTool_Writer )
DECLARE_COMPONENT( HTTIndexPattConvertAlgo )
DECLARE_COMPONENT( HTTIndexPattMergeAlgo )
DECLARE_COMPONENT( HTTCCachePattIO )

DECLARE_SERVICE_FACTORY(TrigHTTBankSvc)
