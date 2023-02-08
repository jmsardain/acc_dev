// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#ifndef HTTCONNECTIONMAP_H
#define HTTCONNECTIONMAP_H

/**
 * @file HTTConnectionMap.h
 * @author Chris Delitzsch
 * @date February 25, 2021
 * @brief Maps first stage sectors to second stage sectors
 *
 * A connection map is a text file that maps the first stage sectors to all possible second stage sectors
 *
 * The first line indicates the first stage sector ID and the following numbers are the IDs of all the 
 * matching second stage, i.e.:
 *
 * First_stage_sector_ID Second_stage_sector_ID_1 Second_stage_sector_ID_2 ... Second_stage_sector_ID_N
 */

#include "AthenaBaseComps/AthMessaging.h"

#include <vector>
#include <fstream>

class HTTConnectionMap : public AthMessaging
{
    public:

        ///////////////////////////////////////////////////////////////////////
        // Constructor/Destructor

        HTTConnectionMap(MsgStream & msg, const std::string & filepath);

        ///////////////////////////////////////////////////////////////////////
        // Simple Getters/Setters

	std::vector<uint32_t> getSecondSectorIDs(uint32_t firstStageSectorID) const
	  { 
	    std::map<uint32_t, std::vector<uint32_t>>::const_iterator it;
	    it = m_connMap.find(firstStageSectorID);
	    if(it != m_connMap.end()) {
	      return it->second;
	    }
	    else{
	      std::vector<uint32_t> emptyVector;
	      return emptyVector;
	    }
	  }

        ///////////////////////////////////////////////////////////////////////
        // Main Interface Functions

    private:

        ///////////////////////////////////////////////////////////////////////
        // Member Variables

	std::map<uint32_t, std::vector<uint32_t>> m_connMap;

        ///////////////////////////////////////////////////////////////////////
        // Helper Functions

        void readFile(std::ifstream & fin);

};

#endif // HTTSSMAP_H
