#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}")/../../TrigHTTConfig/test" && pwd )"  
source $DIR/InputTestSetup.sh

export HTTInputFile=$HTTLogHitFile
#export HTTInputFile="/eos/atlas/atlascerngroupdisk/det-htt/HTTsim/ATLAS-P2-ITK-23-00-01/21.9.15/eta0103phi0305/Output/loghit_direct_N1000.root"
athena --evtMax=10 TrigHTTAlgorithms/HTTOverlapRemovalValidation_jobOptions.py
