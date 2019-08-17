///////////////////////////////////////////////////////////////////////////////
// mode=1: select 4l candidates from GEN7 re-reconstructed 3l data
// debug bits:
//   001 : all
//   002 : passed
//   003 : failed
//   004 : events with |M(zzx)-MZ|<15
//   005 : events with N(leptons) > 4
//   006 : M(ZZ) > 150
//   007 : events with balanced MZ(jj) mass > 180 - should be 90!
//   008 : events with balanced  100 < MZ(jj) < 110 - should be 91.2!
//   009 : events with chi2(Z)<10 for both Z's 
//   010 : events with |M(ZZ)-MH| > 30
//   011 : events with M(ZZ) < 220 - investigate MZZ(2l2j)
//   012 : events with M(Z)  < 10
///////////////////////////////////////////////////////////////////////////////
#include "TF1.h"
#include "TPad.h"
#include "TCanvas.h"
#include "TEnv.h"
#include "TSystem.h"
#include "TLorentzVector.h"

#include "Stntuple/loop/TStnAna.hh"
#include "Stntuple/obj/TStnHeaderBlock.hh"

#include "drs/obj/TVme1742DataBlock.hh"
#include "drs/ana/TPetAnaModule.hh"


const float MZ = 91.2 ;
//_____________________________________________________________________________
TPetAnaModule::TPetAnaModule(const char* name, const char* title): 
  TStnModule(name,title)
{
}

//_____________________________________________________________________________
TPetAnaModule::~TPetAnaModule() {
}

//_____________________________________________________________________________
int TPetAnaModule::Debug(int Passed) {

  TStnHeaderBlock* header = GetHeaderBlock();
//-----------------------------------------------------------------------------
// event-level bits 
//-----------------------------------------------------------------------------
  if ( (PrintLevel() == 1) ) {
    header->Print(Form("%s:001: ALL events",GetName()));
    return 0;
  }

  if ( (PrintLevel() == 2) && Passed) {
    header->Print(Form("%s:002: all PASSED events",GetName()));
    return 0;
  }

  if ( (PrintLevel() == 3) && (Passed == 0) ) {
    header->Print(Form("%s:003: all REJECTED events",GetName()));
    return 0;
  }
  return 0;
}

//_____________________________________________________________________________
void TPetAnaModule::BookEventHistograms(EventHist_t& Hist, const char* Folder) {
  char name [200];
  char title[200];
//-----------------------------------------------------------------------------
//  run number and MET histograms
//-----------------------------------------------------------------------------
  HBook1F(Hist.fRunNumber,"run_number","Run Number",100,0,100,Folder);
  HBook1F(Hist.fEventNumber,"event_number","Event Number",1000,0,1000,Folder);
  for (int i=0; i<4; i++) {
    sprintf(name,"first_cell_%i",i);
    HBook1F(Hist.fFirstCell[i],name,name,1024,0,1024,Folder);

    sprintf(name,"trig_pulse_%i",i);
    HBook2F(Hist.fTrigPulse[i],name,name,1024,0,1024,4000,0,4000,Folder);

    sprintf(name,"last_trig_pulse_%i",i);
    HBook1F(Hist.fLastTrigPulse[i],name,name,1024,0,1024,Folder);
  }

  for (int i=0; i<32; i++) {
    sprintf(name,"wf_%i",i);
    HBook2F(Hist.fWaveForm[i],name,name,1024,0,1024,4000,0,4000,Folder);

    sprintf(name,"wf_last_%i",i);
    HBook1F(Hist.fLastWaveForm[i],name,name,1024,0,1024,Folder);

    sprintf(name,"wf1_%i",i);
    HBook2F(Hist.fWaveForm1[i],name,name,1024,0,1024,4000,0,4000,Folder);

    sprintf(name,"wf1_last_%i",i);
    HBook1F(Hist.fLastWaveForm1[i],name,name,1024,0,1024,Folder);
  }

}

