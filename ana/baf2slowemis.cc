//////////////////////////////////////////////////////////////////////////////
// emission spectrum of BaF2 Slow Emission
///////////////////////////////////////////////////////////////////////////////

#include "ana/TScintillator.hh"

namespace {
  TGraph*   gGraph;
  TGraph*   gGraphNorm;
  TSpline3* gSpl;
  TH1F* Hist, hist; 
  
};

//-----------------------------------------------------------------------------
void TScintillator::create_baf2slow_emission_spectrum(TH1F** Hist) {

  double wl0, wscale, x0,y0,eff0,effscale;

  double tr[3][4] = {
//   wl  eff   x     y
    200., 0., 249., 459.,    // x0,y0
    500., 0., 623., 455.,    // x1,y0
    200., 150., 247., 86.,   // x0,y0
 };


  wl0      = tr[0][0];
  wscale   = (tr[1][0]-tr[0][0])/(tr[1][2]-tr[0][2]);

  eff0     = tr[0][1];
  effscale = (tr[2][1]-tr[0][1])/(tr[2][3]-tr[0][3]);

  printf(" tr[2][1], tr[0][1], tr[2][3], tr[0][3], effscale = %7.3f %7.3f %7.3f %7.3f %7.3f\n",
	 tr[2][1], tr[0][1], tr[2][3], tr[0][3], effscale);

  x0       = tr[0][2];
  y0       = tr[0][3];

  double data[] = {
    280,459, 290,449, 300,437, 307,422, 315,384, 320,360,  
    327,329, 335,279, 340,251, 345,218, 350,189, 354,170, 
    359,159, 362,152, 369,146, 377,145, 381,155, 390,176, 
    396,195, 400,212, 406,235, 414,259, 418,275, 425,297, 
    432,319, 447,352, 460,377, 478,403, 493,419, 508,430, 
    532,442, 551,447, 574,451, 590,454, 598,455,   
    -1.
    };
 
  int np  = 0;
  int loc = 0;

  double wl[1000], eff[1000];

  for (int i=0; data[2*i]>=0; i++) {

    loc = 2*i;

    wl [i]   = wl0  + (data[loc  ]-x0)*wscale;
    eff[i]   = eff0 + (data[loc+1]-y0)*effscale;

    printf ("%2i",i);
    printf (" %7.2f %7.2f %7.2f %7.2f %7.2f"  , data[loc  ], x0, wl0 , wscale  , wl[i] );
    printf (" %7.2f %7.2f %7.2f %7.2f %7.2f\n", data[loc+1], y0, eff0, effscale, eff[i]);

    np++;
  }

  gGraph = new TGraph(np,wl,eff);
  gGraph->SetMarkerStyle(20);
  gGraph->SetMarkerSize(0.5);

//Normalize

  Double_t scale = 1./gGraph->Integral();
  double neff[1000];

  for (Int_t k=0; data[2*k]>=0; k++) {
    
    neff[k] = eff[k]*scale;
  }

  gGraphNorm = new TGraph(np,wl,neff);
  gGraphNorm->SetMarkerStyle(20);
  gGraphNorm->SetMarkerSize(0.5);

  gSpl = new TSpline3("labr3_emission",wl,neff,np,"",0,0);
   
//Make spline into a histogram
 
  int const nbins = 100;
  double wlmax = tr[1][0];
  double wlen, step, intensity;
    

  step = (wlmax-wl0)/nbins;

  *Hist = new TH1F("splhist","",nbins,wl0,wlmax);
  
  for (int j=1; j<=nbins; j++){
    wlen = wl0+(j-1)*step;
    intensity = gSpl->Eval(wlen);
    if (intensity < 0) intensity = 0; 
    
    if (wlen > wl[np-1]) intensity = 0;
    
    (* Hist)->SetBinContent(j,intensity);
  }

  return;
}

//-----------------------------------------------------------------------------

void TScintillator::plot_baf2slow_emission_spectrum(const char* Opt) {

  char option[200];

  TH1F* hist;

  create_baf2slow_emission_spectrum(&hist);
  //Draw the histogram
  sprintf(option,"alp%s",Opt);
  hist->Draw(option);
    
  //Draw the spline 
  gSpl->SetLineColor(2);
  gSpl->Draw("same");
}



