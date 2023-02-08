#!/bin/bash

#RDOs:
#export HTTRDOFile22=$EOS_MGM_URL_HTT/eos/atlas/atlascerngroupdisk/det-htt/HTTsim/ATLAS-P2-ITK-22-02-00/21.9.16/eta0103phi0305/RDOs/mc15_14TeV.900171.PG_singlemu_InvPtFlat1_etaFlat01_03_phiFlat03_05.recon.RDO.e8312_s3595_s3600_r12410/RDO.24440670._0000*

export HTTRDOFile22=$EOS_MGM_URL_HTT/eos/atlas/atlascerngroupdisk/det-htt/HTTsim/ATLAS-P2-ITK-22-02-00/21.9.16/eta0103phi0305/RDOs/mc15_14TeV.900171.PG_singlemu_InvPtFlat1_etaFlat01_03_phiFlat03_05.recon.RDO.e8312_s3595_s3600_r12410/RDO.24440670._000016.pool.root.1
#mc/user.martyniu.june24.hit.v1.pt0.8-400_07eta09_03phi05.1muonper_EXT0/user.martyniu.21748447.EXT0._001677.RDO.pool.root

export HTTRDOFile23=$EOS_MGM_URL_HTT/eos/atlas/atlascerngroupdisk/det-htt/HTTsim/ATLAS-P2-ITK-23-00-01/21.9.15/eta0103phi0305/RDOs/mc15_14TeV.900171.PG_singlemu_InvPtFlat1_etaFlat01_03_phiFlat03_05.recon.RDO.e8312_s3595_s3600_r12392/RDO.24279796._000051.pool.root.1
#export HTTRDOFile23=$EOS_MGM_URL_HTT/eos/atlas/atlascerngroupdisk/det-htt/HTTsim/ATLAS-P2-ITK-23-00-01/21.9.15/eta0103phi0305/RDOs/single_muons.RDO.root

#wrappers:
##########
#export HTTRawHitFile=$EOS_MGM_URL_HTT/eos/atlas/atlascerngroupdisk/det-htt/HTTSim/wrapper_files/user.jahreda.nov2.pt800_01eta03_03phi05.1muonper.21.9.13.prod.dec17.v1_EXT0/user.jahreda.23602267.EXT0._000887.httsim_rawhits_wrap.root
export HTTRawHitFile=$EOS_MGM_URL_HTT/eos/atlas/atlascerngroupdisk/det-htt/HTTsim/ATLAS-P2-ITK-23-00-01/21.9.15/eta0103phi0305/Wrappers/single_muons_wrapper_wTruthUpdates.root
# input wrapper files for each region
export input_map=( \
            [0]="/eos/atlas/atlascerngroupdisk/det-htt/HTTsim/ATLAS-P2-ITK-22-02-00/21.9.16/eta0103phi0305/Wrappers/singlemu_invPtFlat1_1M_wrap.root" \
            [1]="/eos/atlas/atlascerngroupdisk/det-htt/HTTsim/ATLAS-P2-ITK-22-02-00/21.9.16/eta0709phi0305/Wrappers/singlemu_invPtFlat1_10k_wrap.root" \
)

export HTTLogHitFile=$EOS_MGM_URL_HTT/eos/atlas/atlascerngroupdisk/det-htt/HTTsim/ATLAS-P2-ITK-22-02-00/21.9.16/eta0103phi0305/Outputs/singlemu_invPtFlat1_10k_loghits_N1000_P0_9-11-21.root

#reference files
export REF_HTTRawHitFile=$EOS_MGM_URL_HTT/eos/atlas/atlascerngroupdisk/det-htt/HTTSim/wrapper_files/ATLAS-P2-ITK-22-00-00_RDO-21748447_bf1a13f_wrapper.root

# PU200
#ls -p -m /eos/atlas/atlascerngroupdisk/det-htt/HTTsim/ATLAS-P2-ITK-23-00-01/21.9.15/etaALLphiALL/Wrappers/user.martyniu.feb8.1muonpt10.mu200.21.9.13.prod.dev.v5_EXT0/
#RDOs
export PU200RDO_23=$EOS_MGM_URL_HTT/eos/atlas/atlascerngroupdisk/det-htt/HTTsim/ATLAS-P2-ITK-23-00-01/21.9.15/etaALLphiALL/RDO/mc15_14TeV.900036.PG_singlemu_Pt10_etaFlatnp0_43.recon.RDO.e8185_s3630_s3633_r12351_tid24030291_00/RDO.24030291._000127.pool.root.1
export PU200RDO_22=$EOS_MGM_URL_HTT/eos/atlas/atlascerngroupdisk/det-htt/HTTsim/ATLAS-P2-ITK-22-02-00/21.9.16/etaALLphiALL/RDOs/mc15_14TeV.900036.PG_singlemu_Pt10_etaFlatnp0_43.recon.RDO.e8185_s3595_s3600_r12063/RDO.22568851._000140.pool.root.1

#Wrappers
#export PU200DIR_23=$EOS_MGM_URL_HTT/eos/atlas/atlascerngroupdisk/det-htt/HTTsim/ATLAS-P2-ITK-23-00-01/21.9.15/etaALLphiALL/Wrappers/mc15_14TeV.900036.PG_singlemu_Pt10_etaFlatnp0_43.recon.RDO.e8185_s3630_s3633_r12351_tid24030291_00/
export PU200DIR_23=$EOS_MGM_URL_HTT/eos/atlas/atlascerngroupdisk/det-htt/HTTsim/ATLAS-P2-ITK-23-00-01/21.9.15/etaALLphiALL/Wrappers/V2.mc15_14TeV.900036.PG_singlemu_Pt10_etaFlatnp0_43.recon.RDO.e8185_s3630_s3633_r12351_tid24030291_00/
#export PU200DIR_23=$EOS_MGM_URL_HTT/eos/atlas/atlascerngroupdisk/det-htt/HTTsim/ATLAS-P2-ITK-23-00-01/21.9.15/etaALLphiALL/Wrappers/user.martyniu.feb8.1muonpt10.mu200.21.9.13.prod.dev.v5_EXT0/

export PU200FILES_23=`ls -p -m $PU200DIR_23`
export PU200Wrapper_23=`echo $PU200FILES_23 |sed -e "s|httsim|"$PU200DIR_23"\httsim|g"|tr -d '\n'| tr -d ' '`
#export PU200Wrapper_23=`echo $PU200FILES_23 |sed -e "s|^|"$PU200DIR_23"\/|"|tr -d '\n'| tr -d ' '`

#export PU200Wrapper_23="/eos/atlas/atlascerngroupdisk/det-htt/HTTsim/ATLAS-P2-ITK-23-00-01/21.9.15/etaALLphiALL/Wrappers/mc15_14TeV.900036.PG_singlemu_Pt10_etaFlatnp0_43.recon.RDO.e8185_s3630_s3633_r12351_tid24030291_00/httsim_rawhits_wrap_10events.root"
export PU200Wrapper_22=$EOS_MGM_URL_HTT/eos/atlas/atlascerngroupdisk/det-htt/HTTsim/ATLAS-P2-ITK-22-02-00/21.9.16/etaALLphiALL/Wrappers/user.martyniu.mar16.1muonpt10.mu200.21.9.16_v3_EXT0/user.martyniu.24590899.EXT0._000003.httsim_rawhits_wrap.root

