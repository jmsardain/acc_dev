#include "TrigHTTBankGen/HTTMatrixGenAlgo.h"
#include "TrigHTTBankGen/HTTConstGenAlgo.h"
#include "TrigHTTBankGen/HTTMatrixReductionAlgo.h"
#include "TrigHTTBankGen/HTTMatrixMergeAlgo.h"
#include "TrigHTTBankGen/HTTSectorRemovalAlgo.h"
#include "TrigHTTBankGen/HTTPattGenAlgo.h"
#include "TrigHTTBankGen/HTTPattGenTool_Truth.h"
#include "TrigHTTBankGen/HTTPattGenTool_TI.h"
#include "TrigHTTBankGen/HTTPattGenDCAlgo.h"

DECLARE_COMPONENT( HTTConstGenAlgo )
DECLARE_COMPONENT( HTTMatrixMergeAlgo )
DECLARE_COMPONENT( HTTMatrixReductionAlgo )
DECLARE_COMPONENT( HTTSectorRemovalAlgo )
DECLARE_ALGORITHM_FACTORY( HTTMatrixGenAlgo )
DECLARE_ALGORITHM_FACTORY( HTTPattGenAlgo )
DECLARE_COMPONENT( HTTPattGenTool_Truth )
DECLARE_COMPONENT( HTTPattGenTool_TI )
DECLARE_COMPONENT( HTTPattGenDCAlgo )
