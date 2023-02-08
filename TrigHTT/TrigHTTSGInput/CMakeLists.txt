################################################################################
# Package: TrigHTTSGInput
################################################################################

# Declare the package name:
atlas_subdir( TrigHTTSGInput )

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
    Trigger/TrigHTT/TrigHTTObjects
)

# External dependencies:
find_package( Boost COMPONENTS iostreams filesystem thread system )
find_package( HepMC )
find_package( HepPDT )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library(
  TrigHTTSGInputLib         src/*.cxx TrigHTTSGInput/*.h ${TrigHTTSGInputLibDictSource}
  PUBLIC_HEADERS            TrigHTTSGInput
  INCLUDE_DIRS              ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
  PRIVATE_INCLUDE_DIRS      ${HEPPDT_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
  LINK_LIBRARIES            ${ROOT_LIBRARIES} ${Boost_LIBRARIES}
                            AthenaBaseComps GeneratorObjects InDetIdentifier InDetPrepRawData SCT_CablingLib SGtests StoreGateLib TrigCaloEvent  TrigDecisionToolLib TrigHTTObjectsLib TrkExInterfaces TrkToolInterfaces TrkTrack TrkTruthData
  PRIVATE_LINK_LIBRARIES    ${HEPPDT_LIBRARIES} ${HEPMC_LIBRARIES}
                            AtlasDetDescr EventInfo EventPrimitives GaudiKernel IdDict IdDictDetDescr Identifier InDetRawData InDetReadoutGeometry InDetRIO_OnTrack InDetSimData PixelCablingLib ReadoutGeometryBase TileIdentifier TrkFitterInterfaces TrkRIO_OnTrack TrkSpacePoint VxVertex
)

atlas_add_component(
  TrigHTTSGInput            src/components/*.cxx
  INCLUDE_DIRS              ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
  LINK_LIBRARIES            TrigHTTSGInputLib
)

# Install files from the package:
atlas_install_joboptions( share/*jobOptions*.py )

# unit tests
