#ifndef drs_ana_Tfit_calibrations_hh
#define drs_ana_Tfit_calibrations_hh

#include "TObject.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TGraphErrors.h"

class TFitCalibrations: public TObject {
public:

  enum {npt = 5};

  double par[1024][npt][3];
  double err[1024][npt][2];
  
  TH1F   *h_slope[32], *h_offset[32], *h_chi2[32];
  TH1F   *h_mean_slope, *h_mean_offset, *h_mean_chi2;
  TH1F   *h_sigm_slope, *h_sigm_offset, *h_sigm_chi2;

  TH2F*  h2[npt];
  TH1D*  hpy[1024][npt];

  TGraphErrors  *g_calib[32][1024];


  int fit_calib_channel(int Channel, 
			int FirstCell=0, 
			int NCells=1024, 
			const char* Mode="",
			const char* OutputFilename=0);

  // fit several channels together

  int fit_calib_module (int IMin=0, int IMax=32) ;

  static double fitf(double* x, double* p) ;

  ClassDef(TFitCalibrations,0)
};



#endif
