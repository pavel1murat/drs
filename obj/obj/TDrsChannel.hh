//
#ifndef TDrsChannel_hh
#define TDrsChannel_hh

#include "TObject.h"
#include "TF1.h"
#include "TH2.h"
#include "TFolder.h"
#include "TGraphErrors.h"

class TDrsChannel : public TObject {
public:

  enum { kNCells = 1024 };

  struct Hist_t {
    TH1F*    fV;
    TH1F*    fPH;
    TH1F*    fQ;
    TH1F*    fQ1;
    TH2F*    fPHVsQ; 
    TH1F*    fBaseOffset;
    TH1F*    fFitBase;
    TH1F*    fFitChi2;
    TH2F*    fFitBaseVsEvent;
    TH1F*    fNoise;
    TH1F*    fShape;
    TH2F*    fVVsCell[5];
    TH2F*    fShapeVsCell[5];
    TH2F*    fDvVsCell;
    TH2F*    fDv2VsCell;
  } fHist; 

  int    fNumber;
  int    fCachedEventNumber;

  int    fNCells;			// ! = kNCells

  float  fV         [kNCells];		// ! corrected readings
  float  fWt        [kNCells];		// ! weight of the i-th cell
  float  fCellOffset[kNCells];		// ! for calibration
  float  fCellSigma [kNCells];		// ! for calibration

  double fVMax    ;		        // ! pulse height, negative 
  int    fVMaxCell;		        // ! cell, corresponding to VMin
  double fQ       ;
  double fQ1      ;			// ! normalized to 511 KeV
  double fMinThreshold;	                // ! start looking for a maximum above that
  double fBaseOffset;			// ! base offset 
  double fQPeak;

  int           fFirstChannel;
  double        fPulseWidth;

  double        fMinQ     ;		// Q thresholds
  double        fEvenShift;		// 
  TFolder*      fFolder;
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
  TDrsChannel(int i=0);
  ~TDrsChannel();

  double GetQPeak() { return fQPeak; }

  int  GetNumber() const { return fNumber; }
  int  GetNCells() const { return fNCells; }
  TFolder* GetFolder() { return fFolder; }

  double  GetQ () { return fQ ; }
  double  GetQ1() { return fQ1; }
  double  GetBaseOffset() { return fBaseOffset; }
  double  GetEvenShift () { return fEvenShift ; }
  Hist_t* GetHist      () { return &  fHist;    }

  double  GetShape     (int I) { return fV[I]/(fVMax+1.e-12); }

  void    SetBaseOffset  (double Offset   ) { fBaseOffset   = Offset   ; }
  void    SetEvenShift   (double Shift    ) { fEvenShift    = Shift    ; }
  void    SetMinThreshold(double Threshold) { fMinThreshold = Threshold; }
  void    SetMinQ        (double Q        ) { fMinQ         = Q ;        }
  void    SetQPeak       (double Q        ) { fQPeak        = Q ; }
  void    SetFirstChannel(int    Channel  ) { fFirstChannel = Channel; }
  void    SetPulseWidth  (double Width    ) { fPulseWidth   = Width; }

  int     BookHistograms();
  int     FillHistograms();
  void    AddHistogram(TObject* hist, const char* FolderName = "Hist");

  ClassDef(TDrsChannel,0)
};

#endif
