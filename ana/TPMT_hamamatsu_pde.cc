///////////////////////////////////////////////////////////////////////////////
// Photon Detection Efficiency Hamamatsu
///////////////////////////////////////////////////////////////////////////////
#include "ana/TPMT.hh"

void TPMT::MakeHamamatsuSiPMPDEHist(TSpline3** Spline) {

  double wl0, wscale, x0,y0,eff0,effscale;

  double tr[3][4] = {
//   wl  eff   x     y
    300.,    0.,   80., 500.,		// x0,y0
    900.,    0.,  563., 500.,		// x1,y0
    0.,    100.,   80.,  17.		// x0,y
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
      86.,500.,  92.,490.,  99.,471., 103.,452., 112.,405., 124.,356., 
     144.,308., 160.,288., 180.,274., 190.,272., 200.,273., 210.,275., 
     240.,290., 300.,343., 330.,370., 370.,400., 420.,428., 470.,451.,
      520.,470., 560.,483., 630.,500.,
    -1.
  };

  int np  = 0;
  int loc = 0;

  double wl[1000], eff[1000];

  for (int i=0; data[2*i]>=0; i++) {

    loc = 2*i;

    wl [i]   = wl0  + (data[loc  ]-x0)*wscale;
    eff[i]   = (eff0 + (data[loc+1]-y0)*effscale)/100.;

    printf ("%2i",i);
    printf (" %7.2f %7.2f %7.2f %7.2f %7.2f"  , data[loc  ], x0, wl0 , wscale  , wl[i] );
    printf (" %7.2f %7.2f %7.2f %7.2f %7.2f\n", data[loc+1], y0, eff0, effscale, eff[i]);

    np++;
  }

  TGraph* graph = new TGraph(np,wl,eff);
  graph->SetMarkerStyle(20);
  graph->SetMarkerSize(0.5);

  (*Spline) = new TSpline3("hamamatsu_pde",wl,eff,np,"",0,0);
 
 
}
