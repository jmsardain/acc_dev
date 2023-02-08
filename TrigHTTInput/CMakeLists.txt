################################################################################
# Package: TrigHTTInput
################################################################################

# Declare the package name:
atlas_subdir( TrigHTTInput )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
  PUBLIC
    Control/AthenaBaseComps
    Generators/GeneratorObjects
    InnerDetector/InDetConditions/InDetConditionsSummaryService
    InnerDetector/InDetDetDescr/InDetIdentifier
    InnerDetector/InDetRecEvent/InDetPrepRawData
    InnerDetector/InDetRecEvent/InDetRIO_OnTrack
    Tracking/TrkEvent/TrkRIO_OnTrack
    Tracking/TrkEvent/TrkSpacePoint
    Tracking/TrkEvent/TrkTrack
    Tracking/TrkEvent/TrkTruthData
    Tracking/TrkEvent/VxVertex
    Tracking/TrkExtrapolation/TrkExInterfaces
    Tracking/TrkFitter/TrkFitterInterfaces
    Tracking/TrkTools/TrkToolInterfaces
    Tracking/TrkTools/TrkTrackSummaryTool
  PRIVATE
    Control/StoreGate
    DetectorDescription/AtlasDetDescr
    DetectorDescription/IdDict
    DetectorDescription/IdDictDetDescr
    DetectorDescription/Identifier
    DetectorDescription/ReadoutGeometryBase
    Event/EventInfo
    Event/EventPrimitives
    GaudiKernel
    InnerDetector/InDetConditions/InDetBeamSpotService
    InnerDetector/InDetDetDescr/InDetReadoutGeometry
    InnerDetector/InDetDetDescr/PixelCabling
    InnerDetector/InDetDetDescr/SCT_Cabling
    InnerDetector/InDetRawEvent/InDetRawData
    InnerDetector/InDetRawEvent/InDetSimData
    Trigger/TrigHTT/TrigHTTBanks
    Trigger/TrigHTT/TrigHTTConfig
    Trigger/TrigHTT/TrigHTTMaps
    Trigger/TrigHTT/TrigHTTObjects
    Trigger/TrigHTT/xAODTrigHTT
)

# External dependencies:
find_package( Boost COMPONENTS iostreams filesystem thread system )
find_package( HepMC )
find_package( HepPDT )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_root_dictionary(
  TrigHTTInputLib           TrigHTTInputLibDictSource
  ROOT_HEADERS              TrigHTTInput/L1SimFitResult.h TrigHTTInput/L1SimEvent.h TrigHTTInput/TrigHTTInputLinkDef.h
  EXTERNAL_PACKAGES         ROOT Boost
)

# Component(s) in the package:
atlas_add_library(
  TrigHTTInputLib           src/*.cxx TrigHTTInput/*.h ${TrigHTTInputLibDictSource}
  PUBLIC_HEADERS            TrigHTTInput
  INCLUDE_DIRS              ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
  PRIVATE_INCLUDE_DIRS      ${HEPPDT_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
  LINK_LIBRARIES            ${ROOT_LIBRARIES} ${Boost_LIBRARIES}
                            AthenaBaseComps GeneratorObjects InDetIdentifier InDetPrepRawData SCT_CablingLib SGtests StoreGateLib
                            TrigCaloEvent TrigDecisionToolLib TrigHTTBanksLib TrigHTTConfigLib TrigHTTMapsLib TrigHTTObjectsLib
                            TrkExInterfaces TrkToolInterfaces TrkTrack TrkTruthData
  PRIVATE_LINK_LIBRARIES    ${HEPPDT_LIBRARIES} ${HEPMC_LIBRARIES}
                            AtlasDetDescr EventInfo EventPrimitives GaudiKernel IdDict IdDictDetDescr Identifier InDetRawData
                            InDetReadoutGeometry InDetRIO_OnTrack InDetSimData PixelCablingLib ReadoutGeometryBase TileIdentifier
                            TrkFitterInterfaces TrkRIO_OnTrack TrkSpacePoint VxVertex
)

atlas_add_component(
  TrigHTTInput              src/components/*.cxx
  INCLUDE_DIRS              ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
  LINK_LIBRARIES            TrigHTTInputLib
)

# Install files from the package:
atlas_install_joboptions( share/*jobOptions*.py )
atlas_install_runtime( share/HTTrootcomp.py )
atlas_install_scripts( scripts/HTTPlotHits.py )

# unit tests

# inherited from FTK, obsolete now
# file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_testInputWrapper )
# atlas_add_test(             testInputWrapper
#   PRE_EXEC_SCRIPT           .\ ${CMAKE_CURRENT_SOURCE_DIR}/test/ITkSetup.sh
#   SCRIPT                    test/test_TrigHTTInputWrapper.sh
#   PROPERTIES
#     TIMEOUT                 1000
#     WORKING_DIRECTORY       ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_testInputWrapper
# )

file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_testSGToRawHits )
atlas_add_test(             testSGToRawHits
  PRE_EXEC_SCRIPT           .\ ${CMAKE_CURRENT_SOURCE_DIR}/test/ITkSetup.sh
  SCRIPT                    test/test_TrigHTTSGToRawHitsWrapperAlg.sh
  PROPERTIES
    TIMEOUT                 1500
    WORKING_DIRECTORY       ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_testSGToRawHits
  EXTRA_PATTERNS            "INFO Input file|INFO Reading|TempAthenaConfig|input file|Preloading|PyJobTransforms|PerfMonSvc|PMonSD|ConfiguredFactory|GeoModelSvc|CORAL|cvmfs"
)

# file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_testSGToRawNtuple )
# atlas_add_test(             testSGToRawNtuple
#   PRE_EXEC_SCRIPT           .\ ${CMAKE_CURRENT_SOURCE_DIR}/test/ITkSetup.sh
#   SCRIPT                    test/test_TrigHTTSGToRawNtupleWrapperAlg.sh
#   PROPERTIES
#     TIMEOUT                 1000
#     WORKING_DIRECTORY       ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_testSGToRawNtuple
# )

file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_testReadRawHit )
atlas_add_test(             testReadRawHit
  SCRIPT                    test/test_TrigHTTReadRawHitsWrapperAlg.sh
  PROPERTIES
    TIMEOUT                 500
    WORKING_DIRECTORY       ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_testReadRawHit
)

file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_testRawToLogical )
atlas_add_test(             testRawToLogical
  SCRIPT                    test/test_TrigHTTRawToLogicalWrapperAlg.sh
  PROPERTIES
    TIMEOUT                 500
    WORKING_DIRECTORY       ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_testRawToLogical
)

atlas_add_test(             flake8_test_dir
  SCRIPT                    flake8 --select=ATL,F,E7,E9,W6 --enable-extension=ATL900,ATL901 --ignore=F821 ${CMAKE_CURRENT_SOURCE_DIR}/share
)

file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_testDumpOutputStat )
atlas_add_test(             testDumpOutputStat
  SCRIPT                    test/test_TrigHTTDumpOutputStatAlg.sh
  PROPERTIES
    TIMEOUT                 500
    WORKING_DIRECTORY       ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_testDumpOutputStat
  EXTRA_PATTERNS            "INFO Input file|INFO Reading|TempAthenaConfig|input file|Preloading|/eos/atlas/"
)
