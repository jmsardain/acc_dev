

#--------------------------------------------------------------
# Headers and setup
#--------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

from AthenaCommon.AppMgr import theApp, ToolSvc, ServiceMgr
from AthenaCommon.Constants import VERBOSE,DEBUG,INFO

theApp.EvtMax = runArgs.maxEvents
#theApp.skipEvents = runArgs.skipEvents
#ServiceMgr.MessageSvc.debugLimit        = 100000
# from AthenaServices.AthenaServicesConf import AtRanluxGenSvc
# ServiceMgr += AtRanluxGenSvc()
# rndstream="RANDOM"
# seed1=2
# seed2=2
# if hasattr(runArgs, 'jmsseed'):
#     seed1=runArgs.jmsseed
#
# if hasattr(runArgs, 'jmsseed2'):
#     seed2=runArgs.jmsseed2
# ServiceMgr.AtRanluxGenSvc.Seeds= [ rndstream+" %d %d" %(seed1,seed2) ]
# ServiceMgr.AtRanluxGenSvc.EventReseeding = True

from GaudiSvc.GaudiSvcConf import THistSvc

from PerfMonComps.PerfMonFlags import jobproperties
#jobproperties.PerfMonFlags.doSemiDetailedMonitoring = True
#jobproperties.PerfMonFlags.doMallocMonitoring = True

#--------------------------------------------------------------
# HTT Setup
#--------------------------------------------------------------

from TrigHTTConfig.parseRunArgs import parseHTTArgs
from TrigHTTConfig.formatMessageSvc import HTT_OutputLevel, formatMessageSvc
import TrigHTTConfig.HTTTagConfig as HTTTagConfig
import TrigHTTConfig.HTTConfigCompInit as HTTConfig
import TrigHTTMaps.HTTMapConfig as HTTMapConfig
import TrigHTTBanks.HTTBankConfig as HTTBankConfig
import TrigHTTAlgorithms.HTTAlgorithmConfig as HTTAlgorithmConfig

formatMessageSvc()

# Override output level of all HTT components here
#HTT_OutputLevel = DEBUG

print "Input file:  ", runArgs.InFileName
tags = HTTTagConfig.getTags(stage='algo', options=parseHTTArgs(runArgs))
map_tag = tags['map']
bank_tag = tags['bank']
algo_tag = tags['algo']

def defaultFilename():
    f = HTTTagConfig.getDescription(map_tag, bank_tag, algo_tag, filename=True) # deliberately omitting hit filtering tag for now
    f = 'loghits__' +  map_tag['release'] + '-' + map_tag['geoTag'] + '__' + f + '.root'
    return f

OutFileName=getattr(runArgs, 'OutFileName', defaultFilename())
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += ["MONITOROUT DATAFILE='"+OutFileName+"' OPT='RECREATE'"]

MapSvc = HTTMapConfig.addMapSvc(map_tag)
MapSvc.OutputLevel = HTT_OutputLevel

HitFilteringTool = HTTMapConfig.addHitFilteringTool(map_tag)
HitFilteringTool.OutputLevel = HTT_OutputLevel

BankSvc = HTTBankConfig.addBankSvc(map_tag, bank_tag)
BankSvc.OutputLevel = HTT_OutputLevel


# Important to do hit tracing if we have physics samples or events with pileup.
doHitTracing = (map_tag['withPU'] or map_tag['sampleType']=='LLPs')

#--------------------------------------------------------------
# Make the algorithm
#--------------------------------------------------------------

from TrigHTTAlgorithms.TrigHTTAlgorithmsConf import HTTLogicalHitsProcessAlg, HTTEtaPatternFilterTool, HTTPhiRoadFilterTool

