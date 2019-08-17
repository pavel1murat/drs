#include "drs/ana/OscFit.hh"

#include "TF1.h"

ClassImp(OscFit)

//-----------------------------------------------------------------------------
OscFit::OscFit(): TNamed("oscFit", "oscFit") {
  clear();
}


//-----------------------------------------------------------------------------
OscFit::~OscFit() {
}


//-----------------------------------------------------------------------------
void OscFit::clear() {
  evt = 0;
  for (int i=0; i<8; ++i) {
    adc[i]   = 0;
    adcf[i]  = 0;
    t[i]     = 0;
    d[i]     = 0;
    dydx[i]  = 0;
    tau1[i]  = 0;
    tau2[i]  = 0;
    T[i]     = 0;
    sigma[i] = 0;
    bkg[i]   = 0;
    sbkg[i]  = 0;
    v[i]     = 0;
    xmaxi[i] = 0;
    ymaxi[i] = 0;
    xt[i]    = 0;
    yt[i]    = 0;
    chi2[i]  = 10;
    np[i]    = 0;
    p0[i]    = 0;
    p1[i]    = 0;
    dline[i] = 0;
    chi2line[i] = 10;
    npline[i] = 0;
    //
    yscal[i] = 0;
    pti[i] = 0;
    pty[i] = 0;
    ptx[i] = 0;
    plx[i] = 0;
    plx0[i] = 0;
    plc[i] = 0;
    ptok[i] = kTRUE;
      //
    sat[i] = kFALSE;
    ok[i] = kTRUE;
    dok[i] = kTRUE;
    tok[i] = kTRUE;
  }
}

//-----------------------------------------------------------------------------
// this one had references, 'Double_T&' before, not sure how it could ever work
//-----------------------------------------------------------------------------
Double_t OscFit::fline(Double_t xx[], Double_t par[]) {
  Double_t x = *xx;
  Double_t intercept = par[0];
  Double_t slope = par[1];
  return intercept + slope*x;
}


//-----------------------------------------------------------------------------
Double_t OscFit::ITtau(Double_t x, Double_t tau, Double_t T) {
  const Double_t eps = 1e-12;
  const Double_t explim = 100.;
  Double_t arg;

  if (x < 0) return 0;                // function range
  if (tau < 0 || T < 0) return 0;     // sanity check

  Double_t ITtau = 0;

  // case of tau --> 0
  if (tau < eps) return 0;
   
  // case of T --> 0
  if (T < eps) {
    arg = -x/tau;
    if (TMath::Abs(arg) < explim) ITtau = TMath::Exp(arg);
    return ITtau;
  }

  // case of T --> tau
  if (TMath::Abs(tau - T) < eps) {
    // NB: tau > eps here
    arg = -x/tau;
    if (TMath::Abs(arg) < explim) ITtau = (x/tau)*TMath::Exp(arg);
    return ITtau;
  }

  // general case
  Double_t exp_T = 0;
  arg = -x/T;
  if (TMath::Abs(arg) < explim) exp_T = TMath::Exp(arg);
  Double_t exp_tau = 0;
  arg = -x/tau;
  if (TMath::Abs(arg) < explim) exp_tau = TMath::Exp(arg);
  // NB: T-tau is finite here
  ITtau = (tau/(T-tau)) * (exp_T - exp_tau);

  return ITtau;
}

