#ifndef TStripLineFitOld_hh
#define TStripLineFitOld_hh

#include "TDrsFitAlg.hh"
#include "TH1.h"
#include "TH2.h"

class TMinuit;
class TTofData;

class TStripLineFitOld: public TDrsFitAlg {
public:
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
  struct Hist_t {
    TH1F*  fDt[2] ;
  } fHist;

  double   fLevel; // 0.1

  TMinuit*  fMinuit;
  int       fFitMode;

  double   fDtl;			// CFD time at fLevel
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
  TStripLineFitOld(const char* Name="SLine", const char* Title="SLine", TObjArray* ListOfChannels = 0);
  ~TStripLineFitOld();
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

  virtual int    Fit(const TDrsChannel* Channel);
  virtual int    Display();

  ClassDef(TStripLineFitOld,0)
};

#endif
