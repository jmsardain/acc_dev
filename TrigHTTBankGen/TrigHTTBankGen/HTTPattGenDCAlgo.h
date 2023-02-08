// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#ifndef HTTPATTGENDC_H
#define HTTPATTGENDC_H

/**
 * @file HTTPattGenDCAlgo.h
 * @author Riley Xu - rixu@cern.ch
 * @date 2019/06/11
 * @brief Athena algorithm to create a DC pattern bank from TSP patterns.
 *
 * Declarations in this file:
 *      class HTTPattGenDCAlgo
 *
 */

#include "AthenaBaseComps/AthAlgorithm.h"
#include "TrigHTTMaps/ITrigHTTMappingSvc.h"
#include "TrigHTTObjects/HTTPattern.h"
#include "TrigHTTBanks/HTTPatternBank.h"
#include "TrigHTTBanks/HTTIndexPattTool_Reader.h"
#include "TrigHTTBanks/HTTCCachePattIO.h"

#include <string>
#include <vector>
#include <memory>


/*
 * HTTPattGenDC
 *
 * Creates an indexed pattern bank (TSP) to a ccache bank (DC), converting
 * TSP patterns to DC patterns if requested. TSP patterns are added to the DC
 * bank in decreasing coverage order. After the bank is full, patterns of the
 * final coverage will continue to be merged into the bank. Patterns are merged
 * in the order they are read.
 *
 * Usage (job options):
 *      reader = HTTIndexPattTool_Reader() # algo is currently hard-coded to use default-named
 *      reader.layers = layers
 *      reader.bank_path = input_bank
 *
 *      pgdc = HTTPattGenDC()
 *      pgdc.nDC = [1,0,0,0,0,0,1,1]
 *      pgdc.outbank = "ccache.out.root"
 *      ... other optional properties ...
 */
class HTTPattGenDCAlgo : public AthAlgorithm
{
    public:

        HTTPattGenDCAlgo(const std::string& name, ISvcLocator* pSvcLocator);
        StatusCode initialize() override;
        StatusCode execute() override;
        StatusCode finalize() override;


    private:

        /*
         * Implementation: Read the index bank with m_reader. Add/merge patterns into
         * m_dcPatts in addPatternsByCoverage() until bank is full. Finally reformat
         * m_dcPatts into a HTTPatternBank and write to file.
         */

        ///////////////////////////////////////////////////////////////////////
        // Properties

        // Configuration
        std::vector<int> m_nDC; // Number of DC bits for each layer. size() == nLayers.
            // Assumes DC bits are least-significant bits.
            // Used to create m_allowedDCBits.
        size_t m_maxNPatts = -1; // max number of DC patterns that can fit in the bank
        size_t m_nDCMax    = -1; // maximum number of DC bits in a single pattern
        std::string m_outBankPath;
        std::string m_dumpPatternsFile;

        // Metadata
        std::string m_description;

        ///////////////////////////////////////////////////////////////////////
        // Other members

        // Configuration
        size_t m_nLayers = 0; // alias to m_HTTMapping->PlaneMap1stStage()->getNLogiLayers();
        std::vector<ssid_t> m_allowedDCBits; // allowed DC bits per plane, bitmask
        ServiceHandle<ITrigHTTMappingSvc> m_HTTMapping;

        // I/O
        ToolHandle<HTTIndexPattTool_ReaderI> m_reader;
        ServiceHandle<HTTCCachePattIO> m_writer;

        // Data
        size_t m_nPatts = 0; // number of DC patterns stored already
        std::map<sector_t, vector<HTTDCPattern>> m_dcPatts; // intermediate storage of patterns
            // index by sector for efficient merging
            // these are ordered in the order they are stored (i.e. no particular order)


        ///////////////////////////////////////////////////////////////////////
        // Helper Functions

        void addCovRange(size_t maxCov, size_t minCov);
        bool addPattern(std::vector<HTTDCPattern> & dcPatts, pattern_t const & patt, size_t coverage);
        bool mergeable(ssid_t ss1, ssid_t ss2, ssid_t allowed_bits);
        bool mergePattern(HTTDCPattern & tgt_patt, pattern_t const & patt, size_t coverage);
        void createBitmasks();
        void reformatBank(HTTPatternBank & bank, std::map<sector_t, pid_t> const & startPIDs);
        void updateMetadata(HTTPatternBank & bank);
        void dumpPatterns();
};

#endif // HTTPATTGENDC_H
