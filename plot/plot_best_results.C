//

void plot_resolution() {
  TPaveLabel* lab;
  TFile* f = new TFile("/home/murat/drs4/hist/2012-01-23-737v-10pf-003.hist");


  TCanvas* c = new TCanvas("c","c",1200,400);

  c->Divide(3,1);

  c->cd(1);
  //  TH1F* h_dt2 =  (TH1F*) f->Get("Ana/drs/Hist/gaus/dt2");
  TH1F* h_dt2 =  (TH1F*) f->Get("drs/Hist/gaus/dt2");

  TH1F* h2 = h_dt2->Rebin(4);
  h2->SetMarkerStyle(20);
  
  h2->GetXaxis()->SetRangeUser(-1.,1.5);
  h2->GetXaxis()->SetTitle("#sigma_{T_{1}-T_{2}}, ns");
  h2->GetXaxis()->SetTitleOffset(0.8);
  h2->GetXaxis()->SetTitleSize(0.05);

  h2->GetYaxis()->SetTitle("N / 40 psec");
  h2->GetYaxis()->SetTitleSize(0.05);

  gStyle->SetOptStat(000000);
  gStyle->SetOptFit(11);

  h2->SetTitle("");
  h2->Fit("gaus","qw","pe",-0.4,0.5);
  TDrsUtils::DrawPaveLabelNDC(lab,"photopeak",0.15,0.82,0.6,0.9);

  c->cd(2);

  TH1F* h_dt6 =  (TH1F*) f->Get("Ana/drs/Hist/gaus/dt6");

  TH1F* h6 = h_dt6->Rebin(4);
  h6->SetMarkerStyle(20);
  
  h6->GetXaxis()->SetRangeUser(-1.,1.5);
  h6->GetXaxis()->SetTitle("#sigma_{T_{1}-T_{2}}, ns");
  h6->GetXaxis()->SetTitleOffset(0.8);
  h6->GetXaxis()->SetTitleSize(0.05);

  //  gStyle->SetOptStat(1);
  gStyle->SetOptFit(11);

  h6->GetYaxis()->SetTitle("N / 40 psec");
  h6->GetYaxis()->SetTitleSize(0.05);

  h6->SetTitle("");
  h6->Fit("gaus","qw","pe",-0.4,0.5);
  TDrsUtils::DrawPaveLabelNDC(lab,"photopeak",0.15,0.82,0.6,0.9);
  TDrsUtils::DrawPaveLabelNDC(lab,"#chi^{2} < 30",0.15,0.74,0.6,0.82);

  c->cd(3);

  TH1F* h_dt7 =  (TH1F*) f->Get("Ana/drs/Hist/gaus/dt7");

  TH1F* h7 = h_dt7->Rebin(4);
  h7->SetMarkerStyle(20);
  
  h7->GetXaxis()->SetRangeUser(-1.,1.5);
  h7->GetXaxis()->SetTitleOffset(0.8);
  h7->GetXaxis()->SetTitleSize(0.05);
  h7->GetXaxis()->SetTitle("#sigma_{T_{1}-T_{2}}, ns");

  //  gStyle->SetOptStat(1);
  gStyle->SetOptFit(11);

  h7->GetYaxis()->SetTitle("N / 40 psec");
  h7->GetYaxis()->SetTitleSize(0.05);

  h7->SetTitle("");
  h7->Fit("gaus","qw","pe",-0.4,0.5);
  TDrsUtils::DrawPaveLabelNDC(lab,"photopeak",0.15,0.82,0.6,0.9);
  TDrsUtils::DrawPaveLabelNDC(lab,"#chi^{2} < 20",0.15,0.74,0.6,0.82);


}
//-----------------------------------------------------------------------------
void plot_chi2() {
  TPaveLabel  *lab(0);

  TFile* f = new TFile("/home/murat/drs4/hist/2012-01-23-737v-10pf-003.hist");

  TCanvas* c = new TCanvas("c","c",800,600);

  TH1F* h_chi20 =  (TH1F*) f->Get("Ana/drs/Hist/gaus/chi2_00");

  TH1F* h0 = h_chi20->Rebin(10);
  //  h2->SetMarkerStyle(20);
  
  h0->GetXaxis()->SetTitle("fit #chi^{2}");
  h0->GetXaxis()->SetTitleOffset(0.90);
  h0->GetXaxis()->SetTitleSize(0.05);

  h0->GetYaxis()->SetTitle("N / 5");
  h0->GetYaxis()->SetTitleSize(0.05);

  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  h0->SetTitle("");
  h0->Draw();

  TH1F* h_chi21 =  (TH1F*) f->Get("Ana/drs/Hist/gaus/chi2_02");

  TH1F* h1 = h_chi21->Rebin(10);

  h1->SetLineColor(4);
  h1->Draw("same");


  TArrow* a1 = new TArrow(20,15,20,5);
  a1->SetArrowSize(0.015);
  a1->Draw();

  TArrow* a2 = new TArrow(30,15,30,5);
  a2->SetArrowSize(0.015);
  a2->Draw();

  TLegend *leg = new TLegend(0.7,0.75,0.9,0.9);

  leg->AddEntry(h0,"channel 0","f");
  leg->AddEntry(h1,"channel 1","f");
  
  leg->SetFillStyle(0);
  leg->Draw();

  TDrsUtils::DrawPaveLabelNDC(lab,"Leading edge fit #chi^{2}",0.30,0.83,0.85,0.9);
}


