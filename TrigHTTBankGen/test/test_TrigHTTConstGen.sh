PATHTOMATRIX=$EOS_MGM_URL_HTT/eos/atlas/atlascerngroupdisk/det-htt/HTTsim/ATLAS-P2-ITK-23-00-01/21.9.15/eta0103phi0305/Matrix/user.jahreda.24334810.EXT0._000342.matrix.root

HTTConstReduceConstGen_tf.py \
    --NBanks 96 \
    --bankregion 0 \
    --allregions False \
    --inputHTTMatrixFile $PATHTOMATRIX \
    --outputHTTGoodMatrixReducedCheckFile good_matrix_reduced_checkgood.root \
    --outputHTTGoodMatrixReducedIgnoreFile good_matrix_reduced_ignoregood.root


#    --pmap_12L raw_FTKppExample.pmap \
#    --pmap_8L L1TrackExample.pmap \

retVal=$?
if [ $retVal -ne 0 ]; then
    echo "\n\nDumping Log\n\n"
    tail -n +1 log.*
fi
exit $retVal
