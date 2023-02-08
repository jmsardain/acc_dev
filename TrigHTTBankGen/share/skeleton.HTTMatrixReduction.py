#-----------------------------------------------------------------------------
# Athena imports
#-----------------------------------------------------------------------------
from AthenaCommon.Constants import *
from AthenaCommon.AppMgr import theApp, ServiceMgr
#import AthenaPoolCnvSvc.ReadAthenaPool

from PerfMonComps.PerfMonFlags import jobproperties as pmjp
pmjp.PerfMonFlags.doSemiDetailedMonitoring = True # to enable monitoring

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

from AthenaCommon.Logging import logging
httLog = logging.getLogger('HTTMatrixReduction')

from PyJobTransforms.trfUtils import findFile

# force the maximum number of events to 0
theApp.EvtMax = 0 # -1 means all events

from GaudiSvc.GaudiSvcConf import THistSvc

import glob

#-----------------------------------------------------------------------------
# HTT Includes
#-----------------------------------------------------------------------------

from TrigHTTConfig.formatMessageSvc import HTT_OutputLevel, formatMessageSvc
formatMessageSvc()

import TrigHTTConfig.HTTTagConfig as HTTTagConfig
import TrigHTTMaps.HTTMapConfig as HTTMapConfig
from TrigHTTConfig.parseRunArgs import parseHTTArgs

tags = HTTTagConfig.getTags(stage='bank', options=parseHTTArgs(runArgs))
map_tag = tags['map']
bank_tag = tags['bank']
MapSvc = HTTMapConfig.addMapSvc(map_tag)

from TrigHTTBankGen.TrigHTTBankGenConf import HTTMatrixReductionAlgo

#-----------------------------------------------------------------------------
# Algorithms
#-----------------------------------------------------------------------------

HTTMatrixReductionAlgo = HTTMatrixReductionAlgo('HTTMatrixReductionAlgo', OutputLevel = INFO)

HTTMatrixReductionAlgo.nbank=runArgs.NBanks

if not hasattr(runArgs, "allregions") :
    setattr(runArgs, "allregions", False)
HTTMatrixReductionAlgo.allregion = runArgs.allregions

if not hasattr(runArgs, "region") :
    setattr(runArgs, "region", 0)
HTTMatrixReductionAlgo.region = int(runArgs.region) # if the previous option is True this line is not important



if (hasattr(runArgs, "inputHTTGoodMatrixFile")):
    HTTMatrixReductionAlgo.LayerChange_path = runArgs.inputHTTGoodMatrixFile[0]
elif (hasattr(runArgs, "inputHTTMatrixFile")):
    HTTMatrixReductionAlgo.LayerChange_path = runArgs.inputHTTMatrixFile[0]
else:
    print "Warning, did not find any input file to use for reduction!"

outputHTTMatrixReducedFile = "good_matrix_reduced.root"
if (hasattr(runArgs, "outputHTTMatrixReducedFile")):
  outputHTTMatrixReducedFile = runArgs.outputHTTMatrixReducedFile



ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += ["TRIGHTTMATRIX1STSTAGEOUT DATAFILE='"+outputHTTMatrixReducedFile+"' OPT='RECREATE'"]

job += HTTMatrixReductionAlgo
#-----------------------------------------------------------------------------
