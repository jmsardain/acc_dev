#--------------------------------------------------------------
# Headers and setup
#--------------------------------------------------------------
from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doSemiDetailedMonitoring = True

from AthenaCommon.AlgSequence import AlgSequence
alg = AlgSequence()

from AthenaCommon.AppMgr import theApp, ServiceMgr, ToolSvc
theApp.EvtMax = 1  # This is hard-coded to 1 since this algorithm doesn't use events

import os

#-----------------------------------------------------------------------------
# HTT Includes
#-----------------------------------------------------------------------------

from TrigHTTConfig.formatMessageSvc import HTT_OutputLevel, formatMessageSvc
formatMessageSvc()

import TrigHTTConfig.HTTTagConfig as HTTTagConfig
from TrigHTTConfig.parseRunArgs import parseHTTArgs
import TrigHTTMaps.HTTMapConfig as HTTMapConfig

tags = HTTTagConfig.getTags(stage='bank', options=parseHTTArgs(runArgs))
map_tag = tags['map']
bank_tag = tags['bank']
MapSvc = HTTMapConfig.addMapSvc(map_tag)

from TrigHTTBanks.TrigHTTBanksConf import HTTCCachePattIO, HTTIndexPattTool_Reader
from TrigHTTBankGen.TrigHTTBankGenConf import HTTPattGenDCAlgo

#--------------------------------------------------------------
# Create the CCache svc
#--------------------------------------------------------------

ServiceMgr += HTTCCachePattIO()

#--------------------------------------------------------------
# Create the reader tool
#--------------------------------------------------------------

reader = HTTIndexPattTool_Reader()
reader.bank_path = runArgs.inbank
ToolSvc += reader

#--------------------------------------------------------------
# Create the algorithm
#--------------------------------------------------------------

import TrigHTTUtils.utils as utils
import TrigHTTBankGen.HTTBankGenConfig as HTTBankGenConfig

def default_name():
    base = os.path.basename(runArgs.inbank)
    if base.startswith('index.'):
        base = base[6:]
    if base.endswith('.root'):
        base = base[:len(base) - 5]

    config = str(getattr(runArgs, 'nDCMax', '')) + 'dc'
    for x in runArgs.nDC:
        config += str(x)
    if hasattr(runArgs, 'maxPatts'):
        config += '_n' + utils.human_readable(runArgs.maxPatts)

    return 'ccache.' + base + '.' + config + '.root'


PattGenDC = HTTPattGenDCAlgo()
HTTBankGenConfig.applyTag(PattGenDC, bank_tag)
PattGenDC.OutputLevel = HTT_OutputLevel
PattGenDC.description = 'tsp bank: ' + runArgs.inbank + '\n' + HTTTagConfig.getDescription(map_tag, bank_tag)
PattGenDC.outbank = default_name()

algArgs = [
    'outbank', # str
    'dumpFile', # str
]

for arg in algArgs:
    if hasattr(runArgs, arg):
        attr = getattr(runArgs, arg)
        setattr(PattGenDC, arg, attr)

alg += PattGenDC

