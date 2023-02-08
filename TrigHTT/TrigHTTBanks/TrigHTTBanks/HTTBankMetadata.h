#ifndef HTTBANKMETADATA_H
#define HTTBANKMETADATA_H

/**
 * @file HTTBankMetadata.h
 * @brief Declares a struct that stores metadata about pattern banks
 * @author Riley Xu - riley.xu@cern.ch
 * @date 2019-06-13
 *
 * Declarations in this file:
 *      struct HTTBankMetadata
 */


#include <vector>
#include <string>
#include <TTree.h>


struct HTTBankMetadata
{
    std::string description;
    bool isValid = false; // this object contains valid data (i.e. not default constructed)
    bool hasDC = false; // bank contains DC patterns

    ULong64_t nSectors = 0; // total number of sectors containing patterns in this bank
    ULong64_t nPatterns = 0; // total number of patterns (i.e. unique PIDs)
    ULong64_t nPatterns_TSP = 0; // total number of effective TSP patterns.
        // This is the number of patterns that you would get if you "undid" the DC bit merging.
        // This is generally larger than the number of TSP patterns used to create a DC bank.
        // Equal to nPatterns if hasDC = false.
    std::vector<ULong64_t> nPatterns_layer; // number of PIDs per layer.
        // This is the sum of CCacheData::m_nPatterns for the ssids in each layer, which intentionally
        // double-counts DC patterns that contain multiple ssids. This gives a rough estimate of how
        // many DC bits are used. For example, if nPatterns_layer[0] ~ 2 * nPatterns, this means on
        // average each pattern in layer 0 has 1 DC bit.

    ULong64_t maxCoverage = 0; // maximum coverage pattern in this bank
    ULong64_t minCoverage = -1; // minimum coverage pattern in this bank

    static HTTBankMetadata readTree(TTree *t);
    void writeTree(TTree *t) const;

    std::string toString() const;
    inline void setNLayers(size_t n) { nPatterns_layer.resize(n); }
    bool softEqual(HTTBankMetadata const & other) const; // checks for equality in important parameters (i.e. not description)
};


#endif
