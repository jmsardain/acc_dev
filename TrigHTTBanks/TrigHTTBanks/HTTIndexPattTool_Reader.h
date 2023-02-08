#ifndef HTTINDEXPATTREADER_H
#define HTTINDEXPATTREADER_H

/**
 * @file HTTIndexPattTool_Reader.h
 * @brief This file declares AthAlgTools that read pattern banks stored in the indexed format.
 * @author Riley Xu - rixu@cern.ch
 * @date 2019-06-06
 *
 * @sa HTTIndexPattTool_Base.h HTTIndexPattTool_Writer.h
 *
 * Declarations in this file:
 *      class HTTIndexPattTool_ReaderI
 *      class HTTIndexPattTool_Reader
 *      class HTTIndexPattTool_ReaderLegacy
 *
 * The interface is defined and implemented by HTTIndexPattTool_ReaderI, which refactors
 * the implementation of a reader into a single protected virtual function.
 * This in turn is implemented by Reader and ReaderLegacy.
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

#include "TrigHTTUtils/HTTVectorMap.h"
#include "TrigHTTUtils/HTTTypes.h"
#include "TrigHTTUtils/HTTMacros.h"
#include "TrigHTTObjects/HTTPattern.h"
#include "TrigHTTBanks/HTTIndexPattTool_Base.h"

class TTree;
#include "TTreeReader.h"
#include "TTreeReaderArray.h"

#include <string>
#include <vector>
#include <map>

static const InterfaceID IID_HTTIndexPattTool_ReaderI("HTTIndexPattTool_ReaderI", 1, 0);

/*
 * This is an abstract class that implements all the interface functions and
 * refactors the difference between reading HTT and FTK banks into readPatterns_base(),
 * the only virtual function. It is implemented by HTTIndexPattTool_Reader and HTTIndexPattTool_ReaderLegacy.
 *
 * This class does not store the bank in memory, since banks may not fit
 * in memory. Instead, it reads patterns selectively by sector by coverage.
 * It will calculate necessary meta data on read and do a consistency check with
 * the tree Metadata.
 *
 * Implementation: Read and fully parse the index tree first, in initialize().
 * The Data tree entry number of a given (sector, coverage) can then easily be
 * found via a map lookup m_sectorIndex. readPatterns() can then just read that specific entry
 * to return the patterns.
 *
 * Usage:
 *      Job options:
 *          reader = HTTIndexPattTool_Reader()
 *          reader.layers = 8
 *          reader.bank_path = "patterns.root"
 *      Code:
 *          ToolHandle<HTTIndexPattTool_ReaderI> reader;
 *          auto const & sectors = reader->getSectors();
 *          for (auto sector : sectors)
 *              auto patts = reader->readPatterns(sector);
 *
 */
class HTTIndexPattTool_ReaderI : public HTTIndexPattTool_Base
{
    public:

        ///////////////////////////////////////////////////////////////////////
        // AthAlgTool

        HTTIndexPattTool_ReaderI(std::string const & type, std::string const & name, IInterface const * parent);
        virtual ~HTTIndexPattTool_ReaderI() = default;
        static const InterfaceID& interfaceID() { return IID_HTTIndexPattTool_ReaderI; }

        ///////////////////////////////////////////////////////////////////////
        // Reading

        // get a list of all sectors in this bank
        inline std::vector<sector_t> const & getSectors() const { return m_sectorIndex.getKeys(); }
        // get the entire index
        inline HTTVectorMap<sector_t, SectorIndexData> const & getIndex() const { return m_sectorIndex; }
        // get the index data for a specific sector. returns true on success
        inline ErrorStatus getSectorIndex(sector_t sector, SectorIndexData & data) const
        {
            auto it = m_sectorIndex.find(sector);
            if (it == m_sectorIndex.end()) return ES_NOTFOUND;
            data = *it;
            return ES_OK;
        }

        // Main read function. Read patterns from a sector by giving the sector number, or an iterator
        // from iterating through the return of getIndex();
        //
        // Usage:
        //      1. Read all patterns from a sector:
        //              std::vector<pattern_t> patts = readPatterns(sector);
        //
        //      2. Limit patterns read by coverage range (inclusive):
        //              std::vector<pattern_t> patts = readPatterns(sector, maxCov, minCov);
        //
        //      3. Also return the coverages of the patterns (i.e. patts[i] has coverage coverages[i]):
        //              std::vector<size_t> coverages;
        //              std::vector<pattern_t> patts = readPatterns(sector, maxCov, minCov, &coverages);
        //
        std::vector<pattern_t> readPatterns(sector_t sector,
                size_t maxCoverage = -1, size_t minCoverage = 0, std::vector<size_t> * coverages = nullptr);
        std::vector<pattern_t> readPatterns(std::vector<SectorIndexData>::const_iterator it,
                size_t maxCoverage = -1, size_t minCoverage = 0, std::vector<size_t> * coverages = nullptr);

        // tell the reader to open a new file
        inline StatusCode nextFile(std::string const & path)
        {
            m_filepath = path;
            return initialize(); // opens the file, reads the index
        }

        // calculates the number of patterns for each coverage
        // map.key = coverage, map.val = nPatterns
        std::map<size_t, size_t> calcCovTotals() const;

    protected:

        HTTVectorMap<sector_t, SectorIndexData> m_sectorIndex;

        // This function is implemented by Reader and ReaderLegacy.
        virtual std::vector<pattern_t> readPatterns_base(sector_t sector, SectorIndexData const & indexData,
                size_t maxCoverage, size_t minCoverage, std::vector<size_t> * coverages) = 0;
};


/*
 * HTTIndexPattTool_Reader:
 *
 * Reads new HTT-style indexed pattern banks.
 */
class HTTIndexPattTool_Reader : public HTTIndexPattTool_ReaderI
{
    public:

        using HTTIndexPattTool_ReaderI::HTTIndexPattTool_ReaderI;
        virtual ~HTTIndexPattTool_Reader() = default;

        StatusCode initialize() override;

    private:

        // Readers
        TTreeReader m_dataReader;
        std::vector<TTreeReaderArray<UChar_t>> m_LTIndex;

        // Helpers
        std::vector<pattern_t> readPatterns_base(sector_t sector, SectorIndexData const & indexData,
                size_t maxCoverage, size_t minCoverage, std::vector<size_t> * coverages) override;
        void readIndex();
};


/*
 * HTTIndexPattTool_ReaderLegacy:
 *
 * Same as the normal reader, but reads legacy FTK-style banks with format:
 *
 *      CpatternData tree with branches:
 *              npatt/i
 *              cpattern<#>[npatt]/b
 *      CpatternIndex tree with branches:
 *              sector/i
 *              ncov/i
 *              npattern[ncov]/i
 *              coverage[ncov]/i
 *              ndecoder<#>/i
 *              decoderData<#>[ndecoder<#>]/i
 */
class HTTIndexPattTool_ReaderLegacy : public HTTIndexPattTool_ReaderI
{
    public:

        using HTTIndexPattTool_ReaderI::HTTIndexPattTool_ReaderI;
        virtual ~HTTIndexPattTool_ReaderLegacy() = default;

        StatusCode initialize() override;

    private:

        // Data tree variables
        TTree *m_dataTree = nullptr;
        UInt_t m_npatt = 0;
        std::vector<std::vector<UChar_t>> m_cpatterns; // index by (layer, pattern)

        // Helpers
        std::vector<pattern_t> readPatterns_base(sector_t sector, SectorIndexData const & indexData,
                size_t maxCoverage, size_t minCoverage, std::vector<size_t> * coverages) override;
        void readIndex();
};


#endif
