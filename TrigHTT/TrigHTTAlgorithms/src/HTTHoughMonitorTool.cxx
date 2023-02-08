/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTAlgorithms/HTTHoughMonitorTool.h"

#include "TrigHTTAlgorithms/HTTHoughTransformTool.h"
#include "TrigHTTAlgorithms/HTTRoadUnionTool.h"
#include "TrigHTTObjects/HTTLogicalEventInputHeader.h"
#include "TrigHTTObjects/HTTRoad.h"
#include "TrigHTTObjects/HTTTruthTrack.h"

static const InterfaceID IID_HTTHoughMonitorTool("HTTHoughMonitorTool", 1, 0);
const InterfaceID& HTTHoughMonitorTool::interfaceID()
{ return IID_HTTHoughMonitorTool; }

unsigned nHitLayers(std::vector<HTTHit> const & hits);

///////////////////////////////////////////////////////////////////////////////
// Initialize
///////////////////////////////////////////////////////////////////////////////
HTTHoughMonitorTool::HTTHoughMonitorTool(const std::string& algname, const std::string& name, const IInterface *ifc) :
    HTTMonitorBase(algname,name,ifc)
{
    declareInterface<HTTHoughMonitorTool>(this);
}


StatusCode HTTHoughMonitorTool::initialize()
{
    ATH_CHECK(HTTMonitorBase::initialize());
    ATH_CHECK(bookHistograms());
    return StatusCode::SUCCESS;
}


StatusCode HTTHoughMonitorTool::bookHistograms()
{
    setHistDir("/Hough/");

    h_res_withroad_pt_matched_all = new TH1F("h_res_withroad_pt_matched_all", "Road Resolution as function of truth pt;p_{T} (GeV);", 100, -1.5, 1.5);
    h_res_withroad_phi_matched_all = new TH1I("h_res_withroad_phi_matched_all", "Road Resolution as function of truth phi;#phi;", 100, -0.05, 0.05);

    double rmax = m_evtSel->getMin().eta < 3 ? 1100 : 400;
    h_res_hitPhi[0] = new TH2I("h_res_hitPhi_neg", "Hit #varphi - expected #varphi from Hough equation;#Delta#varphi;r_{hit} (mm)", 100, -0.05, 0.05, 11, 0, rmax);
    h_res_hitPhi[1] = new TH2I("h_res_hitPhi_pos", "Hit #varphi - expected #varphi from Hough equation;#Delta#varphi;r_{hit} (mm)", 100, -0.05, 0.05, 11, 0, rmax);
    h_res_hitPhi_qpt = new TH2I("h_res_hitPhi_qpt", "(Hit #varphi - expected #varphi from Hough) / q/p_T;#Delta#varphi / q/p_T (GeV);r_{hit} (mm)", 100, -0.1, 0.1, 110, 0, rmax);
    h_res_hitPhi_cor[0] = new TH2I("h_res_hitPhi_cor_neg", "Hit #varphi - expected #varphi from corrected Hough equation;#Delta#varphi;r_{hit} (mm)", 100, -0.05, 0.05, 11, 0, rmax);
    h_res_hitPhi_cor[1] = new TH2I("h_res_hitPhi_cor_pos", "Hit #varphi - expected #varphi from corrected Hough equation;#Delta#varphi;r_{hit} (mm)", 100, -0.05, 0.05, 11, 0, rmax);

    double A = HTTHoughTransformTool::A;
    h_res_hitA[0] = new TH2I("h_res_hitA_neg", "A value needed in Hough equation for correct phi;r_{hit} (mm);A (#frac{GeV}{c*mm*e})", 100, 0, rmax, 100, 0.5 * A, 1.5 * A);
    h_res_hitA[1] = new TH2I("h_res_hitA_pos", "A value needed in Hough equation for correct phi;r_{hit} (mm);A (#frac{GeV}{c*mm*e})", 100, 0, rmax, 100, 0.5 * A, 1.5 * A);

    ATH_CHECK(regHist(getHistDir(), h_res_withroad_pt_matched_all));
    ATH_CHECK(regHist(getHistDir(), h_res_withroad_phi_matched_all));
    ATH_CHECK(regHist(getHistDir(), h_res_hitPhi[0]));
    ATH_CHECK(regHist(getHistDir(), h_res_hitPhi[1]));
    ATH_CHECK(regHist(getHistDir(), h_res_hitPhi_qpt));
    ATH_CHECK(regHist(getHistDir(), h_res_hitPhi_cor[0]));
    ATH_CHECK(regHist(getHistDir(), h_res_hitPhi_cor[1]));
    ATH_CHECK(regHist(getHistDir(), h_res_hitA[0]));
    ATH_CHECK(regHist(getHistDir(), h_res_hitA[1]));

    return StatusCode::SUCCESS;
}


