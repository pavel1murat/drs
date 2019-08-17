#include "TMath.h"

#include "drs/obj/TDrsFitResults.hh"
#include "drs/ana/TStripLineFit.hh"
#include "drs/base/TDrsUtils.hh"

#include "drs/obj/TTofData.hh"
#include "drs/obj/TDrsGlobals.hh"

#include "TF1.h"
#include "TCanvas.h"
#include "TPaveLabel.h"
#include "TMinuit.h"

#include "murat/plot/smooth.hh"

ClassImp(TStripLineFit)

//-----------------------------------------------------------------------------
double TStripLineFit::PulseShape(double* x, double* par) {
  Double_t f(0), dx;
 
  //Smooth the first Channel and extract the function.
  
  dx = x[0]-par[0];
  f  = smooth::gVar->func->Eval(dx);
  return f;
}

//-----------------------------------------------------------------------------
void TStripLineFit::FCN(Int_t&    NPar , 
			 Double_t* Gin  , 
			 Double_t& F    , 
			 Double_t* Par  , 
			 Int_t     IFlag) {
  
  int    ich, min_bin, max_bin;
  double chi2 = 0;
  double t, v, err, dchi2, f, qp;
  float  *wt;

  TTofData*       data;
  TDrsFitResults* res;
//-----------------------------------------------------------------------------
// use bins from 1 to maxbin in the fit
// maxbin is defined by the pulse intersecting fMaxFraction(default = 0.6) 
// of its height
//-----------------------------------------------------------------------------
  TH1*   hist;
  
  data   = TDrsFitAlgNew::fgFitData;
  res    = TDrsFitAlgNew::fgFitResults;

  ich    = data->GetChannelNumber();

  hist   = data->GetHistShape();
//-----------------------------------------------------------------------------
// fit uses bins from fStartBin to fMaxBin
//-----------------------------------------------------------------------------
  min_bin = res->fMinBin-2;
  max_bin = res->fMaxBin;
  qp      = 0;
  
  for (int i=min_bin; i<=max_bin; i++) {
    v    = hist->GetBinContent(i);

    err  = 0.005+0.02*sqrt(fabs(v));   // d??? rsana::fFitHist->GetBinError  (i);
 					// determine local x - coordinate along the chamber
    t     = hist->GetBinCenter(i);
    f     = TStripLineFit::PulseShape(&t,Par);
    dchi2 = (f-v)*(f-v)/(err*err);
    chi2 += dchi2;
    qp   += 1.;

//     if (drsana::fgInstance->fDebug[0] != 0) {
//       printf("i,v,f,err,dchi2,chi2 = %3i %10.5f %10.5f %10.5f %10.5f %10.5f\n",
// 	     i,v,f,err,dchi2,chi2);
//     }
  NEXT_BIN: ;
  }
  
  F = chi2/(qp-2.999);
}



//-----------------------------------------------------------------------------
TStripLineFit::TStripLineFit(const char* Name, const char* Title, int NChannels) : 
  TDrsFitAlgNew(Name, Title, NChannels) {

  fLevel   = 0.07;
  fFitMode = 1;

  TDrsFitResults  *res;

  fNPar   = 1;
  fMinuit = new TMinuit();
  fMinuit->SetFCN(TStripLineFit::FCN);

  int ich = 1; // Data->GetChannelID()->GetNumber();

  for (int i=0; i<NChannels; i++) {
    SetMinMaxFractions(i,0.02,0.5);
    res       = new TDrsFitResults(i);
    res->fFun = new TF1(Form("f_gaus_ch_%03i",i),TStripLineFit::PulseShape,0,1024,fNPar);
    fListOfFitResults->AddAt(res,i);
  }
  
  SetMinQ1(420.);
  SetMaxQ1(600.);
}

