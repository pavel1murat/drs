///////////////////////////////////////////////////////////////////////////////
// analysis module for 2 channel PET stand
//
// mode=1: select 4l candidates from GEN7 re-reconstructed 3l data
// debug bits:
//   001 : all
//   002 : passed
//   003 : failed
//   004 : 
//   005 : events with inconsistent pixels.. 
//   006 : compton scatterers
//   007 : waveforms with long tails
///////////////////////////////////////////////////////////////////////////////
#include "TF1.h"
#include "TPad.h"
#include "TCanvas.h"
#include "TEnv.h"
#include "TSystem.h"
#include "TInterpreter.h"
#include "TLorentzVector.h"

#include "Stntuple/base/TStnDataset.hh"
#include "Stntuple/loop/TStnAna.hh"
#include "Stntuple/loop/TStnInputModule.hh"
#include "Stntuple/obj/TStnHeaderBlock.hh"
#include "Stntuple/val/stntuple_val_functions.hh"

#include "drs/obj/TVme1742DataBlock.hh"
#include "drs/ana/TPet2AnaModule.hh"
#include "drs/obj/TDrsGlobals.hh"
#include "drs/obj/TDrsFitResults.hh"

#include "TGeant/TVisNode.hh"
#include "TGeant/TVisManager.hh"
#include "TGeant/TCalibManager.hh"
#include "TGeant/TCalibRunRange.hh"

#include "drs/obj/TTofCluster.hh"
#include "drs/obj/TTofCalibData.hh"

#include "drs/geom/TPetDetector.hh"
#include "drs/geom/TTofDetector.hh"

#include "drs/gui/TTofVisNode.hh"

#include "drs/alg/TPetClusterFinder.hh"
#include "drs/ana/TGausFitAlgNew.hh"
#include "drs/ana/TStripLineFit.hh"
#include "drs/obj/TStripLine.hh"

#include "murat/plot/smooth.hh"

namespace {
  TTofData*  gT1;
};


//_____________________________________________________________________________
TPet2AnaModule::TPet2AnaModule(const char* name, const char* title): 
  TStnModule(name,title)
{
  fListOfClusters   = new TObjArray();
  fListOfClusters->SetName("TPet2AnaModule::ListOfClusters");
  fPetReco          = new TPetClusterFinder();
  fCanvas           = 0;
  fListOfChannels   = new TObjArray();
  fListOfAlgorithms = new TObjArray();

//   fNStripLines      = 1;
//   fListOfStripLines = new TObjArray();

//   fStripLineFit     = new TStripLineFit("sfit","sfit",32);

  for (int i=0; i<kNEventHistSets    ; i++) fHist.fEvent    [i] = 0;
  for (int i=0; i<kNClusterHistSets  ; i++) fHist.fCluster  [i] = 0;
  for (int i=0; i<kNStripLineHistSets; i++) fHist.fStripLine[i] = 0;
}

//_____________________________________________________________________________
TPet2AnaModule::~TPet2AnaModule() {
					// it should call ::Delete() internally
  delete fListOfAlgorithms;
//   delete fListOfStripLines;
//   delete fStripLineFit;
}

