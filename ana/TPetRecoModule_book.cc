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
#include "Stntuple/obj/TStnHeaderBlock.hh"

#include "drs/ana/TPetRecoModule.hh"

//-----------------------------------------------------------------------------
//  
//-----------------------------------------------------------------------------
void TPetRecoModule::BookClusterHistograms(ClusterHist_t* Hist, const char* Folder) {
  char name [200];
  char title[200];

  HBook1F(Hist->fCharge,"charge","Cluster Charge",100,0,1000,Folder);
  HBook1F(Hist->fPH    ,"ph"    ,"Pulse Height  ",100,0,1000,Folder);
  HBook1F(Hist->fSize  ,"size"  ,"Cluster Size  ",100,0,100 ,Folder);

}

//-----------------------------------------------------------------------------
//  
//-----------------------------------------------------------------------------
void TPetRecoModule::BookEventHistograms(EventHist_t* Hist, const char* Folder) {
  char name [200];
  char title[200];
  HBook1F(Hist->fRunNumber,"run_number","Run Number",100,0,100,Folder);
  HBook1F(Hist->fEventNumber,"event_number","Event Number",1000,0,1000,Folder);
  HBook2F(Hist->fQEvent   ,"vm_event","Event Q"   ,8,0,8,4,0,4,Folder);
  HBook2F(Hist->fVMaxEvent,"q_event" ,"Event VMax",8,0,8,4,0,4,Folder);

  for (int i=0; i<kMaxChannels; i++) {
    sprintf(name,"v_%03i",i);
    HBook1F(Hist->fV[i],name,name,1024,0,1024,Folder);
    sprintf(name,"u_%03i",i);
    HBook2F(Hist->fU[i],name,name,1024,0,1024,80,-20,20,Folder);
    sprintf(name,"q_%03i",i);
    HBook1F(Hist->fQCh[i],name,name,500,0,1000,Folder);
    sprintf(name,"vmaxch_%03i",i);
    HBook1F(Hist->fVMaxCh[i],name,name,500,0,500,Folder);
  }

  for (int i=0; i<2; i++) {
    sprintf(name,"seed_mod_%i",i);
    HBook2F(Hist->fSeed[i],name,name,4,0,4,4,0,4,Folder);
    sprintf(name,"qmax_mod_%i",i);
    HBook1F(Hist->fQMax[i],name,name,200,0,1000,Folder);
    sprintf(name,"vmax_mod_%i",i);
    HBook1F(Hist->fVMax[i],name,name,500,0,500,Folder);
    sprintf(name,"vmax_vs_ich_mod_%i",i);
    HBook2F(Hist->fVMaxVsChannel[i],name,name,32,0,32,500,0,500,Folder);
  }
}

//_____________________________________________________________________________
void TPetRecoModule::BookHistograms() {
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
      BookEventHistograms(&fHist.fEvent[i],Form("Hist/%s",folder_name));
    }
  }

}

