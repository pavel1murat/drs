// -*- Mode: C++  -*-
//------------------------------------------------------------------------------
// *0002 Jan 09 2001 P.Murat: describe for TGeant
//------------------------------------------------------------------------------
#ifndef TTofModule_hh
#define TTofModule_hh

#include "TGeant/TSubdetector.hh"
#include "TGeant/TG3Box.hh"

class TTofData;

class TTofModule: public TSubdetector {
//-----------------------------------------------------------------------------
// there are 4 CMP sides: north, top, south and bottom (0,1,2,3)
// side is not really a subdetector, but an element of organizational hierarchy
//-----------------------------------------------------------------------------
public:
  int           fNumber;		    // ! module number
  Int_t         fNChannels;		    // ! # of channels (crystals) 
  Int_t         fNTriggerChannels;	    // ! # of trigger channels

  Int_t         fNx;			    // ! # of crystals in X
  Int_t         fNy;			    // ! # of crystals in Y

  Int_t         fNHitCrystals;		    // ! number of hit crystals

  TTofData*     fData[4][4];		    // ! cached, not owned
  TTofData*     fListOfChannels[16];        // !
  TTofData*     fListOfOrderedChannels[16]; // !

  TObjArray*    fListOfClusters;	    // ! reconstructed

  TTofData*     fListOfTriggerChannels[2];  // !
//-----------------------------------------------------------------------------
// functions 
//-----------------------------------------------------------------------------
public:
					// ****** constructors and destructor
  TTofModule(); 
  ~TTofModule();
      					// ****** accesors

  int          Number             () { return fNumber;   }
  int          GetNChannels       () { return fNChannels; }
  int          GetNTriggerChannels() { return fNTriggerChannels; }
  TG3Box*      ContainmentBox     () { return (TG3Box*) fContainment; }
                  
  TTofData*    GetData(int Ix, int Iy) { return fData[Ix][Iy];}

  TTofData*    GetChannel       (int I) { return fListOfChannels       [I]; }
  TTofData*    GetOrderedChannel(int I) { return fListOfOrderedChannels[I]; }

  TTofData*    GetTriggerChannel(int I) { return fListOfTriggerChannels[I]; }

  TTofData**   GetListOfOrderedChannels() { return fListOfOrderedChannels; }

  TObjArray*   GetListOfClusters() { return fListOfClusters; }
//-----------------------------------------------------------------------------
// modifiers
//-----------------------------------------------------------------------------
  void         SetNumber(int i) { fNumber = i; }
					// order is geometrical, I = 4*IX+IY
  void         SetChannel(int I, TTofData* Channel) { 
    fListOfChannels[I] = Channel; 
  }

  void         SetTriggerChannel(int I, TTofData* Channel) { 
    fListOfTriggerChannels[I] = Channel; 
  }

  void         SetOrderedChannel(int I, TTofData* Channel) { 
    fListOfOrderedChannels[I] = Channel; 
  }

  void         SetData(int Ix, int Iy, TTofData* Data) { 
    fData[Ix][Iy] = Data; 
  }
//-----------------------------------------------------------------------------
// overloaded methods of TDetectorElement
//-----------------------------------------------------------------------------
  int          DeclareMaterials    (TGeometryManager* gm);
  int          DeclareTrackingMedia(TGeometryManager* gm);
  int          DeclareGeometry(TGeometryManager* gm, TSubdetector* mother);

  virtual void  PaintXY(Option_t* option = "");
  virtual void  PaintRZ(Option_t* option = "");

  virtual Int_t DistancetoPrimitiveXY(Int_t px, Int_t py);
  virtual Int_t DistancetoPrimitiveRZ(Int_t px, Int_t py);
//-----------------------------------------------------------------------------
// overloaded methods of TObject
//-----------------------------------------------------------------------------
  void         Clear(Option_t* option = "");

  ClassDef(TTofModule,1)
};

#endif

