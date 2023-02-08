#ifndef HTTPATTERNBANK_H
#define HTTPATTERNBANK_H

/*
 * HTTPatternBank.h: This file declares a class that stores patterns, optimized for
 * pattern-matching simulation.
 *
 * Declarations in this file:
 *      class HTTPatternBank
 *
 * Author: Riley Xu
 * Email: rixu@cern.ch
 * Creation Date: 2019-05-29
 */


#include "TrigHTTUtils/HTTTypes.h"
#include "TrigHTTUtils/HTTVectorMap.h"
#include "TrigHTTBanks/HTTBankMetadata.h"

#include <iostream>

/*
 * HTTPatternBank:
 *
 * A read only object that stores patterns in a highly compressed form: the
 * ccache format. This is done to conserve RAM while being efficient for
 * simulating pattern-matching, allowing very quick lookup for all patterns
 * containing a specific ssid. See below for information about the ccache
 * format.
 *
 * HTTPatternBank does not implement any write/edit functions exposed to the
 * user, to keep the class simple. Instead, it declares friend classes that
 * can create a bank or read one from a file.
 *
 * Patterns are organized by storing pattern IDs (pids), indexed in 3 levels:
 *      1. Layer
 *      2. SSID
 *      3. Sector
 *
 * DC patterns are encoded by placing the same PID into several (SSID, sector)
 * indexes for a given layer.
 *
 * There are two main ways to read the bank.
 *      1. Using indexes: This is O(1) operation and the best way to loop thorugh any level of
 *         organization. Elements are accessed with getXXX() functions. See the example loop below.
 *      2. Using ssid/sector values: This requires an O(log n) search. Using the findXXX() functions.
 *
 * Example loop:
 *      for (size_t iLayer = 0; iLayer < bank.getNLayers(); iLayer++) {
 *          for (size_t iSSID = 0; iSSID < bank.getNSSIDs(iLayer); iSSID++) {
 *              for (size_t iSector = 0; iSector < bank.getNSectors(iLayer, iSSID); iSector++) {
 *                  vector<PID> pids = getPatternIDs(iLayer, iSSID, iSector);
 *              }
 *          }
 *      }
 */
class HTTPatternBank
{
    public:
        ///////////////////////////////////////////////////////////////////////
        // Main Interface Functions
        //
        // No bounds checks on any function taking an index input!!!
        ///////////////////////////////////////////////////////////////////////

        // Get sizes
        inline size_t getNLayers() const { return m_data.size(); }
        inline size_t getNSSIDs(size_t layer) const { return m_data[layer].size(); }
        inline size_t getNSectors(size_t layer, size_t iSSID) const { return m_data[layer][iSSID].size(); }
        inline HTTBankMetadata const & getMetadata() const { return m_metadata; }

        // Access elements via a HTTPatternBank index
        inline vector<ssid_t> const & getSSIDs(size_t layer) const
            { return m_data[layer].getKeys(); }
        inline vector<sector_t> const & getSectors(size_t layer, size_t iSSID) const
            { return m_data[layer][iSSID].getKeys(); }
        inline vector<pid_t> getPatternIDs(size_t layer, size_t iSSID, size_t iSector) const
            { return decodeCCache(m_data[layer][iSSID][iSector]); }

        // Convert an index to the key value
        inline ssid_t getSSID(size_t layer, size_t iSSID) const
            { return m_data[layer].getKeys()[iSSID]; }
        inline sector_t getSector(size_t layer, size_t iSSID, size_t iSector) const
            { return m_data[layer][iSSID].getKeys()[iSector]; }

        // Convert key values to indexes
        inline size_t getISSID(size_t layer, ssid_t ssid) const
            { return m_data[layer].getKeyIndex(ssid); }
        inline size_t getISector(size_t layer, ssid_t ssid, sector_t sector) const
            { return m_data[layer][m_data[layer].getKeyIndex(ssid)].getKeyIndex(sector); }

        // Access elements via ssid/sector values
        //      Do not use the following 2 function to iterate through any level of organization.
        //      They require a O(log n) search each time. Instead, use the index methods above.
        vector<sector_t> findSectors(size_t layer, ssid_t ssid) const;
        vector<pid_t> findPatternIDs(size_t layer, ssid_t ssid, sector_t sector) const;


        ///////////////////////////////////////////////////////////////////////
        // Debug
        ///////////////////////////////////////////////////////////////////////

        long estimateRAM(bool ccache = true) const; // estimate RAM usage (bytes) in this ccache form, or if we had used naive form
        void printSSIDData(size_t layer, ssid_t ssid) const; // prints all data for a given (layer, ssid)
        std::string to_string() const; // prints the entire bank


    private:
        /*
         * Implementation note: HTTPatternBank is meant to abstract away the compressed
         * storage format. So we don't want externals to access the struct CCacheData.
         */

        ///////////////////////////////////////////////////////////////////////
        // Main Pattern Storage
        ///////////////////////////////////////////////////////////////////////
        struct CCacheData // Stores info from the ccache format, for a given (SSID, sector).
        {
            pid_t firstPID; // first pattern id
            size_t nPatterns; // number of patterns (unpacked)
            vector<uint8_t> codes; // encodes deltas of pattern ids to preceding pattern (see below).
        };
        vector<HTTVectorMap<ssid_t, HTTVectorMap<sector_t, CCacheData>>> m_data; // index by (layer, ssid, sector)

        HTTBankMetadata m_metadata;

        ///////////////////////////////////////////////////////////////////////
        // Helper Functions
        ///////////////////////////////////////////////////////////////////////
        inline void setNLayers(size_t n) { m_data.resize(n); m_metadata.setNLayers(n); }
        static CCacheData encodeCCache(vector<pid_t> pids);
        static vector<pid_t> decodeCCache(CCacheData const & data);


        ///////////////////////////////////////////////////////////////////////
        // Friends
        ///////////////////////////////////////////////////////////////////////
        friend class HTTCCachePattIO;
        friend class HTTPattGenDCAlgo;
        friend std::ostream& operator<<(std::ostream& os, const HTTPatternBank::CCacheData & c);
};




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
 *   0xE0, 0xE2..0xEF : (unused)
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


#endif // HTTPATTERNBANK_H
