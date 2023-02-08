/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHTTBYTESTREAM_TRIGHTTTPBYTESTREAMTOOL_H
#define TRIGHTTBYTESTREAM_TRIGHTTTPBYTESTREAMTOOL_H

// Local includes
#include "TrigHTTObjects/HTTTrack.h"
#include "TrigHTTObjects/HTTCluster.h"
#include "TrigHTTObjects/HTTEventInfo.h"
#include "TrigHTTObjects/HTTHit.h"
#include "TrigHTTObjects/HTT_SSID.h"

// Athena includes:
#include "AthenaBaseComps/AthAlgTool.h"
#include "ByteStreamCnvSvcBase/FullEventAssembler.h"
#include "ByteStreamData/RawEvent.h"

// System includes
#include <vector>
#include <sstream>


/**
 *   @short Tool doing the TPResult <-> ByteStream conversion
 *
 *          The actual work of translating the information in an TPResult object
 *          into ROB fragments and vice-versa is done with this tool.
 *
 *  @author
 *  @date
 */

struct HTTModuleInfo {
  unsigned int Id; // identifier MODID
  unsigned int type; // 0x0=raw,  0x1=clustered, 0x2=clusteredwraw, 0x3=unused TYPE
  unsigned int det; // pixel=0x0, strip=0x1 DET

};

enum dataFormat {RAWPIXEL, RAWSTRIP, PIXELCLUSTER, STRIPCLUSTER, S1TRACK, S2TRACK, EXTENSION};


class TrigHTTTPByteStreamTool : public AthAlgTool {

 public:
  /// Default constructor
  TrigHTTTPByteStreamTool( const std::string& type, const std::string& name,
			   const IInterface* parent );
  /// Default destructor
  virtual ~TrigHTTTPByteStreamTool() = default;

  /// Function to initialise the tool
  virtual StatusCode initialize() override;

  /// Convert fragments to Event Headers
  StatusCode convertEventHeader( const std::vector<unsigned long int>& fragment, HTTEventInfo& event, dataFormat& dataType) ;

  /// Convert Event Header to fragment
  StatusCode convertEventHeader(  const HTTEventInfo& event, dataFormat& dataType, std::vector<unsigned long int>& fragment);

  /// Convert fragments to Event Footers
  StatusCode convertEventFooter( const std::vector<unsigned long int>& fragment, HTTEventInfo& info) ;

  /// Convert Event Footer to fragment
  StatusCode convertEventFooter( const HTTEventInfo& info, std::vector<unsigned long int>& fragment );

  /// Convert fragments to Module Headers
  StatusCode convertModuleHeader( const std::vector<unsigned long int>& fragment, HTTModuleInfo& info) ;

  /// Convert Module Header to fragment
  StatusCode convertModuleHeader( const HTTModuleInfo& info, std::vector<unsigned long int>& fragment );

  /// Convert fragments to Raw Strip Module Header
  StatusCode convertRawStripModuleHeader( const unsigned long int& fragment, HTTEventInfo& rawhead) ;

  /// Convert Raw Strips Module Header to fragment
  StatusCode convertRawStripModuleHeader( const HTTEventInfo& rawhead, unsigned long int& fragment );

  /// Convert fragments to Raw Strip Module Data
  StatusCode convertRawStripModuleData( const unsigned long int& fragment, HTTHit& raw) ;

  /// Convert Raw Strips Module Data to fragment
  StatusCode convertRawStripModuleData( const HTTHit& raw, unsigned long int& fragment );

  /// Convert fragments to Pixel Cluster Module Data
  StatusCode convertPixelClusterModuleData( const unsigned long int& fragment, HTTHit& hit) ;

  /// Convert Pixel Cluster Module Data to fragment
  StatusCode convertPixelClusterModuleData( const  HTTHit& hit, unsigned long int& fragment );

  /// Convert fragments to Pixel Cluster Footer
  StatusCode convertPixelClusterFooter( const unsigned long int& fragment, HTTEventInfo& clusterfooter) ;

  /// Convert Pixel Cluster Footer to fragment
  StatusCode convertPixelClusterFooter( const HTTEventInfo& clusterfooter, unsigned long int& fragment );

  /// Convert fragments to Strip Cluster Module Data
  StatusCode convertStripClusterModuleData( const unsigned long int& fragment, HTTHit& hit) ;

  /// Convert Strip Cluster Module Data to fragment
  StatusCode convertStripClusterModuleData( const HTTHit& hit, unsigned long int& fragment );

  /// Convert fragments to Strip Cluster Footer
  StatusCode convertStripClusterFooter( const unsigned long int& fragment, HTTEventInfo& clusterfooter) ;

  /// Convert Strip Cluster Footer to fragment
  StatusCode convertStripClusterFooter( const HTTEventInfo& clusterfooter, unsigned long int& fragment );

  /// Convert fragments to Track Block Data
  StatusCode convertTrackBlockData( const std::vector<unsigned long int>& fragment, HTTTrack& track, bool& WITHSSID) ;

  /// Convert Track Block Data to fragment
  StatusCode convertTrackBlockData( const HTTTrack& track, const bool& WITHSSID, std::vector<unsigned long int>& fragment);

  /// Function used in Track Block for creating detMap
  StatusCode createTrackDetMap( const HTTTrack& track, unsigned int& detMap); 

  /// Convert fragments to Track Pixel Cluster
  StatusCode convertTrackPixelCluster( const unsigned long int& fragment, HTTHit& cluster) ;

  /// Convert Track Pixel Cluster to fragment
  StatusCode convertTrackPixelCluster( const HTTHit& cluster, unsigned long int& fragment );

  /// Convert fragments to Track Strip Cluster
  StatusCode convertTrackStripCluster( const unsigned long int& fragment, HTTHit& cluster) ;

  /// Convert Track Strip Cluster to fragment
  StatusCode convertTrackStripCluster( const HTTHit& cluster, unsigned long int& fragment );

  //Convert SSID fragment to HTT Track SSID information 
  StatusCode convertSSIDData(const unsigned long int& fragment, HTTHit& hit, HTT_SSID& ssid);

  //Convert HTT Track SSID information to SSID fragment 
  StatusCode convertSSIDData(const HTTHit& hit, const HTT_SSID& ssid, unsigned long int& fragment);

 private:

  // ------------------------- Private types -----------------------------------
  /// Structure holding the status words and rob/rod error flags
  struct DataStatus {
    bool rob_error {false};
    bool rod_error {false};
    uint32_t status_word {0};
    uint32_t status_info {0};
  };

  // ------------------------- Private methods ---------------------------------
  /* /\** */
  /*  * @brief Helper method to extract  header/footer/payload information */



  // ------------------------- Properties --------------------------------------
  /// @name Properties holding module IDs for RoI modules
  /// @{
  /* /// EM Module IDs to decode */
  /* Gaudi::Property<std::vector<uint16_t>> m_emModuleID { */
  /*   this, "EMModuleIds", {0xa8, 0xa9, 0xaa, 0xab}, */
  /*   "Vector of module IDs of EM RoI ROBs" */
  /* }; */
  /// @}

  // ------------------------- Other private members ---------------------------

}; // class TrigHTTTPByteStreamTool



#endif // TRIGHTTBYTESTREAM_TRIGHTTTPBYTESTREAMTOOL_H
