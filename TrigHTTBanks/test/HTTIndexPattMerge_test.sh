#!/bin/bash

# Test that the transform runs by merging a bank with itself
bankpath=$EOS_MGM_URL_HTT/eos/atlas/atlascerngroupdisk/det-htt/HTTsim/ATLAS-P2-ITK-17-06-00/21.9.2/eta0103phi0305/PatternBanks/test/index.HTTSimExample.10M.noWC.root

HTTIndexPattMerge_tf.py \
    --inputs "${bankpath},${bankpath}" \
    --output 'index.HTTIndexPattMerge_test.root' \

retVal=$?
if [ $retVal -ne 0 ]; then
    echo "\n\nDumping Log\n\n"
    tail -n +1 log.*
fi
exit $retVal
