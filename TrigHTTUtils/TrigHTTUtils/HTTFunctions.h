/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/*
 * HTTFunctions.h
 *
 * Declares several generic utility functions.
 */

#ifndef HTTFUNCTIONS_H
#define HTTFUNCTIONS_H

#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

#include <TString.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>

#include "HTTMacros.h"
#include "HTTTypes.h"

#include <cmath>
#include <vector>
#include <sstream>



/*****************************************************************************/
/* ROOT Error Checking                                                       */
/*****************************************************************************/

// Checks whether a TTreeReader is properly set
inline bool checkTTR(TTreeReader const & reader)
{
    if (reader.GetEntryStatus() > TTreeReader::kEntryNotLoaded)
    {
        MsgStream msg(Athena::getMessageSvc(), "checkTTR()");
        msg << MSG::ERROR << "Error status " << reader.GetEntryStatus()
            << " setting up reader for " << reader.GetName() << endmsg;
        return false;
    }
    return true;
}


// Checks whether a TTreeReaderValue/Array is properly set
// Usage: checkTTRV(val1, val2, ...);

inline bool checkTTRV() { return true; } // Base case for recusion below

template <typename ... Targs>
bool checkTTRV(ROOT::Internal::TTreeReaderValueBase const & value, Targs const & ... Fargs) // recursive variadic function
{
    if (value.GetSetupStatus() < 0)
    {
        MsgStream msg(Athena::getMessageSvc(), "checkTTRV()");
        msg << MSG::ERROR << "Error status " << value.GetSetupStatus()
            << " setting up reader for " << value.GetBranchName() << endmsg;
        return false;
    }
    return checkTTRV(Fargs...);
}



/*****************************************************************************/
/* Bit Manipulations and Arithmetic
 *
 * Avoid using templates because right-shifts will not work correctly if
 * accidentally called using a signed type.
 */
/*****************************************************************************/

// Returns a list of words, each with one bit set for each bit set in x
// Sorted by LSB to MSB.
inline std::vector<uint32_t> splitSetBits32(uint32_t x)
{
    std::vector<uint32_t> out;
    uint32_t setbit = 0x1;
    while (setbit != 0 && x != 0)
    {
        if (x & 1) out.push_back(setbit);
        x = x >> 1;
        setbit = setbit << 1;
    }
    return out;
}


// Returns the first set bit of x, starting from the LSB (0 indexed).
// Returns -1 if x == 0.
inline int firstSetBit32(uint32_t x)
{
    unsigned bit = 0;
    while (x)
    {
        if (x & 1) return bit;
        x >>= 1;
        bit++;
    }
    return -1;
}


// Divide x/y, rounding up. x,y can not be 0.
inline unsigned divUp(unsigned x, unsigned y)
{
    // return (x + y - 1) / y; runs risk of overflow
    return 1 + ((x - 1) / y);
}


// Gray code: https://en.wikipedia.org/wiki/Gray_code#Converting_to_and_from_Gray_code
inline uint32_t binaryToGray(uint32_t num)
{
    return num ^ (num >> 1);
}


inline uint32_t grayToBinary32(uint32_t num)
{
    num = num ^ (num >> 16);
    num = num ^ (num >> 8);
    num = num ^ (num >> 4);
    num = num ^ (num >> 2);
    num = num ^ (num >> 1);
    return num;
}


// Calculate euclidean norm
inline double eucnorm(double x, double y, double z)
{
    return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
}


// Calculate opening angle between two vectors in euclidean space
inline double eucangle(double x, double y, double z, double u, double v, double w)
{
    return acos((x * u + y * v + z * w) / (eucnorm(x, y, z) * eucnorm(u, v, w)));
}



/*****************************************************************************/
/* String Manipulations                                                      */
/*****************************************************************************/

template <typename T>
inline std::string formatHex(T x)
{
    std::stringstream ss;
    ss << std::showbase << std::hex << x;
    return ss.str();
}


inline std::string to_string(pattern_t const & patt)
{
    std::string out;
    for (size_t i = 0; i < patt.size(); i++)
    {
        if (i > 0) out += ",";
        out += std::to_string(patt[i]);
    }
    return out;
}



/*****************************************************************************/
/* Vectors and Indexing                                                      */
/*****************************************************************************/

/**
 * Given a vector of sizes (of arrays), generates a vector of all combinations of indices to
 * index one element from each array.
 *
 * For example, given [2 3], generates [(0 0) (1 0) (0 1) (1 1) (0 2) (1 2)].
 */
std::vector<std::vector<int>> getComboIndices(std::vector<size_t> const & sizes);



/*****************************************************************************/
/* RMS95 Value Calculation                                                   */
/*****************************************************************************/

/**
 * This function is used to calculate RMS95 value for 1D histograms.
 * It was ported from 
 * https://gitlab.cern.ch:8443/fpastore/l1tracksim/-/blob/master/PatRec/macros/FraMacros/Functions.cxx#L253
 * See more details there.
 */
double rms95(TH1 const * h);



#endif // HTTFUNCTIONS_H
