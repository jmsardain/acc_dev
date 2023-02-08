/*
 * HTTPattGenAlgo.cxx: See HTTPattGenAlgo.h.
 *
 * Created: August 21st, 2019
 * Author: Riley Xu
 * Email: rixu@cern.ch
 */

#include "TrigHTTBankGen/HTTPattGenAlgo.h"
#include "TrigHTTBankGen/HTTPattGenTool_TI.h"
#include "TrigHTTBankGen/HTTPattGenTool_Truth.h"
#include "GaudiKernel/IEventProcessor.h"

/*****************************************************************************/
/* Constructors                                                              */
/*****************************************************************************/


HTTPattGenAlgo::HTTPattGenAlgo(const std::string& name, ISvcLocator* pSvcLocator) :
    AthAlgorithm(name, pSvcLocator),
    m_pattGen("HTTPattGenTool_Truth"),
    m_writer("HTTIndexPattTool_Writer")
{
    declareProperty("pattGenTool", m_pattGen);
    declareProperty("monitorPath", m_monitorFilePath);
}


/*****************************************************************************/
/* Athena Overrides                                                          */
/*****************************************************************************/

StatusCode HTTPattGenAlgo::initialize()
{
    m_monitorFile = TFile::Open(m_monitorFilePath.c_str(), "recreate");
    if (!m_monitorFile || m_monitorFile->IsZombie()) ATH_MSG_FATAL("Couldn't open " << m_monitorFilePath);
    ATH_MSG_INFO("Writing monitoring to " << m_monitorFilePath);

    ATH_CHECK(m_pattGen.retrieve());
    ATH_CHECK(m_writer.retrieve());

    m_nPatts_WC.resize(m_pattGen->getNLayers() + 1);
    bookHistograms();

    return StatusCode::SUCCESS;
}


void HTTPattGenAlgo::bookHistograms()
{
    m_monitorFile->cd();

    m_h_nWC_layer = new TH1I("h_nWC_layer",
            "Number of patterns with a wildcard in each layer;layer index;# patterns",
            9, 0, 9);

    if (m_pattGen.type() == "HTTPattGenTool_TI")
    {
        HTTPattGenTool_TI* ti = dynamic_cast<HTTPattGenTool_TI*>(m_pattGen.get());
        HTTTrackPars const & min = ti->getSliceMin();
        HTTTrackPars const & max = ti->getSliceMax();
        for (unsigned i = 0; i < HTTTrackPars::NPARS; i++)
        {
            m_h_noSector[i] = new TH1I(("h_noSector_" + HTTTrackPars::parName(i)).c_str(),
                    ("Track parameters that matched no sectors in slice;" + HTTTrackPars::parName(i) + ";# events").c_str(),
                    50, min[i], max[i]);
            m_h_wc[i] = new TH1I(("h_wc_" + HTTTrackPars::parName(i)).c_str(),
                    ("Track parameters that failed because of too many wildcards;" + HTTTrackPars::parName(i) + ";# events").c_str(),
                    50, min[i], max[i]);
            m_h_ok[i] = new TH1I(("h_ok_" + HTTTrackPars::parName(i)).c_str(),
                    ("Track parameters that successfully generated a pattern;" + HTTTrackPars::parName(i) + ";# events").c_str(),
                    50, min[i], max[i]);
        }
    }
    else if (m_pattGen.type() == "HTTPattGenTool_Truth")
    {
        m_h_truthMisses = new TH1I("h_truthMisses", "# layers missed by the truth muon;# layers;# events", m_pattGen->getNLayers() + 1, 0, m_pattGen->getNLayers() + 1);
        m_h_matchMisses = new TH1I("h_matchMisses", "# layers missed from sector matching;# layers;# events", m_pattGen->getNLayers() + 1, 0, m_pattGen->getNLayers() + 1);
    }
}


