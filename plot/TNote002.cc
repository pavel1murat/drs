///////////////////////////////////////////////////////////////////////////////
// plots for DRS note_002 - resolution stdies
// -------------------------
// directory with the histogram files: by default: $WORK_DIR/results
// can be redefined with "drs.HistDir" in .rootrc
// 
// figures:
// ---------
// Fig 1: chi2 of the gaussian fit
// Fig 2: gaussian sigma
// Fig 3: CTR: GAUS FIT vs CFD 
// Fig 4: dt(norm) - check that the first point doesn't bias the fit
//
// Fig 41: chi2 of the EXP1 fit for the two highest channels
// Fig 42: 
// Fig 43: CTR: EXP1 FIT vs CFD 
// Fig 44: dt(norm) - for EXP1 - check that the first point doesn't bias the fit
//
// Fig 53: CTR: GAUS FIT vs EXP1 FIT
////////////////////////////////////////////////////////////////////////////////
#include "plot/TNote002.hh"
#include "TPaveLabel.h"
#include "TArrow.h"
#include "TObjArray.h"
#include "TObjString.h"
#include <iostream>

ClassImp(TNote002)

//_____________________________________________________________________________
TNote002::TNote002(int PlotMode, int BlessingMode): 
  TPlotCdfNote(PlotMode,BlessingMode) {

  const char*  hist_dir;
  char         res_dir[500];
  
  fFiguresDir   = Form("%s/%s",gEnv->GetValue("drs.Figures","./"),"srcs01_000072");
  fWorkDir      = gSystem->Getenv("WORK_DIR");

  sprintf(res_dir,"%s/hist",fWorkDir.Data());
  hist_dir      = gEnv->GetValue("drs.HistDir",res_dir);

  fFilename     = Form("%s/srcs01.000072.drs_exp1.hist",hist_dir);

  fModule       = "Pet2Ana";

};

//_____________________________________________________________________________
TNote002::~TNote002() {
};

//-----------------------------------------------------------------------------
const char* TNote002::GetFiguresDir() {
  return fFiguresDir.Data();
}

