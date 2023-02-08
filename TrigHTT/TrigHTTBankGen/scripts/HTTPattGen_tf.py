#!/usr/bin/env python

'''
HTTPattGen_tf.py:

Transform to generate a TSP pattern bank. There are two main methods: track inversion and truth tracks.
Use the --method flag to select the method. See HTTPattGenTool_*.h for more information.

Usage:
    HTTPattGen_tf.py \
            --maxEvents 100 \

        # optional arguments

            --output "index.out.root" \
            --WC_max 2 \
            --WC_planes 1,1,1,1,1,1,1,1 \
            --rndStreamName RANDOM

    Using truth tracks:
            --method truth \
            --truth "wrapper.truth.root" \
            --rawInput True \

    Using track inversion:
            --method TI \


'''


import argparse
import sys
import time
import traceback
import logging

# Setup core logging here
from PyJobTransforms.trfLogger import msg

from PyJobTransforms.trfExitCodes import trfExit
from PyJobTransforms.transform import transform
from PyJobTransforms.trfExe import athenaExecutor
from PyJobTransforms.trfArgs import addAthenaArguments
from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace

import PyJobTransforms.trfExceptions as trfExceptions
import PyJobTransforms.trfArgClasses as trfArgClasses

from TrigHTTConfig.parseRunArgs import addHTTBanksArgs
from TrigHTTConfig.parseRunArgs import addHTTMapsArgs

@stdTrfExceptionHandler
@sigUsrStackTrace

# ___________________________________________________________________________ #
def main():

    trf = getTransform()
    trf.parseCmdLineArgs(sys.argv[1:])
    trf.execute()
    trf.generateReport()

    msg.info("%s stopped at %s, trf exit code %d" % (sys.argv[0], time.asctime(), trf.exitCode))
    sys.exit(trf.exitCode)


# ___________________________________________________________________________ #
def getTransform():
# Get the base transform with all arguments added
    trf = transform(executor = athenaExecutor(name = 'HTTPattGen',
                                              skeletonFile = 'TrigHTTBankGen/skeleton.HTTPattGen.py'))
    addAthenaArguments(trf.parser)
    addHTTBanksArgs(trf.parser)
    addHTTPattGenArgs(trf.parser)
    addHTTMapsArgs(trf.parser)
    return trf



# ___________________________________________________________________________ #
def addHTTPattGenArgs(parser):
    # Add a specific HTT argument group
    parser.defineArgGroup('HTTPattGen', 'Options for HTTPattGenAlgo for pattern generation')

    # Enable easy copy-paste from C++ argument initializers
    def declareProperty(argName, argType, helpText=""):
        parser.add_argument('--' + argName,
                type=trfArgClasses.argFactory(argType, runarg=True),
                help=helpText,
                group='HTTPattGen')

    declareProperty("method", trfArgClasses.argString, "Pattern generation method: \"TI\" for track inversion, \"truth\" for truth tracks.");
    declareProperty("output", trfArgClasses.argString, "An automatic filename will be generated if not specified (Recommended)");
    declareProperty("truth", trfArgClasses.argString, "Path to wrapper file containing hits from truth tracks");
    declareProperty("rawInput", trfArgClasses.argBool, "whether truth file contains raw hits or logical hits");


if __name__ == '__main__':
    main()
