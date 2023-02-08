#include "TrigHTTInput/L1SimFitResult.h"
#include <iomanip>

//using namespace L1TrackSim;
std::ostream& operator<<(std::ostream& s, const FitResult& f){

  s<< std::fixed 
   <<"Fit Result: " << ((f.fitstat() == fit_status::good ) ? "Good " : "Bad ") 
   <<" curv=" << std::setprecision(3)<< std::setw(5) << f.curv()
   <<" eta="       << std::setprecision(2)<< std::setw(5) << f.eta()
   <<" phi="       << std::setprecision(2)<< std::setw(5) << f.phi()
   <<" z0="        << std::setprecision(2)<< std::setw(5) << f.z0()
   <<" d0="        << std::setprecision(2)<< std::setw(5) << f.d0()
   <<" chi2="      << std::setprecision(2)<< std::setw(7) << f.chi2ndf()
   <<" deltaR="    << std::setprecision(4)<< std::setw(5) << f.deltaR()
   <<" barcodem="  << std::setw(3) << f.barcodem()
   <<" nmuons="    << std::setw(6) << f.nmuons()
   <<" nhitlayers="<< std::setw(3) << f.nhitlayers()
   <<" passHW="    <<f.passHW()
   <<" patType="  <<f.patternType()
   <<" pattern="   << std::setw(6) << f.pattern()
   <<" setID="    << f.setID()
   <<" event="   << std::setw(5) << f.eventId()<<"\n";
  s<<"      Hits: ";
  for (auto hit: f.hitComb()) { s<<" "<<(hit);}
  s <<"\n";

  s<<"      Miss layers: ";
  for (auto lay: f.layMiss()) { s<<" "<<(lay);}
  s <<"\n";


  return s;

}
