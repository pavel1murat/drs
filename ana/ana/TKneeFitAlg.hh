#ifndef TKneeFitAlg_hh
#define TKneeFitAlg_hh

#include "TH1.h"
#include "TH2.h"

#include "TDrsConfig.hh"
#include "TDrsFitAlg.hh"

class TKneeFitAlg: public TDrsFitAlg {
public:
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
  struct Hist_t {
    TH1F*     fDt    [10];
    TH2F*     fDtVsT0[10];

    TH1F*     fOffset[kMaxNChannels];
    TH1F*     fSlope [kMaxNChannels];
    TH1F*     fT0    [kMaxNChannels];
    TH1F*     fSigT0 [kMaxNChannels];
    TH1F*     fDtNorm[kMaxNChannels];

    TH2F*     fDt2VsPH1;		 //
    TH2F*     fDt2VsPH2;		 //
    TH2F*     fDt2VsT0;			 //
  };

  Hist_t fHist ;
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
  TKneeFitAlg(const char* Name="knee", const char* Title="knee", TObjArray* ListOfChannels = 0);
  ~TKneeFitAlg();

  static  double  PulseShape(double* X, double* Par);

  static    void  FCN(Int_t&    NPar , 
		      Double_t* Gin  , 
		      Double_t& F    , 
		      Double_t* Par  , 
		      Int_t     IFlag) ;

  virtual double  Dt(TDrsFitResults* R1, TDrsFitResults* R2, double Level = 0);
  virtual double  T0(TDrsFitResults* Res, double Level = 0);

  virtual int     Display();
  virtual int     Fit    (const TDrsChannel* Channel);

  virtual int BookHistograms();
  virtual int FillHistograms();

  ClassDef(TKneeFitAlg,0)
};



#endif
