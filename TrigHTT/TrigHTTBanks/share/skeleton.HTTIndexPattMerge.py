#--------------------------------------------------------------
# Headers and setup
#--------------------------------------------------------------
from AthenaCommon.Constants import *
from AthenaCommon.AppMgr import theApp

from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doSemiDetailedMonitoring = True

from AthenaCommon.AlgSequence import AlgSequence
alg = AlgSequence()

from AthenaCommon.AppMgr import ToolSvc

from TrigHTTBanks.TrigHTTBanksConf import HTTIndexPattTool_Reader, HTTIndexPattTool_Writer, HTTIndexPattMergeAlgo

from TrigHTTConfig.formatMessageSvc import HTT_OutputLevel, formatMessageSvc
formatMessageSvc()

import TrigHTTConfig.HTTTagConfig as HTTTagConfig
import TrigHTTMaps.HTTMapConfig as HTTMapConfig
tags = HTTTagConfig.getTags(stage='map')
map_tag = tags['map']
MapSvc = HTTMapConfig.addMapSvc(map_tag)

#--------------------------------------------------------------
# Parse arguments for Athena stuff
#--------------------------------------------------------------

# Number of events to be processed: hardcoded to 1
theApp.EvtMax = 1

#--------------------------------------------------------------
# Create the reader and writer tools
#--------------------------------------------------------------

from AthenaCommon.AppMgr import ToolSvc
import glob as glob

reader = HTTIndexPattTool_Reader()

files=[]
for expression in runArgs.inputs:
    files += glob.glob(expression)

if not files: # Glob will fail on eos paths
    files = runArgs.inputs

reader.bank_path = files[0]
ToolSvc += reader

writer = HTTIndexPattTool_Writer()
writer.bank_path = runArgs.output
ToolSvc += writer

#--------------------------------------------------------------
# Create the algorithm
#--------------------------------------------------------------

PattMerge = HTTIndexPattMergeAlgo()
PattMerge.OutputLevel = INFO
PattMerge.inputs = files
alg += PattMerge

#==============================================================
# End of job options file
#==============================================================
