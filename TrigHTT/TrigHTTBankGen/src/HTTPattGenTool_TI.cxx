// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

/*
 * HTTPattGenTool_TI.cxx: See HTTPattGenTool_TI.h.
 *
 * Authors: D. Britzger, after PattBankGenAlgo by L. Luongo, G. Volpi, ....
 * Major update: Riley Xu (rixu@cern.ch) August 8th, 2019
 *
 */

#include "TrigHTTBankGen/HTTPattGenTool_TI.h"

#include "TrigHTTUtils/HTTMacros.h"
#include "TrigHTTObjects/HTTTrack.h"
#include "TrigHTTBanks/ITrigHTTBankSvc.h"
#include "TrigHTTBanks/HTTSectorBank.h"

#include <cmath>

using std::vector;

/*****************************************************************************/
/* Constructor                                                               */
/*****************************************************************************/

HTTPattGenTool_TI::HTTPattGenTool_TI(const std::string& algname, const std::string &name, const IInterface *ifc) :
    HTTPattGenToolI(algname, name, ifc),
    m_HTTBankSvc("TrigHTTBankSvc", name)
{
    // Sector parameters
    declareProperty("phi_min", m_sliceMin.phi);
    declareProperty("phi_max", m_sliceMax.phi);
    declareProperty("c_min", m_sliceMin.qOverPt);
    declareProperty("c_max", m_sliceMax.qOverPt);
    declareProperty("d0_min", m_sliceMin.d0);
    declareProperty("d0_max", m_sliceMax.d0);
    declareProperty("z0_min", m_sliceMin.z0);
    declareProperty("z0_max", m_sliceMax.z0);
    declareProperty("eta_min", m_sliceMin.eta);
    declareProperty("eta_max", m_sliceMax.eta);

    // Beam spot
    declareProperty("d0alpha", m_d0_alpha, "Default 0");
    declareProperty("beamspotX", m_beamspotX);
    declareProperty("beamspotY", m_beamspotY);
}


StatusCode HTTPattGenTool_TI::initialize()
{
    ATH_CHECK(HTTPattGenToolI::initialize());
    ATH_CHECK(m_HTTBankSvc.retrieve());

    // Get slice info
    m_sectorSlice = m_HTTBankSvc->SectorSlice();
    setSliceParameters();

    // Get HTTFitConstantBank
    m_constBank = m_HTTBankSvc->FitConstantBank_1st();
    m_constBank->prepareInvFitConstants();

    return StatusCode::SUCCESS;
}


/*****************************************************************************/
/* Configuration                                                             */
/*****************************************************************************/

// If slice parameters are not set, take them from the sector slice boundaries.
void HTTPattGenTool_TI::setSliceParameters()
{
    std::pair<HTTTrackPars, HTTTrackPars> bnds = m_sectorSlice->getBoundaries();

    for (unsigned i = 0; i < HTTTrackPars::NPARS; i++)
    {
        //if (std::isnan(m_sliceMin[i])) m_sliceMin[i] = bnds.first[i];
        //if (std::isnan(m_sliceMax[i])) m_sliceMax[i] = bnds.second[i];

        // Gaudi throws warning if param is nan by default, so hopefully no one wants 0 as their bound
        if (m_sliceMin[i] == 0) m_sliceMin[i] = bnds.first[i];
        if (m_sliceMax[i] == 0) m_sliceMax[i] = bnds.second[i];
    }

    ATH_MSG_INFO("Slice parameters min: " << m_sliceMin);
    ATH_MSG_INFO("Slice parameters max: " << m_sliceMax);
}


/*****************************************************************************/
/* Core Functions                                                            */
/*****************************************************************************/

