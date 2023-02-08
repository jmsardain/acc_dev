import os

if 'InputXAOD' in os.environ:
    testFile = os.environ['InputXAOD'] 
    #testFile = '/eos/atlas/atlascerngroupdisk/det-htt/mc/mc15_14TeV.301493.MadGraphPythia8EvtGen_A14NNPDF23LO_RS_G_hh_bbbb_c10_M800.recon.AOD.e6011_s3185_s3186_r9871/AOD.13131314._000562.pool.root.1'
else:
    print "Path to input file not specified"

print "Input xAOD", testFile

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = [testFile]

import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = athenaCommonFlags.FilesInput()

from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

from TrigHTTFastSim.TrigHTTFastSimConf import HTTFastSim

# HTT smearing service
from AthenaCommon import CfgMgr
from AthenaCommon.Constants import VERBOSE,DEBUG,INFO

from TrigHTTFastSim.TrigHTTFastSimConf import TrigHTTSmearingSvc_TDR
HTTSmearingSvc = TrigHTTSmearingSvc_TDR(OutputLevel=DEBUG)

ServiceMgr += HTTSmearingSvc

theTrigHTTFastSim = HTTFastSim(SmearSvc=HTTSmearingSvc)

theJob += theTrigHTTFastSim

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
xAODStreamName = "XAODStream"
xAODFileName = "myXAOD.pool.root"
outStream = MSMgr.NewPoolRootStream( xAODStreamName, xAODFileName )

# HTT tracks
outStream.AddItem(['xAOD::TrackParticleContainer#httTracks'])
outStream.AddItem(['xAOD::TrackParticleAuxContainer#httTracksAux.'])
# Offline tracks
outStream.AddItem(['xAOD::TrackParticleContainer#OffTracks'])
outStream.AddItem(['xAOD::TrackParticleAuxContainer#OffTracksAux.'])

#svcMgr += CfgMgr.THistSvc()
#svcMgr.THistSvc.Output += ["ANOTHERSTREAM DATAFILE='myfile.root' OPT='RECREATE'"]

