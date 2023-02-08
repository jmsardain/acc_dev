################################################################################
# Package: TrigHTTConfig
################################################################################

# Declare the package name:
atlas_subdir( TrigHTTConfig )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
  PUBLIC
    Control/AthContainers
    Control/AthenaBaseComps
    Control/AthenaKernel
    GaudiKernel
    Trigger/TrigHTT/TrigHTTObjects
    Trigger/TrigHTT/TrigHTTUtils
)

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( Boost )

# Component(s) in the package:
atlas_add_library(
  TrigHTTConfigLib    src/*.cxx TrigHTTConfig/*.h
  PUBLIC_HEADERS      TrigHTTConfig
  INCLUDE_DIRS        ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
  LINK_LIBRARIES      ${ROOT_LIBRARIES} ${Boost_LIBRARIES}
                      AthContainers AthenaBaseComps AthenaKernel TrigHTTObjectsLib TrigHTTUtilsLib
)

atlas_add_component(
  TrigHTTConfig       src/components/*.cxx
  INCLUDE_DIRS        ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
  LINK_LIBRARIES      TrigHTTConfigLib
)

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( test/InputTestSetup.sh )

atlas_install_generic(  config/map_file/*
  DESTINATION           share/htt_configuration/map_files
)

# Install map files to testing directory
atlas_install_generic(  config/map_file/*
  DESTINATION           this_does_nothing
  BUILD_DESTINATION     ${CMAKE_CURRENT_BINARY_DIR}/htt_configuration/map_files
)
