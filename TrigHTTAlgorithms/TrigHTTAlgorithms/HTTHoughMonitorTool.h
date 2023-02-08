/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTTHOUGHMONITORTOOL_H
#define HTTHOUGHMONITORTOOL_H

#include "TrigHTTMonitor/HTTMonitorBase.h"
#include "TrigHTTUtils/HTTTypes.h"
#include "TGraphAsymmErrors.h"

class HTTRoadFinderToolI;
class HTTHoughTransformTool;

/////////////////////////////////////////////////////////////////////////////
class HTTHoughMonitorTool: public HTTMonitorBase
{
    public:

        HTTHoughMonitorTool(const std::string&, const std::string&, const IInterface*);
        virtual ~HTTHoughMonitorTool() = default;

        static const InterfaceID& interfaceID();

        virtual StatusCode initialize() override;
        virtual StatusCode finalize() override;
        virtual StatusCode fillHistograms() override;

        void setTool(const IAlgTool* hough_tool);

    protected:

        virtual StatusCode bookHistograms() override;

    private:

        std::vector<const HTTHoughTransformTool*> m_houghTools;

        // Crude efficiency counters
        unsigned m_nMiss = 0;
        unsigned m_nHitsBelowThreshold = 0;

        // Resolution
        TH1F* h_res_withroad_pt_matched_all;
        TH1I* h_res_withroad_phi_matched_all;
        TH2I* h_res_hitPhi[2];     // neg q, pos q
        TH2I* h_res_hitPhi_qpt;
        TH2I* h_res_hitPhi_cor[2]; // neg q, pos q
        TH2I* h_res_hitA[2];       // neg q, pos q

        StatusCode fillHitHistograms();
        StatusCode fillRoadHistograms(std::vector<HTTRoad*> const * roads);
        StatusCode drawImage(std::string const & ext, HTTHoughTransformTool const * tool, std::vector<HTTRoad*> const * roads, HTTTruthTrack const * t = nullptr);
};

#endif // HTTHoughMonitorTool_h
