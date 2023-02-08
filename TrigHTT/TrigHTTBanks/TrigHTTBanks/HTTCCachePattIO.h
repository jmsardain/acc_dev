#ifndef HTTCCACHEPATTIO_H
#define HTTCCACHEPATTIO_H

/**
 * @file HTTCCachePattIO.h
 * @brief This file declares functions that read/write pattern banks stored in the ccache format.
 * @author Riley Xu - rixu@cern.ch
 * @date May 31, 2019
 *
 * Declarations in this file:
 *      class HTTCCachePattIO
 *
 * ccache file format definition:
 *      The file shall contain, at root level, TTrees with name "Layer#", where # ranges from [0, nLayers).
 *      Each "Layer#" TTree contains the following branches:
 *              ssid/I
 *              sector/I
 *              firstPID/I
 *              nPatterns/I
 *              nCodes/I
 *              codes[nCodes]/b
 *      There is one entry for each unique (ssid, sector) combination, sorted first by ssid then sector.
 *      firstPID, nPatterns, and codes correspond to the encoded pattern IDs. See documentation in
 *      HTTPatternBank.h about the ccache encoding (also copied at the end of this document).
 *
 *      Note that these have been slightly refactored from FTKSim:
 *              firstPattern -> firstPID
 *              nPattern.numPattern -> nPatterns
 *              nData -> nCodes
 *              data[nData]/I -> codes[nCodes]/b
 *      Besides better names, the conversion from int32 data array to uint8 codes array is more accurate
 *      and saves 20% filesize for that branch.
 *
 *      The file may contain TTrees with names "BadModules" and "Metadata". TODO
 */

#include "AthenaBaseComps/AthService.h"

#include "TrigHTTBanks/HTTPatternBank.h"


/*
 * HTTCCachePattIO:
 *
 * This class reads/writes a pattern bank stored in a root file, in the ccache format,
 * to a HTTPatternBank. Will calculate necessary meta data on read and do a consistency check with
 * the tree Metadata.
 *
 * Workflow:
 *      // Writing
 *      ccio.write(filepath, bank);
 *
 *      // Reading
 *      HTTPatternBank bank = ccio.read(filepath);
 */
class HTTCCachePattIO : public AthService
{
    public:
        ///////////////////////////////////////////////////////////////////////
        // AthService

        HTTCCachePattIO(const std::string & name, ISvcLocator *svc) : AthService(name, svc) {}
        virtual ~HTTCCachePattIO() = default;

        static const InterfaceID & interfaceID();
        virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIf);

        virtual StatusCode initialize() { return StatusCode::SUCCESS; }
        virtual StatusCode finalize() { return StatusCode::SUCCESS; }

        ///////////////////////////////////////////////////////////////////////
        // Interface

        // @param legacy set true to read FTKSim-style ccache files
        HTTPatternBank read(std::string filepath, bool legacy = false) const;

        // @return 0 on success, -1 on error
        int write(std::string filepath, HTTPatternBank const & bank, bool overwrite=false) const;

    private:

        // Private helper class to handle metadata while reading. Would prefer not to
        // make an exposed reader class to simplify interface. Also, after loading the pattern
        // bank into memory, there is no reason to keep around a reader class.
        class ReaderHelper;

        std::string verifyPath(std::string filepath) const;
};


inline const InterfaceID & HTTCCachePattIO::interfaceID()
{
    static const InterfaceID IID("HTTCCachePattIO", 1, 0);
    return IID;
}


/*
 * ccache: Encodes pattern ids for a given (layer,SSID,sector). The ids are packed
 * by sorting from smallest to largest, and encoding, for each id, the difference (delta)
 * from its preceding id. The smallest id is stored as the starting point.
 *
 * A DC pattern may be in multiple (layer, ssid, sector) entries due to DC bits.
 *
 * Code key: (bits counting with 0 = LSB)
 *   0x00-0x7f : delta-1, i.e. delta=1..128
 *   0x8x : store bit  7..10 if delta-1>0x0000007f
 *   0x9x : store bit 11..14 if delta-1>0x000007ff
 *    ...
 *   0xDx : store bit 27..30 if delta-1>0x07ffffff
 *   0xE1 : store bit 31     if delta-1>0x7fffffff
 *   0xE4..0xE7 : store length (1..4 byte, big-endian) (unused)
 *   0xE8..0xEB : store nPatterns (1..4 byte, big-endian) (unused)
 *   0xEC..0xEF : store firstPID address relative to sector start (1..4 byte, big-endian) (unused)
 *   0xFx : encode repeated occurance of delta=1 (repeat=2..17)
 *
 * Example:
 *      CCacheData = {
 *          m_firstPID = 0;
 *          m_nPatterns = 4; (this is redundant)
 *          m_codes = [0x0a, 0x81, 0x42, 0x82, 0x15]
 *      }              ____  __________  __________
 *
 *      This corresponds to deltas [0x0b, 0xC3, 0x116].
 *          0x0a corresponds to row 0 above, so just add 1 to get the delta.
 *          0x81 indicates the 7-10th bits are 0x1 => read next entry: the first 7 bits are 0x42 ==> delta-1 = 000 1100 0010
 *          0x82 indicates the 7-10th bits are 0x2 => read next entry: the first 7 bits are 0x15 ==> delta-1 = 001 0001 1001
 *      and pattern ids [0, 11, 206, 484]
 */

#endif // HTTCCACHEFILEIO_H