alg = HTTLogicalHitsProcessAlg()
alg.OutputLevel = HTT_OutputLevel
alg.Monitor = True
#alg.Monitor = False
alg.HoughMonitor = algo_tag['hough']
alg.LLPHoughMonitor = algo_tag['houghplotJMS']
alg.HitFiltering = algo_tag['HitFiltering']
alg.writeOutputData = algo_tag['writeOutputData']
alg.Clustering = True
alg.SpacePoints = algo_tag['SpacePoints']
alg.Monitor = True
#alg.Monitor = False
alg.tracking = algo_tag['doTracking']
alg.outputHitTxt = algo_tag['outputHitTxt']
alg.RunSecondStage = algo_tag['secondStage']
alg.DoMissingHitsChecks = algo_tag['DoMissingHitsChecks']
alg.HTTTrackKey="OutputTracksTest"
alg.description = HTTTagConfig.getDescription(map_tag, bank_tag, algo_tag)
alg.histoPrintDetail = 2

theJob += alg

#--------------------------------------------------------------
# Make hit tools
#--------------------------------------------------------------

from TrigHTTInput.TrigHTTInputConf import HTT_RawToLogicalHitsTool
HTTRawLogic = HTT_RawToLogicalHitsTool()
HTTRawLogic.OutputLevel = HTT_OutputLevel
HTTRawLogic.SaveOptional = 2
if (map_tag['sampleType'] == 'skipTruth'):
    HTTRawLogic.SaveOptional = 1
HTTRawLogic.TowersToMap = [0] # TODO TODO why is this hardcoded?
ToolSvc += HTTRawLogic
alg.RawToLogicalHitsTool = HTTRawLogic

### For random hit tool:
### Average number of charged tracks per unit of pseudo-rapdity and and pp collision is ~6 (in barrel)
### (See figure 8 of https://arxiv.org/pdf/1808.07224.pdf)
### We are looking at a 0.2x0.2 region, so divide by x5 for 1/5 of eta, and divide by 2*pi/0.2 for our narrow phi region
### But multiply by 200 for mu of 200, and we get 7.6, so we call it 8 tracks (not fluctuating number right now)
### TO DO CHECK IF THIS IS RIGHT?!?!?!?

if algo_tag['randomRawHits']:
    from TrigHTTInput.TrigHTTInputConf import HTT_RandRawHitsFromInvConstsTool
    InputTool = HTT_RandRawHitsFromInvConstsTool()
    InputTool.OutputLevel = HTT_OutputLevel
    InputTool.NTracks = 8
    InputTool.PoissonFluctuate = False
    InputTool.Generate1stStageTracks = True
    InputTool.NMaxWC = 2
    ToolSvc += InputTool
elif algo_tag['realAndRandomHits']:
    alg.SecondInputToolN = 1
    from TrigHTTInput.TrigHTTInputConf import HTTInputHeaderTool
    InputTool = HTTInputHeaderTool("HTTReadInput")
    InputTool.OutputLevel = HTT_OutputLevel
    InputTool.InFileName = runArgs.InFileName
    #InputTool.ReadTruthTracks = True
    ToolSvc += InputTool
    from TrigHTTInput.TrigHTTInputConf import HTT_RandRawHitsFromInvConstsTool
    InputToolRand = HTT_RandRawHitsFromInvConstsTool()
    InputToolRand.OutputLevel = HTT_OutputLevel
    InputToolRand.NTracks = 8
    InputToolRand.PoissonFluctuate = False
    InputToolRand.Generate1stStageTracks = True
    InputToolRand.NMaxWC = 2
    ToolSvc += InputToolRand
elif algo_tag['realHitsOverlay']:
    alg.FirstInputToolN = algo_tag['FirstInputToolN']
    alg.SecondInputToolN = 1 ## We could configure this, for now we don't
    from TrigHTTInput.TrigHTTInputConf import HTTInputHeaderTool
    InputTool = HTTInputHeaderTool("HTTReadInput")
    InputTool.OutputLevel = HTT_OutputLevel
    InputTool.InFileName = runArgs.InFileName
    ToolSvc += InputTool
    from TrigHTTInput.TrigHTTInputConf import HTT_ReadRawRandomHitsTool
    InputTool2 = HTT_ReadRawRandomHitsTool("HTT_ReadRawRandomHitsTool")
    InputTool2.OutputLevel = HTT_OutputLevel
    InputTool2.InFileName = runArgs.InFileName2
    ToolSvc += InputTool2
    alg.InputTool2 = InputTool2
