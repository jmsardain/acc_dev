#--------------------------------------------------------------
# Headers and setup
#--------------------------------------------------------------
from AthenaCommon.Constants import *
from AthenaCommon.AppMgr import theApp, ToolSvc, ServiceMgr

from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doSemiDetailedMonitoring = True

from AthenaCommon.AlgSequence import AlgSequence
alg = AlgSequence()

import glob

#--------------------------------------------------------------
# Parse arguments for Athena stuff
#--------------------------------------------------------------

# Number of events to be processed
if hasattr(runArgs, "maxEvents"):
    theApp.EvtMax = runArgs.maxEvents
else:
    theApp.EvtMax = 1

#-----------------------------------------------------------------------------
# HTT Includes
#-----------------------------------------------------------------------------

from TrigHTTConfig.formatMessageSvc import HTT_OutputLevel, formatMessageSvc
import TrigHTTMaps.HTTMapConfig as HTTMapConfig
import TrigHTTBanks.HTTBankConfig as HTTBankConfig
import TrigHTTBanks.TrigHTTBanksConf as HTTBanks
import TrigHTTBankGen.TrigHTTBankGenConf as HTTBankGen
import TrigHTTBankGen.HTTBankGenConfig as HTTBankGenConfig

formatMessageSvc()

from TrigHTTConfig.parseRunArgs import parseHTTArgs
import TrigHTTConfig.HTTTagConfig as HTTTagConfig

tags = HTTTagConfig.getTags(stage='bank', options=parseHTTArgs(runArgs))

map_tag = tags['map']
bank_tag = tags['bank']
MapSvc = HTTMapConfig.addMapSvc(map_tag)
BankSvc = HTTBankConfig.addBankSvc(map_tag, bank_tag)



MapSvc.OutputLevel = HTT_OutputLevel
BankSvc.OutputLevel = HTT_OutputLevel

#--------------------------------------------------------------
# Create the rng service
#--------------------------------------------------------------

from AthenaServices.AthenaServicesConf import AtRanluxGenSvc

rng = AtRanluxGenSvc()
HTTBankGenConfig.applyTag(rng, bank_tag)
ServiceMgr += rng

#--------------------------------------------------------------
# Create the pattern generation tool
#--------------------------------------------------------------

if not hasattr(runArgs, "method"):
    raise RuntimeError, "Must specify pattern generation method"

if runArgs.method == 'TI': # See documentation in HTTPattGenToolI.h and HTTPattGen_tf.py on different methods
    pattGenTool = HTTBankGen.HTTPattGenTool_TI()
    toolArgs = []
elif runArgs.method == 'truth':
    pattGenTool = HTTBankGen.HTTPattGenTool_Truth()
    toolArgs = ['truth', 'rawInput']

for arg in toolArgs:
    attr = getattr(runArgs, arg)
    setattr(pattGenTool, arg, attr)

pattGenTool.OutputLevel = HTT_OutputLevel
HTTBankGenConfig.applyTag(pattGenTool, bank_tag)

ToolSvc += pattGenTool

#--------------------------------------------------------------
# Create hit tools
#--------------------------------------------------------------

# Note the tools still need to be created/added even if not used

from TrigHTTInput.TrigHTTInputConf import HTTInputHeaderTool
HTTReadInput = HTTInputHeaderTool()
files = []

if hasattr(runArgs, 'truth'):
    for ex in runArgs.truth.split(','):
        files += glob.glob(ex)
    HTTReadInput.InFileName = files

from TrigHTTInput.TrigHTTInputConf import HTT_RawToLogicalHitsTool
HTTRawLogic = HTT_RawToLogicalHitsTool()
HTTRawLogic.SaveOptional = 2
HTTRawLogic.TowersToMap = [0]

ToolSvc += HTTRawLogic
ToolSvc += HTTReadInput

#--------------------------------------------------------------
# Create the writer tool
#--------------------------------------------------------------

import TrigHTTUtils.utils as utils

defaultFileName = 'index.' + runArgs.method + '_n' + utils.human_readable(theApp.EvtMax) + '.reg' + map_tag['region'] + '.root'
description = HTTTagConfig.getDescription(bank_tag)
if runArgs.method == 'truth':
    description += '\ttruth:' + runArgs.truth + '\n'

writer = HTTBanks.HTTIndexPattTool_Writer()
writer.OutputLevel = HTT_OutputLevel
writer.bank_path = getattr(runArgs, 'output', defaultFileName)
writer.description = description

ToolSvc += writer

#--------------------------------------------------------------
# Create the algorithm
#--------------------------------------------------------------

PattGen = HTTBankGen.HTTPattGenAlgo()
PattGen.OutputLevel = HTT_OutputLevel
PattGen.pattGenTool = pattGenTool

alg += PattGen

#==============================================================
# End of job options file
#==============================================================
