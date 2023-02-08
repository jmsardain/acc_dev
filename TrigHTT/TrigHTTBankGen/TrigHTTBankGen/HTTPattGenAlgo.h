#ifndef HTTPattGenAlgo_h
#define HTTPattGenAlgo_h

/*
 * HTTPattGenAlgo.h: This file declares an athena algorithm for generating patterns.
 *
 * Declarations in this file:
 *      class HTTPattGenAlgo
 *
 * Created: August 21st, 2019
 * Author: Riley Xu
 * Email: rixu@cern.ch
 */


#include <memory>

#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

#include "TrigHTTMaps/ITrigHTTMappingSvc.h"
#include "TrigHTTObjects/HTTTrackPars.h"
#include "TrigHTTBanks/HTTIndexPattTool_Writer.h"
#include "TrigHTTBankGen/HTTPattGenToolI.h"

#include <TH1I.h>


/*
 * This algorithm uses HTTPattGenToolI to create a pattern bank.
 * The tool handles the creation of patterns iteratively, while this algorithm
 * handles metadata and writing to file.
 *
 * Usage (job options):
 *
 *      pgt = HTTPattGen[Truth/TI]()
 *      ... set properties ...
 *
 *      pga = HTTPattGenAlgo()
 *      pga.pattGenTool = pgt
 */
class HTTPattGenAlgo: public AthAlgorithm
{
    public:
        HTTPattGenAlgo(const std::string& name, ISvcLocator* pSvcLocator);
        StatusCode initialize() override;
        StatusCode execute() override;
        StatusCode finalize() override;

    private:

        ///////////////////////////////////////////////////////////////////////
        // Properties

        ToolHandle<HTTPattGenToolI> m_pattGen;
        ToolHandle<HTTIndexPattTool_Writer> m_writer;
        std::string m_monitorFilePath = "pattgen_monitor.root";

        ///////////////////////////////////////////////////////////////////////
        // Metrics

        TFile *m_monitorFile = nullptr;

        size_t m_iter = 0; // current iteration
        size_t m_failedIters = 0; // failed iterations
        size_t m_emptyIters = 0; // number of iterations that returned no pattern candidates, most likely because no sectors were found

        std::vector<size_t> m_nPatts_WC; // change to TH1I TODO
            // number of patterns generated, organized by number of WCs
            // index by number of misses / WCs
            // includes count for patterns with WCs exceeding user constraints

        TH1I* m_h_nWC_layer; // Number of patterns with a wildcard in each layer

        // Track inversion specific metrics
        TH1I* m_h_noSector[HTTTrackPars::NPARS];  // Generation track pars when no sectors are found for a generated pattern
        TH1I* m_h_wc[HTTTrackPars::NPARS];        // Generation track pars when pattern candidates have too many wildcards
        TH1I* m_h_ok[HTTTrackPars::NPARS];        // Generation track pars when a pattern is successfully generated

        // Truth muon specific metrics
        TH1I* m_h_truthMisses; // # layers missed by the truth muon
        TH1I* m_h_matchMisses; // # layers missed because couldn't find a perfect sector matching the muon

        ///////////////////////////////////////////////////////////////////////
        // Helpers

        void bookHistograms();
        std::string nPatts_toString() const;
        void fillTrackParHists(TH1I* h[HTTTrackPars::NPARS], HTTTrackPars const & pars);
        void printHist(std::string header, TH1I* h);

};



#endif
