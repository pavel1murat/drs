//-----------------------------------------------------------------------------
#include "TInterpreter.h"
#include "Stntuple/scripts/global_vars.h"
#include "drs/ana/scripts/modules.hh"

def_name drs4_001("drs_example");
def_name drs4_002("drs_calib");
def_name drs4_003("drs_reco");
def_name drs4_004("drs_convert");
def_name drs4_004("drs_convert_drsorc");
def_name drs4_005("drs_pet");
def_name drs4_006("drs_pet2");
def_name drs4_007("drs_spline");
def_name drs4_008("drs_gausavg");
def_name drs4_009("drs_gaus");
def_name drs4_010("drs_exp1");

drsana* x;

//-----------------------------------------------------------------------------
// convert an oscilloscope file into a ROOT format - why would we need entries?
//-----------------------------------------------------------------------------
void oscbin(const char* ifname, Int_t entry1=0, Int_t entry2=0) {
  DRS4bin* drs4bin = new DRS4bin(ifname);
  drs4bin->Convert2root(entry1,entry2);
}


//-----------------------------------------------------------------------------
// convert an oscilloscope file into a ROOT format - why would we need entries?
//-----------------------------------------------------------------------------
void convert_to_root(const char* ifname, Int_t entry1=0, Int_t entry2=0) {
  DRS4bin* drs4bin = new DRS4bin(ifname);
  drs4bin->Convert2root(entry1,entry2);
}


//-----------------------------------------------------------------------------
// so far channels 2 and 3 have been used only in UCNa22ham2_1.root
// for the test stand data taking we use channels 0 and 1
//-----------------------------------------------------------------------------
void loop_time(const char* Filename = "data/aaaa_001.root") {
  x = new drsana(Filename);
  x->LoopTime();
  TGausFitAlg* ga = (TGausFitAlg*) x->GetAlgorithm("gaus");
  ga->fHist.fDt[2]->Fit("gaus");
  tb();
}


//-----------------------------------------------------------------------------
// this is just an example
// channels 2 and 3 have been used only in UCNa22ham2_1.root
//-----------------------------------------------------------------------------
void display() {
  x = new drsana("data/UCNa22ham2_1.root",2,3);
  int i = 0;
  x->LoopTime(1);
  x->ProcessEvent(i++); x->Fit_time(2); x->Fit_time(3); x->Display();
}



//-----------------------------------------------------------------------------
// UCNa22ham2_1.root optimizations: clipping capasitor
// for the test stand data taking we use channels 0 and 1
//-----------------------------------------------------------------------------
void loop_mppc_clip(const char* Filename = "data/UCNa22ham2_1.root") {
					// 
  x = new drsana(Filename);

  x->fBaseOffset[2]  =  10;
  x->fBaseOffset[3]  = -20;
					// pulse height fraction used in the 
					// leading edge fit
  x->fMinFraction[2] = 0.10;
  x->fMinFraction[3] = 0.15;
					// 
  x->fMaxFraction[2] = 0.40;
  x->fMaxFraction[3] = 0.45;
					// non-scattered photon energy cutoffs
  x->fMinPulse[2]    =  40.;
  x->fMinPulse[3]    = 250.;
					// even-odd shift , old DRS4 ?
  x->fEvenShift[2]   = 2.41;
  x->fEvenShift[3]   = 2.34;

  x->LoopTime();

  x->fHist.fDt[2]->Fit("gaus","w");
}

//-----------------------------------------------------------------------------
void loop_mppc_data(const char* Filename = "data/2011-12-02-mppc-na22.root") {
					// 
  x = new drsana(Filename);
					// pulse height fraction used in the 
					// leading edge fit
  x->fMinFraction[0] = 0.03;
  x->fMinFraction[1] = 0.03;
					// 
  x->fMaxFraction[0] = 0.13;
  x->fMaxFraction[1] = 0.13;
					// non-scattered photon energy cutoffs
  x->fMinPulse[0]    = 160.;
  x->fMinPulse[1]    = 160.;

  x->LoopTime();

  x->fHist.fDt[2]->Fit("gaus","w");
}

