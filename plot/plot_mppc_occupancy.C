//

//-----------------------------------------------------------------------------
int plot_mppc_occupancy(const char* Filename) {

  TH2F* h20 = gh2(Filename,"Pet2Ana","event_0/seed_mod_0");

  TH2F* h21 = gh2(Filename,"Pet2Ana","event_0/seed_mod_1");


  TCanvas* c1 = new_slide("mppc_occupancy","MPPC matrix occupancy",2,1,1000,500);

  TPad* p1 = (TPad*) c1->GetPrimitive("p1");

  p1->cd(1); h20->Draw("box");
  p1->cd(2); h21->Draw("box");

}

//-----------------------------------------------------------------------------
int plot_qmax(const char* Filename) {

  TH1F* h10 = gh1(Filename,"Pet2Ana","event_0/qmax_mod_0");

  TH1F* h11 = gh1(Filename,"Pet2Ana","event_0/qmax_mod_1");


  TCanvas* c1 = new_slide("qmax_mod","Q(MAX) for different modules",2,1,1000,500);

  TPad* p1 = (TPad*) c1->GetPrimitive("p1");

  p1->cd(1); 
  h10->GetXaxis()->SetRangeUser(0,49.9);
  h10->Fit("gaus","","",17,30);


  p1->cd(2); 
  h11->GetXaxis()->SetRangeUser(0,49.9);
  h11->Fit("gaus","","",16,30);
}

//-----------------------------------------------------------------------------
int plot_qmax_vs_channel(const char* Filename) {

  TH2F* h20 = gh2(Filename,"Pet2Ana","event_0/qmax_vs_ich");

  TCanvas* c1 = new_slide("qmax_mod","Q(MAX) for different modules",1,1,1000,500);

  TPad* p1 = (TPad*) c1->GetPrimitive("p1");

  p1->cd(1); 
  h20->GetYaxis()->SetRangeUser(0,49.9);
  h20->Draw("box");

}


//-----------------------------------------------------------------------------
int plot_occ12(const char* Filename) {

  TH2F* h20 = gh2(Filename,"Pet2Ana","event_0/seed_mod_0");

  TH2F* h21 = gh2(Filename,"Pet2Ana","event_0/seed_mod_1");

  int ipad;

  char name[200];

  TCanvas* c1 = new_slide("occ1","MPPC OCC1",4,4,1000,1000);

  TPad* p1 = (TPad*) c1->GetPrimitive("p1");

  for (int ix=0; ix<4; ix++) {
    for (int iy=0; iy<4; iy++) {
      ipad = ix+4*iy+1;

      sprintf(name,"event_1/occ1_%i_%i",ix,iy);
      
      p1->cd(ipad); 
      gh2(Filename,"Pet2Ana",name)->Draw("box");
    }
  }

  TCanvas* c2 = new_slide("occ2","MPPC OCC2",4,4,1000,1000);

  p1 = (TPad*) c2->GetPrimitive("p1");

  for (int ix=0; ix<4; ix++) {
    for (int iy=0; iy<4; iy++) {
      ipad = ix+4*iy+1;

      sprintf(name,"event_1/occ2_%i_%i",ix,iy);
      
      p1->cd(ipad); 
      gh2(Filename,"Pet2Ana",name)->Draw("box");
    }
  }

  return 0;
}

