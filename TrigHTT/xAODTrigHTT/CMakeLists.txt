################################################################################
# package: xAODTrigHTT
################################################################################

# Declare the package name:
atlas_subdir( xAODTrigHTT )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
  PUBLIC
    Control/AthContainers
    Event/xAOD/xAODCore
    Event/xAOD/xAODBase
  PRIVATE
    Control/AthLinks
)

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library(
  xAODTrigHTT               Root/*.cxx
  PUBLIC_HEADERS            xAODTrigHTT
  PRIVATE_INCLUDE_DIRS      ${ROOT_INCLUDE_DIRS}
  LINK_LIBRARIES            AthContainers xAODCore xAODBase
  PRIVATE_LINK_LIBRARIES    ${ROOT_LIBRARIES} ${extra_libs} AthLinks
)

atlas_add_dictionary(
  xAODTrigHTTDict           xAODTrigHTT/xAODTrigHTTDict.h xAODTrigHTT/selection.xml
  INCLUDE_DIRS              ${ROOT_INCLUDE_DIRS}
  LINK_LIBRARIES            ${ROOT_LIBRARIES} ${extra_libs} AthContainers AthLinks xAODBase xAODCore xAODTrigHTT
  EXTRA_FILES               Root/dict/*.cxx
)
