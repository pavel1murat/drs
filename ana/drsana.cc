//-----------------------------------------------------------------------------
// Debugging bits:
// 1: charge < 60 in channel 0, < 80 in channel 2
//-----------------------------------------------------------------------------
#include <math.h>

#include "ana/drsana.hh"

#include <TStyle.h>
#include <TCanvas.h>
#include <TMath.h>
#include "TInterpreter.h"
#include "TSystem.h"
// #include "TFitResult.h"
// #include "Fit/FitResult.h"

#include "obj/TStripLine.hh"

#include "ana/TKneeFitAlg.hh"
#include "ana/TExpoFitAlg.hh"
#include "ana/TRussFitAlg.hh"
#include "ana/TGausFitAlg.hh"
#include "ana/TStripLineFitOld.hh"
#include "obj/TDrsGlobals.hh"

ClassImp(drsana)


drsana* drsana::fgInstance;

//-----------------------------------------------------------------------------
drsana::drsana(const char* Filename, int Mode) {
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
  TFile*        f; 
  TTree*        tree;
  int           rc;
  TDrsChannel*  ch;

  const char* fn = "data/aaaa_001.root";

  drsana::fgInstance = this;

  fMode     = Mode;
  fFilename = Filename;

  if (Filename) fn = Filename;

  // allow different folders to have histograms with the same name

  TH1::AddDirectory(0);

  f = (TFile*)gROOT->GetListOfFiles()->FindObject(fn);
  if (! f) f = new TFile(fn);

  tree = (TTree*)gDirectory->Get("pulse");

  fListOfChannels   = new TObjArray(kMaxNChannels);
  fListOfAlgorithms = new TObjArray();

  fListOfStripLines = 0;
  					// here fChan[0] and fChan[1] are defined
					// list of algorithms is defined at the same point
  Init(tree);
}

//-----------------------------------------------------------------------------
drsana::~drsana() {
  if (!fChain) return;
  delete fChain->GetCurrentFile();

  delete fListOfChannels; 

  if (fMode == 1) {
    delete fListOfStripLines;
    delete fStripLineFit;
  }
}


//-----------------------------------------------------------------------------
Int_t drsana::GetEntry(Long64_t entry) {
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}

//-----------------------------------------------------------------------------
Long64_t drsana::LoadTree(Long64_t entry) {
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (!fChain->InheritsFrom(TChain::Class()))  return centry;
   TChain *chain = (TChain*)fChain;
   if (chain->GetTreeNumber() != fCurrent) {
      fCurrent = chain->GetTreeNumber();
      Notify();
   }
   return centry;
}

