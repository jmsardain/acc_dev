# TrigHTT

Last update: April 27, 2021

## Packages

In order of dependencies:

1. TrigHTTUtils: Basic utilities to be used by all other packages
2. TrigHTTObjects: Container classes for things like patterns, roads, etc.
3. TrigHTTConfig: Configuration manager
4. TrigHTTMaps: Map classes and I/O
7. TrigHTTInput: Input formating
8. TrigHTTBanks: Constants (pattern banks, fit constants, etc.) classes and I/O
9. TrigHTTBankGen: Generating constants
10. TrigHTTAlgorithms: Simulation algorithms, like pattern matching

## Cloning and building the repository

1. Create a top-level working directory.
    ```sh
    mkdir HTTSim
    cd HTTSim
    ```
2. Prepare git directory for athena (you can use HTTPS or Kerberos instead if you want)
    ```sh
    mkdir athena
    cd athena
    setupATLAS # alternatively, source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh
    lsetup git
    git init
    git sparse-checkout set Projects/TrigHTT/ Trigger/TrigHTT/
    git remote add origin ssh://git@gitlab.cern.ch:7999/atlas-tdaq-ph2upgrades/atlas-tdaq-htt/tdaq-htt-offline/athena.git
    ```
3. Pull the directory
    ```sh
    git fetch
    git checkout 21.9
    ```
4. Prepare a build directory
    ```sh
    cd .. # You should now be back in the HTTSim directory
    mkdir -p build
    cd build
    asetup 21.9.16,Athena,here
    ```
5. Run the build (you should be in HTTSim/build/ with Athena setup as in the previous step)
    ```sh
    cmake ../athena/Projects/TrigHTT
    make # You may need to add "-j 8" to enable multi-threading
    ```
6. Test the build (this will require eos access)
    ```sh
    ctest -j 8
    ```

To recompile after changes, simply repeat step 5.


## Running the main transform

