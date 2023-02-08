// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#ifndef HTTROADUNIONTOOL_H
#define HTTROADUNIONTOOL_H

/**
 * @file HTTRoadUnionTool.h
 * @author Riley Xu - riley.xu@cern.ch
 * @date November 20th, 2020
 * @brief Wrapper class to combine multiple road-finding tools
 *
 * Declarations in this file:
 *      class HTTRoadUnionTool : public AthAlgTool, virtual public HTTRoadFinderToolI
 */

#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "TrigHTTAlgorithms/HTTRoadFinderToolI.h"


// This class is merely a lightweight wrapper around multiple road finder tools.
class HTTRoadUnionTool : public AthAlgTool, virtual public HTTRoadFinderToolI
{
    public:

        ///////////////////////////////////////////////////////////////////////
        // AthAlgTool

        HTTRoadUnionTool(const std::string&, const std::string&, const IInterface*);

        virtual StatusCode initialize() override;
        virtual StatusCode finalize() override;

        ///////////////////////////////////////////////////////////////////////
        // HTTRoadFinderToolI

        virtual StatusCode getRoads(const std::vector<const HTTHit*> & hits, std::vector<HTTRoad*> & roads) override;

        ///////////////////////////////////////////////////////////////////////
        // HTTRoadUnionTool

        ToolHandleArray<HTTRoadFinderToolI> const & tools() const { return m_tools; }

    private:

        ///////////////////////////////////////////////////////////////////////
        // Handles

        ToolHandleArray<HTTRoadFinderToolI> m_tools;
};

#endif
