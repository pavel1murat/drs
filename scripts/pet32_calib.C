//

//-----------------------------------------------------------------------------
// fitting function - a gaussian
//-----------------------------------------------------------------------------
double gas(double* x, double*p) {

  double dx  = (x[0]-p[1])/p[2];

  double f   = p[0]*TMath::Exp(-0.5*dx*dx);

  return f;
}

//-----------------------------------------------------------------------------
int pet32_calib(const char* HistFile, int PlotMode = 0) {

  char   hist_name[100], fn[100];

  double peak[100], sigm[100], chi2[100];
  double y, ymax;

  int ibmax;

  TH1F*  h[32];

  TF1*   f[32];

  for (int i=0; i<32; i++) {

    sprintf(hist_name,"event_0/q_%03d",i);

    h[i] = gh1(HistFile,"Pet2Ana",hist_name);

    sprintf(fn,"gas_%02i",i);

    f[i] = new TF1(fn,gas,5,100,3);

    // look for a maximum away from zero

    ibmax = -1;
    ymax  = -1;

    for (int ib=5; ib<100; ib++) {
      y = h[i]->GetBinContent(ib);
      if (y > ymax) {
	ymax  = y;
	ibmax = ib;
      }
    }

    f[i]->SetParameter(0,10);
    f[i]->SetParameter(1,ibmax);
    f[i]->SetParameter(2,1.5);

    f[i]->SetParLimits(2,0,10);

    h[i]->Fit(f[i],"Q","",ibmax-4,ibmax+4);

    peak[i] = f[i]->GetParameter(1);
    sigm[i] = f[i]->GetParameter(2);
    chi2[i] = f[i]->GetChisquare();
  }

  for (int i=0; i<32; i++) {
    printf(" %3i %10.4f %10.4f %10.4f\n",i,peak[i],sigm[i],chi2[i]);
  }

  TCanvas* c;
  TPad*    p1;
  char     name[100];
  int ic;

  if (PlotMode != 0) {
//-----------------------------------------------------------------------------
// plot histograms - fit results - 32 channels, 4 canvases divided by by 4x2
//-----------------------------------------------------------------------------

    for (int i=0; i<4; i++) {
      sprintf(name,"slide_%i",i+1);

      c = new_slide(name,name,2,4,800,1000);
      p1 = (TPad*) c->GetPrimitive("p1");

      for (int j=0; j<8; j++) {
	ic = i*8+j;
	p1->cd(j+1);

	h[ic]->GetXaxis()->SetRangeUser(5,55);
	h[ic]->Draw();
      }
    }
  }

  return 0;
}
