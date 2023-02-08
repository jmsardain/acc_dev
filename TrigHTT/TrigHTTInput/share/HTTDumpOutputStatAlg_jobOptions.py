###############################################################
#
# HTTDumpOutputStatAlg job options file
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
HTTInputFile = []
if 'HTTInputFile' in os.environ:
    for ex in os.environ['HTTInputFile'].split(','):
        files = glob.glob(ex)
        if files:
            HTTInputFile += files
        else:
            HTTInputFile += [ex]
else:
    HTTInputFile = ["httsim_input.root"]

print "Input file:  ", HTTInputFile

 #output
HTTOutputFile = []
if 'HTTOutputFile' in os.environ :
     for ex in os.environ['HTTOutputFile'].split(','):
        files = glob.glob(ex)
        if files:
            HTTOutputFile += files
        else:
            HTTOutputFile += [ex]
else :
     HTTOutputFile = ["httsim_output.root"]
print "Output file",  HTTOutputFile


from TrigHTTInput.TrigHTTInputConf import HTTOutputHeaderTool
HTTReadInput = HTTOutputHeaderTool("HTTReadInput", OutputLevel = DEBUG)
HTTReadInput.InFileName=HTTInputFile
HTTReadInput.RWstatus="READ"
print HTTReadInput
ToolSvc += HTTReadInput

HTTWriteOutput = HTTOutputHeaderTool("HTTWriteOutput", OutputLevel = DEBUG)
HTTWriteOutput.InFileName=HTTOutputFile
HTTWriteOutput.RWstatus="RECREATE"
print HTTWriteOutput
ToolSvc += HTTWriteOutput


from TrigHTTInput.TrigHTTInputConf import HTTDumpOutputStatAlg
theAlg = HTTDumpOutputStatAlg(OutputLevel = DEBUG)

theAlg.InputTool  = HTTReadInput
theAlg.OutputTool = HTTWriteOutput


theJob += theAlg
print theJob
###############################################################