//_____________________________________________________________________________
int TPet2AnaModule::Debug(int Passed) {

  TStnHeaderBlock* header = GetHeaderBlock();
  TTofData*        ch;
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

  if (PrintLevel() == 4) {
    int nch = fTofDetector->GetNChannels();
    for (int ich=0; ich<nch; ich++) {
      ch = fTofDetector->GetData(ich);
      printf(" entry: %5i ich= %3i q=%10.3f vmax=%10.f q1=%10.3f",
	     TDrsGlobals::Instance()->GetEntry(),
	     ich,ch->GetQ(),ch->GetV1Max(),ch->GetQ1());
    }
  }

  if (PrintLevel() == 5) {
//-----------------------------------------------------------------------------
// events with inconsistent pixels
//-----------------------------------------------------------------------------
    int        ix1, ix2, iy1, iy2, idx, idy, idr2, ich1, ich2;
    TTofData  *ch1, *ch2;
    

    ich1 = GetTimeChannel(0);
    ich2 = GetTimeChannel(1);
					// require photopeak on one side

    if ( ((fTimeQ[0] > 450) && (fTimeQ[1] < 450)) ||
	 ((fTimeQ[0] < 450) && (fTimeQ[1] > 450))    ) {

      ch1  = fTofDetector->GetData(ich1);
      ch2  = fTofDetector->GetData(ich2);
      ix1  = ch1->GetIX();
      iy1  = ch1->GetIY();
      ix2  = ch2->GetIX();
      iy2  = ch2->GetIY();
      
      idx  = (ix1-ix2);
      idy  = (iy1+iy2-3);
      idr2 = idx*idx+idy*idy;
     
      if (idr2 > 2) {
	GetHeaderBlock()->Print(Form("entry: %5i q1(%2i)=%8.1f q1(%2i)=%8.1f ix1, ix2, iy1,iy2 = %2i %2i %2i %2i",
				     TDrsGlobals::Instance()->GetEntry(),
				     ich1, fTimeQ[0], ich2, fTimeQ[1],
				     ix1, ix2, iy1, iy2));
      }
    }

  }

  if (PrintLevel() == 6) {
//-----------------------------------------------------------------------------
// select compton scatterers
//-----------------------------------------------------------------------------
    int        ix0, ix1, ix2, iy0, iy1, iy2, idx, idy, idr2, ich1, ich2, compt_side;
    TTofData  *ch00, *ch01, *ch10, *ch11;
    TTofModule   *m0, *m1;
    
    m0 = fTofDetector->GetModule(0);
    m1 = fTofDetector->GetModule(1);

    ch00 = m0->GetOrderedChannel(0);
    ch10 = m1->GetOrderedChannel(0);

    ch01  = m0->GetOrderedChannel(1);
    ch11  = m1->GetOrderedChannel(1);
    compt_side = -1;
					// require photopeak on one side

    if ( ((ch00->GetQ1() > 420) && (ch10->GetQ1() < 420))) {
      // ch1 - the largest
      compt_side = 1;
    }
    else if ((ch00->GetQ1() < 420) && (ch10->GetQ1() > 420)) {
      compt_side = 0;
    }

    if (compt_side >= 0) {

      ix0  = ch00->GetIX();
      iy0  = ch00->GetIY();
      ix1  = ch10->GetIX();
      iy1  = ch10->GetIY();
    
      idx  = (ix1-ix2);
      idy  = (iy1+iy2-3);
      idr2 = idx*idx+idy*idy; 
     
      GetHeaderBlock()->Print(Form("entry: %5i side=0: q0=%8.1f q1=%8.1f side=1: q0=%8.1f q1=%8.1f ix1, ix2, iy1,iy2 = %2i %2i %2i %2i",
				   TDrsGlobals::Instance()->GetEntry(),
				   ch00->GetQ1(), ch01->GetQ1(), 
				   ch10->GetQ1(), ch11->GetQ1(), 
				   ix0, ix1, iy0, iy1));
    }
  }

  if (PrintLevel() == 7) {
//-----------------------------------------------------------------------------
// events with something on the tail
//-----------------------------------------------------------------------------
    int        ix1, ix2, iy1, iy2, idx, idy, idr2, ich1, ich2;
    TTofData  *ch1, *ch2;
    

    ich1 = GetTimeChannel(0);

    if ( ((fTimeQ[0] > 420) && (fTimeQ[1] < 600))) {

      ch1  = fTofDetector->GetData(ich1);

      for (int i=800; i<1024; i++) {
	if (ch1->GetV1(i) > 5) {
	  GetHeaderBlock()->Print();
	  break;

	    //Form("entry: %5i q1(%2i)=%8.1f q1(%2i)=%8.1f ix1, ix2, iy1,iy2 = %2i %2i %2i %2i",
	    //		     TDrsGlobals::Instance()->GetEntry(),
	    //		     ich1, fTimeQ[0], ich2, fTimeQ[1],
	    //		     ix1, ix2, iy1, iy2));
	}
      }
    }
  }
  if (PrintLevel() == 8) {
//-----------------------------------------------------------------------------
// events with something on the tail
//-----------------------------------------------------------------------------
    int        ix1, ix2, iy1, iy2, idx, idy, idr2, ich1, ich2;
    TTofData  *ch1, *ch2;
    
    TDrsFitAlgNew* gs2;
    TDrsFitResults *r1, *r2;


     ich1 = GetTimeChannel(0);
     ich2 = GetTimeChannel(1);


    //#include "drs/ana/scripts/modules.hh"

    gs2 = GetAlgorithm("gaus2");

    r1  = gs2->GetFitResults(ich1);
    r2  = gs2->GetFitResults(ich2);

    Double_t delta_t;
    delta_t = gs2->Dt(r1,r2,1);
 
    if (fabs(delta_t) > 2.5) {
      GetHeaderBlock()->Print(Form("entry %6i large delta_t %f",
				   TDrsGlobals::Instance()->GetEntry(),
				   delta_t));
    }
  }

  if (PrintLevel() == 9) {
//-----------------------------------------------------------------------------
// ch#21 run 118
//-----------------------------------------------------------------------------
    int        ix1, ix2, iy1, iy2, idx, idy, idr2, ich1, ich2;
    TTofData  *ch1, *ch2;
    
    TDrsFitAlgNew* gs2;
    TDrsFitResults *r1, *r2;


    ch1 = (TTofData*) fTofDetector->GetData(21);

    if ((ch1->fV1Max > 435) && (ch1->fV1Max < 445)) {
      GetHeaderBlock()->Print(Form("entry %6i large v1max %f",
				   TDrsGlobals::Instance()->GetEntry(),
				   ch1->fV1Max));
    }
  }

  if (PrintLevel() == 10) {
//-----------------------------------------------------------------------------
// events with something on the tail
//-----------------------------------------------------------------------------
    int        ix1, ix2, iy1, iy2, idx, idy, idr2, ich1, ich2;
    TTofData  *ch1, *ch2;
    
    TDrsFitAlgNew* gs2;
    TDrsFitResults *r1, *r2;


    ich1 = 20;
    ich2 = 21;

    //#include "drs/ana/scripts/modules.hh"

//     r1  = fStripLineFit->GetFitResults(ich1);
//     r2  = fStripLineFit->GetFitResults(ich2);

//     Double_t delta_t;
//     delta_t = (r2->fPar[0]);
 
//     TTofData *dt2 = r2->GetTofData();
//     //select the photopeak

//     if ( ( dt2->GetQ1() > 420) && ( dt2->GetQ1() < 600) ) {

//       if (delta_t > 0.2) {
// 	GetHeaderBlock()->Print(Form("entry %6i large delta_t %f",
// 				     TDrsGlobals::Instance()->GetEntry(),
// 				     delta_t));
//       }
//     }
 }
 
 return 0;
}


