//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Sep 22 00:42:32 2011 by ROOT version 5.30/01
// from TTree drs/produced from DRS4 binary file
// found on file: data_meander.bin.root
//////////////////////////////////////////////////////////

#ifndef DRSBinRead_h
#define DRSBinRead_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

class DRSBinRead {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Float_t         t[1024];
   Float_t         v1[1024];
   UChar_t         usedchan[4];
   Int_t           event;
   Int_t           year;
   Int_t           month;
   Int_t           day;
   Int_t           hour;
   Int_t           minute;
   Int_t           second;
   Int_t           millisecond;

   // List of branches
   TBranch        *b_t;   //!
   TBranch        *b_v1;   //!
   TBranch        *b_usedchan;   //!
   TBranch        *b_event;   //!
   TBranch        *b_year;   //!
   TBranch        *b_month;   //!
   TBranch        *b_day;   //!
   TBranch        *b_hour;   //!
   TBranch        *b_minute;   //!
   TBranch        *b_second;   //!
   TBranch        *b_millisecond;   //!

   DRSBinRead(TTree *tree=0);
   virtual ~DRSBinRead();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef DRSBinRead_cxx
DRSBinRead::DRSBinRead(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("data_meander.bin.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("data_meander.bin.root");
      }
      f->GetObject("drs",tree);

   }
   Init(tree);
}

DRSBinRead::~DRSBinRead()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t DRSBinRead::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t DRSBinRead::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void DRSBinRead::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("t", t, &b_t);
   fChain->SetBranchAddress("v1", v1, &b_v1);
   fChain->SetBranchAddress("usedchan", usedchan, &b_usedchan);
   fChain->SetBranchAddress("event", &event, &b_event);
   fChain->SetBranchAddress("year", &year, &b_year);
   fChain->SetBranchAddress("month", &month, &b_month);
   fChain->SetBranchAddress("day", &day, &b_day);
   fChain->SetBranchAddress("hour", &hour, &b_hour);
   fChain->SetBranchAddress("minute", &minute, &b_minute);
   fChain->SetBranchAddress("second", &second, &b_second);
   fChain->SetBranchAddress("millisecond", &millisecond, &b_millisecond);
   Notify();
}

Bool_t DRSBinRead::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void DRSBinRead::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t DRSBinRead::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef DRSBinRead_cxx
