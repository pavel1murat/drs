//

#include "drs/obj/TDrsChannel.hh"
#include "drs/base/TDrsUtils.hh"

ClassImp(TDrsChannel)

//-----------------------------------------------------------------------------
TDrsChannel::TDrsChannel(int Number) : TObject() {
  char  name[100];

  fNumber          = Number;
  fNCells          = 1024;
  fQPeak           = 511.;
					// zero cell offsets
  for (int i=0; i<fNCells; i++) {
    fCellOffset[i] = 0;
  }

  fBaseOffset   = 0;
  fEvenShift    = 0.;
  fMinThreshold = 20.;
  fMinQ         = 420.;
  fFirstChannel = 0;
  fPulseWidth   = 200;

  sprintf(name,"channel_%03i",fNumber);
  fFolder = new TFolder(name,name);
  //  fFolder->AddFolder("Hist","List of histograms");
}

//-----------------------------------------------------------------------------
TDrsChannel::~TDrsChannel() {
}

//_____________________________________________________________________________
void     TDrsChannel::AddHistogram(TObject* hist, const char* FolderName) {
  TFolder* fol = (TFolder*) fFolder->FindObject(FolderName);
  //  fol->Add(hist); 
  fFolder->Add(hist); 
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int TDrsChannel::BookHistograms() {

  char  name[100];

  int i = fNumber;
  
  Hist_t* Hist = &fHist;

  sprintf(name,"base_offset");
  Hist->fBaseOffset  = new TH1F(name,name,500,-25,25);
  AddHistogram(Hist->fBaseOffset);

  Hist->fFitBase  = new TH1F("fit_base","fit_base",250,-10,10);
  AddHistogram(Hist->fFitBase);

  Hist->fFitBaseVsEvent  = new TH2F("fit_base_vs_ev","fit_base vs event",200,0,20000,400,-10,10);
  AddHistogram(Hist->fFitBaseVsEvent);

  Hist->fFitChi2  = new TH1F("fit_chi2","fit_chi2",100,0,10);
  AddHistogram(Hist->fFitChi2);

  //  sprintf(name,"q_%02i",i);
  sprintf(name,"q");
  Hist->fQ  = new TH1F(name,name,1000,0,2000);
  AddHistogram(Hist->fQ);

  Hist->fQ1  = new TH1F("q1","q1",200,0,1000);
  AddHistogram(Hist->fQ1);

  //  sprintf(name,"ph_%02i",i);
  sprintf(name,"ph");
  Hist->fPH = new TH1F(name,Form("pulse height, channel %2i",i),250,0,500);
  AddHistogram(Hist->fPH);
      

  //  sprintf(name,"v_%02i",i);
  sprintf(name,"v");
  Hist->fV      = new TH1F(name,name,1024,0,1024);
  AddHistogram(Hist->fV);

  //  sprintf(name,"ph_vs_q_%02i",i);
  sprintf(name,"ph_vs_q");
  Hist->fPHVsQ  = new TH2F(name,name,250,0,500,200,0,1000);
  AddHistogram(Hist->fPHVsQ);

  // histograms for the pedestal runs

  //  sprintf(name,"v_vs_cell_%02i",i);
  sprintf(name,"v_vs_cell_0");
  Hist->fVVsCell[0]   = new TH2F(name,name,1024,0,1024,600,-20,580);
  AddHistogram(Hist->fVVsCell[0]);

  sprintf(name,"v_vs_cell_1");
  Hist->fVVsCell[1]   = new TH2F(name,name,1024,0,1024,600,-20,580);
  AddHistogram(Hist->fVVsCell[1]);

  // calibration histograms

  //  sprintf(name,"dv_vs_cell_%02i",i);
  sprintf(name,"dv_vs_cell");
  Hist->fDvVsCell = new TH2F(name,name,1024,0,1024,200,-10,10);
  AddHistogram(Hist->fDvVsCell);

  // distance from the fit ...
  
  //  sprintf(name,"dv2_vs_cell_%02i",i);
  sprintf(name,"dv2_vs_cell");
  Hist->fDv2VsCell = new TH2F(name,name,1024,0,1024,200,-10,10);
  AddHistogram(Hist->fDv2VsCell);

  // shapes are [supposed to be] normalized to 100

  //  sprintf(name,"shape_%02i",i);
  sprintf(name,"shape");
  Hist->fShape    = new TH1F(name,name,1024,0,1024);
  AddHistogram(Hist->fShape);

  sprintf(name,"shape_vs_cell_0");
  Hist->fShapeVsCell[0]   = new TH2F(name,name,1024,0,1024,600,-0.1,1.1);
  AddHistogram(Hist->fShapeVsCell[0]);

  sprintf(name,"shape_vs_cell_1");
  Hist->fShapeVsCell[1]   = new TH2F(name,name,1024,0,1024,600,-0.1,1.1);
  AddHistogram(Hist->fShapeVsCell[1]);

  return 0;
}


//-----------------------------------------------------------------------------
int TDrsChannel::FillHistograms() {
					// 80 channels - before the leading edge

  for (int i=0; i<100; i+=1) {
    fHist.fBaseOffset->Fill(fV[i]);
  }

  fHist.fPH->Fill(fVMax);
  fHist.fQ->Fill(fQ);
  fHist.fQ1->Fill(fQ1);

  fHist.fPHVsQ->Fill(fQ,fVMax);
  
  for (int i=0; i<=kNCells; i++) {
    fHist.fVVsCell[0]->Fill(i,fV[i]);
    if (fQ > fMinQ) {
      fHist.fVVsCell[1]->Fill(i,fV[i]);
    }
  }

  double mean, chi2;
//----------------------------------------------------------------------------
// offsets defined in the config script are already subtracted
// the histogram means need to be added to the used offsets:
// offset_new = offset_old + mean
// use 100 points
//-----------------------------------------------------------------------------
  TDrsUtils::FitPol0(fV,0,100,&mean,&chi2);
  fHist.fFitBase->Fill(mean);
  fHist.fFitChi2->Fill(chi2);
  fHist.fFitBaseVsEvent->Fill(fCachedEventNumber,mean);
   
  
  return 0;
}
