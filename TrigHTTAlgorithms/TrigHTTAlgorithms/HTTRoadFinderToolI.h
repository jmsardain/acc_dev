#ifndef HTTROADFINDERI_H
#define HTTROADFINDERI_H

/**
 * @file HTTRoadFinderToolI.h
 * @author Riley Xu - rixu@cern.ch
 * @date 10/23/19
 * @brief Interface declaration for road finder tools
 *
 * This class is implemented by
 *      - HTTRoadUnionTool
 *      - HTTPatternMatchTool
 *      - HTTSectorMatchTool
 *      - HTTHoughTransformTool
 *      - HTTHough1DShiftTool
 */

#include "GaudiKernel/IAlgTool.h"

#include "TrigHTTUtils/HTTTypes.h"
#include "TrigHTTObjects/HTTRoad.h"

#include <vector>

class HTTHit;


static const InterfaceID IID_HTTRoadFinderToolI("HTTRoadFinderToolI", 1, 0);


/**
 * A road finder returns a vector of roads given a vector of hits.
 *
 * Note that the roads are owned by the tool, and are cleared at each successive
 * call of getRoads().
 */
class HTTRoadFinderToolI : virtual public IAlgTool
{
    public:
        static const InterfaceID& interfaceID();
        virtual StatusCode getRoads(const std::vector<const HTTHit*> & hits, std::vector<HTTRoad*> & roads) = 0;
};


inline const InterfaceID & HTTRoadFinderToolI::interfaceID()
{
    return IID_HTTRoadFinderToolI;
}


#endif
