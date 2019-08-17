//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
#ifndef drs_obj_TTofCluster_hh
#define drs_obj_TTofCluster_hh

#include "TObject.h"

class TTofCluster : public TObject {
					// bits 0-7 are used by the system,
					// use bit8 as "used" flag
					// or ROOT is using bits 24-31???
  enum  {kUsedBit = BIT(8) };

public:
				// so far strips:0, wire: 1

protected: 
  Int_t      fNCrystals;
  Float_t    fPH;
  Float_t    fCharge;
  Int_t      fModule;
  Int_t      fIX;
  Int_t      fIY;
//-----------------------------------------------------------------------------
public:
  TTofCluster();
  virtual ~TTofCluster();

//   Int_t    Init(Int_t   Side  , Int_t Wedge, Int_t View   , 
// 		Int_t   First , Int_t N    , Int_t NMerged, Int_t Seed,
// 		Float_t Energy, float Coord, float Sigma  );
//-----------------------------------------------------------------------------
// accessors
//-----------------------------------------------------------------------------

  int   GetNCrystals() { return fNCrystals; }
  float GetPH       () { return fPH;        }
  float GetCharge   () { return fCharge;    }
  int   GetModule   () { return fModule;    }

  virtual void  Print(const char* Opt = "") const ;

  ClassDef (TTofCluster,1)
};

#endif

