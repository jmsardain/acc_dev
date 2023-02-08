#--------------------------------------------------------------
# Headers and setup
#--------------------------------------------------------------
from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doSemiDetailedMonitoring = True

from AthenaCommon.AlgSequence import AlgSequence
alg = AlgSequence()

from AthenaCommon.AppMgr import theApp, ServiceMgr, ToolSvc
theApp.EvtMax = 1  # This is hard-coded to 1 since this algorithm doesn't use events

#-----------------------------------------------------------------------------
# HTT Includes
#-----------------------------------------------------------------------------

from TrigHTTConfig.formatMessageSvc import HTT_OutputLevel, formatMessageSvc
formatMessageSvc()

import TrigHTTConfig.HTTTagConfig as HTTTagConfig
import TrigHTTMaps.HTTMapConfig as HTTMapConfig

tags = HTTTagConfig.getTags(stage='bank')
map_tag = tags['map']
bank_tag = tags['bank']
MapSvc = HTTMapConfig.addMapSvc(map_tag)

from TrigHTTBankGen.TrigHTTBankGenConf import HTTSectorRemovalAlgo

#--------------------------------------------------------------
# Create the algorithm
#--------------------------------------------------------------

SectorRemoval = HTTSectorRemovalAlgo()
SectorRemoval.OutputLevel = HTT_OutputLevel

algArgs = [
    'input',
    'method',
]
for arg in algArgs:
    if hasattr(runArgs, arg):
        attr = getattr(runArgs, arg)
        setattr(SectorRemoval, arg, attr)


alg += SectorRemoval

