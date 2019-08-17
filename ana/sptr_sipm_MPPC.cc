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
void sptrleadedge(TH1F** Hist) {

  double t0,tscale,x0,y0,v0,vscale,tmax;

  double tr[3][4] = {
    // t(ns)  v   x     y
         0.,  0., 356., 91.,    // x0,y0
         2.4, 0., 616., 91.,    // x1,y0
         0.,  9., 356., 597.    // x0,y1
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
    375, 102,
    412, 257,
    437, 393,
    470, 490,
    546, 566,
    589, 576,
    611, 576,
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

  
void TPMT::sptr_MPPC_sipm(TH1F** Hist){

  double t0,tscale,x0,y0,v0,vscale,tmax;
  double tr[3][4] = {


    
 // t (ns) v   x     y
    -8.4,  0., 496., 91.,    // x0,y0
    221.6, 0., 957., 91.,    // x1,y0
    -8.4,  9., 496., 597.    // x0,y1
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
    496.,585.,  500.,585.,  510.,585.,  513.,585.,  515.,585.,
    519.,585.,  527.,531.,  538.,452.,  553.,383.,  570.,300.,  
    576.,280.,  585.,255.,  595.,235.,  619.,193.,  640.,169.,  
    663.,150.,  698.,132.,  734.,124.,  770.,114.,  811.,112., 
    837.,110.,  870.,107.,  914.,103.,  930.,95.,   957.,91.,
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
  
  //gGraph->Draw("alp");
  // gSpl->Draw("alp");

  
 //make spline into histogram

  int const nbins = 1000;
  tmax = tr[1][0];
  double time, step, voltage0, voltage;
  double peak = gSpl->Eval(2.6);  

  step = tmax/nbins;

  *Hist = new TH1F("sptr_tail_hist","",nbins,0,tmax);
  
  for (int j=1; j<=nbins; j++){
    time = (j-0.5)*step;
    if (time <= 2.6) voltage0 = peak;
    else if (time > 2.6)  voltage0 = gSpl->Eval(time);
    else if (voltage < 0) voltage0 = 0; 
    else if (time > t[np-1]) voltage0 = 0;
    
    voltage = voltage0*9.2*(1-exp(-1.469*time));
    
    (*Hist)->SetBinContent(j,voltage);

    (*Hist)->Draw();
  }
  
}


		      
			 
			    
				 
				 




