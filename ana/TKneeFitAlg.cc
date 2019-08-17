//

#include "drs/ana/drsana.hh"
#include "drs/base/TDrsUtils.hh"
#include "drs/ana/TKneeFitAlg.hh"
#include "drs/obj/TDrsFitResults.hh"

ClassImp(TKneeFitAlg)


//-----------------------------------------------------------------------------
// function of 3 parameters
//-----------------------------------------------------------------------------
double TKneeFitAlg::PulseShape(double* X, double* Par) {
  double f, dx;
  
  dx = X[0] - Par[2];
  
  if (dx <= 0) {
    f = Par[0];
  }
  else {
    //    f  = Par[0] - Par[1]*(1-TMath::Exp(-dx/Par[3]));
    f = Par[0] + Par[1]*dx;
  }

  return f;
}

//_____________________________________________________________________________
void TKneeFitAlg::FCN(Int_t&    NPar , 
		      Double_t* Gin  , 
		      Double_t& F    , 
		      Double_t* Par  , 
		      Int_t     IFlag) {

  int    ich, min_bin, max_bin;
  double chi2 = 0;
  double t, v, err, dchi2, f;
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
  
  for (int i=min_bin; i<=max_bin; i++) {
    v    = hist->GetBinContent(i);

    if (wt[i] == 0) goto NEXT_BIN;

    err  = 1.; // drsana::fFitHist->GetBinError  (i);
					// determine local x - coordinate along the chamber
    t     = hist->GetBinCenter(i);
    f     = TKneeFitAlg::PulseShape(&t,Par);
    dchi2 = (f-v)*(f-v)/(err*err);
    chi2 += dchi2;

    if (drsana::fgInstance->fDebug[0] != 0) {
      printf("i,v,f,err,dchi2,chi2 = %3i %10.5f %10.5f %10.5f %10.5f %10.5f\n",
	     i,v,f,err,dchi2,chi2);
    }
  NEXT_BIN: ;
  }
  
  F = chi2;
}

//-----------------------------------------------------------------------------
TKneeFitAlg::TKneeFitAlg(const char* Name, const char* Title, TObjArray* ListOfChannels) : 
  TDrsFitAlg(Name, Title, ListOfChannels) {

  TDrsFitResults   *res;
  TDrsChannel      *ch;

  //  fListOfFitResults = new TObjArray();

  for (int i=0; i<fNChannels; i++) {
    ch = (TDrsChannel*) fCachedListOfChannels->At(i);
    if (ch) {
      res       = (TDrsFitResults*) fListOfFitResults->At(i);
      res->fFun = new TF1(Form("f_knee_ch_%03i",i),TKneeFitAlg::PulseShape,0,1024,3);

      fMinFraction[i]  = 0.03;
      fMaxFraction[i]  = 0.10 ;
      fNFitBins   [i]  = 4;
    }
  }
}


//-----------------------------------------------------------------------------
TKneeFitAlg::~TKneeFitAlg() {
}


