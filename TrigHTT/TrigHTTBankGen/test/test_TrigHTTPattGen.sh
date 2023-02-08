#!/bin/bash

# Test that the transform runs without errors


# Used in CI, fetch files first.
if [ ! -z $EOS_MGM_URL_HTT ]; then
    echo "Copying from eos to local"
    filedir='/eos/atlas/atlascerngroupdisk/det-htt/HTTsim/ATLAS-P2-ITK-17-06-00/21.9.2/eta0103phi0305/SectorBanks'

    corrgen=corrgen_raw_8L_reg0_checkGood1.gcon
    xrdcp -f $EOS_MGM_URL_HTT/$filedir/$corrgen $corrgen

    sectorHW=sectorsHW_raw_8L_reg0_checkGood1.patt
    xrdcp -f $EOS_MGM_URL_HTT/$filedir/$sectorHW $sectorHW

fi


HTTPattGen_tf.py \
    --method TI \
    --output patterns.root \
    --maxEvents 100 \
    --bankTag "jahreda_20-07-20" \


retVal=$?
if [ $retVal -ne 0 ]; then
    echo -e "\n\nDumping Log\n\n"
    tail -n +1 log.*
fi
exit $retVal
