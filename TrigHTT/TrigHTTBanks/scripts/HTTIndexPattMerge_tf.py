#!/usr/bin/env python

'''
HTTIndexPattMerge_tf.py:

Transform to merge pattern banks in the index format together into a single file.

Usage:
    HTTIndexPattMerge_tf.py \
            --inputs "file1.root,file2.root,..." \
            --output "out.root"
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

    sys.exit(trf.exitCode)


# ___________________________________________________________________________ #
def getTransform():
# Get the base transform with all arguments added
    trf = transform(executor = athenaExecutor(name = 'HTTIndexPattMergeAlgo',
                                              skeletonFile = 'TrigHTTBanks/skeleton.HTTIndexPattMerge.py'))
    addAthenaArguments(trf.parser)
    addHTTIndexPattMergeAlgoArgs(trf.parser)
    return trf



# ___________________________________________________________________________ #
def addHTTIndexPattMergeAlgoArgs(parser):
    # Add a specific HTT argument group
    parser.defineArgGroup('HTTIndexPattMergeAlgo', 'Options for HTTIndexPattMergeAlgo for merging pattern banks')

    # Enable easy copy-paste from C++ argument initializers
    def declareProperty(argName, argType, helpText=""):
        parser.add_argument('--' + argName,
                type=trfArgClasses.argFactory(argType, runarg=True),
                help=helpText,
                group='HTTIndexPattMergeAlgo')

    declareProperty("inputs", trfArgClasses.argList);
    declareProperty("output", trfArgClasses.argString);


if __name__ == '__main__':
    main()