//_____________________________________________________________________________
int TPet2AnaModule::BeginJob() {

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
//-----------------------------------------------------------------------------
// init reconstruction algorithms
//-----------------------------------------------------------------------------
  TDrsFitAlgNew*   alg;
  
//-----------------------------------------------------------------------------
// add algorithm and channel folders
//-----------------------------------------------------------------------------
  TFolder* hf = (TFolder*) fFolder->FindObject("Hist");

  int nalg = fListOfAlgorithms->GetEntriesFast();
  for (int i=0; i<nalg; i++) {
    alg = GetAlgorithm(i);
    hf->Add(alg->GetFolder());
  }

  return 0;
}

//-----------------------------------------------------------------------------
// run-dependent initializations
//-----------------------------------------------------------------------------
int TPet2AnaModule::BeginRun() {

  TCalibRunRange* crr(0);

  char calib_name[200];

  int rn = GetHeaderBlock()->RunNumber();
  TCalibManager* cm = TCalibManager::Instance();

  fTofDetector->InitCalibrations(rn,cm);

  sprintf(calib_name,"config");

  crr = cm->GetRunRange("tof","config",rn);
  if (crr == 0) {
    Error("BeginRun",Form("missing TOF CONFIG table for run number %8i\n",rn));
    return -1;
  }
//-----------------------------------------------------------------------------
// if new run range, load the file
//-----------------------------------------------------------------------------
  if (fCalibRunRange != crr) {

    FILE* f  = fopen(crr->GetFilename(),"r");
    if (f == 0) {
      Error("Init",Form("missing file %s\n",crr->GetFilename()));
      return -2;
    }

    fCalibRunRange = crr;

    gInterpreter->LoadMacro(crr->GetFilename());
  }

  printf(" .... executing %s::Pet2AnaModule_init_run\n",crr->GetFilename());
  gInterpreter->ProcessLine(Form("Pet2AnaModule_init_run(%i);",rn));
  printf(" .... back from %s::Pet2AnaModule_init_run\n",crr->GetFilename());

  return 0;
}

