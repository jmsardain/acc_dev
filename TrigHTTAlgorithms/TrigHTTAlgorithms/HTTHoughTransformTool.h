/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTTHOUGHTRANSFORMTOOL_H
#define HTTHOUGHTRANSFORMTOOL_H

/**
 * @file HTTHoughTransformTool.h
 * @author Riley Xu - riley.xu@cern.ch
 * @date October 31st, 2020
 * @brief Implements road finding using a Hough transform.
 *
 * Declarations in this file:
 *      class HTTHoughTransformTool : public AthAlgTool, virtual public HTTRoadFinderToolI
 *
 * Using the Lorentz force equation, one can relate the phi of a track and the
 * coordinate of a single hit:
 *
 *      A * q / pT = sin(phi_track - phi_hit) / r
 *
 * where
 *      A   : 3 * 10^-4 GeV / (c*mm*e)
 *      q   : charge of the particle
 *      pT  : transverse momentum
 *      r   : cylindrical radius of the hit from the beamline
 *      phi : in radians
 *
 * Here, q/pT and phi_track are unknown. This equation forms a line in q/pT vs
 * phi_track space. Each hit will have its own line based on its phi and r.
 * However, note that hits belonging to the same track will have lines that
 * intersect at the track's q/pT and phi. In this manner, we can conduct pattern
 * -matching by looking for intersections of these pT-phi lines.
 *
 * To easily find intersections, we first pixelate (equivalently, we make a 2d
 * histogram from) the graph of all the hit's lines in q/pT vs phi_track space.
 * We then apply a convolution (i.e. a scanning window) to pick out points with
 * multiple lines going through them. These points become our roads.
 *
 * In principle the Hough transform can be used for an entire region (i.e. .2 phi x .2 eta) or larger.
 * However this can lead to an excessive number of hits/lines in the transform image, leading
 * to spurious intersections. Instead, we can use multiple transforms that each cover a slice
 * in z0, and simply combine all the roads found.
 *
 * References:
 *      Martensson Thesis: http://uu.diva-portal.org/smash/get/diva2:1341509/FULLTEXT01.pdf
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
#include <utility>
#include <unordered_set>

class IHTTEventSelectionSvc;
class ITrigHTTBankSvc;
class ITrigHTTMappingSvc;


/* I adopt the following nomenclature within this class:
 *      image: The 'graph' in q/pT vs phi_track space, filled with a line calculated as above for each hit.
 *      point: A specific q/pT and phi_track bin in the above image; i.e. what is normally called a pixel
 *             but I don't want to confuse this with the detector type. A point's value is the number of
 *             lines that go through it.
 *
 * For the first iteration, x refers to phi_track, and y refers to q/pT, although
 * this should remain flexible. These are set via the variables m_par_x and m_par_y.
 *
 * NOTE: We store the image in graph sense and not computer-science sense. That is,
 * the row-index is y. The y-axis still points downwards, so that y=0 represents the
 * lowest bin.
 *      image[y=0][x=0]      : lowest q/pT and lowest phi_track bin
 *      image[y=size-1][x=0] : highest q/pT and lowest phi_track bin
 *
 * I use the following units for relevant variables:
 *      x,y,z,r : mm
 *      q       : e
 *      pT      : GeV / c
 */
class HTTHoughTransformTool : public AthAlgTool, virtual public HTTRoadFinderToolI
{
    public:

        ///////////////////////////////////////////////////////////////////////
        // AthAlgTool

        HTTHoughTransformTool(const std::string&, const std::string&, const IInterface*);

        virtual StatusCode initialize() override;
        virtual StatusCode finalize() override;

        ///////////////////////////////////////////////////////////////////////
        // HTTRoadFinderToolI

        virtual StatusCode getRoads(const std::vector<const HTTHit*> & hits, std::vector<HTTRoad*> & roads) override;

        ///////////////////////////////////////////////////////////////////////
        // HTTHoughTransformTool

        double getMinX() const { return m_parMin[m_par_x]; }
        double getMaxX() const { return m_parMax[m_par_x]; }
        double getMinY() const { return m_parMin[m_par_y]; }
        double getMaxY() const { return m_parMax[m_par_y]; }
        unsigned getThreshold() const { return m_threshold[m_threshold.size() / 2]; }
        int getSubRegion() const { return m_subRegion; }

        static constexpr double A = 0.0003;

        // Apply correction due to B != 2T everywhere. This correction should be ADDED to
        // phi_track.
        static double fieldCorrection(unsigned region, double y, double r);

        double yToX(double y, HTTHit const * h) const;


        typedef vector2D<std::pair<int, std::unordered_set<const HTTHit*>>> Image;
            // An image is a 2d array of points, where each point has a value.
            // The value starts as the number of hit layers, but can change with effects
            // like a convolution. Also stored are all hits that contributed to each bin.
            // Size m_imageSize_y * m_imageSize_x. (NOTE y is row coordinate)
        Image const & getImage() const { return m_image; } // Returns the image generated from the last call of getRoads

