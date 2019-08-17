#ifndef TPet2AnaModule_hh
#define TPet2AnaModule_hh
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
class TTofData;
class TStripLine;
class TStripLineFit;
class TCanvas;
class TDrsFitAlgNew;
class TCalibRunRange;

class TPet2AnaModule: public TStnModule {
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

  struct StripLineHist_t {
    TH1F* fDeltaT;              // from the fit 
  };

  struct EventHist_t {
    TH1F*         fRunNumber;
    TH1F*         fEventNumber;
    TH1F*         fNClusters;
    TH1F*         fNHitCrystals;
    TH2F*         fQ1Event;
    TH2F*         fQ1Module[2];
    TH2F*         fVMaxEvent;
    TH1F*         fVTrig [4];              // per event
    TH1F*         fV       [kMaxChannels]; // per event
    TH2F*         fU       [kMaxChannels]; // all events
    TH1F*         fQCh     [kMaxChannels]; // integrated charge per channel
    TH1F*         fQ1Ch    [kMaxChannels]; // Q1 per channel
    TH1F*         fVMaxCh  [kMaxChannels]; // per event
    TH1F*         fPedestal[kMaxChannels]; // per event
    TH1F*         fChi2Ped [kMaxChannels]; // per event

    TH2F*         fSeed [2];
    TH1F*         fQMax [2];
    TH1F*         fQ1Max[2];
    TH1F*         fVMax [2];

    TH2F*         fVMaxVsChannel;
    TH2F*         fQMaxVsChannel;
    TH2F*         fQ1MaxVsChannel;

    TH2F*         fPH2VsPH1;
    TH2F*         fQ2VsQ1;    
    
    TH2F*         fOcc1[4][4];
    TH2F*         fOcc2[4][4];

    TH2F*         fT0VsFirstCell[2];
    TH2F*         fDfc10VsEntry;
    TH2F*         fDfc20VsEntry;
    TH2F*         fDfc30VsEntry;


    TH1F*         fP0Q1Max;
    TH1F*         fP0Q1Sum[2]; // 0:all, 1: Q1max<420
    TH1F*         fP1Q0Max;
    TH1F*         fP1Q0Sum[2];

    TH1F*         fP0C1Dt;
    TH1F*         fP1C0Dt;
  };

  // event SET  0: all    events
  // event SET  1: passed events

  enum { kNEventHistSets     = 100 };
  enum { kNClusterHistSets   = 100 };
  enum { kNStripLineHistSets = 100 };

  struct Hist_t {
    EventHist_t*      fEvent    [kNEventHistSets   ];
    ClusterHist_t*    fCluster  [kNClusterHistSets ];
    StripLineHist_t*  fStripLine[kNStripLineHistSets];
  };

  Hist_t              fHist;		// ! 
  int                 fHistSet;

  TCanvas*            fCanvas;
//-----------------------------------------------------------------------------
  TTofDetector*       fTofDetector;

  TObjArray*          fListOfClusters;
				        // reconstruction algorithm
  TPetClusterFinder*  fPetReco;
				        // pointers to the data blocks used

  TVme1742DataBlock*  fVme1742DataBlock;

  TObjArray*          fListOfAlgorithms;
  TObjArray*          fListOfChannels;

  TCalibRunRange*     fCalibRunRange;

  int                 fTimeChannel[20];  // channels used for time measurements
  double              fTimeQ      [20];

  int                 fPhotopeak;

  TStripLineFit*      fStripLineFit;
  int                 fNStripLines;
  TObjArray*          fListOfStripLines;
//-----------------------------------------------------------------------------
//  functions
//-----------------------------------------------------------------------------
public:
  TPet2AnaModule(const char* name="Pet2Ana", const char* title="Pet2Ana");
  ~TPet2AnaModule();
//-----------------------------------------------------------------------------
// accessors
//-----------------------------------------------------------------------------
  TDrsFitAlgNew* GetAlgorithm(int I) {
    return (TDrsFitAlgNew*) fListOfAlgorithms->At(I); 
  }

  TDrsFitAlgNew* GetAlgorithm(const char* Name) {
    return (TDrsFitAlgNew*) fListOfAlgorithms->FindObject(Name); 
  }

  int GetTimeChannel(int I) { return fTimeChannel[I]; }

  TPetClusterFinder* GetClusterFinder() { return fPetReco; }
  TStripLineFit*     GetStripLineFit () { return fStripLineFit; }
//-----------------------------------------------------------------------------
// setters
//-----------------------------------------------------------------------------
  void AddAlgorithm(TObject* Alg) { fListOfAlgorithms->Add(Alg);  }

  void SetTimeChannel(int I, int Channel) { fTimeChannel[I] = Channel; }
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
  void    BookStripLineHistograms(StripLineHist_t* Hist, const char* Folder);
  void    BookHistograms       ();

  int     FillClusterHistograms(ClusterHist_t* Hist, TTofCluster* Cluster);
  int     FillEventHistograms  (EventHist_t*   Hist);
  int     FillStripLineHistograms(StripLineHist_t* Hist, TStripLine* StripLine);
  int     FillHistograms       ();

  int     Debug(int Passed);

  int     DrawPH();

  int     ProcessStripLine(TStripLine* StripLine);

  ClassDef(TPet2AnaModule,0)
};


#endif
