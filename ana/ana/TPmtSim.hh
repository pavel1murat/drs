#ifndef TPmtSim_hh
#define TPmtSim_hh

#include "TNamed.h"
#include "TH1.h"
#include "TRandom3.h"
#include "TFile.h"
#include "TTree.h"
#include "TSpline.h"

class TScintillator;
class TPMT;

class TPmtSim : public TNamed {
public:

  TScintillator*  fScintillator;
  TPMT*           fPmt;
  
  TString fOutputFn;

  struct Hist_t {
    TH1D    *h_npe[2]; 
    TH1D    *h_decay[2];
    TH1D    *h_te[2];
    TH1D    *h_tf[2];
    TH1D    *h_prob[2];
    TH1D    *h_dtpmt[2]; 
    TH1D    *h_t10[2];
    TH1D    *h_te_ev[2];
    TH1D    *h_delta_t;
    TH1D    *h_delta_t10;
   
  };

  //add histograms for PDE of different PMT's and SiPM's
 
  Hist_t  fHist;

  int     fEventNumber;
  int     fNChannels;
  float   fV[4][1024];                  // simulated voltages in two channels
  float   fT[1024];                     // times
  int     fUsedChan[4];

  int     fTc1;				// I don't know what it is...

  double  fNpePerMeV;
  double  fMeanNpe;
  double  fDecayTime;
  double  fPmtJitter ;
  double  fPhotoEfficiency;
  double  fGain;
  double  fPhETravelTime;
					// work variables
  int     fNPe[2];			// number of photoelectrons
  double  fTFirst[2];			// first electron arrival time
  double  fTCfd10[2];			// CFD (10%) emulation

  int     fWriteOutput;
  
  TFile*  fOutputFile;
  TTree*  fOutputTree;
  TRandom3* fRn;
 
//-----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------
  TPmtSim(const char* Scintillator, const char* Pmt);
  ~TPmtSim();

  void BookHistograms(Hist_t* Hist);
  int  GenerateEvents(int NEvents);
  int  SimulateChannel(int Channel);

  ClassDef(TPmtSim,0)
};


#endif
