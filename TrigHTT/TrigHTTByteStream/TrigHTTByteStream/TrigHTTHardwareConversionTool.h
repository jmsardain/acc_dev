/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHTTBYTESTREAM_TRIGHTTHARDWARECONVERSIONTOOL_H
#define TRIGHTTBYTESTREAM_TRIGHTTHARDWARECONVERSIONTOOL_H

// Athena includes:
#include "AthenaBaseComps/AthAlgTool.h"

#include <vector>
#include <limits.h>
#include <iostream>
#include <sstream>
#include <math.h>
#include <bitset>
#include <iomanip>      // std::setfill, std::setw


/**
 *   @short Tool to help with conversion to hardware formats (converting to int, etc)
 *  @author
 *  @date
 */


class TrigHTTHardwareConversionTool : public AthAlgTool {

 public:
  /// Default constructor
  TrigHTTHardwareConversionTool( const std::string& type, const std::string& name,
			   const IInterface* parent );
  /// Default destructor
  virtual ~TrigHTTHardwareConversionTool() = default;

  /// Function to initialise the tool
  virtual StatusCode initialize() override;

  /// Get 32-bit IEEE 754 format of the decimal value
  StatusCode GetBinary32( float inputvalue, std::string &str_binary );

  /// Convert the 32-bit binary encoding into hexadecimal
  StatusCode Binary2Hex ( std::string str_binary, std::string &str_hex );    

  /// Convert the 32-bit float encoding into hexadecimal
  StatusCode GetHexFrom32 ( float inputvalue, std::string &str_hex );

 private:
    union
    {
      float input;   // assumes sizeof(float) == sizeof(int)
      int   output;
    }    data; //used in GetBinary32


  // ------------------------- Other private members ---------------------------

}; // class TrigHTTHardwareConversionTool



#endif // TRIGHTTBYTESTREAM_TRIGHTTHARDWARECONVERSIONTOOL_H