//-----------------------------------------------------------------------------
void next() {
  x->ProcessEvent(i++); 
  x->Display(); 
  x->DisplayShape(); 
  printf("%f  %f %f %f \n",
	 x->fChannel[0]->fEPar[2],
	 x->fChannel[2]->fEPar[2],
	 x->fChannel[0]->fQ,
	 x->fChannel[2]->fQ);
}

//-----------------------------------------------------------------------------
// convert binary VME1742 file into STNTUPLE
//-----------------------------------------------------------------------------
void drs_convert(const char* Dataset, int RunNumber) {
  char ifile[200], ofile[200];

  TVme1742* vme = TVme1742::Instance();
  vme->SetRunNumber(RunNumber);

  sprintf(ifile,"/data/pet01/a/datasets/%s/%s.%06i",Dataset,Dataset,RunNumber);

  TString ods = Dataset;
  ods[3] = 's';
  
  sprintf(ofile,"/data/pet01/a/datasets/%s/%s.%06i",ods.Data(),ods.Data(),RunNumber);

  int rc = vme->ConvertToStntuple(ifile,ofile);
}

//-----------------------------------------------------------------------------
// convert binary DRSOSC file ('.osc') into STNTUPLE
//-----------------------------------------------------------------------------
void drs_convert_drsosc(const char* InputFile, int RunNumber) {

  TDrs4EvalBoard* drsosc = TDrs4EvalBoard::Instance();
  drsosc->SetRunNumber(RunNumber);

  TString ofile = InputFile;
  if (ofile.Index('/') >= 0) ofile.Replace(0,ofile.Last('/')+1,"./");

  ofile.Replace(ofile.Index(".osc"),4,".stn");

  int rc = drsosc->ConvertToStntuple(InputFile,ofile.Data(),RunNumber);
}

//-----------------------------------------------------------------------------
void drs_example() {
  m_pet = (TPetAnaModule*) g.x->AddModule("TPetAnaModule",0,"PetAna","PetAna");
}

//-----------------------------------------------------------------------------
void drs_calib() {
  m_pet = (TPetAnaModule*) g.x->AddModule("TPetAnaModule",0,"PetAna","PetAna");
}

//-----------------------------------------------------------------------------
void drs_reco() {
  m_prc = (TPetRecoModule*) g.x->AddModule("TPetRecoModule",0,"PetReco","PetReco");
}


//-----------------------------------------------------------------------------
// no time fit
//-----------------------------------------------------------------------------
void drs_pet() {
  m_pet2 = (TPet2AnaModule*) g.x->AddModule("TPet2AnaModule",0,"Pet2Ana","Pet2Ana");

  TTofDetector* tof = TPetDetector::Instance()->GetTofDetector();

  TGausFitAlgNew* alg = new TGausFitAlgNew("gaus",
					   "gaussian fit to the pulse shape",
					   tof->GetNChannels());
}

//-----------------------------------------------------------------------------
void drs_pet2() {
  m_pet2 = (TPet2AnaModule*) g.x->AddModule("TPet2AnaModule",0,"Pet2Ana","Pet2Ana");
  // alg = new TKneeFitAlg("knee","knee fit",fListOfChannels);
  // fListOfAlgorithms->Add(alg);

  // alg = new TExpoFitAlg("expo","expo fit",fListOfChannels);
  // fListOfAlgorithms->Add(alg);

  TTofDetector* tof = TPetDetector::Instance()->GetTofDetector();

  TDrsFitAlgNew* alg;

  alg = new TGausFitAlgNew("gaus","gaussian fit",tof->GetNChannels());
  m_pet2->AddAlgorithm(alg);

  alg = new TPolnFitAlgNew("poln","pol(2) fit",tof->GetNChannels());
  m_pet2->AddAlgorithm(alg);

}

