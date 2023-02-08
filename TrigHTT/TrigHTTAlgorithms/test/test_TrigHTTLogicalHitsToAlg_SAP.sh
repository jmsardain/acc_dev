#!/bin/bash
# This test simply runs HTTLogicalHitsToAlg_tf to make sure it runs without error, with sectors as patterns

region=0
NEVENTS=5
echo "Running on region $region with $NEVENTS events"

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}")/../../TrigHTTConfig/test" && pwd )"

source $DIR/InputTestSetup.sh
InputHTTRawHitFile=$EOS_MGM_URL_HTT${input_map[$region]}
#InputHTTRawHitFile=$PU200Wrapper_23
sampleType=singleMuons
withPU=false

echo "Input file = $InputHTTRawHitFile"

# constants
dir_map=( \
            [0]="ATLAS-P2-ITK-22-02-00/21.9.16/eta0103phi0305" \
            [1]="ATLAS-P2-ITK-22-02-00/21.9.16/eta0709phi0305" \
        )
input_dir=$EOS_MGM_URL_HTT/eos/atlas/atlascerngroupdisk/det-htt/HTTsim/${dir_map[$region]}

# Need to copy files over from eos for ctests
# These are files pointed to by the bank tag in HTTBankTags.py
if [ ! -z $EOS_MGM_URL_HTT ]; then
    FileDir=$input_dir/SectorBanks

    consts1=corrgen_raw_8L.gcon
    xrdcp -s -f $FileDir/$consts1 $consts1

    sectors1=sectorsHW_raw_8L.patt
    xrdcp -s -f $FileDir/$sectors1 $sectors1

    # if [ $region == 0 ]; then
    #     consts2=corrgen_raw_13L_reg0_checkGood1.gcon
    #     xrdcp -s -f $FileDir/$consts2 $consts2

    #     sectors2=sectorsHW_raw_13L_reg0_checkGood1.patt
    #     xrdcp -s -f $FileDir/$sectors2 $sectors2
    # fi
fi

# if [ $region == 0 ]; then
#     echo "Running with second stage enabled."
#     secondStage=true
# else
#     secondStage=false
# fi

HTTLogicalHitsToAlg_tf.py \
    --maxEvents $NEVENTS \
    --InFileName $InputHTTRawHitFile \
    --sampleType $sampleType \
    --withPU $withPU \
    --bankTag 'EF_TaskForce_dev22' \
    --region $region \
    --secondStage False \
    --sectorsAsPatterns True \
    > transform_output.txt

retVal=$?

sed 's/^.\{,9\}//' log.*

exit $retVal
