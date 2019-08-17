///////////////////////////////////////////////////////////////////////////////
// plots for cdf-10008
// -------------------------
// directory with the histogram files: by default: $WORK_DIR/results
// can be redefined with "zzx.HistDir" in .rootrc
// 
// figures:
// ---------
// Fig 1: residuals DY0 and DY1 as a function of Z 
// Fig 2: DY2rec and DZ2rec vs Ytru and Ztrue
// Fig 3: non-linearity corrections
// Fig 4: corrected residuals
// Fig 5: DY2 and DZ2 resolutions, averaged over the volume
// Fig 6: 2D and 1d plots for one event for 20x20mm crystal and 2x2mm SiPM
// Fig 7: Y4 resolution 
// Fig 8: sigma (xy)[0] vs ZT
// Fig 9: dz3 vs Zt
//
// Fig 12: Y-resolution vs SIPM size for 20x20x10mm3 and 20x20x20mm3 crystals
// Fig 13: Z-resolution , Z2C, vs SIPM size for 20x20xL  and 20x20xL crystals
// Fig 14: Y-resolution , Y4C, vs SIPM size for 20x20xL  and 20x20xL crystals
//
// Fig 21: DY2 vs Y2R
//
// Fig 71: DY2:YT and DY2:YR2
////////////////////////////////////////////////////////////////////////////////
#include "plot/TNote001.hh"
#include "TPaveLabel.h"
#include "TArrow.h"
#include "TObjArray.h"
#include "TObjString.h"
#include <iostream>

ClassImp(TNote001)

//_____________________________________________________________________________
TNote001::TNote001(int PlotMode, int BlessingMode): 
  TPlotCdfNote(PlotMode,BlessingMode) {

  const char* hist_dir;
  char         res_dir[500];
  
  fFiguresDir   = Form("%s/mcrystal",gEnv->GetValue("drs.Figures","."));
  fWorkDir      = gSystem->Getenv("WORK_DIR");

  sprintf(res_dir,"%s/mcrystal",fWorkDir.Data());
  hist_dir      = gEnv->GetValue("drs.HistDir",res_dir);

  fModule       = "mcrystal";

};

//_____________________________________________________________________________
TNote001::~TNote001() {
};

//-----------------------------------------------------------------------------
const char* TNote001::GetFiguresDir() {
  return fFiguresDir.Data();
}

//-----------------------------------------------------------------------------
// determine name of the .eps and .gif files in the ./figures directory
// today's default set is 'frr_03', all the plots should be coming from it
//-----------------------------------------------------------------------------
void TNote001::remake_plots() {

  int fig [] = { 610, 

		 -1
  };

  int figi;

  for (int i=0; fig[i]>0; i++) {
    figi = fig[i];
    plot (figi); 
    print(figi); 
  }
}


//-----------------------------------------------------------------------------
void TNote001::plot_y_res(int Iy) {

  char fn[100];

  int ix = 0;

  TH1F* h;

  TF1* f;

  double yfit[10], sigy[10], z[10], sigz[10];

  for (int iz=0; iz<9; iz++) {
    sprintf(fn,"hist/mcrystal/mcrystal_zscan_%02i_%02i_%02i.hist",ix,Iy,iz);
    h = gh1(fn,fModule.Data(),"evt_0/yrec_2");

    h->Fit("gaus");

    f = h->GetFunction("gaus");

    yfit[iz] = f->GetParameter(1);
    sigy[iz] = f->GetParameter(2);

    z[iz]    = iz/10.;
    sigz[iz]  = 0;
  }

  TH2F* h2 = new TH2F("h2","Reconstructed Y and #sigma(Y) vs Z",1,0,1,100,-1,1);

  TGraphErrors* gr_y = new TGraphErrors(10,z,yfit,sigz,sigy);

  h2->SetStats(0);
  h2->Draw();

  gr_y->SetTitle("");

  gr_y->Draw("LP,same");
  
}

//-----------------------------------------------------------------------------
// plot Y resolution from the Z scan - plot reconstructed coordinate as 
// a function of true one
//-----------------------------------------------------------------------------
void TNote001::plot_z_res(int Iy) {

  char fn[100];

  int ix = 0;
  //  int iy = 0;

  TH1F* h;

  TF1* f;

  double zfit[10], sigz[10], z[10], ez[10];

  for (int iz=0; iz<9; iz++) {
    sprintf(fn,"hist/mcrystal/mcrystal_zscan_%02i_%02i_%02i.hist",ix,Iy,iz);
    h = gh1(fn,fModule.Data(),"evt_0/zrec_2");

    h->Fit("gaus");

    f = h->GetFunction("gaus");

    zfit[iz] = f->GetParameter(1);
    sigz[iz] = f->GetParameter(2);

    z[iz]    = iz/10.;
    ez[iz]  = 0;
  }

  TH2F* hz = new TH2F("hz",Form("Reconstructed Z and #sigma(Z) vs Z, IY=%i",Iy),1,0,1,100,0.,1);

  TGraphErrors* gr_z = new TGraphErrors(10,z,zfit,ez,sigz);

  hz->SetStats(0);
  hz->Draw();

  gr_z->SetTitle("gr_z");

  gr_z->Draw("LP,same");
  
}

