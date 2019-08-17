//

#include "TCanvas.h"

#include "drs/obj/TTofData.hh"
#include "drs/obj/TDrsFitResults.hh"
#include "drs/ana/TDrsFitAlgNew.hh"


ClassImp (TDrsFitAlgNew)


TTofData*       TDrsFitAlgNew::fgFitData    = 0;
TDrsFitResults* TDrsFitAlgNew::fgFitResults = 0;

//-----------------------------------------------------------------------------
TDrsFitAlgNew::TDrsFitAlgNew(const char* Name, const char* Title, int NChannels) 
  : TNamed(Name, Title) 
{

  TDrsChannel *ch;
  char    name[200];

  TDrsFitResults *res;
 
  fCachedData = 0;
  fFitMode    = 1;
					// sparse array
  fNChannels = NChannels; 
  fNTrigChannels = fNChannels/8;     // DRS4 has one trigger channel per 8 signal ones

  fDisplayV             = new TH1F* [fNChannels];
  fListOfFitResults     = new TObjArray(fNChannels);
  fListOfTrigFitResults = new TObjArray(fNTrigChannels);

  int n = 0;

//   for (int i=0; i<fNChannels; i++) {
//     ch = (TDrsChannel*) fCachedListOfChannels->UncheckedAt(i);
//     if (ch) {
//       res = new TDrsFitResults(ch);
//       fListOfFitResults->AddAt(res,i);

//       fUsedChannelNumber[n] = i;
//       n++;
//       sprintf(name,"%s_display_%03i",Name,i);
//       fDisplayV[i] = new TH1F(name,name,kNCells,0,kNCells);
//     }
//   }

  fDisplayResults = 0;
					// create a folder
  fFolder = new TFolder(Name,Title);

  fMinQ1 = 420.;
  fMaxQ1 = 600.;

  fCanvas = 0;
}

//-----------------------------------------------------------------------------
TDrsFitAlgNew::~TDrsFitAlgNew() {
  delete    fListOfFitResults;
  delete [] fDisplayV;
}

//-----------------------------------------------------------------------------
int TDrsFitAlgNew::Fit(const TTofData* Data) {
  return 0;
}


//_____________________________________________________________________________
void     TDrsFitAlgNew::AddHistogram(TObject* hist, const char* FolderName) {
  TFolder* fol = (TFolder*) fFolder->FindObject(FolderName);
  //  fol->Add(hist); 
  fFolder->Add(hist); 
}



//_____________________________________________________________________________
void TDrsFitAlgNew::HBook1F(TH1F*& Hist, const char* Name, const char* Title,
			 Int_t Nx, Double_t XMin, Double_t XMax,
			 const char* FolderName)
{
  // book 2D histogram, add it to the module's list of histograms and 
  // return pointer to it to the user

  Hist = new TH1F(Name,Title,Nx,XMin,XMax);
  AddHistogram(Hist,FolderName);
}

//_____________________________________________________________________________
void TDrsFitAlgNew::HBook2F(TH2F*& Hist, const char* Name, const char* Title,
			 Int_t Nx, Double_t XMin, Double_t XMax,
			 Int_t Ny, Double_t YMin, Double_t YMax,
			 const char* FolderName)
{
  // book 2D histogram, add it to the module's list of histograms and 
  // return pointer to it to the user

  Hist = new TH2F(Name,Title,Nx,XMin,XMax,Ny,YMin,YMax);
  AddHistogram(Hist,FolderName);
}

//_____________________________________________________________________________
void TDrsFitAlgNew::HProf(TProfile*& Hist, const char* Name, const char* Title,
		       Int_t Nx, Double_t XMin, Double_t XMax,
		       Double_t YMin, Double_t YMax,
		       const char* FolderName)
{
  // book 2D histogram, add it to the module's list of histograms and 
  // return pointer to it to the user

  Hist = new TProfile(Name,Title,Nx,XMin,XMax,YMin,YMax);
  AddHistogram(Hist,FolderName);
}




//_____________________________________________________________________________
void TDrsFitAlgNew::DeleteHistograms(TFolder* Folder) {
  // internal method...

  if (Folder == NULL) Folder = fFolder;

  TObject  *o1;

  TIter    it1(Folder->GetListOfFolders());

  while ((o1 = it1.Next())) {
    if (o1->InheritsFrom("TFolder")) {
      DeleteHistograms((TFolder*) o1);
    }
    else if (o1->InheritsFrom("TH1")) {
      Folder->Remove(o1);
      delete o1;
    }
  }
}


//-----------------------------------------------------------------------------
int TDrsFitAlgNew::Display() {
  Error("Display","Not implemented yet");
  return 0;
}
