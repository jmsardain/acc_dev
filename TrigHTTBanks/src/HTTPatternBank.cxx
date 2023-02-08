/*
 * HTTPatternBank.cxx: See HTTPatternBank.h
 *
 * Author: Riley Xu
 * Email: rixu@cern.ch
 * Creation Date: 2019-05-29
 */

#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

#include "TrigHTTBanks/HTTPatternBank.h"
#include "TrigHTTUtils/HTTFunctions.h"

#include <sstream>

vector<sector_t> HTTPatternBank::findSectors(size_t layer, ssid_t ssid) const
{
    auto it = m_data[layer].find(ssid);
    if (it == m_data[layer].end()) return vector<sector_t>();
    else return it->getKeys();
}


vector<pid_t> HTTPatternBank::findPatternIDs(size_t layer, ssid_t ssid, sector_t sector) const
{
    auto it_ssid = m_data[layer].find(ssid);
    if (it_ssid == m_data[layer].end()) return vector<pid_t>();
    else
    {
        size_t iSSID = std::distance(m_data[layer].begin(), it_ssid);
        auto it_sector = m_data[layer][iSSID].find(sector);
        if (it_sector == m_data[layer][iSSID].end()) return vector<pid_t>();
        else return decodeCCache(*it_sector);
    }
}


// See HTTPatterBank.h for information about the ccache format
vector<pid_t> HTTPatternBank::decodeCCache(HTTPatternBank::CCacheData const & data)
{
    vector<pid_t> pids;
    pids.reserve(data.nPatterns);

    pids.push_back(data.firstPID);
    uint32_t delta_m1 = 0; // delta-1
    for (uint8_t code : data.codes)
    {
        if (code <= 0x7f) {
            delta_m1 |= code;
            pids.push_back(pids.back() + delta_m1 + 1);
            delta_m1 = 0;
        } else if (code <= 0x8F) { // 0x8X
            delta_m1 |= (code & 0xF) << 7;
        } else if (code <= 0x9F) { // 0x9X
            delta_m1 |= (code & 0xF) << 11;
        } else if (code <= 0xAF) { // 0xAx
            delta_m1 |= (code & 0xF) << 15;
        } else if (code <= 0xBF) { // 0xBx
            delta_m1 |= (code & 0xF) << 19;
        } else if (code <= 0xCF) { // 0xCx
            delta_m1 |= (code & 0xF) << 23;
        } else if (code <= 0xDF) { // 0xDx
            delta_m1 |= (code & 0xF) << 27;
        } else if (code == 0xE1) {
            delta_m1 |= 1 << 31;
        } else if (code >= 0xF0) {
            // assert delta_m1 == 0
            unsigned nRepeats = (code & 0xF) + 2;
            for (unsigned i = 0; i < nRepeats; i++)
                pids.push_back(pids.back() + 1);
        } else {
            MsgStream msg(Athena::getMessageSvc(), "HTTPatternBank::decodeCCache()");
            msg << MSG::WARNING << "encountered unused code " << formatHex(code) << endmsg;
        }
    }

    if (pids.size() != data.nPatterns)
    {
        MsgStream msg(Athena::getMessageSvc(), "HTTPatternBank::decodeCCache()");
        msg << MSG::WARNING << "number of patterns mismatch:\n"
            << "\tExpected " << data.nPatterns << ", decoded " << pids.size() << endmsg;
    }

    return pids;
}


// Encode repeated occurences of delta = 1
void encodeDelta1(vector<uint8_t> & codes, size_t nDelta1)
{
    if (nDelta1 == 0)
    {
        return;
    }
    else if (nDelta1 == 1)
    {
        codes.push_back(0x00);
    }
    else if (nDelta1 > 17)
    {
        codes.push_back(0xFF);
        encodeDelta1(codes, nDelta1 - 17);
    }
    else
    {
        codes.push_back(0xF0 | (nDelta1 - 2));
    }
}


inline uint32_t fillBits(uint32_t n)
{
    return (1 << n) - 1;
}


// Encode a single delta - 1 > 1
void encodeDeltaM1(vector<uint8_t> & codes, uint32_t deltaM1)
{
    uint8_t key = 0xE0;
    int nBits = 31;
    while (nBits >= 7)
    {
        if (deltaM1 > fillBits(nBits))
            codes.push_back(key | ((deltaM1 >> nBits) & 0xf));
        key -= 0x10;
        nBits -= 4;
    }
    codes.push_back(deltaM1 & 0x7f);
}


