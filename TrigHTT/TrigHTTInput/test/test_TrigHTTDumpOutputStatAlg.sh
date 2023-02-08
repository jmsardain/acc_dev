#!/bin/sh
 
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}")/../../TrigHTTConfig/test" && pwd )"  
source $DIR/InputTestSetup.sh

export HTTInputFile=$HTTLogHitFile
echo "Running on Input file $HTTInputFile"
athena --evtMax=3  TrigHTTInput/HTTDumpOutputStatAlg_jobOptions.py
