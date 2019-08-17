//

#include "drs/ana/TEvenFitAlg.hh"
#include "drs/obj/TDrsFitResults.hh"

#include "drs/ana/drsana.hh"

ClassImp(TEvenFitAlg)


int TEvenFitAlg::fgFitChannel = -1;

//-----------------------------------------------------------------------------
TEvenFitAlg::TEvenFitAlg(const char* Name, const char* Title, TObjArray* ListOfChannels) : 
  TDrsFitAlg(Name, Title, ListOfChannels) {
  
  TDrsChannel*  ch;
  TDrsFitResults* res;

  int nch = fCachedListOfChannels->GetEntries();

  for (int i=0; i<nch; i++) {
    ch = (TDrsChannel*) fCachedListOfChannels->At(i);
    if (ch) {
      res = new TDrsFitResults(ch);
      fListOfFitResults->AddAt(res,i);
      
      res->fFun = new TF1(Form("f_expo_ch_%03i",i),TEvenFitAlg::Fun,0,1024,3);
    }
  }

  fNPar = 1;
}

//-----------------------------------------------------------------------------
TEvenFitAlg::~TEvenFitAlg() {
}


//-----------------------------------------------------------------------------
int TEvenFitAlg::BookHistograms() {

  Hist_t* Hist = &fHist;

  char   name[100];
  TDrsChannel*  ch;

  DeleteHistograms();
//-----------------------------------------------------------------------------
// book histograms for all channels used
//-----------------------------------------------------------------------------
  int nch = fCachedListOfChannels->GetEntries();
  for (int i=0; i<nch; i++) {
    ch = (TDrsChannel*) fCachedListOfChannels->At(i);
    if (ch) {
					// T0 is par[2], the slope is par[3]
      sprintf(name,"shift_%2i",i);
      Hist->fShift[i] = new TH1F(name,name,500,-5,5);

      sprintf(name,"chi2_%2i",i);
      Hist->fChi2 [i] = new TH1F(name,name,500,-5,5);
    }
  }
//-----------------------------------------------------------------------------
// also book histograms for each used channel
//-----------------------------------------------------------------------------

  return 0;
}


//-----------------------------------------------------------------------------
int TEvenFitAlg::FillHistograms() {

  double           q1,  q2;
  TDrsChannel     *ch, *ch1, *ch2;
  TDrsFitResults  *r, *r1,  *r2;

  Hist_t* Hist = &fHist;
//-----------------------------------------------------------------------------
// book histograms for all channels used
//-----------------------------------------------------------------------------
  int nch = fCachedListOfChannels->GetEntries();
  for (int i=0; i<nch; i++) {
    ch = (TDrsChannel*) fCachedListOfChannels->At(i);
    if (ch) {
					// T0 is par[2], the slope is par[3]
      r = GetFitResults(i);

      fHist.fShift[i]->Fill(r->fPar[0]);
      fHist.fChi2 [i]->Fill(r->fChi2/510.);
    }
  }
  return 0;
}



//_____________________________________________________________________________
void TEvenFitAlg::FCN(Int_t&    NPar , 
		      Double_t* Gin  , 
		      Double_t& F    , 
		      Double_t* Par  , 
		      Int_t     IFlag) {

  int    ich, start_bin, maxbin;
  double chi2 = 0;
  double t, v, err, dchi2, f, v1, v2, v3, dy;
//-----------------------------------------------------------------------------
// use bins from 1 to maxbin in the fit
// maxbin is defined by the pulse intersecting fMaxFraction(default = 0.6) of its height
//-----------------------------------------------------------------------------
  ich       = TEvenFitAlg::fgFitChannel;
//-----------------------------------------------------------------------------
// check whether shift of even bins by a constant aligns then with the odd bins
//-----------------------------------------------------------------------------
  for (int i=1; i<1023; i+=2) {
    v1    = drsana::fgInstance->b1_c[ich][i-1];
    v2    = drsana::fgInstance->b1_c[ich][i];
    v3    = drsana::fgInstance->b1_c[ich][i+1];

    err  = 1.;	   // drsana::fFitHist->GetBinError  (i);
//-----------------------------------------------------------------------------
// shift even bins
//-----------------------------------------------------------------------------
    dy    = (v1+v3)/2.-(v2+Par[0]);

    dchi2 = (dy*dy)/(err*err);
    chi2 += dchi2;

    if (drsana::fgInstance->fDebug[0] != 0) {
      printf("i,v,f,err,dchi2,chi2 = %3i %10.5f %10.5f %10.5f %10.5f %10.5f\n",
	     i,v,f,err,dchi2,chi2);
    }
  }
  
  F = chi2;
}

//-----------------------------------------------------------------------------
// this is a one parameter shift
//-----------------------------------------------------------------------------
int TEvenFitAlg::Fit(const TDrsChannel* Channel) {

  Long64_t        nentries, ientry, nb, nent;
  double          sum_v1  , sum_v2, sum_v12, sum_v22;
  double          v1min   , v2min , dt;
  int             ich;
  TDrsChannel     *ch; 
  TDrsFitResults  *r ;

  double           arglist[10]; 
  double           grad[10];
  int              ierflg;

  if (fMinuit) delete fMinuit;

  fMinuit = new TMinuit(1);

  fMinuit->SetFCN(TEvenFitAlg::FCN);

  //  drsana::fgInstance = this;

  fgFitChannel = Channel->GetNumber();
//-----------------------------------------------------------------------------
// 1-parameter time fit analysis
// Minuit 
//-----------------------------------------------------------------------------
  arglist[0]  = 1;
  fMinuit->mnexcm("SET ERR", arglist ,1,ierflg);
//-----------------------------------------------------------------------------
// Set starting values and step sizes for parameters and do minimization
//-----------------------------------------------------------------------------
  fMinuit->mnparm(0, "p0"   , 0.1, 1., -50, 50, ierflg);

  arglist[0] = 1000;
  arglist[1] = 1.;
  fMinuit->mnexcm("MIGRAD", arglist ,2,ierflg);

  r  = GetFitResults(fgFitChannel);
  
  for (int ip=0; ip<fNPar; ip++) {
    fMinuit->GetParameter(ip,r->fPar[ip],r->fSig[ip]);
  }
//-----------------------------------------------------------------------------
// get chi2 of the fit - FCN is supposed to return it....
//-----------------------------------------------------------------------------
  fMinuit->Eval(fNPar,grad,r->fChi2,r->fPar,3);

  return 0;
}

//-----------------------------------------------------------------------------
double TEvenFitAlg::Fun(double* X, double* Par) {
  return 0;
}


