///////////////////////////////////////////////////////////////////////////////

namespace {
  const int unused_npt = 3;		// place holder
  const double kSigmaToFwhm = 2*TMath::Sqrt(2*TMath::Log(2.));
};

TCanvas*  c_fit_photopeaks;
TCanvas*  c_fits;

//-----------------------------------------------------------------------------
int fit_photopeaks(const char* Filename, double XMin, double XMax) {

  double   mean[32], emean[32], sigm[32], esigm[32], chi2[32], x[32], ex[32], res[32];

  TH2F* h2 = gh2(Filename,"Pet2Ana","event_0/qmax_vs_ich");

  int       icc, ipad, new_canvas;

  char    cname[200];

  TH1F* h_mean = new TH1F("h_mean","Photopeak mean",32,0,32);
  TH1F* h_sigm = new TH1F("h_sigm","Photopeak width",32,0,32);
  TH1F* h_chi2 = new TH1F("h_chi2","Photopeak fit chi2/Dof",32,0,32);
  TH1F* h_fwhm = new TH1F("h_fwhm","Photopeak FWHM",32,0,32);


  new_canvas = 1;
  icc        = 0;

  for (int i=0; i<32; i++) {

    if (new_canvas == 1) {
      icc++;

      sprintf(cname,"fit_canvas_%i",icc);

      c_fits = new TCanvas(cname,cname,1200,500);
      c_fits->Divide(4,2);

      new_canvas = 0;
      ipad       = 0;
    }

    ipad++;
    c_fits->cd(ipad);
    if (ipad == 8) new_canvas = 1;

    int bin = i+1;
//-----------------------------------------------------------------------------
// a histogram name has the channel number, not the bin number, in it
//-----------------------------------------------------------------------------
    TH1D* hpy = h2->ProjectionY(Form("py_%02i",i),bin,bin);
    hpy->Fit("gaus","q0","",XMin,XMax);

    f = hpy->GetFunction("gaus");

    if (f == 0) {
      mean [i] = -1.;
      sigm [i] = -1.;
      res  [i] = -1.;
    }
    else {
      mean [i] = f->GetParameter(1);
      sigm [i] = f->GetParameter(2);

      hpy->Fit("gaus","q","",mean[i]-2.*fabs(sigm[i]),mean[i]+2.*fabs(sigm[i]));
      f = hpy->GetFunction("gaus");

      mean [i] = f->GetParameter(1);
      emean[i] = f->GetParError (1);
      esigm[i] = f->GetParError (2);
      chi2 [i] = f->GetChisquare ();
      x    [i] = i;
      ex   [i] = 0;
      
      h_mean->SetBinContent(i+1,mean [i]);
      h_mean->SetBinError  (i+1,emean[i]);
      h_sigm->SetBinContent(i+1,sigm [i]);
      h_sigm->SetBinError  (i+1,esigm[i]);
      
      res[i] = sigm[i]/mean[i]*kSigmaToFwhm;
      
      h_chi2->SetBinContent(i+1,chi2 [i]);
      h_fwhm->SetBinContent(i+1,res  [i]);
    }

    hpy->GetXaxis()->SetRangeUser(0,XMax*2);
    hpy->Draw();
  }


  c_fit_photopeaks = new_slide("c_fit_photopeaks",
			       Form("Photopeak fits: %s [%5.1f-%5.1f]",
				    Filename,XMin,XMax),2,2,1200,800);

  TPad* p1 = (TPad*) c_fit_photopeaks->GetPrimitive("p1");

  p1->cd(1);
  h_mean->SetMarkerStyle(20);
  h_mean->GetYaxis()->SetRangeUser(0,50);
  h_mean->Draw("ep");

  p1->cd(2);
  h_sigm->SetMarkerStyle(20);
  h_sigm->GetYaxis()->SetRangeUser(0,3);
  h_sigm->Draw("ep");

  p1->cd(3);
  h_chi2->GetYaxis()->SetRangeUser(0,100);
  h_chi2->Draw("");

  p1->cd(4);
  h_fwhm->GetYaxis()->SetRangeUser(0,0.25);
  h_fwhm->Fit("pol0");
//-----------------------------------------------------------------------------
// write the constants out
//-----------------------------------------------------------------------------

  for (int i=0; i<32; i++) {
    printf("%3i  %10.3f  %10.3f  %10.3f\n",i,mean[i],sigm[i],chi2[i]);
  }
}


//-----------------------------------------------------------------------------
void print_photopeaks(int RunNumber) {

  char fn[200];

  sprintf(fn,"figures/runs/%06i/fit_photopeaks_summary-%06i.png",RunNumber,RunNumber);
  c_fit_photopeaks->Print(fn);
  sprintf(fn,"figures/runs/%06i/fit_photopeaks_summary-%06i.eps",RunNumber,RunNumber);
  c_fit_photopeaks->Print(fn);

  sprintf(fn,"figures/runs/%06i/fit_photopeaks_fits_1-%06i.png",RunNumber,RunNumber);
  fit_canvas_1->Print(fn);
  sprintf(fn,"figures/runs/%06i/fit_photopeaks_fits_2-%06i.png",RunNumber,RunNumber);
  fit_canvas_2->Print(fn);
  sprintf(fn,"figures/runs/%06i/fit_photopeaks_fits_3-%06i.png",RunNumber,RunNumber);
  fit_canvas_3->Print(fn);
  sprintf(fn,"figures/runs/%06i/fit_photopeaks_fits_4-%06i.png",RunNumber,RunNumber);
  fit_canvas_4->Print(fn);

  sprintf(fn,"figures/runs/%06i/fit_photopeaks_fits_1-%06i.eps",RunNumber,RunNumber);
  fit_canvas_1->Print(fn);
  sprintf(fn,"figures/runs/%06i/fit_photopeaks_fits_2-%06i.eps",RunNumber,RunNumber);
  fit_canvas_2->Print(fn);
  sprintf(fn,"figures/runs/%06i/fit_photopeaks_fits_3-%06i.eps",RunNumber,RunNumber);
  fit_canvas_3->Print(fn);
  sprintf(fn,"figures/runs/%06i/fit_photopeaks_fits_4-%06i.eps",RunNumber,RunNumber);
  fit_canvas_4->Print(fn);

}
