###############################################################
#
# TrigHTTWrapper job options file
#
#==============================================================

from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()
from AthenaCommon.AppMgr import ToolSvc

#--------------------------------------------------------------
# HTT Includes
#--------------------------------------------------------------

from TrigHTTConfig.formatMessageSvc import formatMessageSvc
from TrigHTTInput.TrigHTTInputConf import HTTSGToRawHitsTool, HTT_RawToLogicalHitsTool, HTTLogicalHitsWrapperAlg

formatMessageSvc()

import TrigHTTConfig.HTTTagConfig as HTTTagConfig
import TrigHTTMaps.HTTMapConfig as HTTMapConfig

tags = HTTTagConfig.getTags(stage='map')
map_tag = tags['map']
MapSvc = HTTMapConfig.addMapSvc(map_tag)

#--------------------------------------------------------------
# Make Tools and Algs
#--------------------------------------------------------------


if hasattr(runArgs, "outputNTUP_HTTIPFile"):
    OutputNTUP_HTTIPFile = runArgs.outputNTUP_HTTIPFile
else:
    OutputNTUP_HTTIPFile = "httsim_logicalhits_wrap.root"


HTTSGInput = HTTSGToRawHitsTool(maxEta=3.2, minPt=0.8*GeV)
HTTSGInput.OutputLevel = DEBUG
HTTSGInput.ReadTruthTracks = True
ToolSvc += HTTSGInput


HTTRawLogic = HTT_RawToLogicalHitsTool()
HTTRawLogic.OutputLevel = DEBUG
HTTRawLogic.SaveOptional = 2
HTTRawLogic.TowersToMap = [1]
ToolSvc += HTTRawLogic


wrapper = HTTLogicalHitsWrapperAlg()
wrapper.OutputLevel = DEBUG
wrapper.OutFileName = OutputNTUP_HTTIPFile
wrapper.InputTool = HTTSGInput
wrapper.RawToLogicalHitsTool = HTTRawLogic
theJob += wrapper

###############################################################
