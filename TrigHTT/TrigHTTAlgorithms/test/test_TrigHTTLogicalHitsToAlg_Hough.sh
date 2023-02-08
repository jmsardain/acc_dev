#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}")/../../TrigHTTConfig/test" && pwd )"
source $DIR/InputTestSetup.sh
region=0
InputHTTRawHitFile=$EOS_MGM_URL_HTT${input_map[$region]}
echo "Running on region $region"

# This test simply runs HTTLogicalHitsToAlg_tf to make sure it runs without error, with Hough transform

# Need to copy files over from eos for ctests
# These are files pointed to by the bank tag in HTTBankTags.py
if [ ! -z $EOS_MGM_URL_HTT ]; then
    FileDir='/eos/atlas/atlascerngroupdisk/det-htt/HTTsim/ATLAS-P2-ITK-22-02-00/21.9.16/eta0103phi0305/SectorBanks'

    consts1=corrgen_raw_8L.gcon
    xrdcp -s -f $EOS_MGM_URL_HTT/$FileDir/$consts1 $consts1

    #consts2=corrgen_raw_13L.gcon
    #xrdcp -s -f $EOS_MGM_URL_HTT/$FileDir/$consts2 $consts2

    sectors=sectorsHW_raw_8L.patt
    xrdcp -s -f $EOS_MGM_URL_HTT/$FileDir/$sectors $sectors
fi

HTTLogicalHitsToAlg_tf.py \
    --maxEvents 100 \
    --InFileName $InputHTTRawHitFile \
    --sampleType singleMuons \
    --withPU False \
    --algoTag 'hough' \
    --bankTag 'EF_TaskForce_dev22' \
    > transform_output.txt

retVal=$?

sed 's/^.\{,9\}//' log.*

exit $retVal
