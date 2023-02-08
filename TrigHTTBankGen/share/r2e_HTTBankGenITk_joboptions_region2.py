from AthenaCommon.AlgSequence import AlgSequence
alg = AlgSequence()

from GaudiSvc.GaudiSvcConf import THistSvc

from AthenaCommon.AppMgr import ToolSvc, ServiceMgr

from PyJobTransforms.trfUtils import findFile

#--------------------------------------------------------------
# HTT Includes
#--------------------------------------------------------------

from TrigHTTBankGen.TrigHTTBankGenConf import *
from TrigHTTInput.TrigHTTInputConf import HTTSGToRawHitsTool
from TrigHTTMaps.TrigHTTMapsConf import HTTClusteringTool
from TrigHTTConfig.formatMessageSvc import HTT_OutputLevel, formatMessageSvc

formatMessageSvc()

import TrigHTTConfig.HTTConfigCompInit as HTTConfig
import TrigHTTConfig.HTTTagConfig as HTTTagConfig
import TrigHTTMaps.HTTMapConfig as HTTMapConfig
import TrigHTTBankGen.HTTBankGenConfig as HTTBankGenConfig

region='2'
print "HTT bank gen, Running on region ", region

tags = HTTTagConfig.getTags('bank', {'region': region, 'mapTag': 'EF_TaskForce_dev22'})
map_tag = tags['map']
bank_tag = tags['bank']
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

HTTSGToRawHitsTool = HTTSGToRawHitsTool()
HTTSGToRawHitsTool.OutputLevel      = DEBUG
HTTSGToRawHitsTool.ReadTruthTracks  = True
HTTSGToRawHitsTool.UseNominalOrigin = True
ToolSvc += HTTSGToRawHitsTool

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

HTTMatrixGen.PT_THRESHOLD = 1.0; # GeV
HTTMatrixGen.TRAIN_PDG = 13; # Traing samples particle ID
HTTMatrixGen.NBanks = 1

HTTBankGenConfig.applyTag(HTTMatrixGen, bank_tag, map_tag)


alg += HTTMatrixGen
