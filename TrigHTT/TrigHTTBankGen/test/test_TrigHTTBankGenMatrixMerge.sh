BanksDir='/eos/atlas/atlascerngroupdisk/det-htt/HTTsim/ATLAS-P2-ITK-17-06-00/21.9.2/eta0103phi0305/SectorBanks/19-12-3_jahreda21.9'

consts1=$BanksDir/corrgen_raw_8L_reg0_checkGood1.gcon
consts2=$BanksDir/corrgen_raw_13L_reg0_checkGood1.gcon

if [ ! -z $EOS_MGM_URL_HTT ]; then
   echo "Copying from eos $EOS_MGM_URLHTT to local"
   file=`basename $consts1`
   xrdcp -f $EOS_MGM_URL_HTT/$consts1 $file

   file=`basename $consts2`
   xrdcp -f $EOS_MGM_URL_HTT/$consts2 $file
fi



HTTMatrixMerge_tf.py \
    --NBanks 96 \
    --genconst False \
    --inputHTTMatrixFile "$EOS_MGM_URL_HTT/eos/atlas/atlascerngroupdisk/det-htt/HTTsim/ATLAS-P2-ITK-17-06-00/21.9.2/eta0103phi0305/Matrix/user.jahreda.16050716.EXT0._002780.matrix_13L_18Dim.root" "$EOS_MGM_URL_HTT/eos/atlas/atlascerngroupdisk/det-htt/HTTsim/ATLAS-P2-ITK-17-06-00/21.9.2/eta0103phi0305/Matrix/user.jahreda.16050716.EXT0._002790.matrix_13L_18Dim.root" \
    --bankregion 0 \
    --outputMergedHTTMatrixFile combined_matrix_13L_18Dim.root \
    --bankTag "jahreda_20-07-20" \


retVal=$?
if [ $retVal -ne 0 ]; then
    echo "\n\nDumping Log\n\n"
    tail -n +1 log.*
fi
exit $retVal

#    --HTTMatrixFileRegEx '/eos/atlas/atlascerngroupdisk/det-htt/HTTsim/*/*/*/Matrix/*roo*' \