// NB: Error logging should be centralized at this location; the tool classes
// shouldn't do it themselves. It may require some restructuring, i.e. TI has to
// store m_pars for logging instead of stack allocating it. But this way, we have
// consistent monitoring that doesn't obfuscate the actual pattern generation code.
// Also, the tools don't have to worry about ROOT objects or file I/O.
StatusCode HTTPattGenAlgo::execute()
{
    static bool done = false;

    if (done) {
      IEventProcessor* appMgr=nullptr;
      ATH_CHECK(service("ApplicationMgr",appMgr));
      if (!appMgr) {
	ATH_MSG_ERROR("Failed to retrieve ApplicationMgr as IEventProcessor");
	return StatusCode::FAILURE;
      }
      ATH_CHECK(appMgr->stopRun());
    }

    // Generate the next pattern
    ErrorStatus es = m_pattGen->next();

    switch (es)
    {
        case ES_OK:
        {
            // Add the pattern to the bank
            m_writer->addPattern(m_pattGen->getPattern());

    ////////////////////////////////////////////
    // Everything else below is just monitoring

            m_iter++;
            m_nPatts_WC[m_pattGen->getNWC()]++;
            layer_bitmask_t wcLayers = m_pattGen->getPattern().wcLayers();
            for (size_t layer = 0; layer < m_pattGen->getNLayers(); layer++)
                if ((wcLayers >> layer) & 1) m_h_nWC_layer->Fill(layer);
            if (m_pattGen.type() == "HTTPattGenTool_TI")
                fillTrackParHists(m_h_ok, dynamic_cast<HTTPattGenTool_TI*>(m_pattGen.get())->getGenPars());
            break;
        }
        case ES_EOF:
            done = true;
            break;
        case ES_EMPTY:
            m_iter++;
            m_failedIters++;
            m_emptyIters++;
            if (m_pattGen.type() == "HTTPattGenTool_TI")
                fillTrackParHists(m_h_noSector, dynamic_cast<HTTPattGenTool_TI*>(m_pattGen.get())->getGenPars());
            break;
        case ES_CONSTRAINT: // returned when pattern candidates have too many wildcards
            m_iter++;
            m_failedIters++;
            m_nPatts_WC[m_pattGen->getNWC()]++;
            if (m_pattGen.type() == "HTTPattGenTool_TI")
                fillTrackParHists(m_h_wc, dynamic_cast<HTTPattGenTool_TI*>(m_pattGen.get())->getGenPars());
            break;
        default:
            m_iter++;
            m_failedIters++;
            break;
    }

    if (m_pattGen.type() == "HTTPattGenTool_Truth" && es != ES_EOF)
    {
        HTTPattGenTool_Truth* pg = dynamic_cast<HTTPattGenTool_Truth*>(m_pattGen.get());
        size_t missed_truth = pg->getNLayersMissed_truth();
        m_h_truthMisses->Fill(missed_truth);
        m_h_matchMisses->Fill(pg->getNWC() - missed_truth);
    }

    if (m_iter % 100000 == 0)
        ATH_MSG_INFO("Trials: " << m_iter << " (" << m_failedIters << " failed). Patterns: " << nPatts_toString());

    return StatusCode::SUCCESS;
}


StatusCode HTTPattGenAlgo::finalize()
{
      m_writer->write();
      
      ATH_MSG_INFO("Trials: " << m_iter << " total, " << m_failedIters << " failed, " << m_emptyIters << " empty.");
      ATH_MSG_INFO("WC coverages: " << nPatts_toString());
      printHist("Wildcards/layer: ", m_h_nWC_layer);
      if (m_pattGen.type() == "HTTPattGenTool_Truth")
	{
	  printHist("Truth misses: ", m_h_truthMisses);
	  printHist("Match misses: ", m_h_matchMisses);
	}
      
      m_monitorFile->Write();
      m_monitorFile->Close();

    return StatusCode::SUCCESS;
}

/*****************************************************************************/
/* Helpers                                                                   */
/*****************************************************************************/

void HTTPattGenAlgo::printHist(std::string header, TH1I* h)
{
    msg(MSG::INFO) << header;
    for (Int_t i=0; i < h->GetNcells(); i++)
        msg() << h->GetBinContent(i) << " ";
    msg() << endmsg;
}

std::string HTTPattGenAlgo::nPatts_toString() const
{
    std::string out("(0 WCs) [");
    size_t n = 0;
    for (; n < m_nPatts_WC.size(); n++)
    {
        if (n != 0) out += " ";
        if (n == m_pattGen->getMaxWC() + 1) out += "| ";
        out += std::to_string(m_nPatts_WC[n]);
    }
    out += "] (" + std::to_string(n - 1) + " WCs)";
    return out;
}

void HTTPattGenAlgo::fillTrackParHists(TH1I* h[HTTTrackPars::NPARS], HTTTrackPars const & pars)
{
    for (unsigned i = 0; i < HTTTrackPars::NPARS; i++)
        h[i]->Fill(pars[i]);
}

