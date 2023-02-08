/**
 * @file HTTCCachePattIO.cxx
 *
 * @brief See HTTCCachePattIO.h
 *
 * @author Riley Xu - rixu@cern.ch
 *
 * Creation Date: 2019-05-31
 */

#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "TrigHTTBanks/HTTCCachePattIO.h"
#include "TrigHTTBanks/HTTBankMetadata.h"
#include "TrigHTTUtils/HTTFunctions.h"

#include <TFile.h>
#include <TString.h>
#include <TRegexp.h>
#include <TTree.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>

#include <set>
#include <boost/filesystem.hpp>


StatusCode HTTCCachePattIO::queryInterface(const InterfaceID& riid, void** ppvIf)
{
    if (interfaceID() == riid)
    {
        *ppvIf = dynamic_cast<HTTCCachePattIO*>(this);
        addRef();
        return StatusCode::SUCCESS;
    }
    else
    {
        return AthService::queryInterface(riid, ppvIf);
    }

}

/*****************************************************************************/
/* Read Functions and Helpers                                                */
/*****************************************************************************/

class HTTCCachePattIO::ReaderHelper
{
    public:
        ReaderHelper(MsgStream & msg, HTTPatternBank & bank, bool legacy)
            : m_msg(msg), m_bank(bank), m_legacy(legacy) {}

        MsgStream & msg() const { return m_msg; }
        MsgStream & msg(const MSG::Level lvl) const { return msg() << lvl; }
        bool msgLvl(const MSG::Level lvl) const
        {
            if (m_msg.level() <= lvl) {
                m_msg << lvl;
                return true;
            } else {
                return false;
            }
        }

        void read(std::string filepath);

    private:

        // Main members
        MsgStream & m_msg;
        HTTPatternBank & m_bank;
        bool m_legacy;

        // Meta data intermediates
        std::set<sector_t> m_sectors;
        vector<vector<size_t>> m_nTSP;
            // keep track of number of TSP patterns per DC pattern
            // index by (layer, PID)

        // Helpers
        void readLayer(TTree* tree, size_t layer);
        void updateMetadata(HTTBankMetadata const & fileMetadata);
        void updateNTSP(size_t layer, HTTPatternBank::CCacheData const & data);

        static HTTPatternBank::CCacheData makeCCacheData(Int_t firstPID, Int_t nPatterns, TTreeReaderArray<UChar_t> & codes);
        static HTTPatternBank::CCacheData makeCCacheData_legacy(Int_t firstPID, Int_t nPatterns, TTreeReaderArray<Int_t> & codes);
};


HTTPatternBank HTTCCachePattIO::read(std::string filepath, bool legacy) const
{
    HTTPatternBank bank;
    ReaderHelper reader(msg(), bank, legacy);
    reader.read(filepath);
    return bank;
}


void HTTCCachePattIO::ReaderHelper::read(std::string filepath)
{
    TFile *tfile = TFile::Open(filepath.c_str(), "READ");
    if (!tfile || tfile->IsZombie()) ATH_MSG_FATAL("unable to open file " << filepath);

    ATH_MSG_INFO("reading " << filepath);

    size_t nLayersFound = 0;
    HTTBankMetadata file_metadata;

    // Loop over keys in the file
    for (const auto&& curr_obj : *tfile->GetListOfKeys())
    {
        TString name(curr_obj->GetName());
        if (name.BeginsWith("Layer"))
        {
            int layer = TString(name(TRegexp("[0-9]+"))).Atoi();
            if (layer < 0)
            {
                ATH_MSG_WARNING("encountered negative layer in object " << name.Data());
                continue;
            }

            // m_bank is populated here
            readLayer(dynamic_cast<TTree*>(tfile->Get(name)), layer);
            nLayersFound++;
        }
        else if (name.EqualTo("Metadata"))
        {
            TTree *metadataTree = dynamic_cast<TTree*>(tfile->Get(name));
            file_metadata = HTTBankMetadata::readTree(metadataTree);
        }
        else
        {
            ATH_MSG_WARNING("unimplemented/unrecognized object " << name.Data());
        }
    }

    if (m_bank.getNLayers() != nLayersFound)
        ATH_MSG_WARNING("read() only found " << nLayersFound
            << "layers but expected " << m_bank.getNLayers());

    updateMetadata(file_metadata);

    ATH_MSG_INFO("read " << m_bank.getMetadata().nPatterns << " patterns; "
        << m_bank.getMetadata().nPatterns_TSP << " TSP patterns");
    delete tfile;
}


