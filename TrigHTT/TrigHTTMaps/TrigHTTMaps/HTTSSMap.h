// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#ifndef HTTSSMAP_H
#define HTTSSMAP_H


/**
 * @file HTTSSMap.h
 * @author Riley Xu - rixu@cern.ch (rewrite from FTK)
 * @date December 13th, 2019
 * @brief Creates superstrip IDs from HTTHits.
 *
 * A superstrip map is a text file that defines the size of the superstrips for each detector type.
 * The first line states the version number:
 *
 *          > Version xxx
 *
 * Following, there is a line for each physical layer (?) as defined by a corresponding plane map.
 * Each line is a space-separated array of the following info:
 *
 *          > isPixel isEndCap physLayerID phiWidth phiTotal etaWidth etaTotal
 *
 * For strip modules there are no eta columns. Comments can be written by starting the line with '!'.
 *
 * See also https://twiki.cern.ch/twiki/bin/view/Atlas/FTKConfigFiles#Super_strip_map_SSMAP.
 */


#include "AthenaBaseComps/AthMessaging.h"
#include "TrigHTTUtils/HTTMacros.h"
#include "TrigHTTUtils/HTTTypes.h"
#include "TrigHTTObjects/HTTHit.h"
#include "HTTRegionMap.h"

#include <vector>
#include <fstream>

/*
 * This class stores the widths of superstrips and modules, and converts HTTHits to ssids.
 */
class HTTSSMap : public AthMessaging
{
    public:

        ///////////////////////////////////////////////////////////////////////
        // Constructor/Destructor

        HTTSSMap(MsgStream & msg, HTTRegionMap *rmap, const std::string & filepath);

        ///////////////////////////////////////////////////////////////////////
        // Simple Getters/Setters

        const HTTRegionMap* getRegionMap() const { return m_rmap; }
        const HTTPlaneMap* getPlaneMap() const { return m_rmap->getPlaneMap(); }

        uint32_t getSSPhiWidth(const HTTHit &hit) const
            { return m_ssm[hit.getLayer()][hit.getSection()].ss_width_phi; }
        uint32_t getSSEtaWidth(const HTTHit &hit) const
            { return m_ssm[hit.getLayer()][hit.getSection()].ss_width_eta; }
        uint32_t getModulePhiWidth(const HTTHit &hit) const
            { return m_ssm[hit.getLayer()][hit.getSection()].module_width_phi; }
        uint32_t getModuleEtaWidth(const HTTHit &hit) const
            { return m_ssm[hit.getLayer()][hit.getSection()].module_width_eta; }

        uint32_t getSSPhiWidth(size_t layer, size_t section) const
            { return m_ssm.at(layer).at(section).ss_width_phi; }
        uint32_t getSSEtaWidth(size_t layer, size_t section) const
            { return m_ssm.at(layer).at(section).ss_width_eta; }
        uint32_t getModulePhiWidth(size_t layer, size_t section) const
            { return m_ssm.at(layer).at(section).module_width_phi; }
        uint32_t getModuleEtaWidth(size_t layer, size_t section) const
            { return m_ssm.at(layer).at(section).module_width_eta; }

        ///////////////////////////////////////////////////////////////////////
        // Main Interface Functions

        ssid_t getSSID(const HTTHit &hit) const;

    private:

        struct SectionInfo
        {
            uint32_t ss_width_phi     = 0;
            uint32_t ss_width_eta     = 0; // pixels only
            uint32_t module_width_phi = 0;
            uint32_t module_width_eta = 0; // pixels only
        };

        ///////////////////////////////////////////////////////////////////////
        // Member Variables

        HTTRegionMap *m_rmap = nullptr;

        std::vector<std::vector<SectionInfo>> m_ssm; // m_ssm[nplanes][nsections]

        ///////////////////////////////////////////////////////////////////////
        // Helper Functions

        void readFile(std::ifstream & fin);
        ssid_t getSSID_strip(const HTTHit &hit) const;
        ssid_t getSSID_pixel(const HTTHit &hit) const;

};

#endif // HTTSSMAP_H
