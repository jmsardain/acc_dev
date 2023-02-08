// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

/**
 * @file HTTRegionSlices.h
 * @author Riley Xu - riley.xu@cern.ch
 * @date Janurary 7th, 2021
 * @brief Stores slice definitions for HTT regions
 *
 * See header.
 */

#include "TrigHTTUtils/HTTFunctions.h"
#include "TrigHTTConfig/HTTRegionSlices.h"

#include <string>
#include <iostream>
#include <fstream>


///////////////////////////////////////////////////////////////////////////////
// Constructor/Desctructor
///////////////////////////////////////////////////////////////////////////////


HTTRegionSlices::HTTRegionSlices(MsgStream & msg, std::string const & filepath) :
  AthMessaging(msg)
{
  // Open the file
  ATH_MSG_INFO("Reading " << filepath);
  std::ifstream fin(filepath);
  if (!fin.is_open())
    {
      ATH_MSG_FATAL("Couldn't open " << filepath);
      throw ("HTTRegionSlices couldn't open " + filepath);
    }

  // Variables to fill
  unsigned region;
  std::string line, key;
  HTTTrackPars min, max;

  // Parse the file
  bool ok = true;
  while (getline(fin, line))
    {
      if (line.empty() || line[0] == '#') continue;
      std::istringstream sline(line);

      ok = ok && (sline >> key);
      if (!ok) break;
      if (key == "region")
	{
	  ok = ok && (sline >> region);
	  if (ok && region > 0) m_regions.push_back({ min, max });
	  ok = ok && (region == m_regions.size());
	  min = HTTTrackPars(); // reset
	  max = HTTTrackPars(); // reset
	}
      else if (key == "phi") ok = ok && (sline >> min.phi >> max.phi);
      else if (key == "eta") ok = ok && (sline >> min.eta >> max.eta);
      else if (key == "qpt") ok = ok && (sline >> min.qOverPt >> max.qOverPt);
      else if (key == "d0") ok = ok && (sline >> min.d0 >> max.d0);
      else if (key == "z0") ok = ok && (sline >> min.z0 >> max.z0);
      else ok = false;

      if (!ok) break;
    }

  if (!ok)
    {
      ATH_MSG_FATAL("Found error reading file at line: " << line);
      throw "HTTRegionSlices read error";
    }

  m_regions.push_back({ min, max }); // last region still needs to be added
}



///////////////////////////////////////////////////////////////////////////////
// Interface Functions
///////////////////////////////////////////////////////////////////////////////


bool HTTRegionSlices::inRegion(unsigned region, HTTTruthTrack const & t) const
{
  if (region >= m_regions.size())
    {
      ATH_MSG_ERROR("inRegion() region " << region << " out-of-bounds " << m_regions.size());
      return false;
    }
  HTTTrackPars min = m_regions[region].first;
  HTTTrackPars max = m_regions[region].second;
  HTTTrackPars cur = t.getPars();

  for (unsigned i = 0; i < HTTTrackPars::NPARS; i++)
    {
      if (cur[i] < min[i]) return false;
      if (cur[i] > max[i]) return false;
    }

  return true;
}
