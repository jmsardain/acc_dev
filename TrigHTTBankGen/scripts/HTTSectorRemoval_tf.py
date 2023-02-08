#!/usr/bin/env python

'''
HTTSectorRemoval_tf.py:

See HTTSectorRemovalAlgo.h for more information.

Usage:
    HTTSectorRemoval_tf.py \
            --input 'matrix.in.root' \

        # optional arguments

            --method "discard_wildcards" \
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
    trf = transform(executor = athenaExecutor(name = 'HTTSectorRemoval',
                                              skeletonFile = 'TrigHTTBankGen/skeleton.HTTSectorRemoval.py'))
    addAthenaArguments(trf.parser)
    addHTTSectorRemovalArgs(trf.parser)
    return trf



# ___________________________________________________________________________ #
def addHTTSectorRemovalArgs(parser):
    # Add a specific HTT argument group
    parser.defineArgGroup('HTTSectorRemoval', 'Configuration of HTTSectorRemovalAlgo')

    # Enable easy copy-paste from C++ argument initializers
    def declareProperty(argName, helpText, argType):
        parser.add_argument('--' + argName,
                type=trfArgClasses.argFactory(argType, runarg=True),
                help=helpText,
                group='HTTSectorRemoval')

    declareProperty("input", "Input path to matrix file", trfArgClasses.argString)
    declareProperty("method", "Main method to use. Can be: \n\tdiscard_wildcards -- TODO", trfArgClasses.argString)


if __name__ == '__main__':
    main()
