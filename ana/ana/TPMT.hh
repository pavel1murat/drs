#ifndef TPMT_hh
#define TPMT_hh

#include "TNamed.h"
#include "TH1.h"
#include "TFile.h"
#include "TSpline.h"
#include "TH1.h"
#include "TF1.h"
#include "TMath.h"
#include "TRandom3.h"


class TPMT : public TNamed {
public:
 
 
  TRandom3*   fRandom;
  double      fPmtJitter ;
  double      fGain;
  double      fPhETravelTime;
  TSpline3*   fSplPDE;
  TH1F*       fSPTR;
  
  TPMT(const char* Name);
  ~TPMT();

//   static void MakeHR9800PDEHist(TSpline3** spl_PDE);
//   static void MakeSTMPDEHist(TSpline3** spl_PDE);
//   static void MakePhotek240PDEHist(TSpline3** spl_PDE);
//   static void MakeMatchDataPDEHist(TSpline3** spl_PDE);
  
  static void MakeHamamatsuSiPMPDEHist(TSpline3** spl_PDE);
 
  static void MakeDefaultPDEHist(TSpline3** spl_PDE);            //default hist that gives PDE
                                                                 // of 1 for all wavelengths

  static void sptr_MPPC_sipm(TH1F** Hist);
  static void sptr_STM_sipm(TH1F** Hist);
  static void sptr_default(TH1F** Hist);

  double GetSPTR();
  double GetJitter() { return fRandom->Gaus(0,fPmtJitter); }
  double GetPDE(double Wavelength);
  


 ClassDef(TPMT,0);
};

#endif
