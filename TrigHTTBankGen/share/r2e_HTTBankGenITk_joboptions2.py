from AthenaCommon.AlgSequence import AlgSequence
alg = AlgSequence()

from GaudiSvc.GaudiSvcConf import THistSvc

from AthenaCommon.AppMgr import ToolSvc, ServiceMgr

from PyJobTransforms.trfUtils import findFile

#--------------------------------------------------------------
# HTT Includes
#--------------------------------------------------------------

from TrigHTTBankGen.TrigHTTBankGenConf import *
from TrigHTTInput.TrigHTTInputConf import HTT_SGToRawHitsTool
from TrigHTTMaps.TrigHTTMapsConf import HTTClusteringTool
from TrigHTTUtils.formatMessageSvc import HTT_OutputLevel, formatMessageSvc

formatMessageSvc()

import TrigHTTUtils.HTTTagConfig as HTTTagConfig
import TrigHTTMaps.HTTMapConfig as HTTMapConfig
import TrigHTTBankGen.HTTBankGenConfig as HTTBankGenConfig

### CHANGE THE REGION HERE
region = '0'
(map_tag, bank_tag, _) = HTTTagConfig.getTags('bank', {'region': region})
MapSvc = HTTMapConfig.addMapSvc(map_tag)

#--------------------------------------------------------------
# THistSvc
#--------------------------------------------------------------

MatrixFileName="matrix.root"
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += ["TRIGHTTMATRIXOUT DATAFILE='"+MatrixFileName+"' OPT='RECREATE'"]

#--------------------------------------------------------------
# Hits Tool
#--------------------------------------------------------------

HTT_SGToRawHitsTool = HTT_SGToRawHitsTool()
HTT_SGToRawHitsTool.OutputLevel      = DEBUG
HTT_SGToRawHitsTool.ReadTruthTracks  = True
HTT_SGToRawHitsTool.UseNominalOrigin = True
ToolSvc += HTT_SGToRawHitsTool

#--------------------------------------------------------------
# Clustering
#--------------------------------------------------------------

HTTClusteringTool = HTTClusteringTool()
HTTClusteringTool.OutputLevel = DEBUG
ToolSvc += HTTClusteringTool

#--------------------------------------------------------------
# Main Algo
#--------------------------------------------------------------

HTTMatrixGen = HTTMatrixGenAlgo()
HTTMatrixGen.OutputLevel = DEBUG
HTTMatrixGen.Clustering  = True;
HTTMatrixGen.IdealiseGeometry = 2;

HTTMatrixGen.PT_THRESHOLD = 1.0; # GeV
HTTMatrixGen.D0_THRESHOLD = 0.1; # 0.1 mm max

HTTMatrixGen.TRAIN_PDG = 13; # Traing samples particle ID
HTTMatrixGen.NBanks = 1

HTTBankGenConfig.applyTag(HTTMatrixGen, bank_tag, map_tag)


alg += HTTMatrixGen
