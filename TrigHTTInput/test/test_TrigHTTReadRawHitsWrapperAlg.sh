
#!/bin/sh
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}")/../../TrigHTTConfig/test" && pwd )"  
source $DIR/InputTestSetup.sh

export InputHTTRawHitFile=$HTTRawHitFile
export OutputHTTRawHitFile="httsim_rawhits_wrap.OUT.root"
athena --evtMax=5  TrigHTTInput/HTTReadRawHitsWrapperAlg_jobOptions.py

