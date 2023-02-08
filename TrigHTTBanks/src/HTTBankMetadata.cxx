/**
 * @file HTTBankMetadata.cxx
 * @author Riley Xu - rixu@cern.ch
 * @brief See HTTBankMetadata.h
 */

#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

#include "TrigHTTBanks/HTTBankMetadata.h"


HTTBankMetadata HTTBankMetadata::readTree(TTree *t)
{
    HTTBankMetadata metadata;
    if (!t)
    {
        MsgStream msg(Athena::getMessageSvc(), "HTTBankMetadata");
        msg << MSG::ERROR << "readTree() null metadata tree" << endmsg;
        return metadata;
    }

    std::string *description = &metadata.description;
    t->SetBranchAddress("description", &description);
    t->SetBranchAddress("isValid", &metadata.isValid);
    t->SetBranchAddress("hasDC", &metadata.hasDC);

    t->SetBranchAddress("nSectors", &metadata.nSectors);
    t->SetBranchAddress("nPatterns", &metadata.nPatterns);
    t->SetBranchAddress("nPatterns_TSP", &metadata.nPatterns_TSP);
    std::vector<ULong64_t> *nPatterns_layer = &metadata.nPatterns_layer;
    t->SetBranchAddress("nPatterns_layer", &nPatterns_layer);

    t->SetBranchAddress("maxCoverage", &metadata.maxCoverage);
    t->SetBranchAddress("minCoverage", &metadata.minCoverage);

    t->GetEntry(0);
    return metadata;
}

void HTTBankMetadata::writeTree(TTree *t) const
{
    if (!t)
    {
        MsgStream msg(Athena::getMessageSvc(), "HTTBankMetadata");
        msg << MSG::ERROR << "readTree() null metadata tree" << endmsg;
        return;
    }

    // make a copy because ROOT is stupid and doesn't accept const void *
    HTTBankMetadata copy(*this);

    t->Branch("description", &copy.description);
    t->Branch("isValid", &copy.isValid);
    t->Branch("hasDC", &copy.hasDC);

    t->Branch("nSectors", &copy.nSectors);
    t->Branch("nPatterns", &copy.nPatterns);
    t->Branch("nPatterns_TSP", &copy.nPatterns_TSP);
    t->Branch("nPatterns_layer", &copy.nPatterns_layer);

    t->Branch("maxCoverage", &copy.maxCoverage);
    t->Branch("minCoverage", &copy.minCoverage);

    t->Fill();
    t->Write();
}

std::string HTTBankMetadata::toString() const
{
    return description + "\n"
        + std::to_string(isValid) + "\n"
        + std::to_string(hasDC) + "\n"
        + std::to_string(nSectors) + "\n"
        + std::to_string(nPatterns) + "\n"
        + std::to_string(nPatterns_TSP) + "\n"
        + std::to_string(maxCoverage) + "\n"
        + std::to_string(minCoverage) + "\n";
}


bool HTTBankMetadata::softEqual(HTTBankMetadata const & other) const
{
    if (other.isValid != isValid) return false;
    if (other.hasDC != hasDC) return false;
    if (other.nSectors != nSectors) return false;
    if (other.nPatterns != nPatterns) return false;
    if (hasDC)
    {
        if (other.nPatterns_TSP != nPatterns_TSP) return false;
        if (other.nPatterns_layer != nPatterns_layer) return false;
    }
    else
    {
        //if (other.maxCoverage != maxCoverage) return false;
        //if (other.minCoverage != minCoverage) return false;
    }
    return true;
}
