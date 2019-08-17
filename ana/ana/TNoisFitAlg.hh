#ifndef TNoisFitAlg_hh
#define TNoisFitAlg_hh

#include "TH1.h"
#include "TH2.h"

#include "TDrsConfig.hh"
#include "TDrsFitAlg.hh"

class TNoisFitAlg: public TDrsFitAlg {
public:
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
  struct Hist_t {
    TH1F*     fOffset[kMaxNChannels];
    TH1F*     fNoise [kMaxNChannels];
  };

  Hist_t fHist ;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
  TNoisFitAlg(const char* Name="nois", const char* Title="nois", TObjArray* ListOfChannels = 0);
  ~TNoisFitAlg();

  virtual int     Fit    (const TDrsChannel* Channel);

  virtual int BookHistograms();
  virtual int FillHistograms();

  ClassDef(TNoisFitAlg,0)
};



#endif
