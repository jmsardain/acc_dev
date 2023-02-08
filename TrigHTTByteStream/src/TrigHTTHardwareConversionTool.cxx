/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
// Local includes:
#include "TrigHTTByteStream/TrigHTTHardwareConversionTool.h"

// Gaudi includes:
#include "GaudiKernel/MsgStream.h"

TrigHTTHardwareConversionTool::TrigHTTHardwareConversionTool( const std::string& type, const std::string& name,
						  const IInterface* parent )
  : AthAlgTool( type, name, parent ) {}


StatusCode TrigHTTHardwareConversionTool::initialize() {
  ATH_MSG_DEBUG("Initialising TrigHTTHardwareConversionTool");

  return StatusCode::SUCCESS;
}

StatusCode TrigHTTHardwareConversionTool::GetBinary32( float inputvalue, std::string &str_binary ) {
 
    data.input = inputvalue;
 
    std::bitset<sizeof(float) * CHAR_BIT>   bits(data.output);
 
    str_binary = bits.to_string<char, std::char_traits<char>, std::allocator<char> >(); 
    return StatusCode::SUCCESS;
}

StatusCode TrigHTTHardwareConversionTool::Binary2Hex ( std::string str_binary, std::string &str_hex ) {

  int result = 0 ;
   
  for (size_t count = 0; count < str_binary.length() ; ++count) {
    result *=2;
    result += str_binary[count]=='1'? 1 : 0;
  }  
   
  std::stringstream ss;
  ss << "0x" << std::hex << std::setw(8) << std::setfill('0')  << result;
   
  str_hex = ss.str();   
  return StatusCode::SUCCESS;
}

StatusCode TrigHTTHardwareConversionTool::GetHexFrom32 ( float inputvalue, std::string &str_hex ) {
  std::string str_binary;
  StatusCode sc = GetBinary32(inputvalue, str_binary);
  if (sc.isFailure()) return sc;
  return Binary2Hex(str_binary, str_hex);
}

