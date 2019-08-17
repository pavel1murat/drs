//-----------------------------------------------------------------------------
int plot_qmax(const char* HistFile, int PlotMode = 0) {

  char   hist_name[100], fn[100], name[100];

  double peak[100], sigm[100], chi2[100];
  double y, ymax;

  TH2F* h2;

  sprintf(name,"c_plot_qmax");

  c = new_slide(name,name,2,2,1000,800);
  p1 = (TPad*) c->GetPrimitive("p1");
  
  p1->cd(1);
  h2 = gh2(HistFile,"Pet2Ana","event_0/qmax_vs_ich");
  h2->GetYaxis()->SetRangeUser(0,60);
  h2->Draw("box");

  p1->cd(2);
  gh2(HistFile,"Pet2Ana","event_0/q1max_vs_ich")->Draw("box");

  p1->cd(3);
  gh1(HistFile,"Pet2Ana","event_0/q1max_mod_0")->Fit("gaus","w","",460,580);

  p1->cd(4);
  gh1(HistFile,"Pet2Ana","event_0/q1max_mod_1")->Fit("gaus","w","",460,580);

  return 0;
}
