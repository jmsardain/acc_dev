///////////////////////// -*- C++ -*- /////////////////////////////
// HTTTrackTest_v1.h 
// Header file for class HTTTrackTest_v1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef XAODTRIGHTT_HTTTRACKTEST_V1_H
#define XAODTRIGHTT_HTTTRACKTEST_V1_H 1

// STL includes
#include <iosfwd>

// Gaudi includes
#include "AthContainers/AuxElement.h"

// Forward declaration

namespace xAOD {

  class HTTTrackTest_v1: public SG::AuxElement
  { 
    
  public: 
    
    HTTTrackTest_v1();
    virtual ~HTTTrackTest_v1() {};
    
    //getter
   float pt() const;
    
    //setter
    void setPt(float); 
    
    
  private: 
    
  }; 
  
  /////////////////////////////////////////////////////////////////// 
  // Inline methods: 
  /////////////////////////////////////////////////////////////////// 
  //std::ostream& operator<<( std::ostream& out, const HTTTrackTest_v1& o );
  
} //> end namespace xAOD

#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::HTTTrackTest_v1, SG::AuxElement );
 

#endif //> !XAODTRIGHTT_HTTTRACKTEST_V1_H
