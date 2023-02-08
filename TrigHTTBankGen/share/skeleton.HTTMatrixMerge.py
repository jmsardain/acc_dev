#-----------------------------------------------------------------------------
# Athena imports
#-----------------------------------------------------------------------------
from AthenaCommon.Constants import *
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr
#import AthenaPoolCnvSvc.ReadAthenaPool

from PerfMonComps.PerfMonFlags import jobproperties as pmjp
pmjp.PerfMonFlags.doSemiDetailedMonitoring = True # to enable monitoring

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

from AthenaCommon.Logging import logging
httLog = logging.getLogger('HTTMatrixMerge')

# force the maximum number of events to 0
theApp.EvtMax = 0 # -1 means all events

from GaudiSvc.GaudiSvcConf import THistSvc

import glob

#-----------------------------------------------------------------------------
# HTT Includes
#-----------------------------------------------------------------------------

from TrigHTTConfig.formatMessageSvc import HTT_OutputLevel, formatMessageSvc
formatMessageSvc()

from TrigHTTConfig.parseRunArgs import parseHTTArgs
import TrigHTTConfig.HTTTagConfig as HTTTagConfig
import TrigHTTMaps.HTTMapConfig as HTTMapConfig


from TrigHTTBankGen.TrigHTTBankGenConf import HTTMatrixMergeAlgo

#-----------------------------------------------------------------------------
# Algorithms
#-----------------------------------------------------------------------------
HTTMatrixMergeAlgo = HTTMatrixMergeAlgo('HTTMatrixMergeAlgo', OutputLevel = INFO)

if not hasattr(runArgs, "NBanks"):
    setattr(runArgs, "NBanks", 1)
HTTMatrixMergeAlgo.nbank=runArgs.NBanks

if not hasattr(runArgs, "allregions") :
    setattr(runArgs, "allregions", False)
HTTMatrixMergeAlgo.allregion = runArgs.allregions

if not hasattr(runArgs, "bankregion") :
    setattr(runArgs, "bankregion", 0)
HTTMatrixMergeAlgo.region=runArgs.bankregion # if the previous option is True this line is not important

tags = HTTTagConfig.getTags(stage='algo', options=parseHTTArgs(runArgs))
map_tag = tags['map']
bank_tag = tags['bank']
algo_tag = tags['algo']


file_path=[]
if hasattr(runArgs,'HTTMatrixFileRegEx') :
    for expression in runArgs.HTTMatrixFileRegEx.split(','):
        if ('matrix' in expression and 'root' in expression):
          file_path += glob.glob(expression) ### so we don't pick up other root files

    if hasattr(runArgs,'MaxInputFiles'):
        HTTMatrixMergeAlgo.file_path = file_path[:runArgs.MaxInputFiles]
    else:
        HTTMatrixMergeAlgo.file_path = file_path
else :
    HTTMatrixMergeAlgo.file_path=runArgs.inputHTTMatrixFile

mergedName = "combined_matrix.root"
if hasattr(runArgs, 'outputMergedHTTMatrixFile'):
  mergedName = runArgs.outputMergedHTTMatrixFile

HTTMatrixMergeAlgo.Monitor = True

ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += ["TRIGHTTMATRIXOUT DATAFILE='"+mergedName+"' OPT='RECREATE'"]

job += HTTMatrixMergeAlgo
#-----------------------------------------------------------------------------
