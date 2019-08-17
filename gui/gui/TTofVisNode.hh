#ifndef TTofVisNode_hh
#define TTofVisNode_hh

#include "Gtypes.h"
#include "TObjArray.h"

#include "TGeant/TVisNode.hh"

class TTofDetector;

class TTofVisNode: public TVisNode {
public:
  enum {
    kPickStubs = 0,
    kPickMuons = 1,
    kPickHitss = 2
  };
protected:
//   TObjArray**   fListOfMuons;
//   TObjArray**   fListOfStubs;
//   TObjArray**   fListOfTracks;

//   Color_t         fStubColor[2];	// 0:west, 1:east
//   Color_t         fMuonColor;

  Int_t           fDisplayHits;
  Int_t           fPickMode;
public:
					// ****** constructors and destructor
  TTofVisNode(const char* name = "");
  virtual ~TTofVisNode();
					// ****** accessors

//   TObjArray* GetListOfMuons() { return *fListOfMuons; }
//   TObjArray* GetListOfStubs() { return *fListOfMuons; }

					// ****** modifiers

//   void  SetListOfMuons (TObjArray** List) { fListOfMuons  = List; }
//   void  SetListOfStubs (TObjArray** List) { fListOfStubs  = List; }
//   void  SetListOfTracks(TObjArray** List) { fListOfTracks = List; }

  void  SetPickMode   (Int_t Mode) { fPickMode    = Mode; }
  void  SetDisplayHits(Int_t Mode) { fDisplayHits = Mode; }

  virtual void Paint  (Option_t* option = "");
  virtual void PaintXY(Option_t* option = "");
  virtual void PaintRZ(Option_t* option = "");

  virtual Int_t DistancetoPrimitive  (Int_t px, Int_t py);
  virtual Int_t DistancetoPrimitiveXY(Int_t px, Int_t py);
  virtual Int_t DistancetoPrimitiveRZ(Int_t px, Int_t py);

  ClassDef(TTofVisNode,0)
};


#endif
