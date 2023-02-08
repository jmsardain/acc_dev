#DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"     
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}")/../../TrigHTTConfig/test" && pwd )"  

source $DIR/InputTestSetup.sh
export InputHTTRawHitFile=$HTTRawHitFile
export OutputHTTClusteredHitFile="httsim_clusteredhits_wrap.OUT.root"

athena --evtMax=10  TrigHTTInput/HTTRawToClusteredHitsWrapperAlg_jobOptions.py



