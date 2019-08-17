//
#ifndef drs_ana_functions_hh
#define drs_ana_functions_hh

#include "TNamed.h"
#include "TRandom3.h"
#include "TH2.h"
#include "TF1.h"
#include "TProfile.h"
#include "TFolder.h"
#include "TFile.h"

#include "murat/plot/smooth_new.hh"

#include "TGeant/TG3Constants.hh"
#include "TGeant/TG3Box.hh"
#include "TGeant/TGeometryManager.hh"
#include "Stntuple/geom/TTrajectoryPoint.hh"


class mcrystal : public TNamed {
public:

  struct RecoRes_t {
    double fMean;
    double fSigma;
    double fQMin;
    double fQMax;   
    double fQMin2;			// sum of 2
    double fQMax2;
  };

  enum  {kNEventHistSets = 10};
					// event histograms
  struct EventHist_t {
    TH2F*  fYVsX[5];
    TH1F*  fNRef[5];
    TH1F*  fNPhotons;
    TH1F*  fDyRec[3];
    TH1F*  fSigY[3];
    TH1F*  fDxRec[3];
    TH1F*  fSigX[3];
    TH1F*  fDzRec[3];
    TH1F*  fSigZ[3];

    TH2F*  fSigVsZt [2];		// vs true coordinate
    TH2F*  fSigVsZr2[2];		// vs reconstructed coordinate

    TH2F*  fDy0VsZr2;
    TH2F*  fDy1VsZr2;
					// vs nominal histograms...
    TH2F*  fDy2VsYt;
    TH2F*  fDy2VsZt;
    TH2F*  fDz2VsYt;
    TH2F*  fDz2VsZt;

    TH2F*  fDz3VsZt;
    TH2F*  fDz3VsZr3;
    TH2F*  fDzc3VsZr3;

    TH2F*  fDz4VsZt;

    TH2F*  fDy3VsYt;
    TH2F*  fDy3VsZt;
    TH2F*  fDy3VsY3;

    TH2F*  fDy4VsYt;
    TH2F*  fDy4VsZt;

    TH2F*  fDy4VsYr4;
    TH2F*  fDy4VsZc2;

    TH2F*  fDy4cVsYr4;
    TH2F*  fDy4cVsZc2;

    TH2F*  fDy2VsYr2;
    TH2F*  fDz2VsZr2;

    TH2F*  fDx2cVsXr2;
    TH2F*  fDy2cVsYr2;
    TH2F*  fDz2cVsZr2;

    TH2F*  fQmmyVsZ[2];
    TH2F*  fQmmxVsZ[2];

    TH2F*  fQmmy2VsY[2];

    TH2F*  fQmmy2VsZ[2];

    TH2F*  fYpVsXp[2];

  };
					// analysis hist structure
  struct Hist_t {
    EventHist_t*  fEvent[kNEventHistSets];
  };

  Hist_t           fHist;

  TG3Box*          fCrystal;
  TTrackingMedium* fMedium;
					// crystal dimensions (half-size)
  double           fDx;
  double           fDy;
  double           fDz;
					// SiPM dimensions (full size)
  double           fSipmDx;
  double           fSipmDy;

  double           fX0;
  double           fY0;
  double           fZ0;
  int              fNReflections;

  double           fGap;

  double           fYield;

  int              fStop;

  double           fReflEff;
  double           fPhotoEff;

  double           fNPhMean; 
  double           fNPhotons; // per event

  TTrajectoryPoint fPoint;
  TTrajectoryPoint fLastPoint;

  TGeometryManager *fGm;

  TRandom3*         fRn;

  TF1*              fFun;

  long int          fEventNumber;
					// =1: stop at Z=Zmax only 
					// =2: stop at both Z's
  int              fTracingMode; 
					// =0: fixed point
					// =1: poits distributed uniformly within the volume
  int              fPosMode;      

  TFolder*         fAnaFolder;
  TFolder*         fFolder;
  TFolder*         fHistFolder;
					// output of ::Reconstruct()
  RecoRes_t        fRecY[2];
  RecoRes_t        fRecX[2];
					// reconstructed coordinates
  double           fXRec[10];
  double           fSigX[10];
  double           fYRec[10];
  double           fSigY[10];
  double           fZRec[10];
  double           fSigZ[10];
					// corrected reconstructed coordinates
  double           fXCorr[10];
  double           fYCorr[10];
  double           fZCorr[10];
					// non-linearity corrections
  TFile*           fReferenceHistFile;
  smooth_new*      fYCorrFunction;
  smooth_new*      fY4CorrFunction;
  smooth_new*      fZCorrFunction[10];
  smooth_new*      fZVsSigma[2];
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
  mcrystal (double Dx = 1.0, 
	    double Dy = 1.0, 
	    double Dz = 1.0, 
	    double SipmDx = 0.4, 
	    double SipmDy = 0.4,
	    double Gap    = 0.05,
	    double Yield = 300 );


  ~mcrystal(); 
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
  void SetCrystalDimensions(double Dx, double Dy, double Dz) {
    fDx = Dx; fDy = Dy; fDz = Dz; 
  }

  void SetSipmSize(double Dx, double Dy) {
    fSipmDx = Dx; fSipmDy = Dy;
  }
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
  int  BookHistograms  ();
  int  BookEventHistograms(EventHist_t* Hist, const char* Folder);
  void ResetHistograms (TFolder* Folder = 0, const char* Opt = "");

  int  FillProfileHistograms(EventHist_t* Hist);
  int  FillRecoHistograms   (EventHist_t* Hist);
  int  FillHistograms       ();

  static double Fun(double* X, double* P);

  int  Reconstruct(TH1D* Hist, RecoRes_t* Res);

  int  ReconstructEvent();
  int  Run(int Nevents = 1);
  //   int  Trace();
  int  TracePhoton(TTrajectoryPoint* Start, TG3Box* Vol, int Mode);
//-----------------------------------------------------------------------------
// the following helper methods allow to save 1 line per request, 
// which in case of 100's booked histograms is a non-negligible number
//-----------------------------------------------------------------------------

  void  AddHistogram(TObject* hist, const char* FolderName = "Hist");

  void  HBook1F(TH1F*& Hist, const char* Name, const char* Title,
		Int_t Nx, Double_t XMin, Double_t XMax,
		const char* FolderName = "Hist");

  void  HBook2F(TH2F*& Hist, const char* Name, const char* Title,
		Int_t Nx, Double_t XMin, Double_t XMax,
		Int_t Ny, Double_t YMin, Double_t YMax,
		const char* FolderName = "Hist");

  void  HProf (TProfile*& Hist, const char* Name, const char* Title,
	       Int_t Nx, Double_t XMin, Double_t XMax,
	       Double_t YMin, Double_t YMax,
	       const char* FolderName = "Hist");

  int  SaveFolder(TFolder* Folder, TDirectory* Dir);
  void SaveHist(const char* Filename); 

  ClassDef(mcrystal,0)
};

  
#endif
