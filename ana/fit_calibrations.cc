///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
#include "TMath.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TPad.h"
#include "TCanvas.h"

#include "Stntuple/val/stntuple_val_functions.hh"

#include "ana/fit_calibrations.hh"

ClassImp(TFitCalibrations) 

//-----------------------------------------------------------------------------
double TFitCalibrations::fitf(double* x, double* p) {
  double dx, f;

  dx = (x[0]-p[1])/p[2];
  f  = p[0]*TMath::Exp(-dx*dx/2.);

  return f;
}

//-----------------------------------------------------------------------------
// calibration constants for all cells of a given VME1742 channel
//-----------------------------------------------------------------------------
int TFitCalibrations::fit_calib_channel(int Channel, int FirstCell, int NCells, 
		      const char* Mode,
		      const char* OutputFilename) {

  char hist_name[200], fn[200];

  double y[npt], ey[npt], ybin, ymax, xmn;

  int   loc, first_cell, last_cell;

  double fft[1024][3];   // offset, slope, chi2

//   const char fn[npt][200] = {
//     "hist/clb001.000012.drs_example.hist", // -0.4V
//     "hist/clb001.000011.drs_example.hist", // -0.25V
//     "hist/clb001.000013.drs_example.hist", //  0.0V
//     "hist/clb001.000014.drs_example.hist"  //  0.4V
//   };
				// in millivolts

  //  double x [npt] = {-400., -250.,  0., 400.} ;

  double x [npt] = {-400., -200.,  0., 200., 400.} ;
  double ex[npt] = {   0.,    0.,  0.,   0.,   0.} ;

  int run_number [] = { 
    32, 70, 106, 138, 171,  // channel #00
    33, 72, 107, 139, 172,  // channel #01
    34, 73, 108, 141, 173,  // channel #02
    35, 74, 109, 142, 174,  // channel #03
    36, 75, 110, 143, 175,  // channel #04
    37, 76, 111, 144, 176,  // channel #05
    38, 77, 112, 145, 177,  // channel #06
    39, 78, 113, 146, 178,  // channel #07
    40, 79, 114, 147, 179,  // channel #08
    41, 80, 115, 148, 180,  // channel #09
    42, 81, 116, 149, 181,  // channel #10
    43, 82, 117, 150, 182,  // channel #11
    44, 83, 118, 151, 183,  // channel #12
    45, 84, 119, 152, 184,  // channel #13
    46, 85, 120, 153, 185,  // channel #14
    47, 86, 121, 154, 186,  // channel #15
    53, 87, 122, 155, 187,  // channel #16
    54, 88, 123, 156, 188,  // channel #17
    55, 89, 124, 157, 189,  // channel #18
    56, 90, 125, 158, 190,  // channel #19
    57, 91, 126, 159, 191,  // channel #20
    58, 92, 127, 160, 192,  // channel #21
    59, 93, 128, 161, 193,  // channel #22
    60, 94, 129, 162, 194,  // channel #23
    61, 95, 130, 163, 195,  // channel #24
    62, 96, 131, 164, 196,  // channel #25
    63,100, 132, 165, 197,  // channel #26
    64,101, 133, 166, 198,  // channel #27
    65,102, 134, 167, 200,  // channel #28
    66,103, 135, 168, 201,  // channel #29
    67,104, 136, 169, 202,  // channel #30
    68,105, 137, 170, 203,  // channel #31
    -1
  };


  TF1* f = new TF1("f",fitf,0.,4100.,3);

  TCanvas* c  = new_slide(Form("channel_%02i",Channel),
			  Form("channel_%02i",Channel),3,2,1000,700);

  TPad* p1    = (TPad*) c->GetPrimitive("p1");
  TPad* title = (TPad*) c->GetPrimitive("title");

  TString mode = Mode;

  h_slope [Channel]    = new TH1F(Form("slope_%03i" ,Channel),"Slope" ,200,3.6,4.);
  h_offset[Channel]    = new TH1F(Form("offset_%03i",Channel),"offset",500,1300,2300);
  h_chi2  [Channel]    = new TH1F(Form("chi2_%03i"  ,Channel),"chi2"  ,200,0,20);

  loc = npt*Channel;

  first_cell = FirstCell;
  last_cell  = FirstCell+NCells;

  for (int i=0; i<npt; i++) {

    sprintf(hist_name,"event_0/wf1_%i",Channel);
    sprintf(fn, "hist/clb001.%06i.drs_example.hist",run_number[loc+i]);

    h2[i] = gh2(fn,"PetAna",hist_name);

    for (int icell=first_cell; icell<last_cell; icell++) {
      hpy[icell][i] = h2[i]->ProjectionY(Form("py_%i_%i",i,icell),icell+1,icell+1);

      // search for a channel with max contents

      int nbins  = hpy[icell][i]->GetNbinsX();

      ymax = -100.;
      xmn  = -1;

      for (int ibin=1; ibin<=nbins; ibin++) {
	ybin = hpy[icell][i]->GetBinContent(ibin);
	if (ybin > ymax) {
	  ymax = ybin;
	  xmn  = ibin+0.5;
	}
      }

      if (mode.Index("debug") >= 0) {
	printf(" icell = %5i xmn = %10.3f\n",icell, xmn);
      }

      f->SetParameter(0,100.);
      f->SetParameter(1,xmn);
      f->SetParameter(2,3.5);

      f->SetParError(0,10.);
      f->SetParError(1,2.);
      f->SetParError(2,1.);

      p1->cd(i+1); 
      hpy[icell][i]->GetXaxis()->SetRangeUser(xmn-20,xmn+20);
      hpy[icell][i]->Fit(f,"Q","",xmn-20.,xmn+20.);
//-----------------------------------------------------------------------------
// if fit went well, store fit results
//-----------------------------------------------------------------------------
      par[icell][i][0] = f->GetParameter(1);
      par[icell][i][1] = f->GetParameter(2);
      par[icell][i][2] = f->GetChisquare();

      err[icell][i][0] = f->GetParError(1);
      err[icell][i][1] = f->GetParError(2);
    }
  }
//-----------------------------------------------------------------------------
// now fit straight lines through the points
//-----------------------------------------------------------------------------
  FILE* file(0);

  if (OutputFilename != 0) {
    file = fopen(OutputFilename,"w");
  }
  else {
    file = fopen("/dev/stdout","w");
  }

//------------------------------------------------------------------------------
// fit and plot the calibration results
//------------------------------------------------------------------------------
  for (int icell=first_cell; icell<last_cell; icell++) {
    c->SetName (Form("channel_%02i_cell_%04i",Channel,icell));
    c->SetTitle(Form("channel %02i cell %04i",Channel,icell));

    for (int i=0; i<npt; i++) {
      y [i] = par[icell][i][0];
      ey[i] = par[icell][i][1];
      //      printf(" x, y, ey = %10.4f %10.4f %10.4f\n",x[i], y[i], ey[i]);
    }

//-----------------------------------------------------------------------------
// perform straight line fit
//-----------------------------------------------------------------------------
    p1->cd(npt+1); 
    g_calib[Channel][icell] = new TGraphErrors(npt,x,y,ex,ey);
    g_calib[Channel][icell]->Fit("pol1","Q");
    g_calib[Channel][icell]->Draw("ap");

    f = g_calib[Channel][icell]->GetFunction("pol1");

    fft[icell][0] = f->GetParameter(0);
    fft[icell][1] = f->GetParameter(1);
    fft[icell][2] = f->GetChisquare ();

    fprintf(file," %4i %5i %10.3f  %12.5f  %10.3f\n", 
	    Channel, icell, fft[icell][0], fft[icell][1], fft[icell][2]);

    
  }

  fclose(file);
//-----------------------------------------------------------------------------
// finally, histogram the results
//-----------------------------------------------------------------------------
  for (int icell=0; icell<NCells; icell++) {
    h_offset[Channel]->Fill(fft[icell][0]);
    h_slope [Channel]->Fill(fft[icell][1]);
    h_chi2  [Channel]->Fill(fft[icell][2]);
  }
  
  char  cname[200];

  TCanvas* c1  = new_slide(Form("channel_%02i_calib",Channel),
			   Form("channel_%02i_calib",Channel),3,1,1200,500);

  p1 = (TPad*) c1->GetPrimitive("p1");

  p1->cd(1); h_offset[Channel]->Draw();
  p1->cd(2); h_slope [Channel]->Draw();
  p1->cd(3); h_chi2  [Channel]->Draw();

  sprintf(cname,"channel_%02i_calib.eps",Channel);
  c1->Print(cname);
  sprintf(cname,"channel_%02i_calib.png",Channel);
  c1->Print(cname);

  return 0;
}


