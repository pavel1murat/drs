#ifndef TExp1FitAlgNew_hh
#define TExp1FitAlgNew_hh

#include "TDrsFitAlgNew.hh"
#include "TH1.h"
#include "TH2.h"

class TMinuit;
class TTofData;

class TExp1FitAlgNew: public TDrsFitAlgNew {
public:
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
  struct Hist_t {
    TH1F*  fDt [10];
    TH1F*  fDtl[10];

    TH1F*     fNFitPoints[kMaxNChannels];
    TH1F*     fChi2      [kMaxNChannels];
    TH1F*     fDtNorm    [kMaxNChannels];
    TH2F*     fShape     [kMaxNChannels];
    TH1F*     fP0        [kMaxNChannels]; // exp1sian sigma of the fit 
    TH1F*     fP1        [kMaxNChannels]; // exp1sian sigma of the fit 
    TH1F*     fP2        [kMaxNChannels]; // exp1sian sigma of the fit 
    TH1F*     fDtChan    [kMaxNChannels]; // DT, with one of 2 channels being this
    TH1F*     fT0Fit     [kMaxNChannels]; // T0, as fit in this channel
    TH1F*     fDtFit     [kMaxNChannels]; // T0Fit-T0, as fit in this channel
    TH2F*     fAvePulse  [kMaxNChannels]; // the pulse shape
    TH2F*     fAveShape  [kMaxNChannels]; // the pulse shape, normalized
    
    TH1F*     fDt2021;
    TH1F*     fDtSL;

    TH2F*     fDtVsPH1;   //
    TH2F*     fDtVsPH2;   //
    TH2F*     fDtVsT0;    //
    TH2F*     fDtlVsDP0;    //

    TH2F*     fDt2VsEntry;
    TH2F*     fDt2VsDfc;

    TH2F*     fDt2VsIch1;
    TH2F*     fDt2VsIch2;

  } fHist;

  double   fLevel; // 0.1

  TMinuit*  fMinuit;

  double   fDtl;			// CFD time at fLevel
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
  TExp1FitAlgNew(const char* Name="exp1", const char* Title="exp1", int NChannels = 1);
  ~TExp1FitAlgNew();
					// default way of calculationg delta_T, in channels

  virtual double T0   (TDrsFitResults* R, double Level) ;
  virtual double Dt   (TDrsFitResults* R1, TDrsFitResults* R2, double Level);

					// emulate CFD

  virtual double T0CFD(TDrsFitResults* R, double Level);
  virtual double DtCFD(TDrsFitResults* R1, TDrsFitResults* R2, double Level);

  void    SetLevel(double Level) { fLevel = Level; }

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

  ClassDef(TExp1FitAlgNew,0)
};

#endif
