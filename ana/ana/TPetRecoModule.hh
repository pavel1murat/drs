#ifndef TPetRecoModule_hh
#define TPetRecoModule_hh
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"

#include "Stntuple/loop/TStnModule.hh"

class TVme1742DataBlock ;

class TPetClusterFinder;
class TTofDetector;
class TTofCluster;
class TCanvas;

class TPetRecoModule: public TStnModule {
//-----------------------------------------------------------------------------
//  histograms
//-----------------------------------------------------------------------------
public:

  enum {kMaxChannels = 32 };

  struct ClusterHist_t {
    TH1F* fCharge;
    TH1F* fPH;			// pulse height
    TH1F* fSize;		// N(crystals)
  };

  struct EventHist_t {
    TH1F*         fRunNumber;
    TH1F*         fEventNumber;
    TH1F*         fNClusters;
    TH1F*         fNHitCrystals;
    TH2F*         fQEvent;
    TH2F*         fVMaxEvent;
    TH1F*         fV     [kMaxChannels]; // per event
    TH2F*         fU     [kMaxChannels]; // all events
    TH1F*         fQCh   [kMaxChannels]; // integrated charge per channel
    TH1F*         fVMaxCh[kMaxChannels]; // per event

    TH2F*         fSeed[2];
    TH1F*         fQMax[2];
    TH1F*         fVMax[2];
    TH2F*         fVMaxVsChannel[2];
    
  };

  // event SET  0: all    events
  // event SET  1: passed events

  enum { kNEventHistSets   = 10 };
  enum { kNClusterHistSets = 10 };

  struct Hist_t {
    EventHist_t       fEvent    [kNEventHistSets   ];
    ClusterHist_t     fCluster  [kNClusterHistSets ];
  };

  Hist_t              fHist;		// ! 
  int                 fHistSet;

  TCanvas*            fCanvas;
//-----------------------------------------------------------------------------
  TTofDetector*   fTofDetector;

  TObjArray*      fListOfClusters;

				        // reconstruction algorithm
  TPetClusterFinder*   fPetReco;
				        // pointers to the data blocks used

  TVme1742DataBlock*   fVme1742DataBlock;

//-----------------------------------------------------------------------------
//  functions
//-----------------------------------------------------------------------------
public:
  TPetRecoModule(const char* name="PetReco", const char* title="PetReco");
  ~TPetRecoModule();
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
  void    BookClusterHistograms(ClusterHist_t*  Hist, const char* Folder);
  void    BookEventHistograms  (EventHist_t*    Hist, const char* Folder);
  void    BookHistograms       ();

  int     FillClusterHistograms(ClusterHist_t* Hist, TTofCluster* Cluster);
  int     FillEventHistograms  (EventHist_t*   Hist);
  int     FillHistograms       ();

  int     Debug(int Passed);

  int     DrawPH();

  ClassDef(TPetRecoModule,0)
};

#endif