//-----------------------------------------------------------------------------
Double_t OscFit::ITtausigma(Double_t x, Double_t tau, Double_t T, Double_t sigma) {
  const Double_t eps = 1e-12;
  const Double_t explim = 100.;
  const Double_t erfclim = 100.;
  Double_t arg;

  if (tau < 0 || T < 0) return 0;                 // sanity check
  if (sigma < 0) return 0;                        // is that possible? -- Yes!

  if (sigma == 0) return ITtau(x,tau,T);    // exect solution for sigma = 0

  // case tau --> 0
  if (tau < eps) return 0;

  // case T --> 0
  if (T < eps) {
    Double_t exp_x = 0;
    arg = -x/tau;
    if (TMath::Abs(arg) < explim) exp_x = TMath::Exp(arg);
    Double_t exp_sigma2 = 0;
    arg = sigma*sigma/2/tau/tau;
    if (TMath::Abs(arg) < explim) exp_sigma2 = TMath::Exp(arg);
    
    Double_t erfc_xsigmatau = 0;
    if (sigma*erfclim > TMath::Abs((x-sigma*sigma/tau)/sqrt(2.))) {
      arg = -(x-sigma*sigma/tau)/(sigma*sqrt(2.));
      erfc_xsigmatau = TMath::Erfc(arg);
    }
    else if (x-sigma*sigma/tau > 0) erfc_xsigmatau = 2;   // negative argument
    else erfc_xsigmatau = 0;                              // positive argument

    Double_t ITtausigma = 0.5*exp_x*exp_sigma2*erfc_xsigmatau;
    return ITtausigma;
  }

  // case tau --> T
  if (TMath::Abs(T-tau) < eps) {
    Double_t tt = tau > T? tau: T;

    Double_t erfc_xsigmatt = 0;
    if (sigma*erfclim > TMath::Abs((x-sigma*sigma/tt)/sqrt(2.))) {
      arg = -(x-sigma*sigma/tt)/(sigma*sqrt(2.));
      erfc_xsigmatt = TMath::Erfc(arg);
    }
    else if (x-sigma*sigma/tt > 0) erfc_xsigmatt = 2;  // negative argument
    else erfc_xsigmatt = 0;                            // positive argument
    
    Double_t exp_xsigmatt2 = 0;
    if (sigma*TMath::Sqrt(erfclim) > (x-sigma*sigma/tt)/sqrt(2.)) {
      arg = (x-sigma*sigma/tt)/(sigma*sqrt(2.));
      exp_xsigmatt2 = TMath::Exp(-arg*arg);
    }

    Double_t term1 = sigma*exp_xsigmatt2;
    Double_t sqrt_pi = TMath::Sqrt(TMath::Pi());
    Double_t sqrt_2 = TMath::Sqrt(2.);
    Double_t term2 = sqrt_pi*(x-sigma*sigma/tt)/sqrt_2*erfc_xsigmatt;
    Double_t exp_x = 0;
    arg = -x/tt;
    if (TMath::Abs(arg) < explim) exp_x = TMath::Exp(arg);
    Double_t exp_sigma2 = 0;
    arg = sigma*sigma/2/tt/tt;
    if (TMath::Abs(arg) < explim) exp_sigma2 = TMath::Exp(arg);
    Double_t inv_sqrt_2pi = (1./TMath::Sqrt(2.*TMath::Pi()));
    Double_t ITtausigma = inv_sqrt_2pi*exp_x*exp_sigma2*(1./tt)*(term1+term2);
    return ITtausigma;
  }

  // regular case

  // T term
  Double_t exp_xT = 0;
  arg = -x/T;
  if (TMath::Abs(arg) < explim) exp_xT = TMath::Exp(arg);
  //
  Double_t exp_sigma2T2 = 0;
  arg = sigma*sigma / (2*T*T);
  if (TMath::Abs(arg) < explim) exp_sigma2T2 = TMath::Exp(arg);
  //
  Double_t erfc_xsigmaT = 0;
  if (sigma*erfclim > TMath::Abs((x-sigma*sigma/T)/sqrt(2.))) {
    arg = -(x-sigma*sigma/T)/(sigma*sqrt(2.));
    erfc_xsigmaT = TMath::Erfc(arg);
  }
  else if (x-sigma*sigma/T > 0) erfc_xsigmaT = 2;       // negative argument
  else erfc_xsigmaT = 0;                                // positive argument
  //
  Double_t term_T = 0.5*exp_xT*exp_sigma2T2*erfc_xsigmaT;
  
  // tau term
  Double_t exp_xtau = 0;
  arg = -x/tau;
  if (TMath::Abs(arg) < explim) exp_xtau = TMath::Exp(arg);
  //
  Double_t exp_sigma2tau2 = 0;
  arg = sigma*sigma / (2*tau*tau);
  if (TMath::Abs(arg) < explim) exp_sigma2tau2 = TMath::Exp(arg);
  //
  Double_t erfc_xsigmatau = 0;
  if (sigma*erfclim > TMath::Abs((x-sigma*sigma/tau)/sqrt(2.))) {
    arg = -(x-sigma*sigma/tau)/(sigma*sqrt(2.));
    erfc_xsigmatau = TMath::Erfc(arg);
  }
  else if (x-sigma*sigma/tau > 0) erfc_xsigmatau = 2;   // negative argument
  else erfc_xsigmatau = 0;                              // positive argument
  //
  Double_t term_tau = 0.5*exp_xtau*exp_sigma2tau2*erfc_xsigmatau;
  
  Double_t ITtausigma = (tau/(T-tau))*(term_T - term_tau);
  return ITtausigma;
}

//-----------------------------------------------------------------------------
Double_t OscFit::fPsigma(Double_t *xx, Double_t *par) {
  const Double_t eps = 1e-12;
  
  Double_t A     = par[0];
  Double_t x0    = par[1];
  Double_t tau1  = par[2];
  Double_t tau2  = par[3];
  Double_t T     = par[4];
  Double_t sigma = par[5];

  Int_t    npar  = 6;

  Double_t x = *xx - x0;
  
  if (tau1 < 0 || tau2 < 0 || T < 0) return 0;    // sanity check
  if (tau2 < eps) return 0;                       // discharge time physical limit
  if (sigma < 0 ) return 0;                       // is that possible? -- Yes!
  
  if (x <= -10.*TMath::Abs(sigma)) return 0;
  
  Double_t norm  = A*(tau1+tau2)/tau2/tau2;
  Double_t tau12 = tau1*tau2/(tau1+tau2);
  
  Double_t fPsigma = norm * (ITtausigma(x,tau2,T,sigma) - ITtausigma(x,tau12,T,sigma));
  return fPsigma;
}