// See HTTPatterBank.h for information about the ccache format
HTTPatternBank::CCacheData HTTPatternBank::encodeCCache(vector<pid_t> pids)
{
    CCacheData data;
    if (pids.empty()) return data;

    std::sort(pids.begin(), pids.end());
    data.firstPID = pids[0];
    data.nPatterns = pids.size();

    size_t nDelta1 = 0;
    for (size_t i = 1; i < pids.size(); i++)
    {
        pid_t delta = pids[i] - pids[i-1];
        if (delta == 1)
        {
            nDelta1++;
        }
        else
        {
            if (nDelta1 > 0)
            {
                encodeDelta1(data.codes, nDelta1);
                nDelta1 = 0;
            }
            encodeDeltaM1(data.codes, delta - 1);
        }
    }
    if (nDelta1 > 0) encodeDelta1(data.codes, nDelta1);

    return data;
}



///////////////////////////////////////////////////////////////////////////////
// Debug
///////////////////////////////////////////////////////////////////////////////


long HTTPatternBank::estimateRAM(bool ccache) const
{
    if (ccache)
    {
        uint32_t nData = 0; // the number of stored struct CCacheData objects
        uint32_t nKeys = 0; // the number of ssid or sector values
        for (size_t iLayer = 0; iLayer < getNLayers(); iLayer++)
        {
            nKeys += m_data[iLayer].size();
            for (size_t iSSID = 0; iSSID < getNSSIDs(iLayer); iSSID++)
            {
                nData += m_data[iLayer][iSSID].size();
                nKeys += m_data[iLayer][iSSID].size();
            }
        }
        return nData * sizeof(CCacheData) + nKeys * sizeof(ssid_t);
    }
    else
    {
        return m_metadata.nPatterns * sizeof(uint32_t) * getNLayers(); // assume store one 32bit ssid per layer per pattern
    }
}


std::ostream& operator<<(std::ostream& os, const HTTPatternBank::CCacheData & ccache)
{
    os << "1st: " << ccache.firstPID << ", N: " << ccache.nPatterns << ", codes:";

    std::ios_base::fmtflags flags((std::ios_base::fmtflags)os.flags());
    os << std::showbase << std::hex << std::right << std::setw(4);
    for (uint8_t code : ccache.codes) os << " " << code;
    os.flags(flags);

    return os;
}


void HTTPatternBank::printSSIDData(size_t layer, ssid_t ssid) const
{
    MsgStream msg(Athena::getMessageSvc(), "HTTPatternBank");
    msg << MSG::INFO << "\nEncoded and decoded pattern ID data for layer " << layer << ", ssid " << ssid <<":\n";
    size_t iSSID = getISSID(layer, ssid);
    if (iSSID == getNSSIDs(layer)) // SSID not in bank
    {
        msg << "The SSID = " << ssid << " in layer = " << layer << " are not in the pattern bank" << "\n";
        return; 
    }

    for (size_t iSector = 0; iSector < getNSectors(layer, iSSID); iSector++)
    {
        sector_t sector = getSector(layer, iSSID, iSector);
        CCacheData ccache = m_data[layer][iSSID].getData()[iSector];
        vector<pid_t> pids = getPatternIDs(layer, iSSID, iSector);

        msg << "Sector " << sector << ":\n";
        msg << "\t1st: " << ccache.firstPID << ", N: " << ccache.nPatterns << ", codes:";

        std::ios_base::fmtflags flags((std::ios_base::fmtflags)msg.flags());
        msg << std::showbase << std::hex << std::internal << std::setfill('0') << std::setw(4);
        for (uint8_t code : ccache.codes) msg << " " << code;
        msg.flags(flags);

        msg << "\n\tDecoded PIDs:";
        for (pid_t pid : pids) msg << " " << pid;
        msg << "\n";
    }
    msg << endmsg;
}


std::string HTTPatternBank::to_string() const
{
    std::stringstream s;
    s << "Pattern Bank:\n";
    for (size_t layer = 0; layer < m_data.size(); layer++)
    {
        s << "\tLayer " << layer << "\n";
        auto ssids = m_data[layer].getKeys();
        auto sector_maps = m_data[layer].getData();
        for (size_t issid = 0; issid < ssids.size(); issid++)
        {
            s << "\t\tssid " << ssids[issid] << "\n";
            auto sectors = sector_maps[issid].getKeys();
            auto cdata = sector_maps[issid].getData();
            for (size_t isector = 0; isector < sectors.size(); isector++)
                s << "\t\t\tsector: " << sectors[isector] << ", " << cdata[isector] << "\n";
        }
    }

    return s.str();
}
