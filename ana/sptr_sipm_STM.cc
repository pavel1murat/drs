//////////////////////////////////////////////////////////////////////////////
// emission spectrum of LaBr3 
///////////////////////////////////////////////////////////////////////////////

#include "ana/TPMT.hh"


namespace {
  TGraph*   gGraph;
  TGraph*   gGraphNorm;
  TSpline3* gSpl;
  TH1F* Hist, hist; 
  
};



//-----------------------------------------------------------------------------
//The first histogram is to find an equation that fits the leading edge of sptr
//pulse for hamamatsu SiPM.  Uses digitization of image with 1 ns divisions
//-----------------------------------------------------------------------------
void sptrleadedge2(TH1F** Hist) {

  double t0,tscale,x0,y0,v0,vscale,tmax;

  double tr[3][4] = {
    // t(ns)  v   x     y
         0.,  0., 438., 82.,    // x0,y0
         2.,  0., 636., 82.,    // x1,y0
         0.,  9., 438., 496.    // x0,y1
  };



  t0      = tr[0][0];
  tscale   = (tr[1][0]-tr[0][0])/(tr[1][2]-tr[0][2]);
  
  v0     = tr[0][1];
  vscale = (tr[2][1]-tr[0][1])/(tr[2][3]-tr[0][3]);

  printf(" tr[2][1], tr[0][1], tr[2][3], tr[0][3], effscale = %7.3f %7.3f %7.3f %7.3f %7.3f\n",
	 tr[2][1], tr[0][1], tr[2][3], tr[0][3], vscale);
  
  x0       = tr[0][2];
  y0       = tr[0][3];
  
  double data[] = {
    445, 84,
    458, 116,
    476, 206,
    492, 264,
    508, 331,
    522, 396,
    535, 432,
    559, 456,
    588, 463,
    620, 470,
    636, 473,
    670, 473,
    680, 473,
    -1.
  };
 
  int np  = 0;
  int loc = 0;

  double t[100], v[100];
  
  for (int i=0; data[2*i]>=0; i++) {

    loc = 2*i;

    t [i]   = t0  + (data[loc  ]-x0)*tscale;
    v[i]   = v0 + (data[loc+1]-y0)*vscale;

    printf ("%2i",i);
    printf (" %7.2f %7.2f %7.2f %7.2f %7.2f"  , data[loc  ], x0, t0 , tscale  , t[i] );
    printf (" %7.2f %7.2f %7.2f %7.2f %7.2f\n", data[loc+1], y0, v0, vscale, v[i]);

    np++;
  }

  gGraph = new TGraph(np,t,v);
  gGraph->SetMarkerStyle(20);
  gGraph->SetMarkerSize(0.5);
 
  TF1* fit = new TF1("fit","[0]*(1-exp(-[1]*x+[2]))",0,11);
  fit->SetParName(0,"c0");
  fit->SetParName(1,"c1");
  fit->SetParName(2,"c2");

  fit->SetParameters(10,1.,1.);
  
  gGraph->Fit("fit");
  
  gGraph->Draw("alp");
}

//////////////////////////////////////////////////////////////
//the second graph for the sptr pulse tail taken from image with 50 ns divisions hamamatsu sipm
////////////////////////////////////////////////////////////////

  
void TPMT::sptr_STM_sipm(TH1F** Hist){

  double t0,tscale,x0,y0,v0,vscale,tmax;
  double tr[3][4] = {

 // t (ns) v    x     y
   -19.2,  0., 496., 91.,    // x0,y0
    400.8, 0., 916., 91.,    // x1,y0
   -19.2,  9., 496., 594.    // x0,y1
  };



  t0      = tr[0][0];
  tscale   = (tr[1][0]-tr[0][0])/(tr[1][2]-tr[0][2]);

  v0     = tr[0][1];
  vscale = (tr[2][1]-tr[0][1])/(tr[2][3]-tr[0][3]);

  printf(" tr[2][1], tr[0][1], tr[2][3], tr[0][3], effscale = %7.3f %7.3f %7.3f %7.3f %7.3f\n",
	 tr[2][1], tr[0][1], tr[2][3], tr[0][3], vscale);
  
  x0       = tr[0][2];
  y0       = tr[0][3];

  double data[] = {
    517.,568.,  518.,551.,  519.,537.,  520.,494.,
    521.,474.,  522.,458.,  523.,440.,  524.,425.,  525.,407.,  
    526.,393.,  527.,382.,  528.,369.,  530.,352.,  536.,333.,  
    542.,313.,  552.,298.,  568.,260.,  580.,242.,  610.,203., 
    630.,181.,  674.,150.,  726.,126.,  795.,109.,  835.,99.,
    894.,94.,     
    -1.
  };
  
  int np  = 0;
  int loc = 0;

  double t[1000], v[1000];

  for (int i=0; data[2*i]>=0; i++) {

    loc = 2*i;

    t[i]   = t0  + (data[loc  ]-x0)*tscale;
    v[i]   = v0 + (data[loc+1]-y0)*vscale;

    printf ("%2i",i);
    printf (" %7.2f %7.2f %7.2f %7.2f %7.2f"  , data[loc  ], x0, t0 , tscale  , t[i] );
    printf (" %7.2f %7.2f %7.2f %7.2f %7.2f\n", data[loc+1], y0, v0, vscale, v[i]);

    np++;
  }

  gGraph = new TGraph(np,t,v);
  gGraph->SetMarkerStyle(20);
  gGraph->SetMarkerSize(0.5);
  
  gSpl = new TSpline3("sptr",t,v,np,"",0,0);
  
  gGraph->Draw("alp");
  gSpl->Draw("");

  
 //make spline into histogram

  int const nbins =1000;
  tmax = tr[1][0];
  double time, step, voltage0, voltage;
  double peak = gSpl->Eval(2);  

  step = tmax/nbins;

  *Hist = new TH1F("sptr_tail_hist","",nbins,0,tmax);
  
  for (int j=1; j<=nbins; j++){
    time = (j-0.5)*step;
    float tail;
    if      (time <= 2) voltage0 = peak;
    else if (time > 2)  voltage0 = gSpl->Eval(time);

    else if (voltage < 0) voltage0 = 0; 
    else if (time > t[np-1]) voltage0 = 0;
    
    voltage = voltage0*9.03*(1-exp(-1.63*time));//*exp(-time/148.9);
    
    (*Hist)->SetBinContent(j,voltage);
    (*Hist)->Draw();
  }
  
}


		      
			 
			    
				 
				 