//_____________________________________________________________________________
void TPetAnaModule::BookHistograms() {
  TFolder* fol;
  TFolder* hist_folder;

  char folder_name[200];

  Delete("hist");
  hist_folder = (TFolder*) GetFolder()->FindObject("Hist");
//-----------------------------------------------------------------------------
//  book event histograms
//-----------------------------------------------------------------------------
  int  book_event_histset[kNEventHistSets];
  for (int i=0; i<kNEventHistSets; i++) book_event_histset[i] = 0;

  book_event_histset[ 0] = 1;   // all events

  for (int i=0; i<kNEventHistSets; i++) {
    if (book_event_histset[i] != 0) {
      sprintf(folder_name,"event_%i",i);
      fol = (TFolder*) hist_folder->FindObject(folder_name);
      if (! fol) fol = hist_folder->AddFolder(folder_name,folder_name);
      BookEventHistograms(fHist.fEvent[i],Form("Hist/%s",folder_name));
    }
  }

}

//_____________________________________________________________________________
int TPetAnaModule::BeginJob() {

  RegisterDataBlock("Vme1742DataBlock" ,"TVme1742DataBlock",&fVme1742DataBlock);

                                        // book histograms
  BookHistograms();
				        // passing debug flags
  TStnModule::BeginJob();

  return 0;
}

//-----------------------------------------------------------------------------
// run-dependent initializations
//-----------------------------------------------------------------------------
int TPetAnaModule::BeginRun() {
  return 0;
}

//_____________________________________________________________________________
int TPetAnaModule::FillEventHistograms(EventHist_t& Hist) {

  float a;
  int   ich, first_cell, icc;

  TStnHeaderBlock* h = GetHeaderBlock();

  Hist.fRunNumber ->Fill(h->RunNumber());
  Hist.fEventNumber ->Fill(h->EventNumber());


  for (int ig=0; ig<4; ig++) {
    first_cell = fVme1742DataBlock->FirstCell(ig);

    Hist.fFirstCell[ig]->Fill(first_cell);

    Hist.fLastTrigPulse[ig]->Reset();
    for (int cell=0; cell<1024; cell++) {
      a = fVme1742DataBlock->TriggerData(ig,cell);
      Hist.fTrigPulse[ig]->Fill(cell,a);
      Hist.fLastTrigPulse[ig]->Fill(cell,a);
    }
  }

  for (int ig=0; ig<4; ig++) {

    first_cell = fVme1742DataBlock->FirstCell(ig);

    for (int ic=0; ic<8; ic++) {
      ich = ig*8+ic;
      Hist.fLastWaveForm[ich]->Reset();
      
      for (int cell=0; cell<1024; cell++) {
	a = fVme1742DataBlock->GroupData(ig,ic,cell);
	Hist.fWaveForm[ich]->Fill(cell,a);
	Hist.fLastWaveForm[ich]->Fill(cell,a);

	icc = cell+first_cell;
	if (icc >= 1024) icc = icc-1024;

	Hist.fWaveForm1    [ich]->Fill(icc,a);
	Hist.fLastWaveForm1[ich]->Fill(icc,a);
      }
    }
  }
 
  return 0;
}

//-----------------------------------------------------------------------------
int TPetAnaModule::FillHistograms() {

//-----------------------------------------------------------------------------
// EVENT set 0: all    events 
//-----------------------------------------------------------------------------
  FillEventHistograms(fHist.fEvent[0]);
//-----------------------------------------------------------------------------
// EVENT set 1: passed events
//-----------------------------------------------------------------------------
//  if (GetPassed()) FillEventHistograms(fHist.fEvent[1]);

  return 0;
}

//_____________________________________________________________________________
int TPetAnaModule::Event(int ientry) {

  int            passed (0), id_word, loose_id_word, dir;
  double         pt, chi2_tot;

  fVme1742DataBlock->GetEntry(ientry);
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
  TStnModule::SetPassed(passed);

  FillHistograms();
  Debug(passed);

  return 0;                    
}

//_____________________________________________________________________________
int TPetAnaModule::EndJob() {
  //  printf("----- end job: ---- %s\n",GetName());

  return 0;
}