else:
    from TrigHTTInput.TrigHTTInputConf import HTTInputHeaderTool
    InputTool = HTTInputHeaderTool("HTTReadInput")
    InputTool.OutputLevel = HTT_OutputLevel
    InputTool.InFileName = runArgs.InFileName
    #InputTool.ReadTruthTracks = True
ToolSvc += InputTool
alg.InputTool = InputTool


if algo_tag['EtaPatternRoadFilter']:
    print("Setting Up EtaPatternFilter")
    RoadFilter = HTTEtaPatternFilterTool()
    alg.FilterRoads=True
    if len(algo_tag['threshold']) != 1:
        raise NotImplementedError("EtaPatternRoadFilter does not support multi-value/neighboring bin treshold")
    RoadFilter.threshold=algo_tag['threshold'][0]
    RoadFilter.EtaPatterns=algo_tag['EtaPatternRoadFilter']
    ToolSvc += RoadFilter

if  algo_tag['PhiRoadFilter']!=None:
    print("Setting Up PhiRoadFilter")
    PhiRoadFilter = HTTPhiRoadFilterTool()
    alg.FilterRoads2=True
    if len(algo_tag['threshold'])!=1:
        raise NotImplementedError("PhiRoadFilter does not support multi-value/neighboring bin treshold")
    PhiRoadFilter.threshold=algo_tag['threshold'][0]
    PhiRoadFilter.window=HTTAlgorithmConfig.floatList(algo_tag['PhiRoadFilter'])
    ToolSvc += PhiRoadFilter

# writing down the output file
from TrigHTTInput.TrigHTTInputConf import HTTOutputHeaderTool
HTTWriteOutput = HTTOutputHeaderTool("HTTWriteOutput")
HTTWriteOutput.OutputLevel = HTT_OutputLevel
HTTWriteOutput.InFileName = ["test"]
HTTWriteOutput.RWstatus = "HEADER" # do not open file, use THistSvc
HTTWriteOutput.RunSecondStage = alg.RunSecondStage
ToolSvc += HTTWriteOutput
alg.OutputTool= HTTWriteOutput

#--------------------------------------------------------------
# Spacepoint tool
#--------------------------------------------------------------

from TrigHTTMaps.TrigHTTMapsConf import HTTSpacePointsTool, HTTSpacePointsTool_v2

spacepoints_version = 2
if spacepoints_version == 1:
    SPT = HTTSpacePointsTool()
    SPT.FilteringClosePoints = True
else:
    SPT = HTTSpacePointsTool_v2()
    SPT.FilteringClosePoints = False
    SPT.PhiWindow=0.008

SPT.OutputLevel = HTT_OutputLevel
SPT.Duplication = True
SPT.Filtering = algo_tag['SpacePointFiltering']
ToolSvc += SPT
alg.SpacePointTool = SPT

#--------------------------------------------------------------
# Make RF/TF tools
#--------------------------------------------------------------

from TrigHTTAlgorithms.TrigHTTAlgorithmsConf import HTTPatternMatchTool, HTTTrackFitterTool, HTTSectorMatchTool, HTTHoughRootOutputTool, HTTNNTrackTool, HTTOverlapRemovalTool

if algo_tag['sectorsAsPatterns']:
    RF = HTTSectorMatchTool()
    HTTAlgorithmConfig.applyTag(RF, algo_tag)
    ToolSvc += RF
elif algo_tag['hough']:
    RF = HTTAlgorithmConfig.addHoughTool(map_tag, algo_tag,doHitTracing)
    alg.DoNNTrack = algo_tag['TrackNNAnalysis']
    if algo_tag['TrackNNAnalysis']:
      NNTrackTool = HTTNNTrackTool()
      ToolSvc += NNTrackTool
    alg.DoHoughRootOutput = algo_tag['hough_rootoutput']
    if algo_tag['hough_rootoutput']:
      rootOutTool = HTTHoughRootOutputTool()
      ToolSvc += rootOutTool
      HoughRootOutputName="HTTHoughOutput.root"
      ServiceMgr.THistSvc.Output += ["TRIGHTTHOUGHOUTPUT DATAFILE='"+HoughRootOutputName+"' OPT='RECREATE'"]
