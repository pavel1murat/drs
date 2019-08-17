//
#include "TMath.h"

#include "drs/obj/TDrsFitResults.hh"
#include "drs/ana/TExp1FitAlgNew.hh"
#include "drs/base/TDrsUtils.hh"

#include "drs/obj/TTofData.hh"
#include "drs/obj/TDrsGlobals.hh"

#include "TF1.h"
#include "TCanvas.h"
#include "TPaveLabel.h"
#include "TMinuit.h"

ClassImp(TExp1FitAlgNew)

//-----------------------------------------------------------------------------
// pulse = A*(exp(B(T-T0)-1)
//-----------------------------------------------------------------------------
double TExp1FitAlgNew::PulseShape(double* x, double* par) {
  double f, dx;

  dx = (x[0]-par[2]);

  if (dx < 0)  f = 0;
  else         f = par[0]*(TMath::Exp(par[1]*dx)-1);

  return f;
}

//_____________________________________________________________________________
void TExp1FitAlgNew::FCN(Int_t&    NPar , 
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
    f     = TExp1FitAlgNew::PulseShape(&t,Par);
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
TExp1FitAlgNew::TExp1FitAlgNew(const char* Name, const char* Title, int NChannels) : 
  TDrsFitAlgNew(Name, Title, NChannels) {

  fLevel   = 0.07;
  fFitMode = 1;

  TDrsFitResults  *res;

  fNPar   = 3;
  fMinuit = new TMinuit();
  fMinuit->SetFCN(TExp1FitAlgNew::FCN);

  int ich = 1; // Data->GetChannelID()->GetNumber();

  for (int i=0; i<NChannels; i++) {
    SetMinMaxFractions(i,0.02,0.15);
    res       = new TDrsFitResults(i);
    res->fFun = new TF1(Form("f_exp1_ch_%03i",i),TExp1FitAlgNew::PulseShape,0,1024,fNPar);
    fListOfFitResults->AddAt(res,i);
  }

  for (int i=0; i<fNTrigChannels; i++) {
    SetMinMaxTrigFractions(i,0.02,0.15);
    res       = new TDrsFitResults(i);
    res->fFun = new TF1(Form("f_exp1_tr_%03i",i),TExp1FitAlgNew::PulseShape,0,1024,fNPar);
    fListOfTrigFitResults->AddAt(res,i);
  }
  
  SetMinMaxFractions(20,0.02,0.25);
  SetMinMaxFractions(21,0.02,0.25);

  SetMinQ1(420.);
  SetMaxQ1(600.);
}

//-----------------------------------------------------------------------------
TExp1FitAlgNew::~TExp1FitAlgNew() {
}


//----------------------------------------------------------------------------
// calculate derivative at a given 'Level', extrapolate down to zero to get the time stampb
// Fitted amplitudes are normalized
// to 100, however the fit parameters may be different
// 10% correspond to Level=10					// time fit histograms


// parameters of the exp1 fit: y = p0*(exp(p1*(x-p2))-1) for x > p2; (p2 = T0)
//                             y = 0 for x < p2
//
// returns delta_T in units of channels
// Level : 0.1, 0.2 etc...
//----------------------------------------------------------------------------- 
double TExp1FitAlgNew::T0(TDrsFitResults* Res, double Level) {

  double       p0, p1, t0, t2, x;
    
  p0   = Res->fPar[0];
  p1   = Res->fPar[1];
  t0   = Res->fPar[2];

  x    = Level/p0;

  t2   = t0 + (TMath::Log(1+x) - x/(1+x))/p1;
  
  return t2;
}


//-----------------------------------------------------------------------------
// assuming the signal shape has its maximum at 1
// search for t0 at 'Level', regardless to the fPar[0] value returned by the fit
// this is why the answer depends on 2*ln(fPar[0]/Level) 
//-----------------------------------------------------------------------------
double TExp1FitAlgNew::T0CFD(TDrsFitResults* Res, double Level) {

  double       p0, p1, t0, t1, x;
    
  p0   = Res->fPar[0];
  p1   = Res->fPar[1];
  t0   = Res->fPar[2];

  x    = Level/p0;

  t1   = t0 + TMath::Log(1+x)/p1; 
  
  return t1;
}


//-----------------------------------------------------------------------------
double TExp1FitAlgNew::Dt(TDrsFitResults* R1, TDrsFitResults* R2, double Level) {

  double       t1, t2, dt;
    
  t1 = T0(R1,Level);
  t2 = T0(R2,Level);
  
  dt = t1-t2;

  return dt;
}

//-----------------------------------------------------------------------------
// emulate Constant Fraction Discriminator (CFD)
// calculate T0 at a certain level , as the original fitted distribution 
// was normalized to 100, Level=10 means 10%
//-----------------------------------------------------------------------------
double       TExp1FitAlgNew::DtCFD(TDrsFitResults* R1, TDrsFitResults* R2, double Level) {
  double t1, t2, dt;

  t1 = T0CFD(R1,Level);
  t2 = T0CFD(R2,Level);

  dt  = t1-t2; 		// in channels
  return dt;
}

//-----------------------------------------------------------------------------
int TExp1FitAlgNew::BookHistograms() {

  int   ich1, ich2, ich, nch;
  TDrsChannel* ch;
  char    name[200];

  Hist_t*  Hist = &fHist;

  HBook1F(Hist->fDt [0],"dt0","Delta(T)[0] EXP1 (ns)",500,-5,5);
  HBook1F(Hist->fDt [1],"dt1","Delta(T)[1] EXP1 (ns)",500,-5,5);
  HBook1F(Hist->fDt [2],"dt2","Delta(T)[2] EXP1 (ns)",500,-5,5);
  HBook1F(Hist->fDt [3],"dt3","Delta(T)[3] EXP1 (ns)",500,-5,5);
  HBook1F(Hist->fDt [4],"dt4","Delta(T)[4] EXP1 (ns)",500,-5,5);
  HBook1F(Hist->fDt [5],"dt5","Delta(T)[5] EXP1 (ns) [chi2<5]",500,-5,5);
  HBook1F(Hist->fDt [6],"dt6","Delta(T)[6] EXP1 (ns) [chi2<3]",500,-5,5);
  HBook1F(Hist->fDt [7],"dt7","Delta(T)[7] EXP1 (ns) [chi2<2]",500,-5,5);
  HBook1F(Hist->fDt [8],"dt8","Delta(T)[8] EXP1 (ns) - DTM3"  ,500,-5,5);
  HBook1F(Hist->fDt2021,"dt_20_21","Delta(T)20-21 EXP1 (ns)"  ,500,-5,5);
  HBook1F(Hist->fDtSL  ,"dt_sl","Delta(T) SL (ns)"  ,1000,-10,10);

					// 10% CFD histograms

  HBook1F(Hist->fDtl[0],"dtl_0","Delta(TL)[0] (ns)",500,-5,5);
  HBook1F(Hist->fDtl[1],"dtl_1","Delta(TL)[1] (ns)",500,-5,5);
  HBook1F(Hist->fDtl[2],"dtl_2","Delta(TL)[2] (ns)",500,-5,5);
  HBook1F(Hist->fDtl[3],"dtl_3","Delta(TL)[3] (ns)",500,-5,5);
  HBook1F(Hist->fDtl[4],"dtl_4","Delta(TL)[4] (ns)",500,-5,5);
  HBook1F(Hist->fDtl[5],"dtl_5","Delta(TL)[5] (ns)",500,-5,5);
  HBook1F(Hist->fDtl[6],"dtl_6","Delta(TL)[6] (ns)",500,-5,5);
  HBook1F(Hist->fDtl[7],"dtl_7","Delta(TL)[7] (ns)",500,-5,5);

  for (int i=0; i<fNChannels; i++) {
					// ich is either -1 or the channel number
    ich = fTimeChannelNumber[i];

    if (ich >= 0) {
      sprintf(name,"chi2_%02i",i);
      HBook1F(Hist->fChi2[i],name,name,100,0,20);

      sprintf(name,"npt_%02i",i);
      HBook1F(Hist->fNFitPoints[i],name,name,50,0,50);

      sprintf(name,"dtnorm_%02i",i);
      HBook1F(Hist->fDtNorm[i],name,name,100,-0,1);

      sprintf(name,"shape_%0i",i);
      HBook2F(Hist->fShape[i],name,name,1024,0,1024,100,0,1);

      sprintf(name,"p0_%0i",i);
      HBook1F(Hist->fP0[i],name,name,500,0,0.2);

      sprintf(name,"p1_%0i",i);
      HBook1F(Hist->fP1[i],name,name,200,0,1.);

      sprintf(name,"p2_%0i",i);
      HBook1F(Hist->fP2[i],name,name,500,0,500);

      sprintf(name,"dt_chan_%0i",i);
      HBook1F(Hist->fDtChan[i],name,name,500,-5,5);

      sprintf(name,"t0_fit_%0i",i);
      HBook1F(Hist->fT0Fit[i],name,name,500,0,500);

      sprintf(name,"dt_fit_%0i",i);
      HBook1F(Hist->fDtFit[i],name,name,500,-125,125);

      sprintf(name,"ave_pulse_%02i",i);
      HBook2F(Hist->fAvePulse[i],name,name,1024,0,1024,700,-20,680);

      sprintf(name,"ave_shape_%0i",i);
      HBook2F(Hist->fAveShape[i],name,name,1024,0,1024,110,0,1.1);
    }
  }

  HBook2F(Hist->fDtVsPH1  ,"dt_vs_ph1","Dt(2) vs PH(1)" ,250,0,500,250,-5,5);
  HBook2F(Hist->fDtVsPH2  ,"dt_vs_ph2","Dt(2) vs PH(2)", 250,0,500,250,-5,5);
  HBook2F(Hist->fDtVsT0   ,"dt_vs_t0" ,"Dt(2) vs T0"   ,1000,0,200,250,-5,5);
  HBook2F(Hist->fDtlVsDP0 ,"dtl_vs_dp0","Dt(2) vs DP0"   ,250,-50,50,250,-5,5);

  HBook2F(Hist->fDt2VsEntry,"dt2_vs_entry","Dt2 Vs Entry",100,0,100,250,-5,5);

  HBook2F(Hist->fDt2VsDfc,"dt2_vs_dfc","Dt2 Vs D(first Cell)",50,-25,25,250,-5,5);

  HBook2F(Hist->fDt2VsIch1,"dt2_vs_ich1","Dt2 Vs Ich1",50,0,50,250,-5,5);
  HBook2F(Hist->fDt2VsIch2,"dt2_vs_ich2","Dt2 Vs Ich2",50,0,50,250,-5,5);

  return 0;
}

//-----------------------------------------------------------------------------
int TExp1FitAlgNew::FillHistograms() {

  //  float level = 0.07;

  double           q1,  q2, ph1, ph2, chi2_1, chi2_2, t_sl, dt2021, dt_sl, t16, t20, t21;
  TTofData        *dt, *data1, *data2, *data_20, *data_21;
  TDrsFitResults  *res, *r1,  *r2, *r20, *r21;
  int              ich1, ich2, igr1, igr2, idfc, good_event;

  TVme1742DataBlock* vme;

  Hist_t*  Hist = &fHist;

  ich1 = GetTimeChannelNumber(0);
  ich2 = GetTimeChannelNumber(1);

  igr1 = ich1/8;
  igr2 = ich2/8;

  vme = TDrsGlobals::Instance()->GetVme1742DataBlock();

  idfc = vme->FirstCell(igr2)-vme->FirstCell(igr1);

  r1  = GetFitResults(ich1);
  r2  = GetFitResults(ich2);

  r20 = GetFitResults(20);
  r21 = GetFitResults(21);

  t16 = T0CFD(r1 ,fLevel);
  t20 = T0CFD(r20,fLevel);
  t21 = T0CFD(r21,fLevel);

  t_sl   = (t20+t21)/2.;
  dt2021 = (t20-t21)/5.;
  dt_sl  = (t16-t_sl)/5.;

  data1 = r1->GetTofData();
  data2 = r2->GetTofData();
					// pedestal-subtracted...
  ph1  = data1->GetV1Max();
  ph2  = data2->GetV1Max();
					// normalized to 511 MeV in the photopeak
  q1   = data1->GetQ1();
  q2   = data2->GetQ1();

  chi2_1 = r1->fChi2;
  chi2_2 = r2->fChi2;

  fDt  = Dt   (r1,r2,fLevel)/5. ; 		// convert to ns, channel is 200 ps
  fDtl = DtCFD(r1,r2,fLevel)/5. ; 		// in ns

  fT0  = (T0(r1,fLevel)+T0(r2,fLevel))/2.; 

  //    if ((ph1 < fMinPulse[ich1]) || (ph2 < fMinPulse[ich2])) {

  fHist.fDt [0]->Fill(fDt );
  fHist.fDtl[0]->Fill(fDtl);

  good_event == 0;
  if ((q1 > GetMinQ1()) && (q2 > GetMinQ1())) {
    good_event = 1;
  }
//-----------------------------------------------------------------------------
// each channels knows about the thresholds ? - that may be algorithm-specific..
//-----------------------------------------------------------------------------
  if ((q1 < GetMinQ1()) || (q2 < GetMinQ1())) {
    fHist.fDt [1]->Fill(fDt );
    fHist.fDtl[1]->Fill(fDtl);
  }
  else if ((q1 > GetMinQ1()) && (q1 < GetMaxQ1()) && 
	   (q2 > GetMinQ1()) && (q2 < GetMaxQ1())    ) {
//-----------------------------------------------------------------------------
// both signals are large enough
//-----------------------------------------------------------------------------
    fHist.fDt [2]->Fill(fDt );
    fHist.fDtl[2]->Fill(fDtl);

    fHist.fDt2VsIch1->Fill(ich1,fDt);
    fHist.fDt2VsIch2->Fill(ich2,fDt);

    fHist.fDt2021->Fill(dt2021);
    fHist.fDtSL->Fill(dt_sl);

    fHist.fDt2VsDfc->Fill(idfc,fDt);

    fHist.fDtFit[ich1]->Fill(T0(r1,fLevel)-r1->GetTofData()->fT0);
    fHist.fDtFit[ich2]->Fill(T0(r2,fLevel)-r2->GetTofData()->fT0);

    fHist.fDtlVsDP0->Fill(r1->fPar[0]-r2->fPar[0],fDtl);

    int ient = (int) (TDrsGlobals::Instance()->GetEntry()/200.);

    fHist.fDt2VsEntry->Fill(ient,fDt);

    fHist.fDtVsPH1->Fill(q1,fDt);
    fHist.fDtVsPH2->Fill(q2,fDt);
    fHist.fDtVsT0 ->Fill(fT0,fDt);
    
    if ((r1->fPar[0] < 10) && (r2->fPar[0] < 10)) {
      fHist.fDt [3]->Fill(fDt);
      fHist.fDtl[3]->Fill(fDtl);
    }

    if ((r1->fPar[0] > 10) && (r2->fPar[0] > 10)) {
      fHist.fDt [4]->Fill(fDt);
      fHist.fDtl[4]->Fill(fDtl);
    }

    if ((chi2_1 < 5) && (chi2_2 < 5)) {
      fHist.fDt[5]->Fill(fDt);
      fHist.fDtl[5]->Fill(fDtl);
    }

    if ((chi2_1 < 3) && (chi2_2 < 3)) {
      fHist.fDt[6]->Fill(fDt);
      fHist.fDtl[6]->Fill(fDtl);
    }

    if ((chi2_1 < 2) && (chi2_2 < 2)) {
      fHist.fDt[7]->Fill(fDt);
      fHist.fDtl[7]->Fill(fDtl);
    }

    if (fDt < -2.) {
      printf(">>> TExp1FitAlgNew: entry = %5i  dt = %10.3f \n", 
	     TDrsGlobals::Instance()->GetEntry(),fDt);
    }
//-----------------------------------------------------------------------------
// also fill histograms for all channels used
//-----------------------------------------------------------------------------
    double t0, t0n, y; 
    int    it0, ich;

    for (int i=0; i<2; i++) {
      ich = fTimeChannelNumber[i];

      res = (TDrsFitResults*) fListOfFitResults->At(ich);
      dt  = res->GetTofData();

      t0  = T0(res,fLevel);
      t0n = t0-((int) t0);

      fHist.fChi2[ich]->Fill(res->fChi2);
      fHist.fNFitPoints[ich]->Fill(res->GetNFitPoints());

      fHist.fDtNorm [ich]->Fill(t0n);
      fHist.fP0     [ich]->Fill(res->fPar[0]);
      fHist.fP1     [ich]->Fill(res->fPar[1]);
      fHist.fP2     [ich]->Fill(res->fPar[2]);
      fHist.fT0Fit  [ich]->Fill(res->fT0Fit);

      if (good_event) {
	fHist.fDtChan [ich]->Fill(fDt);
      }
	
      it0 = ((int) t0)-10;
      if (it0 < 0) it0 = 0;

      // 	for (int icell=it0; icell<kNCells; icell++) {
      // 	  y = ch->GetHist()->fShape->GetBinContent(icell+1);
      // 	  fHist.fShape[i]->Fill(icell-it0,y);
      // 	}
    }
  }

  return 0;
}


//-----------------------------------------------------------------------------
// Exp1sian fit
//-----------------------------------------------------------------------------
int TExp1FitAlgNew::Fit(const TTofData* Data) { 

  int     max_bin, min_bin, found(0);
  int     ierflg, ich, imin, imax; 
  double  arglist[10], grad[10], vmin, slope, mean, chi2, vmax, t0;
  double          min_fraction, max_fraction;
  TDrsFitResults* res;

  ich = Data->GetChannelID()->GetNumber();

  if (Data->GetTriggerChannel() == 0) { 
    res = (TDrsFitResults*) fListOfFitResults->At(ich);
    min_fraction = fMinFraction[ich];
    max_fraction = fMaxFraction[ich];
  }
  else {
    res = (TDrsFitResults*) fListOfTrigFitResults->At(ich);
    min_fraction = fMinTrigFraction[ich];
    max_fraction = fMaxTrigFraction[ich];
  }

  res->fTofData = (TTofData*) Data;
  
  vmax = Data->GetV1Max();
					// determine the fit range
  int ncells = Data->GetNCells();
//-----------------------------------------------------------------------------
// now move in the opposite direction to find the 2% level
//-----------------------------------------------------------------------------
  t0      = Data->GetT0();
  //  min_bin = int(t0-5.);
  min_bin = int(t0-0.);
  if (min_bin < 0) min_bin = 0;

  max_bin = Data->GetI1Max();
  for (int i=max_bin; i>=0; i--) {
    if (Data->GetV1(i) > vmax*fMaxFraction[ich]) {
      max_bin = i;
    }
    else {
      break;
    }
  }
    
  if (max_bin < 0   ) max_bin=0;
  if (max_bin > 1023) max_bin=1023;
//-----------------------------------------------------------------------------
// fit range is defined, fit V1 with the exp1 function
//-----------------------------------------------------------------------------
  res->fMinBin = min_bin;
  res->fMaxBin = max_bin;

  Data->fHistV1->GetXaxis()->SetRangeUser(0.,1024.);

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
    fMinuit->mnparm(0, "p0"   , 2.    , 0.01, 0.6        , 2        , ierflg);
    fMinuit->mnparm(1, "p1"   , 0.4   , 0.1 , 0.         , 5        , ierflg);
    fMinuit->mnparm(2, "p2"   , t0    , 0.1 , t0-15      , t0+10    , ierflg);
    
    arglist[0] = 1000;
    arglist[1] = 1.;

    fMinuit->mnexcm("MIGRAD", arglist ,2,ierflg);

    for (int ip=0; ip<fNPar; ip++) {
      fMinuit->GetParameter(ip,res->fPar[ip],res->fSig[ip]);
      res->fFun->SetParameter(ip,res->fPar[ip]);
    }

    fMinuit->Eval(fNPar,grad,chi2,res->fPar,3);
    res->fChi2       = chi2/(max_bin-min_bin+1-fNPar+1.e-12);
  }
  else if (fFitMode == 1) {

    res->fFun->SetParameter(0,0.02);
    res->fFun->SetParameter(1,0.35);
    res->fFun->SetParameter(2,t0 );

    res->fFun->SetParLimits(0,1.e-3,0.2);
    res->fFun->SetParLimits(1,0.1,1.);
    res->fFun->SetParLimits(2,t0-20,t0+10);
//-----------------------------------------------------------------------------
// this is where the fit happens - why am I fitting V1, and not shape?
///-----------------------------------------------------------------------------
//    Data->fHistV1->Fit(res->fFun,"w0q","",min_bin,max_bin);
//    Data->fHistV1->Fit(res->fFun,"w0q","",min_bin,max_bin+3);

//    Data->fHistV1->Fit(res->fFun,"w0q","",min_bin,max_bin+2);
    Data->fHistShape->Fit(res->fFun,"0q","",min_bin-0.5,max_bin+0.5);
  
    for (int i=0; i<fNPar; i++) {
      res->fPar[i] = res->fFun->GetParameter(i);
      res->fSig[i] = res->fFun->GetParError (i);
    }
    res->fChi2 = res->fFun->GetChisquare()/(res->fFun->GetNumberFitPoints()-fNPar+0.0001);
  }

  res->fT0Fit = T0CFD(res,fLevel);
//-----------------------------------------------------------------------------
// accumulate the common histogram for the channel
//-----------------------------------------------------------------------------
    int    i1;
    double y1;

  if ((Data->GetQ1() > 420) && (Data->GetQ1() < 600)) {
    imin = (int) (res->fT0Fit-20.);
    if (imin < 0) imin = 0;
    imax = 1024;
    
    for (int i=imin; i<imax; i++) {
      i1 = (int) (i-res->fT0Fit+20);
      y1 = Data->fHistShape->GetBinContent(i);
      fHist.fAveShape[ich]->Fill(i1,y1);

      y1 = Data->GetV1(i);
      fHist.fAvePulse[ich]->Fill(i1,y1);
    }
  }
  
  return 0;
}