//-----------------------------------------------------------------------------
void drsana::Init(TTree *tree) {
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers

  char         cmd[200], fn[200], config_file[200];
  int          rc;
  TDrsChannel* ch;

  if (!tree) return;

  fAnaFolder  = gROOT->GetRootFolder()->AddFolder("Ana","DRSANA Main Folder");
  fFolder     = fAnaFolder->AddFolder("drs","DRSANA Folder");
  fHistFolder = fFolder->AddFolder("Hist","DRS Hist Folder");

  fChain = tree;
  fCurrent = -1;
  fChain->SetMakeClass(1);

  fChain->SetBranchAddress("b1_t" , b1_t   , &b_b1_t);

   // kludge: so far, use channels 0,1 or 2,3

  if (fChain->GetBranch("b1_c1") != 0) fChain->SetBranchAddress("b1_c1", b1_c[0], &b_b1_c1);
  if (fChain->GetBranch("b1_c2") != 0) fChain->SetBranchAddress("b1_c2", b1_c[1], &b_b1_c2);
  if (fChain->GetBranch("b1_c3") != 0) fChain->SetBranchAddress("b1_c3", b1_c[2], &b_b1_c3);
  if (fChain->GetBranch("b1_c4") != 0) fChain->SetBranchAddress("b1_c4", b1_c[3], &b_b1_c4);

  fChain->SetBranchAddress("event"   , &fEventNumber, &b_event);
  fChain->SetBranchAddress("tc1"     , &tc1         , &b_tc1  );

  if (fChain->GetBranch("usedchan") != 0) {
    fChain->SetBranchAddress("usedchan", &usedchan    , &b_usedchan);
    fChain->SetBranchAddress("nch"     , &fNChannels  , &b_nch);
  }
  else {
					// case of the only old file I have
    fNChannels  = 2;
    usedchan[0] = 3; // 2;
    usedchan[1] = 4; // 3;
  }

  int ientry = LoadTree(0);
  if (ientry < 0) return ;
//-----------------------------------------------------------------------------
// read first entry to determine number of channels
//-----------------------------------------------------------------------------
  int nb = fChain->GetEntry(0);
  
  if  (fNChannels == 1) {
					// a kludge
					// make sure we can handle configuration with 
					// only one channel read
    usedchan[1] = usedchan[0];
    fNChannels = 2;
  }

  for (int i=0; i<fNChannels; i++) {
    fChan[i] = usedchan[i]-1;
					// allocate space only for used channels

    //    if (fChan[i] >= 0) ch = NewChannel(i,fChan[i]);
    if (fChan[i] >= 0) ch = NewChannel(fChan[i],fChan[i]);
  }
//-----------------------------------------------------------------------------
// list of channels initialized, initialize list of algorithms used
//-----------------------------------------------------------------------------
  TDrsFitAlg* alg;
  TStripLine* sl;

  // alg = new TKneeFitAlg("knee","knee fit",fListOfChannels);
  // fListOfAlgorithms->Add(alg);

  // alg = new TExpoFitAlg("expo","expo fit",fListOfChannels);
  //fListOfAlgorithms->Add(alg);

  alg = new TGausFitAlg("gaus","gaussian fit to the pulse shape",fListOfChannels);
  fListOfAlgorithms->Add(alg);

  if (fMode == 0) {
    fNStripLines = 0;
  }
  else if (fMode == 1) {
//-----------------------------------------------------------------------------
// fMode == 1: process stripline
//-----------------------------------------------------------------------------
    fListOfStripLines = new TObjArray();
    fNStripLines    = 1;
    sl              = new TStripLine();
    sl->fChannel[0] = (TDrsChannel*) fListOfChannels->At(0);
    sl->fChannel[1] = (TDrsChannel*) fListOfChannels->At(1);
    fListOfStripLines->Add(sl);

    fStripLineFit   = new TStripLineFitOld("slin","slin",fListOfChannels);
    fHistFolder->Add(fStripLineFit->GetFolder());
  }

  //alg = new TExp1FitAlgNew("exp1","exponential fit",fListOfChannels);
  //fListOfAlgorithms->Add(alg);

  //alg = new TExpoFitAlg("nois","noise fit",fListOfChannels);
  //fListOfAlgorithms->Add(alg);

  // alg = new TRussFitAlg("russ","russ fit",fListOfChannels);
  // fListOfAlgorithms->Add(alg);
//-----------------------------------------------------------------------------
// add algorithm and channel folders
//-----------------------------------------------------------------------------
  int nalg = fListOfAlgorithms->GetEntriesFast();
  for (int i=0; i<nalg; i++) {
    alg = GetAlgorithm(i);
    fHistFolder->Add(alg->GetFolder());
  }

  for (int ich=0; ich<GetNChannels(); ich++) {
    ch = GetChannel(ich);
    if (ch) {
      fHistFolder->Add(ch->GetFolder());
    }
  }
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
  fCanvas = 0;
  fNPar   = 3;

  for (int i=0; i<kNDebugFlags; i++) fDebug[i] = 0;

  sprintf(cmd,"echo %s | awk -F / '{print $NF}' | sed 's/.root/.C/'",fFilename.Data());
  FILE* pipe = gSystem->OpenPipe(cmd,"r");
  fgets(fn,10000,pipe);
  gSystem->ClosePipe(pipe);
  sprintf(config_file,"drs/config/%s",fn);

  sprintf(cmd,"ls %s > /dev/null 2>>/dev/null",config_file);

  rc = gSystem->Exec(cmd);
  if (rc == 0) {
    gInterpreter->LoadMacro(config_file);
    gInterpreter->ProcessLine("init();");
  }
  else {
    Warning("Init",Form("No config file %s",config_file));
  }

}


