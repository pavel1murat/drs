#ifndef TGausFitAlg_hh
#define TGausFitAlg_hh

#include "TDrsFitAlg.hh"
#include "TH1.h"
#include "TH2.h"

class TMinuit;

class TGausFitAlg: public TDrsFitAlg {
public:
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
  struct Hist_t {
    TH1F*  fDt [10];
    TH1F*  fDtl[10];

    TH1F*     fChi2      [kMaxNChannels];
    TH1F*     fDtNorm    [kMaxNChannels];
    TH2F*     fShape     [kMaxNChannels];
    TH1F*     fGSigma    [kMaxNChannels]; // gaussian sigma of the fit 
    TH1F*     fFitBase   [kMaxNChannels]; // as coming out of the signal fit
    TH1F*     fBaseOffset[kMaxNChannels]; // as coming out of independent fit
    TH1F*     fBaseChi2  [kMaxNChannels]; // chi2/Ndf  of the independent fit

    TH2F*     fDtVsPH1;   //
    TH2F*     fDtVsPH2;   //
    TH2F*     fDtVsT0;    //

    TH2F*     fDtVsSig1;
    TH2F*     fDtVsSig2;

    TH2F*     fDt3VsSig1;
    TH2F*     fDt3VsSig2;
    TH2F*     fDt3VsSig12;

    TH2F*     fDt4VsSig1;
    TH2F*     fDt4VsSig2;
    TH2F*     fDt4VsSig12;

  } fHist;

  double   fLevel; // 0.1

  TMinuit*  fMinuit;

  double    fChi2Max[2];

  double   fDtl;			// CFD time at fLevel
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
  TGausFitAlg(const char* Name="gaus", const char* Title="gaus", TObjArray* ListOfChannels = 0);
  ~TGausFitAlg();
					// default way of calculationg delta_T, in channels

  virtual double Dt   (TDrsFitResults* R1, TDrsFitResults* R2, double Level = 0);
  virtual double T0   (TDrsFitResults* R, double Level = 0) ;


  void    SetLevel(double Level) { fLevel = Level; }

  void SetChi2Max(int Channel, double Chi2Max) {
    fChi2Max[Channel] = Chi2Max;
  }

					// emulate CFD

  virtual double DtCFD(TDrsFitResults* R1, TDrsFitResults* R2, double Level);
  virtual double T0CFD(TDrsFitResults* R, double Level = 0);

  virtual int BookHistograms();
  virtual int FillHistograms();

  static  double PulseShape(double* x, double* par);
  static  void   FCN(Int_t&    NPar , 
		     Double_t* Gin  , 
		     Double_t& F    , 
		     Double_t* Par  , 
		     Int_t     IFlag);

  virtual int    Fit(const TDrsChannel* Channel);
  virtual int    Display();

  ClassDef(TGausFitAlg,0)
};

#endif