//-----------------------------------------------------------------------------
void drs_spline() {
  m_pet2 = (TPet2AnaModule*) g.x->AddModule("TPet2AnaModule",0,"Pet2Ana","Pet2Ana");
  // alg = new TKneeFitAlg("knee","knee fit",fListOfChannels);
  // fListOfAlgorithms->Add(alg);

  // alg = new TExpoFitAlg("expo","expo fit",fListOfChannels);
  // fListOfAlgorithms->Add(alg);

  TTofDetector* tof = TPetDetector::Instance()->GetTofDetector();

  TDrsFitAlgNew* alg;

  alg = new TGausFitAlgNew("gaus","gaussian fit",tof->GetNChannels());
  m_pet2->AddAlgorithm(alg);

  alg = new TSplineFitAlgNew("spln","Spline fit",tof->GetNChannels());
  m_pet2->AddAlgorithm(alg);

  //  alg = new TGausSingleFit("gaus2","Avg. Gaus fit",tof->GetNChannels());
  // m_pet2->AddAlgorithm(alg);

}


void drs_gausavg() {
  m_pet2 = (TPet2AnaModule*) g.x->AddModule("TPet2AnaModule",0,"Pet2Ana","Pet2Ana");

  TTofDetector* tof = TPetDetector::Instance()->GetTofDetector();

  TDrsFitAlgNew* alg;

  alg = new TGausFitAlgNew("gaus","Gaussian fit",tof->GetNChannels());
  m_pet2->AddAlgorithm(alg);

  alg = new TGausSingleFit("gaus2","Avg. Gaus fit",tof->GetNChannels());
   m_pet2->AddAlgorithm(alg);

//   alg = new TStripLineFit("SLine","Strip Line Fit",tof->GetNChannels());
//   m_pet2->AddAlgorithm(alg);

  m_pet2->SetPrintLevel(8);

}

//------------
//For when we need to add the StripLine Fit to the list of fitters.
//------------


// void drs_strip() {
//   m_pet2 = (TPet2AnaModule*) g.x->AddModule("TPet2AnaModule",0,"Pet2Ana","Pet2Ana");
  
//   TTofDetector *tof = TPetDetector::Instance()->GetTofDetector();
  
//   TDrsFitAlgNew* alg;
  
//   alg = new TGausFitAlgNew("gaus","Gaussian fit",tof->GetNChannels());
//   m_pet2->AddAlgorithm(alg);
  

  
//   m_pet2->SetPrintLevel(9);

// }

//-----------------------------------------------------------------------------
void drs_gaus() {
  m_pet2 = (TPet2AnaModule*) g.x->AddModule("TPet2AnaModule",0,"Pet2Ana","Pet2Ana");
  // alg = new TKneeFitAlg("knee","knee fit",fListOfChannels);
  // fListOfAlgorithms->Add(alg);

  // alg = new TExpoFitAlg("expo","expo fit",fListOfChannels);
  // fListOfAlgorithms->Add(alg);

  TTofDetector* tof = TPetDetector::Instance()->GetTofDetector();

  TDrsFitAlgNew* alg;

  alg = new TGausFitAlgNew("gaus","gaussian fit",tof->GetNChannels());
  m_pet2->AddAlgorithm(alg);

}

//-----------------------------------------------------------------------------
void drs_exp1() {
  m_pet2 = (TPet2AnaModule*) g.x->AddModule("TPet2AnaModule",0,"Pet2Ana","Pet2Ana");
  // alg = new TKneeFitAlg("knee","knee fit",fListOfChannels);
  // fListOfAlgorithms->Add(alg);

  // alg = new TExpoFitAlg("expo","expo fit",fListOfChannels);
  // fListOfAlgorithms->Add(alg);

  TTofDetector* tof = TPetDetector::Instance()->GetTofDetector();

  TDrsFitAlgNew* alg;

  alg = new TGausFitAlgNew("gaus","gaussian fit",tof->GetNChannels());
  m_pet2->AddAlgorithm(alg);

  alg = new TExp1FitAlgNew("exp1","exp1 fit",tof->GetNChannels());
  m_pet2->AddAlgorithm(alg);

}

