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
#include "Stntuple/val/stntuple_val_functions.hh"

#include "drs/obj/TVme1742DataBlock.hh"
#include "drs/ana/TPetRecoModule.hh"

#include "TGeant/TVisNode.hh"
#include "TGeant/TVisManager.hh"
#include "TGeant/TCalibManager.hh"

#include "drs/obj/TTofCluster.hh"
#include "drs/obj/TTofCalibData.hh"

#include "drs/geom/TPetDetector.hh"
#include "drs/geom/TTofDetector.hh"

#include "drs/gui/TTofVisNode.hh"

#include "drs/alg/TPetClusterFinder.hh"

//_____________________________________________________________________________
TPetRecoModule::TPetRecoModule(const char* name, const char* title): 
  TStnModule(name,title)
{
  fListOfClusters = new TObjArray();
  fListOfClusters->SetName("TPetRecoModule::ListOfClusters");
  fPetReco        = new TPetClusterFinder();
  fCanvas         = 0;
}

//_____________________________________________________________________________
TPetRecoModule::~TPetRecoModule() {
}

//_____________________________________________________________________________
int TPetRecoModule::Debug(int Passed) {

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
int TPetRecoModule::BeginJob() {

  const char* name;
  TVisNode*   tof_node;

  TPetDetector* pet = TPetDetector::Instance();
  fTofDetector = pet->GetTofDetector();

  RegisterDataBlock("Vme1742DataBlock" ,"TVme1742DataBlock",&fVme1742DataBlock);

                                        // book histograms
  BookHistograms();
				        // passing debug flags
  TStnModule::BeginJob();

//-----------------------------------------------------------------------------
// visualization if necessary
//-----------------------------------------------------------------------------
  TVisManager* vm = fAna->GetVisManager();
  if (vm) {
    name = fTofDetector->GetName();
    tof_node = (TVisNode*) vm->GetListOfNodes()->FindObject(name);
    if (tof_node == 0) {
      tof_node = new TTofVisNode(name);
      vm->AddNode(tof_node);
    }
  }
  return 0;
}

//-----------------------------------------------------------------------------
// run-dependent initializations
//-----------------------------------------------------------------------------
int TPetRecoModule::BeginRun() {

  int rn = GetHeaderBlock()->RunNumber();

  TCalibManager* cm = TCalibManager::Instance();
  fTofDetector->InitCalibrations(rn,cm);

  if (rn == 12) {
    fPetReco->SetMinCell( 50);
    fPetReco->SetMaxCell(150);
  }
  else if (rn == 13) {
    fPetReco->SetMinCell( 50);
    fPetReco->SetMaxCell(150);
  }
  else if (rn == 14) {
    fPetReco->SetMinCell( 50);
    fPetReco->SetMaxCell(150);
  }

  return 0;
}

//_____________________________________________________________________________
int TPetRecoModule::FillEventHistograms(EventHist_t* Hist) {

  float        a, q, vmax, v1;
  int          ich, first_cell, icc;
  TTofModule*  mod;
  TTofData     *data, *ch;
  TTofChannelID  chid;

  TStnHeaderBlock* h = GetHeaderBlock();

  Hist->fRunNumber ->Fill(h->RunNumber());
  Hist->fEventNumber ->Fill(h->EventNumber());

//-----------------------------------------------------------------------------
// individual channels
//-----------------------------------------------------------------------------
  Hist->fQEvent->Reset();
  Hist->fVMaxEvent->Reset();

  for (int i=0; i<32; i++) Hist->fV[i]->Reset();

  for (int im=0; im<2; im++) {
    mod = fTofDetector->GetModule(im);

    for (int ix=0; ix<4; ix++) {
      for (int iy=0; iy<4; iy++) {

	data = mod->GetData(ix,iy);

	q    = data->GetQ();
	vmax = data->GetV1Max();

	Hist->fVMaxEvent->Fill(ix+im*4,iy,vmax);
	Hist->fQEvent->Fill(ix+im*4,iy,q);

	ich = fTofDetector->GetCalibData()->GetChannel(im,ix,iy);
	Hist->fQCh   [ich]->Fill(q);
	Hist->fVMaxCh[ich]->Fill(vmax);

	for (int cell=0; cell<1024; cell++) {
	  v1 = data->GetV1(cell);
	  Hist->fV[ich]->SetBinContent(cell+1,v1);
	  Hist->fU[ich]->Fill(cell+1,v1);
	}
      }
    }
//-----------------------------------------------------------------------------
// get highest channel
//-----------------------------------------------------------------------------
    ch = mod->GetOrderedChannel(0);

    Hist->fSeed[im]->Fill(ch->GetIX(),ch->GetIY());
    Hist->fQMax[im]->Fill(ch->GetQ());
    Hist->fVMax[im]->Fill(ch->GetV1Max());

    Hist->fVMaxVsChannel[im]->Fill(ch->GetChannelNumber(),ch->GetV1Max());
  }

  return 0;
}

//_____________________________________________________________________________
int TPetRecoModule::FillClusterHistograms(ClusterHist_t* Hist, TTofCluster* Cluster) {

  float a;
  int   ich, first_cell, icc;


  Hist->fCharge->Fill(Cluster->GetCharge()); 
  Hist->fPH->Fill(Cluster->GetPH());

  Hist->fSize->Fill(Cluster->GetNCrystals());

  return 0;
}

//-----------------------------------------------------------------------------
int TPetRecoModule::FillHistograms() {
  TTofCluster* cl;
//-----------------------------------------------------------------------------
// EVENT set 0: all    events 
//-----------------------------------------------------------------------------
  FillEventHistograms(&fHist.fEvent[0]);
//-----------------------------------------------------------------------------
// EVENT set 1: passed events
//-----------------------------------------------------------------------------
//  if (GetPassed()) FillEventHistograms(fHist.fEvent[1]);

  int ncl = fListOfClusters->GetEntriesFast();

  for (int i=0; i<ncl; i++) {
    cl = (TTofCluster*) fListOfClusters->At(i);

    FillClusterHistograms(&fHist.fCluster[0],cl);

    if (cl->GetModule() == 0) FillClusterHistograms(&fHist.fCluster[1],cl);
    if (cl->GetModule() == 1) FillClusterHistograms(&fHist.fCluster[2],cl);
  }

  return 0;
}

//_____________________________________________________________________________
int TPetRecoModule::Event(int ientry) {

  int            passed (0), id_word, loose_id_word, dir, nm, ncl;
  double         pt, chi2_tot;
  TTofModule     *module;
  TObjArray*     list;

  fVme1742DataBlock->GetEntry(ientry);

  fTofDetector->InitEvent(fVme1742DataBlock);

  fPetReco->SetRunEventNumber(GetHeaderBlock()->RunNumber(),
			      GetHeaderBlock()->EventNumber());

  nm = fTofDetector->GetNModules();

  for (int im=0; im<nm; im++) {
    module  = fTofDetector->GetModule(im);
    fPetReco->ProcessModule(fTofDetector,im);
//-----------------------------------------------------------------------------
// cache pointers to the reconstructed clusters in the common list - 
// useful for histogramming
//-----------------------------------------------------------------------------
    list = module->GetListOfClusters();
    ncl  = list->GetEntriesFast();
    for (int i=0; i<ncl; i++) {
      fListOfClusters->Add(list->At(i));
    }
  }
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
  TStnModule::SetPassed(passed);

  FillHistograms();
  Debug(passed);

  return 0;                    
}

//-----------------------------------------------------------------------------
int TPetRecoModule::EndJob() {
  return 0;
}

//-----------------------------------------------------------------------------
int TPetRecoModule::DrawPH() {

  if (fCanvas == 0) fCanvas = new_slide("ccc","ccc",1,3,550,1000);

  TPad* p1 = (TPad*) fCanvas->GetPrimitive("p1");

  TTofData*  ch;
  TTofModule* m;

  int        ich;

  p1->cd(1);
  fHist.fEvent[0].fQEvent->Draw("box");

  for (int im=0; im<2; im++) {
    p1->cd(im+2);

    m = fTofDetector->GetModule(im);

    for (int i=0; i<4; i++) {
      ich = m->GetOrderedChannel(i)->GetChannelNumber();
      if (i==0) fHist.fEvent[0].fV[ich]->Draw();
      else fHist.fEvent[0].fV[ich]->Draw("same");
    }
  }

  return 0;
}