//_____________________________________________________________________________
int TPet2AnaModule::FillEventHistograms(EventHist_t* Hist) {

  float          a, q, q1, vmax, v1, ph1, ph2, ped_width;
  int            ich, first_cell, icc, ich1, ich2, nch, ix1, ix2, iy1, iy2;
  int            igr1, igr2;
  TTofModule*    mod;
  TTofData       *data, *ch, *ch1, *ch2;
  TTofChannelID  chid;

  TStnHeaderBlock* h = GetHeaderBlock();

  Hist->fRunNumber ->Fill(h->RunNumber());
  Hist->fEventNumber ->Fill(h->EventNumber());
//-----------------------------------------------------------------------------
// individual channels
//-----------------------------------------------------------------------------
  Hist->fQ1Event  ->Reset();
  Hist->fVMaxEvent->Reset();

  for (int i=0; i<4; i++) {
    Hist->fVTrig[i]->Reset();
    for (int cell=0; cell<1024; cell++) {
      Hist->fVTrig[i]->SetBinContent(cell+1,fVme1742DataBlock->TriggerData(i,cell));
    }
  }

  for (int i=0; i<32; i++) Hist->fV[i]->Reset();

  for (int im=0; im<2; im++) {
    Hist->fQ1Module[im]->Reset();
    mod = fTofDetector->GetModule(im);

    for (int ix=0; ix<4; ix++) {
      for (int iy=0; iy<4; iy++) {
	data = mod->GetData(ix,iy);
	q1   = data->GetQ1();
	Hist->fQ1Event->Fill(ix+im*4,iy,q1);

	Hist->fQ1Module[im]->Fill(ix,iy,q1);
      }
    }
//-----------------------------------------------------------------------------
// get highest channel
//-----------------------------------------------------------------------------
    ch = mod->GetOrderedChannel(0);

    Hist->fSeed [im]->Fill(ch->GetIX(),ch->GetIY());
    Hist->fQMax [im]->Fill(ch->GetQ());
    Hist->fQ1Max[im]->Fill(ch->GetQ1());
    Hist->fVMax [im]->Fill(ch->GetV1Max());

    Hist->fVMaxVsChannel->Fill (ch->GetChannelNumber(),ch->GetV1Max());
    Hist->fQMaxVsChannel->Fill (ch->GetChannelNumber(),ch->GetQ ()   );
    Hist->fQ1MaxVsChannel->Fill(ch->GetChannelNumber(),ch->GetQ1()   );
  }

  nch = fTofDetector->GetNChannels();

  for (int ich=0; ich<nch; ich++) {
    data = fTofDetector->GetData(ich);
      
    q    = data->GetQ ();
    q1   = data->GetQ1();
    vmax = data->GetV1Max();
      //      Hist->fVMaxEvent->Fill(ix+im*4,iy,vmax);
    Hist->fQCh   [ich]->Fill(q);
    Hist->fQ1Ch  [ich]->Fill(q1);
    Hist->fVMaxCh[ich]->Fill(vmax);
    Hist->fPedestal[ich]->Fill(data->GetPedestal());

    ped_width = sqrt(data->GetChi2Ped ());
    Hist->fChi2Ped [ich]->Fill(ped_width);

    for (int cell=0; cell<1024; cell++) {
      v1 = data->GetV1(cell);
      Hist->fV[ich]->SetBinContent(cell+1,v1);
      Hist->fU[ich]->Fill(cell+1,v1);
    }
  }
//-----------------------------------------------------------------------------
// channels providing timing information
//-----------------------------------------------------------------------------
  ich1 = GetTimeChannel(0);
  ich2 = GetTimeChannel(1);

  igr1 = ich1/8;
  igr2 = ich2/8;

  ch1  = fTofDetector->GetData(ich1);
  ch2  = fTofDetector->GetData(ich2);
  ix1  = ch1->GetIX();
  iy1  = ch1->GetIY();
  ix2  = ch2->GetIX();
  iy2  = ch2->GetIY();

  Hist->fOcc2[ix1][iy1]->Fill(ix2,iy2);
  Hist->fOcc1[ix2][iy2]->Fill(ix1,iy1);

					// this is pulse height, defined in ProcessEvent
  ph1 = ch1->GetV1Max();
  ph2 = ch2->GetV1Max();

  Hist->fPH2VsPH1->Fill(ph1,ph2);
  Hist->fQ2VsQ1->Fill(ch1->GetQ1(),ch2->GetQ1());
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
  TDrsFitAlgNew* fa =  (TDrsFitAlgNew*) fListOfAlgorithms->At(0);
  TDrsFitResults *fr1, *fr2;
  int             if1, if2, iff0, iff1,  iff2, iff3;
  double          t01, t02;

  fr1  = fa->GetFitResults(ich1);
  fr2  = fa->GetFitResults(ich2);

  t01  = fr1->GetT0Fit();
  t02  = fr2->GetT0Fit();

  if1  = fVme1742DataBlock->FirstCell(igr1);
  if2  = fVme1742DataBlock->FirstCell(igr2);

  iff0  = fVme1742DataBlock->FirstCell(0);
  iff1  = fVme1742DataBlock->FirstCell(1);
  iff2  = fVme1742DataBlock->FirstCell(2);
  iff3  = fVme1742DataBlock->FirstCell(3);

  Hist->fT0VsFirstCell[0]->Fill(if1,t01);
  Hist->fT0VsFirstCell[1]->Fill(if2,t02);

  Hist->fDfc10VsEntry->Fill(TDrsGlobals::Instance()->GetEntry()/200.,iff1-iff0);
  Hist->fDfc20VsEntry->Fill(TDrsGlobals::Instance()->GetEntry()/200.,iff2-iff0);
  Hist->fDfc30VsEntry->Fill(TDrsGlobals::Instance()->GetEntry()/200.,iff3-iff0);
//-----------------------------------------------------------------------------
// study compton scattering
//-----------------------------------------------------------------------------
  TTofModule   *m0, *m1;
  double        t0, t1, t2, dt;
  TTofData      *ch00, *ch01, *ch10, *ch11;
    
  m0 = fTofDetector->GetModule(0);
  m1 = fTofDetector->GetModule(1);

  ch00 = m0->GetOrderedChannel(0);
  ch10 = m1->GetOrderedChannel(0);

  ch01  = m0->GetOrderedChannel(1);
  ch11  = m1->GetOrderedChannel(1);

  TDrsFitAlgNew* fitter = GetAlgorithm("gaus");

  if (ch00->GetQ1() > 420) {
    Hist->fP0Q1Max->Fill(ch10->GetQ1());
    Hist->fP0Q1Sum[0]->Fill(ch10->GetQ1()+ch11->GetQ1());

    if ((ch10->GetQ1() < 420) && (ch10->GetQ1()+ch11->GetQ1() > 420)) {
      Hist->fP0Q1Sum[1]->Fill(ch10->GetQ1()+ch11->GetQ1());
      ich1 = ch10->GetChannelNumber();
      ich2 = ch11->GetChannelNumber();
      t1   = fitter->GetFitResults(ich1)->GetT0Fit();
      t2   = fitter->GetFitResults(ich2)->GetT0Fit();
      dt   = (t2-t1)/5.;
      Hist->fP0C1Dt->Fill(dt);  // photopeak in channel #0, compton channel#1
    }
  }

  if (ch10->GetQ1() > 420) {
    Hist->fP1Q0Max->Fill(ch00->GetQ1());
    Hist->fP1Q0Sum[0]->Fill(ch00->GetQ1()+ch01->GetQ1());

    if ((ch00->GetQ1() < 420) && (ch00->GetQ1()+ch01->GetQ1() > 420)) {
      Hist->fP1Q0Sum[1]->Fill(ch00->GetQ1()+ch01->GetQ1());
      ich1 = ch00->GetChannelNumber();
      ich2 = ch01->GetChannelNumber();
      t1   = fitter->GetFitResults(ich1)->GetT0Fit();
      t2   = fitter->GetFitResults(ich2)->GetT0Fit();
      dt   = (t2-t1)/5.;
      Hist->fP1C0Dt->Fill(dt);
    }
  }

  return 0;
}