ErrorStatus HTTPattGenTool_TI::nextCandidates(vector<HTTPattern> & pattCands, size_t & layersMissed)
{
    bool smear = true;
    pattCands.clear();

    // Build track with random numbers
    m_genPars = getRandomTrackPars();

    // Find sectors that contain this track using sector slices
    vector<int> sectors = m_sectorSlice->searchSectors(m_genPars);
    if (sectors.size() == 0) return ES_FAIL;

    // Generate constraints (random numbers from the standard Gaussian Distribution or 0)
    static vector<double> constr(m_constBank->getNConstr()); // filled with 0
    if (smear)
        for (unsigned int i=0; i<constr.size(); ++i)
            constr[i] = getRandomGaus();

    // Get the pattern candidates
    pattCands = bestPatterns(m_genPars, sectors, constr, layersMissed);

    return ES_OK;
}


// Get random track using the sector parameters.
// FTK Inherited.
HTTTrackPars HTTPattGenTool_TI::getRandomTrackPars() const
{
    HTTTrackPars pars;

    pars.qOverPt = (m_sliceMax.qOverPt - m_sliceMin.qOverPt) * getRandom() + m_sliceMin.qOverPt;
    pars.eta = (m_sliceMax.eta - m_sliceMin.eta) * getRandom() + m_sliceMin.eta;
    pars.z0  = (m_sliceMax.z0  - m_sliceMin.z0)  * getRandom() + m_sliceMin.z0;
    pars.phi = (m_sliceMax.phi - m_sliceMin.phi) * getRandom() + m_sliceMin.phi;
    pars.phi = remainder(pars.phi, 2.*M_PI); // Convert phi to the format used in the constants calculations

    double d0;
    if ((m_sliceMin.d0 != m_sliceMax.d0) && (m_d0_alpha != 0.)) {
        // generate density f(r)= k*abs(r)^n
        //  int_0^x dx' = int_r0^r f(r')dr'
        //   x = F(r)-F(r0)
        //   z=x+F(r0) = F(r)
        //   F^-1(z) = r
        //   F(r) = k*r*abs(r)^n
        //   1/k = r1*abs(r1)^n-r0*abs(r0)^n
        //   z/abs(z) * (abs(z)/k)^(1/(n+1)) = r

        double f0=(m_sliceMin.d0!=0.) ? m_sliceMin.d0*pow(fabs(m_sliceMin.d0),m_d0_alpha) : m_sliceMin.d0;
        double f1=(m_sliceMax.d0!=0.) ? m_sliceMax.d0*pow(fabs(m_sliceMax.d0),m_d0_alpha) : m_sliceMax.d0;
        double k=1./(f1-f0);
        if(k<0.0) {
            // swap f0,f1
            k=f0; f0=f1; f1=k;
            k=1./(f1-f0);
        }
        double z=getRandom()+k*f0;
        d0 = (z !=0.0) ? pow(fabs(z)/k,1./(1.+m_d0_alpha)) : 0.0;
        if(z<0.0) d0= -d0;

    } else {
        d0=(m_sliceMax.d0 - m_sliceMin.d0) * getRandom() + m_sliceMin.d0;
    }
    pars.d0 = d0;

    if (!isBeamAtZero())
        transformToOrigin(pars); // transform D0,Z0 (defined wrt beamspot) to origin

    return pars;
}


// Create a pattern from 'track' for each sector in sectors, using an inverse
// linear fit. Pick the patterns with the least misses.
vector<HTTPattern> HTTPattGenTool_TI::bestPatterns(HTTTrackPars const & pars,
        vector<sector_t> const & sectors, vector<double> const & constr, size_t & layersMissed) const
{
    // TODO make more intuitive. Separate storing the track parameters (input)
    // from the coordinates of the hits (output) in m_constBank->invlinfit
    HTTTrack track;
    //need to initialise the track to the right size
    track.setNLayers(pmap()->getNLogiLayers());
    track.setPars(pars);
    track.setPhi(track.getPhi(), false);
    track.setD0(track.getD0());
    track.setZ0(track.getZ0());

    size_t minMisses = -1; // Note we could short-cut by setting this to m_maxWCs, but useful to keep around bad patterns (if none are good) for monitoring
    vector<HTTPattern> best_patts;

    for (sector_t sector : sectors)
    {
        m_constBank->invlinfit(sector, track, &constr[0]); // sets track.setCoord

        HTTPattern patt; size_t misses;
        bool ok = makePattern(track, sector, patt, misses);
        if (!ok) continue;

        if (misses < minMisses)
        {
            minMisses = misses;
            best_patts.clear();
            best_patts.push_back(patt);
        }
        else if (misses == minMisses)
        {
            best_patts.push_back(patt);
        }
    }

    layersMissed = minMisses;
    return best_patts;
}


