//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
#ifndef drs_obj_TTofChannelID_hh
#define drs_obj_TTofChannelID_hh

#include "TObject.h"

class TTofChannelID {
public:
  int         fChannelID;		// channel ID
  int         fNumber;			// channel number (cable)
//-----------------------------------------------------------------------------
public:
  TTofChannelID();
  TTofChannelID(int Number, int Module, int IX, int IY);
  ~TTofChannelID();
//-----------------------------------------------------------------------------
// accessors
//-----------------------------------------------------------------------------
  int           GetChannelID() const { return fChannelID; }
  int           GetModule   () const { return (fChannelID >> 16) & 0xff; }
  int           GetIX       () const { return (fChannelID >>  8) & 0xff; }
  int           GetIY       () const { return (fChannelID >>  0) & 0xff; }
  int           GetNumber   () const { return fNumber; }

  void          Set (int Number, int Module, int IX, int IY) {
    fChannelID = (Module<<16) + (IX<<8) + IY;
    fNumber    = Number;
  }

  virtual void  Print(const char* Opt = "") const ;

  ClassDef (TTofChannelID,1)
};

#endif