//_____________________________________________________________________________
int TPet2AnaModule::FillClusterHistograms(ClusterHist_t* Hist, TTofCluster* Cluster) {

  float a;
  int   ich, first_cell, icc;


  Hist->fCharge->Fill(Cluster->GetCharge()); 
  Hist->fPH->Fill(Cluster->GetPH());

  Hist->fSize->Fill(Cluster->GetNCrystals());

  return 0;
}

//_____________________________________________________________________________
int TPet2AnaModule::FillStripLineHistograms(StripLineHist_t* Hist, 
					    TStripLine* StripLine) {

  return 0;
}

//-----------------------------------------------------------------------------
int TPet2AnaModule::FillHistograms() {
  TTofCluster  *cl;
  TTofData     *ch1, *ch2;
  int          ich2;
//-----------------------------------------------------------------------------
// EVENT set 0: all    events 
//-----------------------------------------------------------------------------
  FillEventHistograms(fHist.fEvent[0]);

  ch1 = fTofDetector->GetModule(0)->GetOrderedChannel(0);
  ch2 = fTofDetector->GetModule(1)->GetOrderedChannel(0);
//-----------------------------------------------------------------------------
// EVENT set 1: passed events
//-----------------------------------------------------------------------------
  if (fPhotopeak == 1) { 
    FillEventHistograms(fHist.fEvent[1]);

    ich2 = ch2->GetChannelID()->GetNumber();
					// debug...
    if (ich2 == 24) {
      FillEventHistograms(fHist.fEvent[24]);
    }
  }

  int ncl = fListOfClusters->GetEntriesFast();

  for (int i=0; i<ncl; i++) {
    cl = (TTofCluster*) fListOfClusters->At(i);

    FillClusterHistograms(fHist.fCluster[0],cl);

    if (cl->GetModule() == 0) FillClusterHistograms(fHist.fCluster[1],cl);
    if (cl->GetModule() == 1) FillClusterHistograms(fHist.fCluster[2],cl);
  }
//-----------------------------------------------------------------------------
// algorithm histograms
//-----------------------------------------------------------------------------
  int nalg = fListOfAlgorithms->GetEntriesFast();

  TDrsFitAlgNew* alg;

  for (int i=0; i<nalg; i++) {
    alg = GetAlgorithm(i);
//-----------------------------------------------------------------------------
// for time measurement to make sense, an algorithm needs 2 used channels
//-----------------------------------------------------------------------------
    if ((alg->GetTimeChannelNumber(0) >= 0) && (alg->GetTimeChannelNumber(1) >= 0)) {
      alg->FillHistograms();
    }
  }
  // strip line

  //  fStripLineFit->FillHistograms();
  //   FillStripLineHistograms();

  return 0;
}


