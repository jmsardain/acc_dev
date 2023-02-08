#!/bin/bash

# Test that the transform runs
bankpath=$EOS_MGM_URL_HTT/eos/atlas/atlascerngroupdisk/det-htt/HTTsim/ATLAS-P2-ITK-17-06-00/21.9.2/eta0103phi0305/PatternBanks/indexed.19-05-29_step2.2_pt800_01eta03_03phi05.root

HTTPattGenDC_tf.py \
    --inbank $bankpath \
    --outbank 'ccache.test_TrigHTTPattGenDC.root' \
    --nDC '1,0,0,0,0,0,1,1' \
    --maxPatts 1000


retVal=$?
if [ $retVal -ne 0 ]; then
    echo -e "\n\nDumping Log\n\n"
    tail -n +1 log.*
fi
exit $retVal
