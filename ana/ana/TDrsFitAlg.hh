///////////////////////////////////////////////////////////////////////////////
// 2 times
///////////////////////////////////////////////////////////////////////////////
#ifndef TDrsFitAlg_hh
#define TDrsFitAlg_hh

#include "TNamed.h"
#include "TObjArray.h"
#include "TFolder.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"

#include "TDrsConfig.hh"

class TDrsChannel;
class TDrsFitResults;
class TCanvas;
class TH1F;

class TDrsFitAlg: public TNamed {

public:

  TObjArray*  fCachedListOfChannels;		// ! cached, not owned
  TObjArray*  fListOfFitResults;

  TObjArray*  fCachedListOfTriggerChannels;
  TObjArray*  fListOfTriggerFitResults;

  int         fNChannels;
  int*        fUsedChannelNumber;               // used channel numbers, currently there are two of them
  double      fPar     [2][10];
  int         fNPar;

  double      fDt;
  double      fT0;
					// channel to be fit
  static TDrsChannel*    fgFitChannel;
  static TDrsFitResults* fgFitResults;

  TCanvas*   fCanvas;

  TH1F*      fDisplayV[kMaxNChannels];   // only for display purposes

  TFolder*   fFolder;

  int        fEventNumber;
					// pulse handling
  double fMinFraction[kMaxNChannels];
  double fMaxFraction[kMaxNChannels];
  int    fNFitBins   [kMaxNChannels];

  int    fDisplayResults; 		// 1: run event display...
//-----------------------------------------------------------------------------
// for fitting
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
  TDrsFitAlg(const char* Name="drsfit", const char* Title="drs fit", TObjArray* ListOfChannels = 0);
  ~TDrsFitAlg();

  TDrsFitResults*  GetFitResults(int I) { return (TDrsFitResults*) fListOfFitResults->At(I); }

  TDrsFitResults*  GetTriggerFitResults(int I) { return (TDrsFitResults*) fListOfTriggerFitResults->At(I); }

  TDrsChannel*  GetChannel(int I) const { return (TDrsChannel*) fCachedListOfChannels->At(I); }

  TDrsChannel*  GetTriggerChannel(int I) const { return (TDrsChannel*) fCachedListOfTriggerChannels->At(I); }

  TFolder*      GetFolder() const { return fFolder; }

  int           GetUsedChannelNumber(int I) const { return fUsedChannelNumber[I]; }
  int           GetDisplayResults   ()      const { return fDisplayResults; }

  TDrsChannel*  GetUsedChannel      (int I) const { 
    return (TDrsChannel*) fCachedListOfChannels->At(fUsedChannelNumber[I]); 
  }

  void    SetMinFraction(int Ch, double Fraction) { fMinFraction[Ch] = Fraction; }
  void    SetMaxFraction(int Ch, double Fraction) { fMaxFraction[Ch] = Fraction; }
  void    SetNFitBins   (int Ch, int    N       ) { fNFitBins   [Ch] = N       ; }

  void    SetMinMaxFractions(int Ch, double MinFraction, double MaxFraction) {
    fMinFraction[Ch] = MinFraction;
    fMaxFraction[Ch] = MaxFraction;
  }

  void    SetDisplayResults(int YesNo) { fDisplayResults = YesNo; }
  void    SetEventNumber(int Number) { fEventNumber = Number; }
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

  virtual int Fit    (const TDrsChannel* Channel) = 0;
  virtual int Display();

  virtual int BookHistograms() = 0;
  virtual int FillHistograms() = 0;
					// time difference between the 2 photons

  virtual double       Dt(TDrsFitResults* R1, TDrsFitResults* R2, double Level = 0) = 0;
  virtual double       T0(TDrsFitResults* R , double Level = 0) = 0;

  ClassDef (TDrsFitAlg,0)
};



#endif
