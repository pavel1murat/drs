#ifndef OscFit_hh
#define OscFit_hh

#include <TROOT.h>
#include <TEnv.h>
#include <TF1.h>
#include <TMath.h>
#include <TFile.h>
#include <TTree.h>
#include <TDirectory.h>
#include <TMarker.h>
#include <TCanvas.h>

#include <TGraph.h>
#include <TGraphErrors.h>
#include <TH2.h>

#include <iostream>
#include <sstream>
#include <cmath>
#include <string>
#include <vector>
#include <map>

using std::cout;     using std::endl;

class OscFit: public TNamed {
public:
  Int_t evt;
  Float_t adc[8];
  Float_t adcf[8];        // ADC from fit
  Float_t t[8];
  Float_t d[8];
  Float_t dydx[8];        // derivative
  Float_t tau1[8];
  Float_t tau2[8];
  Float_t T[8];
  Float_t sigma[8];
  Float_t bkg[8];         // flat background before the signal
  Float_t sbkg[8];        // sigma of the flat background
  Float_t v[8];
  Float_t xmaxi[8];       // beginning of the local maximum
  Float_t ymaxi[8];       // beginning of the local maximum
  Float_t xt[8];          // x-coordinate of point to draw tangent
  Float_t yt[8];          // y-coordinate of point to draw tangent
  Float_t chi2[8];
  Float_t np[8];
  // line fit
  Float_t p0[8];          // slope in y = a*x + b
  Float_t p1[8];          // intercept
  Float_t dline[8];       // intersection of the fitted line with y-axis
  Float_t chi2line[8];    // chi2 of line fit
  Float_t npline[8];
  // for point algorithm
  Float_t yscal[8];       // average of three points around the maximum
  Float_t pti[8];         // integral which use pt algorithm
  Float_t pty[8];
  Float_t ptx[8];
  Float_t plx[8];
  Float_t plx0[8];
  Float_t plc[8];        // chi2/NDF for 3-point fit
  Bool_t ptok[8];
   // flag
  Bool_t sat[8];
  Bool_t ok[8];
  Bool_t dok[8];
  Bool_t tok[8];
public:

  OscFit();
  ~OscFit();

  void clear();

  static Double_t fline(Double_t xx[], Double_t par[]) ;

  static Double_t ITtau     (Double_t x, Double_t tau, Double_t T);

  static Double_t ITtausigma(Double_t x  , 
			     Double_t tau, 
			     Double_t T  , 
			     Double_t sigma);

  static Double_t fPsigma   (Double_t *xx, Double_t *par);

  TF1*     fpulse(Double_t xmin, Double_t xmax,
		  Double_t A, Double_t x0, 
		  Double_t tau1, Double_t tau2, 
		  Double_t T, Double_t sigma,
		  const char* name="fpulse") ;

					// linear fit of a TGraph
  Double_t lfit(const Float_t x[], 
		const Float_t y[], 
		Int_t         x1_i, 
		Int_t         npoints, 
		Double_t&     p0, 
		Double_t&     p1) ;
					// linear fit with equal weights
  Double_t pol1fast(const Float_t x [], 
		    const Float_t y [], 
		    const Float_t ey[], 
		    Int_t         ifirst, 
		    Int_t         np    , 
		    Double_t&     am    , 
		    Double_t&     bm    ) ;

  ClassDef(OscFit,1)
};

#endif
