///////////////////////////////////////////////////////////////////////////////
// Histograms for PDE of the different PMT's and SiPM's
//
///////////////////////////////////////////////////////////////////////////////
#include "ana/TPMT.hh"

ClassImp(TPMT);

//-----------------------------------------------------------------------------
TPMT::~TPMT() {
  delete fRandom;
}


TPMT::TPMT(const char* Name) :  TNamed(Name,Name) {

  fRandom = new TRandom3();

  if (strcmp(Name,"R9800")== 0) {
   // Hamamatsu R9800 for FWHM=270 ps sigma ~ 115 ps;
    fPmtJitter     = 0.115;
    fPhETravelTime = 1.0;		// random, 1 ns
    MakeDefaultPDEHist(&fSplPDE);
    sptr_default(&fSPTR);
    
  }
  else if (strcmp(Name,"STM") == 0) {
  					// STM SiPM
    fPmtJitter     = 0.5;
    fPhETravelTime = 1.0;		// random, 1 ns
    MakeDefaultPDEHist(&fSplPDE);
    sptr_STM_sipm(&fSPTR);
  }
  else if (strcmp(Name, "MPPC") == 0) {
  					// Hamamatsu SiPM
    fPmtJitter     = 0.5;
    fPhETravelTime = 1.0;		// random, 1 ns
    MakeHamamatsuSiPMPDEHist(&fSplPDE); 
    sptr_MPPC_sipm(&fSPTR);
  }
 else if (strcmp(Name, "Photek240") == 0) {
  					// Photek 240x96
    fPmtJitter     = 0.100/2.35;
    fPhETravelTime = 1.0;		// 1 ns
    MakeDefaultPDEHist(&fSplPDE);        //add pointer to PDE hist
    sptr_default(&fSPTR);
 

  }
  else {
 					// trying to match the data
    fPmtJitter     = 0.100;
    fPhETravelTime = 1.0;		//52 1 ns
    MakeDefaultPDEHist(&fSplPDE);
    sptr_default(&fSPTR);
  }
}


// //-----------------------------------------------------------------------------
// void TPMT::MakeHR9800PDEHist(TSpline3** spl_PDE) {
// }

// //-----------------------------------------------------------------------------
// void TPMT::MakeSTMPDEHist(TSpline3** spl_PDE) {
//   MakeDefaultPDEHist(spl_PDE);
// }

// //-----------------------------------------------------------------------------
// void TPMT::MakePhotek240PDEHist(TSpline3** spl_PDE) {
//   MakeDefaultPDEHist(spl_PDE);
// }


// //-----------------------------------------------------------------------------
// void TPMT::MakeMatchDataPDEHist(TSpline3** spl_PDE) {
//   MakeDefaultPDEHist(spl_PDE);
// }

//-----------------------------------------------------------------------------
//default photon detection efficiency, a constant 1 for all wavelengths

void TPMT::MakeDefaultPDEHist(TSpline3** Spline) {

  double wl0, wscale, x0,y0,eff0,effscale;


  double data[] = {
    10.,1.,   20.,1.,   40.,1.,   100.,1.,
    200.,1.,  400.,1.,  1000.,1.,  2000.,1.,
    -1.
  };

  int np  = 0;
  int loc = 0;

  double wl[10], eff[10];

  for (int i=0; data[2*i]>=0; i++) {

    loc = 2*i;

    wl [i]   = data[loc ];
    eff[i]   = data[loc+1];

    np++;
  }

  TGraph* gr = new TGraph(np,wl,eff);
  gr->SetMarkerStyle(20);
  gr->SetMarkerSize(0.5);
  //gr->Draw("alp");

  (*Spline) = new TSpline3("default_pde",wl,eff,np,"",0,0);
  (*Spline)->SetLineColor(2);
  //(*Spline)->Draw("same");
  return;
}
//---------------------------------------------------------------

void TPMT::sptr_default(TH1F** gauss){
  (*gauss) = new TH1F("","",100,10,30);
  
  for (int i=1; i<=100; i++){
    double xbin = (*gauss) -> GetBinCenter(i);
    double y = TMath::Gaus(xbin,20,1,kTRUE);
    (*gauss) -> SetBinContent(i,y);
    return;
  }
}

//-----------------------------------------------------------------------------
double TPMT::GetSPTR() {

  if ((strcmp(GetName(),"R9800") == 0) || (strcmp(GetName(),"Photek240") == 0)) {
    return fPhETravelTime;   // **CONSTANT*;
  }
  else {
     return  fSPTR->GetRandom();
  }
}


//-----------------------------------------------------------------------------
double TPMT::GetPDE(double Wavelength) {
  double pde;
  pde = fSplPDE->Eval(Wavelength);
  return pde;
}

