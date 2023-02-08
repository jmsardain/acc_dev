import glob

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


InputHTTOutHitFile = []
if 'InputHTTOutHitFile' in os.environ :
     for ex in os.environ['InputHTTOutHitFile'].split(','):
        files = glob.glob(ex)
        if files:
            InputHTTOutHitFile += files
        else:
            InputHTTOutHitFile += [ex]
else :
     InputHTTOutHitFile = ["httsim_loghits.root"]
print "OutHTTSim file",  InputHTTOutHitFile


from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# create the Map Svc
import TrigHTTConfig.HTTTagConfig as HTTTagConfig
import TrigHTTMaps.HTTMapConfig as HTTMapConfig

tags = HTTTagConfig.getTags(stage='map')
map_tag = tags['map']
MapSvc = HTTMapConfig.addMapSvc(map_tag)
MapSvc.OutputLevel = INFO


from AthenaCommon.AppMgr import ToolSvc
from TrigHTTByteStream.TrigHTTByteStreamConf import HTTByteStreamDecoderAlg, TrigHTTTPByteStreamTool

theTPConverterTool = TrigHTTTPByteStreamTool(OutputLevel = DEBUG)
ToolSvc += theTPConverterTool

from TrigHTTInput.TrigHTTInputConf import HTTInputHeaderTool
HTTReadInput = HTTInputHeaderTool(OutputLevel = INFO)
HTTReadInput.InFileName=InputHTTRawHitFile
ToolSvc += HTTReadInput


from TrigHTTInput.TrigHTTInputConf import HTTOutputHeaderTool
HTTReadInput2 = HTTOutputHeaderTool(OutputLevel = INFO)
HTTReadInput2.InFileName=InputHTTOutHitFile
ToolSvc += HTTReadInput2

BSConverterAlg = HTTByteStreamDecoderAlg(OutputLevel = DEBUG)
BSConverterAlg.TPDecoderTool = theTPConverterTool
BSConverterAlg.InputTool = HTTReadInput
BSConverterAlg.OutputTool = HTTReadInput2
from distutils.util import strtobool


BSConverterAlg.DoDummyTestVectors= strtobool(os.environ['HTTBSDUMMYINPUT'])

print BSConverterAlg

svcMgr.MessageSvc.debugLimit        = 100000

theJob += BSConverterAlg

print theJob
