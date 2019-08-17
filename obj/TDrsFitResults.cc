//

#include "TCanvas.h"

#include "drs/obj/TDrsChannel.hh"
#include "drs/obj/TDrsFitResults.hh"

ClassImp(TDrsFitResults)

//-----------------------------------------------------------------------------
TDrsFitResults::TDrsFitResults(TDrsChannel* Channel) : TObject() {
  fChNumber        = Channel->GetNumber();
  fChannel         = Channel;
  fTofData         = 0;
  fUsedPointsGraph = 0;
  fFun             = 0;
}

//-----------------------------------------------------------------------------
TDrsFitResults::TDrsFitResults(TTofData* Data) : TObject() {
  fChNumber        = -1;
  fChannel         = 0;
  fTofData         = Data;
  fUsedPointsGraph = 0;
  fFun             = 0;
}

//-----------------------------------------------------------------------------
TDrsFitResults::TDrsFitResults(int ChNumber) : TObject() {
  fChNumber        = ChNumber;
  fChannel         = 0;
  fTofData         = 0;
  fUsedPointsGraph = 0;
  fFun             = 0;
}

//-----------------------------------------------------------------------------
TDrsFitResults::~TDrsFitResults() {
  if (fUsedPointsGraph) delete fUsedPointsGraph ;
  if (fFun)             delete fFun;
}