//-----------------------------------------------------------------------------
int TKneeFitAlg::Fit(const TDrsChannel* Channel) {

  int           rc(0);
  int           ierflg, nx, max_bin, min_bin, start_bin, bin1, bin2, imax, imin, found;
  int           ich, ncells;

  double        arglist[10], grad[10], vmin, vmax, slope, a, t0, t0_corr, ymax ;
  TF1*          fun;
  char          pname[100];

  TDrsFitResults *res;


  ich         = Channel->GetNumber();
  res         = GetFitResults(ich);
  //  v           = Channel->fV;
					// maximum
  vmax        = Channel->fVMax;
					// find last bin to use..., pulse is negative

  ncells      = Channel->fNCells;
  min_bin     = -1;
  max_bin     = Channel->fVMaxCell;
  found       = 0;
//-----------------------------------------------------------------------------
// look for the last bin to use in the "knee-fit"
// in case of low light yield, max_bin could correspond to the second or third maximum
//-----------------------------------------------------------------------------
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
// now go back to determine range for fitting the slope
// this is a preliminary step
//-----------------------------------------------------------------------------
  min_bin = -1;
  for (int i=max_bin; i>=0; i--) {
					// to determine first approximation for T0...
    if (Channel->fV[i] < 3.) {
      min_bin = i;
      break;
    }
  }
//-----------------------------------------------------------------------------
// fast fit of the slope - use 3 points!
//-----------------------------------------------------------------------------
  double par[10], base, sig_par[10], sig_base, chi2_1, chi2_2;

  max_bin = min_bin+(fNFitBins[ich]-1);
  TDrsUtils::FitPol1(Channel->fV,min_bin,max_bin,par,&chi2_1); 
//-----------------------------------------------------------------------------
// having slope, determine 0-th approximation for T0
//-----------------------------------------------------------------------------
  t0 = (Channel->fBaseOffset-par[0])/par[1]; 

  imax = (int) (t0-5);
  if (imax < 1   ) imax = 1;
  if (imax > ncells) {
    Error("Fit_time",Form("imax = %i, reset to 1023",imax));
    imax = ncells;
  }

  imin = imax-40;
  if (imin < 1 ) imin = 1;
  res->fMinBin = imin;
//-----------------------------------------------------------------------------
// constant fit of the base offset for given event
//-----------------------------------------------------------------------------
  double base_offset;
  TDrsUtils::FitPol0(Channel->fV,imin,imax,&base_offset,&chi2_2);
//-----------------------------------------------------------------------------
// now refit the slope, perhaps using different points
//-----------------------------------------------------------------------------
  min_bin = -1;
  for (int i=max_bin; i>=0; i--) {
    a = Channel->fV[i]-base_offset;
					// to determine first approximation for T0...
    if (a < 3.) {
      min_bin = i;
      break;
    }
  }

  max_bin = min_bin+(fNFitBins[ich]-1);
  TDrsUtils::FitPol1(Channel->fV,min_bin,max_bin,par,&chi2_1); 

  res->fMaxBin = max_bin;
//-----------------------------------------------------------------------------
// and recalculate slightly corrected T0
//-----------------------------------------------------------------------------
  t0_corr = (base_offset-par[0])/par[1] - 0.0*vmax/par[1];

  res->fPar[0] = base_offset;
  res->fPar[1] = par[1];
  res->fPar[2] = t0_corr;

  TF1* f = res->fFun;

  f->SetParameter(0,base_offset);
  f->SetParameter(1,par[1]);
  f->SetParameter(2,t0_corr);

  f->SetRange(res->fMinBin-0.5,res->fMaxBin+1);
//-----------------------------------------------------------------------------
// now that we have T0, study channel by channel deviations from the straight line
// select 'before the pulse' and 'after the pulse' regions, fit them separately
// 'imin' and 'imax' are the channel numbers
//-----------------------------------------------------------------------------
  double mean, chi2;
					// before the pulse
  imax = (int) (t0_corr-10);
  if (imax > 1023) {
    Error("Fit",Form("002: imax = %i, set to 1023",imax));
    imax = 1023;
  }
  if (imax < 0) imax = 0;
  imin = imax-300;
  if (imin < 0) imin = 0;

  TDrsUtils::FitPol0(Channel->fV,imin,imax,&mean,&chi2);

  for (int i=imin; i<=imax; i++) {
    Channel->fHist.fDvVsCell->Fill(i,Channel->fV[i]-mean);
  }
					// after the pulse
  imin = (int) (t0_corr+350);
  if (imin > 1023) {
    Error("Fit",Form("003: imin = %i, set to 1023",imin));
    imin = 1023;
  }

  if (imin < 0   ) {
    Error("Fit",Form("004: imin = %i, set to 0",imin));
    imin = 0;
  }

  imax = imin+300;
  if (imax > 1023) imax = 1023;

  TDrsUtils::FitPol0(Channel->fV,imin,imax,&mean,&chi2);

  for (int i=imin; i<=imax; i++) {
    Channel->fHist.fDvVsCell->Fill(i,Channel->fV[i]-mean);
  }

  imin   = 0;
  imax   = 1023;
  TDrsUtils::FitPol0(Channel->fV,imin,imax,&mean,&chi2);

  for (int i=imin; i<=imax; i++) {
    Channel->fHist.fDv2VsCell->Fill(i,Channel->fV[i]-mean);
  }
//-----------------------------------------------------------------------------
// FILL SHAPE histogram this can be done only after the base offsets are fit accurately
//-----------------------------------------------------------------------------
  float y;
  Channel->fHist.fShape->Reset();
					// recalculate vmin using fitted base
  vmax = Channel->fVMax-base_offset;

  for (int i=0; i<1024; i++) { 
    y = Channel->fV[i]/(Channel->fVMax+1.e-12);
    Channel->fHist.fShape->Fill(i,y);
  }

  return 0;
}



