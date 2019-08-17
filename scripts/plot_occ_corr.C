//-----------------------------------------------------------------------------
int plot_occ_corr(const char* HistFile, int PlotMode = 0) {

  char cname[100], hist_name[100];

  TCanvas* c;
  TPad*    p1;
  TH2F*    h2;

  sprintf(cname,"module_0");
  c = new_slide(cname,cname,4,4,1000,1000);
  p1 = (TPad*) c->GetPrimitive("p1");

  for (int ix=0; ix<4; ix++) {
    for (int iy=0; iy<4; iy++) {

      p1->cd(ix+4*iy+1);
     
      sprintf(hist_name,"event_1/occ1_%i_%i",ix,iy);

      h2 = gh2(HistFile,"Pet2Ana",hist_name);
      h2->SetStats(0);
      h2->Draw("box");
    }
  }

  sprintf(cname,"module_1");
  c = new_slide(cname,cname,4,4,1000,1000);
  p1 = (TPad*) c->GetPrimitive("p1");

  for (int ix=0; ix<4; ix++) {
    for (int iy=0; iy<4; iy++) {

      p1->cd(ix+4*iy+1);
     
      sprintf(hist_name,"event_1/occ2_%i_%i",ix,iy);

      h2 = gh2(HistFile,"Pet2Ana",hist_name);
      h2->SetStats(0);
      h2->Draw("box");
    }
  }

  return 0;
}
