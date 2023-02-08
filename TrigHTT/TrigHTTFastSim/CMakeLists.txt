################################################################################
# Package: TrigHTTFastSim
################################################################################

# Declare the package name:
atlas_subdir( TrigHTTFastSim )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
  PUBLIC
    Control/AthenaBaseComps
  PRIVATE
    GaudiKernel
)

# External dependencies:
find_package( ROOT )
find_package( Boost COMPONENTS iostreams program_options filesystem thread system )

# Component(s) in the package:
atlas_add_root_dictionary(
  TrigHTTFastSimLib         TrigHTTFastSimLibDictSource
  ROOT_HEADERS              TrigHTTFastSim/*.h
  EXTERNAL_PACKAGES         Boost ROOT
)

# Component(s) in the package:
atlas_add_library(
  TrigHTTFastSimLib         src/*.cxx TrigHTTFastSim/*.h ${TrigHTTFastSimLibDictSource}
  PUBLIC_HEADERS            TrigHTTFastSim
  INCLUDE_DIRS              ${Boost_INCLUDE_DIR} ${ROOT_INCLUDE_DIRS}
  LINK_LIBRARIES            ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AsgTools AthenaBaseComps xAODEventInfo xAODTracking
  PRIVATE_LINK_LIBRARIES    ${HEPPDT_LIBRARIES}  GaudiKernel
)

atlas_add_component(
  TrigHTTFastSim            src/components/*.cxx
  INCLUDE_DIRS              ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
  LINK_LIBRARIES            TrigHTTFastSimLib
)

file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_testXAOD )
atlas_add_test(             testXAOD
  SCRIPT                    testOutput/testXAOD.sh
  PROPERTIES
    TIMEOUT                 500
    WORKING_DIRECTORY       ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_testXAOD
)

atlas_install_joboptions( share/*_jobOptions.py )
atlas_install_scripts( share/*_jobOptions.py )