//-----------------------------------------------------------------------------
int TKneeFitAlg::Display() {

  TDrsFitResults   *r, *r1, *r2;
  TDrsChannel* ch;
  double     vmax;
  if (fCanvas == 0) {
    fCanvas = new TCanvas("display","display",1000,800);
  }

  fCanvas->cd();

  float x[1024], y[1024], ex[1024], ey[1024];

  int ich1, ich2, ich, np;

  ich1 = GetUsedChannelNumber(0);
  ich2 = GetUsedChannelNumber(1);

  r1  = GetFitResults(ich1);
  r2  = GetFitResults(ich2);

  vmax = 0;
//-----------------------------------------------------------------------------
// finally, fill profile histograms used by the event display, need to do it here
// so far assume that only 2 channels are used...
//-----------------------------------------------------------------------------
  for (int i=0; i<2; i++) {
    ich = fUsedChannelNumber[i];
    ch  = GetChannel(ich);
    fDisplayV[ich]->Reset();
    for (int i=0; i<1024; i++) {
      fDisplayV[ich]->Fill(i,ch->fV[i]);
    }

    if (ch->fVMax > vmax) vmax = ch->fVMax;
  }

  fDisplayV[ich1]->SetMaximum(vmax);

  fDisplayV[ich1]->SetLineColor(2);
  fDisplayV[ich1]->Draw();
  fDisplayV[ich2]->Draw("same");

  r1->fFun->SetLineColor(2);
  r1->fFun->Draw("same");
  r2->fFun->Draw("same");
//-----------------------------------------------------------------------------
// used points, first channel
//-----------------------------------------------------------------------------
  for (int i=0; i<1024; i++) ex[i] = 0;

  for (int i=0; i<2; i++) {
    ich = GetUsedChannelNumber(i);
    ch  = GetChannel(ich);
    r   =  GetFitResults(ich);
    if (r->fUsedPointsGraph) delete r->fUsedPointsGraph;

    np = 0;
    for (int j=r->fMinBin; j<=r->fMaxBin; j++) {
      x [np] = j+0.5;
      y [np] = ch->fV[j];
      ey[np] = 1;
      np++;
    }
    
    r->fUsedPointsGraph = new TGraphErrors(np,x,y,ex,ey);
    r->fUsedPointsGraph->SetMarkerStyle(20);
    r->fUsedPointsGraph->SetMarkerSize(0.9);
  }

  r1->fUsedPointsGraph->SetMarkerColor(2);
  r1->fUsedPointsGraph->Draw("same,pe");

  r2->fUsedPointsGraph->Draw("same,pe");

  TPaveLabel  *l1; 

  drsana* drs  = drsana::fgInstance;

  TDrsUtils::DrawPaveLabelNDC(l1,
			      Form("file: %s Event: %6i",drs->fFilename.Data(),drs->fEntry),
			      0.1,0.85,0.7,0.90);
  return 0;
}




