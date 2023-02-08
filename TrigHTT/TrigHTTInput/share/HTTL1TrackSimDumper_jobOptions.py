###############################################################
#
# TrigHTTWrapper job options file
#
#==============================================================


from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

OutputNTUP_HTTIPFile="test.root"


from AthenaCommon.AppMgr import ToolSvc
from TrigHTTInput.TrigHTTInputConf import HTTL1SimToRawHitTool
HTTInput = HTTL1SimToRawHitTool()
HTTInput.OutputLevel = DEBUG
name="../L1TrackSim_files/outSimple_matchPatternHistsATLAS-P2-ITK-05-00-00_eta0.1_6of8_pt4-400_30M-pix+strips-mixed21111122-mixed_-Imuon-w0-d1.9-N20-P0-R0-pm0-A1000000.root"
HTTInput.inputFileName =name

ToolSvc += HTTInput


from TrigHTTInput.TrigHTTInputConf import HTTRawHitsWrapperAlg
wrapper = HTTRawHitsWrapperAlg(OutputLevel = DEBUG,
                             OutFileName = OutputNTUP_HTTIPFile)


wrapper.InputTool = HTTInput
theJob += wrapper


print theJob
###############################################################
#athena --evtMax 10 ../athena/Trigger/TrigHTT/TrigHTTInput/share/HTTL1TrackSimDumper_jobOptions.py | tee out
