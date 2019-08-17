//////////////////////////////////////////////////////////////////////////////
// emission spectrum of LYSO
///////////////////////////////////////////////////////////////////////////////

#include "ana/TScintillator.hh"

namespace {
  TGraph*   gGraph;
  TGraph*   gGraphNorm;
  TSpline3* gSpl;
  TH1F* Hist, hist; 
  
};

//-----------------------------------------------------------------------------
void TScintillator::create_lyso_emission_spectrum(TH1F** Hist) {

  double wl0, wscale, x0,y0,eff0,effscale;

  double tr[3][4] = {
//   wl  eff   x     y
    300., 0., 230., 879.,    // x0,y0
    700., 0., 916., 879.,    // x1,y0
    300., 1., 230., 204.     // x0,y1
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
    298,876,  319,875, 337,873, 349,867, 355,853, 361,829,  366,787,
    369,738,  372,693, 375,641, 378,608, 381,550, 388,511,  394,465,
    404,411,  414,364, 420,347, 422,336, 429,309, 437,282,  443,265,
    447,261,  452,261, 458,270, 464,286, 469,307, 475,330,  480,361,
    485,396,  490,439, 496,476, 500,504, 509,563, 515,593,  521,626,
    533,678,  551,730, 573,773, 598,808, 637,833, 682,855,  729,867,
    781,873,  837,876, 875,878,
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

    (*Hist)->SetBinContent(j,intensity);
  }

  return;
}

//-----------------------------------------------------------------------------

void TScintillator:: plot_lyso_emission_spectrum(const char* Opt) {

  char option[200];
  
  TH1F* hist;
  
  create_lyso_emission_spectrum(&hist);
  
  //Draw the histogram
  sprintf(option,"alp%s",Opt);
  hist->Draw(option);

  //Draw the spline 
  gSpl->SetLineColor(2);
  gSpl->Draw("same");
}




