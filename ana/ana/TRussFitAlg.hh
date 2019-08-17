#ifndef TRussFitAlg_hh
#define TRussFitAlg_hh

#include "TDrsConfig.hh"
#include "TDrsFitAlg.hh"

#include "TH1.h"

class TRussFitAlg: public TDrsFitAlg {
public:

  struct Hist_t {
    TH1F*     fDt    [10];
    TH1F*     fT0    [kMaxNChannels];
    TH1F*     fChi2  [kMaxNChannels];
    TH1F*     fMax   [kMaxNChannels];
    TH1F*     fSlo   [kMaxNChannels];

  } fHist;
//-----------------------------------------------------------------------------
// methods
//-----------------------------------------------------------------------------
  TRussFitAlg(const char* Name="russ", const char* Title="Russ-like fit", TObjArray* ListOfChannels = 0);
  ~TRussFitAlg();
					// fit function

  virtual double  Dt(TDrsFitResults* R1, TDrsFitResults* R2, double Level = 0);
  virtual double  T0(TDrsFitResults* Res, double Level = 0);

  static  double Fun    (double* X, double* Par);

  virtual int Fit(const TDrsChannel* Channel);
  virtual int Display();

  virtual int BookHistograms();
  virtual int FillHistograms();

  ClassDef(TRussFitAlg,0)
};



#endif
