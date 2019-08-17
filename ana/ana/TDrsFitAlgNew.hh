///////////////////////////////////////////////////////////////////////////////
// 2 times
///////////////////////////////////////////////////////////////////////////////
#ifndef TDrsFitAlgNew_hh
#define TDrsFitAlgNew_hh

#include "TNamed.h"
#include "TObjArray.h"
#include "TFolder.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"

#include "TDrsConfig.hh"

class TDrsFitResults;
class TCanvas;
class TH1F;
class TTofData;

class TDrsFitAlgNew: public TNamed {

public:

  TTofData*   fCachedData;		// ! cached, not owned
  TObjArray*  fListOfFitResults;

  TObjArray*  fCachedListOfTrigChannels;
  TObjArray*  fListOfTrigFitResults;

  int         fNChannels;
  int         fNTrigChannels;           // nch/8
  int         fTimeChannelNumber[10];   // used channel numbers, one per module
					// 2012-12-27: currently there are 2 modules
  double      fPar     [2][10];
  int         fNPar;

  double      fDt;
  double      fT0;
					// data to be fit
  static TTofData*       fgFitData;
  static TDrsFitResults* fgFitResults;

  TCanvas*   fCanvas;

  TH1F       **fDisplayV;               // only for display purposes

  TFolder*   fFolder;
					// pulse handling
  double     fMinFraction[kMaxNChannels];
  double     fMaxFraction[kMaxNChannels];

  double     fMinTrigFraction[kMaxNChannels];
  double     fMaxTrigFraction[kMaxNChannels];

  int        fNFitBins[kMaxNChannels]   ;

  int        fDisplayResults; 		// 1: run event display...

  double     fMinQ1;
  double     fMaxQ1;
  int        fFitMode;                  // = 1 : use TH1::Fit 
					// = 2 : use Minuit (for gaus)
//-----------------------------------------------------------------------------
// for fitting
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
  TDrsFitAlgNew(const char* Name="drsfit", const char* Title="drs fit", int NChannels = 1);
  ~TDrsFitAlgNew();

  TDrsFitResults*  GetFitResults(int I) { return (TDrsFitResults*) fListOfFitResults->At(I); }
  TDrsFitResults*  GetTrigFitResults(int I) { return (TDrsFitResults*) fListOfTrigFitResults->At(I); }

  TFolder*         GetFolder() const { return fFolder; }

  int              GetTimeChannelNumber(int I) const { return fTimeChannelNumber[I]; }
  int              GetDisplayResults   ()      const { return fDisplayResults; }
  double           GetMinQ1() { return fMinQ1; }
  double           GetMaxQ1() { return fMaxQ1; }
//-----------------------------------------------------------------------------
// setters
//-----------------------------------------------------------------------------
  void    SetMinQ1(double Q) { fMinQ1 = Q; }
  void    SetMaxQ1(double Q) { fMaxQ1 = Q; }
  void    SetTimeChannelNumber(int I, int Channel) { fTimeChannelNumber[I]  = Channel ; }
  void    SetMinFraction(int Ch, double Fraction)  { fMinFraction      [Ch] = Fraction; }
  void    SetMaxFraction(int Ch, double Fraction)  { fMaxFraction      [Ch] = Fraction; }
  void    SetNFitBins   (int Ch, int    N       )  { fNFitBins         [Ch] = N       ; }

  void    SetMinMaxFractions(int Ch, double MinFraction, double MaxFraction) {
    fMinFraction[Ch] = MinFraction;
    fMaxFraction[Ch] = MaxFraction;
  }

  void    SetMinMaxTrigFractions(int Ch, double MinFraction, double MaxFraction) {
    fMinTrigFraction[Ch] = MinFraction;
    fMaxTrigFraction[Ch] = MaxFraction;
  }

  void    SetDisplayResults(int YesNo) { fDisplayResults = YesNo; }
//-----------------------------------------------------------------------------
// the following helper methods allow to save 1 line per request, which in 
// case of 100's histograms booked is a non-negligible number
// taken from TStnModule
//-----------------------------------------------------------------------------
  void  DeleteHistograms(TFolder* Folder = NULL);

  void  AddHistogram(TObject* hist, const char* FolderName = "Hist");

  void  HBook1F(TH1F*& Hist, const char* Name, const char* Title,
		Int_t Nx, Double_t XMin, Double_t XMax,
		const char* FolderName = "Hist");

  void  HBook2F(TH2F*& Hist, const char* Name, const char* Title,
		Int_t Nx, Double_t XMin, Double_t XMax,
		Int_t Ny, Double_t YMin, Double_t YMax,
		const char* FolderName = "Hist");

  void  HProf (TProfile*& Hist, const char* Name, const char* Title,
	       Int_t Nx, Double_t XMin, Double_t XMax,
	       Double_t YMin, Double_t YMax,
	       const char* FolderName = "Hist");

  virtual int Fit    (const TTofData* Data) = 0;
  virtual int Display();

  virtual int BookHistograms() = 0;
  virtual int FillHistograms() = 0;
					// time difference between the 2 photons

  virtual double T0(TDrsFitResults* R , double Level) = 0;
  virtual double Dt(TDrsFitResults* R1, TDrsFitResults* R2, double Level) = 0;

  virtual double T0CFD(TDrsFitResults* R, double Level) = 0;
  virtual double DtCFD(TDrsFitResults* R1, TDrsFitResults* R2, double Level) = 0;

  ClassDef (TDrsFitAlgNew,0)
};



#endif