//-----------------------------------------------------------------------------
// why this is V1, and not Shape? - switch to shape...
//-----------------------------------------------------------------------------
int TExp1FitAlgNew::Display() {

  if (fCanvas == 0) {
    fCanvas = new TCanvas("exp1_shape_display","exp1_shape_display",1000,800);
  }

  float x[1024], y[1024], ex[1024], ey[1024];

  int            ich1, ich2, ich, np, ichan;
  TTofData       *dt1, *dt2;
  TDrsFitResults *r1 , *r2;
//-----------------------------------------------------------------------------
// used points, first channel
//-----------------------------------------------------------------------------
  fCanvas->cd();

  ich1 = fTimeChannelNumber[0];
  ich2 = fTimeChannelNumber[1];

  r1  = GetFitResults(ich1);
  r2  = GetFitResults(ich2);

  dt1 = r1->GetTofData();
  dt2 = r2->GetTofData();

  double xmin, xmax;

  xmin = r1->fPar[2]-10; // 50;
  if (xmin < 0) xmin = 0;
  xmax = r1->fPar[2]+40; // 350;
  if (xmax > 1024) xmax = 1024;

  //  fHist.fShape[ich1]->GetXaxis()->SetRangeUser(xmin,xmax);

  dt1->GetHistShape()->SetLineColor(2);
  dt1->GetHistShape()->Draw();
  dt2->GetHistShape()->Draw("sames");

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

  TDrsUtils::DrawPaveLabelNDC(l1,Form("Entry: %6i ; q1, q2 = %7.1f %7.1f, dt_cfd(0.1)=%8.4f dtg=%8.4f",
				      TDrsGlobals::Instance()->GetEntry(), 
				      dt1->GetQ1(),
				      dt2->GetQ1(), 
				      T0CFD(r1,0.1)-T0CFD(r2,0.1),
				      Dt(r1,r2,fLevel)),
				      0.1,0.86,0.7,0.90);
  return 0;
}
