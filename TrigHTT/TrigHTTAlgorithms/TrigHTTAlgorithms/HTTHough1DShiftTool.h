// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#ifndef HTTHOUGH1DSHIFTTOOL_H
#define HTTHOUGH1DSHIFTTOOL_H

/**
 * @file HTTHough1DShiftTool.h
 * @author Riley Xu - riley.xu@cern.ch
 * @date November 6th, 2020
 * @brief Implements road finding using Elliot's simplified Hough transform.
 *
 * Declarations in this file:
 *      class HTTHough1DShiftTool : public AthAlgTool, virtual public HTTRoadFinderToolI
 *
 * Using the Lorentz force equation, one can relate the phi of a track and the
 * coordinate of a single hit:
 *
 *      q / pT = sin(phi_track - phi_hit) / (A * r)
 *
 *      phi_track = phi_hit + asin(A * r * q / pT)
 *
 * where
 *      A   : 10^-4 GeV / (c*mm*e)
 *      q   : charge of the particle
 *      pT  : transverse momentum
 *      r   : cylindrical radius of the hit from the beamline
 *      phi : in radians
 *
 * Here, q/pT and phi_track are unknown. This equation forms a line in q/pT vs
 * phi_track space. However, note that hits belonging to the same track will have
 * lines that intersect at the track's q/pT and phi. In this manner, we can conduct
 * road-finding by looking for intersections of these pT-phi lines.
 *
 * Note that in the barrel, r is approximately constant in each layer. Then scanning
 * q/pT is the same as shifting phi in each layer. If you find a q/pT such that, after
 * the shifts, each layer has a hit at the same phi, that indicates a track with the given
 * q/pT and phi.
 */

#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "TrigHTTUtils/HTTTypes.h"
#include "TrigHTTUtils/HTTVectors.h"
#include "TrigHTTObjects/HTTRoad.h"
#include "TrigHTTObjects/HTTHit.h"
#include "TrigHTTObjects/HTTTrackPars.h"
#include "TrigHTTAlgorithms/HTTRoadFinderToolI.h"

#include "TFile.h"

#include <string>
#include <vector>
#include <map>
#include <boost/dynamic_bitset_fwd.hpp>

class IHTTEventSelectionSvc;
class ITrigHTTMappingSvc;
class ITrigHTTBankSvc;



/*
 * The hits of an event are stored in a bit vector for each layer, with the number
 * of bits being the number of phi bins. A hit sets a bit (or a range of bits to
 * account for resolution effects) in the respective layer's bit vector.
 *
 * Shifting phi is now simply a bit shift of the bit vectors. Roads are created
 * whenever a threshold number of layers have the same phi bin set after the shift.
 *
 * Shifts are precalculated in the initialize routine. Instead of iterating over
 * q/pt, we can iterate over n-bin shift increments in the outer layer. Each shift
 * in the outer layer uniquely determines a q/pt and the corresponding shifts in
 * the other layers. This is better than a q/pt scan by taking into account the
 * quantization in the binning. Actually, we can optimize even better by iterating
 * over n-bin shifts in the difference between the inner and outer layer, to help
 * account also for the hit extension.
 *
 * I use the following units for relevant variables:
 *      x,y,z,r : mm
 *      q       : e
 *      pT      : GeV / c
 */
class HTTHough1DShiftTool : public AthAlgTool, virtual public HTTRoadFinderToolI
{
    public:

        ///////////////////////////////////////////////////////////////////////
        // AthAlgTool

        HTTHough1DShiftTool(const std::string&, const std::string&, const IInterface*);

        virtual StatusCode initialize() override;
        virtual StatusCode finalize() override;

        ///////////////////////////////////////////////////////////////////////
        // HTTRoadFinderToolI

        virtual StatusCode getRoads(const std::vector<const HTTHit*> & hits, std::vector<HTTRoad*> & roads) override;

    private:

        ///////////////////////////////////////////////////////////////////////
        // Handles

        ServiceHandle<IHTTEventSelectionSvc>  m_EvtSel;
        ServiceHandle<ITrigHTTMappingSvc> m_HTTMapping;
        ServiceHandle<ITrigHTTBankSvc> m_HTTBankSvc;

        ///////////////////////////////////////////////////////////////////////
        // Properties

