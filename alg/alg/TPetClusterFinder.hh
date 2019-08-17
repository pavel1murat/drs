#ifndef TPetClusterFinder_hh
#define TPetClusterFinder_hh

#include "TNamed.h"
#include "drs/geom/TTofModule.hh"

class TMinuit;
class TTofData;
class TTofCalibData;
class TClonesArray;
class TTofDetector;

class TPetClusterFinder: public TNamed {
public:

  const double kLargeChi2;
					// clustering parameters
  struct Parameters_t {
    Int_t      fView;		// HW conv: 0: strips, 1: wires !
    Int_t      fFirst;
    Int_t      fLast;
    Float_t    fSeedThreshold;
    Float_t    fShoulderThreshold;
    Float_t    fMinDev;		// fraction. used in merged cluster finding
    Float_t    fLocThr;		// local thr - used in merged cluster finding
    Float_t    fAdcPerGeV;
    Float_t    fPitch;
    Double_t*  fX;		     // coordinates of the strips
    Float_t    fMin1StripClusterE;   // min E of 1-strip cluster
  };

//-----------------------------------------------------------------------------
// data members
//-----------------------------------------------------------------------------
  int                  fRunNumber;
  int                  fEventNumber;
  int                  fVerbose;

  TMinuit*             fFitter;
					// geometry info to be used [not owned]
  TTofDetector*        fTofDetector;
  TTofModule*          fModule;		// currently processed chamber

  TTofCalibData*       fCalibData;	// clib data for a given channel

  TObjArray*     fListOfClusters[2];
					// cells for pass 1 pedestal determination
  int                  fMinCell;
  int                  fMaxCell;

  float                fMinThreshold;   // over the pedestal

  float                fQScale ;        // scale of the integrated charge, default=50
//-----------------------------------------------------------------------------
// methods
//-----------------------------------------------------------------------------
public:
					// ****** constructors and destructor
  TPetClusterFinder();
  ~TPetClusterFinder();
//-----------------------------------------------------------------------------
// accessors
//-----------------------------------------------------------------------------
  int    Verbose() { return fVerbose; }
//-----------------------------------------------------------------------------
// modifiers
//-----------------------------------------------------------------------------
  void   SetTofDetector(TTofDetector* Tof) { fTofDetector = Tof; }

  void   SetRunEventNumber(int RunNumber, int EventNumber) {
    fRunNumber   = RunNumber;
    fEventNumber = EventNumber;
  }

  void   SetVerbose(int Flag) { fVerbose = Flag; }

  void   SetMinCell(int MinCell) { fMinCell = MinCell; }
  void   SetMaxCell(int MaxCell) { fMaxCell = MaxCell; }
  void   SetQScale (float QScale) { fQScale = QScale; }
//-----------------------------------------------------------------------------
// other methods
//-----------------------------------------------------------------------------
  Int_t SubtractPedestals(TTofData* Data);

  Int_t ProcessModule(TTofDetector*  Tof, int Module);

  Int_t DoClustering  (Int_t*        Data, 
		       Parameters_t* Par, 
		       TClonesArray* ListOfClusters);

  ClassDef(TPetClusterFinder,0)
};

#endif
