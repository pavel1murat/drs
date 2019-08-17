//

#include "TMath.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TPaveLabel.h"

#include "drs/obj/TDrsFitResults.hh"
#include "drs/obj/TDrsChannel.hh"

#include "drs/base/TDrsUtils.hh"

#include "drs/ana/TExpoFitAlg.hh"
#include "drs/ana/drsana.hh"

ClassImp(TExpoFitAlg)


//-----------------------------------------------------------------------------
double TExpoFitAlg::Fun(double* x, double* par) {
  double f;

  f = par[0]*(1-TMath::Exp(-par[1]*(x[0]-par[2])));

  return f;
}



//-----------------------------------------------------------------------------
TExpoFitAlg::TExpoFitAlg(const char* Name, const char* Title, TObjArray* ListOfChannels) : 
  TDrsFitAlg(Name, Title, ListOfChannels) {
  
  TDrsChannel*  ch;

  TDrsFitResults  *res;

  for (int i=0; i<fNChannels; i++) {
    ch = (TDrsChannel*) fCachedListOfChannels->At(i);
    if (ch) {

      SetMinMaxFractions(i,0.05,0.50); 

      res = new TDrsFitResults(ch);
      fListOfFitResults->AddAt(res,i);
      
      res->fFun = new TF1(Form("f_expo_ch_%03i",i),TExpoFitAlg::Fun,0,1024,3);
    }
  }
}

//-----------------------------------------------------------------------------
TExpoFitAlg::~TExpoFitAlg() {
}


//-----------------------------------------------------------------------------
double TExpoFitAlg::T0(TDrsFitResults* R, double Level) {
  double t0 = R->fPar[2] - 1./R->fPar[1]*TMath::Log(1-Level/R->fPar[0]);
  return t0;
}

//-----------------------------------------------------------------------------
double TExpoFitAlg::Dt(TDrsFitResults* R1, TDrsFitResults* R2, double Level) {
  double t1, t2, dt;
  t1 = R1->fPar[2];
  t2 = R2->fPar[2];
  dt = t1-t2;
  return dt;
}

//-----------------------------------------------------------------------------
int TExpoFitAlg::BookHistograms() {

  Hist_t* Hist = &fHist;

  char  name[100];

  HBook1F(Hist->fDt[0],"dt0","Delta(T)[0] EXPO (ns)",500,-2.5,2.5);
  HBook1F(Hist->fDt[1],"dt1","Delta(T)[1] EXPO (ns)",500,-2.5,2.5);
  HBook1F(Hist->fDt[2],"dt2","Delta(T)[2] EXPO (ns)",500,-2.5,2.5);

  HBook1F(Hist->fDt01,"dt01","DT01        EXPO (ns)",500,-2.5,2.5);

  for (int i=0; i<fNChannels; i++) {
    TDrsChannel* ch = GetChannel(i);
    if (ch) {

      sprintf(name,"max_%0i",i);
      HBook1F(Hist->fMax[i],name,name,100,0,200);

      sprintf(name,"slope_%0i",i);
      HBook1F(Hist->fSlo[i],name,name,100,0,1);

      sprintf(name,"chi2_%0i",i);
      HBook1F(Hist->fChi2[i],name,name,100,0,10);

      sprintf(name,"shape_%0i",i);
      HBook2F(Hist->fShape[i],name,name,1024,0,1024,100,0,1);

      sprintf(name,"dtnorm_%02i",i);
      HBook1F(Hist->fDtNorm[i],name,name,100,-0,1);
    }
  }
//-----------------------------------------------------------------------------
// also book histograms for each used channel
//-----------------------------------------------------------------------------

  return 0;
}

//-----------------------------------------------------------------------------
int TExpoFitAlg::FillHistograms() {

  double           q1,  q2, ph1, ph2, t0, t0n, y;
  TDrsChannel     *ch, *ch1, *ch2;
  TDrsFitResults  *r, *r1,  *r2;
  int              ich, it0;

  Hist_t* Hist   = &fHist;

  ch1 = GetUsedChannel(0);
  ch2 = GetUsedChannel(1);

  r1  = GetFitResults(GetUsedChannelNumber(0));
  r2  = GetFitResults(GetUsedChannelNumber(1));

  ph1  = ch1->fVMax;
  ph2  = ch2->fVMax;

  q1   = ch1->fQ;
  q2   = ch2->fQ;
					// exponential fit, T0 == fPar[2]
  fDt = (r1->fPar[2]-r2->fPar[2])/5.;

  double level = 0.1;
  double dt01  = T0(r1,level)-T0(r2,level);

  fHist.fDt [0]->Fill(fDt );

  if ((q1 < ch1->fMinQ) || (q2 < ch2->fMinQ)) {
    fHist.fDt[1]->Fill(fDt);
  }
  else {
    fHist.fDt[2]->Fill(fDt);

    fHist.fDt01->Fill(dt01);

    if (fabs(fDt-0.03) < 0.01) {
      printf("EXPO_003: entry: %5i dt = %10.3f\n",
	     drsana::fgInstance->fEntry,
	     fDt);
    }
    else if (fabs(fDt-0.19) < 0.01) {
      printf("EXPO_019: entry: %5i dt = %10.3f\n",
	     drsana::fgInstance->fEntry,
	     fDt);
    }
  }

  for (int i =0; i<2; i++) {
    ch = GetUsedChannel(i);
    ich = ch->GetNumber();
    r  = GetFitResults(ich);

    t0 =  T0(r,0);
    it0 = ((int) t0)-10;
    if (it0 < 0) it0 = 0;

    for (int i=it0; i<kNCells; i++) {
      y = ch->GetHist()->fShape->GetBinContent(i+1);
      fHist.fShape[ich]->Fill(i-it0,y);
    }

    t0n = t0-((int) t0);
    fHist.fDtNorm[ich]->Fill(t0n);
  }

  return 0;
}


