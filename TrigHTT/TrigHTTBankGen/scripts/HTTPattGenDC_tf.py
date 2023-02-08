#!/usr/bin/env python

'''
HTTPattGenDC_tf.py:

Transform to generate a DC pattern bank from a TSP bank.
See HTTPattGenDCAlgo.h for more information.

Usage:
    HTTPattGenDC_tf.py \
            --inbank 'index.in.root' \
            --nDC '1,0,0,0,0,0,1,1' \

        # optional arguments

            --outbank 'ccache.out.root' \
            --maxPatts 3750000 \
            --nDCMax 3 \
            --dumpFile "patt.root"
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

@stdTrfExceptionHandler
@sigUsrStackTrace

# ___________________________________________________________________________ #
def main():

    trf = getTransform()
    trf.parseCmdLineArgs(sys.argv[1:])
    trf.execute()
    trf.generateReport()

    sys.exit(trf.exitCode)



# ___________________________________________________________________________ #
def getTransform():
# Get the base transform with all arguments added
    trf = transform(executor = athenaExecutor(name = 'HTTPattGenDC',
                                              skeletonFile = 'TrigHTTBankGen/skeleton.HTTPattGenDC.py'))
    addAthenaArguments(trf.parser)
    addHTTBanksArgs(trf.parser)
    addHTTPattGenDCArgs(trf.parser)
    return trf



# ___________________________________________________________________________ #
def addHTTPattGenDCArgs(parser):
    # Add a specific HTT argument group
    parser.defineArgGroup('HTTPattGenDC', 'Configuration of HTTPattGenDCAlgo for generating DC pattern banks')

    # Enable easy copy-paste from C++ argument initializers
    def declareProperty(argName, helpText, argType):
        parser.add_argument('--' + argName,
                type=trfArgClasses.argFactory(argType, runarg=True),
                help=helpText,
                group='HTTPattGenDC')

    declareProperty("inbank", "Input bank in the indexed format", trfArgClasses.argString)
    declareProperty("outbank", "Filepath for the output bank in the ccache format. A default filename will be generated if not specified (Recommended)", trfArgClasses.argString)
    declareProperty("dumpFile", "Optional filepath to dump list of patterns", trfArgClasses.argString)


if __name__ == '__main__':
    main()
