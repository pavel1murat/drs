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
#include "drs/ana/TGausFitAlgNew.hh"
#include "drs/ana/TStripLineFit.hh"

#include "drs/ana/TPet2AnaModule.hh"

//-----------------------------------------------------------------------------
//  
//-----------------------------------------------------------------------------
void TPet2AnaModule::BookClusterHistograms(ClusterHist_t* Hist, const char* Folder) {
  char name [200];
  char title[200];

  HBook1F(Hist->fCharge,"charge","Cluster Charge",100,0,1000,Folder);
  HBook1F(Hist->fPH    ,"ph"    ,"Pulse Height  ",100,0,1000,Folder);
  HBook1F(Hist->fSize  ,"size"  ,"Cluster Size  ",100,0,100 ,Folder);

}

//-----------------------------------------------------------------------------
//  
//-----------------------------------------------------------------------------
void TPet2AnaModule::BookStripLineHistograms(StripLineHist_t* Hist, 
					     const char* Folder) {
  char name [200];
  char title[200];

  HBook1F(Hist->fDeltaT,"deltat","Delta T12",100,-1,1, Folder);

}

//-----------------------------------------------------------------------------
//  
//-----------------------------------------------------------------------------
void TPet2AnaModule::BookEventHistograms(EventHist_t* Hist, const char* Folder) {
  char name [200];
  char title[200];
  HBook1F(Hist->fRunNumber  ,"run_number"  ,"Run Number"  , 100,0, 100,Folder);
  HBook1F(Hist->fEventNumber,"event_number","Event Number",1000,0,1000,Folder);
  HBook2F(Hist->fQ1Event    ,"q1_event"    ,"Event Q1"    ,8,0,8,4,0,4,Folder);
  HBook2F(Hist->fVMaxEvent  ,"vmax_event"  ,"Event VMax"  ,8,0,8,4,0,4,Folder);

  HBook2F(Hist->fQ1Module[0],"q1_mod_0"    ,"Module[0] Event Occupancy",4,0,4,4,0,4,Folder);
  HBook2F(Hist->fQ1Module[1],"q1_mod_1"    ,"Module[1] Event Occupancy",4,0,4,4,0,4,Folder);
 
  for (int i=0; i<4; i++) {
    sprintf(name,"vtrig_%i",i);
    HBook1F(Hist->fVTrig[i],name,name,1024,0,1024,Folder);
  }

  for (int i=0; i<kMaxChannels; i++) {
    sprintf(name,"v_%03i",i);
    HBook1F(Hist->fV[i],name,name,1024,0,1024,Folder);
    sprintf(name,"u_%03i",i);
    HBook2F(Hist->fU[i],name,name,1024,0,1024,200,-50,950,Folder);
    sprintf(name,"q_%03i",i);
    HBook1F(Hist->fQCh[i],name,name,2000,0,2000,Folder);
    sprintf(name,"q1_%03i",i);
    HBook1F(Hist->fQ1Ch[i],name,name,200,0,1000,Folder);
    sprintf(name,"vmaxch_%03i",i);
    HBook1F(Hist->fVMaxCh[i],name,name,1000,0,1000,Folder);
    sprintf(name,"ped_%03i",i);
    HBook1F(Hist->fPedestal[i],name,name,500,-25,25,Folder);
    sprintf(name,"c2p_%03i",i);
    HBook1F(Hist->fChi2Ped[i],name,name,100,0,5,Folder);
  }

  for (int i=0; i<2; i++) {
    sprintf(name,"seed_mod_%i",i);
    HBook2F(Hist->fSeed[i],name,name,4,0,4,4,0,4,Folder);
    sprintf(name,"qmax_mod_%i",i);
    HBook1F(Hist->fQMax[i],name,name,600,0,300,Folder);
    sprintf(name,"q1max_mod_%i",i);
    HBook1F(Hist->fQ1Max[i],name,name,200,0,1000,Folder);
    sprintf(name,"vmax_mod_%i",i);
    HBook1F(Hist->fVMax[i],name,name,500,0,500,Folder);

    sprintf(name,"t0_vs_fc_%i",i);
    HBook2F(Hist->fT0VsFirstCell[i],name,name,1024,0,1024,400,0,400,Folder);
  }

  sprintf(name,"vmax_vs_ich");
  HBook2F(Hist->fVMaxVsChannel,name,name,32,0,32,500,0,500,Folder);
  sprintf(name,"qmax_vs_ich");
  HBook2F(Hist->fQMaxVsChannel,name,name,32,0,32,600,0,300,Folder);
  sprintf(name,"q1max_vs_ich");
  HBook2F(Hist->fQ1MaxVsChannel,name,name,32,0,32,200,0,1000,Folder);
//-----------------------------------------------------------------------------
// in an "event" we have 2 charges
//-----------------------------------------------------------------------------
  HBook2F(Hist->fQ2VsQ1  ,"q2_vs_q1"  ,"Q(2) vs Q(1)"  ,200,0,1000,200,0,1000,Folder);
  HBook2F(Hist->fPH2VsPH1,"ph2_vs_ph1","PH(2) vs PH(1)",250,0,500,250,0,500,Folder);

  for (int ix=0; ix<4; ix++) {
    for (int iy=0; iy<4; iy++) {
      HBook2F(Hist->fOcc1[ix][iy],
	      Form("occ1_%i_%i",ix,iy),Form("OCC1 ix2=%i iy2=%i",ix,iy),
	      4,0,4,4,0,4,Folder);
   
      HBook2F(Hist->fOcc2[ix][iy],
	      Form("occ2_%i_%i",ix,iy),Form("OCC2 ix1=%i iy1=%i",ix,iy),
	      4,0,4,4,0,4,Folder);
   
    }
  }

  sprintf(name,"dfc10_vs_entry");
  HBook2F(Hist->fDfc10VsEntry,name,name,100,0,100,1024,-512,512,Folder);
  sprintf(name,"dfc20_vs_entry");
  HBook2F(Hist->fDfc20VsEntry,name,name,100,0,100,1024,-512,512,Folder);
  sprintf(name,"dfc30_vs_entry");
  HBook2F(Hist->fDfc30VsEntry,name,name,100,0,100,1024,-512,512,Folder);

  sprintf(name,"p0_q1max");
  HBook1F(Hist->fP0Q1Max,name,name,200,0,1000,Folder);
  sprintf(name,"p0_q1sum_0");
  HBook1F(Hist->fP0Q1Sum[0],name,name,200,0,1000,Folder);
  sprintf(name,"p0_q1sum_1");
  HBook1F(Hist->fP0Q1Sum[1],name,name,200,0,1000,Folder);

  sprintf(name,"p1_q0max");
  HBook1F(Hist->fP1Q0Max,name,name,200,0,1000,Folder);
  sprintf(name,"p1_q0sum_0");
  HBook1F(Hist->fP1Q0Sum[0],name,name,200,0,1000,Folder);
  sprintf(name,"p1_q0sum_1");
  HBook1F(Hist->fP1Q0Sum[1],name,name,200,0,1000,Folder);

  sprintf(name,"p0_c1_dt");
  HBook1F(Hist->fP0C1Dt,name,name,200,-5,5,Folder);

  sprintf(name,"p1_c0_dt");
  HBook1F(Hist->fP1C0Dt,name,name,200,-5,5,Folder);

}

