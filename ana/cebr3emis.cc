
//////////////////////////////////////////////////////////////////////////////
// emission spectrum of CeBr3 
///////////////////////////////////////////////////////////////////////////////

#include "ana/TScintillator.hh"

namespace {
  TGraph*   gGraph;
  TGraph*   gGraphNorm;
  TSpline3* gSpl;
  TH1F* Hist, hist; 
  
};

//-----------------------------------------------------------------------------
void TScintillator:: create_cebr3_emission_spectrum(TH1F** Hist) {

  double wl0, wscale, x0,y0,eff0,effscale;

  double tr[3][4] = {
//   wl  eff   x     y
    300., 0., 234., 507.,    // x0,y0
    500., 0., 664., 507.,    // x1,y0
    300., 1., 234., 134.     // x0,y1
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
    286,506,  302,503, 317,496, 323,489, 329,479, 332,471,  334,466,
    337,459,  340,450, 346,426, 351,400, 355,378, 358,358,  360,337,
    362,316,  364,295, 367,268, 369,244, 372,212, 375,183,  379,160,
    381,146,  383,139, 386,136, 391,140, 393,146, 399,154,  402,164,
    408,182,  416,202, 424,225, 431,253, 438,283, 445,318,  451,345,
    461,385,  469,404, 481,428, 493,453, 507,476, 519,487,  537,497,
    563,502,  568,505,  599,505, 
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

void TScintillator::plot_cebr3_emission_spectrum(const char* Opt) {

  char option[200];
  
  TH1F*  hist;
  
  create_cebr3_emission_spectrum(&hist);
  
   //Draw the histogram
  sprintf(option,"alp%s",Opt);
  hist->Draw(option);
    
    //Draw the spline 
  gSpl->SetLineColor(2);
  gSpl->Draw("same");
}