//-----------------------------------------------------------------------------
// determine name of the .eps and .gif files in the ./figures directory
// today's default set is 'frr_03', all the plots should be coming from it
//-----------------------------------------------------------------------------
void TNote002::remake_plots() {

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


//_____________________________________________________________________________
void TNote002::plot(Int_t Figure, const char* CanvasName) {
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

  if (Figure == 1) {
//-----------------------------------------------------------------------------
//  Fig 1: chi^2 of the gaussian fit
//-----------------------------------------------------------------------------
    TH1F    *h1;
    
    fCanvas  = new_slide(name,title,2,2,1000,700);

    fP1      = (TPad*) fCanvas->GetPrimitive("p1");

					// chi2
    fP1->cd(1);
    h1 = gh1(fFilename.Data(),fModule.Data(),"gaus/chi2_00");
    h1->Draw();
   
					// chi2
    fP1->cd(2);
    h1 = gh1(fFilename.Data(),fModule.Data(),"gaus/chi2_01");
    h1->Draw();
					// n fit points
    fP1->cd(3);
    h1 = gh1(fFilename.Data(),fModule.Data(),"gaus/npt_00");
    h1->Draw();
   
					// n fit points
    fP1->cd(4);
    h1 = gh1(fFilename.Data(),fModule.Data(),"gaus/npt_01");
    h1->Draw();
  }
  if (Figure == 2) {
//-----------------------------------------------------------------------------
//  Fig 2: gaussian sigma
//-----------------------------------------------------------------------------
    TH1F    *h1;
    
    fCanvas  = new_slide(name,title,2,1,1000,500);

    fP1      = (TPad*) fCanvas->GetPrimitive("p1");

					// reconstructed Y
    fP1->cd(1);
    h1 = gh1(fFilename.Data(),fModule.Data(),"gaus/gsigma_0");
    h1->SetTitle("#sigma of the gaussian fit");
    h1->GetXaxis()->SetRangeUser(0,9.99);
    h1->GetXaxis()->SetTitle("#sigma, channels");
    h1->Draw();
   
					// reconstructed Z
    fP1->cd(2);
    h1 = gh1(fFilename.Data(),fModule.Data(),"gaus/gsigma_1");
    h1->SetTitle("#sigma of the gaussian fit");
    h1->GetXaxis()->SetRangeUser(0,9.99);
    h1->GetXaxis()->SetTitle("#sigma, channels");
    h1->Draw();
  }
  if (Figure == 3) {
//-----------------------------------------------------------------------------
//  Fig 2: timing resolution vs chi2 cut
//-----------------------------------------------------------------------------
    TH1F    *h1;
    TLegend *leg;
    double  chi2[4] = { 2., 3., 5., 10.};
    double  ex[4] = { 0,0,0,0};
    double  fwhm [4], efw[4], fwhm_cfd [4], efw_cfd[4];
    
    fCanvas  = new_slide(name,title,1,1,1000,500);

    fP1      = (TPad*) fCanvas->GetPrimitive("p1");

					// reconstructed Y
    fP1->cd(1);
//-----------------------------------------------------------------------------
// get timing for gaussian fit
//-----------------------------------------------------------------------------
    h1 = gh1(fFilename.Data(),fModule.Data(),"gaus/dt2");
    h1->Fit("gaus","q0");
    fwhm[3] = h1->GetFunction("gaus")->GetParameter(2)*2.35;
    efw [3] = h1->GetFunction("gaus")->GetParError(2)*2.35;

    h1 = gh1(fFilename.Data(),fModule.Data(),"gaus/dt5");
    h1->Fit("gaus","q0");
    fwhm[2] = h1->GetFunction("gaus")->GetParameter(2)*2.35;
    efw [2] = h1->GetFunction("gaus")->GetParError(2)*2.35;

    h1 = gh1(fFilename.Data(),fModule.Data(),"gaus/dt6");
    h1->Fit("gaus","q0");
    fwhm[1] = h1->GetFunction("gaus")->GetParameter(2)*2.35;
    efw [1] = h1->GetFunction("gaus")->GetParError(2)*2.35;

    h1 = gh1(fFilename.Data(),fModule.Data(),"gaus/dt7");
    h1->Fit("gaus","q0");
    fwhm[0] = h1->GetFunction("gaus")->GetParameter(2)*2.35;
    efw [0] = h1->GetFunction("gaus")->GetParError(2)*2.35;

    TGraphErrors* gr = new TGraphErrors(4,chi2,fwhm,ex,efw);
//-----------------------------------------------------------------------------
// get "CFD" resolution
//-----------------------------------------------------------------------------
    h1 = gh1(fFilename.Data(),fModule.Data(),"gaus/dtl_2");
    h1->Fit("gaus","q0");
    fwhm_cfd[3] = h1->GetFunction("gaus")->GetParameter(2)*2.35;
    efw_cfd [3] = h1->GetFunction("gaus")->GetParError(2)*2.35;

    h1 = gh1(fFilename.Data(),fModule.Data(),"gaus/dtl_5");
    h1->Fit("gaus","q0");
    fwhm_cfd[2] = h1->GetFunction("gaus")->GetParameter(2)*2.35;
    efw_cfd [2] = h1->GetFunction("gaus")->GetParError(2)*2.35;

    h1 = gh1(fFilename.Data(),fModule.Data(),"gaus/dtl_6");
    h1->Fit("gaus","q0");
    fwhm_cfd[1] = h1->GetFunction("gaus")->GetParameter(2)*2.35;
    efw_cfd [1] = h1->GetFunction("gaus")->GetParError(2)*2.35;

    h1 = gh1(fFilename.Data(),fModule.Data(),"gaus/dtl_7");
    h1->Fit("gaus","q0");
    fwhm_cfd[0] = h1->GetFunction("gaus")->GetParameter(2)*2.35;
    efw_cfd [0] = h1->GetFunction("gaus")->GetParError(2)*2.35;

    TGraphErrors* gr_cfd = new TGraphErrors(4,chi2,fwhm_cfd,ex,efw_cfd);

    TH2F* h2 = new TH2F("h2","timing resolution vs chi2 fit cut",1,0,11,1,0,0.5);
    h2->SetStats(0);

    h2->GetXaxis()->SetTitle("#chi^{2}/N_{DOF} cut value");
    h2->GetYaxis()->SetTitle("CTR, ns");
    h2->Draw();

    gr->SetMarkerStyle(20);
    gr->SetMarkerSize(1);
    gr->Draw("Pe");

    gr_cfd->SetMarkerStyle(24);
    gr_cfd->SetMarkerSize(1);
    gr_cfd->Draw("Pe");
//-----------------------------------------------------------------------------
// finally - legend
//-----------------------------------------------------------------------------
    leg = new TLegend(0.6,0.76,0.9,0.89,"");
    leg->AddEntry(gr,"gaussian fit","ep");
    leg->AddEntry(gr_cfd,"CDF timing","ep");
    leg->SetFillColor(0);
    leg->SetBorderSize(0);
    leg->Draw();
  }
  if (Figure == 4) {
//-----------------------------------------------------------------------------
//  Fig 4: dtnorm for GAUS
//-----------------------------------------------------------------------------
    TH1F    *h1;
    
    fCanvas  = new_slide(name,title,2,1,1000,500);

    fP1      = (TPad*) fCanvas->GetPrimitive("p1");

					// dtnorm - reduced T0
    fP1->cd(1);
    h1 = gh1(fFilename.Data(),fModule.Data(),"gaus/dtnorm_00");
    h1->SetTitle("T0 reduced, within the first bin");
    h1->GetXaxis()->SetTitle("reduced T0");
    h1->SetMinimum(0);
    h1->Rebin(2);
    h1->Fit("pol0","","ep");
   
					// dtnorm - reduced T0
    fP1->cd(2);
    h1 = gh1(fFilename.Data(),fModule.Data(),"gaus/dtnorm_01");
    h1->SetTitle("reduced T0");
    h1->GetXaxis()->SetTitle("reduced T0");
    h1->SetMinimum(0);
    h1->Rebin(2);
    h1->Fit("pol0","","ep");
  }
  if (Figure == 41) {
//-----------------------------------------------------------------------------
//  Fig 41: exp1 fit chi2
//-----------------------------------------------------------------------------
    TH1F    *h1;
    
    fCanvas  = new_slide(name,title,2,2,1000,700);

    fP1      = (TPad*) fCanvas->GetPrimitive("p1");

					// reconstructed Y
    fP1->cd(1);
    h1 = gh1(fFilename.Data(),fModule.Data(),"exp1/chi2_00");
    h1->SetTitle("#chi^2/NDof of the EXP1 fit");
    h1->GetXaxis()->SetRangeUser(0,9.99);
    h1->GetXaxis()->SetTitle("#chi2/NDOF");
    h1->Draw();
   
					// reconstructed Z
    fP1->cd(2);
    h1 = gh1(fFilename.Data(),fModule.Data(),"exp1/chi2_01");
    h1->SetTitle("#chi^2/NDof of the EXP1 fit");
    h1->GetXaxis()->SetRangeUser(0,9.99);
    h1->GetXaxis()->SetTitle("#chi2/NDOF");
    h1->Draw();

					// n fit points
    fP1->cd(3);
    h1 = gh1(fFilename.Data(),fModule.Data(),"exp1/npt_00");
    h1->Draw();
   
					// n fit points
    fP1->cd(4);
    h1 = gh1(fFilename.Data(),fModule.Data(),"exp1/npt_01");
    h1->Draw();
  }
  if (Figure == 43) {
//-----------------------------------------------------------------------------
//  Fig 43: exp1 timing resolution vs chi2 cut
//-----------------------------------------------------------------------------
    TH1F    *h1;
    TLegend *leg;
    double  chi2[4] = { 2., 3., 5., 10.};
    double  ex[4] = { 0,0,0,0};
    double  fwhm [4], efw[4], fwhm_cfd [4], efw_cfd[4];
    
    fCanvas  = new_slide(name,title,1,1,1000,500);

    fP1      = (TPad*) fCanvas->GetPrimitive("p1");

					// reconstructed Y
    fP1->cd(1);
//-----------------------------------------------------------------------------
// get timing for gaussian fit
//-----------------------------------------------------------------------------
    h1 = gh1(fFilename.Data(),fModule.Data(),"exp1/dt2");
    h1->Fit("gaus","q0");
    fwhm[3] = h1->GetFunction("gaus")->GetParameter(2)*2.35;
    efw [3] = h1->GetFunction("gaus")->GetParError(2)*2.35;

    h1 = gh1(fFilename.Data(),fModule.Data(),"exp1/dt5");
    h1->Fit("gaus","q0");
    fwhm[2] = h1->GetFunction("gaus")->GetParameter(2)*2.35;
    efw [2] = h1->GetFunction("gaus")->GetParError(2)*2.35;

    h1 = gh1(fFilename.Data(),fModule.Data(),"exp1/dt6");
    h1->Fit("gaus","q0");
    fwhm[1] = h1->GetFunction("gaus")->GetParameter(2)*2.35;
    efw [1] = h1->GetFunction("gaus")->GetParError(2)*2.35;

    h1 = gh1(fFilename.Data(),fModule.Data(),"exp1/dt7");
    h1->Fit("gaus","q0");
    fwhm[0] = h1->GetFunction("gaus")->GetParameter(2)*2.35;
    efw [0] = h1->GetFunction("gaus")->GetParError(2)*2.35;

    TGraphErrors* gr = new TGraphErrors(4,chi2,fwhm,ex,efw);
//-----------------------------------------------------------------------------
// get "CFD" resolution
//-----------------------------------------------------------------------------
    h1 = gh1(fFilename.Data(),fModule.Data(),"exp1/dtl_2");
    h1->Fit("gaus","q0");
    fwhm_cfd[3] = h1->GetFunction("gaus")->GetParameter(2)*2.35;
    efw_cfd [3] = h1->GetFunction("gaus")->GetParError(2)*2.35;

    h1 = gh1(fFilename.Data(),fModule.Data(),"exp1/dtl_5");
    h1->Fit("gaus","q0");
    fwhm_cfd[2] = h1->GetFunction("gaus")->GetParameter(2)*2.35;
    efw_cfd [2] = h1->GetFunction("gaus")->GetParError(2)*2.35;

    h1 = gh1(fFilename.Data(),fModule.Data(),"exp1/dtl_6");
    h1->Fit("gaus","q0");
    fwhm_cfd[1] = h1->GetFunction("gaus")->GetParameter(2)*2.35;
    efw_cfd [1] = h1->GetFunction("gaus")->GetParError(2)*2.35;

    h1 = gh1(fFilename.Data(),fModule.Data(),"exp1/dtl_7");
    h1->Fit("gaus","q0");
    fwhm_cfd[0] = h1->GetFunction("gaus")->GetParameter(2)*2.35;
    efw_cfd [0] = h1->GetFunction("gaus")->GetParError(2)*2.35;

    TGraphErrors* gr_cfd = new TGraphErrors(4,chi2,fwhm_cfd,ex,efw_cfd);

    TH2F* h2 = new TH2F("h2","EXP1 timing resolution vs chi2 fit cut",1,0,11,1,0,0.5);
    h2->SetStats(0);

    h2->GetXaxis()->SetTitle("#chi^{2}/N_{DOF} cut value");
    h2->GetYaxis()->SetTitle("CTR, ns");
    h2->Draw();

    gr->SetMarkerStyle(20);
    gr->SetMarkerSize(1);
    gr->Draw("Pe");

    gr_cfd->SetMarkerStyle(24);
    gr_cfd->SetMarkerSize(1);
    gr_cfd->Draw("Pe");
//-----------------------------------------------------------------------------
// finally - legend
//-----------------------------------------------------------------------------
    leg = new TLegend(0.6,0.76,0.9,0.89,"");
    leg->AddEntry(gr,"gaussian fit","ep");
    leg->AddEntry(gr_cfd,"CDF timing","ep");
    leg->SetFillColor(0);
    leg->SetBorderSize(0);
    leg->Draw();
  }
  if (Figure == 44) {
//-----------------------------------------------------------------------------
//  Fig 44: dtnorm for EXP1
//-----------------------------------------------------------------------------
    TH1F    *h1;
    
    fCanvas  = new_slide(name,title,2,1,1000,500);

    fP1      = (TPad*) fCanvas->GetPrimitive("p1");

					// dtnorm - reduced T0
    fP1->cd(1);
    h1 = gh1(fFilename.Data(),fModule.Data(),"exp1/dtnorm_00");
    h1->SetTitle("EXP1 fit: T0 reduced, within the first bin");
    h1->GetXaxis()->SetTitle("reduced T0");
    h1->SetMinimum(0);
    h1->Rebin(2);
    h1->Fit("pol0","","ep");
					// dtnorm - reduced T0
    fP1->cd(2);
    h1 = gh1(fFilename.Data(),fModule.Data(),"exp1/dtnorm_01");
    h1->SetTitle("EXP1 fit: reduced T0");
    h1->GetXaxis()->SetTitle("reduced T0");
    h1->SetMinimum(0);
    h1->Rebin(2);
    h1->Fit("pol0","","ep");
  }
  if (Figure == 45) {
//-----------------------------------------------------------------------------
//  Fig 45: EXP1 fit parameters
//-----------------------------------------------------------------------------
    TH1F    *h1;
    
    fCanvas  = new_slide(name,title,3,1,1000,400);

    fP1      = (TPad*) fCanvas->GetPrimitive("p1");

					// P0
    fP1->cd(1);
    h1 = gh1(fFilename.Data(),fModule.Data(),"exp1/p0_0");
    h1->SetTitle("EXP1 fit: P0");
    h1->GetXaxis()->SetRangeUser(0.,0.0999);
    h1->GetXaxis()->SetTitle("P0");
    h1->Draw();

    h1 = gh1(fFilename.Data(),fModule.Data(),"exp1/p0_1");
    h1->Draw("same");
					// P1
    fP1->cd(2);
    h1 = gh1(fFilename.Data(),fModule.Data(),"exp1/p1_0");
    h1->SetTitle("EXP1 fit: P1");
    h1->GetXaxis()->SetTitle("P1");
    h1->Draw();

    h1 = gh1(fFilename.Data(),fModule.Data(),"exp1/p1_1");
    h1->Draw("same");
					// P2
    fP1->cd(3);
    h1 = gh1(fFilename.Data(),fModule.Data(),"exp1/p2_0");
    h1->SetTitle("EXP1 fit: P2");
    h1->GetXaxis()->SetTitle("P2");
    h1->Draw();

    h1 = gh1(fFilename.Data(),fModule.Data(),"exp1/p2_1");
    h1->Draw("same");
  }
  if (Figure == 53) {
//-----------------------------------------------------------------------------
//  Fig 53: CTR: GAUS vs EXP1 for different chi2 cuts
//-----------------------------------------------------------------------------
    TH1F    *h1;
    TLegend *leg;
    double  chi2[4] = { 2., 3., 5., 10.};
    double  ex[4] = { 0,0,0,0};
    double  fwhm [4], efw[4], fwhm_cfd [4], efw_cfd[4];
    
    fCanvas  = new_slide(name,title,1,1,1000,500);

    fP1      = (TPad*) fCanvas->GetPrimitive("p1");

					// reconstructed Y
    fP1->cd(1);
//-----------------------------------------------------------------------------
// get timing for gaussian fit
//-----------------------------------------------------------------------------
    h1 = gh1(fFilename.Data(),fModule.Data(),"gaus/dt2");
    h1->Fit("gaus","q0");
    fwhm[3] = h1->GetFunction("gaus")->GetParameter(2)*2.35;
    efw [3] = h1->GetFunction("gaus")->GetParError(2)*2.35;

    h1 = gh1(fFilename.Data(),fModule.Data(),"gaus/dt5");
    h1->Fit("gaus","q0");
    fwhm[2] = h1->GetFunction("gaus")->GetParameter(2)*2.35;
    efw [2] = h1->GetFunction("gaus")->GetParError(2)*2.35;

    h1 = gh1(fFilename.Data(),fModule.Data(),"gaus/dt6");
    h1->Fit("gaus","q0");
    fwhm[1] = h1->GetFunction("gaus")->GetParameter(2)*2.35;
    efw [1] = h1->GetFunction("gaus")->GetParError(2)*2.35;

    h1 = gh1(fFilename.Data(),fModule.Data(),"gaus/dt7");
    h1->Fit("gaus","q0");
    fwhm[0] = h1->GetFunction("gaus")->GetParameter(2)*2.35;
    efw [0] = h1->GetFunction("gaus")->GetParError(2)*2.35;

    TGraphErrors* gr = new TGraphErrors(4,chi2,fwhm,ex,efw);
//-----------------------------------------------------------------------------
// get "CFD" resolution
//-----------------------------------------------------------------------------
    h1 = gh1(fFilename.Data(),fModule.Data(),"exp1/dt2");
    h1->Fit("gaus","q0");
    fwhm_cfd[3] = h1->GetFunction("gaus")->GetParameter(2)*2.35;
    efw_cfd [3] = h1->GetFunction("gaus")->GetParError(2)*2.35;

    h1 = gh1(fFilename.Data(),fModule.Data(),"exp1/dt5");
    h1->Fit("gaus","q0");
    fwhm_cfd[2] = h1->GetFunction("gaus")->GetParameter(2)*2.35;
    efw_cfd [2] = h1->GetFunction("gaus")->GetParError(2)*2.35;

    h1 = gh1(fFilename.Data(),fModule.Data(),"exp1/dt6");
    h1->Fit("gaus","q0");
    fwhm_cfd[1] = h1->GetFunction("gaus")->GetParameter(2)*2.35;
    efw_cfd [1] = h1->GetFunction("gaus")->GetParError(2)*2.35;

    h1 = gh1(fFilename.Data(),fModule.Data(),"exp1/dt7");
    h1->Fit("gaus","q0");
    fwhm_cfd[0] = h1->GetFunction("gaus")->GetParameter(2)*2.35;
    efw_cfd [0] = h1->GetFunction("gaus")->GetParError(2)*2.35;

    TGraphErrors* gr_cfd = new TGraphErrors(4,chi2,fwhm_cfd,ex,efw_cfd);

    TH2F* h2 = new TH2F("h2","GAUS AND EXP1 timing resolution vs chi2 fit cut",1,0,11,1,0,0.5);
    h2->SetStats(0);

    h2->GetXaxis()->SetTitle("#chi^{2}/N_{DOF} cut value");
    h2->GetYaxis()->SetTitle("CTR, ns");
    h2->Draw();

    gr->SetMarkerStyle(20);
    gr->SetMarkerSize(1);
    gr->Draw("Pe");

    gr_cfd->SetMarkerStyle(24);
    gr_cfd->SetMarkerSize(1);
    gr_cfd->Draw("Pe");
//-----------------------------------------------------------------------------
// finally - legend
//-----------------------------------------------------------------------------
    leg = new TLegend(0.6,0.76,0.9,0.89,"");
    leg->AddEntry(gr,"timing by GAUS","ep");
    leg->AddEntry(gr_cfd,"timing by EXP1","ep");
    leg->SetFillColor(0);
    leg->SetBorderSize(0);
    leg->Draw();
  }
    
}

