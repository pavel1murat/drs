///////////////////////////////////////////////////////////////////////////////
// these are plots I need for every run 
// fig 1: pulse heights and charges..., mostly pulse heights, charge - resolution
//
///////////////////////////////////////////////////////////////////////////////
void plot_standard(const char* Filename, int Figure) {
  TFile* f;
  TCanvas* c;
  TH1F* h;
  TH2F* h2;

  if (Figure == 1) { 
    c = new TCanvas("fig_1","Pulse height, Charge",1200,800);

    c->Divide(2,2);

    c->cd(1);
    gh1(Filename,"Pet2Ana","event_0/q1_000")->Fit("gaus","","",420,600);

    c->cd(3);
    gh1(Filename,"Pet2Ana","event_0/q1_001")->Fit("gaus","","",420,600);

    c->cd(2);
    gh1(Filename,"Pet2Ana","event_0/vmaxch_000")->Draw();

    c->cd(4);
    gh1(Filename,"Pet2Ana","event_0/vmaxch_001")->Draw();
  }

  if (Figure == 2) { 
    c = new TCanvas("fig_2","gaussian resolution",1000,500);

    c->Divide(2,2);

    c->cd(1);
    h = gh1(Filename,"Pet2Ana","gaus/dt2");
    h->GetXaxis()->SetRangeUser(-1.,1);
    h->Fit("gaus");

    c->cd(3);
    h = gh1(Filename,"Pet2Ana","gaus/dtl_2");
    h->GetXaxis()->SetRangeUser(-1.,1);
    h->Fit("gaus");

    c->cd(2);
    gh1(Filename,"Pet2Ana","gaus/chi2_00")->Draw();

    c->cd(4);
    gh1(Filename,"Pet2Ana","gaus/chi2_01")->Draw();
  }

  if (Figure == 3) {
    c = new TCanvas("fig_3","gaussian width",1000,800);

    c->Divide(2,2);

    c->cd(1);
    h = gh1(Filename,"Pet2Ana","gaus/gsigma_0");
    h->GetXaxis()->SetRangeUser(0.,10);
    h->Draw();

    c->cd(3);
    h = gh1(Filename,"Pet2Ana","gaus/gsigma_1");
    h->GetXaxis()->SetRangeUser(0.,10);
    h->Draw();

    c->cd(2);
    h = gh1(Filename,"Pet2Ana","gaus/base_offset0");
    h->Fit("gaus");

    c->cd(4);
    h = gh1(Filename,"Pet2Ana","gaus/base_offset1");
    h->Fit("gaus");
  }

  if (Figure == 4) {
    x->GetChannel(0)->fHist.fShape->GetFunction("f_gaus_ch_000")->Draw("same");
    x->GetChannel(2)->fHist.fShape->GetFunction("f_gaus_ch_002")->Draw("same");
  }

}
