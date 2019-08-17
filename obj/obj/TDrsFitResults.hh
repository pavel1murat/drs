#ifndef TDrsFitResults_hh
#define TDrsFitResults_hh

//#include "TDrsFitAlg.hh"
#include "TGraphErrors.h"

class TDrsChannel;
class TTofData;

class TDrsFitResults: public TObject {
//-----------------------------------------------------------------------------
// parameter fit in a given channel
//-----------------------------------------------------------------------------
public:
  TDrsChannel*  fChannel;		// fit channel
  TTofData*     fTofData;
  double        fPar[10];
  double        fSig[10];
  double        fT0Fit;			// result of the T0 fit in this channel
  double        fChi2;			// chi^2 per DOF
  int           fChNumber;	        // channel number
  int           fMinBin;		// bins used in the fit
  int           fMaxBin;
  double        fBaseOffset;
  double        fBaseChi2;
  TGraphErrors* fUsedPointsGraph;	// points used in the fit, for display
  TF1*          fFun;
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
  TDrsFitResults(int          Channelumber = 1);
  TDrsFitResults(TDrsChannel* Channel);
  TDrsFitResults(TTofData*    Data   );

  int    GetChNumber  () { return fChNumber; }
  double GetT0Fit     () { return fT0Fit;    }
  int    GetNFitPoints() { return fMaxBin-fMinBin+1; }

  TTofData* GetTofData() { return fTofData; }

  ~TDrsFitResults();

  ClassDef(TDrsFitResults,0)
};

#endif