// Create a single pattern from the coordinates in 'track'. Updates misses
// with the number of layers the track coordinates fall outside the sector.
// These misses are filled with wildcards. Returns true on success.
// FTK Inherited.
bool HTTPattGenTool_TI::makePattern(HTTTrack const & track,
        sector_t sector, HTTPattern & patt, size_t & misses) const
{
    patt = { sector, 1, pattern_t(m_nLayers) };
    misses = 0;

    for (size_t layer = 0; layer < m_nLayers; layer++)
    {
        SiliconTech Tech = pmap()->getDetType(layer);
        module_t moduleID = m_sectorBank->getModules(sector)[layer];

        // Add wildcard if the sector has a wildcard here
        if (moduleID == MODULE_BADMODULE)
        {
            if (!m_WCAllowed[layer]) return false;
            patt.ssids[layer] = SSID_WILDCARD;
            misses++;
            continue;
        }

        // Comment from FTK: TODO
        // potential BUG with HWMODEID=2, the section number and module number are not known
        int section = 0;

        // Find x coordinate
        double x = track.getPhiCoord(layer);
        double x_width = m_HTTMapping->SSMap()->getModulePhiWidth(layer,section);

        if (x < 0 || x >= x_width) // fit fell out of module bounds
        {
            ATH_MSG_DEBUG("x: " << x << " xwidth: " << x_width);
            return false;
        }

        // Find y coordinate
        double y_width = 0, y = 0;
        if (Tech == SiliconTech::pixel)
        {
            y = track.getEtaCoord(layer);
            y_width = m_HTTMapping->SSMap()->getModuleEtaWidth(layer,section);
            if (y < 0 || y >= y_width) return false;
        }


        // Find ssid
        HTTHit hit;
        hit.setHitType(HitType::mapped);
        hit.setDetType(Tech);
	if (Tech == SiliconTech::strip) {
	  hit.setIdentifierHash(moduleID >> 2);
	  hit.setEtaIndex(moduleID & 0b11);
	}
	else {
	  hit.setIdentifierHash(moduleID);
	}
        hit.setLayer(layer);
        hit.setSection(section); // is this right?
        hit.setPhiCoord(x);
        if (Tech == SiliconTech::pixel) hit.setEtaCoord(y);

        patt.ssids[layer] = m_HTTMapping->SSMap()->getSSID(hit);
    }

    return true;
}



/*****************************************************************************/
/* Helper Functions                                                          */
/*****************************************************************************/

// FTK Inherited
// I think the idea is that this lets us generate tracks with any beamspot by moving the parameters always to the origin
void HTTPattGenTool_TI::transformToOrigin(HTTTrackPars & pars) const
{
    static double const B_FACTOR=-0.3*2.083;// conversion factor 1/Mev -> 1/mm
    // parameters defined at the nominal vertex
    double phi= pars.phi;
    double rho=B_FACTOR* pars.qOverPt;
    double cotTheta= TMath::SinH(pars.eta);
    // parameters defined at beamspot
    double d1= pars.d0;
    double z1= pars.z0;

    double sPhi=sin(phi);
    double cPhi=cos(phi);
    double R= -m_beamspotX*sPhi+m_beamspotY*cPhi;
    double Q=  m_beamspotX*cPhi+m_beamspotY*sPhi;

    double h1=2.*d1+rho*(d1*d1-Q*Q);
    double d0=R+h1/(sqrt(rho*h1+1.)+1.);

    double s= -Q/(rho*d1+1.);
    double alpha=rho*s;
    if(fabs(alpha)>1.E-7) {
        s=asin(alpha)/rho;
    }
    double z0=z1+cotTheta*s;

    pars.d0 = d0;
    pars.z0 = z0;
}

