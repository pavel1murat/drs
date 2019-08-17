//-----------------------------------------------------------------------------
int plot_dt2_24_01(const char* HistFile, int PlotMode = 0) {

  char   hist_name[100], fn[100], name[100];

  double peak[100], sigm[100], chi2[100];
  double y, ymax;

  TH2F* h2;

  sprintf(name,"c_plot_dt2_24_01");

  c = new_slide(name,name,2,2,1100,1000);
  p1 = (TPad*) c->GetPrimitive("p1");
  
  p1->cd(1);
  h2 = gh2(HistFile,"Pet2Ana","event_24/seed_mod_0");
  //  h2->GetXaxis()->SetRangeUser(0,15.99);
  h2->SetStats(0);
  h2->Draw("box");

  p1->cd(2);
  h2 = gh2(HistFile,"Pet2Ana","event_24/seed_mod_1");
  //  h2->GetXaxis()->SetRangeUser(16,31.99);
  h2->SetStats(0);
  h2->Draw("box");

  p1->cd(3);
  h2 = gh2(HistFile,"Pet2Ana","gaus/dt2_vs_ich224");
  h2->ProjectionY()->Fit("gaus");

  p1->cd(4);
  h2 = gh2(HistFile,"Pet2Ana","gaus/dt2_vs_ich224");
  h2->GetXaxis()->SetRangeUser(0,15.99);
  h2->SetStats(0);
  h2->Draw("box");


  return 0;
}

//-----------------------------------------------------------------------------
int plot_dt2_24_02(const char* HistFile, int PlotMode = 0) {

  char   hist_name[100], fn[100], name[100];

  double peak[100], sigm[100], chi2[100];
  double y, ymax;

  TH2F* h2;

  sprintf(name,"c_plot_dt2_24_02");

  c = new_slide(name,name,2,3,1000,850);
  p1 = (TPad*) c->GetPrimitive("p1");
  
  h2 = gh2(HistFile,"Pet2Ana","gaus/dt2_vs_ich224");

  p1->cd(1);
  h2->ProjectionY("py_10",10,10)->Rebin(2)->Fit("gaus","w","",-1,1);

  p1->cd(2);
  h2->ProjectionY("py_11",11,11)->Rebin(2)->Fit("gaus","w","",-1,1);

  p1->cd(3);
  h2->ProjectionY("py_12",12,12)->Rebin(2)->Fit("gaus","w","",-1,1);

  p1->cd(4);
  h2->ProjectionY("py_14",14,14)->Rebin(2)->Fit("gaus","w","",-1,1);

  p1->cd(5);
  h2->ProjectionY("py_15",15,15)->Rebin(2)->Fit("gaus","w","",-1,1);

  p1->cd(6);
  h2->ProjectionY("py_16",16,16)->Rebin(2)->Fit("gaus","w","",-1,1);

  return 0;
}