//-----------------------------------------------------------------------------
int TKneeFitAlg::BookHistograms()  {

  TDrsChannel*  ch;
  char   name[100];

  Hist_t* Hist = &fHist;

  HBook1F(Hist->fDt [0],"dt0","Delta(T)[0] KNEE (ns)",500,-2.5,2.5,"Hist");
  HBook1F(Hist->fDt [1],"dt1","Delta(T)[1] KNEE (ns)",500,-2.5,2.5,"Hist");
  HBook1F(Hist->fDt [2],"dt2","Delta(T)[2] KNEE (ns)",500,-2.5,2.5,"Hist");
  HBook1F(Hist->fDt [3],"dt3","Delta(T)[3] KNEE (ns)",500,-2.5,2.5,"Hist");

  HBook2F(Hist->fDtVsT0[0],"dt_vs_t","Delta(T) vs T[0] (ns)",200,0,200,300,-1.5,1.5,"Hist");

  HBook2F(Hist->fDt2VsPH1,"dt2_vs_ph1","Dt(2) vs PH(1)", 250,0,500,250,-2.5,2.5,"Hist");
  HBook2F(Hist->fDt2VsPH2,"dt2_vs_ph2","Dt(2) vs PH(2)", 250,0,500,250,-2.5,2.5,"Hist");
  HBook2F(Hist->fDt2VsT0 ,"dt2_vs_t0" ,"Dt(2) vs T0"   ,1000,0,200,250,-2.5,2.5,"Hist");

//-----------------------------------------------------------------------------
// book histograms for all channels used
//-----------------------------------------------------------------------------
  for (int i=0; i<fNChannels; i++) {
    ch = (TDrsChannel*) fCachedListOfChannels->At(i);
    if (ch) {
					// T0 is par[2], the slope is par[3]
      sprintf(name,"offset_%02i",i);
      HBook1F(fHist.fOffset[i],name,name,100,-5,5);

      sprintf(name,"slope_%02i",i);
      HBook1F(Hist->fSlope[i],name,name,100,0,20);

      sprintf(name,"t0_%02i",i);
      HBook1F(fHist.fT0[i],name,name,1024,0,1024);

      sprintf(name,"sigt0_%02i",i);
      HBook1F(Hist->fSigT0[i],name,name,100,0,10);
  
      sprintf(name,"dtnorm_%02i",i);
      HBook1F(Hist->fDtNorm[i],name,name,100,-0,1);
    }
  }

   return 0;
}


//-----------------------------------------------------------------------------
int TKneeFitAlg::FillHistograms() {

  TDrsChannel    *ch1, *ch2, *ch;
  TDrsFitResults  *r1, *r2, *res;

  double  ph1, ph2, q1, q2, t1, t2, t1n;

  Hist_t* Hist = &fHist;

  ch1 = GetUsedChannel(0);
  ch2 = GetUsedChannel(1);

  r1  = GetFitResults(ch1->GetNumber());
  r2  = GetFitResults(ch2->GetNumber());

  t1  = T0(r1);
  t2  = T0(r2);
  fDt = (t1-t2)/5.;
  fT0 = (t1+t2)/2.;
					// within the bin
  fHist.fDt[0]->Fill(fDt);
  fHist.fDtVsT0[0]->Fill(fT0,fDt);
//-----------------------------------------------------------------------------
// require both pulses to have reasonable amplitudes, try to cut out the
// Compton part of the spectrum
//-----------------------------------------------------------------------------
  ph1  = ch1->fVMax;
  ph2  = ch2->fVMax;

  q1   = ch1->fQ;
  q2   = ch2->fQ;

  if ((q1 < ch1->fMinQ) || (q2 < ch2->fMinQ)) {
    fHist.fDt [1]->Fill(fDt);
  }
  else {
//-----------------------------------------------------------------------------
// both pulses above the threshold
//-----------------------------------------------------------------------------
    fHist.fDt [2]->Fill(fDt);
    double dt_corr = fDt-0.0030*(ph1-65.);
    fHist.fDt[3]->Fill(dt_corr);

    fHist.fDt2VsPH1->Fill(ph1,fDt);
    fHist.fDt2VsPH2->Fill(ph2,fDt);
    fHist.fDt2VsT0 ->Fill(fT0 ,fDt);
  }
//-----------------------------------------------------------------------------
// also fill histograms for all channels used
//-----------------------------------------------------------------------------
  for (int i=0; i<fNChannels; i++) {
    ch = (TDrsChannel*) fCachedListOfChannels->At(i);
    if (ch) {
      res = (TDrsFitResults*) fListOfFitResults->At(i);

					// T0 is par[2], the slope is par[3]

      fHist.fOffset[i]->Fill(res->fPar[0]);
      fHist.fSlope [i]->Fill(res->fPar[1]);
      fHist.fT0    [i]->Fill(res->fPar[2]);
      fHist.fSigT0 [i]->Fill(res->fSig[2]);

      t1  = T0(res);
      t1n = t1-((int) t1);

      fHist.fDtNorm[i]->Fill(t1n);

      
    }
  }

  return 0;
}

 
//-----------------------------------------------------------------------------
double TKneeFitAlg::T0(TDrsFitResults* R1, double Level) {

  double t0 = R1->fPar[2];
   
  return  t0;
}

//-----------------------------------------------------------------------------
double TKneeFitAlg::Dt(TDrsFitResults* R1, TDrsFitResults* R2, double Level) {

  double dt = R1->fPar[2]-R2->fPar[2];
   
  return  dt;
}
