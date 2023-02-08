from AthenaCommon.AlgSequence import AlgSequence
alg = AlgSequence()

from GaudiSvc.GaudiSvcConf import THistSvc

from AthenaCommon.AppMgr import ToolSvc, ServiceMgr

from PyJobTransforms.trfUtils import findFile

#--------------------------------------------------------------
# HTT Includes
#--------------------------------------------------------------

from TrigHTTAlgorithms.TrigHTTAlgorithmsConf import HTTHoughTransformTool, HTTRoadUnionTool
from TrigHTTBankGen.TrigHTTBankGenConf import *
from TrigHTTInput.TrigHTTInputConf import HTTSGToRawHitsTool
from TrigHTTMaps.TrigHTTMapsConf import HTTClusteringTool
from TrigHTTConfig.formatMessageSvc import HTT_OutputLevel, formatMessageSvc

formatMessageSvc()

import TrigHTTConfig.HTTConfigCompInit as HTTConfig
import TrigHTTConfig.HTTTagConfig as HTTTagConfig
import TrigHTTMaps.HTTMapConfig as HTTMapConfig
import TrigHTTBankGen.HTTBankGenConfig as HTTBankGenConfig


region='0'
print "HTT bank gen, Running on region ", region


tags = HTTTagConfig.getTags('algo', {'region': region, 'mapTag': 'EF_TaskForce_dev22'})
map_tag = tags['map']
bank_tag = tags['bank']
algo_tag = tags['algo']
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
HTTMatrixGen.IdealiseGeometry = 1;
HTTMatrixGen.SingleSector = False;
HTTMatrixGen.HoughConstants = True;

HTTMatrixGen.PT_THRESHOLD = 1.0; # GeV
HTTMatrixGen.D0_THRESHOLD = 2.0; # mm
HTTMatrixGen.TRAIN_PDG = 13; # Traing samples particle ID
HTTMatrixGen.NBanks = 1

union = HTTRoadUnionTool()

x_min = 0.3
x_max = 0.5

x_buffer = (x_max - x_min) / 216 * 6
x_min -= x_buffer
x_max += x_buffer

y_min = -1.0
y_max = 1.0

y_buffer = (y_max - y_min) / 216 * 2
y_min -= y_buffer
y_max += y_buffer

tools = []
nSlice = HTTMapConfig.getNSubregions(map_tag) if True else 1

d0_list = [0]

for d0 in d0_list:
    for iSlice in range(nSlice):
        t = HTTHoughTransformTool("HoughTransform_" + str(d0) + '_' + str(iSlice))

        t.subRegion = iSlice if nSlice > 1 else -1
        t.phi_min = x_min
        t.phi_max = x_max
        t.qpT_min = y_min
        t.qpT_max = y_max
        t.d0_min = d0
        t.d0_max = d0
        t.nBins_x = 216 + 2 * 6
        t.nBins_y = 216 + 2 * 2
        t.threshold = [7]
        t.convolution = []
        t.combine_layers = []
        t.scale = []
        t.convSize_x = 0
        t.convSize_y = 0
        t.traceHits = False
        t.localMaxWindowSize = 0
        t.fieldCorrection = True
        t.hitExtend_x = [2,1,0,0,0,0,0,0]

        tools.append(t)

union.tools = tools # NB don't manipulate union.tools directly; for some reason the attributes get unset. Only set like is done here

ToolSvc += union
HTTMatrixGen.RoadFinder = union

HTTBankGenConfig.applyTag(HTTMatrixGen, bank_tag, map_tag)

alg += HTTMatrixGen
