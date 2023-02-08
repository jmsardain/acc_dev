###############################################################
#
# TrigHTTWrapper job options file
#
#==============================================================

# Helper function from transforms
from TrigHTTConfig.formatMessageSvc import formatMessageSvc
formatMessageSvc()

import TrigHTTConfig.HTTTagConfig as HTTTagConfig
import TrigHTTMaps.HTTMapConfig as HTTMapConfig

tags = HTTTagConfig.getTags(stage='map')
map_tag = tags['map']
MapSvc = HTTMapConfig.addMapSvc(map_tag)

from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()



if hasattr(runArgs,"outputNTUP_HTTIPFile") :
    OutputNTUP_HTTIPFile = runArgs.outputNTUP_HTTIPFile
else :
    OutputNTUP_HTTIPFile = "httsim_rawntuple_wrap.root"
print "Output file", OutputNTUP_HTTIPFile

from AthenaCommon.AppMgr import ToolSvc
from TrigHTTInput.TrigHTTInputConf import HTTSGToRawHitsTool
HTTSGInput = HTTSGToRawHitsTool( maxEta= 3.2, minPt= 0.8*GeV)
HTTSGInput.OutputLevel = DEBUG
HTTSGInput.ReadTruthTracks = True
ToolSvc += HTTSGInput


from TrigHTTInput.TrigHTTInputConf import HTTRawNtupleWrapperAlg
wrapper = HTTRawNtupleWrapperAlg(OutputLevel = DEBUG,
                             OutFileName = OutputNTUP_HTTIPFile)

wrapper.InputTool = HTTSGInput
theJob += wrapper

print theJob
###############################################################