//-----------------------------------------------------------------------------
Bool_t drsana::Notify() {
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

//-----------------------------------------------------------------------------
// 'per channel' histograms
//-----------------------------------------------------------------------------
int drsana::BookHistograms(EventHist_t* Hist) {

  int           ich1, ich2, nalg;
  char          name[100], title[100];
  TDrsChannel  *ch;
  TDrsFitAlg   *alg;

  TFolder*       fol;
  TFolder*       hist_folder;

  hist_folder = (TFolder*) GetFolder()->FindObject("Hist");

  char           folder_name[200];

  Hist->fRc = new TH1F("rc","Event Processing Code",100,-99,1);

  int nch = GetNChannels();

  for (int i=0; i<nch; i++) {
    ch = GetChannel(i);
    if (ch) {
      ch->BookHistograms();
    }
  }
//-----------------------------------------------------------------------------
// in an "event" we have 2 charges
//-----------------------------------------------------------------------------
  Hist->fQ2VsQ1   = new TH2F("q2_vs_q1"  ,"Q(2) vs Q(1)"  ,500,0,1000,500,0,1000);
  Hist->fPH2VsPH1 = new TH2F("ph2_vs_ph1","PH(2) vs PH(1)",250,0, 500,250,0, 500);
//-----------------------------------------------------------------------------
// loop over algorithms
//-----------------------------------------------------------------------------
  nalg = fListOfAlgorithms->GetEntriesFast();

  for (int i=0; i<nalg; i++) {
    alg = GetAlgorithm(i);
    alg->BookHistograms();
  }
					// book stripline hists, conditionally
  if (fMode == 1) {
    fStripLineFit->BookHistograms();
  }

  return 0;
}

//-----------------------------------------------------------------------------
void drsana::Show(Long64_t entry) {
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}


//-----------------------------------------------------------------------------
// this displays the knee fit results
//-----------------------------------------------------------------------------
int drsana::Display(const char* Algorithm) {
  GetAlgorithm(Algorithm)->Display();
  return 0;
}

//-----------------------------------------------------------------------------
int drsana::FillHistograms(EventHist_t* Hist) {

  double        v1min, v2min, v1ph, v2ph, ph;

  int           ich1, ich2;
  TDrsChannel   *ch, *ch1, *ch2;

  fHist.fRc->Fill(fRc);

//-----------------------------------------------------------------------------
// fill individual channel histograms
//-----------------------------------------------------------------------------
  int nch = GetNChannels();

  for (int i=0; i<nch; i++) {
    ch = GetChannel(i);
    if (ch) {
      ch->FillHistograms();
    }
  }
//-----------------------------------------------------------------------------
// algorithm histograms
//-----------------------------------------------------------------------------
  int nalg = fListOfAlgorithms->GetEntriesFast();

  TDrsFitAlg* alg;

  for (int i=0; i<nalg; i++) {
    alg = GetAlgorithm(i);
    alg->FillHistograms();
  }
//-----------------------------------------------------------------------------
// these are channels providing timing
//-----------------------------------------------------------------------------
  ich1 = fChan[0];
  ich2 = fChan[1];

  ch1  = GetChannel(ich1);
  ch2  = GetChannel(ich2);
					// this is pulse height, defined in ProcessEvent
  v1ph = ch1->fVMax;
  v2ph = ch2->fVMax;

  Hist->fPH2VsPH1->Fill(v1ph,v2ph);
  Hist->fQ2VsQ1->Fill(ch1->fQ,ch2->fQ);

  if (fMode == 1) {
//-----------------------------------------------------------------------------
// fill stripline histograms, conditionally
//-----------------------------------------------------------------------------
    fStripLineFit->FillHistograms();
  }

  return 0;
}


//-----------------------------------------------------------------------------
int drsana::ProcessStripLine(TStripLine* StripLine) {
  // TStripLineFit*  SFit; 

  TDrsChannel  *ch0, *ch1;
  TH1F*         hist;

  //----Range of fit
  double Smin, Smax;
  Smin = 180;
  Smax = 300;
  //----------------

  // Smooth the First Channel  , stripline has 2 channels
  ch0  = StripLine->fChannel[0];
  hist = ch0->GetHist()->fShape;
  smooth::smooth_hist(hist,Smin,Smax);

  // Fit the second channel with the parametrized first one.

  ch1 = StripLine->fChannel[1];
  fStripLineFit->Fit(ch1);

//   fStripLineFit->GetFitResults(20)->fTofData = gT1;

  //  fStripLineFit->Display();
  return 0;
}

//-----------------------------------------------------------------------------
int drsana::ProcessEvent(Long64_t jentry, int Mode) {
  int           rc[2], ich, nch(2);
  Long64_t      nb;
  Long64_t      ientry;
  TDrsChannel  *ch;
//-----------------------------------------------------------------------------
// read event
//-----------------------------------------------------------------------------
  ientry = LoadTree(jentry);
  if (ientry < 0) return -1;

  nb     = fChain->GetEntry(jentry);
  fEntry = jentry;

  TDrsGlobals::Instance()->SetEntry(jentry);
  fRc    = 0;
//-----------------------------------------------------------------------------
// correct event for the saw-tooth offset between odd and even channels
// correct even channels only
// so far used for Hamamatsu only
//-----------------------------------------------------------------------------
  nch = GetNChannels();
  for (int ich=0; ich<nch; ich++) {
    ch  = GetChannel(ich);
    if (ch) {
      ch->fCachedEventNumber = fEntry;
					// keep 'raw data' unchanged
      for (int i=0; i<1024; i++) {
	ch->fV[i] = -b1_c[ich][i];
      }
//-----------------------------------------------------------------------------
// first subtract odd-even shifts
// then subtract individual cell offsets - so far only for old DRS4 ? 
// so far use old offsets, determined assuming negative channel contents
//-----------------------------------------------------------------------------
      for (int i=0; i<kNCells; i+=2) {
	ch->fV[i] -= ch->fEvenShift;
      }

      for (int i=0; i<kNCells; i++) {
	ch->fV[i] += ch->fCellOffset[i];
	ch->fV[i] -= ch->fBaseOffset;
      }
//-----------------------------------------------------------------------------
// calculate pulse height (wrt the base)
// how does one calculate the base position? - histogram the cell counts !
// this is the step one
// also correct for the base offset here, to do it only once
//-----------------------------------------------------------------------------
      ch->fVMax     = 0;
      ch->fVMaxCell = -1;

      for (int i=0; i<1024; i++) {
	if (ch->fV[i] > ch->fVMax) { 
	  ch->fVMax     = ch->fV[i];
	  ch->fVMaxCell = i;
	}
					// use the same loop to initialize weights to 1
	ch->fWt[i] = 1.;
      }

      ch->fQ = 0;
      int i1 = ch->fFirstChannel;
      int i2 = i1+5*ch->fPulseWidth;
      if (i2 > 1024) i2 = 1024;

      for (int i=i1; i<i2; i++) {
					// voltages are negative and the charge is going to be 
					// positive
	ch->fQ = ch->fQ + ch->fV[i]/100;
      }

      ch->fQ1 = ch->fQ*511./ch->fQPeak;
//-----------------------------------------------------------------------------
//  fill signal shape histogram - it is used for fitting  and it makes sense to 
//  have maximum at 1
//-----------------------------------------------------------------------------
      ch->fHist.fShape->Reset();
      ch->fHist.fV->Reset();

      for (int i=0; i<kNCells; i++) {
	double sh = ch->fV[i]/(ch->fVMax+1.e-12);
	//	ch->fHist.fShape->Fill(i+0.5,sh);
	ch->fHist.fV->SetBinContent(i+1,ch->fV[i]);
	ch->fHist.fShape->SetBinContent(i+1,sh);
	//	ch->fHist.fShape->SetBinError  (i+1,1./(ch->fVMax+1.e-12));
	//	ch->fHist.fShape->SetBinError  (i+1,1.+1.0*sh);

	ch->fHist.fShapeVsCell[0]->Fill(i,sh);
	if (ch->fQ > ch->fMinQ) {
	  ch->fHist.fShapeVsCell[1]->Fill(i,sh);
	}
      }
    }
  }
//-----------------------------------------------------------------------------
// loop over the algorithms and do fitting
//-----------------------------------------------------------------------------
  int nalg = fListOfAlgorithms->GetEntriesFast();

  TDrsFitAlg* alg;

  for (int i=0; i<nalg; i++) {
    alg = (TDrsFitAlg*) fListOfAlgorithms->UncheckedAt(i);
    alg->SetEventNumber(fEventNumber);

    for (int ich=0; ich<nch; ich++) {
      ch = GetChannel(ich);
      if (ch) {
	alg->Fit(ch);
      }
    }
  }
  if (fMode == 1) { 
//-----------------------------------------------------------------------------
// process strip lines, conditionally
//-----------------------------------------------------------------------------
    TStripLine* sl;
    for (int i=0; i<fNStripLines; i++) {
      sl = (TStripLine*) fListOfStripLines->At(i);
      ProcessStripLine(sl);
    }
  }

  return fRc;
}

//-----------------------------------------------------------------------------
void drsana::Loop(int NEvents) {
//   In a ROOT session, you can do:
//      Root > .L drs.C
//      Root > drs t
//      Root > t.GetEntry(12); // Fill t data members with entry number 12
//      Root > t.Show();       // Show values of entry 12
//      Root > t.Show(16);     // Read and show values of entry 16
//      Root > t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch

  Long64_t nentries, nent;

  drsana::fgInstance = this;

  if (fChain == 0) return;

  BookHistograms(&fHist);

  nentries = fChain->GetEntriesFast();
  
  if (NEvents >  0) nent = NEvents;
  else              nent = nentries;

  for (Long64_t jentry=0; jentry<nent;jentry++) {
    ProcessEvent  (jentry,kDefaultMode);
    FillHistograms(&fHist);
  }
}

//-----------------------------------------------------------------------------
int drsana::Debug(int IEntry) {
  TDrsChannel   *ch1, *ch2;

  if (DebugFlag(1) != 0) {
    ch1 = GetChannel(fChan[0]);
    ch2 = GetChannel(fChan[1]);
    if ((ch1->fQ < 60) || (ch2->fQ < 80)) {
      printf(" >> IEntry = %5i fChannel[0].fQ = %10.3f fChannel[1].fQ = %10.3f\n",
	     IEntry,ch1->fQ,ch2->fQ);
    }
  }
  
  return 0;
}


//-----------------------------------------------------------------------------
int drsana::LoopTime(int NEvents) {
  int         rc(0), nalg;
  Long64_t    nentries, ientry, nb;
  TDrsFitAlg* alg; 

  if (fChain == 0) return -1;
  
  BookHistograms(&fHist);

  drsana::fgInstance = this;

  nentries = 0;
  if (NEvents >  0) nentries = NEvents;

  nentries = fChain->GetEntriesFast();

  if ((NEvents > 0) && (NEvents < nentries)) nentries = NEvents;
  
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
    
    // ientry = LoadTree(jentry);
    // if (ientry < 0) return -1;
    // nb = fChain->GetEntry(jentry);
    // 					// correct for offset
    // for (int i=0; i<1024; i+=2) {
    //   b1_c[fChan[0]][i] += fEvenShift[fChan[0]];
    //   b1_c[fChan[1]][i] += fEvenShift[fChan[1]];
    // }

    rc = ProcessEvent(jentry, kLoopTimeMode);

    // 					// need because of fVMin...
    FillHistograms(&fHist);
//-----------------------------------------------------------------------------
// time fit analysis
//-----------------------------------------------------------------------------
    if (fRc < 0) {
					// one of the fits didn't converge...
      goto NEXT_EVENT;
    }
//-----------------------------------------------------------------------------
// display
//-----------------------------------------------------------------------------
    nalg = fListOfAlgorithms->GetEntriesFast();

    for (int i=0; i<nalg; i++) {
      alg = GetAlgorithm(i);
      if (alg->GetDisplayResults()) {
	alg->Display();
      }
    }
//-----------------------------------------------------------------------------
// debugging printouts
//-----------------------------------------------------------------------------
    Debug(jentry);

  NEXT_EVENT:;
  }

  return 0;
}

