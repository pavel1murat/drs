#ifndef TScintillator_hh
#define TScintillator_hh

#include "TNamed.h"
#include "TH1.h"
#include "TRandom3.h"
#include "TFile.h"
#include "TTree.h"
#include "TSpline.h"


class TScintillator : public TNamed {
public:

  double  fNPhotonsPerMeV;
  int     fNEmissionLines;
  double  fMeanNpe;        // parameter of a coll to Poisson
  double  fWeight[10];
  double  fDecayTime[10];
  TH1F*   fEmissionSpectrum[10];

  TRandom3* fRn;
//-----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------
  TScintillator(const char* Name);
  ~TScintillator();
 
  double GetDecayTime(int I)            { return fDecayTime[I]; }
  int    GetNEmissionLines()            { return fNEmissionLines; }
  double GetMeanNpe()                   { return fMeanNpe; }
  double GetPhotonProperties(double* DecayTime, double* WaveLength);

  // decay time and the wavelength for the next photon

  //int GetPhotonProperties(double* DecayTime, double* WaveLength);

  void create_baf2fast_emission_spectrum(TH1F** Hist);
  void create_baf2slow_emission_spectrum(TH1F** Hist);
  void create_lyso_emission_spectrum(TH1F** Hist); 
  void create_labr3_emission_spectrum(TH1F** Hist);
  void create_cebr3_emission_spectrum(TH1F** Hist);

  void plot_baf2fast_emission_spectrum(const char* Option = "alp");
  void plot_baf2slow_emission_spectrum(const char* Option = "alp");
  void plot_lyso_emission_spectrum    (const char* Option = "alp"); 
  void plot_labr3_emission_spectrum   (const char* Option = "alp");
  void plot_cebr3_emission_spectrum   (const char* Option = "alp");

  ClassDef(TScintillator,0)
    };

#endif