//_____________________________________________________________________________
void TNote001::plot(Int_t Figure, const char* CanvasName) {
//-----------------------------------------------------------------------------
//  make sure all the needed scripts are loaded
//-----------------------------------------------------------------------------
  char        macro[200];

  const char* script[] = { 
//    "plot/make_fake_rate_hist.C",
//    "plot/make_ratio_hist.C",
//    "plot/plot_ntrk10.C",
    0 
  };

  const char* work_dir = gSystem->Getenv("PWD");

  for (int i=0; script[i] != 0; i++) {
    sprintf(macro,"%s/murat/%s",work_dir,script[i]);
    if (! gInterpreter->IsLoaded(macro)) gInterpreter->LoadMacro(macro);
  }
//-----------------------------------------------------------------------------
  char        name[200], title[200];

  //  int old_opt_stat = gStyle->GetOptStat();
  
  if ((! CanvasName) || (CanvasName == "")) {
    sprintf(name,"fig_%i",Figure);
    sprintf(title,"figure_%i",Figure);
  }
  else {
    sprintf(name,"%s",CanvasName);
    sprintf(title,"figure %i",Figure);
  }
//-----------------------------------------------------------------------------
//  colors: black in 
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Fig 1: residuals DY0 and DY1 as a function of Z 
//-----------------------------------------------------------------------------
  if (Figure == 1) {

    char fn[100];
    TH2F*   h2;

    sprintf(fn,"hist/mcrystal/mcrystal_uniform_lyso_20_20_20_sipm_04_04.hist");

    
    
    fCanvas  = new_slide(name,title,2,1,1000,500);
    fP1      = (TPad*) fCanvas->GetPrimitive("p1");

					// reconstructed Y
    fP1->cd(1);
    h2 = gh2(fn,fModule.Data(),"evt_0/dy0_vs_zt");
    h2->Draw();
					// reconstructed Z
    fP1->cd(2);
    plot_z_res(0);
  }
//-----------------------------------------------------------------------------
// Fig 2: DY2rec and DZ2rec vs Ytru and Ztrue
//----------------------------------------------------------------------------- 
  if (Figure == 2) {
    char fn[100];
    TH2F*   h2;

    sprintf(fn,"hist/mcrystal/mcrystal_uniform_lyso_20_20_20_sipm_04_04.hist");

    fCanvas  = new_slide(name,title,2,2,1000,800);
    fP1      = (TPad*) fCanvas->GetPrimitive("p1");

					// DY:Y
    fP1->cd(1);
    h2 = gh2(fn,fModule.Data(),"evt_0/dy2_vs_yt");
    h2->Draw();
					// DY:Z
    fP1->cd(2);
    h2 = gh2(fn,fModule.Data(),"evt_0/dy2_vs_zt");
    h2->Draw();
					// DZ2:Y
    fP1->cd(3);
    h2 = gh2(fn,fModule.Data(),"evt_0/dz2_vs_yt");
    h2->Draw();
					// DY2:Z
    fP1->cd(4);
    h2 = gh2(fn,fModule.Data(),"evt_0/dz2_vs_zt");
    h2->Draw();
  }
    
//-----------------------------------------------------------------------------
// Fig 3: non-linearity corrections
//----------------------------------------------------------------------------- 
  if (Figure == 3) {
    char fn[100];
    TH2F   *hy, *hz;
    TH1D   *hy_px, *hz_px;

    sprintf(fn,"hist/mcrystal/mcrystal_uniform_lyso_20_20_20_sipm_04_04.hist");

    fCanvas  = new_slide(name,title,2,2,1000,800);
    fP1      = (TPad*) fCanvas->GetPrimitive("p1");

					// DY:Yrec
    fP1->cd(1);
    hy = gh2(fn,fModule.Data(),"evt_0/dy2_vs_yr2");
    hy->Draw();
					// DZ:Zrec
    fP1->cd(2);
    hz = gh2(fn,fModule.Data(),"evt_0/dz2_vs_zr2");
    hz->Draw();
					// DZ2:Y
    fP1->cd(3);
    hy_px = hy->ProfileX();
    hy_px->Draw();
					// DY2:Z
    fP1->cd(4);
    hz_px = hz->ProfileX();
    hz_px->Draw();
  }
    
//-----------------------------------------------------------------------------
// Fig 4: corrected residuals
//----------------------------------------------------------------------------- 
  if (Figure == 4) {
    char fn[100];
    TH2F   *hy, *hz;
    TH1D   *hy_px, *hz_px;

    sprintf(fn,"hist/mcrystal/mcrystal_uniform_lyso_20_20_20_sipm_04_04.hist");

    fCanvas  = new_slide(name,title,2,2,1000,800);
    fP1      = (TPad*) fCanvas->GetPrimitive("p1");

					// DY:Yrec
    fP1->cd(1);
    hy = gh2(fn,fModule.Data(),"evt_0/dy2c_vs_yr2");
    hy->Draw();
					// DZ:Zrec
    fP1->cd(2);
    hz = gh2(fn,fModule.Data(),"evt_0/dz2c_vs_zr2");
    hz->Draw();
					// DZ2:Y
    fP1->cd(3);
    hy_px = hy->ProfileX();
    hy_px->Draw();
					// DY2:Z
    fP1->cd(4);
    hz_px = hz->ProfileX();
    hz_px->Draw();
  }
//-----------------------------------------------------------------------------
// Fig 5: resolutions, averaged over the volume
//----------------------------------------------------------------------------- 
  if (Figure == 5) {
    char fn[100];
    TH2F   *hy, *hz;
    TH1D   *hy_py, *hz_py;

    sprintf(fn,"hist/mcrystal/mcrystal_uniform_lyso_20_20_20_sipm_04_04.hist");

    fCanvas  = new_slide(name,title,2,1,1000,500);
    fP1      = (TPad*) fCanvas->GetPrimitive("p1");

					// DY:Yrec
    fP1->cd(1);
    hy_py = gh2(fn,fModule.Data(),"evt_0/dy2c_vs_yr2")->ProjectionY();
    hy_py->Fit("gaus","w",""); // ,-0.15,0.15);
					// DZ:Zrec
    fP1->cd(2);
    hz_py = gh2(fn,fModule.Data(),"evt_0/dz2c_vs_zr2")->ProjectionY();
    hz_py->Fit("gaus","w"); // ,"",-0.2,0.2);
  }
//-----------------------------------------------------------------------------
// Fig 6: 2D and 1d plots for one event for 20x20mm crystal and 2x2mm SiPM
//----------------------------------------------------------------------------- 
  if (Figure == 6) {
    char fn[100];
    TH2F   *h0, *h3;
    TH1D   *h1;

    sprintf(fn,"hist/mcrystal/mcrystal_uniform_lyso_20_20_20_sipm_04_04.hist");

    fCanvas  = new_slide(name,title,2,2,800,800);
    fP1      = (TPad*) fCanvas->GetPrimitive("p1");

					// Z = +5mm side
    fP1->cd(1);
    h0 = gh2(fn,fModule.Data(),"evt_0/y_vs_x_0");
    h0->SetStats(0);
    h0->Draw("box"); // can make it "Lego2"
					// Z = -5mm side
    fP1->cd(2);
    h3 = gh2(fn,fModule.Data(),"evt_0/y_vs_x_3");
    h3->SetStats(0);
    h3->Draw("box");

    fP1->cd(3);
    h1 = h0->ProjectionX();
    h1->SetMinimum(0);
    h1->Draw();

    fP1->cd(4);
    h1 = h3->ProjectionX();
    h1->SetMinimum(0);
    h1->Draw();
  }
//-----------------------------------------------------------------------------
// Fig 7: resolutions, averaged over the volume
//----------------------------------------------------------------------------- 
  if (Figure == 7) {
    char fn[100];
    TH2F   *hy, *hz;
    TH1D   *hy_py, *hz_py;

    sprintf(fn,"hist/mcrystal/mcrystal_uniform_lyso_20_20_20_sipm_04_04.hist");

    fCanvas  = new_slide(name,title,2,1,1000,500);
    fP1      = (TPad*) fCanvas->GetPrimitive("p1");

					// DY:Yrec
    fP1->cd(1);
    hy = gh2(fn,fModule.Data(),"evt_0/dy4c_vs_yr4");
    hy->Draw();

    fP1->cd(2);
    hy_py = hy->ProjectionY("fig_7_hy_py");

    hy_py->Fit("gaus","w",""); // ,-0.15,0.15);
  }
//-----------------------------------------------------------------------------
// Fig 8: sig[0] vs ZT
//----------------------------------------------------------------------------- 
  if (Figure == 8) {
    char fn[100];
    TH2F   *hy, *hz;
    TH1D   *hy_py, *hz_py;

    sprintf(fn,"hist/mcrystal/mcrystal_uniform_lyso_20_20_20_sipm_04_04.hist");

    fCanvas  = new_slide(name,title,1,1,800,800);
    fP1      = (TPad*) fCanvas->GetPrimitive("p1");

    fP1->cd(1);
    hy = gh2(fn,fModule.Data(),"evt_0/s0_vs_zt");
    hy->GetYaxis()->SetRangeUser(0,0.999);
    hy->SetStats(0);
    hy->Draw();
  }
//-----------------------------------------------------------------------------
// Fig 9: dz3 vs Zt
//----------------------------------------------------------------------------- 
  if (Figure == 9) {
    char fn[100];
    TH2F   *hy, *hz;
    TH1D   *hy_py, *hz_py;

    sprintf(fn,"hist/mcrystal/mcrystal_uniform_lyso_20_20_20_sipm_04_04.hist");

    fCanvas  = new_slide(name,title,2,1,1100,600);
    fP1      = (TPad*) fCanvas->GetPrimitive("p1");

    fP1->cd(1);
    hy = gh2(fn,fModule.Data(),"evt_0/dz3_vs_zt");
    //    hy->GetYaxis()->SetRangeUser(0,0.999);
    hy->SetStats(0);
    hy->Draw();

    fP1->cd(2);
    hz_py = gh2(fn,fModule.Data(),"evt_0/dzc3_vs_zr3")->ProjectionY("fig_9_hz_py");
    hz_py->Fit("gaus","w","",-0.12,0.12);
  }
//-----------------------------------------------------------------------------
// Fig 11: Y-resolution vs SIPM size for 20x20x10mm3 and 20x20x20mm3 crystals
//----------------------------------------------------------------------------- 
  if (Figure == 11) {
    char    fn_20_10_02[100], fn_20_10_04[100];
    char    fn_20_20_02[100], fn_20_20_04[100];
    char    fn_20_30_02[100], fn_20_30_04[100];

    TH1D   *h_20_10_02, *h_20_10_04;
    TH1D   *h_20_20_02, *h_20_20_04;
    TH1D   *h_20_30_02, *h_20_30_04;

    sprintf(fn_20_10_02,"hist/mcrystal/mcrystal_uniform_lyso_20_20_10_sipm_02_02.hist");
    sprintf(fn_20_10_04,"hist/mcrystal/mcrystal_uniform_lyso_20_20_10_sipm_04_04.hist");
    sprintf(fn_20_20_02,"hist/mcrystal/mcrystal_uniform_lyso_20_20_20_sipm_02_02.hist");
    sprintf(fn_20_20_04,"hist/mcrystal/mcrystal_uniform_lyso_20_20_20_sipm_04_04.hist");
    sprintf(fn_20_30_02,"hist/mcrystal/mcrystal_uniform_lyso_20_20_30_sipm_02_02.hist");
    sprintf(fn_20_30_04,"hist/mcrystal/mcrystal_uniform_lyso_20_20_30_sipm_04_04.hist");

    fCanvas  = new_slide(name,title,3,2,1000,800);
    fP1      = (TPad*) fCanvas->GetPrimitive("p1");

					// 2mm SiPM's
    fP1->cd(1);
    h_20_10_02 = gh2(fn_20_10_02,fModule.Data(),"evt_0/dy2c_vs_yr2")->ProjectionY("h_py_20_10_02");
    h_20_10_02->Fit("gaus","w");
					// 
    fP1->cd(2);
    h_20_20_02 = gh2(fn_20_20_02,fModule.Data(),"evt_0/dy2c_vs_yr2")->ProjectionY("h_py_20_20_02");
    h_20_20_02->Fit("gaus","w");

    fP1->cd(3);
    h_20_30_02 = gh2(fn_20_30_02,fModule.Data(),"evt_0/dy2c_vs_yr2")->ProjectionY("h_py_20_30_02");
    h_20_30_02->Fit("gaus","w");

    fP1->cd(4);
    h_20_10_04 = gh2(fn_20_10_04,fModule.Data(),"evt_0/dy2c_vs_yr2")->ProjectionY("h_py_20_10_04");
    h_20_10_04->Fit("gaus","w");

    fP1->cd(5);
    h_20_20_04 = gh2(fn_20_20_04,fModule.Data(),"evt_0/dy2c_vs_yr2")->ProjectionY("h_py_20_20_04");
    h_20_20_04->Fit("gaus","w");

    fP1->cd(6);
    h_20_30_04 = gh2(fn_20_30_04,fModule.Data(),"evt_0/dy2c_vs_yr2")->ProjectionY("h_py_20_30_04");
    h_20_30_04->Fit("gaus","w");

  }
    
//-----------------------------------------------------------------------------
// Fig 12: Y-resolution vs SIPM size for 20x20x10mm3 and 20x20x20mm3 crystals
//----------------------------------------------------------------------------- 
  if (Figure == 12) {
    char    fn_20_10_02[100], fn_20_10_04[100];
    char    fn_20_20_02[100], fn_20_20_04[100];
    char    fn_20_30_02[100], fn_20_30_04[100];
    char    fn_40_30_02[100], fn_40_30_04[100];

    TH1D   *h_20_10_02, *h_20_10_04;
    TH1D   *h_20_20_02, *h_20_20_04;
    TH1D   *h_20_30_02, *h_20_30_04;
    TH1D   *h_40_30_02, *h_40_30_04;

    double x[10], y[10];

    sprintf(fn_20_10_02,"hist/mcrystal/mcrystal_uniform_lyso_20_20_10_sipm_02_02.hist");
    sprintf(fn_20_10_04,"hist/mcrystal/mcrystal_uniform_lyso_20_20_10_sipm_04_04.hist");
    sprintf(fn_20_20_02,"hist/mcrystal/mcrystal_uniform_lyso_20_20_20_sipm_02_02.hist");
    sprintf(fn_20_20_04,"hist/mcrystal/mcrystal_uniform_lyso_20_20_20_sipm_04_04.hist");
    sprintf(fn_20_30_02,"hist/mcrystal/mcrystal_uniform_lyso_20_20_30_sipm_02_02.hist");
    sprintf(fn_20_30_04,"hist/mcrystal/mcrystal_uniform_lyso_20_20_30_sipm_04_04.hist");
    sprintf(fn_40_30_02,"hist/mcrystal/mcrystal_uniform_lyso_40_40_30_sipm_02_02.hist");
    sprintf(fn_40_30_04,"hist/mcrystal/mcrystal_uniform_lyso_40_40_30_sipm_04_04.hist");

    fCanvas  = new_slide(name,title,1,1,1000,800);
    fP1      = (TPad*) fCanvas->GetPrimitive("p1");

					// 2mm SiPM's
    fP1->cd(1);
    h_20_10_02 = gh2(fn_20_10_02,fModule.Data(),"evt_0/dy2c_vs_yr2")->ProjectionY("h_py_20_10_02");
    h_20_10_02->Fit("gaus","wq0");
    x[0] = 1.0;
    y[0] = h_20_10_02->GetFunction("gaus")->GetParameter(2)*2.35;

    h_20_20_02 = gh2(fn_20_20_02,fModule.Data(),"evt_0/dy2c_vs_yr2")->ProjectionY("h_py_20_20_02");
    h_20_20_02->Fit("gaus","wq0");
    x[1] = 2.0;
    y[1] = h_20_20_02->GetFunction("gaus")->GetParameter(2)*2.35;

    h_20_30_02 = gh2(fn_20_30_02,fModule.Data(),"evt_0/dy2c_vs_yr2")->ProjectionY("h_py_20_30_02");
    h_20_30_02->Fit("gaus","wq0");
    x[2] = 3.0;
    y[2] = h_20_30_02->GetFunction("gaus")->GetParameter(2)*2.35;

    h_40_30_02 = gh2(fn_40_30_02,fModule.Data(),"evt_0/dy2c_vs_yr2")->ProjectionY("h_py_40_30_02");
    h_40_30_02->Fit("gaus","wq0");
    x[3] = 3.0;
    y[3] = h_40_30_02->GetFunction("gaus")->GetParameter(2)*2.35;


    TGraph* gr1  = new TGraph(3,x,y);
    gr1->SetMarkerStyle(20);
    gr1->SetMarkerSize(1);
    gr1->SetTitle("FWHM(Y2) vs crystal length, 20x20xL crystals");

    gr1->GetYaxis()->SetRangeUser(0.,0.5);
    gr1->GetYaxis()->SetTitle("FWHM (mm)");
    gr1->GetXaxis()->SetTitle("L (cm)");
    gr1->GetXaxis()->SetRangeUser(0,5);
    gr1->Draw("ALP");
    
					// 4mm SiPM's

    h_20_10_04 = gh2(fn_20_10_04,fModule.Data(),"evt_0/dy2c_vs_yr2")->ProjectionY("h_py_20_10_04");
    h_20_10_04->Fit("gaus","wq0");
    x[0] = 1.0;
    y[0] = h_20_10_04->GetFunction("gaus")->GetParameter(2)*2.35;

    h_20_20_04 = gh2(fn_20_20_04,fModule.Data(),"evt_0/dy2c_vs_yr2")->ProjectionY("h_py_20_20_04");
    h_20_20_04->Fit("gaus","wq0");
    x[1] = 2.0;
    y[1] = h_20_20_04->GetFunction("gaus")->GetParameter(2)*2.35;

    h_20_30_04 = gh2(fn_20_30_04,fModule.Data(),"evt_0/dy2c_vs_yr2")->ProjectionY("h_py_20_30_04");
    h_20_30_04->Fit("gaus","wq0");
    x[2] = 3.0;
    y[2] = h_20_30_04->GetFunction("gaus")->GetParameter(2)*2.35;

    h_40_30_04 = gh2(fn_40_30_04,fModule.Data(),"evt_0/dy2c_vs_yr2")->ProjectionY("h_py_40_30_04");
    h_40_30_04->Fit("gaus","wq0");
    x[3] = 3.0;
    y[3] = h_40_30_04->GetFunction("gaus")->GetParameter(2)*2.35;

    TGraph* gr2  = new TGraph(3,x,y);
    gr2->SetMarkerStyle(24);
    gr2->SetMarkerSize(1);
    gr2->SetTitle("FWHM(Y) vs crystal length, 20x20xLength crystals, 4x4mm SIPM");

    gr2->Draw("lp");
					// 40x40x30 crystals 

    h_40_30_02 = gh2(fn_40_30_02,fModule.Data(),"evt_0/dy2c_vs_yr2")->ProjectionY("h_py_40_30_02");
    h_40_30_02->Fit("gaus","wq0");
    x[0] = 3.0;
    y[0] = h_40_30_02->GetFunction("gaus")->GetParameter(2)*2.35;

    TGraph* gr3  = new TGraph(1,x,y);
    gr3->SetMarkerStyle(21);
    gr3->SetMarkerSize(1);
    gr3->SetTitle("FWHM(Y2) vs crystal length, 40x40xL crystals");

    gr3->Draw("LP");

    h_40_30_04 = gh2(fn_40_30_04,fModule.Data(),"evt_0/dy2c_vs_yr2")->ProjectionY("h_py_40_30_04");
    h_40_30_04->Fit("gaus","wq0");
    x[0] = 3.0;
    y[0] = h_40_30_04->GetFunction("gaus")->GetParameter(2)*2.35;

    TGraph* gr4  = new TGraph(1,x,y);
    gr4->SetMarkerStyle(25);
    gr4->SetMarkerSize(1);
    gr4->SetTitle("FWHM(Y2) vs crystal length, 40x40xL crystals");

    TLegend* leg = new TLegend(0.1,0.6,0.4,0.9);

    leg->AddEntry(gr1,"20x20mm crystal, 2x2mm SIPM","p");
    leg->AddEntry(gr2,"20x20mm crystal, 4x4mm SIPM","p");
    leg->AddEntry(gr3,"40x40mm crystal, 2x2mm SIPM","p");
    leg->AddEntry(gr4,"40x40mm crystal, 2x2mm SIPM","p");
    leg->SetFillColor(0);

    gr4->Draw("LP");

    leg->Draw("same");
  }
//-----------------------------------------------------------------------------
// Fig 13: Z-resolution , Z2C, vs SIPM size for 20x20xL  and 20x20xL crystals
//----------------------------------------------------------------------------- 
  if (Figure == 13) {
    char    fn_20_10_02[100], fn_20_10_04[100];
    char    fn_20_20_02[100], fn_20_20_04[100];
    char    fn_20_30_02[100], fn_20_30_04[100];
    char    fn_40_30_02[100], fn_40_30_04[100];

    TH1D   *h_20_10_02, *h_20_10_04;
    TH1D   *h_20_20_02, *h_20_20_04;
    TH1D   *h_20_30_02, *h_20_30_04;
    TH1D   *h_40_30_02, *h_40_30_04;

    double x[10], y[10];

    sprintf(fn_20_10_02,"hist/mcrystal/mcrystal_uniform_lyso_20_20_10_sipm_02_02.hist");
    sprintf(fn_20_10_04,"hist/mcrystal/mcrystal_uniform_lyso_20_20_10_sipm_04_04.hist");
    sprintf(fn_20_20_02,"hist/mcrystal/mcrystal_uniform_lyso_20_20_20_sipm_02_02.hist");
    sprintf(fn_20_20_04,"hist/mcrystal/mcrystal_uniform_lyso_20_20_20_sipm_04_04.hist");
    sprintf(fn_20_30_02,"hist/mcrystal/mcrystal_uniform_lyso_20_20_30_sipm_02_02.hist");
    sprintf(fn_20_30_04,"hist/mcrystal/mcrystal_uniform_lyso_20_20_30_sipm_04_04.hist");
    sprintf(fn_40_30_02,"hist/mcrystal/mcrystal_uniform_lyso_40_40_30_sipm_02_02.hist");
    sprintf(fn_40_30_04,"hist/mcrystal/mcrystal_uniform_lyso_40_40_30_sipm_04_04.hist");

    fCanvas  = new_slide(name,title,1,1,1000,800);
    fP1      = (TPad*) fCanvas->GetPrimitive("p1");

					// 2mm SiPM's
    fP1->cd(1);
    h_20_10_02 = gh2(fn_20_10_02,fModule.Data(),"evt_0/dz2c_vs_zr2")->ProjectionY("fig_13_h_py_20_10_02");
    h_20_10_02->Fit("gaus","wq0");
    x[0] = 1.0;
    y[0] = h_20_10_02->GetFunction("gaus")->GetParameter(2)*2.35;

    h_20_20_02 = gh2(fn_20_20_02,fModule.Data(),"evt_0/dz2c_vs_zr2")->ProjectionY("fig_13_h_py_20_20_02");
    h_20_20_02->Fit("gaus","wq0");
    x[1] = 2.0;
    y[1] = h_20_20_02->GetFunction("gaus")->GetParameter(2)*2.35;

    h_20_30_02 = gh2(fn_20_30_02,fModule.Data(),"evt_0/dz2c_vs_zr2")->ProjectionY("fig_13_h_py_20_30_02");
    h_20_30_02->Fit("gaus","wq0");
    x[2] = 3.0;
    y[2] = h_20_30_02->GetFunction("gaus")->GetParameter(2)*2.35;


    TGraph* gr1  = new TGraph(3,x,y);
    gr1->SetMarkerStyle(20);
    gr1->SetMarkerSize(1);
    gr1->SetTitle("FWHM(Z) vs crystal length");

    gr1->GetYaxis()->SetRangeUser(0.,0.5);
    gr1->GetXaxis()->SetRangeUser(0.,5.);
    gr1->GetYaxis()->SetTitle("FWHM (mm)");
    gr1->GetXaxis()->SetTitle("L (cm)");
    gr1->Draw("ALP");
    
					// 4mm SiPM's

    h_20_10_04 = gh2(fn_20_10_04,fModule.Data(),"evt_0/dz2c_vs_zr2")->ProjectionY("fig_13_h_py_20_10_04");
    h_20_10_04->Fit("gaus","wq0");
    x[0] = 1.0;
    y[0] = h_20_10_04->GetFunction("gaus")->GetParameter(2)*2.35;

    h_20_20_04 = gh2(fn_20_20_04,fModule.Data(),"evt_0/dz2c_vs_zr2")->ProjectionY("fig_13_h_py_20_20_04");
    h_20_20_04->Fit("gaus","wq0");
    x[1] = 2.0;
    y[1] = h_20_20_04->GetFunction("gaus")->GetParameter(2)*2.35;

    h_20_30_04 = gh2(fn_20_30_04,fModule.Data(),"evt_0/dz2c_vs_zr2")->ProjectionY("fig_13_h_py_20_30_04");
    h_20_30_04->Fit("gaus","wq0");
    x[2] = 3.0;
    y[2] = h_20_30_04->GetFunction("gaus")->GetParameter(2)*2.35;


    TGraph* gr2  = new TGraph(3,x,y);
    gr2->SetMarkerStyle(24);
    gr2->SetMarkerSize(1);
    gr2->SetTitle("FWHM(Z) vs crystal length, 4x4mm SIPM");

    gr2->Draw("lp");
					// 40x40x30 crystals 

    h_40_30_02 = gh2(fn_40_30_02,fModule.Data(),"evt_0/dy2c_vs_yr2")->ProjectionY("fig_13_h_py_40_30_02");
    h_40_30_02->Fit("gaus","wq0");
    x[0] = 3.0;
    y[0] = h_40_30_02->GetFunction("gaus")->GetParameter(2)*2.35;

    TGraph* gr3  = new TGraph(1,x,y);
    gr3->SetMarkerStyle(21);
    gr3->SetMarkerSize(1);
    gr3->SetTitle("FWHM(Y2) vs crystal length");

    gr3->Draw("LP");

    h_40_30_04 = gh2(fn_40_30_04,fModule.Data(),"evt_0/dy2c_vs_yr2")->ProjectionY("fig_13_h_py_40_30_04");
    h_40_30_04->Fit("gaus","wq0");
    x[0] = 3.0;
    y[0] = h_40_30_04->GetFunction("gaus")->GetParameter(2)*2.35;

    TGraph* gr4  = new TGraph(1,x,y);
    gr4->SetMarkerStyle(25);
    gr4->SetMarkerSize(1);
    gr4->SetTitle("FWHM(Y2) vs crystal length, 40x40xL crystals");

    TLegend* leg = new TLegend(0.1,0.6,0.4,0.9);

    leg->AddEntry(gr1,"20x20mm crystal, 2x2mm SIPM","p");
    leg->AddEntry(gr2,"20x20mm crystal, 4x4mm SIPM","p");
    leg->AddEntry(gr3,"40x40mm crystal, 2x2mm SIPM","p");
    leg->AddEntry(gr4,"40x40mm crystal, 4x4mm SIPM","p");
    leg->SetFillColor(0);

    gr4->Draw("LP");

    leg->Draw("same");
  }

//-----------------------------------------------------------------------------
// Fig 14: Y-resolution , Y4C, vs SIPM size for 20x20xL  and 20x20xL crystals
//----------------------------------------------------------------------------- 
  if (Figure == 14) {
    char    fn_20_10_02[100], fn_20_10_04[100];
    char    fn_20_20_02[100], fn_20_20_04[100];
    char    fn_20_30_02[100], fn_20_30_04[100];
    char    fn_40_30_02[100], fn_40_30_04[100];

    TH1D   *h_20_10_02, *h_20_10_04;
    TH1D   *h_20_20_02, *h_20_20_04;
    TH1D   *h_20_30_02, *h_20_30_04;
    TH1D   *h_40_30_02, *h_40_30_04;

    double x[10], y[10];

    sprintf(fn_20_10_02,"hist/mcrystal/mcrystal_uniform_lyso_20_20_10_sipm_02_02.hist");
    sprintf(fn_20_10_04,"hist/mcrystal/mcrystal_uniform_lyso_20_20_10_sipm_04_04.hist");
    sprintf(fn_20_20_02,"hist/mcrystal/mcrystal_uniform_lyso_20_20_20_sipm_02_02.hist");
    sprintf(fn_20_20_04,"hist/mcrystal/mcrystal_uniform_lyso_20_20_20_sipm_04_04.hist");
    sprintf(fn_20_30_02,"hist/mcrystal/mcrystal_uniform_lyso_20_20_30_sipm_02_02.hist");
    sprintf(fn_20_30_04,"hist/mcrystal/mcrystal_uniform_lyso_20_20_30_sipm_04_04.hist");
    sprintf(fn_40_30_02,"hist/mcrystal/mcrystal_uniform_lyso_40_40_30_sipm_02_02.hist");
    sprintf(fn_40_30_04,"hist/mcrystal/mcrystal_uniform_lyso_40_40_30_sipm_04_04.hist");

    fCanvas  = new_slide(name,title,1,1,1000,800);
    fP1      = (TPad*) fCanvas->GetPrimitive("p1");

					// 2mm SiPM's
    fP1->cd(1);
    h_20_10_02 = gh2(fn_20_10_02,fModule.Data(),"evt_0/dy4c_vs_yr4")->ProjectionY("fig_14_h_py_20_10_02");
    h_20_10_02->Fit("gaus","wq0");
    x[0] = 1.0;
    y[0] = h_20_10_02->GetFunction("gaus")->GetParameter(2)*2.35;

    h_20_20_02 = gh2(fn_20_20_02,fModule.Data(),"evt_0/dy4c_vs_yr4")->ProjectionY("fig_14_h_py_20_20_02");
    h_20_20_02->Fit("gaus","wq0");
    x[1] = 2.0;
    y[1] = h_20_20_02->GetFunction("gaus")->GetParameter(2)*2.35;

    h_20_30_02 = gh2(fn_20_30_02,fModule.Data(),"evt_0/dy4c_vs_yr4")->ProjectionY("fig_14_h_py_20_30_02");
    h_20_30_02->Fit("gaus","wq0");
    x[2] = 3.0;
    y[2] = h_20_30_02->GetFunction("gaus")->GetParameter(2)*2.35;


    TGraph* gr1  = new TGraph(3,x,y);
    gr1->SetMarkerStyle(20);
    gr1->SetMarkerSize(1);
    gr1->SetTitle("FWHM(Y4), vs crystal length");

    gr1->GetXaxis()->SetRangeUser(0.,5);
    gr1->GetXaxis()->SetTitle("L (cm)");

    gr1->GetYaxis()->SetRangeUser(0.,0.5);
    gr1->GetYaxis()->SetTitle("FWHM (mm)");

    gr1->Draw("ALP");
    
					// 4mm SiPM's

    h_20_10_04 = gh2(fn_20_10_04,fModule.Data(),"evt_0/dy4c_vs_yr4")->ProjectionY("fig_14_h_py_20_10_04");
    h_20_10_04->Fit("gaus","wq0");
    x[0] = 1.0;
    y[0] = h_20_10_04->GetFunction("gaus")->GetParameter(2)*2.35;

    h_20_20_04 = gh2(fn_20_20_04,fModule.Data(),"evt_0/dy4c_vs_yr4")->ProjectionY("fig_14_h_py_20_20_04");
    h_20_20_04->Fit("gaus","wq0");
    x[1] = 2.0;
    y[1] = h_20_20_04->GetFunction("gaus")->GetParameter(2)*2.35;

    h_20_30_04 = gh2(fn_20_30_04,fModule.Data(),"evt_0/dy4c_vs_yr4")->ProjectionY("fig_14_h_py_20_30_04");
    h_20_30_04->Fit("gaus","wq0");
    x[2] = 3.0;
    y[2] = h_20_30_04->GetFunction("gaus")->GetParameter(2)*2.35;


    TGraph* gr2  = new TGraph(3,x,y);
    gr2->SetMarkerStyle(24);
    gr2->SetMarkerSize(1);
    gr2->SetTitle("FWHM(Y4) vs crystal length, 4x4mm SIPM");

    gr2->Draw("lp");
					// 40x40x30 crystals 

    h_40_30_02 = gh2(fn_40_30_02,fModule.Data(),"evt_0/dy4c_vs_yr4")->ProjectionY("fig_14_h_py_40_30_02");
    h_40_30_02->Fit("gaus","wq0");
    x[0] = 3.0;
    y[0] = h_40_30_02->GetFunction("gaus")->GetParameter(2)*2.35;

    TGraph* gr3  = new TGraph(1,x,y);
    gr3->SetMarkerStyle(21);
    gr3->SetMarkerSize(1);
    gr3->SetTitle("FWHM(Y2) vs crystal length, 40x40xL crystals");

    gr3->Draw("LP");

    h_40_30_04 = gh2(fn_40_30_04,fModule.Data(),"evt_0/dy4c_vs_yr4")->ProjectionY("fig_14_h_py_40_30_04");
    h_40_30_04->Fit("gaus","wq0");
    x[0] = 3.0;
    y[0] = h_40_30_04->GetFunction("gaus")->GetParameter(2)*2.35;

    TGraph* gr4  = new TGraph(1,x,y);
    gr4->SetMarkerStyle(25);
    gr4->SetMarkerSize(1);
    gr4->SetTitle("FWHM(Y2) vs crystal length, 40x40xL crystals");

    TLegend* leg = new TLegend(0.1,0.6,0.4,0.9);

    leg->AddEntry(gr1,"20x20mm crystal, 2x2mm SIPM","p");
    leg->AddEntry(gr2,"20x20mm crystal, 4x4mm SIPM","p");
    leg->AddEntry(gr3,"40x40mm crystal, 2x2mm SIPM","p");
    leg->AddEntry(gr4,"40x40mm crystal, 4x4mm SIPM","p");
    leg->SetFillColor(0);

    gr4->Draw("LP");

    leg->Draw("same");
  }
    
//-----------------------------------------------------------------------------
// Fig 15: DY1 and DY0 vs Z rec
//----------------------------------------------------------------------------- 
  if (Figure == 15) {
    char    fn_20_20_02[100], fn_20_20_04[100];

    TH2F   *h1, *h2;

    sprintf(fn_20_20_04,"hist/mcrystal/mcrystal_uniform_lyso_20_20_20_sipm_04_04.hist");

    fCanvas  = new_slide(name,title,2,1,1000,600);
    fP1      = (TPad*) fCanvas->GetPrimitive("p1");

					// 2mm SiPM's
    fP1->cd(2);
    h2 = gh2(fn_20_20_04,fModule.Data(),"evt_0/dy1_vs_zr2");
    h2->Draw();

    fP1->cd(1);
    h1 = gh2(fn_20_20_04,fModule.Data(),"evt_0/dy0_vs_zr2");
    h1->Draw();

  }
//-----------------------------------------------------------------------------
// Fig 21: DY2 vs Y2R
//----------------------------------------------------------------------------- 
  if (Figure == 21) {
    char fn[100];
    TH2F   *hy, *hz;
    TH1D   *hy_py, *hz_py;

    sprintf(fn,"hist/mcrystal/mcrystal_uniform_lyso_20_20_20_sipm_04_04.hist");

    fCanvas  = new_slide(name,title,1,1,800,600);
    fP1      = (TPad*) fCanvas->GetPrimitive("p1");

    fP1->cd(1);
    hy = gh2(fn,fModule.Data(),"evt_0/dy2_vs_yr2");
    //    hy->GetYaxis()->SetRangeUser(0,0.999);
    hy->SetStats(0);
    hy->Draw();
  }
//-----------------------------------------------------------------------------
// Fig 71: DY2:YT and DY2:YR2
//----------------------------------------------------------------------------- 
  if (Figure == 71) {
    char fn[100];
    TH2F   *hy, *hz;
    TH1D   *hy_py, *hz_py;

    sprintf(fn,"hist/mcrystal/mcrystal_uniform_lyso_20_20_20_sipm_04_04.hist");

    fCanvas  = new_slide(name,title,2,1,1000,500);
    fP1      = (TPad*) fCanvas->GetPrimitive("p1");

					// DY:Yrec
    fP1->cd(1);
    hy = gh2(fn,fModule.Data(),"evt_0/dy2_vs_yt");
    hy->Draw();

    fP1->cd(2);
    hy = gh2(fn,fModule.Data(),"evt_0/dy2_vs_yr2");
    hy->Draw();
  }
//-----------------------------------------------------------------------------
// Fig 72: corrected resolutions in DY2, averaged over the volume
//----------------------------------------------------------------------------- 
  if (Figure == 72) {
    char fn[100];
    TH2F   *hy, *hz;
    TH1D   *hy_py, *hz_py;

    sprintf(fn,"hist/mcrystal/mcrystal_uniform_lyso_20_20_20_sipm_04_04.hist");

    fCanvas  = new_slide(name,title,2,1,1000,500);
    fP1      = (TPad*) fCanvas->GetPrimitive("p1");

					// DY:Yrec
    fP1->cd(1);
    hy = gh2(fn,fModule.Data(),"evt_0/dy2c_vs_yr2");
    hy->Draw();

    fP1->cd(2);
    hy_py = hy->ProjectionY("fig_71_hy_py");

    hy_py->Fit("gaus","w",""); // ,-0.15,0.15);
  }
}
