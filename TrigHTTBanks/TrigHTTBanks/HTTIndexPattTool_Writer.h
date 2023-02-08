#ifndef HTTINDEXPATTWRITER_H
#define HTTINDEXPATTWRITER_H

/**
 * @file HTTIndexPattTool_Writer.h
 * @brief This file declares an AthAlgTool that writes pattern banks stored in the indexed format.
 * @author Riley Xu - rixu@cern.ch
 * @date 2019-06-06
 *
 * @sa HTTIndexPattTool_Base.h HTTIndexPattTool_Reader.h
 *
 * Declarations in this file:
 *      class HTTIndexPattTool_Writer
 *
 * Inheritance tree:
 *                                   +------------+
 *                                   | AthAlgTool |
 *                                   +------+-----+
 *                                          |
 *                                          |
 *                                 +--------+---------+
 *                                 | HTTIndexPattTool_Base |
 *                                 +----+------+------+
 *                                      |      |
 *          +--------------------+      |      |    +---------------------+
 *          | HTTIndexPattTool_Writer +------+      +----+ HTTIndexPattTool_ReaderI |
 *          +--------------------+                  +----+-------------+--+
 *                                                       |             |
 *                                                       |             |
 *                                                       |             |
 *                                         +-------------+------+    +-+------------------------+
 *                                         | HTTIndexPattTool_Reader |    | HTTIndexPattTool_ReaderLegacy |
 *                                         +--------------------+    +--------------------------+
 *
 */


#include "TrigHTTUtils/HTTTypes.h"
#include "TrigHTTUtils/HTTMacros.h"
#include "TrigHTTObjects/HTTPattern.h"
#include "TrigHTTBanks/HTTIndexPattTool_Base.h"

class TTree;

#include <string>
#include <vector>
#include <map>


static const InterfaceID IID_HTTIndexPattTool_Writer("HTTIndexPattTool_Writer", 1, 0);

/*
 * HTTIndexPattTool_Writer:
 *
 * Writes patterns to a pattern bank in the indexed format by accumulating
 * patterns one-by-one, merging patterns to count coverage. Flushes patterns
 * to file periodically.
 *
 * Usage:
 *      Job options:
 *          writer = HTTIndexPattTool_Writer()
 *          writer.layers = 8
 *          writer.bank_path = "patterns.root"
 *      Code:
 *          ToolHandle<HTTIndexPattTool_Writer> writer;
 *          writer->addPattern(sector, pattern);
 *          writer->write();
 */
class HTTIndexPattTool_Writer : public HTTIndexPattTool_Base
{
    public:

        ///////////////////////////////////////////////////////////////////////
        // AthAlgTool

        HTTIndexPattTool_Writer(std::string const & type, std::string const & name, IInterface const * parent);
        virtual ~HTTIndexPattTool_Writer() =default;
        virtual StatusCode initialize() override;
        virtual StatusCode finalize() override;
        static const InterfaceID& interfaceID() { return IID_HTTIndexPattTool_Writer; }

        ///////////////////////////////////////////////////////////////////////
        // Interface

        void write();

        inline void addPattern(HTTPattern const & p) { addPattern(p.sector, p.ssids, p.coverage); }
        void addPattern(sector_t sector, pattern_t const & pattern, size_t coverage = 1);

        void setDescription(std::string const & s) { m_description = s; } // also a Gaudi property

    private:
        /* Implementation:
         *
         * Store the index persistently in memory. Every maxPatterns, call flush(). Each flush()
         * writes patterns to a new temporary tree "Temp#" which has a single branch "LTI_cov_map"
         * corresponding to IWSectorData.LTI_cov_map, with one entry per sector.
         * Thus the index in memory must store multiple pointers in IWSectorData.tempEntry.
         * The merge to the final Data tree occurs in write(), along with writing the Index tree.
         */

        static const size_t maxPatterns = 100000000; // flush every 100M unique patterns

        ///////////////////////////////////////////////////////////////////////
        // State

        size_t m_nPatterns = 0; // number of unique patterns, reset each flush
        size_t m_nTemps = 0; // also temp tree number to be written

        ///////////////////////////////////////////////////////////////////////
        // Data

        struct IWSectorData // all pattern data for a single sector
        {
            // Index
            std::vector<Long64_t> tempEntry;
                // pointer to entry number in the Temp trees. index by Temp tree number.
                // appended to on flush. -1 for no entry in this Temp tree.
            std::vector<std::vector<ssid_t>> SSIDs; // index by (layer, lookup table index)

            // Data
            std::map<std::vector<UChar_t>, UInt_t> LTI_cov_map; // one entry per pattern
                // key: LTIndexes (size == m_nLayers)
                // val: coverage
        };
        std::map<sector_t, IWSectorData> m_sectorIndex;

        ///////////////////////////////////////////////////////////////////////
        // ROOT variables

        std::vector<TTree*> m_tempTrees;
        TTree* m_indexTree = nullptr;
        TTree* m_dataTree = nullptr;
        TTree* m_metadataTree = nullptr;

        ///////////////////////////////////////////////////////////////////////
        // Metadata

        std::string m_description;

        ///////////////////////////////////////////////////////////////////////
        // Helpers

        void flush();
        void mergeTemps(IWSectorData & sectorData);
        void reformat(IWSectorData & sectorData, std::vector<UInt_t> & nPatterns,
                std::vector<UInt_t> & coverages, std::vector<std::vector<UChar_t>> & LTIndexes);
        void writeIndex(sector_t sector, SectorIndexData & indexData);
        void writeData(std::vector<UInt_t> & nPatterns, std::vector<std::vector<UChar_t>> & LTIndexes);
        void writeMetadata();
};


#endif
