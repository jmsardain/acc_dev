#include <cmath>

double L1TTsigma_ript_vs_pt(double pt) { double ilogpt = 1/std::log(std::fabs(pt)); return -1.49e+09*(-7.37e-10  + ilogpt*(3.23e-10 + ilogpt*( -3.21e-10 ) ) ); }
double L1TTsigma_rphi_vs_pt(double pt) { double ilogpt = 1/std::log(std::fabs(pt)); return -9.58e+08*(-5.08e-10  + ilogpt*(3.44e-10 + ilogpt*( -1.51e-09 ) ) ); }
double L1TTsigma_reta_vs_pt(double pt) { double ilogpt = 1/std::log(std::fabs(pt)); return -8.27e+08*(-7.60e-10  + ilogpt*(-2.09e-10 + ilogpt*( -5.73e-10 ) ) ); }
double L1TTsigma_rd0_vs_pt(double pt) { double ilogpt = 1/std::log(std::fabs(pt)); return -3.49e+07*(-6.27e-09  + ilogpt*(-1.59e-08 + ilogpt*( -2.10e-08 ) ) ); }
double L1TTsigma_rzed_vs_pt(double pt) { double ilogpt = 1/std::log(std::fabs(pt)); return -2.38e+07*(-3.93e-08  + ilogpt*(1.42e-09 + ilogpt*( -7.09e-09 ) ) ); }

double  L1TTsigma_ript_vs_eta(double eta) { return 1.67e-03  + eta*eta*( -3.80e-05  + eta*eta*(  3.75e-04 + eta*eta*(2.28e-05 + eta*eta*( -8.27e-06 + eta*eta*( 1.86e-06 ) ) ) ) ); }
double   L1TTmean_ript_vs_eta(double eta) { return -2.17e-04 + eta*( 2.97e-11 + eta*( -8.65e-05 + eta*( 3.86e-11 + eta*( 3.58e-05 ) ) ) ); }
double  L1TTsigma_rphi_vs_eta(double eta) { return 8.36e-04  + eta*eta*( 8.97e-05  + eta*eta*(  6.88e-05 + eta*eta*(6.87e-06 + eta*eta*( -8.73e-06 + eta*eta*( 1.89e-06 ) ) ) ) ); }
double   L1TTmean_rphi_vs_eta(double eta) { return -9.95e-05 + eta*( -1.84e-12 + eta*( 3.98e-05 + eta*( -1.62e-12 + eta*( 3.06e-06 ) ) ) ); }
double  L1TTsigma_reta_vs_eta(double eta) { return 1.37e-03  + eta*eta*( -2.32e-04  + eta*eta*(  2.38e-04 + eta*eta*(1.22e-06 + eta*eta*( -1.23e-05 + eta*eta*( 1.10e-06 ) ) ) ) ); }
double   L1TTmean_reta_vs_eta(double eta) { return 5.12e-05 + eta*( -3.22e-09 + eta*( 6.82e-05 + eta*( 8.35e-10 + eta*( -2.71e-05 ) ) ) ); }
double  L1TTsigma_rd0_vs_eta(double eta) { return 1.67e-01  + eta*eta*( 1.63e-02  + eta*eta*(  1.38e-02 + eta*eta*(-3.01e-04 + eta*eta*( -1.12e-04 + eta*eta*( 1.29e-04 ) ) ) ) ); }
double   L1TTmean_rd0_vs_eta(double eta) { return 2.30e-02 + eta*( 2.46e-08 + eta*( 1.82e-03 + eta*( -2.32e-08 + eta*( -1.12e-03 ) ) ) ); }
double  L1TTsigma_rzed_vs_eta(double eta) { return 5.92e-01  + eta*eta*( -1.41e-02  + eta*eta*(  8.76e-02 + eta*eta*(1.22e-02 + eta*eta*( -1.82e-03 + eta*eta*( 3.02e-04 ) ) ) ) ); }
double   L1TTmean_rzed_vs_eta(double eta) { return 4.51e-02 + eta*( -1.07e-12 + eta*( -7.63e-02 + eta*( -4.07e-13 + eta*( 3.32e-02 ) ) ) ); }

double L1TTsigma_reta( double pt, double eta ) { return L1TTsigma_reta_vs_pt( pt ) * L1TTsigma_reta_vs_eta( eta ); } 
double L1TTsigma_rphi( double pt, double eta ) { return L1TTsigma_rphi_vs_pt( pt ) * L1TTsigma_rphi_vs_eta( eta ); } 
double L1TTsigma_ript( double pt, double eta ) { return L1TTsigma_ript_vs_pt( pt ) * L1TTsigma_ript_vs_eta( eta ); } 
double L1TTsigma_rzed( double pt, double eta ) { return L1TTsigma_rzed_vs_pt( pt ) * L1TTsigma_rzed_vs_eta( eta ); } 
double  L1TTsigma_rd0( double pt, double eta ) { return  L1TTsigma_rd0_vs_pt( pt ) *  L1TTsigma_rd0_vs_eta( eta ); } 
