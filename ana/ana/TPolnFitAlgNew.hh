#ifndef TPolnFitAlgNew_hh
#define TPolnFitAlgNew_hh

#include "TDrsFitAlgNew.hh"
#include "TH1.h"
#include "TH2.h"

class TMinuit;
class TTofData;

class TPolnFitAlgNew: public TDrsFitAlgNew {
public:
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
  struct Hist_t {
    TH1F*  fDt [10];
    TH1F*  fDtl[10];
    TH1F*  fDtm[10];

    TH1F*     fChi2      [kMaxNChannels];
    TH1F*     fDtNorm    [kMaxNChannels];
    TH2F*     fShape     [kMaxNChannels];
    TH1F*     fGSigma    [kMaxNChannels]; // gaussian sigma of the fit 
    TH1F*     fFitBase   [kMaxNChannels]; // as coming out of the signal fit
    TH1F*     fDtChan    [kMaxNChannels]; // DT, with one of 2 channels being this
    TH1F*     fT0Fit     [kMaxNChannels]; // T0, as fit in this channel
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

    TH2F*     fDt2VsEntry;
    TH2F*     fDt2VsDfc; 

  } fHist;

  double   fLevel; // 0.1

  TMinuit*  fMinuit;

  double   fDtl;			// CFD time at fLevel
  float    fDtm[10];			// Dt mean used for linear extrapolation
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
  TPolnFitAlgNew(const char* Name="poln", const char* Title="poln", int NChannels = 1);
  ~TPolnFitAlgNew();
					// default way of calculationg delta_T, in channels

  void    SetLevel(double Level) { fLevel = Level; }

  virtual double T0   (TDrsFitResults* R, double Level) ;
  virtual double Dt   (TDrsFitResults* R1, TDrsFitResults* R2, double Level);

					// emulate CFD

  virtual double T0CFD(TDrsFitResults* R, double Level);
  virtual double DtCFD(TDrsFitResults* R1, TDrsFitResults* R2, double Level);

  virtual int BookHistograms();
  virtual int FillHistograms();

  static  double PulseShape(double* x, double* par);
  static  void   FCN(Int_t&    NPar , 
		     Double_t* Gin  , 
		     Double_t& F    , 
		     Double_t* Par  , 
		     Int_t     IFlag);

  virtual int    Fit(const TTofData* Data);
  virtual int    Display();

  ClassDef(TPolnFitAlgNew,0)
};

#endif
