///////////////////////////////////////////////////////////////////////////////
// fPhETravelTime: time, needed for electron to travel from a photocathode 
//                 to the first dinode (MCP)...
//
///////////////////////////////////////////////////////////////////////////////
#include "ana/TScintillator.hh"

ClassImp(TScintillator)

//-----------------------------------------------------------------------------
TScintillator::~TScintillator() {
  delete fRn;
}

//-----------------------------------------------------------------------------
TScintillator::TScintillator(const char* Name) :  TNamed(Name,Name) {

  if (strcmp(Name,"lyso") == 0) {
    fNPhotonsPerMeV = 26000;
    fNEmissionLines = 1;
    fDecayTime[0]   = 50; // ns
    fWeight   [0]   = 1.;
    create_lyso_emission_spectrum(&fEmissionSpectrum[0]);
  }

  else if (strcmp(Name,"lso") == 0) {
    fNPhotonsPerMeV     = 26000;
    fNEmissionLines = 1;
    fDecayTime[0]   = 40; // ns
    fWeight   [0]   = 1.;
    create_lyso_emission_spectrum(&fEmissionSpectrum[0]);
    
  }
  else if (strcmp(Name,"labr3") == 0) {
    fNPhotonsPerMeV      = 61000;
    fNEmissionLines = 1;
    fDecayTime[0]   = 16; // ns
    fWeight   [0]   = 1.;
    create_labr3_emission_spectrum(&fEmissionSpectrum[0]);
  }
  else if (strcmp(Name,"cebr3") == 0) {
    fNPhotonsPerMeV      = 68000;
    fNEmissionLines = 1;
    fDecayTime[0]   = 27; // ns
    fWeight   [0]   = 1.;
    create_cebr3_emission_spectrum(&fEmissionSpectrum[0]);
    
  }
  else if (strcmp(Name,"baf2") == 0) {
    fNPhotonsPerMeV      = 12000;
    fNEmissionLines = 2;
    fDecayTime[0]   = 0.6; //ns
    fDecayTime[1]   = 625; // ns
    fWeight   [0]   = 0.2;
    fWeight   [1]   = 0.8;
    create_baf2fast_emission_spectrum(&fEmissionSpectrum[0]);
    create_baf2slow_emission_spectrum(&fEmissionSpectrum[1]);
      
  }
  
  fMeanNpe     =  fNPhotonsPerMeV*0.511;	// photon from e+e- annihilation at rest
  fRn = new TRandom3();
}

//-----------------------------------------------------------------------------
double TScintillator::GetPhotonProperties(double* DecayTime, double* WaveLength) {
  
  int i(1);

  *WaveLength = 0;

  // define the emission line

  int line;

  if (fNEmissionLines == 1) {
    line        = 0;
  }
  else {
    // more than one line - not defined yet..
    // look at the weights and choose the line
    double  prob;
    prob = fRn->Rndm(i);
    if (prob < fWeight[0]){
      line  = 0;
    }
    else {
      line = 1;
    }
  }
  
  // determine the photon wavelength
  *DecayTime  = fDecayTime[line];
  *WaveLength = fEmissionSpectrum[line]->GetRandom();

  return -1;
}

// int TScintillator::GetMeanNpe(double *LightYield){
//  *LightYield = fMeanNpe;
// }
