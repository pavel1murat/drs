//-----------------------------------------------------------------------------
int plot_seed(const char* HistFile, int PlotMode = 0) {

  char   hist_name[100], fn[100], name[100];

  double peak[100], sigm[100], chi2[100];
  double y, ymax;

  TH2F* h2;

  sprintf(name,"c_plot_seed");

  c = new_slide(name,name,2,1,1100,600);
  p1 = (TPad*) c->GetPrimitive("p1");
  
  p1->cd(1);
  h2 = gh2(HistFile,"Pet2Ana","event_0/seed_mod_0");
  h2->SetStats(0);
  h2->Draw("box");

  p1->cd(2);
  h2 = gh2(HistFile,"Pet2Ana","event_0/seed_mod_1");
  h2->SetStats(0);
  h2->Draw("box");

  return 0;
}
