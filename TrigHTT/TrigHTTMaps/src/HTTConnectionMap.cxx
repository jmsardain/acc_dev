// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

/**
 * @file HTTConnectionMap.cxx
 * @author Chris Delitzsch - chris.malena.delitzsch@cern.ch
 * @date February 25, 2021
 * @brief See HTTConnectionMap.h
 */

#include "TrigHTTMaps/HTTConnectionMap.h"

#include <string>
#include <sstream>
#include <exception>

using namespace std;

///////////////////////////////////////////////////////////////////////////////
// Constructor/Desctructor
///////////////////////////////////////////////////////////////////////////////


HTTConnectionMap::HTTConnectionMap(MsgStream & msg, const std::string & filepath) :
    AthMessaging(msg)
{

    // Open file stream
    ATH_MSG_INFO("Reading " << filepath);
    ifstream fin(filepath);
    if (!fin.is_open())
    {
        ATH_MSG_FATAL("Couldn't open " << filepath);
        throw ("HTTConnectionMap couldn't open " + filepath);
    }

    // Read the file, fill m_connMap
    readFile(fin);

}


void HTTConnectionMap::readFile(ifstream & fin)
{

    string line;

    while (getline(fin, line)){

      if (line.size()==0 || line[0] == '!') continue; // comment/empty line

      std::vector<uint32_t> secondStageIDs;
      
      uint32_t sectorID;
      uint32_t firstStageID;
      int counter = 0;
      
      istringstream sline(line);

      while (sline >> sectorID)	{ 
	//first stage sector ID is first entry
	if(counter == 0){
	  firstStageID = sectorID;
	}
	else{
	  secondStageIDs.push_back(sectorID);
	}

	counter++;
      }

      //fill the map
      m_connMap[firstStageID] = secondStageIDs;

    } // end getline
}