// Populates m_bank.m_data[layer] and m_bank.m_metadata.nPatterns_layer[layer]
void HTTCCachePattIO::ReaderHelper::readLayer(TTree* tree, size_t layer)
{
    if (!tree) // can happen if the dynamic_cast fails
    {
        ATH_MSG_FATAL("readLayer() received a null pointer");
        return;
    }
    if (layer >= m_bank.getNLayers()) m_bank.setNLayers(layer + 1);

    // setup ttree readers
    TTreeReader reader(tree);
    if (!checkTTR(reader)) ATH_MSG_FATAL("readLayer() data reader init failed");
    TTreeReaderValue<Int_t> ssid(reader, "ssid");
    TTreeReaderValue<Int_t> sector(reader, "sector");
    TTreeReaderValue<Int_t> firstPID(reader, (m_legacy) ? "firstPattern" : "firstPID");
    TTreeReaderValue<Int_t> nPatterns(reader, (m_legacy) ? "nPattern.numPattern" : "nPatterns");

    // unfortunately messy pointer handling to deal with legacy type
    std::unique_ptr<TTreeReaderArray<UChar_t>> codes;
    std::unique_ptr<TTreeReaderArray<Int_t>> codes_legacy;
    if (m_legacy) codes_legacy = std::unique_ptr<TTreeReaderArray<Int_t>>(new TTreeReaderArray<Int_t>(reader, "data"));
    else codes = std::unique_ptr<TTreeReaderArray<UChar_t>>(new TTreeReaderArray<UChar_t>(reader, "codes"));

    // temp containers to populate m_bank
    vector<ssid_t> ssids; // keys for outer HTTVectorMap
    vector<HTTVectorMap<sector_t, HTTPatternBank::CCacheData>> ssid_data; // values for outer HTTVectorMap
    vector<sector_t> sectors; // keys for inner HTTVectorMap
    vector<HTTPatternBank::CCacheData> sector_data; // values for inner HTTVectorMap

    // read the entries
    size_t nPatternTotal = 0;
    Int_t curr_ssid;
    int i = 0; // branch for first read
    while (reader.Next())
    {
        if (!checkTTRV(ssid, sector, firstPID, nPatterns))
            ATH_MSG_FATAL("readLayer() TTree reader values failed");

        if (i == 0)
        {
            curr_ssid = *ssid;
            i++;
        }
        else if (curr_ssid != *ssid) // new ssid = new outer HTTVectorMap entry
        {
            ssids.push_back(curr_ssid);
            ssid_data.push_back(HTTVectorMap<sector_t, HTTPatternBank::CCacheData>(std::move(sectors), std::move(sector_data)));
            sectors.clear(); // redundant since moved but just in case
            sector_data.clear();
            curr_ssid = *ssid;
        }

        nPatternTotal += *nPatterns;
        HTTPatternBank::CCacheData data =
            (m_legacy) ? makeCCacheData_legacy(*firstPID, *nPatterns, *codes_legacy) :
                         makeCCacheData(*firstPID, *nPatterns, *codes);
        updateNTSP(layer, data);

        sectors.push_back(*sector);
        m_sectors.insert(*sector);
        sector_data.push_back(data);
    }

    // add the final ssid entry, since it won't be caught by the else if statement above
    if (!sectors.empty())
    {
        ssids.push_back(curr_ssid);
        ssid_data.push_back(HTTVectorMap<sector_t, HTTPatternBank::CCacheData>(std::move(sectors), std::move(sector_data)));
    }

    // update m_bank and meta data
    m_bank.m_data[layer].assign(std::move(ssids), std::move(ssid_data));
    m_bank.m_metadata.nPatterns_layer[layer] = nPatternTotal;

    ATH_MSG_DEBUG("read " << nPatternTotal << " patterns from layer " << layer);
}


// If fileMetadata is valid, do consistency check. Meta data calculated by this class will take
// priority, but will output warning messages.
void HTTCCachePattIO::ReaderHelper::updateMetadata(HTTBankMetadata const & fileMetadata)
{
    m_bank.m_metadata.isValid = true;
    m_bank.m_metadata.nSectors = m_sectors.size();

    // multiply entries of m_nTSP, which is by layer, together to get nTSP total
    vector<size_t> nTSP;
    for (size_t layer = 0; layer < m_nTSP.size(); layer++)
    {
        if (nTSP.size() < m_nTSP[layer].size()) nTSP.resize(m_nTSP[layer].size());
        for (size_t pid = 0; pid < m_nTSP[layer].size(); pid++)
        {
            if (m_nTSP[layer][pid] != 0 && nTSP[pid] == 0) nTSP[pid] = 1;
            nTSP[pid] *= m_nTSP[layer][pid];
        }
    }

    m_bank.m_metadata.nPatterns = nTSP.size();
    for (size_t pid = 0; pid < nTSP.size(); pid++)
    {
        if (nTSP[pid] > 1)
        {
            m_bank.m_metadata.hasDC = true;
            break;
        }
    }
    for (size_t pid = 0; pid < nTSP.size(); pid++)
    {
        if (nTSP[pid] == 0)
            ATH_MSG_WARNING("pattern " << pid << " had no ssid entry in some layer");
        else m_bank.m_metadata.nPatterns_TSP += nTSP[pid];
    }

    if (fileMetadata.isValid && !m_bank.m_metadata.softEqual(fileMetadata))
        ATH_MSG_WARNING("inconsistent metadata with file:\n"
            << "Calculated:\n" << m_bank.m_metadata.toString()
            << "File:\n" << fileMetadata.toString());
}


