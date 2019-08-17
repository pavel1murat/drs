///////////////////////////////////////////////////////////////////////////////
// emission spectrum of LaBr3
///////////////////////////////////////////////////////////////////////////////
void plot_labr3_emission_spectrum(const char* Option="alp") {

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
    319,640,     334,639,     350,636, 359,630,     370,601,     388,469,
    404,319,     415,254,     420,237,     424,226,     430,225,
    436,237,     442,259,     452,300,     470,372,     487,400,
    498,402,     509,415,     520,434,     530,460,     540,494,
    550,530,     559,560,     570,585,     580,602,     591,614,
    600,620,     610,626,     620,630,     630,633,     640,635,
    670,636,     690,636,     719,637,     820,640,
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

  TGraph* gr = new TGraph(np,wl,eff);
  gr->SetMarkerStyle(20);
  gr->SetMarkerSize(0.5);

  TSpline3* spl3 = new TSpline3("labr3_emission",wl,eff,np,"",0,0);

  char option[200];

  //  sprintf(option,"alp%s",Opt);
  gr->Draw(Option);

  spl3->SetLineColor(2);
  spl3->Draw("same");

  return;
}


//-----------------------------------------------------------------------------
void plot_emission_spectrum(const char* CrystalName, const char* Option) {

  if (strcmp(CrystalName,"labr3") == 0) {
    plot_labr3_emission_spectrum(Option);
  }
}
