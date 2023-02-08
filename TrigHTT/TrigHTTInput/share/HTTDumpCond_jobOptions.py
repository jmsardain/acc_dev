#-----------------------------------------------------------------------------
# Athena imports
#-----------------------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

from AthenaCommon.AppMgr import ToolSvc

import AthenaCommon.Configurable as Configurable
Configurable.log.setLevel(INFO)

#--------------------------------------------------------------
# HTT Includes
#--------------------------------------------------------------

from TrigHTTConfig.formatMessageSvc import formatMessageSvc
from TrigHTTInput.TrigHTTInputConf import HTTDetectorTool, HTTDumpCondAlgo

formatMessageSvc()

import TrigHTTConfig.HTTTagConfig as HTTTagConfig
import TrigHTTMaps.HTTMapConfig as HTTMapConfig

tags = HTTTagConfig.getTags(stage='map')
map_tag = tags['map']
MapSvc = HTTMapConfig.addMapSvc(map_tag)

HTTDet = HTTDetectorTool()
ToolSvc += HTTDet

HTTDumpCond = HTTDumpCondAlgo("HTTDumpCondAlgo", OutputLevel=INFO)
HTTDumpCond.DumpGlobalToLocalMap = True
theJob += HTTDumpCond
