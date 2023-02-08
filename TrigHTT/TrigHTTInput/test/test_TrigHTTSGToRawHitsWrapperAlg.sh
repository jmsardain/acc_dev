#!/bin/sh

#THIS IS FOR ATLAS-P2-ITK-22-02-00

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}")/../../TrigHTTConfig/test" && pwd )"  
source $DIR/InputTestSetup.sh
RDOFILE=$HTTRDOFile22
#RDOFILE=$PU200RDO_22

export REGION=0
NEVENTS=5

echo "TEST Running now on $RDOFILE, in region $REGION, for $NEVENTS events"
Reco_tf.py \
--jobNumber 242000 \
--inputRDOFile $RDOFILE \
--maxEvents $NEVENTS \
--outputESDFile OUT.ESD.pool.root \
--geometryVersion ATLAS-P2-ITK-22-02-00 \
--conditionsTag OFLCOND-MC15c-SDR-14-05 \
--DataRunNumber 242000 \
--imf all:'False' \
--digiSteeringConf StandardInTimeOnlyTruth \
--ignoreErrors True \
--postInclude all:'InDetSLHC_Example/postInclude.SLHC_Setup_ITK.py'  RAWtoESD:'InDetSLHC_Example/postInclude.AnalogueClustering.py' \
--preExec 'all:from AthenaCommon.GlobalFlags import globalflags; globalflags.DataSource.set_Value_and_Lock("geant4");' RAWtoESD:'rec.UserAlgs=["TrigHTTInput/HTTSGToRawHitsWrapperAlg_jobOptions.py"];' \
--preInclude all:'InDetSLHC_Example/preInclude.SiliconOnly.py,InDetSLHC_Example/preInclude.SLHC_Setup.py,InDetSLHC_Example/preInclude.SLHC_Setup_Strip_GMX.py,InDetSLHC_Example/preInclude.SLHC_Calorimeter_mu0.py' 'HITtoRDO:InDetSLHC_Example/preInclude.SLHC.py' 'default:InDetSLHC_Example/preInclude.SLHC.SiliconOnly.Reco.py,InDetSLHC_Example/SLHC_Setup_Reco_TrackingGeometry_GMX.py' \
--postExec all:'ServiceMgr.PixelLorentzAngleSvc.ITkL03D = True'
 > transform_output.txt
retVal=$?

# this dumps the log into the test.log:
#sed 's/^.\{,9\}//' log.RAWtoESD

exit $retVal