elif algo_tag['hough_1d']:
    RF = HTTAlgorithmConfig.addHough1DShiftTool(map_tag, algo_tag)
else:
    RF = HTTPatternMatchTool()
    HTTAlgorithmConfig.applyTag(RF, algo_tag)
    ToolSvc += RF

TF_1st= HTTTrackFitterTool("HTTTrackFitterTool_1st")
HTTAlgorithmConfig.applyTag(TF_1st, algo_tag)
ToolSvc += TF_1st

RF.OutputLevel = HTT_OutputLevel
TF_1st.OutputLevel = HTT_OutputLevel

alg.RoadFinder = RF
alg.TrackFitter_1st = TF_1st

OR_1st = HTTOverlapRemovalTool("HTTOverlapRemovalTool_1st")
OR_1st.OutputLevel = HTT_OutputLevel
OR_1st.ORAlgo = "Normal"
OR_1st.doFastOR = algo_tag['doFastOR']
OR_1st.NumOfHitPerGrouping = 5
# For invert grouping use the below setup
# OR1st.ORAlgo = "Invert"
# OR1st.NumOfHitPerGrouping = 3
if algo_tag['hough']:
    OR_1st.nBins_x = algo_tag['xBins'] + 2 * algo_tag['xBufferBins']
    OR_1st.nBins_y = algo_tag['yBins'] + 2 * algo_tag['yBufferBins']
    OR_1st.localMaxWindowSize = algo_tag['localMaxWindowSize']
    OR_1st.roadSliceOR = algo_tag['roadSliceOR']

#--------------------------------------------------------------
# Second stage fitting
#--------------------------------------------------------------

if algo_tag['secondStage']:

    TF_2nd = HTTTrackFitterTool("HTTTrackFitterTool_2nd")
    HTTAlgorithmConfig.applyTag(TF_2nd, algo_tag)
    TF_2nd.Do2ndStageTrackFit = True
    TF_2nd.OutputLevel = HTT_OutputLevel
    ToolSvc += TF_2nd
    alg.TrackFitter_2nd = TF_2nd

    OR_2nd = HTTOverlapRemovalTool("HTTOverlapRemovalTool_2nd")
    OR_2nd.DoSecondStage = True
    OR_2nd.OutputLevel = HTT_OutputLevel
    OR_2nd.ORAlgo = "Normal"
    OR_2nd.doFastOR = algo_tag['doFastOR']
    OR_2nd.NumOfHitPerGrouping = 5
    # For invert grouping use the below setup
    # OR2nd.ORAlgo = "Invert"
    # OR2nd.NumOfHitPerGrouping = 3
    ToolSvc += OR_2nd
    alg.OverlapRemoval_2nd = OR_2nd

    from TrigHTTAlgorithms.TrigHTTAlgorithmsConf import HTTExtrapolator
    HTTExtrapolatorTool = HTTExtrapolator()
    HTTExtrapolatorTool.OutputLevel = HTT_OutputLevel
    HTTExtrapolatorTool.Ncombinations = 16
    ToolSvc += HTTExtrapolatorTool
    alg.Extrapolator = HTTExtrapolatorTool

#--------------------------------------------------------------
# Configure LRT
#--------------------------------------------------------------

# We'll use this in other tools if LRT is requested
evtSelLRT = HTTConfig.addEvtSelSvc(map_tag,"EvtSelLRTSvc")

