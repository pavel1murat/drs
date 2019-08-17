///////////////////////////////////////////////////////////////////////////////
// plots for the technical note on DOI measurement
// -------------------------
// directory with the histogram files: by default: $WORK_DIR/results
// can be redefined with "drs.HistDir" in .rootrc
// 
// figures:
// ---------
// Fig.   1:  
// Fig.   2:  
// Fig.   3:  
// Fig.   3:  
////////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include "TArrow.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TH1.h"
#include "TEnv.h"
#include "TH2.h"
#include "TInterpreter.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TMath.h"
#include "TPad.h"
#include "TEnv.h"
#include "TProfile.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TPaveLabel.h"
#include "TSystem.h"
#include "TString.h"
#include "TText.h"
#include "Stntuple/val/stntuple_val_functions.hh"
#include "murat/plot/TPlotCdfNote.hh"


class TNote002: public TPlotCdfNote {
public:
//-----------------------------------------------------------------------------
// data members
// 1. files:
//-----------------------------------------------------------------------------
  TString   fModule;			// "Pet2Ana"
  TString   fFilename;
//-----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------
public:

  TNote002 (int PlotMode=kNoteMode, int BlessingMode=0);
  ~TNote002();

  void  remake_plots();
//-----------------------------------------------------------------------------
// overloaded methods of TPlotCdfNote
//-----------------------------------------------------------------------------
  virtual void plot        (int Figure, const char* CanvasName = 0);

  virtual const char*  GetFiguresDir();

  ClassDef(TNote002,0)
  
};

