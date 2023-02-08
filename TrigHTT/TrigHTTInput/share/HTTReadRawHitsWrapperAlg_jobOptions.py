###############################################################
#
# TrigHTTWrapper job options file
#
#==============================================================
import os
import glob

from TrigHTTConfig.formatMessageSvc import formatMessageSvc
formatMessageSvc()

import TrigHTTConfig.HTTTagConfig as HTTTagConfig
import TrigHTTMaps.HTTMapConfig as HTTMapConfig

tags = HTTTagConfig.getTags(stage='map')
map_tag = tags['map']
MapSvc = HTTMapConfig.addMapSvc(map_tag)

from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.Constants import DEBUG


#input
InputHTTRawHitFile = []
if 'InputHTTRawHitFile' in os.environ:
    for ex in os.environ['InputHTTRawHitFile'].split(','):
        files = glob.glob(ex)
        if files:
            InputHTTRawHitFile += files
        else:
            InputHTTRawHitFile += [ex]
else :
    InputHTTRawHitFile = ["httsim_rawhits_wrap.root"]
print "Input file",  InputHTTRawHitFile

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



from TrigHTTInput.TrigHTTInputConf import HTTInputHeaderTool
HTTReadInput = HTTInputHeaderTool("HTTReadInput", OutputLevel = DEBUG)
HTTReadInput.InFileName=InputHTTRawHitFile
HTTReadInput.RWstatus="READ"
#HTTReadInput.ReadTruthTracks = True
print HTTReadInput
ToolSvc += HTTReadInput

HTTWriteOutput = HTTInputHeaderTool("HTTWriteInput", OutputLevel = DEBUG)
HTTWriteOutput.InFileName=OutputHTTRawHitFile
HTTWriteOutput.RWstatus="RECREATE"
print HTTWriteOutput
ToolSvc += HTTWriteOutput

from TrigHTTInput.TrigHTTInputConf import HTTRawHitsWrapperAlg
wrapper = HTTRawHitsWrapperAlg(OutputLevel = DEBUG)

wrapper.InputTool = HTTReadInput
wrapper.OutputTool = HTTWriteOutput

theJob += wrapper

print theJob
###############################################################