//-----------------------------------------------------------------------------
int TPet2AnaModule::ProcessStripLine(TStripLine* StripLine) {
  // TStripLineFit*  SFit; 
  TTofData* T2;

  //----Range of fit
  double Smin, Smax;
  Smin = 180;
  Smax = 300;
  //----------------

  // Smooth the First Channel  
  gT1 = StripLine->GetTofData(0);
  smooth::smooth_hist(gT1->fHistShape,Smin,Smax);

  // Fit the second channel with the parametrized first one.
  T2 = StripLine->GetTofData(1);
//   fStripLineFit->Fit(T2);

//   fStripLineFit->GetFitResults(20)->fTofData = gT1;

  //  fStripLineFit->Display();
  return 0;
}


//_____________________________________________________________________________
int TPet2AnaModule::Event(int ientry) {

  int            passed (0), id_word, loose_id_word, dir, nm, ncl, nch, ich;
  double         pt, chi2_tot;
  TTofModule     *module;
  TTofData       *channel;      
  TObjArray*     list;

  fVme1742DataBlock->GetEntry(ientry);
  
  TDrsGlobals::Instance()->SetEntry(ientry);
  TDrsGlobals::Instance()->SetRunNumber(GetHeaderBlock()->RunNumber());
  TDrsGlobals::Instance()->SetEventNumber(GetHeaderBlock()->EventNumber());
  TDrsGlobals::Instance()->SetVme1742DataBlock(fVme1742DataBlock);

  fTofDetector->InitEvent(fVme1742DataBlock);

  fPetReco->SetRunEventNumber(GetHeaderBlock()->RunNumber(),
			      GetHeaderBlock()->EventNumber());

  nm = fTofDetector->GetNModules();

  for (int im=0; im<nm; im++) {
    module  = fTofDetector->GetModule(im);
    fPetReco->ProcessModule(fTofDetector,im);
    
    channel          = module->GetOrderedChannel(0);
    ich              = channel->GetChannelID()->GetNumber();
    fTimeChannel[im] = ich;
    fTimeQ[im]       = channel->GetQ1();
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

//----------------------------------------------------------------------------
//Initialize the StripLines
//----------------------------------------------------------------------------
//   TStripLine* sl;

//   sl = new TStripLine();

//   sl->fTofData[0] = fTofDetector->GetData(20);
//   sl->fTofData[1] = fTofDetector->GetData(21);

//   fListOfStripLines->Add(sl);
//-----------------------------------------------------------------------------
// loop over the algorithms and do fitting
//-----------------------------------------------------------------------------
  int nalg = fListOfAlgorithms->GetEntriesFast();

  TDrsFitAlgNew* alg;

  for (int i=0; i<nalg; i++) {
    alg = (TDrsFitAlgNew*) fListOfAlgorithms->UncheckedAt(i);

    for (int im=0; im<nm; im++) {
      module  = fTofDetector->GetModule(im);
//-----------------------------------------------------------------------------
// TimeChannelNumber is an unfortunate name, 
// supposed to mean 'channel used for the timing measurements'
//-----------------------------------------------------------------------------
      alg->SetTimeChannelNumber(im,-1);

      for (int ich=0; ich<module->GetNChannels(); ich++) {
	channel = module->GetOrderedChannel(ich);
	if (channel->GetQ1() > 30.) {
	  alg->Fit(channel);
	  if (ich == 0) {
	    alg->SetTimeChannelNumber(im,channel->GetChannelID()->GetNumber());
	  }
	}
      }
					// now handle trigger channels

      for (int ich=0; ich<module->GetNTriggerChannels(); ich++) {
	channel = module->GetTriggerChannel(ich);
	alg->Fit(channel);
      }
    }
  }
//-----------------------------------------------------------------------------
// process strip lines
//-----------------------------------------------------------------------------
//   for (int i=0; i<fNStripLines; i++) {
//     sl = (TStripLine*) fListOfStripLines->At(i);
//     ProcessStripLine(sl);
//   }
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
  fPhotopeak = (((fTimeQ[0] > 420.) && (fTimeQ[0] < 600.)) && 
		((fTimeQ[1] > 420.) && (fTimeQ[1] < 600.))    );

  passed     = ((fTimeQ[0] > 420.) && (fTimeQ[1] > 420.));
  TStnModule::SetPassed(passed);

  FillHistograms();
  Debug(passed);

  return 0;                    
}

//-----------------------------------------------------------------------------
int TPet2AnaModule::EndJob() {
  return 0;
}

//-----------------------------------------------------------------------------
int TPet2AnaModule::DrawPH() {
  TTofData*  ch;
  TTofModule* m;

  int        ich;
  TPad       *p1, *p2, *p3, *p4;

  if (fCanvas == 0) {
    fCanvas = new TCanvas("ccc","ccc",1100,750);
    p1 = new TPad("p1","p1",0.01,0.51,0.69,0.99,-1,1,1);
    p2 = new TPad("p2","p2",0.71,0.51,0.99,0.99);
    p3 = new TPad("p3","p3",0.01,0.01,0.69,0.49);
    p4 = new TPad("p4","p4",0.71,0.01,0.99,0.49,-1,1,1);
  }
  else {
    p1 = (TPad*) fCanvas->GetPrimitive("p1");
    p2 = (TPad*) fCanvas->GetPrimitive("p2");
    p3 = (TPad*) fCanvas->GetPrimitive("p3");
    p4 = (TPad*) fCanvas->GetPrimitive("p4");
  }

  p1->cd(); p1->Clear();
  m = fTofDetector->GetModule(0);
  for (int i=0; i<4; i++) {
    ich = m->GetOrderedChannel(i)->GetChannelNumber();
    if (i==0) fHist.fEvent[0]->fV[ich]->Draw();
    else fHist.fEvent[0]->fV[ich]->Draw("same");
  }
  fCanvas->cd();
  p1->Draw();

  p3->cd(); p3->Clear();
  m = fTofDetector->GetModule(1);
  for (int i=0; i<4; i++) {
    ich = m->GetOrderedChannel(i)->GetChannelNumber();
    if (i==0) fHist.fEvent[0]->fV[ich]->Draw();
    else fHist.fEvent[0]->fV[ich]->Draw("same");
  }
  fCanvas->cd();
  p3->Draw();

//-----------------------------------------------------------------------------
// now draw occupancy plots
//-----------------------------------------------------------------------------
  p2->cd(); p2->Clear();
  TH2F* h2;

  h2 = fHist.fEvent[0]->fQ1Module[0];
  h2->SetStats(0);
  h2->Draw("box");
  fCanvas->cd();
  p2->Draw();

  p4->cd(); p4->Clear();
  h2 = fHist.fEvent[0]->fQ1Module[1];
  h2->SetStats(0);
  h2->Draw("box");
  fCanvas->cd();
  p4->Draw();

  fCanvas->Modified();
  fCanvas->Update();

  return 0;
}
