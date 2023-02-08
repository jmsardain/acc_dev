#ifndef HTTROADFILTERI_H
#define HTTROADFILTERI_H

/**
 * @file HTTRoadFilterToolI.h
 * @author Elliot Lipeles  lipeles@cern.ch
 * @date 03/25/21
 * @brief Interface declaration for road filter tools
 *
 * This class is implemented by
 *      - HTTEtaPatternFilterTool
 */

#include "GaudiKernel/IAlgTool.h"

#include "TrigHTTUtils/HTTTypes.h"
#include "TrigHTTObjects/HTTRoad.h"

#include <vector>


static const InterfaceID IID_HTTRoadFilterToolI("HTTRoadFilterToolI", 1, 0);


/**
 * A road filter returns a vector of roads given a vector of roads.
 *
 * Note that the postfilter_roads are owned by the tool, and are cleared at each successive
 * call of filterRoads().
 */
class HTTRoadFilterToolI : virtual public IAlgTool
{
    public:
        static const InterfaceID& interfaceID();
        virtual StatusCode filterRoads(const std::vector<HTTRoad*> & prefilter_roads, std::vector<HTTRoad*> & postfilter_roads) = 0;
};


inline const InterfaceID & HTTRoadFilterToolI::interfaceID()
{
    return IID_HTTRoadFilterToolI;
}


#endif
