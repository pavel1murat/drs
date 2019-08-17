//-----------------------------------------------------------------------------
int plot_dt2_vs_ich(const char* HistFile, int PlotMode = 0) {

  char   hist_name[100], fn[100], name[100];

  double peak[100], sigm[100], chi2[100];
  double y, ymax;

  TH2F* h2;

  sprintf(name,"c_plot_dt2_vs_ich");

  c = new_slide(name,name,2,2,1100,1000);
  p1 = (TPad*) c->GetPrimitive("p1");
  
  p1->cd(1);
  h2 = gh2(HistFile,"Pet2Ana","gaus/dt2_vs_ich1");
  h2->GetXaxis()->SetRangeUser(0,15.99);
  h2->SetStats(0);
  h2->Draw("box");

  p1->cd(3);
  h2 = gh2(HistFile,"Pet2Ana","gaus/dt2_vs_ich2");
  h2->GetXaxis()->SetRangeUser(16,31.99);
  h2->SetStats(0);
  h2->Draw("box");

  p1->cd(2);
  h2 = gh2(HistFile,"Pet2Ana","exp1/dt2_vs_ich1");
  h2->GetXaxis()->SetRangeUser(0,15.99);
  h2->SetStats(0);
  h2->Draw("box");

  p1->cd(4);
  h2 = gh2(HistFile,"Pet2Ana","exp1/dt2_vs_ich2");
  h2->GetXaxis()->SetRangeUser(16,31.99);
  h2->SetStats(0);
  h2->Draw("box");


  return 0;
}
