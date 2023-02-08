
################################################################################
# Package: TrigHTTByteStream
################################################################################

# Declare the package name:
atlas_subdir( TrigHTTByteStream )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
  PUBLIC
    Control/AthenaBaseComps
    Control/AthenaKernel
    Control/StoreGate
    Event/ByteStreamCnvSvc
    Event/ByteStreamCnvSvcBase
    Event/ByteStreamData
    GaudiKernel
    Trigger/TrigHTT/TrigHTTInput
    Trigger/TrigHTT/TrigHTTMaps
    Trigger/TrigHTT/TrigHTTObjects
    Trigger/TrigHTT/xAODTrigHTT
)

#     Trigger/TrigConfiguration/TrigConfInterfaces
#     Trigger/TrigConfiguration/TrigConfL1Data
#     Trigger/TrigT1/TrigT1Interfaces
#     Trigger/TrigT1/TrigT1Result
# )

# External dependencies:
# find_package( tdaq-common COMPONENTS CTPfragment eformat )

# tag $(ttr_allow_undefined) was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_library(
  TrigHTTByteStreamLib      src/*.cxx
  PUBLIC_HEADERS            TrigHTTByteStream
  INCLUDE_DIRS              ${TDAQ-COMMON_INCLUDE_DIRS}
  LINK_LIBRARIES            ${TDAQ-COMMON_LIBRARIES}
                            AthenaBaseComps AthenaKernel ByteStreamCnvSvcBaseLib ByteStreamCnvSvcLib ByteStreamData ByteStreamData_test GaudiKernel SGtests StoreGateLib TrigHTTMapsLib TrigHTTObjectsLib
  PRIVATE_LINK_LIBRARIES    ${ROOT_LIBRARIES} xAODTrigHTT
)

atlas_add_component(
  TrigHTTByteStream         src/components/*.cxx
  INCLUDE_DIRS              ${TDAQ-COMMON_INCLUDE_DIRS}        
  LINK_LIBRARIES            TrigHTTByteStreamLib
)

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_data( share/*.ref )

# Unit tests
# reference file in share: the tst compares the log file with the reference

file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_testHTTByteStreamConversionDummy )
atlas_add_test(             testHTTByteStreamConversionDummy
  ENVIRONMENT               HTTBSDUMMYINPUT=1
  SCRIPT                    test/test_HTTByteStreamConversion.sh
  EXTRA_PATTERNS            "^Input file | INFO Reading | Opened file:"
  PROPERTIES
    WORKING_DIRECTORY       ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_testHTTByteStreamConversionDummy
)

file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_testHTTByteStreamConversionEvent )
atlas_add_test(             testHTTByteStreamConversionEvent
  ENVIRONMENT               HTTBSDUMMYINPUT=0
  SCRIPT                    test/test_HTTByteStreamConversion.sh
  EXTRA_PATTERNS            "^Input file | INFO Reading | Opened file:"
  PROPERTIES
    WORKING_DIRECTORY       ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_testHTTByteStreamConversionEvent
)
