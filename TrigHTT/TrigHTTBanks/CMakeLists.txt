################################################################################
# Package: TrigHTTBanks
################################################################################

# Declare the package name:
atlas_subdir( TrigHTTBanks )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
    PUBLIC
        AtlasPolicy
        Control/AthenaBaseComps
        Control/AthenaKernel
        GaudiKernel
        Trigger/TrigHTT/TrigHTTMaps
        Trigger/TrigHTT/TrigHTTObjects
        Trigger/TrigHTT/TrigHTTUtils
)

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( Eigen )
find_package( HepPDT )
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( TBB )

# Create a library for non-Gaudi components
atlas_add_library(
    TrigHTTBanksLib         src/*.cxx TrigHTTBanks/*.h
    PUBLIC_HEADERS          TrigHTTBanks
    INCLUDE_DIRS            ${HEPPDT_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
    LINK_LIBRARIES          ${HEPPDT_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} ${TBB_LIBRARIES} ${Boost_LIBRARIES}
                            AthenaBaseComps GaudiKernel TrigHTTMapsLib TrigHTTObjectsLib TrigHTTUtilsLib
)

# Declare Gaudi component(s) in the package
atlas_add_component(
    TrigHTTBanks            src/components/*.cxx
    INCLUDE_DIRS            ${HEPPDT_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
    LINK_LIBRARIES          TrigHTTBanksLib
)

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( scripts/*_tf.py )

# Define units tests
file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_HTTIndexPattMerge )
atlas_add_test(             HTTIndexPattMerge_test
    SCRIPT                  test/HTTIndexPattMerge_test.sh
    PROPERTIES
        TIMEOUT             1000
        WORKING_DIRECTORY   ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_HTTIndexPattMerge
)

atlas_add_test(             HTTCCachePattIO_test
    SOURCES                 test/HTTCCachePattIO_test.cxx
    INCLUDE_DIRS            ${ROOT_INCLUDE_DIRS}
    LINK_LIBRARIES          ${ROOT_LIBRARIES} TestTools TrigHTTBanksLib TrigHTTObjectsLib
    PROPERTIES
        TIMEOUT             1000
)

# This test doesn't work anymore since the index tools depend on the map service, and idk how to do that with the txt config file
# atlas_add_test( HTTIndexPattIO_test
#     SOURCES                 test/HTTIndexPattIO_test.cxx
#     INCLUDE_DIRS            ${ROOT_INCLUDE_DIRS}
#     LINK_LIBRARIES          ${ROOT_LIBRARIES} TestTools TrigHTTBanksLib TrigHTTObjectsLib
#     ENVIRONMENT             "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share"
#     PROPERTIES
#         TIMEOUT             1000
# )

atlas_add_test(             flake8_test_dir
    SCRIPT                  flake8 --select=ATL,F,E7,E9,W6 --enable-extension=ATL900,ATL901 ${CMAKE_CURRENT_SOURCE_DIR}/python
)
