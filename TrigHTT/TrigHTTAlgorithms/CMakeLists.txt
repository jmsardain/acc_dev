################################################################################
# Package: TrigHTTAlgorithms
################################################################################

# Declare the package name:
atlas_subdir( TrigHTTAlgorithms )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
    PUBLIC
        AtlasPolicy
        Control/AthenaBaseComps
        Control/AthenaKernel
        GaudiKernel
        Trigger/TrigHTT/TrigHTTBanks
        Trigger/TrigHTT/TrigHTTByteStream
        Trigger/TrigHTT/TrigHTTConfig
        Trigger/TrigHTT/TrigHTTInput
        Trigger/TrigHTT/TrigHTTMaps
        Trigger/TrigHTT/TrigHTTMonitor
        Trigger/TrigHTT/TrigHTTObjects
        Trigger/TrigHTT/TrigHTTUtils
        Trigger/TrigHTT/xAODTrigHTT
)

# ECC try to add in onnx prefix path
###list(APPEND CMAKE_PREFIX_PATH "/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBaseExternals/21.2.139/InstallArea/x86_64-centos7-gcc8-opt/")

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
# find_package( HepPDT )
# find_package( CLHEP )
# find_package( TBB )
find_package( lwtnn )

# Component(s) in the package:
atlas_add_library(
    TrigHTTAlgorithmsLib        src/*.cxx TrigHTTAlgorithms/*.h
    PUBLIC_HEADERS              TrigHTTAlgorithms
    INCLUDE_DIRS                ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${LWTNN_INCLUDE_DIRS}
    LINK_LIBRARIES              ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${EIGEN_LIBRARIES} ${LWTNN_LIBRARIES}
                                AthenaBaseComps GaudiKernel TrigHTTBanksLib TrigHTTConfigLib TrigHTTInputLib TrigHTTMapsLib TrigHTTMonitorLib TrigHTTObjectsLib TrigHTTUtilsLib
    PRIVATE_LINK_LIBRARIES      ${ROOT_LIBRARIES} xAODTrigHTT
)

atlas_add_component(
    TrigHTTAlgorithms           src/components/*.cxx
    INCLUDE_DIRS                ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
    LINK_LIBRARIES              TrigHTTAlgorithmsLib
)

# Install files from the package:
atlas_install_headers( TrigHTTAlgorithms )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*jobOptions*.py )
atlas_install_joboptions( share/skeleton.*.py )
atlas_install_runtime( scripts/*_tf.py ) # TODO should this be _scripts?
atlas_install_scripts( scripts/Hough_plots.py )
atlas_install_data( share/*.ref )

# Install files from the package:
atlas_install_scripts( test/*.sh )
atlas_install_scripts( scripts/*.sh )

# Unit tests

#_add_test( emu_scaling_CF EXTRA_PATTERNS "-s TrigSignatureMoniMT.*INFO HLT_.*|TriggerSummaryStep.* chains passed:|TriggerSummaryStep.*+++ HLT_.*" )

foreach(region 0 1)
    set( rundir ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_testLogicalHitsToAlg_${region} )
    file( REMOVE_RECURSE ${rundir} )
    file( MAKE_DIRECTORY ${rundir} )
    atlas_add_test(                 testLogicalHitsToAlg_${region}
        ENVIRONMENT                 REGION=${region}
        SCRIPT                      test/test_TrigHTTLogicalHitsToAlg.sh
        PROPERTIES
            TIMEOUT                 1500
            WORKING_DIRECTORY       ${rundir}
        EXTRA_PATTERNS              "Input file|INFO Reading|TempAthenaConfig|input file|Preloading|TGraphAsymmErrors|Opened file:"
    )
endforeach()

file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_testLogicalHitsToAlg_SAP )
atlas_add_test(                 testLogicalHitsToAlg_SAP
    SCRIPT                      test/test_TrigHTTLogicalHitsToAlg_SAP.sh
    PROPERTIES
        TIMEOUT                 1500
        WORKING_DIRECTORY       ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_testLogicalHitsToAlg_SAP
    EXTRA_PATTERNS              "Input file|INFO Reading|TempAthenaConfig|input file|Preloading|TGraphAsymmErrors|Opened file:"
)

file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_testLogicalHitsToAlg_Hough )
atlas_add_test(                 testLogicalHitsToAlg_Hough
    SCRIPT                      test/test_TrigHTTLogicalHitsToAlg_Hough.sh
    PROPERTIES
        TIMEOUT                 500
        WORKING_DIRECTORY       ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_testLogicalHitsToAlg_Hough
    EXTRA_PATTERNS              "Input file|INFO Reading|TempAthenaConfig|input file|Preloading|TGraphAsymmErrors|Opened file:"
)

file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_testOutputMonitorAlg )
atlas_add_test(                 testOutputMonitorAlg
    SCRIPT                      test/test_TrigHTTOutputMonitorAlg.sh
    PROPERTIES
        TIMEOUT                 1500
        WORKING_DIRECTORY       ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_testOutputMonitorAlg
    EXTRA_PATTERNS              "Input file|INFO Reading|TempAthenaConfig|input file|Preloading|TGraphAsymmErrors|Opened file:"
)

atlas_add_test(                 HTTPatternMatchTool
    SOURCES                     test/HTTPatternMatchTool_test.cxx
    INCLUDE_DIRS                ${ROOT_INCLUDE_DIRS}
    LINK_LIBRARIES              TrigHTTAlgorithmsLib TestTools
    ENVIRONMENT                 "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share"
    PROPERTIES
        TIMEOUT                 500
)
