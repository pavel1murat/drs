//
#ifndef TStripLine_hh
#define TStripLine_hh

#include "TObject.h"
#include "TF1.h"
#include "TH2.h"
#include "TFolder.h"
#include "TGraphErrors.h"

#include "TTofData.hh"
#include "TDrsFitResults.hh"
#include "murat/plot/smooth.hh"

class TStripLine : public TObject {
public:
  TTofData*  fTofData[2];

  smooth::Par_t*    fSmoothPar;

  TDrsFitResults*    fFitResults[2];
  TDrsFitResults*    fFitStrip;

  TDrsChannel*       fChannel[2]; // transition - to make it useful in both cases
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
  TStripLine(int i=0);
  ~TStripLine();

  TTofData* GetTofData(int Channel) { return fTofData[Channel]; }

  ClassDef(TStripLine,0)
};

#endif