void HTTHoughMonitorTool::setTool(const IAlgTool* rf)
{
    if (auto uni = dynamic_cast<const HTTRoadUnionTool*>(rf))
    {
        for (auto & tool : uni->tools())
            if (auto ptr = dynamic_cast<const HTTHoughTransformTool*>(tool.get()))
                m_houghTools.push_back(ptr);
    }
    else if (auto tool = dynamic_cast<const HTTHoughTransformTool*>(rf))
    {
        m_houghTools.push_back(tool);
    }
}


///////////////////////////////////////////////////////////////////////////////
// Filling
///////////////////////////////////////////////////////////////////////////////

StatusCode HTTHoughMonitorTool::fillHistograms()
{
    ATH_CHECK(fillHitHistograms());
    ATH_CHECK(fillRoadHistograms(getMonitorRoads("Roads_1st")));

    clearMonitorData();

    return StatusCode::SUCCESS;
}

StatusCode HTTHoughMonitorTool::fillHitHistograms()
{
    std::vector<HTTTruthTrack> const & truthtracks = m_logicEventInputHeader_1st->optional().getTruthTracks();
    std::vector<HTTHit> const & hits = m_logicEventInputHeader_1st->towers().front().hits();

    if (!m_withPU)
    {
        // auto hough = m_houghTools.front();
        HTTTruthTrack const & truth_t = truthtracks.front();

        for (HTTHit const & hit : hits)
        {
            double A = HTTHoughTransformTool::A;
            double r = hit.getR();
            double qpt = truth_t.getQOverPt() * 1000; // MeV -> GeV
            double phi_hit = hit.getGPhi();
            double phi_trk = truth_t.getPhi();
            double phi_exp = truth_t.getPhi() - asin(r * A * qpt);
            double correction = HTTHoughTransformTool::fieldCorrection(m_evtSel->getRegionID(), qpt, r);
            double phi_cor = phi_exp - correction; // minus, since we're solving for phi_hit not phi_track

            h_res_hitPhi[truth_t.getQ() > 0]->Fill(phi_hit - phi_exp, r);
            h_res_hitPhi_qpt->Fill((phi_hit - phi_exp) / qpt, r);
            h_res_hitPhi_cor[truth_t.getQ() > 0]->Fill(phi_hit - phi_cor, r);
            h_res_hitA[truth_t.getQ() > 0]->Fill(r, sin(phi_trk - phi_hit) / (r * qpt));
        }

    }

    return StatusCode::SUCCESS;
}

