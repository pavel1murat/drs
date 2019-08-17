///////////////////////////////////////////////////////////////////////////////
// these are plots I need for every run 
// fig 1: pulse heights and charges..., mostly pulse heights, charge - resolution
//
///////////////////////////////////////////////////////////////////////////////
void plot_standard_old(const char* Filename, int Figure) {
  TFile* f;
  TCanvas* c;
  TH1F* h;
  TH2F* h2;

  f = (TFile*) gROOT->GetListOfFiles()->FindObject(Filename);

  if (f == 0) {
    f = new TFile(Filename);
  }

  if (Figure == 1) { 
    c = new TCanvas("fig_1","Pulse height, Charge",1200,800);

    c->Divide(2,2);

    c->cd(1);
    h = (TH1F*) f->Get("drs/channel_000/ph");
    h->Draw();

    c->cd(2);
    h = (TH1F*) f->Get("drs/channel_000/q");
    h->GetXaxis()->SetRangeUser(0,399);
    h->Draw();

    c->cd(3);
    h = (TH1F*) f->Get("drs/channel_002/ph");
    h->Draw();

    c->cd(4);
    h = (TH1F*) f->Get("drs/channel_002/q");
    h->GetXaxis()->SetRangeUser(0,399);
     h->Draw();
  }

  if (Figure == 2) { 
    c = new TCanvas("fig_2","Pulse (digital scope)",1000,500);

    c->Divide(2,1);

    c->cd(1);
    h2 = (TH2F*) f->Get("drs/channel_000/v_vs_cell_1");
    h2->Draw();

    c->cd(2);
    h2 = (TH2F*) f->Get("drs/channel_002/v_vs_cell_1");
    h2->Draw();
  }

  if (Figure == 3) {
    // find good event...
    //    x = new drsana("/data/pet01/a/root/2012-01-23-737v-10pf-003.root");
    //    x->LoopTime(1);
    //    x->ProcessEvent(i++,0); x->Display("gaus")
  }

  if (Figure == 4) {
    x->GetChannel(0)->fHist.fShape->GetFunction("f_gaus_ch_000")->Draw("same");
    x->GetChannel(2)->fHist.fShape->GetFunction("f_gaus_ch_002")->Draw("same");
  }

}
