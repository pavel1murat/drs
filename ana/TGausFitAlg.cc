//
#include "TMath.h"

#include "drs/obj/TDrsFitResults.hh"
#include "drs/ana/TGausFitAlg.hh"
#include "drs/obj/TDrsChannel.hh"
#include "drs/base/TDrsUtils.hh"

#include "TCanvas.h"
#include "TPaveLabel.h"
#include "TMinuit.h"

#include "drs/ana/drsana.hh"

ClassImp(TGausFitAlg)

//-----------------------------------------------------------------------------
double TGausFitAlg::PulseShape(double* x, double* par) {
  double f, dx;

  dx = (x[0]-par[1])/par[2];
  //  dx = (x[0]-par[1])/3.964;

  //  f = par[0]*TMath::Exp(-dx*dx/2.)+par[3];
  f = par[0]*TMath::Exp(-dx*dx/2.);

  return f;
}

//_____________________________________________________________________________
void TGausFitAlg::FCN(Int_t&    NPar , 
		      Double_t* Gin  , 
		      Double_t& F    , 
		      Double_t* Par  , 
		      Int_t     IFlag) {

  int    ich, min_bin, max_bin;
  double chi2 = 0;
  double t, v, err, dchi2, f, qp;
  float  *wt;

  TDrsChannel*  ch;
  TDrsFitResults* res;
//-----------------------------------------------------------------------------
// use bins from 1 to maxbin in the fit
// maxbin is defined by the pulse intersecting fMaxFraction(default = 0.6) 
// of its height
//-----------------------------------------------------------------------------
  TH1*   hist;
  
  ch     = TDrsFitAlg::fgFitChannel;
  res    = TDrsFitAlg::fgFitResults;

  ich    = ch->GetNumber();

  hist   = ch->fHist.fShape;
//-----------------------------------------------------------------------------
// fit uses bins from fStartBin to fMaxBin
//-----------------------------------------------------------------------------
  min_bin = res->fMinBin;
  max_bin = res->fMaxBin;
  wt      = ch->fWt;
  qp      = 0;
  
  for (int i=min_bin; i<=max_bin; i++) {
    v    = hist->GetBinContent(i);

    if (wt[i] == 0) goto NEXT_BIN;
    
    err  = 1.; // +0.5*fabs(v); // drsana::fFitHist->GetBinError  (i);
					// determine local x - coordinate along the chamber
    t     = hist->GetBinCenter(i);
    f     = TGausFitAlg::PulseShape(&t,Par);
    dchi2 = (f-v)*(f-v)/(err*err);
    chi2 += dchi2;
    qp   += 1.;

    if (drsana::fgInstance->fDebug[0] != 0) {
      printf("i,v,f,err,dchi2,chi2 = %3i %10.5f %10.5f %10.5f %10.5f %10.5f\n",
	     i,v,f,err,dchi2,chi2);
    }
  NEXT_BIN: ;
  }
  
  F = chi2/(qp-2.999);
}



//-----------------------------------------------------------------------------
TGausFitAlg::TGausFitAlg(const char* Name, const char* Title, TObjArray* ListOfChannels) : 
TDrsFitAlg(Name, Title, ListOfChannels) {

  fLevel          = 0.07;

  TDrsChannel*  ch;

  TDrsFitResults  *res;

  fNPar   = 3;
  fMinuit = new TMinuit();
  fMinuit->SetFCN(TGausFitAlg::FCN);

  fChi2Max[0] = 30.;
  fChi2Max[1] = 30.;

  for (int i=0; i<fNChannels; i++) {
    ch = (TDrsChannel*) fCachedListOfChannels->At(i);
    if (ch) {
      SetMinMaxFractions(i,0.02,0.5); 

      res = new TDrsFitResults(ch);
      fListOfFitResults->AddAt(res,i);
      
      res->fFun = new TF1(Form("f_gaus_ch_%03i",i),TGausFitAlg::PulseShape,0,1024,fNPar);
    }
  }
}

