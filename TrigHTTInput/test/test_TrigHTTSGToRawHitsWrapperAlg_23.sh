
#!/bin/sh

#THIS IS FOR ATLAS-P2-ITK-23-00-02

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}")/../../TrigHTTConfig/test" && pwd )"  
source $DIR/InputTestSetup.sh
RDOFILE=$HTTRDOFile23

echo "Running now on $RDOFILE"
# 	mc15_14TeV.900036.PG_singlemu_Pt10_etaFlatnp0_43.merge.HITS.e8185_s3630_s3633	

export REGION=0
NEVENTS=5

Reco_tf.py \
    --jobNumber 242000 \
    --inputRDOFile  $RDOFILE \
    --maxEvents $NEVENTS \
    --outputAODFile OUT.AOD.pool.root \
    --outputESDFile OUT.ESD.pool.root \
    --geometryVersion ATLAS-P2-ITK-23-00-02 \
    --conditionsTag OFLCOND-MC15c-SDR-14-05 \
    --DataRunNumber 242000 \
    --imf all:'False' \
    --digiSteeringConf StandardInTimeOnlyTruth \
    --ignoreErrors True \
    --postInclude all:'InDetSLHC_Example/postInclude.SLHC_Setup_ITK.py'  RAWtoESD:'InDetSLHC_Example/postInclude.AnalogueClustering.py' \
    --preExec 'all:from AthenaCommon.GlobalFlags import globalflags; globalflags.DataSource.set_Value_and_Lock("geant4"); region=0' RAWtoESD:'rec.UserAlgs=["TrigHTTInput/HTTSGToRawHitsWrapperAlg_jobOptions.py"];' \
    --preInclude all:'InDetSLHC_Example/preInclude.SiliconOnly.py,InDetSLHC_Example/preInclude.SLHC_Setup.py,InDetSLHC_Example/preInclude.SLHC_Setup_Strip_GMX.py,InDetSLHC_Example/preInclude.SLHC_Calorimeter_mu0.py' 'HITtoRDO:InDetSLHC_Example/preInclude.SLHC.py' 'default:InDetSLHC_Example/preInclude.SLHC.SiliconOnly.Reco.py,InDetSLHC_Example/SLHC_Setup_Reco_TrackingGeometry_GMX.py' \
    --postExec all:'ServiceMgr.PixelLorentzAngleSvc.ITkL03D = True'









retValJob=$?

if [ $retValJob -ne 0 ]; then
    echo -e "\n\n\nDumping Logs\n\n"
    tail -n +1 log.*
    exit $retValJob
fi

# compare with ref file
#REFOUT=$EOS_MGM_URL_HTT/eos/atlas/atlascerngroupdisk/det-htt/HTTSim/wrapper_files/ATLAS-P2-ITK-22-00-00_RDO-21748447_bf1a13f_wrapper.root
#HTTrootcomp.py   ${REFOUT} httsim_rawhits_wrap.root
#retVal=$?

#if [ $retVal -ne 0 ]; then
#    echo -e "\n\nDifferent root files\n\n"
#    tail -n +1 test*.log
#    exit $retVal
#fi

exit $retVal
