//



#include "drs/obj/TDrsGlobals.hh"



ClassImp(TDrsGlobals)


TDrsGlobals*  TDrsGlobals::fgInstance = 0;


//-----------------------------------------------------------------------------
TDrsGlobals::TDrsGlobals() {
}

//-----------------------------------------------------------------------------
TDrsGlobals::~TDrsGlobals() {
}

//------------------------------------------------------------------------------
TDrsGlobals::Cleaner::Cleaner() {
}

//------------------------------------------------------------------------------
TDrsGlobals::Cleaner::~Cleaner() {
  if (TDrsGlobals::fgInstance) {
    delete TDrsGlobals::fgInstance;
    TDrsGlobals::fgInstance = 0;
  }
}