//-----------------------------------------------------------------------------
int TFitCalibrations::fit_calib_module (int IMin, int IMax) {

  char fn[200];

  int const nch(32);

  h_mean_slope  = new TH1F("h_mean_slope" ,"MEAN  SLOPE" , nch,0,nch);
  h_sigm_slope  = new TH1F("h_sigm_slope" ,"SIGMA SLOPE" , nch,0,nch);
  h_mean_offset = new TH1F("h_mean_offset","MEAN  OFFSET", nch,0,nch);
  h_sigm_offset = new TH1F("h_sigm_offset","SIGMA OFFSET", nch,0,nch);
  h_mean_chi2   = new TH1F("h_mean_slope" ,"MEAN  Chi2"  , nch,0,nch);
  h_sigm_chi2   = new TH1F("h_sigm_slope" ,"SIGMA Chi2"  , nch,0,nch);


  for (int i=IMin; i<IMax; i++) {
    sprintf(fn,"ampl_channel_%03i.txt",i);
    fit_calib_channel(i,0,1024,"",fn);

    h_mean_offset->SetBinContent(i+1,h_offset[i]->GetMean());
    h_sigm_offset->SetBinContent(i+1,h_offset[i]->GetRMS ());
    h_mean_slope ->SetBinContent(i+1,h_slope [i]->GetMean());
    h_sigm_slope ->SetBinContent(i+1,h_slope [i]->GetRMS ());
    h_mean_chi2  ->SetBinContent(i+1,h_chi2  [i]->GetMean());
    h_sigm_chi2  ->SetBinContent(i+1,h_chi2  [i]->GetRMS ());
  }

  
  TCanvas* cc  = new_slide("calib_summary","Calibration Summary",3,2,1200,600);

  TPad* p1 = (TPad*) cc->GetPrimitive("p1");

  p1->cd(1); h_mean_offset->Draw();
  p1->cd(2); h_mean_slope ->Draw();
  p1->cd(3); h_mean_chi2  ->Draw();
  p1->cd(4); h_sigm_offset->Draw();
  p1->cd(5); h_sigm_slope ->Draw();
  p1->cd(6); h_sigm_chi2  ->Draw();

  return 0;
}
