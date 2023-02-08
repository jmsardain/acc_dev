'''
@file HTTBankTags.py
@author Riley Xu - riley.xu@cern.ch
@date Feb 6th 2020

This file declares a set of tags that the bank service uses to select files.
This allows for a consistent selection of sectors, fit constants, and map files
for downstream uses, such as generating pattern banks or optimizing DC configuration.

The tags also store information about the bank generation configuration used to generate
the files, and represent a canonical configuration (the variables are used as defaults
in new bank generation jobs, when the tag is specified).

In general, tags should not be edited after creation for consistent results.

All options can be further overridden from the command line or job options. For
example, if you're working outside of lxplus but the tag references files on EOS,
you can simply just copy the files to some directory and then override the
bankDir variable. The bank service will still use the file names in the tag.
See HTTBankConfig.py, HTTBankGenConfig.py, and the top-level HTT README for details.
'''

# Template to add a new tag, and some description of the fields
empty_tag = {
    'name':     '',     # Name of this tag
    'package':  'bank', # Package identifier
    'mapTag':   '',     # Dependent map tag name

    ### Files
    'bankDir':                  '',     # Root directory of all the files referenced below
    'formatted':                False,  # If the filenames below have {} replacement fields (see HTTTagConfig)
    'constants_1st':            '',     # Filename for first stage constants
    'constants_2nd':            '',     # Filename for second stage constants
    'constantsNoGuess_1st':     [],     # Filenames for first stage no-guess missing hit constants
    'constantsNoGuess_2nd':     [],     # Filenames for second stage no-guess missing hit constants
    'sectorBank_1st':           '',     # Filename for the first stage sectors
    'sectorBank_2nd':           '',     # Filename for the second stage sectors
    'sectorSlices':             '',     # Filename for the sector slices
    'patternBank':              '',     # Filename for the ccache pattern banks

    ### Matrix Generation
    'WCmax':            0,  # Maximum number of wildcards in a sector/pattern

    ### Constant Generation
    'missHitsConsts':   False,  # Whether to generate constants for all permutations of missing hits

    ### Pattern Generation
    'beamspot':         [], # (x, y) of the beamspot for track inversion
    'd0alpha':          0,  # ? track inversion
    'WCplanes':         [], # Layers in which wildcards are allowed TODO add to matrix gen too
    'rndStreamName':    '', # ?
    'seed':             0,  # Default primary seed
    'seed2':            0,  # Default secondary seed

    ### DC Generation
    'maxPatts':         0,  # Maximum number of patterns to put in DC bank
    'nDCMax':           0,  # Maximum number of DC bits per pattern
    'nDC':              [], # Number of DC bits per layer
}


# Default tag to be used (should be updated to be the latest configuration)
defaultTag = 'EF_TaskForce_dev22'