//-----------------------------------------------------------------------------
TStripLineFit::~TStripLineFit() {
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
double TStripLineFit::T0(TDrsFitResults* Res, double Level) {

  double       t0;
    
  t0   = Res->fPar[0];
  
  return t0;
}


//-----------------------------------------------------------------------------
// assuming the signal shape has its maximum at 1
// search for t0 at 'Level', regardless to the fPar[0] value returned by the fit
// this is why the answer depends on 2*ln(fPar[0]/Level) 
//-----------------------------------------------------------------------------
double TStripLineFit::T0CFD(TDrsFitResults* Res, double Level) {

  double       t0;
    
  t0 = Res->fPar[0];
  
  return t0;
}


//-----------------------------------------------------------------------------
double TStripLineFit::Dt(TDrsFitResults* R1, TDrsFitResults* R2, double Level) {

  double       dtg;
    
  dtg = R1->fPar[0]-R2->fPar[0];

  return dtg;
}

//-----------------------------------------------------------------------------
// emulate Constant Fraction Discriminator (CFD)
// calculate T0 at a certain level , as the original fitted distribution 
// was normalized to 100, Level=10 means 10%
//-----------------------------------------------------------------------------
double       TStripLineFit::DtCFD(TDrsFitResults* R1, TDrsFitResults* R2, double Level) {
  double t1l, t2l, dtl;
  
  t1l = R1->fPar[0];
  t2l = R2->fPar[0];

  dtl = (t1l-t2l); 		// in channels
  return dtl;
}

//-----------------------------------------------------------------------------
int TStripLineFit::BookHistograms() {

  int   ich1, ich2, ich, nch;
  TDrsChannel* ch;
  char    name[200];

  Hist_t*  Hist = &fHist;

  HBook1F(Hist->fDt[0],"dt_0","Delta(T)[0]",500,-5,5);
  HBook1F(Hist->fDt[1],"dt_1","Delta(T)[1]",500,-5,5);

  return 0;
}

//-----------------------------------------------------------------------------
int TStripLineFit::FillHistograms() {

  //  float level = 0.07;

  double           q1,  q2, ph1, ph2, chi2_1, chi2_2;
  double           dt;
  TDrsFitResults  *res, *r1,  *r2;
  int              ich1, ich2, igr1, igr2, idfc, good_event;

  TVme1742DataBlock* vme;

  Hist_t*  Hist = &fHist;

  ich1 = 20; // GetTimeChannelNumber(0);
  ich2 = 21; // GetTimeChannelNumber(1);

  r1  = GetFitResults(ich1);
  r2  = GetFitResults(ich2);

  dt = (r2->fPar[0]);

//   if ((dt < 0.1) || (dt > -1110.7)) {

//     printf(" >>>> TStripLineFit entry = %i   dt = %f\n",
// 	   TDrsGlobals::Instance()->GetEntry(),
// 	   dt);
//   }

  Hist->fDt[0]->Fill(dt);

  TTofData* dt2 = r2->GetTofData();
				// select the photopeak

  if ((dt2->GetQ1() > 420) && ( dt2->GetQ1() < 600)) {
    Hist->fDt[1]->Fill(dt);
  }

  return 0;
}


//-----------------------------------------------------------------------------
// Gaussian fit
//-----------------------------------------------------------------------------
int TStripLineFit::Fit(const TTofData* Data) { 

  int     max_bin, min_bin, found(0);
  int     ierflg, ich, imin, imax; 
  double  arglist[10], grad[10], vmin, slope, mean, chi2, vmax;

  ich = Data->GetChannelID()->GetNumber();

  TDrsFitResults* res = (TDrsFitResults*) fListOfFitResults->At(ich);

  res->fTofData = (TTofData*) Data;
  
  vmax = Data->GetV1Max();
					// determine the fit range
  int ncells = Data->GetNCells();
//-----------------------------------------------------------------------------
// now move in the opposite direction to find the 2% level
//-----------------------------------------------------------------------------
  max_bin = Data->GetMax1Cell();

  min_bin = max_bin;
  for (int i=max_bin; i>=0; i--) {
    if (Data->GetV1(i) > vmax*fMaxFraction[ich]) {
      max_bin = i;
    }
    if (Data->GetV1(i) > vmax*fMinFraction[ich]) { // 2.) { 1=136.2
      min_bin = i;
    }
    else {
      break;
    }
  }
    
  if (max_bin < 0   ) max_bin=0;
  if (max_bin > 1023) max_bin=1023;
//-----------------------------------------------------------------------------
// fit range is defined, fit shape with the gaussian
//-----------------------------------------------------------------------------
  // min_bin      = min_bin-40;
  // if (min_bin < 1) min_bin = 1;

  imin = min_bin-60;
  if (imin < 1) imin=1;
  imax = min_bin-10;
  if (imax < 1) imax=1;

  TDrsUtils::FitPol0(Data->GetV1(),imin,imax,&mean,&chi2);
  res->fBaseOffset = mean;
  res->fBaseChi2   = chi2/(imax-imin+1.e-12);

  res->fMinBin = min_bin;
  res->fMaxBin = max_bin;

  Data->fHistShape->GetXaxis()->SetRangeUser(0.,1024.);

  //  TF1* f = res->fFun;
//-----------------------------------------------------------------------------
// Minuit 
//-----------------------------------------------------------------------------
  TDrsFitAlgNew::fgFitData    = (TTofData*) Data;
  TDrsFitAlgNew::fgFitResults = res;

  if (fFitMode == 2) {
//-----------------------------------------------------------------------------
// use Minuit
//-----------------------------------------------------------------------------
    arglist[0]  = 1;
    fMinuit->mnexcm("SET ERR", arglist ,1,ierflg);
//-----------------------------------------------------------------------------
// Set starting values and step sizes for parameters and do minimization
//-----------------------------------------------------------------------------

//    fMinuit->mnparm(0, "p0"   , max_bin  , 0.1, max_bin-10 , max_bin+10 , ierflg);
    fMinuit->mnparm(0, "p0", 0, 0.001, -5, 5, ierflg);

    arglist[0] = 1000;
    arglist[1] = 1.;

    fMinuit->mnexcm("MIGRAD", arglist ,2,ierflg);

    for (int ip=0; ip<fNPar; ip++) {
      fMinuit->GetParameter(ip,res->fPar[ip],res->fSig[ip]);
      res->fFun->SetParameter(ip,res->fPar[ip]);
    }

    fMinuit->Eval(fNPar,grad,chi2,res->fPar,3);
    res->fChi2 = chi2/(max_bin-min_bin-fNPar+1.e-12);
  }
  else if (fFitMode == 1) {

    //res->fFun->SetParameter(0,max_bin);
    res->fFun->SetParameter(0,0);
    //  f->SetParError(2,0.);
//-----------------------------------------------------------------------------
// this is where the fit happens
///-----------------------------------------------------------------------------
//  Data->fHistShape->Fit(res->fFun,"w0q","",min_bin-0.5,max_bin+0.5);
//    Data->fHistShape->Fit(res->fFun,"w0q","",min_bin-1.5,max_bin);
//    Data->fHistShape->Fit(res->fFun,"w0q","",min_bin-2.5,max_bin+0.5);
    Data->fHistShape->Fit(res->fFun,"w0q","",min_bin-0.5,max_bin);
  
  //  TF1* f_fit = Data->fHistShape->GetFunction(f->GetName());

    for (int i=0; i<fNPar; i++) {
      res->fPar[i] = res->fFun->GetParameter(i);
      res->fSig[i] = res->fFun->GetParError (i);
    }
    res->fChi2 = res->fFun->GetChisquare()/(res->fFun->GetNumberFitPoints()-fNPar+0.0001);
  }

  res->fT0Fit = T0(res,fLevel);

  return 0;
}

//-----------------------------------------------------------------------------
int TStripLineFit::Display() {

  if (fCanvas == 0) {
    fCanvas = new TCanvas("avg_gaus_shape_display","avg_gaus_shape_display",1000,800);
  }

  float x[1024], y[1024], ex[1024], ey[1024];

  int            ich1, ich2, ich, np, ichan;
  TTofData       *dt1, *dt2;
  TDrsFitResults *r1 , *r2;
//-----------------------------------------------------------------------------
// used points, first channel
//-----------------------------------------------------------------------------
  fCanvas->cd();

  ich1 = 20; // fTimeChannelNumber[0];
  ich2 = 21; // fTimeChannelNumber[1];

  r1  = GetFitResults(ich1);
  r2  = GetFitResults(ich2);

  dt1 = r1->GetTofData();
  dt2 = r2->GetTofData();

  double xmin, xmax;

 //  xmin = r1->fPar[2]-10; // 50;
//   if (xmin < 0) xmin = 0;
//   xmax = r1->fPar[2]+40; // 350;
//   if (xmax > 1024) xmax = 1024;

  //  fHist.fShape[ich1]->GetXaxis()->SetRangeUser(xmin,xmax);

  dt1->GetHistShape()->SetLineColor(2);
  dt1->GetHistShape()->Draw();
  dt2->GetHistShape()->Draw("same");

//   TF1* f1 = r1->fFun;

//   f1->SetLineColor(2);
//   f1->SetLineWidth(1);
//   f1->GetXaxis()->SetRangeUser(r1->fMinBin-2.5,r1->fMaxBin+0.5);
//   f1->Draw("same");

  TF1* f2 = r2->fFun;

  f2->SetLineWidth(1);
  f2->GetXaxis()->SetRangeUser(r2->fMinBin-2.5,r2->fMaxBin+0.5);
  f2->Draw("same");

  TPaveLabel  *l1; 

  TDrsUtils::DrawPaveLabelNDC(l1,Form("Entry: %6i ; q1, q2 = %7.1f %7.1f, dt_cfd(0.1)=%8.4f dtg=%8.4f",
				      TDrsGlobals::Instance()->GetEntry(), 
				      dt1->GetQ1(),
				      dt2->GetQ1(), 
				      T0CFD(r1,0.1)-T0CFD(r2,0.1),
				      Dt(r1,r2,fLevel)),
				      0.1,0.86,0.7,0.90);
  return 0;
}
