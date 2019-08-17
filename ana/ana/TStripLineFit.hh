#ifndef TStripLineFit_hh
#define TStripLineFit_hh

#include "TDrsFitAlgNew.hh"
#include "TH1.h"
#include "TH2.h"

class TMinuit;
class TTofData;

class TStripLineFit: public TDrsFitAlgNew {
public:
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
  struct Hist_t {
    TH1F*  fDt[2] ;
  } fHist;

  double   fLevel; // 0.1

  TMinuit*  fMinuit;

  double   fDtl;			// CFD time at fLevel
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
  TStripLineFit(const char* Name="SLine", const char* Title="SLine", int NChannels = 1);
  ~TStripLineFit();
					// default way of calculating delta_T, in channels

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

  ClassDef(TStripLineFit,0)
};

#endif