if algo_tag['lrt']:

    # Finish setting up that event selector
    evtSelLRT.doLRT = True
    evtSelLRT.minLRTpT = algo_tag['lrt_ptmin']
    evtSelLRT.OutputLevel=DEBUG

    # consistency checks
    assert algo_tag['lrt_use_basicHitFilter'] != algo_tag['lrt_use_mlHitFilter'], 'Inconsistent LRT hit filtering setup, need either ML of Basic filtering enabled'
    assert algo_tag['lrt_use_straightTrackHT'] != algo_tag['lrt_use_doubletHT'], 'Inconsistent LRT HT setup, need either double or strightTrack enabled'
    alg.doLRT = True
    # alg.LRTHitFiltering = (not algo_tag['lrt_skip_hit_filtering'])
    alg.LRTHitFiltering = True
    # now set up the rest
    from TrigHTTAlgorithms.TrigHTTAlgorithmsConf import HTTLLPRoadFilterTool
    if algo_tag['lrt_use_basicHitFilter']:
        thefilter = HTTLLPRoadFilterTool()
        ToolSvc += thefilter
        alg.LRTRoadFilter = thefilter
    if algo_tag['lrt_use_doubletHT']:
        from TrigHTTAlgorithms.HTTAlgorithmConfig import addLRTDoubletHTTool
        doubletTool = addLRTDoubletHTTool(algo_tag)
        #doubletTool.OutputLevel=DEBUG
        alg.LRTRoadFinder  = doubletTool
    elif algo_tag['lrt_use_straightTrackHT'] :
        alg.LRTRoadFinder = HTTAlgorithmConfig.addHough_d0phi0_Tool(map_tag, algo_tag, doHitTracing)
    alg.HoughMonitor = False
    alg.LLPHoughMonitor = True

    # Use our event selector in the algorithm
    alg.eventSelector = evtSelLRT

#--------------------------------------------------------------
# Make Monitor Tools and Data Flow Tool
#--------------------------------------------------------------

# from TrigHTTMonitor.TrigHTTMonitorConf import HTTHitMonitorTool, HTTClusterMonitorTool, HTTRoadMonitorTool, HTTTrackMonitorTool, HTTPerformanceMonitorTool, HTTEventMonitorTool, HTTMonitorUnionTool
# from TrigHTTAlgorithms.TrigHTTAlgorithmsConf import HTTDataFlowTool
from TrigHTTMonitor.TrigHTTMonitorConf import HTTPerformanceMonitorTool, HTTMonitorUnionTool
from TrigHTTAlgorithms.TrigHTTAlgorithmsConf import HTTDataFlowTool

# HitMonitor = HTTHitMonitorTool()
# HitMonitor.OutputLevel = HTT_OutputLevel
# HitMonitor.fastMon = algo_tag['fastMon']
# HitMonitor.RunSecondStage = alg.RunSecondStage
# HitMonitor.canExtendHistRanges = algo_tag['canExtendHistRanges']
#
# ClusterMonitor = HTTClusterMonitorTool()
# ClusterMonitor.OutputLevel = HTT_OutputLevel
# ClusterMonitor.Clustering = alg.Clustering
# ClusterMonitor.Spacepoints = alg.SpacePoints
# ClusterMonitor.RunSecondStage = alg.RunSecondStage
# ClusterMonitor.canExtendHistRanges = algo_tag['canExtendHistRanges']
#
# RoadMonitor = HTTRoadMonitorTool()
# RoadMonitor.OutputLevel = HTT_OutputLevel
# RoadMonitor.fastMon = algo_tag['fastMon']
# RoadMonitor.RunSecondStage = alg.RunSecondStage
# RoadMonitor.canExtendHistRanges = algo_tag['canExtendHistRanges']
# RoadMonitor.BarcodeFracCut = algo_tag['barcodeFracMatch']
#
# TrackMonitor = HTTTrackMonitorTool()
# TrackMonitor.OutputLevel = HTT_OutputLevel
# TrackMonitor.fastMon = algo_tag['fastMon']
# TrackMonitor.DoMissingHitsChecks = alg.DoMissingHitsChecks
# TrackMonitor.RunSecondStage = alg.RunSecondStage

PerfMonitor = HTTPerformanceMonitorTool()
PerfMonitor.OutputLevel = HTT_OutputLevel
PerfMonitor.fastMon = algo_tag['fastMon']
PerfMonitor.RunSecondStage = alg.RunSecondStage
PerfMonitor.BarcodeFracCut = algo_tag['barcodeFracMatch']

