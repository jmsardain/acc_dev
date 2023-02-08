###############################################################
#
# TrigHTTWrapper job options file
#
#==============================================================


from TrigHTTConfig.formatMessageSvc import formatMessageSvc
formatMessageSvc()

import TrigHTTConfig.HTTTagConfig as HTTTagConfig
import TrigHTTMaps.HTTMapConfig as HTTMapConfig

tags = HTTTagConfig.getTags(stage='map')
map_tag = tags['map']
MapSvc = HTTMapConfig.addMapSvc(map_tag)

from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

from AthenaCommon.Constants import DEBUG

import glob
import os

#input
InputHTTLogHitFile = []
if 'InputHTTLogHitFile' in os.environ:
    for ex in os.environ['InputHTTLogHitFile'].split(','):
        files = glob.glob(ex)
        if files:
            InputHTTLogHitFile += files
        else:
            InputHTTLogHitFile += [ex]
else :
    InputHTTLogHitFile = ["httsim_loghits_wrap.OUT.root"]
print "Input file",  InputHTTLogHitFile


from TrigHTTInput.TrigHTTInputConf import HTTOutputHeaderTool
HTTReadInput = HTTOutputHeaderTool("HTTReadInput", OutputLevel = DEBUG)
HTTReadInput.InFileName=InputHTTLogHitFile
HTTReadInput.RWstatus="READ"
print HTTReadInput
ToolSvc += HTTReadInput

from TrigHTTInput.TrigHTTInputConf import HTTReadLogicalHitsAlg
wrapper = HTTReadLogicalHitsAlg(OutputLevel = DEBUG)
wrapper.InputTool  = HTTReadInput

print wrapper
theJob += wrapper

print theJob
###############################################################