//-----------------------------------------------------------------------------
TGausFitAlg::~TGausFitAlg() {
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
double TGausFitAlg::T0(TDrsFitResults* Res, double Level) {

  double       t0, c1;
    
  c1 = sqrt(2*TMath::Log(Res->fPar[0]/Level));
  t0 = Res->fPar[1]-(c1+1/c1)*fabs(Res->fPar[2]);
  
  return t0;
}


//-----------------------------------------------------------------------------
// assuming the signal shapes has its maximum at 1
// search for t0 at 'Level'
//-----------------------------------------------------------------------------
double TGausFitAlg::T0CFD(TDrsFitResults* Res, double Level) {

  double       t0, c1;
    
  c1 = sqrt(2*TMath::Log(Res->fPar[0]/Level));
  t0 = Res->fPar[1]-c1*fabs(Res->fPar[2]);
  
  return t0;
}


//-----------------------------------------------------------------------------
double TGausFitAlg::Dt(TDrsFitResults* R1, TDrsFitResults* R2, double Level) {

  double       t1g, t2g, dtg, c1, c2;
    
  double   k(1.0); // by default: 1

  c1 = sqrt(2*TMath::Log(R1->fPar[0]/Level));
  c2 = sqrt(2*TMath::Log(R2->fPar[0]/Level));
  
  t1g = R1->fPar[1]-k*(c1+1./c1)*fabs(R1->fPar[2]);
  t2g = R2->fPar[1]-k*(c2+1./c2)*fabs(R2->fPar[2]);
  
  dtg = (t1g-t2g); 		// in units of channels

  return dtg;
}

//-----------------------------------------------------------------------------
// emulate Constant Fraction Discriminator (CFD)
// calculate T0 at a certain level , as the original fitted distribution 
// was normalized to 100, Level=10 means 10%
//-----------------------------------------------------------------------------
double       TGausFitAlg::DtCFD(TDrsFitResults* R1, TDrsFitResults* R2, double Level) {
  double t1l, t2l, dtl, c1, c2;

  TDrsChannel  *ch1, *ch2;
    
  c1 = sqrt(2*TMath::Log(R1->fPar[0]/Level));
  c2 = sqrt(2*TMath::Log(R2->fPar[0]/Level));

  t1l = R1->fPar[1]-c1*fabs(R1->fPar[2]);
  t2l = R2->fPar[1]-c2*fabs(R2->fPar[2]);

  dtl = (t1l-t2l); 		// in channels
  return dtl;
}

//-----------------------------------------------------------------------------
int TGausFitAlg::BookHistograms() {

  int   ich1, ich2, ich, nch;
  TDrsChannel* ch;
  char    name[200];

  Hist_t*  Hist = &fHist;

  HBook1F(Hist->fDt [0],"dt0","Delta(T)[0] GAUS (ns)",500,-5.,5.);
  HBook1F(Hist->fDt [1],"dt1","Delta(T)[1] GAUS (ns)",500,-5.,5.);
  HBook1F(Hist->fDt [2],"dt2","Delta(T)[2] GAUS (ns)",500,-5.,5.);
  HBook1F(Hist->fDt [3],"dt3","Delta(T)[3] GAUS (ns)",500,-5.,5.);
  HBook1F(Hist->fDt [4],"dt4","Delta(T)[4] GAUS (ns)",500,-5.,5.);
  HBook1F(Hist->fDt [5],"dt5","Delta(T)[5] GAUS (ns) [chi2<5]",500,-5.,5.);
  HBook1F(Hist->fDt [6],"dt6","Delta(T)[6] GAUS (ns) [chi2<3]",500,-5.,5.);
  HBook1F(Hist->fDt [7],"dt7","Delta(T)[7] GAUS (ns) [chi2<2]",500,-5.,5.);
  HBook1F(Hist->fDt [8],"dt8","Delta(T)[8] GAUS (ns) - DTM3"  ,500,-5.,5.);

					// 10% CFD histograms

  HBook1F(Hist->fDtl[0],"dtl_0","Delta(TL)[0] (ns)",500,-5.,5.);
  HBook1F(Hist->fDtl[1],"dtl_1","Delta(TL)[1] (ns)",500,-5.,5.);
  HBook1F(Hist->fDtl[2],"dtl_2","Delta(TL)[2] (ns)",500,-5.,5.);
  HBook1F(Hist->fDtl[5],"dtl_5","Delta(TL)[5] (ns)",500,-5.,5.);
  HBook1F(Hist->fDtl[6],"dtl_6","Delta(TL)[6] (ns)",500,-5.,5.);
  HBook1F(Hist->fDtl[7],"dtl_7","Delta(TL)[7] (ns)",500,-5.,5.);

  nch = fCachedListOfChannels->GetLast()+1;
  for (int i=0; i<nch; i++) {
    ch = GetChannel(i);
    if (ch) {
      ich = ch->GetNumber();

      sprintf(name,"chi2_%02i",i);
      HBook1F(Hist->fChi2[i],name,name,200,0,100);

      sprintf(name,"dtnorm_%02i",i);
      HBook1F(Hist->fDtNorm[i],name,name,100,-0,1);

      sprintf(name,"shape_%0i",i);
      HBook2F(Hist->fShape[i],name,name,1024,0,1024,100,0,1);

      sprintf(name,"gsigma_%0i",i);
      HBook1F(Hist->fGSigma[i],name,name,200,-25,25);

      sprintf(name,"fit_base_%0i",i);
      HBook1F(Hist->fFitBase[i],name,name,100,-0.5,0.5);
    }
  }

  HBook2F(Hist->fDtVsPH1,"dtg_vs_ph1","Dtg(2) vs PH(1)" ,250,0,500,250,-5.,5.);
  HBook2F(Hist->fDtVsPH2,"dtg_vs_ph2","Dtg(2) vs PH(2)", 250,0,500,250,-5.,5.);
  HBook2F(Hist->fDtVsT0 ,"dtg_vs_t0" ,"Dtg(2) vs T0"   ,1000,0,200,250,-5.,5.);

  HBook2F(Hist->fDtVsSig1,"dtg_vs_sig1" ,"Dtg vs sig1",100,-10,10,250,-5,5);
  HBook2F(Hist->fDtVsSig2,"dtg_vs_sig2" ,"Dtg vs sig2",100,-10,10,250,-5,5);

  HBook2F(Hist->fDt3VsSig1 ,"dtg3_vs_sig1" ,"Dtg3 vs sig1",100,0,10,250,-5,5);
  HBook2F(Hist->fDt3VsSig2 ,"dtg3_vs_sig2" ,"Dtg3 vs sig2",100,0,10,250,-5,5);
  HBook2F(Hist->fDt3VsSig12,"dtg3_vs_sig12" ,"Dtg3 vs sig1-sig2",100,-10,10,250,-5,5);

  HBook2F(Hist->fDt4VsSig1 ,"dtg4_vs_sig1"  ,"Dtg4 vs sig1",100,0,10,250,-5.,5.);
  HBook2F(Hist->fDt4VsSig2 ,"dtg4_vs_sig2"  ,"Dtg4 vs sig2",100,0,10,250,-5,5);
  HBook2F(Hist->fDt4VsSig12,"dtg4_vs_sig12" ,"Dtg4 vs sig1-sig2",100,-10,10,250,-5,5);

  return 0;
}

//-----------------------------------------------------------------------------
int TGausFitAlg::FillHistograms() {

  //  float level = 0.07;

  double           q11,  q12, ph1, ph2, chi2_1, chi2_2;
  TDrsChannel     *ch, *ch1, *ch2;
  TDrsFitResults  *res, *r1,  *r2;
  int              ich1, ich2;

  Hist_t*  Hist = &fHist;

  ch1 = GetUsedChannel(0);
  ch2 = GetUsedChannel(1);

  ich1 = ch1->GetNumber();
  ich2 = ch2->GetNumber();

  r1  = GetFitResults(ch1->GetNumber());
  r2  = GetFitResults(ch2->GetNumber());

					// pedestal-subtracted...
  ph1  = ch1->fVMax;
  ph2  = ch2->fVMax;

  q11   = ch1->fQ1;
  q12   = ch2->fQ1;

  chi2_1 = r1->fChi2*ph1*ph1;
  chi2_2 = r2->fChi2*ph2*ph2;

  fDt  = Dt   (r1,r2,fLevel)/5. ; 		// in 200ps/chanel
  fDtl = DtCFD(r1,r2,fLevel)/5. ; 		// in ns

  fT0  = (T0(r1,fLevel)+T0(r2,fLevel))/2.; 

  //    if ((ph1 < fMinPulse[ich1]) || (ph2 < fMinPulse[ich2])) {

  fHist.fDt [0]->Fill(fDt );
  fHist.fDtl[0]->Fill(fDtl);

  if ((q11 < ch1->fMinQ) || (q12 < ch2->fMinQ)) {
    fHist.fDt [1]->Fill(fDt );
    fHist.fDtl[1]->Fill(fDtl);
  }
  else {
//-----------------------------------------------------------------------------
// both signals are large enough
//-----------------------------------------------------------------------------
    fHist.fDt [2]->Fill(fDt );
    fHist.fDtl[2]->Fill(fDtl);

    fHist.fChi2[ich1]->Fill(chi2_1);
    fHist.fChi2[ich2]->Fill(chi2_2);

    fHist.fDtVsPH1->Fill(ph1,fDt);
    fHist.fDtVsPH2->Fill(ph2,fDt);
    fHist.fDtVsT0 ->Fill(fT0,fDt);
    
    fHist.fDtVsSig1->Fill(r1->fPar[2],fDt);
    fHist.fDtVsSig2->Fill(r2->fPar[2],fDt);

					// an attempt to correct the fit result

    //    double dtg3 = fDt-0.0014*(ph1-250)+0.0008*(ph2-300.);

    double dtg3 = fDt-0.001*(ph1-250)+0.001*(ph2-300.);

    fHist.fDt[3]->Fill(dtg3);
    
    double sig12 = r1->fPar[2]-r2->fPar[2];

    fHist.fDt3VsSig1->Fill(r1->fPar[2],dtg3);
    fHist.fDt3VsSig2->Fill(r2->fPar[2],dtg3);
    fHist.fDt3VsSig12->Fill(sig12,dtg3);

					// dtg3/sig1

    double dtg4 = fDt+0.03*(r1->fPar[2]-4)-0.03*(r2->fPar[2]-4);

    fHist.fDt[4]->Fill(dtg4);

    fHist.fDt4VsSig1->Fill(r1->fPar[2],dtg4);
    fHist.fDt4VsSig2->Fill(r2->fPar[2],dtg4);
    fHist.fDt4VsSig12->Fill(sig12,dtg4);

    if ((chi2_1 < 50) && (chi2_2 < 50)) {
      fHist.fDt[5]->Fill(fDt);
      fHist.fDtl[5]->Fill(fDtl);
    }

    if ((chi2_1 < fChi2Max[0]) && (chi2_2 < fChi2Max[1])) {
      fHist.fDt[6]->Fill(fDt);
      fHist.fDtl[6]->Fill(fDtl);
    }

    if ((chi2_1 < 20) && (chi2_2 < 20)) {
      fHist.fDt[7]->Fill(fDt);
      fHist.fDtl[7]->Fill(fDtl);
    }

    if ((fDt > 0.06) && (fDt < 0.1)) {
      printf(">>> event : %5i  fDt = %10.3f \n", fEventNumber,fDt);
    }
//-----------------------------------------------------------------------------
// also fill histograms for all channels used
//-----------------------------------------------------------------------------
    double t0, t0n, y; 
    int    it0, ich;

    for (int i=0; i<fNChannels; i++) {
      ch = (TDrsChannel*) fCachedListOfChannels->At(i);
      if (ch) {
	res = (TDrsFitResults*) fListOfFitResults->At(i);

	t0  = T0(res,fLevel);
	t0n = t0-((int) t0);
	fHist.fDtNorm [i]->Fill(t0n);
	fHist.fGSigma [i]->Fill(res->fPar[2]);
	fHist.fFitBase[i]->Fill(res->fPar[3]);
	
	it0 = ((int) t0)-10;
	if (it0 < 0) it0 = 0;

	for (int icell=it0; icell<kNCells; icell++) {
	  y = ch->GetHist()->fShape->GetBinContent(icell+1);
	  fHist.fShape[i]->Fill(icell-it0,y);
	}
      }
    }
  }

  return 0;
}


//-----------------------------------------------------------------------------
// Gaussian fit
//-----------------------------------------------------------------------------
int TGausFitAlg::Fit(const TDrsChannel* Channel) { 

  int max_bin, min_bin, found(0);
  int     ierflg; 
  double  arglist[10], grad[10], vmin, slope;

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
    if (Channel->fV[i] > vmax*fMaxFraction[ich]) {
      max_bin = i+1;
    }
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
// fit range is defined, fit shape with the gaussian
//-----------------------------------------------------------------------------
  // min_bin      = min_bin-40;
  // if (min_bin < 1) min_bin = 1;

  double mean, chi2;
  int    imin, imax;

  imin = min_bin-60;
  if (imin < 1) imin=1;
  imax = min_bin-10;
  if (imax < 1) imax=1;

  TDrsUtils::FitPol0(Channel->fV,imin,imax,&mean,&chi2);
  res->fBaseOffset = mean;
  res->fBaseChi2   = chi2/(imax-imin+1.e-12);

  res->fMinBin = min_bin;
  res->fMaxBin = max_bin;

  Channel->fHist.fShape->GetXaxis()->SetRangeUser(0.,1024.);

  TF1* f = res->fFun;
//-----------------------------------------------------------------------------
// Minuit 
//-----------------------------------------------------------------------------
  TDrsFitAlg::fgFitChannel = (TDrsChannel*) Channel;
  TDrsFitAlg::fgFitResults = res;

  /*
  arglist[0]  = 1;
  fMinuit->mnexcm("SET ERR", arglist ,1,ierflg);
//-----------------------------------------------------------------------------
// Set starting values and step sizes for parameters and do minimization
//-----------------------------------------------------------------------------
  fMinuit->mnparm(0, "p0"   , 0.8    , 0.01, 0.6        , 2        , ierflg);
  fMinuit->mnparm(1, "p1"   , max_bin, 0.1 , max_bin-20, max_bin+20, ierflg);
  fMinuit->mnparm(2, "p2"   , 3.     , 0.1 , 0.        , 10.       , ierflg);

  //  fMinuit->mnparm(3, "p3"   , 0.     , 0.1 , -3.        , 3.       , ierflg);

  arglist[0] = 1000;
  arglist[1] = 1.;
  fMinuit->mnexcm("MIGRAD", arglist ,2,ierflg);

  for (int ip=0; ip<fNPar; ip++) {
    fMinuit->GetParameter(ip,res->fPar[ip],res->fSig[ip]);
  }

  fMinuit->Eval(fNPar,grad,res->fChi2,res->fPar,3);

  for (int i=0; i<fNPar; i++) {
    f->SetParameter(i,res->fPar[i]);
  }
  */

  f->SetParameter(0,0.7);
  f->SetParameter(1,max_bin);
  f->SetParameter(2,3.964);
  f->SetParLimits(2,1,10);
  //  f->SetParError(2,0.);

  if (fNPar > 3) {
    f->SetParameter(3,0.01);
  }

  Channel->fHist.fShape->Fit(f,"w0q","",min_bin-0.5,max_bin+0.5);
  // Channel->fHist.fShape->Fit(f,"w0q","",min_bin-2.5,max_bin+0.5);
  
  TF1* f_fit = Channel->fHist.fShape->GetFunction(f->GetName());

  for (int i=0; i<fNPar; i++) {
    res->fPar[i] = f_fit->GetParameter(i);
    res->fSig[i] = f_fit->GetParError (i);
  }
  res->fChi2 = f_fit->GetChisquare()/(f_fit->GetNumberFitPoints()-fNPar+0.0001);

  return 0;
}

//-----------------------------------------------------------------------------
int TGausFitAlg::Display() {

  if (fCanvas == 0) {
    fCanvas = new TCanvas("gaus_shape_display","gaus_shape_display",1000,800);
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

  TF1* f1 = r1->fFun;

  f1->SetLineColor(2);
  f1->SetLineWidth(1);
  f1->GetXaxis()->SetRangeUser(r1->fMinBin-0.5,r1->fMaxBin+0.5);
  f1->Draw("same");

  TF1* f2 = r2->fFun;

  f2->SetLineWidth(1);
  f2->GetXaxis()->SetRangeUser(r2->fMinBin-0.5,r2->fMaxBin+0.5);
  f2->Draw("same");

  TPaveLabel  *l1; 

  TDrsUtils::DrawPaveLabelNDC(l1,Form("Entry: %6i ; q1, q2, dte = %7.1f %7.1f %8.4f",
				      drsana::fgInstance->fEntry, 
				      ch1->fQ,
				      ch2->fQ, 
				      T0CFD(r1,0.1)-T0CFD(r2,0.1)),
				      0.1,0.85,0.7,0.90);
  return 0;
}
