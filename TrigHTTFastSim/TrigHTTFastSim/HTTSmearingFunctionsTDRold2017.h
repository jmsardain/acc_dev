#ifndef HTTSMEARINGFUNCTIONS_HTTSMEARINGFUNCTIONS_H
#define HTTSMEARINGFUNCTIONS_HTTSMEARINGFUNCTIONS_H

// ------------------- //
// --- Description --- //
// ------------------- //

// Old set of smearing functions derived in 2017 to be used in TDR
// Documentation here: https://twiki.cern.ch/twiki/bin/viewauth/Atlas/L1TrackSmearing

double L1TTsigma_ript_vs_pt(double);
double L1TTsigma_rphi_vs_pt(double);
double L1TTsigma_reta_vs_pt(double);
double L1TTsigma_rd0_vs_pt(double);
double L1TTsigma_rzed_vs_pt(double);

double  L1TTsigma_ript_vs_eta(double);
double   L1TTmean_ript_vs_eta(double);
double  L1TTsigma_rphi_vs_eta(double);
double   L1TTmean_rphi_vs_eta(double);
double  L1TTsigma_reta_vs_eta(double);
double   L1TTmean_reta_vs_eta(double);
double  L1TTsigma_rd0_vs_eta(double); 
double   L1TTmean_rd0_vs_eta(double);
double  L1TTsigma_rzed_vs_eta(double);
double   L1TTmean_rzed_vs_eta(double);

double L1TTsigma_reta( double, double );
double L1TTsigma_rphi( double, double );
double L1TTsigma_ript( double, double );
double L1TTsigma_rzed( double, double );
double  L1TTsigma_rd0( double, double );

#endif
