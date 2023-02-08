################################################################################
# Package: TrigHTTMaps
################################################################################

# Declare the package name:
atlas_subdir( TrigHTTMaps )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
    PUBLIC
        Control/AthContainers
        Control/AthenaBaseComps
        Control/AthenaKernel
        Control/StoreGate
        GaudiKernel
        Trigger/TrigHTT/TrigHTTConfig
        Trigger/TrigHTT/TrigHTTObjects
        Tools/PathResolver
)

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( Boost )
find_package( lwtnn )

# Component(s) in the package:
atlas_add_library(
    TrigHTTMapsLib      src/*.cxx TrigHTTMaps/*.h
    PUBLIC_HEADERS      TrigHTTMaps
    INCLUDE_DIRS        ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${LWTNN_INCLUDE_DIRS}
    LINK_LIBRARIES      ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${LWTNN_LIBRARIES}
                        AthContainers AthenaBaseComps AthenaKernel TrigHTTConfigLib TrigHTTObjectsLib PathResolver
)

atlas_add_component(
    TrigHTTMaps         src/components/*.cxx
    INCLUDE_DIRS        ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
    LINK_LIBRARIES      TrigHTTMapsLib
)

# Install files from the package:
atlas_install_scripts( scripts/keylayer_zslicemap.py )
atlas_install_joboptions( share/*.py )
atlas_install_data( config/*.txt )
atlas_install_python_modules( python/*.py )

atlas_install_generic(  config/map_file/*
    DESTINATION         share/htt_configuration/map_files
)
atlas_install_generic(  config/ss_file/*
    DESTINATION share/htt_configuration/ss_files
)
atlas_install_generic(  config/hwsdev_file/*
    DESTINATION share/htt_configuration/hwsdev_file
)
atlas_install_generic(  config/bad_modules/*
    DESTINATION share/htt_configuration/bad_modules
)
atlas_install_generic(  config/zslicemaps/*
    DESTINATION share/htt_configuration/zslicemaps
)
atlas_install_generic(  config/eta_patterns/*
    DESTINATION share/htt_configuration/eta_patterns
)

# Install map files to testing directory
atlas_install_generic(  config/map_file/*
    DESTINATION         this_does_nothing
    BUILD_DESTINATION   ${CMAKE_CURRENT_BINARY_DIR}/htt_configuration/map_files
)
atlas_install_generic(  config/ss_file/*
    DESTINATION         this_does_nothing
    BUILD_DESTINATION   ${CMAKE_CURRENT_BINARY_DIR}/htt_configuration/ss_files
)
atlas_install_generic(  config/hwsdev_file/*
    DESTINATION         this_does_nothing
    BUILD_DESTINATION   ${CMAKE_CURRENT_BINARY_DIR}/htt_configuration/hwsdev_file
)
atlas_install_generic(  config/bad_modules/*
    DESTINATION         this_does_nothing
    BUILD_DESTINATION   ${CMAKE_CURRENT_BINARY_DIR}/htt_configuration/bad_modules
)
atlas_install_generic(  config/zslicemaps/*
    DESTINATION         this_does_nothing
    BUILD_DESTINATION   ${CMAKE_CURRENT_BINARY_DIR}/htt_configuration/zslicemaps
)

# Add tests

atlas_add_test(         HTTPlaneMap
    SOURCES             test/HTTPlaneMap_test.cxx
    LINK_LIBRARIES      TrigHTTMapsLib TestTools
    PROPERTIES
        TIMEOUT         120
)

atlas_add_test(         HTTRegionMap
    SOURCES             test/HTTRegionMap_test.cxx
    LINK_LIBRARIES      TrigHTTMapsLib TestTools
    PROPERTIES
        TIMEOUT         120
)

atlas_add_test(         HTTSSMap
    SOURCES             test/HTTSSMap_test.cxx
    LINK_LIBRARIES      TrigHTTMapsLib TestTools
    PROPERTIES
        TIMEOUT         120
)
