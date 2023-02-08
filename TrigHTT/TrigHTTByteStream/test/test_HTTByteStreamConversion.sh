#!/bin/sh
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}")/../../TrigHTTConfig/test" && pwd )"  
source $DIR/InputTestSetup.sh


#input_dir=$EOS_MGM_URL_HTT/eos/atlas/atlascerngroupdisk/det-htt/ 

export InputHTTRawHitFile=$HTTRawHitFile
#input_dir/HTTsim/ATLAS-P2-ITK-17-06-00/21.9.2/eta0103phi0305/Wrappers/httsim_rawhits_wrap_121120.root
export InputHTTOutHitFile=$HTTLogHitFile
#input_dir/HTTsim/ATLAS-P2-ITK-23-00-01/21.9.15/eta0103phi0305/References/loghits__21.9.15-ATLAS-P2-ITK-23-00-01__EF_TaskForce_dev23__EF_TaskForce_dev23__dev_21-02-15.root 

echo "running with dummy flag=$HTTBSDUMMYINPUT"
athena --evtMax=1  TrigHTTByteStream/HTTByteStreamConversion_jobOptions.py
