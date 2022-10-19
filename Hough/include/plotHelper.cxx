#include <dirent.h>
#include <cstring>
#include <stdlib.h>
#include <math.h>
#include <limits>
#include "plotHelper.h"
#ifndef PlotHelper_cxx
#define PlotHelper_cxx

using namespace std;


// ====================================================
// x to d0 (Hough)
float xtod0(int x, double m_step_x, double m_d0_range) { return x*m_step_x - m_d0_range; }
// ====================================================
// y to q/pT (Hough)
float ytoqoverpt(int y, double m_step_y, double m_qOverPt_range) { return y*m_step_y - m_qOverPt_range; }

// ====================================================
// Calculate R of hits
double GetR(double x, double y){
  return sqrt(x*x+y*y);
}
// ====================================================
// Calculate eta of hits
double GetEta(double x, double y, double z){
    double r = GetR(x,y);
    double r3 = sqrt(r*r + z*z);
    double theta = acos(z/r3);
    return -log(tan(theta*0.5));
}
// ====================================================
// Calculate phi of hits
double GetPhi(double x, double y){
    return atan2(y,x);
}
// ====================================================
// Calculate eta difference between hits
double GetDeta(double x1, double y1, double z1,
               double x2, double y2, double z2){
    double eta1 = GetEta(x1,y1,z1);
    double eta2 = GetEta(x2,y2,z2);
    return eta1 - eta2;
}
// ====================================================
// Calculate phi difference between hits
double GetDphi(double x1, double y1, double x2, double y2){
    double phi1 = GetPhi(x1,y1);
    double phi2 = GetPhi(x2,y2);
    double dphi = phi2 - phi1;

    if (dphi>M_PI) dphi-=2*M_PI;
    if (dphi<-M_PI) dphi+=2*M_PI;
    return dphi;
}
// ====================================================
// Calculate Z difference between hits
double GetDz(double z1, double z2){
  return z2 -z1;
}
// ====================================================
// Calculate the slope in rphi
double Phi_slope(double x1, double y1, double x2, double y2){
  double dphi = GetDphi(x1,y1,x2,y2);
  double r1 = GetR(x1,y1);
  double r2 = GetR(x2,y2);
  double dr = r2 - r1;

  double phi_slope = -999;

  if( fabs(dr)>0 ){
      phi_slope = dphi / dr;
  }
  else{

      if( dphi>0 ) {
        phi_slope = std::numeric_limits<double>::max();
      }
      else if( dphi<0 ) {
        phi_slope = -std::numeric_limits<double>::max();
      }
      else {
        phi_slope = 0;
      }
  }

  return phi_slope;
}


void getHists(string inFileName){
  cout<< " " << inFileName << endl;
}

// Creates a directory for saving the plots
void makeDir(string outDir){
  if(!opendir(outDir.c_str())){
    std::cout << "Creating directory " << outDir.c_str() << std::endl;
    const int dir_err = mkdir(outDir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (-1 == dir_err){
      std::cout << "Error creating directory " << outDir.c_str() << std::endl;
      exit(1);
    }
  }
}
#endif