        IntegerProperty m_subRegion = 0; // -1 for entire region (no slicing)
        FloatProperty m_phiMin = 0;
        FloatProperty m_phiMax = 0;
        FloatProperty m_qptMin = 0;
        FloatProperty m_qptMax = 0;
        UnsignedIntegerProperty m_phiBins = 0;
        UnsignedIntegerProperty m_threshold = 0; // Minimum number of hit layers to fire a road
        UnsignedIntegerProperty m_iterStep = 0;  // Instead of iterating over steps in pT, we iterate over iterStep-bin shifts in iterLayer
        UnsignedIntegerProperty m_iterLayer = 0; // Instead of iterating over steps in pT, we iterate over iterStep-bin shifts in iterLayer
        BooleanProperty m_useDiff = false;
        BooleanProperty m_variableExtend = false;
        BooleanProperty m_phiRangeCut = true; // default is to apply the cut
        FloatProperty m_d0spread = -1; // negative means disabled, value in mm
        FloatArrayProperty m_hitExtendProperty; // Number of adjacent bins that each hit fires (can be fractional)
        std::vector<float> m_hitExtend; // need second copy because property is "const" and can't be changed to default
        StringProperty m_bitShift_path; // Instead of calculating bit shifts, input a list of shifts via a text file
        StringProperty m_radii_file; // Use this file to specify radii 

        // this is only good for barrel, use "
        float m_r_phys[int(SiliconTech::nTechs)][8] = {{396.7,    402.7,  559.5,  565.6,  759.5,  765.7,  997.1,  1003.4},
						       {39.2,     100.2,  160.3,  228.3,  291.1,      0,      0,       0}};


	std::vector<float> m_r;  // will be filled from m_r_phys at start or from m_radii_file

        ///////////////////////////////////////////////////////////////////////
        // Convenience

        unsigned m_nLayers; // alias to m_HTTMapping->PlaneMap1stStage()->getNLogiLayers();

        float m_phiStep; // width of one phi bin
        std::vector<double> m_bins; // size == m_phiBins + 1.
            // Bin boundaries, where m_bins[i] is the lower bound of bin i.
            // These are calculated from m_phiMin/Max.

        HTTTrackPars m_regionMin; // alias to m_EvtSel->getRegions()->getMin(m_EvtSel->getReginID())
        HTTTrackPars m_regionMax; // alias to m_EvtSel->getRegions()->getMax(m_EvtSel->getReginID())

        std::vector<std::vector<int>> m_shifts; // size (nShifts, nLayers)
        std::vector<boost::dynamic_bitset<>> m_dropable; // size (nShifts, nLayers)
            // for shift patterns, we can cut duplication by only allowing some layers to be missed when doing 7/8

        std::vector<std::vector<int>> m_d0shifts; // size (nShifts, nLayers), optional subshifts by d0
  
        ///////////////////////////////////////////////////////////////////////
        // Event Storage

        std::vector<HTTRoad_Hough> m_roads;

        ///////////////////////////////////////////////////////////////////////
        // Metadata and Monitoring

        unsigned m_event = 0;
        std::string m_name; // Gets the instance name from the full gaudi name
        TFile m_monitorFile;

        ///////////////////////////////////////////////////////////////////////
        // Helpers

        void calculateShifts();
        float getPtFromShiftDiff(int shift) const;
        void readShifts(std::string const & filepath);
        std::vector<boost::dynamic_bitset<>> makeHitMasks(const std::vector<const HTTHit*> & hits);
        HTTRoad_Hough makeRoad(std::vector<const HTTHit*> hits, int bin_track, std::vector<int> const & shifts);
        void printHitMasks(std::vector<boost::dynamic_bitset<>> const & hitMasks) const;
        void drawHitMasks(std::vector<boost::dynamic_bitset<>> const & hitMasks, std::string const & name);
        void drawHitMasks(std::vector<boost::dynamic_bitset<>> const & hitMasks, std::string const & name, std::vector<int> const & shifts);
        void printShifts() const;

        std::pair<int, int> getBins(const HTTHit*) const;
        float phitrkDiff(float r1, float phi1,  float r2,  float phi2) const;
        std::pair<float, bool> phitrk(int bin, std::vector<int> const &  shifts ) const; // returns phi of track and a bool if the
                                                                                       // value is invalid because at the edge
        float qPt(float r, float deltaPhi) const;

        void calculated0Shifts();
        std::vector<int> applyVariation(std::vector<int> base, std::vector<int> var, int sign) const;
        void calculateDropable();
        std::vector<int> shiftWithDrop(std::vector<int>& shift,unsigned droplayer) const;

        void readRadii(std::string const & filepath);
};


#endif // HTTHOUGH1DSHIFTTOOL_H
