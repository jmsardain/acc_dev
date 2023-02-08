/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTTLLPDOUBLETHOUGHTRANSFORMTOOL_H
#define HTTLLPDOUBLETHOUGHTRANSFORMTOOL_H


#include "TFile.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TrigHTTMonitor/HTTMonitorBase.h"

#include "TrigHTTUtils/HTTTypes.h"
#include "TrigHTTUtils/HTTVectors.h"
#include "TrigHTTObjects/HTTRoad.h"
#include "TrigHTTObjects/HTTHit.h"
#include "TrigHTTObjects/HTTTrackPars.h"
#include "TrigHTTAlgorithms/HTTRoadFinderToolI.h"

//#include "TrigHTTAlgorithms/HTTDataFlowTool.h"


/*
Hough for d0, q/pt using hit doublets
Hough space x -> d0 , y -> q/pt

*/
//class HTTTruthTrack;
class HTTLLPDoubletHoughTransformTool : public AthAlgTool, virtual public HTTRoadFinderToolI {

public:
    HTTLLPDoubletHoughTransformTool(const std::string&, const std::string&, const IInterface*);

    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    virtual StatusCode getRoads(const std::vector<const HTTHit*> & hits, std::vector<HTTRoad*> & roads) override;


private:
    BooleanProperty m_traceHits = true; // Trace each hit that goes in a bin

    IntegerProperty m_threshold = 8; // munimum number of hits required for the road
    IntegerProperty m_threshold50 = 8; // munimum number of hits required for the road

    typedef vector2D<std::pair<int, std::unordered_set<const HTTHit*>>> Image;
    std::vector<HTTRoad_Hough> m_roads;

    const double m_acceptedDistanceBetweenLayersMin = 200; // min R disstance for hits pair filtering
    const double m_acceptedDistanceBetweenLayersMax = 600;
    FloatProperty m_d0_range = 120;
    FloatProperty m_qOverPt_range = 0.002;
    IntegerProperty m_imageSize_x = 216; // i.e. number of bins in d0
    IntegerProperty m_imageSize_y = 216; // i.e. number of bins in q/pT
    BooleanProperty m_triplet = false; // doublet or triplet
    double m_step_x = 0; // helpers (accumulator granularity)
    double m_step_y = 0;
    BooleanProperty m_continuous = true; // assure that there is continuity of the line (i.e. middle bins in d0 are filled when one q/pT step would result in a hole)

    StatusCode fillImage(const HTTHit *hit1, const HTTHit *hit2, Image& image) const;
    bool passThreshold(Image const & image, int x, int y) const;
    bool isLocalMaxima(Image const & image, int x, int y) const;

    HTTRoad_Hough* createRoad(std::unordered_set<const HTTHit*> const & hits, unsigned x, unsigned y) const;
    unsigned m_event = 0;
    void drawImage(Image const & image) const;

    float xtod0(int x) const { return x*m_step_x - m_d0_range; }
    float ytoqoverpt(int y) const { return y*m_step_y - m_qOverPt_range; }
    int m_eventsProcessed = 0;
    int m_roadsGenerated = 0;



    // JMS
    void setHistDir(std::string const & dir) { m_dir = "/MONITOROUT" + dir; }
    std::string const & getHistDir() { return m_dir; }
    void clearHistDir() { m_dir = ""; }
    StatusCode regHist (std::string const & dir, TH1* h) { return m_tHistSvc->regHist (dir + h->GetName(), h); }
    StatusCode regHist (std::string const & dir, TH2* h) { return m_tHistSvc->regHist (dir + h->GetName(), h); }

    ServiceHandle<ITHistSvc>                m_tHistSvc;
    std::string m_dir;


    double R(const double& x, const double& y);
    double Eta(const double& x, const double& y, const double& z);
    double Phi(const double& x, const double& y);

    double Deta(const double& x1, const double& y1, const double& z1,
                const double& x2, const double& y2, const double& z2);
    double Dphi(const double& x1, const double& y1, const double& x2, const double& y2);
    double Dz(const double& z1, const double& z2);
    double Phi_slope(const double& x1, const double& y1, const double& x2, const double& y2);

    TH1D*h_hit_dR_before;
    TH1D*h_hit_dz_before;
    TH1D*h_hit_deta_before;
    TH1D*h_hit_dphi_before;

    TH1D*h_hit_phislope_12;
    TH1D*h_hit_phislope_23;
    TH1D*h_hit_dphislope;

    TH1D*h_hit_dR_after;
    TH1D*h_hit_dz_after;
    TH1D*h_hit_deta_after;
    TH1D*h_hit_dphi_after;

    // TH2D*h_hit1_Rz_before;
    // TH2D*h_hit2_Rz_before;
    // TH2D*h_hit1_Rz_after;
    // TH2D*h_hit2_Rz_after;
    // TH2D*h_hit1_Rphi_before;
    // TH2D*h_hit2_Rphi_before;
    // TH2D*h_hit1_Rphi_after;
    // TH2D*h_hit2_Rphi_after;

    TH2D*h_hit_Rz_ev2;
    TH2D*h_hit_Rphi_ev2;

    TH2D*h_hit_Rz_ev5;
    TH2D*h_hit_Rphi_ev5;

    TH1D*h_nRoads;
    TH1D*h_time;

    std::ofstream myfile;


};

#endif //
