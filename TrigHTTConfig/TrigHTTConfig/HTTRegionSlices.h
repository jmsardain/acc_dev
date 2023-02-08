// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

/**
 * @file HTTRegionSlices.h
 * @author Riley Xu - riley.xu@cern.ch
 * @date Janurary 7th, 2021
 * @brief Stores slice definitions for HTT regions
 *
 * This class reads the slice description file to get the region definition in
 * terms of track parameters. This is complementary to HTTRegionMap, which defines
 * the modules that belong in each region.
 */

#ifndef HTTREGIONSLICES_H
#define HTTREGIONSLICES_H

#include "AthenaBaseComps/AthMessaging.h"
#include "TrigHTTObjects/HTTTrackPars.h"
#include "TrigHTTObjects/HTTTruthTrack.h"

#include <vector>
#include <utility>


class HTTRegionSlices : public AthMessaging
{
 public:

  ///////////////////////////////////////////////////////////////////////
  // Constructors/Initialization

  HTTRegionSlices(MsgStream & msg, std::string const & filepath);

  ///////////////////////////////////////////////////////////////////////
  // Main Interface Functions

  unsigned nRegions() const { return m_regions.size(); }

  std::pair<HTTTrackPars, HTTTrackPars> const & getRegion(unsigned region) const { return m_regions.at(region); }
  HTTTrackPars const & getMin(unsigned region) const { return m_regions.at(region).first; }
  HTTTrackPars const & getMax(unsigned region) const { return m_regions.at(region).second; }

  bool inRegion(unsigned region, HTTTruthTrack const & t) const;

 private:

  std::vector<std::pair<HTTTrackPars, HTTTrackPars>> m_regions; // index by region, min/max

};

#endif // HTTREGIONSLICES_H
