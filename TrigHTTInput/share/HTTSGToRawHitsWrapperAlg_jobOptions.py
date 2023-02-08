###############################################################
#
# TrigHTTWrapper job options file
#
#==============================================================

from TrigHTTConfig.formatMessageSvc import formatMessageSvc
formatMessageSvc()




from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

OutputNTUP_HTTIPFile=[]
if 'outputNTUP_HTTIPFile' in os.environ:
    OutputNTUP_HTTIPFile=[os.environ['outputNTUP_HTTIPFile']]
else :
     OutputNTUP_HTTIPFile = ["httsim_rawhits_wrap.root"]
print "Output file: ", OutputNTUP_HTTIPFile



import TrigHTTConfig.HTTTagConfig as HTTTagConfig
tags = HTTTagConfig.getTags(stage='map')
map_tag = tags['map']

region=os.getenv("REGION","NOTHING")
if (region=="NOTHING"):
  print("HTT: Region environment variable wasn't set, setting it to 0")
  region=0

map_tag['region'] = str(region)

print "Running on region ", region
import TrigHTTConfig.HTTConfigCompInit as HTTConfig
HTTConfig.addEvtSelSvc(map_tag)


from AthenaCommon.AppMgr import ToolSvc
from TrigHTTInput.TrigHTTInputConf import HTTSGToRawHitsTool,  HTTInputHeaderTool
HTTSGInput = HTTSGToRawHitsTool("HTTReadInput", maxEta= 3.2, minPt= 0.8*GeV)
HTTSGInput.OutputLevel = INFO
HTTSGInput.ReadTruthTracks = True
HTTSGInput.filterHits=False
ToolSvc += HTTSGInput

HTTWriteOutput = HTTInputHeaderTool("HTTWriteOutput")
HTTWriteOutput.OutputLevel = INFO
HTTWriteOutput.InFileName=OutputNTUP_HTTIPFile
HTTWriteOutput.RWstatus="RECREATE"
ToolSvc += HTTWriteOutput

from TrigHTTInput.TrigHTTInputConf import HTTRawHitsWrapperAlg
wrapper = HTTRawHitsWrapperAlg(OutputLevel = INFO)
wrapper.InputTool = HTTSGInput
wrapper.OutputTool = HTTWriteOutput
theJob += wrapper




#adding HistSvc (still empty)
#from AthenaCommon.AppMgr import ServiceMgr
#from GaudiSvc.GaudiSvcConf import THistSvc
#ServiceMgr += THistSvc()
#ServiceMgr.THistSvc.Output += ["TRIGHTT DATAFILE='"+OutputNTUP_HTTIPFile+"' OPT='RECREATE'"]


print theJob
###############################################################
