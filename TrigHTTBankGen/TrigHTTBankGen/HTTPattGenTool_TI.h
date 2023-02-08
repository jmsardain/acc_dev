// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#ifndef HTTPattGenTool_TI_h
#define HTTPattGenTool_TI_h

/* HTTPattGenTool_TI.h: This file declares a class that generates patterns using the track
 * inversion method, deriving from HTTPattGenToolI.
 *
 * Declarations in this file:
 *      class HTTPattGenTool_TI
 *
 * Authors: D. Britzger, after PattBankGenAlgo by L. Luongo, G. Volpi, ....
 * Major update: Riley Xu (riley.xu@cern.ch) August 8th, 2019
 */


#include <map>
#include <vector>
#include <cmath>

#include "TrigHTTObjects/HTTPattern.h"
#include "TrigHTTMaps/HTTSSMap.h"
#include "TrigHTTBanks/HTTSectorSlice.h"
#include "TrigHTTBanks/HTTFitConstantBank.h"
#include "TrigHTTBankGen/HTTPattGenToolI.h"


class ITrigHTTBankSvc;
class HTTTrack;

/*
 * HTTPattGenTool_TI
 *
 * Generates patterns using the track inversion algorithm.
 */
class HTTPattGenTool_TI : public HTTPattGenToolI
{
    public:

        ///////////////////////////////////////////////////////////////////////
        // AthAlgTool

        HTTPattGenTool_TI(const std::string&, const std::string&, const IInterface*);
        virtual StatusCode initialize() override;

        ///////////////////////////////////////////////////////////////////////
        // Configuration

        // If slice parameters are not set, take them from the autotune procedure.
        void setSliceParameters();
        inline void setD0Exponent(double d0_alpha) { m_d0_alpha = d0_alpha; }

        inline bool isBeamAtZero() const
            { return hypot(m_beamspotX, m_beamspotY) < 1.0E-14; }
        inline void setBeamspot(double beamspotX, double beamspotY)
            { m_beamspotX = beamspotX; m_beamspotY = beamspotY; }


        ///////////////////////////////////////////////////////////////////////
        // Other

        HTTTrackPars const & getGenPars()  const { return m_genPars; }
        HTTTrackPars const & getSliceMin() const { return m_sliceMin; }
        HTTTrackPars const & getSliceMax() const { return m_sliceMax; }


    private:

        ///////////////////////////////////////////////////////////////////////
        // Properties

        // Slice parameters
        //    These are auto-assigned from the sector slice file, if not set.
        //    Only manually set to be more restrictive than those in file.
        HTTTrackPars m_sliceMin = 0.0;
        HTTTrackPars m_sliceMax = 0.0;

        // Beamspot
        double m_d0_alpha  = 0;
        double m_beamspotX = 0;
        double m_beamspotY = 0;

        ///////////////////////////////////////////////////////////////////////
        // Logging Objects

        HTTTrackPars m_genPars; // Parameters used to generate the current pattern candidates

        ///////////////////////////////////////////////////////////////////////
        // Upstream Objects (not owned)

        ServiceHandle<ITrigHTTBankSvc> m_HTTBankSvc;
        HTTSectorSlice *m_sectorSlice; // not owned
        HTTFitConstantBank *m_constBank; // not owned

        ///////////////////////////////////////////////////////////////////////
        // Core Functions

        // Returns patterns with the minimum number of layers missed, out of
        // all patterns found. This number of layers missed is also returned
        // by reference.
        ErrorStatus nextCandidates(std::vector<HTTPattern> & pattCands, size_t & layersMissed) override;

        HTTTrackPars getRandomTrackPars() const;
        std::vector<HTTPattern> bestPatterns(HTTTrackPars const & pars, std::vector<sector_t> const & sectors,
                std::vector<double> const & constr, size_t & layersMissed) const;
        bool makePattern(HTTTrack const & track,
                sector_t sector, HTTPattern & patt, size_t & misses) const;

        ///////////////////////////////////////////////////////////////////////
        // Helper Functions

        void transformToOrigin(HTTTrackPars & pars) const;
};

#endif
