# Instructions to run HTTSim in athena

First setup:
```
mkdir HTTSim
cd HTTSim
mkdir athena
cd athena
setupATLAS # alternatively, source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh
lsetup git
git init
git sparse-checkout set Projects/TrigHTT/ Trigger/TrigHTT/
git remote add origin ssh://git@gitlab.cern.ch:7999/atlas-tdaq-ph2upgrades/atlas-tdaq-htt/tdaq-htt-offline/athena.git
git fetch
git checkout 21.9
cd .. # You should now be back in the HTTSim directory
mkdir build workspace run
cd build
asetup 21.9.16,Athena,here
cmake ../athena/Projects/TrigHTT
make -j 32
source build/x86_64-*-gcc62-opt/setup.sh
```

Once everything is setup, you would only need to do (to setup and to compile):

```
setupATLAS
cd build
asetup 21.9.16,Athena,here
cmake ../athena/Projects/TrigHTT
make
cd ../
source build/x86_64-*-gcc62-opt/setup.sh
```


To run the code:

```
# Using the DV+jets sample - filtered
## files were given by E. Cheu.
## I also have a copy in /eos/user/j/jzahredd/HTTSim/data/, ping me if you want access
InFile="file1,file2"

NAME="test"
echo $HOSTNAME
SECONDS=0
mkdir rundir-doublet-$NAME
cd rundir-doublet-$NAME
HTTLogicalHitsToAlg_tf.py \
    --maxEvents 100 \ ## to run only 100 events
    --InFileName $InFile \
    --mapTag 'EF_TaskForce_dev22' \
    --algoTag hough \
    --hough True \
    --lrt True \
    --lrt_use_basicHitFilter True \
    --OutFileName doublet-$NAME.root \
    --sampleType LLPs \
    --lrt_use_doubletHT True
cd ..
```
