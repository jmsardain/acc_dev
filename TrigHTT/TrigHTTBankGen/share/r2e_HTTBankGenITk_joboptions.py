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

region=os.getenv("REGION","NOTHING")
if (region=="NOTHING"):
  print("HTT: Region environment variable wasn't set, setting it to 0")
  region='0'
region=os.environ['REGION']
print "HTT bank gen, Running on region ", region

tags = HTTTagConfig.getTags(stage='bank', options={'region': region})
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
HTTMatrixGen.IdealiseGeometry = 2;

HTTMatrixGen.PT_THRESHOLD = 1.0; # GeV
HTTMatrixGen.D0_THRESHOLD = 2.0; # 0.1 mm max

HTTMatrixGen.TRAIN_PDG = 13; # Traing samples particle ID
HTTMatrixGen.NBanks = 1

HTTBankGenConfig.applyTag(HTTMatrixGen, bank_tag, map_tag)


alg += HTTMatrixGen
