################################################################################
# Package: TrigHTTMonitor
################################################################################

# Declare the package name:
atlas_subdir( TrigHTTMonitor )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
    PUBLIC
        AtlasPolicy
        Control/AthenaBaseComps
        Control/AthenaKernel
        GaudiKernel
        Trigger/TrigHTT/TrigHTTBanks
        Trigger/TrigHTT/TrigHTTConfig
        Trigger/TrigHTT/TrigHTTMaps
        Trigger/TrigHTT/TrigHTTObjects
        Trigger/TrigHTT/TrigHTTUtils
)

# Component(s) in the package:
atlas_add_library(
    TrigHTTMonitorLib           src/*.cxx TrigHTTMonitor/*.h
    PUBLIC_HEADERS              TrigHTTMonitor
    INCLUDE_DIRS                ${ROOT_INCLUDE_DIRS}
    LINK_LIBRARIES              ${ROOT_LIBRARIES}
                                AthenaBaseComps GaudiKernel TrigHTTBanksLib TrigHTTConfigLib TrigHTTMapsLib TrigHTTObjectsLib TrigHTTUtilsLib
)

atlas_add_component(
    TrigHTTMonitor              src/components/*.cxx
    INCLUDE_DIRS                ${ROOT_INCLUDE_DIRS}
    LINK_LIBRARIES              TrigHTTMonitorLib
)
