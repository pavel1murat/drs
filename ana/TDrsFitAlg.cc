//

#include "TCanvas.h"

#include "drs/obj/TDrsChannel.hh"
#include "drs/obj/TDrsFitResults.hh"
#include "drs/ana/TDrsFitAlg.hh"

ClassImp (TDrsFitAlg)

TDrsChannel*    TDrsFitAlg::fgFitChannel = 0;
TDrsFitResults* TDrsFitAlg::fgFitResults = 0;

//-----------------------------------------------------------------------------
TDrsFitAlg::TDrsFitAlg(const char* Name, const char* Title, TObjArray* ListOfChannels) : TNamed(Name, Title) {

  TDrsChannel *ch;
  char    name[200];

  TDrsFitResults *res;
 
  fCachedListOfChannels = ListOfChannels;
					// sparse array
  fNChannels = fCachedListOfChannels->GetLast()+1;

  fUsedChannelNumber = new int[fNChannels];

  fListOfFitResults = new TObjArray(fNChannels);

  int n = 0;

  for (int i=0; i<fNChannels; i++) {
    ch = (TDrsChannel*) fCachedListOfChannels->UncheckedAt(i);
    if (ch) {
      res = new TDrsFitResults(ch);
      fListOfFitResults->AddAt(res,i);

      //      fUsedChannelNumber[n] = i;
      fUsedChannelNumber[n] = ch->GetNumber();
      n++;
      sprintf(name,"%s_display_%03i",Name,i);
      fDisplayV[i] = new TH1F(name,name,kNCells,0,kNCells);
    }
  }

  fDisplayResults = 0;
					// create a folder
  fFolder = new TFolder(Name,Title);

  fCanvas = 0;
  
  //  fFolder->Add(this);
  //  TFolder* hist = fFolder->AddFolder("Hist","ListOfHistograms");
				// here comes the hack (see above)
  // x = (Folder_t*) fFolder;
  // x->SetName(name);
  // x->SetTitle(title);
  // x->fFolders = new TList();
}

//-----------------------------------------------------------------------------
TDrsFitAlg::~TDrsFitAlg() {
  delete fUsedChannelNumber;
  delete fListOfFitResults;
}

//-----------------------------------------------------------------------------
int TDrsFitAlg::Fit(const TDrsChannel* Channel) {
  return 0;
}


//_____________________________________________________________________________
void     TDrsFitAlg::AddHistogram(TObject* hist, const char* FolderName) {
  TFolder* fol = (TFolder*) fFolder->FindObject(FolderName);
  //  fol->Add(hist); 
  fFolder->Add(hist); 
}



//_____________________________________________________________________________
void TDrsFitAlg::HBook1F(TH1F*& Hist, const char* Name, const char* Title,
			 Int_t Nx, Double_t XMin, Double_t XMax,
			 const char* FolderName)
{
  // book 2D histogram, add it to the module's list of histograms and 
  // return pointer to it to the user

  Hist = new TH1F(Name,Title,Nx,XMin,XMax);
  AddHistogram(Hist,FolderName);
}

//_____________________________________________________________________________
void TDrsFitAlg::HBook2F(TH2F*& Hist, const char* Name, const char* Title,
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
void TDrsFitAlg::HProf(TProfile*& Hist, const char* Name, const char* Title,
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
void TDrsFitAlg::DeleteHistograms(TFolder* Folder) {
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
int TDrsFitAlg::Display() {
  Error("Display","Not implemented yet");
  return 0;
}
