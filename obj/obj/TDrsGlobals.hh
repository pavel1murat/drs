#ifndef TDrsGlobals_hh
#define TDrsGlobals_hh

#include "TObject.h"
#include "TVme1742DataBlock.hh"

class TDrsGlobals {
public:

  int  fRunNumber;
  int  fEntry;
  int  fEventNumber;

  TVme1742DataBlock*   fVme1742DataBlock;

  static TDrsGlobals*  fgInstance;

  class  Cleaner {
  public: 
    Cleaner ();
    ~Cleaner();
  };
  friend class Cleaner;
//-----------------------------------------------------------------------------
//  functions
//-----------------------------------------------------------------------------
public:

  TDrsGlobals();
  ~TDrsGlobals();

  static TDrsGlobals* Instance() { 
    return fgInstance ? fgInstance : (fgInstance = new TDrsGlobals()) ;
  }
//-----------------------------------------------------------------------------
//  accessors
//-----------------------------------------------------------------------------
  int  GetEntry      () { return fEntry;       }
  int  GetRunNumber  () { return fRunNumber;   }
  int  GetEventNumber() { return fEventNumber; }
  TVme1742DataBlock*  GetVme1742DataBlock() { return fVme1742DataBlock; }
//-----------------------------------------------------------------------------
//  setters
//-----------------------------------------------------------------------------
  void SetEntry      (int Entry) { fEntry       = Entry; }
  void SetRunNumber  (int Run  ) { fRunNumber   = Run;   }
  void SetEventNumber(int Event) { fEventNumber = Event; }
  void SetVme1742DataBlock(TVme1742DataBlock* Block) { fVme1742DataBlock = Block; }

  ClassDef (TDrsGlobals,0)
};



#endif