StatusCode HTTHoughMonitorTool::fillRoadHistograms(std::vector<HTTRoad*> const * roads)
{
    std::vector<HTTTruthTrack> const & truthtracks = m_logicEventInputHeader_1st->optional().getTruthTracks();
    std::vector<HTTHit> const & hits = m_logicEventInputHeader_1st->towers().front().hits();

    // Crude efficiency on single-particle events (no slicing)
    if (!m_withPU && m_houghTools.size() == 1)
    {
        static int ndraw_miss = 0;
        static int ndraw_good = 0;

        auto hough = m_houghTools.front();
        HTTTruthTrack const & truth_t = truthtracks.front();

        if (roads->empty())
        {
            m_nMiss++;
            if (nHitLayers(hits) < hough->getThreshold())
                m_nHitsBelowThreshold++;
            else if (ndraw_miss < 10)
                ATH_CHECK(drawImage("miss" + std::to_string(ndraw_miss++), hough, roads, &truth_t));
        }
        else
        {
            if (ndraw_good < 10)
                ATH_CHECK(drawImage("good" + std::to_string(ndraw_good++), hough, roads, &truth_t));

            for (HTTRoad* road : *roads)
            {
                HTTRoad_Hough const * road_hough = dynamic_cast<HTTRoad_Hough*>(road);
                if (!road_hough) break;

                // Resolution
                h_res_withroad_pt_matched_all->Fill(truth_t.getQ() / truth_t.getPt()*0.001 - road_hough->getY());
                h_res_withroad_phi_matched_all->Fill(truth_t.getPhi() - road_hough->getX());
            }
        }
    }
    else if (m_withPU)
    {
        // Draw first 3 events, but only the subregions with roads
        static int ndraw = 0;
        if (ndraw++ < 3)
        {
            std::vector<bool> plotted(m_houghTools.size());
            for (HTTRoad* road : *roads)
            {
                HTTRoad_Hough const * road_hough = dynamic_cast<HTTRoad_Hough*>(road);
                if (!road_hough) break;

                int subr = road_hough->getSubRegion();
                if (subr == -1) subr = 0;
                else if ((size_t)subr >= m_houghTools.size())
                {
                    ATH_MSG_WARNING("Bad subregion: " << road_hough->getSubRegion());
                    continue;
                }

                if (!plotted[subr])
                {
                    ATH_CHECK(drawImage(std::to_string(ndraw) + "_slice" + std::to_string(subr), m_houghTools[subr], roads));
                    plotted[subr] = true;
                }
            }
        }
    }


    return StatusCode::SUCCESS;
}

StatusCode HTTHoughMonitorTool::drawImage(std::string const & ext, HTTHoughTransformTool const * tool, std::vector<HTTRoad*> const * roads, HTTTruthTrack const * t)
{
    setHistDir("/Hough/Images/");

    auto & image = tool->getImage();

    std::stringstream name, title;
    name << tool->name() << "_" << ext;
    if (t) title << "Truth: " << t->getPars() << ";phi;q/pT (e/GeV)";
    else title << "Hough Transform;phi;q/pT (e/GeV)";

    TH2I* h = new TH2I(name.str().c_str(), title.str().c_str(),
            image.size(1), tool->getMinX(), tool->getMaxX(),
            image.size(0), tool->getMinY(), tool->getMaxY()
    );
    ATH_CHECK(regHist(getHistDir(), h));

    for (unsigned y = 0; y < image.size(0); y++)
        for (unsigned x = 0; x < image.size(1); x++)
            h->SetBinContent(x+1, y+1, image(y, x).first); // +1 since root bins are 1-indexed

    // Hough plotting tool uses the bin error to indicate if a road was found.
    // Bin errors are sqrt(n), so anything > sqrt(n_layers) is safe to use.
    // Error is set to 100 + nLayers_missed.
    static const int offset = 100;
    for (HTTRoad* road : *roads)
    {
        HTTRoad_Hough const * road_hough = dynamic_cast<HTTRoad_Hough*>(road);
        if (road_hough->getSubRegion() != tool->getSubRegion()) continue;
        h->SetBinError(road_hough->getXBin()+1, road_hough->getYBin()+1, offset + road_hough->getNLayers() - road_hough->getNHitLayers()); // +1 since root bins are 1-indexed
    }

    return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////////////////

StatusCode HTTHoughMonitorTool::finalize()
{
    ATH_MSG_INFO("Misses: " << m_nMiss);
    ATH_MSG_INFO("\tNot enough hit layers: " << m_nHitsBelowThreshold);

    return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// Helpers
///////////////////////////////////////////////////////////////////////////////

unsigned nHitLayers(std::vector<HTTHit> const & hits)
{
    std::vector<bool> layers_hit;
    for (HTTHit const & h : hits)
    {
        if (h.getLayer() >= layers_hit.size()) layers_hit.resize(h.getLayer() + 1);
        layers_hit[h.getLayer()] = true;
    }

    unsigned layers = 0;
    for (bool hit : layers_hit) if (hit) layers++;
    return layers;
}
