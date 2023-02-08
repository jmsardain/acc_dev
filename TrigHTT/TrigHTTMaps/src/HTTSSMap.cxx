// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

/**
 * @file HTTSSMap.cxx
 * @author Riley Xu - rixu@cern.ch
 * @date December 13th, 2019
 * @brief See HTTSSMap.h
 */

#include "TrigHTTUtils/HTTFunctions.h"
#include "TrigHTTObjects/HTTHit.h"
#include "TrigHTTObjects/HTT_SSID.h"
#include "TrigHTTMaps/HTTSSMap.h"

#include <string>
#include <sstream>
#include <exception>

using namespace std;


// #define TRACEBACK_ON_BOUNDARY_VIOLATION

///////////////////////////////////////////////////////////////////////////////
// Constructor/Desctructor
///////////////////////////////////////////////////////////////////////////////


HTTSSMap::HTTSSMap(MsgStream & msg, HTTRegionMap *rmap, const std::string & filepath) :
    AthMessaging(msg), m_rmap(rmap)
{
    const HTTPlaneMap *pmap = m_rmap->getPlaneMap();

    // Open file stream
    ATH_MSG_INFO("Reading " << filepath);
    ifstream fin(filepath);
    if (!fin.is_open())
    {
        ATH_MSG_FATAL("Couldn't open " << filepath);
        throw ("HTTSSMap couldn't open " + filepath);
    }

    // Resize m_ssm
    int nplanes = pmap->getNLogiLayers();
    m_ssm.resize(nplanes);
    for (int iplane=0; iplane < nplanes; iplane++)
        m_ssm[iplane].resize(pmap->getNSections(iplane));

    // Read the file, populating m_ssm
    readFile(fin);
}


void HTTSSMap::readFile(ifstream & fin)
{
    const HTTPlaneMap *pmap = m_rmap->getPlaneMap();

    string line;
    int ilayer = 0;
    int nrlayers = pmap->getNDetLayers();

    while (getline(fin, line))
    {
        if (line.size()==0 || line[0] == '!') continue; // comment/empty line
        if (ilayer == nrlayers)
        {
            ATH_MSG_WARNING("readFile() expected only " << nrlayers
                    << " layers but file has more. Check for SSMap file correctness.");
            break;
        }

        ilayer++;
        ATH_MSG_VERBOSE("readFile() reading ilayer " << ilayer << ", line: " << line);

        int isPixel, BEC, physLayer;
        int phiss, phiwidth, etass, etawidth;

        istringstream sline(line);
        sline >> isPixel >> BEC >> physLayer;
        sline >> phiss >> phiwidth >> etass >> etawidth;

        int layer   = pmap->getLayerSection(static_cast<SiliconTech>(isPixel), static_cast<DetectorZone>(BEC), physLayer).layer;
        int section = pmap->getLayerSection(static_cast<SiliconTech>(isPixel), static_cast<DetectorZone>(BEC), physLayer).section;
        if (layer == -1) continue;
            // the SS map creation for a plane is skipped if:
            // * we are creating the unused layer SS map and plane is active
            // * we are creating the active layers SS map and plane is inactive

        m_ssm[layer][section].ss_width_eta = etass;
        m_ssm[layer][section].module_width_eta = etawidth;

        m_ssm[layer][section].ss_width_phi = phiss;
        m_ssm[layer][section].module_width_phi = phiwidth;

    }  // end getline loop

    if (ilayer != nrlayers)
        ATH_MSG_WARNING("readFile() ilayer != nrlayers: " << ilayer << "," << nrlayers
                << ". Check for SSMap file correctness.");
}


///////////////////////////////////////////////////////////////////////////////
// Hit -> SSID
///////////////////////////////////////////////////////////////////////////////


ssid_t HTTSSMap::getSSID(const HTTHit &hit) const
{
    const string msgName = "getSSID() ";
    ATH_MSG_DEBUG(msgName + "processing hit: " << hit);

    ssid_t ssid = SSID_ERROR;
    if (hit.isStrip() == 1)
        ssid = getSSID_strip(hit);
    else
        ssid = getSSID_pixel(hit);

    ATH_MSG_DEBUG(msgName << "returning ssid " << ssid << "(" << formatHex(ssid) << ")");
    return ssid;
}


ssid_t HTTSSMap::getSSID_strip(const HTTHit &hit) const
{
  unsigned int x = hit.getPhiCoord();

    if (x >= m_ssm[hit.getLayer()][hit.getSection()].module_width_phi)
    {
#ifdef TRACEBACK_ON_BOUNDARY_VIOLATION
        void *buffer[100]; int nptrs = backtrace(buffer, 100);
        backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO);
#endif
        ATH_MSG_FATAL("getSSID_strip() Out-of-bounds coordinate: x=" << x << ", range [0,"
                << m_ssm[hit.getLayer()][hit.getSection()].module_width_phi << ")");
        throw std::runtime_error("HTTSSMap::getSSID()");
    }

    x /= m_ssm[hit.getLayer()][hit.getSection()].ss_width_phi;

    HTT_SSID ssid;
    ssid.setStripX(x);
    ssid.setModule(hit.getHTTIdentifierHash());

    return ssid.getSSID();
}


ssid_t HTTSSMap::getSSID_pixel(const HTTHit &hit) const
{
  unsigned int x = hit.getPhiCoord();
  unsigned int y = hit.getEtaCoord();

  if ( hit.getLayer() > unsigned(m_ssm.size()) || hit.getSection() >= unsigned(m_ssm.at(hit.getLayer()).size()) ){
    ATH_MSG_FATAL("getSSID_pixel() Out-of-bounds coordinate: " <<" tot plane "<< m_ssm.size() <<" tot section "<< m_ssm.at(hit.getLayer()).size());
    return SSID_ERROR;
  }

    if ( x >= m_ssm.at(hit.getLayer()).at(hit.getSection()).module_width_phi ||
         y >= m_ssm.at(hit.getLayer()).at(hit.getSection()).module_width_eta )

    {
#ifdef TRACEBACK_ON_BOUNDARY_VIOLATION
        void *buffer[100];int nptrs = backtrace(buffer, 100);
        backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO);
#endif
        ATH_MSG_FATAL("getSSID_pixel() Out-of-bounds coordinate: "
            << "x=" << x << " range [0," << m_ssm[hit.getLayer()][hit.getSection()].module_width_phi << ") ; "
            << "y=" << y << " range [0," << m_ssm[hit.getLayer()][hit.getSection()].module_width_eta << ")");
        throw std::runtime_error("HTTSSMap::getSSID()");
    }

    x /= m_ssm[hit.getLayer()][hit.getSection()].ss_width_phi;
    y /= m_ssm[hit.getLayer()][hit.getSection()].ss_width_eta;

    HTT_SSID ssid;
    ssid.setPixelX(x);
    ssid.setPixelY(y);
    ssid.setModule(hit.getHTTIdentifierHash());

    return ssid.getSSID();
}