# EventMonitor = HTTEventMonitorTool()
# EventMonitor.OutputLevel = HTT_OutputLevel
# EventMonitor.fastMon = algo_tag['fastMon']
# EventMonitor.Clustering = alg.Clustering
# EventMonitor.Spacepoints = alg.SpacePoints
# EventMonitor.RunSecondStage = alg.RunSecondStage
# EventMonitor.BarcodeFracCut = algo_tag['barcodeFracMatch']
#
# DataFlowTool = HTTDataFlowTool()
# DataFlowTool.OutputLevel = HTT_OutputLevel
# DataFlowTool.RunSecondStage = alg.RunSecondStage

# We may want to monitor the default tracking on the larger histogram axes
# and with appropriate truth cuts in order to fill in the LRT efficiencies
if algo_tag['lrt'] :

    # HitMonitor.eventSelector = evtSelLRT
    # HitMonitor.d0Range = algo_tag['lrt_mon_d0Range']
    # HitMonitor.z0Range = algo_tag['lrt_mon_z0Range']
    #
    # ClusterMonitor.eventSelector = evtSelLRT
    # ClusterMonitor.d0Range = algo_tag['lrt_mon_d0Range']
    # ClusterMonitor.z0Range = algo_tag['lrt_mon_z0Range']
    #
    # RoadMonitor.eventSelector = evtSelLRT
    # RoadMonitor.d0Range = algo_tag['lrt_mon_d0Range']
    # RoadMonitor.z0Range = algo_tag['lrt_mon_z0Range']
    #
    # TrackMonitor.eventSelector = evtSelLRT
    # TrackMonitor.d0Range = algo_tag['lrt_mon_d0Range']
    # TrackMonitor.z0Range = algo_tag['lrt_mon_z0Range']

    PerfMonitor.eventSelector = evtSelLRT
    PerfMonitor.d0Range = algo_tag['lrt_mon_d0Range']
    PerfMonitor.z0Range = algo_tag['lrt_mon_z0Range']

    # EventMonitor.eventSelector = evtSelLRT
    # EventMonitor.d0Range = algo_tag['lrt_mon_d0Range']
    # EventMonitor.z0Range = algo_tag['lrt_mon_z0Range']
    #
    # DataFlowTool.eventSelector = evtSelLRT

if (TF_1st.IdealCoordFitType != 0):
    if algo_tag['DoDeltaGPhis']:
        TrackMonitor.Chi2ndofCut = 50.
        PerfMonitor.Chi2ndofCut = 50.
        DataFlowTool.Chi2ndofCut = 50.
        OR_1st.MinChi2 = 50.
    else:
        # TrackMonitor.Chi2ndofCut = 2.
        PerfMonitor.Chi2ndofCut = 2.
        # DataFlowTool.Chi2ndofCut = 2.
        if algo_tag['hough']:
            OR_1st.MinChi2 = 2.
else:
    # TrackMonitor.Chi2ndofCut = 40.
    PerfMonitor.Chi2ndofCut = 40.
    # DataFlowTool.Chi2ndofCut = 40.
    if algo_tag['hough']:
        OR_1st.MinChi2 = 40.

MonitorUnion = HTTMonitorUnionTool()
MonitorUnion.OutputLevel = HTT_OutputLevel
MonitorTools = []
# MonitorTools.append(HitMonitor)
# MonitorTools.append(ClusterMonitor)
# MonitorTools.append(RoadMonitor)
# MonitorTools.append(TrackMonitor)
MonitorTools.append(PerfMonitor)
# MonitorTools.append(EventMonitor)
MonitorUnion.MonitorTools = MonitorTools

ToolSvc += OR_1st
ToolSvc += MonitorUnion
# ToolSvc += DataFlowTool

alg.OverlapRemoval_1st = OR_1st
alg.MonitorUnionTool = MonitorUnion
# alg.DataFlowTool = DataFlowTool
