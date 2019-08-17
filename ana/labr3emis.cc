//////////////////////////////////////////////////////////////////////////////
// emission spectrum of LaBr3 
///////////////////////////////////////////////////////////////////////////////

#include "ana/TScintillator.hh"

namespace {
  TGraph*   gGraph;
  TGraph*   gGraphNorm;
  TSpline3* gSpl;
  TH1F* Hist, hist; 
  
};

//-----------------------------------------------------------------------------
void TScintillator:: create_labr3_emission_spectrum(TH1F** Hist) {

  double wl0, wscale, x0,y0,eff0,effscale;

  double tr[3][4] = {
//   wl  eff   x     y
    300., 0., 260., 640.,    // x0,y0
    500., 0., 857., 640.,    // x1,y0
    300., 1., 260., 224.     // x0,y1
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
    319,640,     334,639,     350,636,     359,630,     370,601,    
    388,469,     404,319,     415,254,     420,237,     424,226,     
    430,225,     436,237,     442,259,     452,300,     470,372,    
    487,400,     498,402,     509,415,     520,434,     530,460,    
    540,494,     550,530,     559,560,     570,585,     580,602,    
    591,614,     600,620,     610,626,     620,630,     630,633,     
    640,635,     670,636,     690,636,     719,637,     820,640,
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

void  TScintillator::plot_labr3_emission_spectrum(const char* Opt) {

  char option[200];

  TH1F* hist;

  create_labr3_emission_spectrum(&hist);
  
  //Draw the histogram
  sprintf(option,"alp%s",Opt);
  hist->Draw(option);

  //Draw the spline 
  gSpl->SetLineColor(2);
  gSpl->Draw("same");
}



