//
#include "TMath.h"

#include "drs/obj/TDrsFitResults.hh"
#include "drs/ana/TNoisFitAlg.hh"
#include "drs/obj/TDrsChannel.hh"
#include "drs/ana/drsana.hh"

ClassImp(TNoisFitAlg)

//-----------------------------------------------------------------------------
TNoisFitAlg::TNoisFitAlg(const char* Name, const char* Title, TObjArray* ListOfChannels) : 
TDrsFitAlg(Name, Title, ListOfChannels) {

  // fLevel          = 0.1;
  // fMinFraction    =  5.; // 2.;
  // fMaxFraction    = 50.;
 }

//-----------------------------------------------------------------------------
TNoisFitAlg::~TNoisFitAlg() {
}


//----------------------------------------------------------------------------
// calculate derivative at a given 'Level', extrapolate down to zero to get the time stampb
// Fitted amplitudes are normalized
// to 100, however the fit parameters may be different
// 10% correspond to Level=10					// time fit histograms


// parameters of the gaussian fit: A*exp(-(x-B)^2/C^2)
// A = Par[0], B = Par[1], c= Par[2]
// returns delta_T in units of channels
// Level : 0.1, 0.2 etc...
//-----------------------------------------------------------------------------
int TNoisFitAlg::BookHistograms() {

  char name[100];
  TDrsChannel* ch;

  Hist_t*  Hist = &fHist;

  int nch = fCachedListOfChannels->GetEntriesFast();

  int n = 0;
  for (int i=0; i<nch; i++) {
    ch = (TDrsChannel*) fCachedListOfChannels->At(i);
    if (ch) {
      sprintf(name,"mean_offset_%2i",i);
      Hist->fOffset[i] = new TH1F(name,name,1000,-50,50);

      sprintf(name,"noise_%2i",i);
      Hist->fNoise[i]  = new TH1F(name,name,500, 0,25);
    }
  }

  return 0;
}

//-----------------------------------------------------------------------------
int TNoisFitAlg::FillHistograms() {

  float level = 10.;

  double           q1,  q2, ph1, ph2;
  TDrsChannel     *ch1, *ch2, *ch;
  TDrsFitResults  *res;

  Hist_t*  Hist = &fHist;

  int nch = fCachedListOfChannels->GetEntriesFast();

  for (int i=0; i<nch; i++) {
    ch = (TDrsChannel*) fCachedListOfChannels->At(i);
    if (ch) {
      res = (TDrsFitResults*) fListOfFitResults->At(i);
      fHist.fOffset[i]->Fill(res->fPar[0]);
      fHist.fNoise [i]->Fill(res->fPar[1]);
    }
  }

  return 0;
}


//-----------------------------------------------------------------------------
// Noise analysis
//-----------------------------------------------------------------------------
int TNoisFitAlg::Fit(const TDrsChannel* Channel) { 

  double   npt1;
  double   v1, sum_v1, sum_v12, v1m, v12m, sigv1;
  drsana*  drs;

  sum_v1  = 0;
  sum_v12 = 0;
    
  npt1    = 0;

  int ich = Channel->GetNumber();

  TDrsFitResults* res = GetFitResults(ich);

  drs = drsana::fgInstance;

  for (int i=1; i<=200; i++) {
    v1 = drs->b1_c[ich][i]-Channel->fBaseOffset;

    if (fabs(v1) < 4) {
      sum_v1  += drs->b1_c[ich][i];
      sum_v12 += drs->b1_c[ich][i]*drs->b1_c[ich][i];
      npt1++;
    }
  }
    
  npt1 += 1.e-12;

  v1m  = sum_v1 /npt1;
  v12m = sum_v12/npt1;
    
  sigv1 = TMath::Sqrt(v12m-v1m*v1m);

  res->fPar[0] = v1m;
  res->fPar[1] = sigv1;
  
  return 0;
}
