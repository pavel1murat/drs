#ifndef TEvenFitAlg_hh
#define TEvenFitAlg_hh

#include "TDrsConfig.hh"
#include "TDrsFitAlg.hh"

#include "TH1.h"
#include "TMinuit.h"

class TEvenFitAlg: public TDrsFitAlg {
public:

  struct Hist_t {
    TH1F*     fShift [kMaxNChannels];
    TH1F*     fChi2  [kMaxNChannels];
  } fHist;

  TMinuit*    fMinuit;
  static  int fgFitChannel;
//-----------------------------------------------------------------------------
// methods
//-----------------------------------------------------------------------------
  TEvenFitAlg(const char* Name="even", const char* Title="odd-even shift fit", TObjArray* ListOfChannels = 0);
  ~TEvenFitAlg();
					// fit function

  static    void  FCN(Int_t&    NPar , 
		      Double_t* Gin  , 
		      Double_t& F    , 
		      Double_t* Par  , 
		      Int_t     IFlag) ;

  static  double Fun    (double* X, double* Par);

  virtual int Fit(const TDrsChannel* Channel);

  virtual int BookHistograms();
  virtual int FillHistograms();

  ClassDef(TEvenFitAlg,0)
};



#endif