//-----------------------------------------------------------------------------
// for a given channel
//-----------------------------------------------------------------------------
int drsana::FitCellOffsets(int Channel, const char* OutputFile) {
  int rc(0);

  double  offset[1024], sig[1024], chi2[1024];

  TH1D*   h_py;
  TF1*    fun;

  Error("FitCellOffsets",Form("fHist.fDv2VsCell not defined"));

  return -1;
  
  FILE* f  = fopen(OutputFile,"w");
  if (f == 0) {
    Error("FitCellOffsets",Form("can\'t open %s",OutputFile));
    return -1;
  }

  // each channel has 1024 cells

  //  TFitResultPtr res; 
  for (int i=1; i<=1024; i++) {
    //    h_py = fHist.fDvVsCell[Channel]->ProjectionY("py",i,i);
    //###    h_py = fHist.fDv2VsCell[Channel]->ProjectionY("py",i,i);
    //    res  = h_py->Fit("gaus","q");
    fun  = h_py->GetFunction("gaus");
    if (fun) {
      offset[i-1] = fun->GetParameter(1);
      sig   [i-1] = fun->GetParameter(2);
      chi2  [i-1] = fun->GetChisquare();
    }
    else {
      offset[i-1] = 999.;
      sig   [i-1] = 999.;
      chi2  [i-1] = 999.;
    }
    // offset[i-1] = res.Get()->Parameter(1); 
    // sig   [i-1] = res.Get()->Parameter(2);
    // chi2  [i-1] = res.Get()->Chi2();
  }
//-----------------------------------------------------------------------------
// done with fitting, write the fit results out
//-----------------------------------------------------------------------------
  fprintf(f,"#--------------------------------------------------------------\n");
  for (int i=0; i<1024; i++) {
    fprintf(f," %5i %8.3f %8.3f %8.3f \n",i,offset[i],sig[i], chi2[i]);
  }
  fprintf(f,"#--------------------------------------------------------------\n");
  fclose(f);
  return 0;
}
  
