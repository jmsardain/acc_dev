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
httLog = logging.getLogger('HTTConstantGen')

# force the maximum number of events to 0
theApp.EvtMax = 0 # -1 means all events

from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()

import glob

#-----------------------------------------------------------------------------
# HTT Includes
#-----------------------------------------------------------------------------

from TrigHTTConfig.formatMessageSvc import HTT_OutputLevel, formatMessageSvc
formatMessageSvc()

from TrigHTTConfig.parseRunArgs import parseHTTArgs
import TrigHTTConfig.HTTTagConfig as HTTTagConfig
import TrigHTTMaps.HTTMapConfig as HTTMapConfig
import TrigHTTConfig.HTTConfigCompInit as HTTConfig


tags = HTTTagConfig.getTags(stage='bank', options=parseHTTArgs(runArgs))
map_tag = tags['map']
bank_tag = tags['bank']
MapSvc = HTTMapConfig.addMapSvc(map_tag)

#-----------------------------------------------------------------------------
# Algorithms
#-----------------------------------------------------------------------------

from TrigHTTBankGen.TrigHTTBankGenConf import HTTConstGenAlgo
algo = HTTConstGenAlgo()
algo.OutputLevel = HTT_OutputLevel
algo.region = HTTConfig.getRegionIndex(map_tag)
algo.missHitsConsts = bank_tag['missHitsConsts']
algo.Monitor = True

if not hasattr(runArgs, "CheckGood2ndStage"):
    if hasattr(runArgs, "outputHTTGoodMatrixReducedIgnoreFile"):
        setattr(runArgs, "CheckGood2ndStage", 0)
    else:
        setattr(runArgs, "CheckGood2ndStage", 1)

HTTConstGenAlgo.CheckGood2ndStage = runArgs.CheckGood2ndStage

if hasattr(runArgs, "DumpHardwareConstants"):
   HTTConstGenAlgo.DumpHardwareConstants = runAargs.DumpHardwareConstants
else:
   HTTConstGenAlgo.DumpHardwareConstants = True

if hasattr(runArgs, "DumpMissingHitsConstants"):
   HTTConstGenAlgo.DumpMissingHitsConstants = runArgs.DumpMissingHitsConstants
else:
   HTTConstGenAlgo.DumpMissingHitsConstants = False

if not hasattr(runArgs, "region") :
    setattr(runArgs, "region", 0)
HTTConstGenAlgo.region=int(runArgs.region) # if the previous option is True this line is not important

if hasattr(runArgs, "skip_sectors"):
    algo.skip_sectors = runArgs.skip_sectors

if hasattr(runArgs, "inputHTTMatrixFile") and hasattr(runArgs, "inputHTTMatrixReducedFile"):
    print("WARNING, Found inputHTTMatrixFile and inputHTTMatrixReducedFile. Will only use the reduced one")
    algo.merged_file_path=runArgs.inputHTTMatrixReducedFile[0]
elif hasattr(runArgs, "inputHTTMatrixFile"):
    algo.merged_file_path=runArgs.inputHTTMatrixFile[0]
elif hasattr(runArgs, "inputHTTMatrixReducedFile"):
    algo.merged_file_path=runArgs.inputHTTMatrixReducedFile[0]
else:
    print("WARNING, NO INPUT FILES SET")

if (hasattr(runArgs, "outputHTTGoodMatrixFile")):
    ServiceMgr.THistSvc.Output += ["TRIGHTTTREEGOODOUT DATAFILE='"+runArgs.outputHTTGoodMatrixFile+"' OPT='RECREATE'"]
    ServiceMgr.THistSvc.Output += ["TRIGHTTCTREEOUT DATAFILE='good_const.root' OPT='RECREATE'"]
    algo.UseHitScaleFactor = True
elif (hasattr(runArgs, "outputHTTGoodMatrixReducedIgnoreFile")):
    ServiceMgr.THistSvc.Output += ["TRIGHTTTREEGOODOUT DATAFILE='"+runArgs.outputHTTGoodMatrixReducedIgnoreFile+"' OPT='RECREATE'"]
    ServiceMgr.THistSvc.Output += ["TRIGHTTCTREEOUT DATAFILE='good_const_reduced_ignore.root' OPT='RECREATE'"]
    algo.UseHitScaleFactor = False
elif (hasattr(runArgs, "outputHTTGoodMatrixReducedCheckFile")):
    ServiceMgr.THistSvc.Output += ["TRIGHTTTREEGOODOUT DATAFILE='"+runArgs.outputHTTGoodMatrixReducedCheckFile+"' OPT='RECREATE'"]
    ServiceMgr.THistSvc.Output += ["TRIGHTTCTREEOUT DATAFILE='good_const_reduced_check.root' OPT='RECREATE'"]
    algo.UseHitScaleFactor = False
else:
    print("WARNING, NO OUTPUT FILES SET")
    algo.UseHitScaleFactor = False


job += algo
#-----------------------------------------------------------------------------
