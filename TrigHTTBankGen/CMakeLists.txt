################################################################################
# Package: TrigHTTBankGen
################################################################################

# Declare the package name:
atlas_subdir( TrigHTTBankGen )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
    PUBLIC
        AtlasPolicy
        Control/AthenaBaseComps
        Control/AthenaKernel
        GaudiKernel
        InnerDetector/InDetConditions/InDetBeamSpotService
	Trigger/TrigHTT/TrigHTTAlgorithms
        Trigger/TrigHTT/TrigHTTBanks
        Trigger/TrigHTT/TrigHTTByteStream
        Trigger/TrigHTT/TrigHTTConfig
        Trigger/TrigHTT/TrigHTTInput
        Trigger/TrigHTT/TrigHTTMaps
        Trigger/TrigHTT/TrigHTTObjects
        Trigger/TrigHTT/TrigHTTUtils
)

# External dependencies:
find_package( Boost COMPONENTS iostreams program_options filesystem thread system )
find_package( Eigen )
find_package( HepPDT )
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( TBB )

# Create a library for non-Gaudi components
atlas_add_library(
    TrigHTTBankGenLib       src/*.cxx TrigHTTBankGen/*.h
    PUBLIC_HEADERS          TrigHTTBankGen
    INCLUDE_DIRS            ${HEPPDT_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
    LINK_LIBRARIES          ${HEPPDT_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} ${TBB_LIBRARIES} ${Boost_LIBRARIES}
    AthenaBaseComps         GaudiKernel TrigHTTUtilsLib TrigHTTObjectsLib TrigHTTConfigLib TrigHTTMapsLib TrigHTTInputLib TrigHTTBanksLib TrigHTTByteStreamLib TrigHTTAlgorithmsLib
)

# Declare Gaudi component(s) in the package
atlas_add_component(
    TrigHTTBankGen          src/components/*.cxx
    INCLUDE_DIRS            ${HEPPDT_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
    LINK_LIBRARIES          TrigHTTBankGenLib
)

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*job[Oo]ptions*.py share/MC12.184002.ParticleGenerator_mu_Pt1to1000_Eta30_x10_HTT.py share/MC12.184002.ParticleGenerator_pi_Pt1to1000_Eta30_x10_HTT.py )
atlas_install_joboptions( share/skeleton.*.py )
atlas_install_runtime( scripts/*_tf.py )

# Unit tests

file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_testMatrixMerge )
atlas_add_test(             testMatrixMerge
    SCRIPT                  test/test_TrigHTTBankGenMatrixMerge.sh
    PROPERTIES
        TIMEOUT             500
        WORKING_DIRECTORY   ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_testMatrixMerge
)

file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_testConstGen )
atlas_add_test(             testConstGen
    SCRIPT                  test/test_TrigHTTConstGen.sh
    PROPERTIES
        TIMEOUT             1500
        WORKING_DIRECTORY   ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_testConstGen
)

file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_testPattGen )
atlas_add_test(             testPattGen
    SCRIPT                  test/test_TrigHTTPattGen.sh
    PROPERTIES
        TIMEOUT             500
        WORKING_DIRECTORY   ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_testPattGen
)

file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_testPattGenDC )
atlas_add_test(             testPattGenDC
    SCRIPT                  test/test_TrigHTTPattGenDC.sh
    PROPERTIES
        TIMEOUT             500
        WORKING_DIRECTORY   ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_testPattGenDC
)

atlas_add_test(             testConstGenAlgebra
    SOURCES                 test/test_TrigHTTConstGenAlgebra.cxx
    INCLUDE_DIRS            ${HEPPDT_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
    LINK_LIBRARIES          TrigHTTBankGenLib
    PROPERTIES
        TIMEOUT             500
)