This runs our main transform, which takes raw hits from HTT [wrapper files](#wrapper-input-files) and goes through the entire processing chain, including clustering, hit filtering, mapping, road finding, tracking, and monitoring.
From the top-level working directory (`HTTSim/` if you followed the build instructions),

1. Source the build
    ```sh
    source build/x86_64-*-gcc62-opt/setup.sh
    ```
2. Create a work dir and a run dir
    ```sh
    mkdir workspace
    cd workspace
    mkdir rundir
    ```
3. Create an executable file `run`
    ```sh
    touch run
    chmod +x run
    ```
4. Copy the following contents into that file.
    ```sh
    #!/bin/bash

    InFile='/eos/atlas/atlascerngroupdisk/det-htt/HTTsim/ATLAS-P2-ITK-22-02-00/21.9.16/eta0103phi0305/Wrappers/singlemu_invPtFlat1_10k_wrap.root'

    cd rundir/
    HTTLogicalHitsToAlg_tf.py \
        --maxEvents 5 \
        --InFileName $InFile \
    ```
5. Run the script. Note you must have eos mounted for this to work (i.e. lxplus)
    ```sh
    ./run
    ```

There are a multitude of other options you can add to the transform, run `HTTLogicalHitsToAlg_tf.py -h` to see a list.

### Running without eos

1. From the top-level `HTTSim/` directory, create a directory to store the bank files
    ```sh
    mkdir ATLAS-22-02-00_eta0103
    cd ATLAS-22-02-00_eta0103

    mkdir SectorBanks
    mkdir PatternBanks
    ```
2. Copy the relevant files using your lxplus login (change the `rixu`!). Make sure you keep the same subdirectory structure and file names.
    ```sh
    eosdir='/eos/atlas/atlascerngroupdisk/det-htt/HTTsim/ATLAS-P2-ITK-22-02-00/21.9.16/eta0103phi0305'
    lxplus='rixu@lxplus.cern.ch' # FIXME

    sectors='SectorBanks/*8L*'
    patterns='PatternBanks/ccache.patterns.root'
    wrapper='/eos/atlas/atlascerngroupdisk/det-htt/HTTsim/ATLAS-P2-ITK-22-02-00/21.9.16/eta0103phi0305/Wrappers/singlemu_invPtFlat1_10k_wrap.root'

    scp $lxplus:$eosdir/$sectors SectorBanks/
    scp $lxplus:$eosdir/$patterns PatternBanks/
    scp $lxplus:$wrapper ./
    ```
3. In your run script, update the path to the wrapper and add a new `bankDir` argument to point to your new local directory
    ```sh
    InFile='../../ATLAS-22-02-00_01eta03/singlemu_invPtFlat1_10k_wrap.root'

    HTTLogicalHitsToAlg_tf.py \
        #...
        --bankDir '../../ATLAS-22-02-00_01eta03/' \
    ```

Note: If these instructions ever go out of date, simply look at the bank tag for the newest files.

### Hough transform

#### Relevant files

* `TrigHTTAlgorithms/HTTHoughTransformTool.h`: Tool for running the hough transform
* `TrigHTTAlgorithms/HTTHough1DShiftTool.h`: Tool for running Elliot's version of the hough transform

#### Running

Add one of the following flags to `HTTLogicalHitsToAlg_tf.py` to use the respective method.
```sh
--algoTag hough # Normal hough transform
--algoTag hough_1d # Elliot's 1d shift version
```
You can see the configuration details by looking at the [tags](#htt-configuration) in `TrigHTTConfig/HTTAlgorithmTags`. As of April 15th 2021, the `hough` tag uses a 216x216 accumulator, 6x2 pad bins, 6 z-slices, and a [2,1,0\*6] extension.

#### Configurables

There are a multitude of other options you can add to the transform, run `HTTLogicalHitsToAlg_tf.py -h` to see a list. For example, to change to the 64x216 configuration, you can use
```sh
--algoTag hough
--xBins 64
--yBins 216
--hitExtend_x , # empty list
--xBufferBins 2
--subrmap htt_configuration/zslicemaps/ATLAS-P2-ITK-22-02-00/eta0103phi0305_KeyLayer-strip_barrel_2_extra03_trim_0_001_NSlices-19.rmap
```

#### Image plots

The transform tools have a helper function `drawImage()` that will draw the transform parameter space and hit lines as a 2D histogram.
These are stored in ROOT files labeled by the name of the tool, and will be written to the rundir. A utility script exists to convert the plots to pngs.
```sh
Hough_plots.py rundir/HoughTransform_1.root
```

### Hit Filtering
The HTTHitFilteringTool provides two kinds of hit/cluster filtering, random (pixel and strip) and stub (strip only)

#### Stub Filtering
Enable with `-HitFiltering True --doStubs True --stubCutsFile path/to/stub_cuts.txt`. There are various sets of stub cut files at /afs/cern.ch/work/c/ckaldero/public/HTTSim/stubCutsFiles/. The ones called "intermediate" and "tight" in recent presentations (as of 27/04/21) are stub_cuts_50evt_rings_20210331_1-1p5_5pct.txt and stub_cuts_50evt_rings_20210331_2-3_5pct.txt. Among this family of stub cut files, "1-1p5" means they were derived for true tracks with 1 < pT < 1.5, and "5pct" means that the stub filtering rejects 5% of the clusters associated with those tracks.

The plane map (pmap) is used to determine which layers can be used for filtering (requires stereo layers on both sides of a stave, strip only). Hits/clusters (taken from event header, so in theory agnostic to whether clustering has been done, but untested in the no-clustering case) pass filtering if they have a partner in the adjacent stereo layer within a delta phi cut set by the stub cut file (varies with layer / disk / ring / etc).

To do:
1. Complete the option to apply the cut firmware-like on nStrips (ie, local phi coordinate)
2. Only look at hit/cluster pairs within designated HCC* chips (two strip rows), as is planned for firmware

#### Random removal
Enable with `--HitFiltering True --doRandomRemoval True --pixelHitRmFrac 0.05 --stripClusRmFrac 0.02` to randomly remove 5% of hits from pixel layers and 2% of clusters from strip layers. `pixelClusRmFrac` and `stripHitRmFrac` also exist (and can be set together). This allows simple emulation of radiation damage.


## Wrapper (input) Files

Wrapper files are stripped-down RDOs stored in HTT object classes, and created by tools such as `TrigHTTInput/HTT_SGToRawHitsTool.h`. These are the inputs to all downstream components, specifically `TrigHTTAlgorithms`. A list of wrapper files is being tracked here: https://twiki.cern.ch/twiki/bin/viewauth/Atlas/HTTForEFTTaskForces

## HTT Configuration

All user-configurable properties in HTT scripts are collected in a tag system. A tag is a simple python dictionary that maps properties to values, with a unique name to identify it. Each package has its own set of tags, which are listed in the `TrigHTTConfig/python/HTTPackageTags.py` files. The tags can be retrieved by using the functions in `TrigHTTConfig/python/HTTTagConfig.py`.
Each package also provides a configuration file that applies the tags to the Athena components.

While the tag, being just a python dictionary, can be retrieved and edited manually, one should only use the configuration functions for maintainability.

### Example Usage
To set up components in the HTTBanks package, in the skeleton or job option file we can use the following:
```py
import TrigHTTConfig.HTTTagConfig as HTTTagConfig
import TrigHTTBanks.HTTBankConfig as HTTBankConfig

(map_tag, bank_tag, _) = HTTTagConfig.getTags('bank', {})
bankSvc = HTTBankConfig.addBankSvc(map_tag, bank_tag)
```
This retrieves the default tag listed in `HTTBankTags.py`, then creates the default-named `TrigHTTBankSvc` object, with the property-values in the tag applied. See the docstrings of each function for more information.

### Overriding Tags
To override tag properties, one can supply an override dictionary to the `getTags()` functions. The dictionary keys should be the properties to override, matching the keys in the tag. For example, if we need to change the bank directory to a local directory instead of eos, we can do
```py
HTTTagConfig.getTags('bank', {'bankDir': '/my/local/directory/'})
```
The choice of tag can also be changed with a `'bankTag': tagName` entry.

### Using Command Line Arguments
Modifying skeleton or job option files to apply overrides is generally not recommended because these are tracked by git, and you may accidentally push personal changes to other users. Instead, we can use command line arguments, although they're only available for transforms. `HTTConfig/python/parseRunArgs.py` provides functions to add arguments to transform parsers and create the override dictionaries from the `runArgs` object. Note, this file collects command line arguments **for all packages**. As an example of using these functions, in the transform file we have:
```py
from TrigHTTUtils.parseRunArgs import addHTTBanksArgs
trf = transform(executor = athenaExecutor(...))
addHTTBanksArgs(trf.parser)
```
and in the skeleton file:
```py
from TrigHTTConfig.parseRunArgs import parseHTTArgs
(map_tag, bank_tag, algo_tag) = HTTTagConfig.getTags('algo', parseHTTArgs(runArgs))
```
The easiest way to see what run arguments are available is to just run with the `-h` flag.

### Notes to Developers
For the sake of maintainability, it is helpful to enforce that property names are consistent between all locations. This includes
1. The Athena component property created with `declareProperty()` in `.cxx` files
2. The key name in the tag
3. The command line argument name