//-----------------------------------------------------------------------------
TF1* OscFit::fpulse(Double_t xmin, Double_t xmax,
		    Double_t A, Double_t x0, Double_t tau1, Double_t tau2, 
		    Double_t T, Double_t sigma, const char* name) {

  TF1* fpulse = new TF1(name, OscFit::fPsigma, xmin, xmax, 6);
  fpulse->SetNpx(1024);
  fpulse->SetParName(0, "A");
  fpulse->SetParName(1, "x0");
  fpulse->SetParName(2, "tau1");
  fpulse->SetParName(3, "tau2");
  fpulse->SetParName(4, "T");
  fpulse->SetParName(5, "sigma");
  fpulse->SetParameter(0, A);
  fpulse->SetParameter(1, x0);
  fpulse->SetParameter(2, tau1);
  fpulse->SetParameter(3, tau2);
  fpulse->SetParameter(4, T);
  fpulse->SetParameter(5, sigma);
  return fpulse;
}


//-----------------------------------------------------------------------------
// linear fit of a TGraph
//-----------------------------------------------------------------------------
Double_t OscFit::lfit(const Float_t x[], 
		      const Float_t y[], 
		      Int_t         x1_i, 
		      Int_t         npoints, 
		      Double_t&     p0, 
		      Double_t&     p1) 
{
  Double_t chi2(0.), eps(1.e-7);
  p0 = p1 = 0;

  TGraph* gr_lfit = new TGraph(npoints, &x[x1_i], &y[x1_i]);
  gr_lfit->SetNameTitle("gr_lfit", "gr_lfit");
  gr_lfit->SetMarkerStyle(20);

  // new TCanvas;
  // gr_lfit->Draw("ap");

//-----------------------------------------------------------------------------
// linear fit of the graph
//-----------------------------------------------------------------------------
  Double_t xmin = x[x1_i] - eps;
  Double_t xmax = x[x1_i+npoints-1] + eps;

  gr_lfit->Fit("pol1", "Q0", "goff", xmin, xmax);

  // gr_lfit->Fit("pol1", "", "", xmin, xmax);
//-----------------------------------------------------------------------------
// retrieve parameters
//-----------------------------------------------------------------------------
  TF1* fpol1 = gr_lfit->GetFunction("pol1");
  if (fpol1) {
    p0 = fpol1->GetParameter("p0");
    p1 = fpol1->GetParameter("p1");
    if (fpol1->GetNDF() > 0) chi2 = fpol1->GetChisquare() / fpol1->GetNDF();
  }

  delete gr_lfit;
  return chi2;
}


///////////////////////////////////////////////////////////////////
// implements the straight line fit with equal weights
// main, the most general version
///////////////////////////////////////////////////////////////////
Double_t OscFit::pol1fast(const Float_t x [], 
			  const Float_t y [], 
			  const Float_t ey[], 
			  Int_t         ifirst, 
			  Int_t         np    , 
			  Double_t&     am    , 
			  Double_t&     bm    ) {

  // if ey == 0 the weights are considered equal

  Double_t eps = 1e-7;
  Double_t huge = 1e10;

  Double_t chi2 = huge;
  am = 0;
  bm = 0;
  
  // fit of the straight line with equal weights
  Double_t wx  = 0;
  Double_t wy  = 0;
  Double_t wxy = 0;
  Double_t wx2 = 0;
  Double_t W   = 0;
  
  for (int i=0; i<np; ++i) {
    Int_t icurr = ifirst+i;
    Float_t xi  = x[icurr];
    Float_t yi  = y[icurr];
    Float_t eyi = (ey)? ey[icurr]: 0;
    Float_t wi  = 1.;
    if (eyi) wi = TMath::Abs(eyi) > eps? 1./(eyi*eyi): 1.;
    W          += wi;
    wx         += wi*xi;
    wx2        += wi*xi*xi;
    wy         += wi*yi;
    wxy        += wi*xi*yi;
  }

  Double_t Discriminant = W*wx2 - wx*wx;
  if (TMath::Abs(Discriminant) < eps) return huge;   // should not be happen, actually

  am = (wy*wx2 - wxy*wx)/Discriminant;    // y = am + bm*x
  bm = (W*wxy  - wx*wy )/Discriminant;

  chi2 = 0;
  for (int i=0; i<np; ++i) {
    Int_t icurr = ifirst+i;
    Double_t yfit = am + bm*x[icurr];
    Double_t r = y[icurr] - yfit;
    if (ey) r = TMath::Abs(ey[icurr]) > eps? r / ey[icurr]: 1.;
    chi2 += r*r;
  }
  //cout<< ".. pol1fast (complete version): chi2 = " << chi2 << " am = " << am << " bm = " << bm <<endl;
  
  Double_t chi2ndf = np > 2? chi2/(np - 2): 0;
  return chi2ndf;
}

