/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTTTYPES_H
#define HTTTYPES_H

/*
 * HTTTypes.h: This file declares several typedefs and enums used throughout HTTSim
 *
 * Created: June 12th, 2019
 * Author: Riley Xu
 * Email: rixu@cern.ch
 */


#include <stdint.h>
#include <vector>
#include <ostream>

/*****************************************************************************/
/* Typedefs                                                                  */
/*****************************************************************************/

typedef int32_t pid_t;      // pattern id
typedef int32_t ssid_t;     // super-strip id (TSP or DC)
typedef int32_t module_t;   // module id
typedef int32_t sector_t;   // sector number
    // Use a custom type to enforce consistency and easily switch to int64_t or other if needed.
    // Objects like map<sector_t, module_t> are also self-documenting instead of map<int, int>.
    // Use signed integers to easily allow wildcards or error codes using negative values, without
    // need of casting or confusion.
    //
    // TODO: FTK-inherited code need to be updated to use these types

typedef uint32_t layer_bitmask_t; // bitmask that stores at least as many bits as layers

typedef std::vector<ssid_t> pattern_t; // Stores one ssid per layer



/*****************************************************************************/
/* Enums                                                                     */
/*****************************************************************************/

//To label the origin of HTTHits
enum class SiliconTech { strip, pixel, nTechs, undefined };
//The detector zone that the HTTHit was in
enum class DetectorZone { barrel, posEndcap, negEndcap, nZones, undefined };
// An Enum to hold the stage of the track
enum class TrackStage{FIRST,SECOND};
// Enum to hold the different types of hits in HTTOfflineHit
enum class OfflineHitType { Measurement, InertMaterial, BremPoint, Scatterer, Perigee, Outlier, Hole, Other};
// enum for categorising roads
enum roadtype {noroad=0,  moremiss=1, miss2wc2=2, miss2wc1=3, miss1wc0=4, miss1wc1=5, miss0wc0=6};
// Enum to hold type of semple (for truth cuts)
enum class SampleType { skipTruth, singleElectrons=11, singleMuons=13, singlePions=211, LLPs=1000 };
// Enum to hold the type of correction for hits on track
enum class TrackCorrType { None=0, First=1, Second=2 };


inline std::ostream & operator<<(std::ostream & os, SiliconTech t)
{
    switch (t)
    {
        case SiliconTech::strip: return (os << "strip");
        case SiliconTech::pixel: return (os << "pixel");
        default: return (os << "undefined");
    }
}

inline std::ostream & operator<<(std::ostream & os, DetectorZone t)
{
    switch (t)
    {
        case DetectorZone::barrel: return (os << "barrel");
        case DetectorZone::posEndcap: return (os << "posEndcap");
        case DetectorZone::negEndcap: return (os << "negEndcap");
        default: return (os << "undefined");
    }
}


#endif // HTTTYPES_H
