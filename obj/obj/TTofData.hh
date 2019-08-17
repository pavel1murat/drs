//-----------------------------------------------------------------------------
// single channel data , use the same for trigger channels? 
//-----------------------------------------------------------------------------
#ifndef drs_obj_TTofData_hh
#define drs_obj_TTofData_hh

#include "TObject.h"
#include "TH1.h"

#include "TTofChannelID.hh"

class TTofData : public TObject {
public:

  enum { kNCells = 1024 };

  TTofChannelID  fChannelID;		// channel ID
  float          fV0[kNCells];
  float          fV1[kNCells];		// pedestal-corrected data, positive pulse
  float          fV1Max   ;		// reconstructed
  int            fI1Max   ;		// VMax cell index
  float          fT0      ;             // approximate T0 value, by TPetClusterFinder
  float          fQ       ;		// reconstructed
  float          fQ1      ;		// reconstructed, calibrated
  float          fPedestal;		// reconstructed
  float          fChi2Ped;              // ped fit chi2  
  float          fMax1V1  ;		// V1 in the first maximum
  int            fMax1Cell;		// cell, corresp. to the 1st maximum
  float          fMinThreshold;         // 
  float          fMinQ;			// charge threshold
  TH1F*          fHistShape;            // normalized shape histogram
  TH1F*          fHistV0;               // raw data
  TH1F*          fHistV1;               // corrected data
  int            fUsed;                 // 0 if not used
  int            fReadoutGroup;         // readout group (or DRS4 chip) number
  int            fTriggerChannel;       // 0 for signal pulses, 1 for trigger pulses
//-----------------------------------------------------------------------------
public:
  TTofData();
  virtual ~TTofData();
//-----------------------------------------------------------------------------
// accessors
//-----------------------------------------------------------------------------
  const TTofChannelID*  GetChannelID()  const { return (const TTofChannelID*) &fChannelID; }

  int  GetModule() { return fChannelID.GetModule(); }
  int  GetChannelNumber() { return fChannelID.GetNumber(); }
  int  GetIX    () { return fChannelID.GetIX();     }
  int  GetIY    () { return fChannelID.GetIY();     }
  int  GetUsed  () { return fUsed ; }

  float       GetV0(int Cell) const { return fV0[Cell];  }
  float       GetV1(int Cell) const { return fV1[Cell];  }

  const float* GetV0() const   { return (const float*) fV0; }
  const float* GetV1() const   { return (const float*) fV1; }

  float       GetPedestal() const { return fPedestal;  }
  float       GetChi2Ped () const { return fChi2Ped;   }
  float       GetQ       () const { return fQ;         }
  float       GetQ1      () const { return fQ1;        }
  float       GetT0      () const { return fT0;        }
  float       GetV1Max   () const { return fV1Max;     }
  int         GetI1Max   () const { return fI1Max;     }
  int         GetNCells  () const { return kNCells;    }
  float       GetMax1V1  () const { return fMax1V1;    }
  int         GetMax1Cell() const { return fMax1Cell;  }
  float       GetMinQ    () const { return fMinQ;      }
  TH1F*       GetHistShape() { return fHistShape; }
  TH1F*       GetHistV0   () { return fHistV0; }
  TH1F*       GetHistV1   () { return fHistV1; }

  int         GetReadoutGroup  () const { return fReadoutGroup;   }
  int         GetTriggerChannel() const { return fTriggerChannel; }
//-----------------------------------------------------------------------------
// setters
//-----------------------------------------------------------------------------
  void        SetV0(int Cell, float V) { fV0[Cell] = V; }
  void        SetV1(int Cell, float V) { fV1[Cell] = V; }

  void        SetReadoutGroup  (int Group) { fReadoutGroup   = Group; }
  void        SetTriggerChannel(int Flag ) { fTriggerChannel = Flag;  }

  void        SetChannelID(int Number, int Module, int IX, int IY) { 
    fChannelID.Set(Number,Module,IX,IY); 
  }

  void        SetV1Max(float V)  { fV1Max = V;  }
  void        SetI1Max(int   I)  { fI1Max = I;  }
  void        SetQ    (float Q)  { fQ     = Q;  }
  void        SetQ1   (float Q1) { fQ1    = Q1; }
  void        SetT0   (float T0) { fT0 = T0; }
  void        SetPedestal(float P) { fPedestal = P; }
  void        SetChi2Ped (float Chi2) { fChi2Ped = Chi2; }
  
  void        SetMax1(int Cell, float V) {
    fMax1Cell = Cell;
    fMax1V1   = V;
  }

  void        SetMinQ(float Q ) { fMinQ = Q; }
  void        SetUsed(int Used) { fUsed = Used; }

  virtual void  Print(const char* Opt = "") const ;

  ClassDef (TTofData,1)
};

#endif

