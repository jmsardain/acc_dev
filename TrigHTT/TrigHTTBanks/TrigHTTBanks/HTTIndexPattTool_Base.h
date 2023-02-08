#ifndef HTTINDEXPATTBASE_H
#define HTTINDEXPATTBASE_H

/**
 * @file HTTIndexPattTool_Base.h
 * @brief This file declares a base class for reading/writing pattern banks stored in the indexed format.
 * @author Riley Xu - rixu@cern.ch
 * @date 2019-06-06
 * @sa HTTIndexPattTool_Reader.h, HTTIndexPattTool_Writer.h
 *
 * Declarations in this file:
 *      struct SectorIndexData
 *      class HTTIndexPattTool_Base
 *
 *  This base class manages common elements between both the reader and writer classes, including
 *  managing the file and metadata. See the inheritance tree below. This class is used to reduce
 *  redundant code, and should not be used directly.
 *
 * Indexed format:
 *      This implements a pattern bank using a lookup table to obtain the SSIDs of a pattern.
 *      There exists a lookup table for each sector. The pattern data are organized by sector
 *      and coverage. This is done to easily read patterns from a specific sector / coverage range
 *      without having to load other things into memory (i.e. ROOT will not uncompress un-needed stuff).
 *
 * Indexed format definition:
 *      The file shall contain, at root level, two TTrees with names "Data, Index".
 *
 *      The Data tree shall have the following branches:
 *              nPatterns/I
 *              LTIndex<#>[nPatterns]/b     // Lookup Table Index
 *      where <#> ranges from [0, nLayers).
 *
 *      The Index tree shall have the following branches:
 *              sector/I
 *              nCoverages/I
 *              nPatterns[nCoverages]/i
 *              coverages[nCoverages]/i     // sorted largest to smallest
 *              firstDataEntry/l
 *              nSSIDs<#>/I
 *              SSIDs<#>[nSSIDs<#>]/I       // unsorted
 *      where <#> ranges from [0, nLayers).
 *
 *      The index tree contains one entry per sector. npattern[i] gives the number of patterns with
 *      coverage coverage[i]. The SSID branches store TSP SSIDs.
 *
 *      The data tree contains one entry per (sector, coverage) pair. These appear in the same order
 *      as listed in coverage[ncov]/i in the index tree. For example, if the first entry in the index
 *      tree is
 *              Index.GetEntry(0): sector=0, nCoverages=2, nPatterns=[5,10], coverages=[2,1]
 *      then the first few entries of the data tree correspond to:
 *              Data.GetEntry(0): (sector=0, coverage=2) nPatterns=5
 *              Data.GetEntry(1): (sector=0, coverage=1) nPatterns=10
 *              Data.GetEntry(2): (sector=1, coverage=...
 *      The firstDataEntry is a redundant pointer to the first entry index in the Data tree
 *      for the sector, for convenience. In the example above, Index.GetEntry(1) would have
 *      firstDataEntry = 2.
 *
 *      The LTIndex<#> branches then give the SSIDs for each pattern by indexing into the correct
 *      sector's SSIDs<#> lookup table.
 *
 *      Note ncov determines the number of entries in the data tree. That is, Data.GetEntries() == sum(Index.ncov).
 *
 *      Optionally, the file may also contain at root level a TTree with name "Metadata". This TTree shall
 *      have the properties given by TrigHTTObjects/HTTBankMetadata.h
 *
 * Inheritance tree:
 *                                   +------------+
 *                                   | AthAlgTool |
 *                                   +------+-----+
 *                                          |
 *                                          |
 *                                 +--------+--------------+
 *                                 | HTTIndexPattTool_Base |
 *                                 +------+------+---------+
 *                                        |      |
 *       +-------------------------+      |      |    +--------------------------+
 *       | HTTIndexPattTool_Writer +------+      +----+ HTTIndexPattTool_ReaderI |
 *       +-------------------------+                  +--+-------------------+---+
 *                                                       |                   |
 *                                                       |                   |
 *                                                       |                   |
 *                                         +-------------+-----------+    +--+----------------------------+
 *                                         | HTTIndexPattTool_Reader |    | HTTIndexPattTool_ReaderLegacy |
 *                                         +-------------------------+    +-------------------------------+
 */

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigHTTMaps/ITrigHTTMappingSvc.h"
#include "TrigHTTBanks/HTTBankMetadata.h"

#include "TFile.h"

#include <string>
#include <vector>
#include <memory>


struct SectorIndexData // Data stored in the index tree for each sector
{
    int64_t firstDataEntry;
    std::vector<UInt_t> nPatterns;
    std::vector<UInt_t> coverages;    // sorted largest to smallest
    std::vector<std::vector<Int_t>> SSIDs; // index by (layer, lookup table index)
};


/*
 * HTTIndexPattTool_Base:
 *
 * This is an abstract class that manages members specific to the file format.
 * It declares Athena properties and implements some metadata functions.
 *
 * Derived by HTTIndexPattTool_Reader/Writer.
 */
class HTTIndexPattTool_Base : public AthAlgTool
{
    public:
        HTTIndexPattTool_Base(std::string const & type, std::string const & name, IInterface const * parent);
        virtual ~HTTIndexPattTool_Base() = default;
        virtual StatusCode initialize() override;

        HTTBankMetadata const & getMetadata() const { return m_metadata; }

    protected:

        // Gaudi properties
        std::string m_filepath;

        // Other properties
        size_t m_nLayers = 0; // alias to m_HTTMapSvc->PlaneMap1stStage()->getNLogiLayers();
        std::string m_option; // ROOT file open option

        // Other objects
        ServiceHandle<ITrigHTTMappingSvc> m_HTTMapSvc;
        std::unique_ptr<TFile> m_file;
        HTTBankMetadata m_metadata;

        // Helpers
        bool checkInputs();
        void calculateMetadata(std::vector<SectorIndexData> const & indexData);
        void updateMetadata(SectorIndexData const & indexData);
};


#endif
