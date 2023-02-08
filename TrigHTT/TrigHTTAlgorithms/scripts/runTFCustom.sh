#!/bin/sh

DIR="/eos/atlas/atlascerngroupdisk/det-htt/HTTsim/ATLAS-P2-ITK-22-02-00/21.9.16"

REGION=0
RUNSAP=False
pileup=0
#PU=1 particule witth PU
pdg=13
NEVENTS=1000
withPU=False
fixpt=0
regionName=''
outDir="."
secondStage=False

pattern=1
#1 = 4DC
#2 = 9DC
#3 = optimized
#4 = 4DC + majority1
#6 = 6DC


while getopts ":n:p:u:w:m:o:e:l:d:b:t:g:f:s:a:z:r:" opt; do
    case "$opt" in
        n)  echo "-n was triggered, Parameter: events=$OPTARG" >&2
            NEVENTS=$OPTARG
            ;;
        r)  echo "-r was triggered, Parameter: REGION=$OPTARG" >&2
            REGION=$OPTARG
            ;;
        p)  echo "-p was triggered, Parameter: particle=$OPTARG" >&2
            pdg=$OPTARG
            ;;
        u)  echo "-s was triggered, Parameter: pileup=$OPTARG" >&2
            pileup=$OPTARG
            ;;
        d)  echo "-d was triggered, Parameter: pattern=$OPTARG" >&2
            pattern=$OPTARG
            ;;
        f) echo "-f was triggered, Parameter: fixpt=$OPTARG" >&2
            fixpt=$OPTARG
            ;;
        o) echo "-o was triggered, Parameter: outDir=$OPTARG" >&2
            outDir=$OPTARG
            ;;
        \?) echo "Invalid option: -$OPTARG"
            ;;
    esac
done

case $pdg in
    '11')  particle="electron";;
    '13')  particle="muon";;
    '211') particle="pion";;
    '0')   particle="ttbar";;
    '1')   particle="jets";;
esac


case $REGION in
    '0') regionName="eta0103phi0305";;
    '1') regionName="eta0709phi0305";;
    '3') regionName="eta2022phi0305";;
esac
echo $regionName
#OutputFile="singlemu_invPtFlat1_loghits_N${NEVENTS}_R${REGION}_P${PU}.root"
OutputDir="${outDir}/reg${REGION}/C${pattern}/${particle}_P${pileup}/"
if [ ${fixpt} != 0 ]; then
    OutputDir="${outDir}/reg${REGION}/C${pattern}_pt${fixpt}/${particle}_P${pileup}/"
fi

mkdir -p ${OutputDir}
cd  ${OutputDir}

OutputFile="HTT${particle}_patt${pattern}_N${NEVENTS}_R${REGION}_P${pileup}.root"

maps="htt_configuration/map_files/rmaps/${regionName}_ATLAS-P2-ITK-22-02-00.rmap"


if [ "$REGION" == 0 ]; then
    WrapperDir="${DIR}/eta0103phi0305/Wrappers/"
#    BANKDIR="/afs/cern.ch/user/j/jahreda/det-htt/HTTsim/user/jahreda/region0-march30/"
    BANKDIR="${DIR}/eta0103phi0305/TFBanks/region0-march30/"
    if  [ "$pileup" ==  0 ] ; then
        secondStage=True
    fi

elif [ "$REGION" == 1 ]; then
    WrapperDir="${DIR}/eta0709phi0305/Wrappers/"
    #BANKDIR1="/eos/atlas/atlascerngroupdisk/det-htt/HTTsim/user/jahreda/region1-official-temp/"
    #BANKDIR="/eos/atlas/atlascerngroupdisk/det-htt/HTTsim/user/jahreda/region1-april5/"
    BANKDIR="${DIR}/eta0709phi0305/TFBanks/region1-april5/"
elif [ "$REGION" == 3 ]; then
    WrapperDir="${DIR}/eta2022phi0305/Wrappers/"
    #old tag
    #WrapperDir="${DIR}/eta2022phi0305/Wrappers/singleMuons_oldTag"
    maps="htt_configuration/map_files/rmaps/eta2022phi0305_ATLAS-P2-ITK-22-02-00_fromWrapper_trim0p08_barcodeCut.rmap"
    #maps="rmaps/eta2022phi0305_ATLAS-P2-ITK-22-02-00_fromWrapper_trim0p08_barcodeCut.rmap"
    #BANKDIR="/eos/atlas/atlascerngroupdisk/det-htt/HTTsim/user/jahreda/region3-april4/"
    #BANKDIR="/eos/atlas/atlascerngroupdisk/det-htt/HTTsim/user/jahreda/region3-april19_trim1/"
    ##BANKDIR="/eos/atlas/atlascerngroupdisk/det-htt/HTTsim/user/jahreda/region3-april19_trim8/"
    #BANKDIR="/afs/cern.ch/user/j/jahreda/det-htt/HTTsim/user/jahreda/region3-april19_trim1/"
    #BANKDIR="${DIR}/eta2022phi0305/TFBanks/region3_may3_trim8/"
    BANKDIR="${DIR}/eta2022phi0305/TFBanks/region3_may3_notrim/"
    #BANKDIR="${DIR}/eta2022phi0305/TFBanks/region3-april19_trim1/"

fi



