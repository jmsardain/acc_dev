#!/bin/sh

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}")/../../TrigHTTConfig/test" && pwd )"  
source $DIR/InputTestSetup.sh
RDOFILE=$HTTRDOFile22

export REGION=0

Reco_tf.py \
    --inputRDOFile $RDOFILE \
    --maxEvents 5 \
    --outputESDFile ESD.pool.root \
    --digiSteeringConf StandardInTimeOnlyTruth \
    --geometryVersion  ATLAS-P2-ITK-22-00-00 \
    --conditionsTag    OFLCOND-MC15c-SDR-14-03 \
    --DataRunNumber    242000 \
    --steering doRAWtoALL \
    --postInclude 'InDetSLHC_Example/postInclude.SLHC_Setup_InclBrl_4.py' \
                  'InDetSLHC_Example/postInclude.DigitalClustering.py' \
    --preExec 'from AthenaCommon.GlobalFlags import globalflags; globalflags.DataSource.set_Value_and_Lock("geant4");' \
    'from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags; SLHC_Flags.doGMX.set_Value_and_Lock(True); SLHC_Flags.LayoutOption="InclinedAlternative";' \
    'from InDetRecExample.InDetJobProperties import InDetFlags; InDetFlags.doStandardPlots.set_Value_and_Lock(False);InDetFlags.useDCS.set_Value_and_Lock(True);' \
    'from PixelConditionsServices.PixelConditionsServicesConf import PixelCalibSvc;ServiceMgr += PixelCalibSvc();ServiceMgr.PixelCalibSvc.DisableDB=True;' \
    'from InDetPrepRawDataToxAOD.InDetDxAODJobProperties import InDetDxAODFlags;InDetDxAODFlags.DumpLArCollisionTime.set_Value_and_Lock(False);InDetDxAODFlags.DumpSctInfo.set_Value_and_Lock(True);InDetDxAODFlags.ThinHitsOnTrack.set_Value_and_Lock(False);' \
    'rec.UserAlgs=["TrigHTTInput/HTTSGToRawHitsWrapperAlg_jobOptions.py"]' \
    --preInclude 'InDetSLHC_Example/preInclude.SLHC_Setup_InclBrl_4.py,InDetSLHC_Example/preInclude.SLHC_Setup_Strip_GMX.py,InDetSLHC_Example/preInclude.SLHC_Calorimeter_mu0.py,InDetSLHC_Example/preInclude.SLHC.SiliconOnly.Reco.py,InDetSLHC_Example/SLHC_Setup_Reco_TrackingGeometry_GMX.py' \
    --postExec 'ToolSvc.InDetSCT_ClusteringTool.useRowInformation=True; from AthenaCommon.AppMgr import ToolSvc; ToolSvc.InDetTrackSummaryTool.OutputLevel=INFO;from InDetPhysValMonitoring.InDetPhysValMonitoringConf import InDetPhysValDecoratorAlg;decorators = InDetPhysValDecoratorAlg();topSequence += decorators'


retValJob=$?

if [ $retValJob -ne 0 ]; then
    echo -e "\n\n\nDumping Logs\n\n"
    tail -n +1 log.*
    exit $retValJob
fi

# compare with ref file
REFOUT=$EOS_MGM_URL_HTT/eos/atlas/atlascerngroupdisk/det-htt/HTTSim/wrapper_files/ATLAS-P2-ITK-22-00-00_RDO-21748447_bf1a13f_wrapper.root
HTTrootcomp.py   ${REFOUT} httsim_rawhits_wrap.root
retVal=$?

if [ $retVal -ne 0 ]; then
    echo -e "\n\nDifferent root files\n\n"
    tail -n +1 test*.log
    exit $retVal
fi

exit $retVal
