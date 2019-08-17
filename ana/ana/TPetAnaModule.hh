#ifndef TPetAnaModule_hh
#define TPetAnaModule_hh
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"

#include "Stntuple/loop/TStnModule.hh"

class TVme1742DataBlock ;

class TPetAnaModule: public TStnModule {
//-----------------------------------------------------------------------------
//  histograms
//-----------------------------------------------------------------------------
public:

  struct EventHist_t {
    TH1F*         fRunNumber;
    TH1F*         fEventNumber;
    TH1F*         fFirstCell    [ 4];
    TH2F*         fTrigPulse    [ 4];
    TH1F*         fLastTrigPulse[ 4];
    TH2F*         fWaveForm     [32];
    TH1F*         fLastWaveForm [32];
    TH2F*         fWaveForm1    [32];
    TH1F*         fLastWaveForm1[32];
  };

  // event SET  0: all    events
  // event SET  1: passed events

  enum {kNEventHistSets = 2};

  struct Hist_t {
    EventHist_t       fEvent    [kNEventHistSets  ];
  };

  Hist_t              fHist;		// ! 
  int                 fHistSet;
//-----------------------------------------------------------------------------
				        // pointers to the data blocks used

  TVme1742DataBlock*   fVme1742DataBlock;
//-----------------------------------------------------------------------------
//  functions
//-----------------------------------------------------------------------------
public:
  TPetAnaModule(const char* name="PetAna", const char* title="PetAna");
  ~TPetAnaModule();
//-----------------------------------------------------------------------------
// accessors
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// setters
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// overloaded methods of TStnModule
//-----------------------------------------------------------------------------
  int     BeginJob();
  int     BeginRun();
  int     Event   (int ientry);
  int     EndJob  ();
//-----------------------------------------------------------------------------
// other methods
//-----------------------------------------------------------------------------
  void    BookEventHistograms  (EventHist_t&   Hist, const char* Folder);
  void    BookHistograms       ();

  int     FillEventHistograms  (EventHist_t&   Hist);
  int     FillHistograms       ();

  int     Debug(int Passed);

  ClassDef(TPetAnaModule,0)
};

#endif
