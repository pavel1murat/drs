//

#include "TMath.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TPaveLabel.h"

#include "drs/ana/TRussFitAlg.hh"
#include "drs/obj/TDrsFitResults.hh"
#include "drs/obj/TDrsChannel.hh"
#include "drs/base/TDrsUtils.hh"
#include "drs/ana/drsana.hh"

ClassImp(TRussFitAlg)


//-----------------------------------------------------------------------------
double TRussFitAlg::Fun(double* x, double* par) {
  double f;

  f = par[0]*(1-TMath::Exp(-par[1]*(x[0]-par[2])));

  return f;
}



//-----------------------------------------------------------------------------
TRussFitAlg::TRussFitAlg(const char* Name, const char* Title, TObjArray* ListOfChannels) : 
  TDrsFitAlg(Name, Title, ListOfChannels) {
  
  TDrsChannel*  ch;

  TDrsFitResults  *res;

  for (int i=0; i<fNChannels; i++) {
    ch = (TDrsChannel*) fCachedListOfChannels->At(i);
    if (ch) {

      SetMinMaxFractions(i,0.05,0.50); 

      res = new TDrsFitResults(ch);
      fListOfFitResults->AddAt(res,i);
      
      res->fFun = new TF1(Form("f_expo_ch_%03i",i),TRussFitAlg::Fun,0,1024,3);
    }
  }
}

//-----------------------------------------------------------------------------
TRussFitAlg::~TRussFitAlg() {
}


//-----------------------------------------------------------------------------
double TRussFitAlg::T0(TDrsFitResults* R, double Level) {
  double t0 = R->fPar[2];
  return t0;
}

//-----------------------------------------------------------------------------
double TRussFitAlg::Dt(TDrsFitResults* R1, TDrsFitResults* R2, double Level) {
  double t1, t2, dt;
  t1 = R1->fPar[2];
  t2 = R2->fPar[2];
  dt = t1-t2;
  return dt;
}

//-----------------------------------------------------------------------------
int TRussFitAlg::BookHistograms() {

  Hist_t* Hist = &fHist;

  char  name[100];

  HBook1F(Hist->fDt[0],"dt0","Delta(T)[0] EXPO (ns)",500,-2.5,2.5);
  HBook1F(Hist->fDt[1],"dt1","Delta(T)[1] EXPO (ns)",500,-2.5,2.5);
  HBook1F(Hist->fDt[2],"dt2","Delta(T)[2] EXPO (ns)",500,-2.5,2.5);

  for (int i=0; i<fNChannels; i++) {
    TDrsChannel* ch = GetChannel(i);
    if (ch) {

      sprintf(name,"max_%0i",i);
      HBook1F(Hist->fMax[i],name,name,100,0,200);

      sprintf(name,"slope_%0i",i);
      HBook1F(Hist->fSlo[i],name,name,100,0,1);

      sprintf(name,"chi2_%0i",i);
      HBook1F(Hist->fChi2[i],name,name,100,0,10);
    }
  }
//-----------------------------------------------------------------------------
// also book histograms for each used channel
//-----------------------------------------------------------------------------

  return 0;
}

//-----------------------------------------------------------------------------
int TRussFitAlg::FillHistograms() {

  double           q1,  q2, ph1, ph2;
  TDrsChannel     *ch1, *ch2;
  TDrsFitResults  *r1,  *r2;

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

  fHist.fDt [0]->Fill(fDt );

  if ((q1 < ch1->fMinQ) || (q2 < ch2->fMinQ)) {
    fHist.fDt[1]->Fill(fDt);
  }
  else {
    fHist.fDt[2]->Fill(fDt);
  }

  return 0;
}


//-----------------------------------------------------------------------------
int TRussFitAlg::Fit(const TDrsChannel* Channel) {

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
  f->SetParameter(2,min_bin-1);

  Channel->fHist.fShape->Fit(f,"w0q","",min_bin,max_bin);

  for (int i=0; i<3; i++) {
    res->fPar[i] = f->GetParameter(i);
    res->fSig[i] = f->GetParError (i);
  }
  res->fChi2 = f->GetChisquare()/(f->GetNumberFitPoints()-2.999);
//-----------------------------------------------------------------------------
// now - the 2nd step - go around and calculate chi2 - need to calculate offset
//-----------------------------------------------------------------------------
  double  t0;
  int     it0;

  t0 = res->fPar[2];

  it0 = (int) t0;
  if (it0 < 100) { 
    Error("Fit",Form("it0 = %i",it0));
    it0 = 3;
  }
  else if (it0 > 1020) {
    Error("Fit",Form("it0 = %i",it0));
    it0 = 1020;
  }

  double par[10], chi2, chi2_russ[100], chi2x, chi2y, dx, dy, drx, dry, rho, nx, ny, dydx;

  TDrsUtils::FitPol0(Channel->fV,it0-50,it0-5,par,&chi2); 

  printf("-- RussFit: channel: %i par[0] = %10.3f fit parameters %10.3f %12.5g %10.3f \n",
	 Channel->GetNumber(),par[0], res->fPar[0],res->fPar[1],res->fPar[2]);

  for (int i=it0-5; i<it0+5; i++)  {
    dy    = (Channel->fV[i]-par[0])/(Channel->fVMax-par[0]+1e-12);
    //    dy    = (Channel->fV[i])/(Channel->fVMax+1e-12);

    dydx = res->fPar[0]*res->fPar[1];

    drx = i+0.5-res->fPar[2];
    dry = dy;

    nx = dydx/sqrt(1.+dydx*dydx);
    ny = -1. /sqrt(1.+dydx*dydx);

    rho = drx*nx+dry*ny; 

    chi2x = dy*dy*100*50;
    chi2y = rho*rho/(dydx*dydx)*100; 
    chi2_russ[i] = chi2x + chi2y;

    printf("Russ Fit: i=%2i, drx = %10.3f dry = %10.3f rho = %10.3f ",
	   i, drx, dry, rho);
    printf("dydx = %10.3f chi2x=%10.3f chi2y = %10.3f  chi2 = %10.3f\n",
	   dydx, chi2x, chi2y, chi2_russ[i]);
  }


  return 0;
}


//-----------------------------------------------------------------------------
int TRussFitAlg::Display() {

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