//-----------------------------------------------------------------------------
void plot_charge() {
  TFile* f = new TFile("/home/murat/drs4/hist/2012-01-23-737v-10pf-003.hist");

  TCanvas* c = new TCanvas("c","c",1000,400);

  gStyle->SetOptStat(0);
  gStyle->SetOptFit(10001);

  c->Divide(2,1);

  c->cd(1);

  TH1F* h_q0 =  (TH1F*) f->Get("Ana/drs/Hist/channel_000/q");

  h_q0->GetXaxis()->SetTitle("pulse charge, arbitrary units");
  h_q0->GetXaxis()->SetRangeUser(0,299);

  h_q0->SetTitle("channel #0");
  h_q0->Fit("gaus","","",120,200);

  c->cd(2);
  TH1F* h_q1 =  (TH1F*) f->Get("Ana/drs/Hist/channel_002/q");

  h_q1->GetXaxis()->SetTitle("pulse charge, arbitrary units");
  h_q1->GetXaxis()->SetRangeUser(0,299);

  h_q1->SetTitle("channel #1");
  h_q1->Fit("gaus","","",120,200);

}


//-----------------------------------------------------------------------------
void plot_q1() {

  TAxis   *xa, *ya, *ya1;

  TFile* f = new TFile("/home/murat/drs4/hist/2012-01-23-737v-10pf-003.hist");

  TPaveLabel  *lab(0);

  TCanvas* c = new TCanvas("c","c",1000,400);

  gStyle->SetOptStat(0);
  gStyle->SetOptFit(10001);

  gStyle->SetStatX(0.90);
  gStyle->SetStatY(0.90);
  gStyle->SetStatW(0.15);
  gStyle->SetStatH(0.15);

  c->Divide(2,1);

  c->cd(1);

  TH1F* h_q0 =  (TH1F*) f->Get("Ana/drs/Hist/channel_000/q1");
  h_q0->Rebin(2);

  h_q0->SetTitle("");
  xa = h_q0->GetXaxis();
  ya = h_q0->GetYaxis();

  xa->SetTitle("pulse charge, KeV");
  xa->SetTitleSize(0.05);

  ya->SetTitle("events / 10 KeV");
  ya->SetTitleSize(0.05);

  h_q0->Fit("gaus","","",420,600);

  TDrsUtils::DrawPaveLabelNDC(lab,"channel 0",0.15,0.82,0.45,0.90);

  c->cd(2);
  TH1F* h_q1 =  (TH1F*) f->Get("Ana/drs/Hist/channel_002/q1");
  h_q1->Rebin(2);

  xa  = h_q1->GetXaxis();
  ya1 = h_q1->GetYaxis();

  xa->SetTitle("pulse charge, KeV");
  xa->SetTitleSize(0.05);

  ya1->SetTitle("events / 10 KeV");
  ya1->SetTitleSize(0.05);

  h_q1->SetTitle("");
  h_q1->Fit("gaus","","",420,600);

  TDrsUtils::DrawPaveLabelNDC(lab,"channel 1",0.15,0.82,0.45,0.90);
}


//-----------------------------------------------------------------------------
void plot_leading_edge() {
  TFile* f = new TFile("/home/murat/drs4/hist/2012-01-23-737v-10pf-003.hist");

  TCanvas* c = new TCanvas("c","c",1000,700);

  TPaveLabel  *lab(0);

  TPad* p = new TPad("p","p",0.4,0.4,0.95,0.95);

  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);


  c->cd();


  TH1F* h =  (TH1F*) f->Get("Ana/drs/Hist/channel_000/shape");

  
  h->GetXaxis()->SetTitle("DRS4 cell number");
  h->GetXaxis()->SetRangeUser(300,1024);

  h->GetYaxis()->SetTitle("Voltage, relative units / 200 psec");

  h->SetTitle("");
  h->Draw();

  TH1F* h1 = (TH1F*) h->Clone("h1");

  h1->GetXaxis()->SetRangeUser(340,399);
  h1->GetYaxis()->SetTitle("");

  h1->SetTitle("");


  gStyle->SetOptFit(1000001);
  p->Draw();
  p->cd();

  h1->Draw();
  //  p->Draw();

  TF1* fun = new TF1("fun","0.851*exp(-0.5*(x-372.9)^2/3.888^2)",360,370);

  fun->Draw("same");

  c->cd();
  TDrsUtils::DrawPaveLabelNDC(lab,"Digitized Waveform",0.12,0.84,0.45,0.90);
}
