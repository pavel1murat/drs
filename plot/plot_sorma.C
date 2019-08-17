///////////////////////////////////////////////////////////////////////////////
// Figure =   1: Qmax vs channel number , run 35
// Figure =   2: normalized charge (Q_511) vs channel number and resulting resolution
// Figure =   3: channel gains at -72.5V 
// Figure =   4: V(bias) scan runs 35-40
// Figure =   5: V(bias) scan, runs 51-55
// Figure =   6: Compton scatter recovery
//               fn_run_35, entry 36
///////////////////////////////////////////////////////////////////////////////

void plot_sorma(int Figure) {

  const char* fn_run_35 = "hist/srcs01.000035.drs_pet2.hist";
  const char* fn_run_36 = "hist/srcs01.000036.drs_pet2.hist";
  const char* fn_run_37 = "hist/srcs01.000037.drs_pet2.hist";

  if (Figure == 1) {
//-----------------------------------------------------------------------------
// normalized charge vs channel number
//-----------------------------------------------------------------------------
    gStyle->SetOptStat(0);
    TH2F* h2 = gh2(fn_run_35,"Pet2Ana","event_0/qmax_vs_ich");

    h2->GetXaxis()->SetTitle("channel number");
    h2->GetYaxis()->SetTitle("Q, pC");
    h2->GetYaxis()->SetRangeUser(0,49.9);
    h2->SetTitle("");

    TCanvas* c = new TCanvas("fig_1","Fig 1",1000,600);
    h2->Draw("box");
  }

  if (Figure == 2) {
//-----------------------------------------------------------------------------
// normalized charge and resolution
//-----------------------------------------------------------------------------
    gStyle->SetOptStat(0);
    TH2F* h2 = gh2(fn_run_35,"Pet2Ana","event_0/q1max_vs_ich");

    h2->GetXaxis()->SetTitle("channel number");
    h2->GetYaxis()->SetTitle("Q, KeV");
    h2->GetYaxis()->SetRangeUser(0,699.9);
    h2->SetTitle("");

    TCanvas* c = new TCanvas("fig_2","Fig 2",1200,600);
    c->Divide(2,1);
    c->cd(1);
    h2->Draw("box");

    c->cd(2);
    h2->ProjectionY("h_fig_2_py",0,32)->Fit("gaus","","",450,560);
  }

  else if (Figure == 3) {
//-----------------------------------------------------------------------------
// distribution for gains
//-----------------------------------------------------------------------------
    double gain[32] = {
      26.929, 26.796, 26.038, 25.783, 26.264, 26.057, 26.085, 26.249, 26.383, 26.772,
      26.656, 27.017, 27.149, 27.466, 27.383, 26.973, 27.948, 27.904, 27.045, 26.978,
      27.173, 26.821, 25.233, 25.092, 26.990, 26.978, 25.817, 25.534, 26.139, 26.156,
      25.465, 25.653
    };

    TH1F* h = new TH1F("gain","",20,20,30);

    for (int i=0; i<32; i++) {
      h->Fill(gain[i]);
    }

    TCanvas* c = new TCanvas("fig_3","Fig 2",1000,600);
    h->GetXaxis()->SetTitle("Q, pC");
    h->Fit("gaus");

    TPaveLabel* lab;

    TDrsUtils::DrawPaveLabelNDC(lab,"Gain Uniformity:  #pm ~5%",0.15,0.82,0.6,0.9);
  }
  else if (Figure == 4) {
//-----------------------------------------------------------------------------
// gain vs overvoltage runs 35 ... 40
//-----------------------------------------------------------------------------
    int    rn   [6] = {   38 ,    37 ,   35  ,   39  ,   40  ,   36   };
    double curr [6] = {   33.,    92.,   320.,   390.,   382., 1540.  };
    double ov   [6] = { 71.50,  72.00,  72.50,  72.50,  72.50, 73.00  };
    double qmax [6] = { 11.2 ,  17.23,  26.66,  26.66,  26.37, 30.31  };
    double sigq [6] = {  0.91,   1.29,  1.75 ,   1.72,   1.77,  1.94  };
    double qfwhm[6] = { 15.52,  14.13,  12.7 ,  12.26,  12.39, 12.96  };
//-----------------------------------------------------------------------------
// most probable voltages in the seed channels for 2 modules
//-----------------------------------------------------------------------------
    double v0max[6] = { 44.80,  70.23,  99.47,   99.85,  99.89,  111.7   };
    double sigv0[6] = {  5.79,   9.57,  12.94,   13.57,  13.31,   15.24  };
				       	       		              			       
    double v1max[6] = { 42.51,  66.74,  97.78,   98.54,  98.12,  112.4   };
    double sigv1[6] = {  6.14,   9.78,  13.49,   14.04,  13.88,   15.56  };

    double res[6];

    int np = 6;
    TGraph* gr = new TGraph(np, ov, qmax);
    TPaveLabel* lab;

    TCanvas* c = new TCanvas("fig_4","Fig 4",1000,800);
    c->Divide(2,2);

    c->cd(1);
    gr->GetYaxis()->SetTitle("Charge, pQ at 50#Omega");
    gr->GetXaxis()->SetTitle("Voltage, V");
    gr->SetMarkerStyle(20);
    gr->SetTitle("");
    gr->Draw("ALP");
    TDrsUtils::DrawPaveLabelNDC(lab,"Charge (40ns) vs Overvoltage",0.15,0.82,0.6,0.9);

    c->cd(2);
    gr = new TGraph(np, ov, v0max);
    gr->GetYaxis()->SetTitle("Pulse height (mV) at 50#Omega");
    gr->GetXaxis()->SetTitle("Voltage, V");
    gr->SetMarkerStyle(20);
    gr->SetTitle("");
    gr->Draw("ALP");

    gr = new TGraph(np, ov, v1max);
    gr->SetMarkerStyle(24);
    gr->SetTitle("");
    gr->Draw("LP,same");
    TDrsUtils::DrawPaveLabelNDC(lab,"Pulse Height vs Overvoltage",0.15,0.82,0.6,0.9);

    c->cd(3);
    gr = new TGraph(np, ov, curr);
    gr->GetYaxis()->SetTitle("Dark Current (#{mu}A) ");
    gr->GetXaxis()->SetTitle("Voltage, V");
    gr->SetMarkerStyle(20);
    gr->SetTitle("");
    gr->Draw("ALP");
    TDrsUtils::DrawPaveLabelNDC(lab,"Dark current vs Overvoltage",0.15,0.82,0.6,0.9);

    c->cd(4);
    gr = new TGraph(np, ov, qfwhm);
    gr->GetYaxis()->SetTitle("energy resolution, FWHM (%)");
    gr->GetXaxis()->SetTitle("Voltage, V");
    gr->SetMarkerStyle(20);
    gr->SetTitle("");
    gr->Draw("ALP");
    TDrsUtils::DrawPaveLabelNDC(lab,"Energy resolution vs Overvoltage",0.15,0.82,0.6,0.9);
  }
  else if (Figure == 5) {
//-----------------------------------------------------------------------------
// gain vs V(bias) runs 51-55
//-----------------------------------------------------------------------------
    int    rn   [5] = {  54  ,      51 ,    55  ,    52 ,   53    };
    double vb   [5] = {  71.5,    72.0 ,    72.0,  72.50,  72.80  };
    double curr0[5] = {  43.,     142. ,   142. ,   670.,  1470.  };
    double curr1[5] = {  36.,     111. ,   104. ,   570.,  1310.  };
    double qmax0[5] = { 20.00,    31.22,  27.10 ,  38.80,  39.68  };
    double sigq0[5] = {  1.00,     1.37,   1.37 ,   1.53,  1.75   };
    double qmax1[5] = { 17.59,    28.28,  22.17 ,  36.63,  38.05  };
    double sigq1[5] = {  0.95,     1.43,   1.21 ,   1.60,  1.69   };
//-----------------------------------------------------------------------------
// most probable voltages in the seed channels for 2 modules
// 0 : channel_7, 1:channel_27
//-----------------------------------------------------------------------------
    double v0max[5] = { 86.55, 137.00, 117.6 ,  170.2 , 175.1   };
    double sigv0[5] = {  9.19,  14.19,  12.41,   17.82,  18.62  };
				       	       		              			       
    double v1max[5] = { 73.53,  120.1,  91.75,  155.0,  163.2  };
    double sigv1[5] = {  7.89,  12.13,  11.00,   15.89,  16.8  };

    double qres0[5], qres1[5];

    int np = 5;

    for (int i=0; i<5; i++) {
      qres0[i] = sigq0[i]/qmax0[i]*2.35;
      qres1[i] = sigq1[i]/qmax1[i]*2.35;
    }


    TGraph* gr = new TGraph(np, vb, qmax0);
    TPaveLabel* lab;

    TCanvas* c = new TCanvas("fig_5","Fig 5",1000,800);
    c->Divide(2,2);

    c->cd(1);
    gr->GetYaxis()->SetTitle("Charge, pQ at 50#Omega");
    gr->GetXaxis()->SetTitle("V^{bias}, V");
    gr->SetMarkerStyle(20);
    gr->SetTitle("");
    gr->Draw("ALP");

    TGraph* gr = new TGraph(np, vb, qmax1);
    gr->SetMarkerStyle(24);
    gr->SetTitle("");
    gr->Draw("LP");

    TDrsUtils::DrawPaveLabelNDC(lab,"Charge (40ns) vs V^{bias}",0.15,0.82,0.6,0.9);

    c->cd(2);
    gr = new TGraph(np, vb, v0max);
    gr->GetYaxis()->SetTitle("Pulse height (mV) at 50#Omega");
    gr->GetXaxis()->SetTitle("Voltage, V");
    gr->SetMarkerStyle(20);
    gr->SetTitle("");
    gr->Draw("ALP");

    gr = new TGraph(np, vb, v1max);
    gr->SetMarkerStyle(24);
    gr->SetTitle("");
    gr->Draw("LP,same");
    TDrsUtils::DrawPaveLabelNDC(lab,"Pulse Height vs V^{bias}",0.15,0.82,0.6,0.9);

    c->cd(3);
    gr = new TGraph(np, vb, curr0);
    gr->GetYaxis()->SetTitle("Dark Current (#{mu}A) ");
    gr->GetXaxis()->SetTitle("Voltage, V");
    gr->SetMarkerStyle(20);
    gr->SetTitle("");
    gr->Draw("ALP");

    TGraph* gr = new TGraph(np, vb, curr1);
    gr->SetMarkerStyle(24);
    gr->SetTitle("");
    gr->Draw("LP");

    TDrsUtils::DrawPaveLabelNDC(lab,"Dark current vs V^{bias}",0.15,0.82,0.6,0.9);

     c->cd(4);
     gr = new TGraph(np, vb, qres0);
     gr->GetYaxis()->SetTitle("energy resolution, FWHM (%)");
     gr->GetXaxis()->SetTitle("Voltage, V");
     gr->SetMarkerStyle(20);
     gr->SetTitle("");
     gr->Draw("ALP");
     
     TGraph* gr = new TGraph(np, vb, qres1);
     gr->SetMarkerStyle(24);
     gr->SetTitle("");
     gr->Draw("LP");

     TDrsUtils::DrawPaveLabelNDC(lab,"Energy resolution vs V^{bias}",0.15,0.82,0.6,0.9);
  }

  if (Figure == 6) {
//-----------------------------------------------------------------------------
// COMPTON recovery
//-----------------------------------------------------------------------------
    TH1F    *h0, *h1, *h2; 
    TPaveLabel  *lab;
    TCanvas* c = new TCanvas("fig_6","Fig 6",1200,800);
    c->Divide(2,2);

    gStyle->SetOptStat(0);

    float qmax = 599.9;

    c->cd(1);
    h1 = gh1(fn_run_35,"Pet2Ana","event_0/p0_q1max");
    h1->GetXaxis()->SetTitle("channel number");
    h1->GetYaxis()->SetTitle("Q, KeV");
    h1->GetYaxis()->SetRangeUser(0,qmax);
    h1->SetTitle("");
    h1->Draw();
    TDrsUtils::DrawPaveLabelNDC(lab,"V^{bias} = -72.5 V",0.15,0.82,0.6,0.9);

    c->cd(2);
    h0 = gh1(fn_run_35,"Pet2Ana","event_0/p0_q1sum_0");
    h0->GetXaxis()->SetTitle("channel number");
    h0->GetYaxis()->SetTitle("Q, KeV");
    h0->GetYaxis()->SetRangeUser(0,qmax);
    h0->SetTitle("");
    h0->Draw();

    h1 = gh1(fn_run_35,"Pet2Ana","event_0/p0_q1sum_1");
    h1->SetFillStyle(3001);
    h1->SetFillColor(42);
    h1->Draw("same");
    TDrsUtils::DrawPaveLabelNDC(lab,"V^{bias} = -72.5 V",0.15,0.82,0.6,0.9);

    c->cd(3);
    h1 = gh1(fn_run_36,"Pet2Ana","event_0/p0_q1max");
    h1->GetXaxis()->SetTitle("channel number");
    h1->GetYaxis()->SetTitle("Q, KeV");
    h1->GetYaxis()->SetRangeUser(0,qmax);
    h1->SetTitle("");
    h1->Draw();
    TDrsUtils::DrawPaveLabelNDC(lab,"V^{bias} = -73.0 V",0.15,0.82,0.6,0.9);

    c->cd(4);
    h0 = gh1(fn_run_36,"Pet2Ana","event_0/p0_q1sum_0");
    h0->GetXaxis()->SetTitle("channel number");
    h0->GetYaxis()->SetTitle("Q, KeV");
    h0->GetYaxis()->SetRangeUser(0,qmax);
    h0->SetTitle("");
    h0->Draw();

    h1 = gh1(fn_run_36,"Pet2Ana","event_0/p0_q1sum_1");
    h1->SetFillStyle(3001);
    h1->SetFillColor(42);
    h1->Draw("same");
    TDrsUtils::DrawPaveLabelNDC(lab,"V^{bias} = -73.0 V",0.15,0.82,0.6,0.9);
  }
}


//-----------------------------------------------------------------------------
void print_sorma(int Figure) {

  char name [200], fn[200];

  sprintf(name,"fig_%i",Figure);

  TCanvas* c = (TCanvas*) gROOT->FindObject(name);

  sprintf(fn,"fig_%i.png",Figure);
  c->Print(fn);

  sprintf(fn,"fig_%i.eps",Figure);
  c->Print(fn);
}
