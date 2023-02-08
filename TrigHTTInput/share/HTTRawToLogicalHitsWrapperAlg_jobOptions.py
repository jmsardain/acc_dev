###############################################################
#
# TrigHTTWrapper job options file
#
#==============================================================


from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

from PyJobTransformsCore.runargs import RunArguments
runArgs = RunArguments()

from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.Constants import DEBUG

import os
import glob

#--------------------------------------------------------------
# HTT Includes
#--------------------------------------------------------------

from TrigHTTConfig.formatMessageSvc import formatMessageSvc
formatMessageSvc()

import TrigHTTConfig.HTTTagConfig as HTTTagConfig
import TrigHTTMaps.HTTMapConfig as HTTMapConfig

tags = HTTTagConfig.getTags(stage='map')
map_tag = tags['map']
MapSvc = HTTMapConfig.addMapSvc(map_tag)

#--------------------------------------------------------------
# Arguments
#--------------------------------------------------------------

InputHTTRawHitFile = []
if 'InputHTTRawHitFile' in os.environ:
    for ex in os.environ['InputHTTRawHitFile'].split(','):
        files = glob.glob(ex)
        if files:
            InputHTTRawHitFile += files
        else:
            InputHTTRawHitFile += [ex]
else:
    InputHTTRawHitFile = ["httsim_rawhits_wrap.root"]

 #output
OutputHTTRawHitFile = []
if 'OutputHTTRawHitFile' in os.environ :
     for ex in os.environ['OutputHTTRawHitFile'].split(','):
        files = glob.glob(ex)
        if files:
            OutputHTTRawHitFile += files
        else:
            OutputHTTRawHitFile += [ex]
else :
     OutputHTTRawHitFile = ["httsim_rawhits_wrap.OUT.root"]
print "OutHTTSim file",  OutputHTTRawHitFile



#--------------------------------------------------------------
# Create the components
#--------------------------------------------------------------

from TrigHTTInput.TrigHTTInputConf import HTTInputHeaderTool, HTTOutputHeaderTool
HTTReadInput = HTTInputHeaderTool(OutputLevel = DEBUG)
HTTReadInput.InFileName=InputHTTRawHitFile
HTTReadInput.RWstatus="READ"
#HTTReadInput.ReadTruthTracks = True
ToolSvc += HTTReadInput

HTTWriteOutput = HTTOutputHeaderTool("HTTWriteOutput", OutputLevel = DEBUG)
HTTWriteOutput.InFileName=OutputHTTRawHitFile
HTTWriteOutput.RWstatus="RECREATE"
print HTTWriteOutput
ToolSvc += HTTWriteOutput



from TrigHTTInput.TrigHTTInputConf import HTT_RawToLogicalHitsTool
HTTRawLogic = HTT_RawToLogicalHitsTool(OutputLevel = DEBUG)
HTTRawLogic.SaveOptional = 2
ToolSvc += HTTRawLogic



from TrigHTTInput.TrigHTTInputConf import HTTLogicalHitsWrapperAlg
wrapper = HTTLogicalHitsWrapperAlg()
wrapper.OutputLevel = DEBUG

wrapper.InputTool = HTTReadInput
wrapper.OutputTool = HTTWriteOutput
wrapper.RawToLogicalHitsTool = HTTRawLogic
wrapper.Clustering = True
wrapper.Monitor = True
theJob += wrapper

print theJob
###############################################################
