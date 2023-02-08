#!/bin/sh

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}")/../../TrigHTTConfig/test" && pwd )"  
source $DIR/InputTestSetup.sh

RDOFILE=$HTTRDOFile

Reco_tf.py \
    --inputRDOFile $RDOFILE \
    --maxEvents 5 \
    --outputESDFile ESD.pool.root \
    --digiSteeringConf StandardInTimeOnlyTruth \
    --geometryVersion  ATLAS-P2-ITK-17-06-00 \
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
    'rec.UserAlgs=["TrigHTTInput/HTTRawToLogicalHitsWrapperAlg_jobOptions.py"]' \
    --preInclude 'InDetSLHC_Example/preInclude.SLHC_Setup_InclBrl_4.py,InDetSLHC_Example/preInclude.SLHC_Setup_Strip_GMX.py,InDetSLHC_Example/preInclude.SLHC_Calorimeter_mu0.py,InDetSLHC_Example/preInclude.SLHC.SiliconOnly.Reco.py,InDetSLHC_Example/SLHC_Setup_Reco_TrackingGeometry_GMX.py' \
    --postExec 'ToolSvc.InDetSCT_ClusteringTool.useRowInformation=True; from AthenaCommon.AppMgr import ToolSvc; ToolSvc.InDetTrackSummaryTool.OutputLevel=INFO;from InDetPhysValMonitoring.InDetPhysValMonitoringConf import InDetPhysValDecoratorAlg;decorators = InDetPhysValDecoratorAlg();topSequence += decorators'


retVal=$?
if [ $retVal -ne 0 ]; then
    echo -e "\n\n\nDumping Logs\n\n"
    tail -n +1 log.*
fi
exit $retVal