// Simple wrapper to create HTTPatternBank::CCacheData from TTree reads
HTTPatternBank::CCacheData HTTCCachePattIO::ReaderHelper::makeCCacheData(
        Int_t firstPID, Int_t nPatterns, TTreeReaderArray<UChar_t> & codes)
{
    HTTPatternBank::CCacheData ccacheData;

    ccacheData.firstPID = firstPID;
    ccacheData.nPatterns = nPatterns;
    ccacheData.codes.resize(codes.GetSize());
    for (size_t i = 0; i < codes.GetSize(); i++)
        ccacheData.codes[i] = codes[i];

    return ccacheData;
}


// Simple wrapper to create HTTPatternBank::CCacheData from TTree reads, legacy format
HTTPatternBank::CCacheData HTTCCachePattIO::ReaderHelper::makeCCacheData_legacy(
        Int_t firstPID, Int_t nPatterns, TTreeReaderArray<Int_t> & codes)
{
    HTTPatternBank::CCacheData ccacheData;

    ccacheData.firstPID = firstPID;
    ccacheData.nPatterns = nPatterns;
    ccacheData.codes.resize(codes.GetSize());
    for (size_t i = 0; i < codes.GetSize(); i++)
        ccacheData.codes[i] = static_cast<UChar_t>(static_cast<UInt_t>(codes[i]));

    return ccacheData;
}


// Updates the number of TSP patterns (per pid) in a layer with patterns from a new CCacheData entry
void HTTCCachePattIO::ReaderHelper::updateNTSP(size_t layer, HTTPatternBank::CCacheData const & data)
{
    if (layer >= m_nTSP.size()) m_nTSP.resize(layer + 1);
    vector<pid_t> pids = HTTPatternBank::decodeCCache(data);
    if ((size_t)pids.back() >= m_nTSP[layer].size()) m_nTSP[layer].resize(pids.back() + 1);
        // PIDs are in increasing order. So only need to check back() to resize nTSP_layer
    for (pid_t pid : pids) m_nTSP[layer][pid]++;
}



/*****************************************************************************/
/* Write Functions and Helpers                                               */
/*****************************************************************************/


// Checks if file exists already. If it does, appends ".v#" to the file name.
std::string HTTCCachePattIO::verifyPath(std::string filepath) const
{
    boost::filesystem::path path(filepath);
    if (!exists(path)) return filepath;

    int n = 2;
    std::string prefix = (path.parent_path() / path.stem()).string();
    std::string extension = path.extension().string();
    do {
        path = prefix + ".v" + std::to_string(n) + extension;
        n++;
    } while (exists(path));

    ATH_MSG_WARNING("write() file " << filepath << " already exists; writing instead to " << path);

    return path.string();
}


int HTTCCachePattIO::write(std::string filepath, HTTPatternBank const & bank, bool overwrite) const
{
    std::string finalpath = (overwrite) ? filepath : verifyPath(filepath);
    std::string writeMode = (overwrite) ? "RECREATE" : "CREATE";
    TFile *outfile = TFile::Open(finalpath.c_str(), writeMode.c_str());
    if (outfile->IsZombie())
    {
        ATH_MSG_WARNING("write() couldn't open file " << filepath);
        return -1;
    }

    Int_t ssid, sector, firstPID, nPatterns, nCodes;
    UChar_t dummy[1] = {}; // ROOT requires a valid object when creating branch

    for (size_t layer = 0; layer < bank.getNLayers(); layer++)
    {
        TTree *tout = new TTree(("Layer" + std::to_string(layer)).c_str(), "HTTSim ccache pattern bank layer data");
        tout->Branch("ssid", &ssid);
        tout->Branch("sector", &sector);
        tout->Branch("firstPID", &firstPID);
        tout->Branch("nPatterns", &nPatterns);
        tout->Branch("nCodes", &nCodes);
        tout->Branch("codes", dummy, "codes[nCodes]/b"); // address to be set below

        for (size_t iSSID = 0; iSSID < bank.getNSSIDs(layer); iSSID++)
        {
            for (size_t iSector = 0; iSector < bank.getNSectors(layer, iSSID); iSector++)
            {
                ssid = bank.getSSID(layer, iSSID);
                sector = bank.getSector(layer, iSSID, iSector);
                HTTPatternBank::CCacheData cdata = bank.m_data[layer][iSSID][iSector];

                firstPID = cdata.firstPID;
                nPatterns = (Int_t)cdata.nPatterns;
                nCodes = (Int_t)cdata.codes.size();
                tout->SetBranchAddress("codes", cdata.codes.data());

                tout->Fill();
            }
        }
    }

    TTree *tmeta = new TTree("Metadata", "Pattern bank metadata");
    bank.m_metadata.writeTree(tmeta);
    outfile->Write();
    ATH_MSG_INFO("write() wrote to " << filepath);

    delete outfile;
    return 0;
}