//-----------------------------------------------------------------------------
// for a given channel
//-----------------------------------------------------------------------------
int drsana::ReadCellOffsets(int Channel, const char* InputFile) {
  int rc(0);

  FILE    *f;
  float   offset, sigma, chi2;
  int     cell, done(0);

  TDrsChannel *ch;

  ch = GetChannel(Channel);

  char    c[1000];
  f  = fopen(InputFile,"r");
  if (f == 0) {
    Error("ReadCellOffsets",Form("missing file %s\n",InputFile));
    return -2;
  }

  while ( ((c[0]=getc(f)) != EOF) && !done) {

					// check if it is a comment line
    if (c[0] != '#') {
      ungetc(c[0],f);
					// read channel number
      fscanf(f,"%i" ,&cell    );
      fscanf(f,"%f" ,&offset  );
      fscanf(f,"%f" ,&sigma   );
      fscanf(f,"%f" ,&chi2   );
//-----------------------------------------------------------------------------
// set calibrations
//-----------------------------------------------------------------------------
      ch->fCellOffset[cell] = offset;
      ch->fCellSigma [cell] = sigma;
    }
					// skip line
    fgets(c,100,f);
  }

  fclose(f);
  return 0;
}
  


//_____________________________________________________________________________
int  drsana::SaveFolder(TFolder* Folder, TDirectory* Dir) {
  // save Folder into a subdirectory
  // do not write TStnModule's - for each TStnModule save contents of its
  // fFolder

  TFolder*     fol;
  TDirectory*  dir;
  TObject*     o;
//-----------------------------------------------------------------------------
// create new subdirectory in Dir to save Folder
//-----------------------------------------------------------------------------
  Dir->cd();
  //  dir = new TDirectory(Folder->GetName(),Folder->GetName(),"");
  dir = Dir->mkdir(Folder->GetName(),Folder->GetName());
  dir->cd();

//   printf(" ------------------- Dir: %s, new dir: %s\n",
// 	 Dir->GetName(),dir->GetName());


  TIter  it(Folder->GetListOfFolders());
  while ((o = it.Next())) {
//     printf(" o->GetName, o->ClassName : %-20s %-20s\n",
// 	   o->GetName(),
// 	   o->ClassName());

    if (strcmp(o->ClassName(),"TFolder") == 0) {
      SaveFolder((TFolder*) o, dir);
      //      dir->cd();
    }
    else if (! o->InheritsFrom("TDrsFitAlg")) {
      //      printf("gDirectory->GetPath = %s\n",gDirectory->GetPath());
      o->Write();
      //      gDirectory->GetListOfKeys()->Print();
    }
  }

  Dir->cd();
  return 0;
}

//_____________________________________________________________________________
void drsana::SaveHist(const char* Filename, Int_t Mode) 
{
  // save histograms booked by all the modules into a file with the given name
  // Mode = 1: save folders
  // Mode = 2: save directories

  TFile* f = new TFile(Filename,"recreate");

  SaveFolder(fAnaFolder,f);

  f->Close();
  delete f;
}
