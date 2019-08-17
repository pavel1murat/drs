//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Oct 20 16:59:33 2011 by ROOT version 5.28/00c
// from TTree drs/produced from DRS4 binary file
// found on file: data/aaaa_001.root
//////////////////////////////////////////////////////////

#ifndef drs_h
#define drs_h

#include "TROOT.h"
#include "TFolder.h"
#include "TChain.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TMath.h"
#include "TGraphErrors.h"
#include "TCanvas.h"

#include "drs/obj/TDrsChannel.hh"
#include "drs/ana/TDrsFitAlg.hh"
#include "drs/ana/TStripLineFitOld.hh"

class TStripLine;

class drsana : public TNamed {

  enum { kMaxNChannels = 10  };
  enum { kNDebugFlags  = 100 };
  enum { kDefaultMode  = 1   };
  enum { kLoopTimeMode = 1   };

public :

  static  drsana*  fgInstance;

  struct EventHist_t {
    TH2F*     fQ2VsQ1;			 //
    TH2F*     fPH2VsPH1;		 //
    TH1F*     fRc;	                 // ! event processing code
   };

  TString            fFilename;	          //! by default run on a single file
  TTree*             fChain;              //! pointer to the analyzed TTree or TChain
  Int_t              fCurrent;            //! current Tree number in a TChain
  int                fRc;
  int                fMode;               //! 0:no stripline, 1:process stripline

  TObjArray*         fListOfAlgorithms;
  TObjArray*         fListOfChannels;

  int                fNStripLines;
  TObjArray*         fListOfStripLines;
  TStripLineFitOld*  fStripLineFit;

  TFolder*           fFolder;
  TFolder*           fAnaFolder;
  TFolder*           fHistFolder;
//-----------------------------------------------------------------------------
// fitting
//-----------------------------------------------------------------------------
					// ! pulse fit results
  int        fDebug[kNDebugFlags];	// ! debug flags
  int        fFitChannel;		// ! channel to fit
  int        fNPar; 			// N(fit parameters)
  int        fMaxMode;                  // ! maximum finding mode, default=1
  int        fEntry;			// ! entry number in the tree 
//-----------------------------------------------------------------------------
// histograms
//-----------------------------------------------------------------------------
  EventHist_t     fHist;

  TCanvas*        fCanvas;	 		 // ! used by Display()
  TCanvas*        fShapeCanvas;
//-----------------------------------------------------------------------------
// Declaration of leaf types
//-----------------------------------------------------------------------------
  Float_t         b1_t [1024];
  Float_t         b1_c[kMaxNChannels][1024];

  Int_t           usedchan[4];
  Int_t           fNChannels;
  Int_t           fEventNumber;
  Int_t           tc1;

  int             fChan[2];   // two analysed channels

  double          fDte;       // delta(T) from exponential fit

  // Int_t           year;
  // Int_t           month;
  // Int_t           day;
  // Int_t           hour;
  // Int_t           minute;
  // Int_t           second;
  // Int_t           millisecond;
//-----------------------------------------------------------------------------
// List of branches
//-----------------------------------------------------------------------------
  TBranch        *b_b1_t;   //!
  TBranch        *b_b1_c1;   //!
  TBranch        *b_b1_c2;   //!
  TBranch        *b_b1_c3;   //!
  TBranch        *b_b1_c4;   //!
  TBranch        *b_nch;        // number of channels, sparsified
  TBranch        *b_usedchan;   //

  TBranch        *b_event;   //!
  TBranch        *b_tc1;        //!

   // TBranch        *b_year;   //!
   // TBranch        *b_month;   //!
   // TBranch        *b_day;   //!
   // TBranch        *b_hour;   //!
   // TBranch        *b_minute;   //!
   // TBranch        *b_second;   //!
   // TBranch        *b_millisecond;   //!
//-----------------------------------------------------------------------------
// methods
//-----------------------------------------------------------------------------
					// Mode = 0: no striplines
					//      = 1: process stripline 

  drsana(const char* Filename=0, int Mode = 0);
  virtual ~drsana();

  virtual Int_t    GetEntry(Long64_t entry);
  virtual Long64_t LoadTree(Long64_t entry);
  virtual void     Init(TTree *tree);

  TDrsChannel*   NewChannel(int I, int ChNumber) {
    TDrsChannel* ch = new TDrsChannel(ChNumber);
    fListOfChannels->AddAt(ch,I);
    return ch;
  }
  
  TDrsChannel*        GetChannel(int I) { return (TDrsChannel*) fListOfChannels->UncheckedAt(I); }
  TFolder*            GetFolder() { return fFolder; }

					// list of channels is sparse, GetEntries() doesn't count
					// zero entries

  int  GetNChannels() { return fListOfChannels->GetLast()+1; }

  TDrsFitAlg*   GetAlgorithm(const char* Name) { 
    return (TDrsFitAlg*) fListOfAlgorithms->FindObject(Name); 
  }

  TDrsFitAlg*   GetAlgorithm(int i) { 
    return (TDrsFitAlg*) fListOfAlgorithms->UncheckedAt(i);
  }

  virtual void     Loop            (int NEvents = -1);
  int              LoopTime        (int NEvents = -1);
  
  virtual Bool_t   Notify();
  virtual void     Show(Long64_t entry = -1);
  
  int              BookHistograms         (EventHist_t* Hist);

  int              FillHistograms     (EventHist_t* Hist);
  
  int              ProcessEvent       (Long64_t entry, int Mode);
  int              ProcessStripLine   (TStripLine* StripLine);
  
  int              Debug       (int IEntry);
  int              DebugFlag   (int I) { return fDebug[I]; }
  int              Display     (const char* Algorithm);

  int              SaveFolder (TFolder* Folder, TDirectory* Dir);
  void             SaveHist   (const char* Filename, Int_t Mode = 2);
//-----------------------------------------------------------------------------
// fitting T0
//-----------------------------------------------------------------------------
  int              FitCellOffsets (int Channel, const char* OutputFile);
  int              ReadCellOffsets(int Channel, const char* OutputFile);

  ClassDef(drsana,0)
};

#endif