    private:

        ///////////////////////////////////////////////////////////////////////
        // Handles

        ServiceHandle<IHTTEventSelectionSvc> m_EvtSel;
        ServiceHandle<ITrigHTTBankSvc> m_HTTBankSvc;
        ServiceHandle<ITrigHTTMappingSvc> m_HTTMapping;

        ///////////////////////////////////////////////////////////////////////
        // Properties

        // ==== Hits ===
        int m_subRegion = 0; // -1 for entire region (no slicing)

        BooleanProperty m_traceHits = true; // Trace each hit that goes in a bin
            // Disabling this will save memory/time since each bin doesn't have to store all its hits
            // but the roads created won't have hits from convolution, etc.
        BooleanProperty m_fieldCorrection = true;

        // === Image ===
        HTTTrackPars::pars_index m_par_x = HTTTrackPars::IPHI; // sets phi as the x variable
        HTTTrackPars::pars_index m_par_y = HTTTrackPars::IHIP; // sets q/pT as the y variable

        HTTTrackPars m_parMin; // These are the bounds of the image, i.e. the region of interest
        HTTTrackPars m_parMax; // Only the two parameters chosen above are used for the image, however others may be used for slicing

        unsigned m_imageSize_x = 0; // i.e. number of bins in phi_track
        unsigned m_imageSize_y = 0; // i.e. number of bins in q/pT

        // === Smearing ===
        unsigned m_convSize_x = 0; // Size of convolution
        unsigned m_convSize_y = 0; //
        std::vector<int> m_conv; // Convolution filter, with size m_convSize_y * m_convSize_x

        std::vector<unsigned> m_hitExtend_x; // Hit lines will fill extra bins in x by this amount on each side, size == nLayers

        unsigned m_nCombineLayers; // number of layers after combined
        std::vector<unsigned> m_combineLayers; // Combine layers i.e. [0,1,2,0,1,2,0,1] will combine (L0, L3, L6) (L1, L4, L7) (L2, L5)
        std::vector<std::vector<unsigned>> m_combineLayer2D; // 2d array of combined layers i.e. [[1,2,3],[0,4,5],[6,7]] will combine (L1, L2, L3), (L0, L4, L5), (L6, L7)
        std::vector<unsigned> m_binScale;

        // === Roads ===
        std::vector<int> m_threshold; // Minimum point value post-convolution to accept as a road (inclusive)
        IntegerProperty m_localMaxWindowSize = 0; // Only create roads from a local maximum, requires traceHits
        BooleanProperty m_useSectors = false; // Will reverse calculate the sector for track-fitting purposes
        BooleanProperty m_idealGeoRoads = false; // will set sectors for the road based on the pt of the track

        ///////////////////////////////////////////////////////////////////////
        // Convenience

        unsigned m_nLayers; // alias to m_HTTMapping->PlaneMap1stStage()->getNLogiLayers();

        double m_step_x = 0; // step size of the bin boundaries in x
        double m_step_y = 0; // step size of the bin boundaries in y
        std::vector<double> m_bins_x; // size == m_imageSize_x + 1.
        std::vector<double> m_bins_y; // size == m_imageSize_y + 1
            // Bin boundaries, where m_bins_x[i] is the lower bound of bin i.
            // These are calculated from m_parMin/Max.

        ///////////////////////////////////////////////////////////////////////
        // Event Storage

        Image m_image;
        std::vector<HTTRoad_Hough> m_roads;


        ///////////////////////////////////////////////////////////////////////
        // Core

        // std::vector<HTTHit const *> filterHits(std::vector<HTTHit const *> const & hits) const;
        Image createLayerImage(std::vector<unsigned> const & combine_layers, std::vector<HTTHit const *> const & hits, unsigned const scale) const;
        Image createImage(std::vector<HTTHit const *> const & hits) const;
        Image convolute(Image const & image) const;

        ///////////////////////////////////////////////////////////////////////
        // Helpers

        std::pair<unsigned, unsigned> yToXBins(size_t yBin_min, size_t yBin_max, HTTHit const * hit) const;
        unsigned getExtension(unsigned y, unsigned layer) const;
        bool passThreshold(Image const & image, unsigned x, unsigned y) const;
        void matchIdealGeoSector(HTTRoad_Hough & r) const;
        void addRoad(std::vector<std::vector<const HTTHit*>> const & hits, layer_bitmask_t hitLayers, unsigned x, unsigned y);
        void addRoad(std::unordered_set<const HTTHit*> const & hits, unsigned x, unsigned y);
        void addRoad(std::vector<const HTTHit*> const & hits, unsigned x, unsigned y);
        int conv(unsigned y, unsigned x) { return m_conv[y * m_convSize_x + x]; } // NOTE: y index is first
        void drawImage(Image const & image, std::string const & name);
};



#endif // HTTHOUGHTRANSFORMTOOL_H