//-----------------------------------------------------------------------------
int TExpoFitAlg::Fit(const TDrsChannel* Channel) {

					// determine the fit range

  int  i0, max_bin, min_bin, found(0);

  double vmax;

  int ich = Channel->GetNumber();

  TDrsFitResults* res = (TDrsFitResults*) fListOfFitResults->At(ich);
  
  vmax = Channel->fVMax;
					// determine the fit range

  int ncells = Channel->GetNCells();

  for (int i=0; i<ncells; i++) {
    if (Channel->fV[i] > Channel->fMinThreshold) {
      min_bin = i; 
      found   = 1;
      break;
    }
  }

  if (found == 0) {
    return -1;
  }
					// look for the first maximum
  for (int i=min_bin; i<ncells; i++) {
    max_bin = i;
    if (Channel->fV[i+1] < Channel->fV[i]) { 
      break;
    }
  }
//-----------------------------------------------------------------------------
// now move in the opposite direction to find the 2% level
//-----------------------------------------------------------------------------
  min_bin = max_bin;
  for (int i=max_bin; i>=0; i--) {
    if (Channel->fV[i] > vmax*fMinFraction[ich]) { // 2.) { 1=136.2
      min_bin = i;
    }
    else {
      break;
    }
  }

  if (max_bin < 0   ) max_bin=0;
  if (max_bin > 1023) max_bin=1023;
//-----------------------------------------------------------------------------
// now move in the opposite direction to find the 2% level
// fMinFraction is in percent....
//-----------------------------------------------------------------------------
  min_bin = max_bin;
  i0 = max_bin;
  for (int i=i0-1; i>=0; i--) {
    if (Channel->fV[i] > vmax*fMaxFraction[ich]) {
      max_bin = i;
    }
    if (Channel->fV[i] >= vmax*fMinFraction[ich]) {
      min_bin = i;
    }
    else {
      break;
    }
  }

  res->fMinBin = min_bin;
  res->fMaxBin = max_bin;
//-----------------------------------------------------------------------------
// fit shape with A(1-exponential)
//-----------------------------------------------------------------------------
  TF1* f = res->fFun;

  f->SetParameter(0,1.);
  f->SetParameter(1,0.1 );
  f->SetParameter(2,min_bin-2);

  Channel->fHist.fShape->Fit(f,"w0q","",min_bin,max_bin);

  for (int i=0; i<3; i++) {
    res->fPar[i] = f->GetParameter(i);
    res->fSig[i] = f->GetParError (i);
  }
  res->fChi2 = f->GetChisquare()/(f->GetNumberFitPoints()-2.999);


  return 0;
}


//-----------------------------------------------------------------------------
int TExpoFitAlg::Display() {

  if (fCanvas == 0) {
    fCanvas = new TCanvas("expo_shape_display","expo_shape_display",1000,800);
  }

  float x[1024], y[1024], ex[1024], ey[1024];

  int ich1, ich2, ich, np, ichan;
  TDrsChannel   *ch1, *ch2;
  TDrsFitResults *r1, *r2;
//-----------------------------------------------------------------------------
// used points, first channel
//-----------------------------------------------------------------------------
  fCanvas->cd();

  ich1 = fUsedChannelNumber[0];
  ich2 = fUsedChannelNumber[1];

  ch1 = GetChannel(ich1);
  ch2 = GetChannel(ich2);

  r1  = GetFitResults(ich1);
  r2  = GetFitResults(ich2);

  double xmin, xmax;

  xmin = r1->fPar[2]-10; // 50;
  if (xmin < 0) xmin = 0;
  xmax = r1->fPar[2]+40; // 350;
  if (xmax > 1024) xmax = 1024;

  //  fHist.fShape[ich1]->GetXaxis()->SetRangeUser(xmin,xmax);

  ch1->GetHist()->fShape->SetLineColor(2);
  ch1->GetHist()->fShape->Draw();
  ch2->GetHist()->fShape->Draw("same");

  //  TF1* f1 = fHist.fShape[ich1]->GetFunction("gaus");

  TF1* f1 = r1->fFun;

  if (f1) {
    f1->SetLineColor(2);
    f1->SetLineWidth(1);
    f1->Draw("same");
  }

  //  TF1* f2 = fHist.fShape[ich2]->GetFunction("gaus");
  TF1* f2 = r2->fFun;
  if (f2) {
    f2->SetLineWidth(1);
    f2->Draw("same");
  }

  TPaveLabel  *l1; 

  TDrsUtils::DrawPaveLabelNDC(l1,Form("Entry: %6i ; q1, q2, dte = %7.1f %7.1f %8.4f",
				      drsana::fgInstance->fEntry, 
				      ch1->fQ,
				      ch2->fQ, 
				      (r1->fPar[2]-r2->fPar[2])),
			      0.1,0.85,0.7,0.90);
  return 0;
}
