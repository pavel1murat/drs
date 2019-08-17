#ifndef TExpoFitAlg_hh
#define TExpoFitAlg_hh

#include "TDrsConfig.hh"
#include "TDrsFitAlg.hh"

#include "TH1.h"

class TExpoFitAlg: public TDrsFitAlg {
public:

  struct Hist_t {
    TH1F*     fDt    [10];
    TH1F*     fT0    [kMaxNChannels];
    TH1F*     fChi2  [kMaxNChannels];
    TH1F*     fMax   [kMaxNChannels];
    TH1F*     fSlo   [kMaxNChannels];
    TH2F*     fShape [kMaxNChannels];
    TH1F*     fDt01;
    TH1F*     fDtNorm[kMaxNChannels];
  } fHist;
//-----------------------------------------------------------------------------
// methods
//-----------------------------------------------------------------------------
  TExpoFitAlg(const char* Name="expo", const char* Title="expo fit", TObjArray* ListOfChannels = 0);
  ~TExpoFitAlg();
					// fit function

  virtual double  Dt(TDrsFitResults* R1, TDrsFitResults* R2, double Level = 0);
  virtual double  T0(TDrsFitResults* Res, double Level = 0);

  static  double Fun    (double* X, double* Par);

  Hist_t* GetHist() { return &fHist; }

  virtual int Fit(const TDrsChannel* Channel);
  virtual int Display();

  virtual int BookHistograms();
  virtual int FillHistograms();

  ClassDef(TExpoFitAlg,0)
};



#endif
