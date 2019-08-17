///////////////////////////////////////////////////////////////////////////////

namespace {
  const int npt = 3;

  double voltage [3] = { 72.80, 73.30, 73.55  };
  double q1      [3] = { 71.2 , 102.4, 112.8  };
  double q2      [3] = { 71.6 , 104.8, 120.7  };

  double sigq1   [3] = {  2.4 , 3.4, 4.3 } ;
  double sigq2   [3] = {  2.2 , 3.0, 3.1 } ;

};

//-----------------------------------------------------------------------------
int gain_vs_voltage() {

  char fn[200];

  double x[npt], y[npt], ex[npt], ey[npt];

  for (int i=0; i<npt; i++) {
    ey[i] = 0.;
    ex[i] = 0.;
  }

  TGraphErrors  *g1 = new TGraphErrors(npt,voltage,q1,ex,ey);

  g1->GetYaxis()->SetRangeUser(65,145);
  g1->GetXaxis()->SetTitle("MPPC voltage, V");
  g1->SetTitle("MPPC Photopeak position vs voltage");

  g1->SetMarkerColor(1);
  g1->SetLineColor(1);
  g1->SetMarkerStyle(20);

  g1->Draw("ALP");

  TGraphErrors  *g2 = new TGraphErrors(npt,voltage,q2,ex,ey);
 
  g2->SetMarkerColor(2);
  g2->SetLineColor(2);
  g2->SetMarkerStyle(20);
  g2->SetTitle("channel 2");

  g2->Draw("LP,same");
  
  TLegend *leg = new TLegend(0.7,0.8,0.9,0.9);

  leg->SetBorderSize(0);
  leg->SetFillStyle(0);

  leg->AddEntry(g1,"channel 1","pe");
  leg->AddEntry(g2,"channel 2","pe");


  leg->Draw();
}


//-----------------------------------------------------------------------------
int eres_vs_voltage() {

  char fn[200];

  double x[npt], y1[npt], y2[npt], ex[npt], ey[npt];

  for (int i=0; i<npt; i++) {
    x[i]  = voltage[i];
    y1 [i] = sigq1[i]/q1[i]*2.3;
    y2 [i] = sigq2[i]/q2[i]*2.3;
    ey[i] = 0.;
    ex[i] = 0.;
  }

  TGraphErrors  *g1 = new TGraphErrors(npt,x,y1,ex,ey);

  g1->GetYaxis()->SetRangeUser(0,0.1);
  g1->GetXaxis()->SetTitle("MPPC voltage, V");
  g1->SetTitle("MPPC Photopeak resolution vs voltage, FWHM");

  g1->SetMarkerColor(1);
  g1->SetLineColor(1);
  g1->SetMarkerStyle(20);

  g1->Draw("ALP");

  TGraphErrors  *g2 = new TGraphErrors(npt,x,y2,ex,ey);
 
  g2->SetMarkerColor(2);
  g2->SetLineColor(2);
  g2->SetMarkerStyle(20);
  g2->SetTitle("channel 2");

  g2->Draw("LP,same");
  
  TLegend *leg = new TLegend(0.7,0.8,0.9,0.9);

  leg->SetBorderSize(0);
  leg->SetFillStyle(0);

  leg->AddEntry(g1,"channel 1","pe");
  leg->AddEntry(g2,"channel 2","pe");


  leg->Draw();
}

//-----------------------------------------------------------------------------
int plot_results() {

  TCanvas* c = new_slide("a","a",2,1,1000,600);

  TPad* p1 = (TPad*) c->GetPrimitive("p1");

  p1->cd(1);

  gain_vs_voltage();

  p1->cd(2);

  eres_vs_voltage();

  return 0;
}