# chose the patterns
case $pattern in
    #---> 4DCbit
    '1')  BANK="ccache.T_dc_21111122_max4_3.75M.root";;
    #---> 9DCbit
    '2')  BANK="ccache.T_dc_21111122_max9_3.75M.root";;
    # pt1-optmized
    '3')  BANK="patternBank_ATLAS-P2-ITK-22-02-00_eta0.10-0.30_phi0.30-0.50_6of8_pt1-400_90M-Y-pix+stripsCentre2-mixed21111122max6-recMar21-minpatpt4-use9N2M.root"
        if [ "$REGION" == 3 ]; then
            BANK="patternBank_ATLAS-P2-ITK-22-02-00_eta2.00-2.20_phi0.30-0.50_6of8_pt1-400_82M-z150-pix+strips-ecCentre-mixed21111122max9-Apr21-minpatpt4-use9N2M.root"
        fi
        ;;
    # pt2-optimized
    '4')  BANK="patternBank_ATLAS-P2-ITK-22-02-00_eta0.10-0.30_phi0.30-0.50_6of8_pt1-400_90M-Y-pix+stripsCentre2-mixed21111122max6-recMar21-minpatpt4-use9N2M-2GeV.root";;
    '5')  BANK="ccache.T_dc_21111122_max11_3.75M.root";;
    '6')  BANK="ccache.T_dc_21111122_max6_3.75M.root";;
    '7')  BANK="ccache.T_dc_21111122_max1_3.75M.root";;

esac



sample=''

# inputs
if [ ${pdg} == 13 ]; then
    InputFileName="singlemu_invPtFlat1_1M_wrap.root"
    DirPU200Name="muonInvPtFlat1.mu200"
    DirPU200="${WrapperDir}/${DirPU200Name}"
    InputHTTRawHitFilePU=`ls -p -m $DirPU200/* |tr -d '\n'`
    sample='singleMuons'
elif [ ${pdg} == 11 ]; then
    InputFileName="singleele_invPtFlat1_10k_wrap.root"
    DirPU200Name="eleInvPtFlat1.mu200"
    DirPU200="${WrapperDir}/${DirPU200Name}"
    InputHTTRawHitFilePU=`ls -p -m $DirPU200/* |tr -d '\n'`
    #InputHTTRawHitFilePU="${WrapperDir}/singleele_invPtFlat1_10k_mu200.root"
    sample='singleElectrons'
elif [ ${pdg} == 211 ]; then
    InputFileName="singlepion_invPtFlat1_10k_wrap.root"
    DirPU200Name="pionInvPtFlat1.mu200"
    DirPU200="${WrapperDir}/${DirPU200Name}"
    InputHTTRawHitFilePU=`ls -p -m $DirPU200/* |tr -d '\n'`
    #InputHTTRawHitFilePU="${WrapperDir}/"
    sample='singlePions'
elif [ ${pdg} == 0 ]; then
    Dirttbar="${DIR}/etaALLphiALL/Wrappers/user.martyniu.apr13.ttbar.mu200.21.9.16_v1_EXT0"
    InputHTTRawHitFileTT=`ls -p -m $Dirttbar/* |tr -d '\n'`
    InputFile=${InputHTTRawHitFileTT}
    sample='skipTruth'
    withPU=True
fi

if [ ${pdg} == 13 ]; then
    if [ ${fixpt} == 10 ]; then
        InputFileName="singlemu_FixedPt10_20k_wrap.root"
    elif [ ${fixpt} == 1 ]; then
        InputFileName="singlemu_FixedPt1_20k_wrap.root"
    elif [ ${fixpt} == 2 ]; then
        InputFileName="singlemu_FixedPt2_20k_wrap.root"
    fi
fi



# final settings, including the PU
if [ ${pdg} != 0 ]; then
    if  [ "$pileup" ==  0 ] ; then
        InputHTTRawHitFile="${WrapperDir}/${InputFileName}"
        InputFile=${InputHTTRawHitFile}
        withPU=False
    elif  [ "$pileup" ==  1 ] ; then
        InputFile=${InputHTTRawHitFilePU}
        withPU=True
    fi
fi


echo "RUNNING ${NEVENTS} events on Region ${REGION} with PU=${pileup}: File=${InputFile},  Bank=${BANKDIR} secondStage=${secondStage}"



#set -x  # show command

HTTLogicalHitsToAlg_tf.py \
--maxEvents=${NEVENTS} \
--InFileName ${InputFile} \
--OutFileName ${OutputFile} \
--mapTag 'EF_TaskForce_dev22' \
--bankDir ${BANKDIR} \
--sectorsAsPatterns ${RUNSAP} \
--constants_1st "corrgen_raw_8L_reg${REGION}_checkGood1.gcon" \
--sectorSlices "slices_8L_reg${REGION}.root" \
--sectorBank_1st "sectorsHW_raw_8L_reg${REGION}_checkGood1.patt" \
--patternBank ${BANK} \
--region ${REGION} \
--sampleType  $sample \
--withPU $withPU  \
--rmap $maps \
--writeOutputData False \
--constants_2nd "corrgen_raw_13L_reg0_checkGood1.gcon" \
--sectorBank_2nd "sectorsHW_raw_13L_reg0_checkGood1.patt" \
--secondStage $secondStage \
| tee "out_${particle}_patt${pattern}_N${NEVENTS}_R${REGION}_P${pileup}.log"


cd -
echo "Done $OutputDir$OutputFile"
