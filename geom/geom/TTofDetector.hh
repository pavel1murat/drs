// -*- Mode: C++  -*-
//------------------------------------------------------------------------------
// Jan 08 2001 P.Murat: CMP description for TGeant (based on work done by
//                      G.Velev)
//------------------------------------------------------------------------------
#ifndef TTofDetector_hh
#define TTofDetector_hh

#include "TGeant/TSubdetector.hh"
#include "TTofModule.hh"
#include "drs/obj/TTofData.hh"

class TG3Box;
class TTrackingMedium;
class TVme1742DataBlock;
class TTofReadoutMap;
class TCalibManager;
class TTofCalibData;

class TTofDetector : public TSubdetector {
public:
  //  enum { kNSides = 4 };
//------------------------------------------------------------------------------
//  data members
//------------------------------------------------------------------------------
protected:
					// ****** geometry

  TTofModule*        fModule;		// TOF modules
  int                fNModules;
  int                fNChannels;	// total number of readout channels
  int                fNTriggerChannels; // number of trigger channels
  //  int                fNReadoutGroups;

					// 2 different lengths: 
					// 0=vertical, 1=horizontal
  TG3Shape*          fChamberVolume [2];
  TG3Shape*          fGasVolume     [2];
					// CSP counters are of 2 different 
					// widths 

  TG3Shape*          fScintillationCounterVolume[2];

					// CMP tracking media - each subdetector
					// has its own media described
  TTrackingMedium*   fTmedGas;
					// ! ****** event data
  TVme1742DataBlock* fVmeData;          // !

  TTofCalibData*     fCalibData;        // !
  TTofData*          fData;	        // !
  TTofData*          fTriggerData;      // ! 
//------------------------------------------------------------------------------
//  function members
//------------------------------------------------------------------------------

public:
                                        // ****** constructors and destructor
  TTofDetector();
  ~TTofDetector();
                                        // event initialization routine

  int            InitEvent(TObject* event, int read_available_input = 0);

					// initialize detector starting from 
					// STNTUPLE

  int            InitEvent     (TVme1742DataBlock* Data);
  int            InitTofModules(TVme1742DataBlock* Data);

  int            InitCalibrations(int RunNumber, TCalibManager* Manager);

                                        // initalize geometry and declare it to
					// the geometry manager

  int            DeclareMaterials    (TGeometryManager* gm);
  int            DeclareTrackingMedia(TGeometryManager* gm);

  int            DeclareGeometry     (TGeometryManager* gm, 
				      TSubdetector* mother);

					// ****** data access methods ******

  TTrackingMedium* TmedGas()  { return fTmedGas; }

  TG3Shape*       ScintillationCounterVolume(int i) { 
    return fScintillationCounterVolume[i]; 
  }

  int             GetNModules () { return fNModules;  }
  int             GetNChannels() { return fNChannels; }
  int             GetNTriggerChannels() { return fNTriggerChannels; }
  TTofCalibData*  GetCalibData() { return fCalibData; }

  TTofData*       GetData       (int Channel) { return fData + Channel; }
  TTofData*       GetTriggerData(int Channel) { return fTriggerData + Channel; }
//-----------------------------------------------------------------------------
//  CMP data
//-----------------------------------------------------------------------------
  TVme1742DataBlock* GetVmeData() { return fVmeData; }

  TTofModule*        GetModule(int I) { return fModule+I; }

			     		// ****** other methods
//-----------------------------------------------------------------------------
// MC hit generation routine called from GEANT's gustep
//-----------------------------------------------------------------------------
  int            GenerateHit();
  virtual int    GenerateHit(float* p1, float* p2, int mc_number);

						// ****** drawing

  virtual void  PaintXY(Option_t* option = "");
  virtual void  PaintRZ(Option_t* option = "");

  virtual Int_t DistancetoPrimitiveXY(Int_t px, Int_t py);
  virtual Int_t DistancetoPrimitiveRZ(Int_t px, Int_t py);
//-----------------------------------------------------------------------------
// overloaded functions of TObject
//-----------------------------------------------------------------------------
  void           Clear(Option_t* opt="");
  void           Print(Option_t* opt="") const ;

  ClassDef(TTofDetector,0)
};

#endif