//_____________________________________________________________________________
void TPet2AnaModule::BookHistograms() {

  TFolder*       fol;
  TFolder*       hist_folder;
  int            nalg;
  TDrsFitAlgNew  *alg;

  char           folder_name[200];

  Delete("hist");
  hist_folder = (TFolder*) GetFolder()->FindObject("Hist");
//-----------------------------------------------------------------------------
//  book event histograms
//-----------------------------------------------------------------------------
  int  book_event_histset[kNEventHistSets];
  for (int i=0; i<kNEventHistSets; i++) book_event_histset[i] = 0;

  book_event_histset[ 0] = 1;   // all events
  book_event_histset[ 1] = 1;   // events with photopeak on both sides
  book_event_histset[24] = 1;   // events with photopeak and ICH2==24

  for (int i=0; i<kNEventHistSets; i++) {
    if (book_event_histset[i] != 0) {
      sprintf(folder_name,"event_%i",i);
      fol = (TFolder*) hist_folder->FindObject(folder_name);
      if (! fol) fol = hist_folder->AddFolder(folder_name,folder_name);
      fHist.fEvent[i] = new EventHist_t;
      BookEventHistograms(fHist.fEvent[i],Form("Hist/%s",folder_name));
    }
  }
//-----------------------------------------------------------------------------
//  book cluster histograms
//-----------------------------------------------------------------------------
  int  book_cluster_histset[kNClusterHistSets];
  for (int i=0; i<kNClusterHistSets; i++) book_cluster_histset[i] = 0;

  book_cluster_histset[ 0] = 1;   // all events
  book_cluster_histset[ 1] = 1;   // 
  book_cluster_histset[ 2] = 1;   // 

  for (int i=0; i<kNClusterHistSets; i++) {
    if (book_cluster_histset[i] != 0) {
      sprintf(folder_name,"cls_%i",i);
      fol = (TFolder*) hist_folder->FindObject(folder_name);
      if (! fol) fol = hist_folder->AddFolder(folder_name,folder_name);
      fHist.fCluster[i] = new ClusterHist_t;
      BookClusterHistograms(fHist.fCluster[i],Form("Hist/%s",folder_name));
    }
  }
//-----------------------------------------------------------------------------
//  book strip line histograms
//-----------------------------------------------------------------------------
  int  book_stripline_histset[kNStripLineHistSets];
  for (int i=0; i<kNStripLineHistSets; i++) book_stripline_histset[i] = 0;

  book_stripline_histset[ 0] = 1;   // all events

  for (int i=0; i<kNStripLineHistSets; i++) {
    if (book_stripline_histset[i] != 0) {
      sprintf(folder_name,"sl_%i",i);
      fol = (TFolder*) hist_folder->FindObject(folder_name);
      if (! fol) fol = hist_folder->AddFolder(folder_name,folder_name);
      fHist.fStripLine[i] = new StripLineHist_t;
      BookStripLineHistograms(fHist.fStripLine[i],Form("Hist/%s",folder_name));
    }
  }
//-----------------------------------------------------------------------------
// loop over algorithms
//-----------------------------------------------------------------------------
  nalg = fListOfAlgorithms->GetEntriesFast();

  for (int i=0; i<nalg; i++) {
    alg = GetAlgorithm(i);
    alg->BookHistograms();
  }
//-----------------------------------------------------------------------------
// handle striplines
//-----------------------------------------------------------------------------
//  fStripLineFit->BookHistograms();
}

