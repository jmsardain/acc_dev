// bit-by-bit description imported from file:
// https://gitlab.cern.ch/atlas-tdaq-p2-firmware/tdaq-htt-firmware/system-simulation/-/blob/master/DataFormatting/DataFormat.C
// latest September 30, 2020
// From Elliot


// v 4 April 16 2020

#include <TrigHTTByteStream/TrigHTTTPByteStreamStructure_190321.h>

// Scale factor for different use (so far same sf for track and cluster angles

  double sf_curv = 1000;
  double sf_eta = 1000;
  double sf_phi = 1000;
  double sf_z0 = 10;
  double sf_d0 = 10;
  double sf_chi2 = 1;