# Listed with most recent first. Keys are the tag names.
HTTBankTags = {
        'EF_TaskForce_dev22': {
            ### Basic
            'name':              'EF_TaskForce_dev22',
            'package':           'bank',
            'mapTag':            'EF_TaskForce_dev22',
            ### Files
            'bankDir':           '/eos/atlas/atlascerngroupdisk/det-htt/HTTsim/ATLAS-P2-ITK-22-02-00/21.9.16/{regionName}/',
            'formatted':         True,
            'constants_1st':     'SectorBanks/corrgen_raw_8L.gcon',
            'constants_2nd':     'SectorBanks/corrgen_raw_13L_reg0_checkGood1.gcon',
            'sectorBank_1st':    'SectorBanks/sectorsHW_raw_8L.patt',
            'sectorBank_2nd':    'SectorBanks/sectorsHW_raw_13L_reg0_checkGood1.patt',
            'sectorSlices':      'SectorBanks/slices_8L.root',
            'patternBank':       'PatternBanks/ccache.T_dc_21111122_max4_3.75M.root',
            'constantsNoGuess_1st':     [
                    'SectorBanks/corrgen_raw_8L_skipPlane0.gcon',
                    'SectorBanks/corrgen_raw_8L_skipPlane1.gcon',
                    'SectorBanks/corrgen_raw_8L_skipPlane2.gcon',
                    'SectorBanks/corrgen_raw_8L_skipPlane3.gcon',
                    'SectorBanks/corrgen_raw_8L_skipPlane4.gcon',
                    'SectorBanks/corrgen_raw_8L_skipPlane5.gcon',
                    'SectorBanks/corrgen_raw_8L_skipPlane6.gcon',
                    'SectorBanks/corrgen_raw_8L_skipPlane7.gcon',
            ],
            'constantsNoGuess_2nd':     [
                    'SectorBanks/corrgen_raw_13L_skipPlane0.gcon',
                    'SectorBanks/corrgen_raw_13L_skipPlane1.gcon',
                    'SectorBanks/corrgen_raw_13L_skipPlane2.gcon',
                    'SectorBanks/corrgen_raw_13L_skipPlane3.gcon',
                    'SectorBanks/corrgen_raw_13L_skipPlane4.gcon',
                    'SectorBanks/corrgen_raw_13L_skipPlane5.gcon',
                    'SectorBanks/corrgen_raw_13L_skipPlane6.gcon',
                    'SectorBanks/corrgen_raw_13L_skipPlane7.gcon',
            ],
            ### Matrix Generation
            'WCmax':            2,
            ### Constant Generation
            'missHitsConsts':   False,
            ### Pattern Generation
            'WCplanes':         [True] * 8,
            'beamspot':         [0, 0],
            'd0alpha':          0,
            'rndStreamName':    'RANDOM',
            'seed':             42,
            'seed2':            1337,
            ### DC Generation
            'maxPatts':         3750000,
            'nDCMax':           4,
            'nDC':              [2,1,1,1,1,1,2,2],
        },
        'EF_TaskForce_dev23': {
            ### Basic
            'name':              'EF_TaskForce_dev23',
            'package':           'bank',
            'mapTag':            'EF_TaskForce_dev23',
            ### Files
            'bankDir':           '/eos/atlas/atlascerngroupdisk/det-htt/HTTsim/ATLAS-P2-ITK-23-00-01/21.9.15/{regionName}/',
            'formatted':         True,
            'constants_1st':     'SectorBanks/corrgen_raw_8L.gcon',
            'constants_2nd':     'SectorBanks/corrgen_raw_13L.gcon',
            'sectorBank_1st':    'SectorBanks/sectorsHW_raw_8L.patt',
            'sectorBank_2nd':    'SectorBanks/sectorsHW_raw_13L.patt',
            'sectorSlices':      'SectorBanks/slices_8L.root',
            'patternBank':       'PatternBanks/ccache.patterns.root',
            'constantsNoGuess_1st':     [
                    'SectorBanks/corrgen_raw_8L_skipPlane0.gcon',
                    'SectorBanks/corrgen_raw_8L_skipPlane1.gcon',
                    'SectorBanks/corrgen_raw_8L_skipPlane2.gcon',
                    'SectorBanks/corrgen_raw_8L_skipPlane3.gcon',
                    'SectorBanks/corrgen_raw_8L_skipPlane4.gcon',
                    'SectorBanks/corrgen_raw_8L_skipPlane5.gcon',
                    'SectorBanks/corrgen_raw_8L_skipPlane6.gcon',
                    'SectorBanks/corrgen_raw_8L_skipPlane7.gcon',
            ],
            'constantsNoGuess_2nd':     [
                    'SectorBanks/corrgen_raw_13L_skipPlane0.gcon',
                    'SectorBanks/corrgen_raw_13L_skipPlane1.gcon',
                    'SectorBanks/corrgen_raw_13L_skipPlane2.gcon',
                    'SectorBanks/corrgen_raw_13L_skipPlane3.gcon',
                    'SectorBanks/corrgen_raw_13L_skipPlane4.gcon',
                    'SectorBanks/corrgen_raw_13L_skipPlane5.gcon',
                    'SectorBanks/corrgen_raw_13L_skipPlane6.gcon',
                    'SectorBanks/corrgen_raw_13L_skipPlane7.gcon',
            ],
            ### Matrix Generation
            'WCmax':            2,
            ### Constant Generation
            'missHitsConsts':   False,
            ### Pattern Generation
            'WCplanes':         [True] * 8,
            'beamspot':         [0, 0],
            'd0alpha':          0,
            'rndStreamName':    'RANDOM',
            'seed':             42,
            'seed2':            1337,
            ### DC Generation
            'maxPatts':         3750000,
            'nDCMax':           4,
            'nDC':              [2,1,1,1,1,1,2,2],
        },
        '21.9.2': {
            ### Basic
            'name':              '21.9.2',
            'package':           'bank',
            'mapTag':            'TDRConf-v1',
            ### Files
            'bankDir':           '/eos/atlas/atlascerngroupdisk/det-htt/HTTsim/ATLAS-P2-ITK-17-06-00/21.9.2/{regionName}/',
            'formatted':         True,
            'constants_1st':     'SectorBanks/corrgen_raw_8L.gcon',
            'constants_2nd':     'SectorBanks/corrgen_raw_13L.gcon',
            'sectorBank_1st':    'SectorBanks/sectorsHW_raw_8L.patt',
            'sectorBank_2nd':    'SectorBanks/sectorsHW_raw_13L.patt',
            'sectorSlices':      'SectorBanks/slices_8L.root',
            'patternBank':       'PatternBanks/ccache.patterns.root',
            'constantsNoGuess_1st':     [
                    'SectorBanks/corrgen_raw_8L_skipPlane0.gcon',
                    'SectorBanks/corrgen_raw_8L_skipPlane1.gcon',
                    'SectorBanks/corrgen_raw_8L_skipPlane2.gcon',
                    'SectorBanks/corrgen_raw_8L_skipPlane3.gcon',
                    'SectorBanks/corrgen_raw_8L_skipPlane4.gcon',
                    'SectorBanks/corrgen_raw_8L_skipPlane5.gcon',
                    'SectorBanks/corrgen_raw_8L_skipPlane6.gcon',
                    'SectorBanks/corrgen_raw_8L_skipPlane7.gcon',
            ],
            'constantsNoGuess_2nd':     [
                    'SectorBanks/corrgen_raw_13L_skipPlane0.gcon',
                    'SectorBanks/corrgen_raw_13L_skipPlane1.gcon',
                    'SectorBanks/corrgen_raw_13L_skipPlane2.gcon',
                    'SectorBanks/corrgen_raw_13L_skipPlane3.gcon',
                    'SectorBanks/corrgen_raw_13L_skipPlane4.gcon',
                    'SectorBanks/corrgen_raw_13L_skipPlane5.gcon',
                    'SectorBanks/corrgen_raw_13L_skipPlane6.gcon',
                    'SectorBanks/corrgen_raw_13L_skipPlane7.gcon',
            ],
            ### Matrix Generation
            'WCmax':            2,
            ### Constant Generation
            'missHitsConsts':   False,
            ### Pattern Generation
            'WCplanes':         [True] * 8,
            'beamspot':         [0, 0],
            'd0alpha':          0,
            'rndStreamName':    'RANDOM',
            'seed':             42,
            'seed2':            1337,
            ### DC Generation
            'maxPatts':         3750000,
            'nDCMax':           4,
            'nDC':              [2,1,1,1,1,1,2,2],
        },
        'jahreda_20-07-20' : {
            'name':              'jahreda_20-07-20',
            'package':           'bank',
            'mapTag':            'TDRConf-v1',
            'bankDir':           '/eos/atlas/atlascerngroupdisk/det-htt/HTTsim/ATLAS-P2-ITK-17-06-00/21.9.2/eta0103phi0305/SectorBanks/20-07-20-jahreda/',
            'formatted':         False,
            'constants_1st':     'corrgen_raw_8L_reg0_checkGood1.gcon',
            'constants_2nd':     'corrgen_raw_13L_reg0_checkGood1.gcon',
            'sectorBank_1st':    'sectorsHW_raw_8L_reg0_checkGood1.patt',
            'sectorBank_2nd':    'sectorsHW_raw_13L_reg0_checkGood1.patt',
            'sectorSlices':      'slices_8L_reg0.root',
            'patternBank':       'ccache.T_dc_21111122_max4_3.75M.root',
            'constantsNoGuess_1st':     [
                    'corrgen_raw_8L_reg0_checkGood1_skipPlane0.gcon',
                    'corrgen_raw_8L_reg0_checkGood1_skipPlane1.gcon',
                    'corrgen_raw_8L_reg0_checkGood1_skipPlane2.gcon',
                    'corrgen_raw_8L_reg0_checkGood1_skipPlane3.gcon',
                    'corrgen_raw_8L_reg0_checkGood1_skipPlane4.gcon',
                    'corrgen_raw_8L_reg0_checkGood1_skipPlane5.gcon',
                    'corrgen_raw_8L_reg0_checkGood1_skipPlane6.gcon',
                    'corrgen_raw_8L_reg0_checkGood1_skipPlane7.gcon',
            ],
            'constantsNoGuess_2nd':     [
                    'corrgen_raw_13L_reg0_checkGood1_skipPlane0.gcon',
                    'corrgen_raw_13L_reg0_checkGood1_skipPlane1.gcon',
                    'corrgen_raw_13L_reg0_checkGood1_skipPlane2.gcon',
                    'corrgen_raw_13L_reg0_checkGood1_skipPlane3.gcon',
                    'corrgen_raw_13L_reg0_checkGood1_skipPlane4.gcon',
                    'corrgen_raw_13L_reg0_checkGood1_skipPlane5.gcon',
                    'corrgen_raw_13L_reg0_checkGood1_skipPlane6.gcon',
                    'corrgen_raw_13L_reg0_checkGood1_skipPlane7.gcon',
            ],
            ### Matrix Generation
            'WCmax':            2,
            ### Constant Generation
            'missHitsConsts':   False,
            ### Pattern Generation
            'WCplanes':         [True] * 8,
            'beamspot':         [0, 0],
            'd0alpha':          0,
            'rndStreamName':    'RANDOM',
            'seed':             42,
            'seed2':            1337,
            ### DC Generation
            'maxPatts':         3750000,
            'nDCMax':           4,
            'nDC':              [2,1,1,1,1,1,2,2],
        },
}

