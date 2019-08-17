///////////////////////////////////////////////////////////////////////////////
// fPhETravelTime: time, needed for electron to travel from a photocathode 
//                 to the first dinode (MCP)...
//
///////////////////////////////////////////////////////////////////////////////
#include "ana/TPmtSim.hh"
#include "TScintillator.hh"
#include "TPMT.hh"

ClassImp(TPmtSim)

//-----------------------------------------------------------------------------
TPmtSim::~TPmtSim() {
  delete fRn;
}


//-----------------------------------------------------------------------------
// Scintillator Options:lyso,lso, labr3, cebr3, baf2
// Pmt Options: R9800,STM,MPPC, Photek240   



TPmtSim::TPmtSim(const char* ScintillatorName, const char* Pmt) : 
  TNamed(ScintillatorName,Pmt) {

  fScintillator = new TScintillator(ScintillatorName); //choose scintillator 

  fPmt          = new TPMT(Pmt);
  
// simulate readout of two channels
  fNChannels    = 2;
					// need +1
  fUsedChan[0]  = 1;
  fUsedChan[1]  = 2;
  fUsedChan[2]  = 3;
  fUsedChan[3]  = 4;
  
  fTc1          = 0;
  
  fOutputFn     = "pmt_sim.root";

fGain         = 100.;
fPhETravelTime = 20;			// to move pulse into bin=100

BookHistograms(&fHist);

fWriteOutput = 1;

fRn  = new TRandom3();


}
  




//-----------------------------------------------------------------------------
void TPmtSim::BookHistograms(Hist_t* Hist) {

  Hist->h_npe[0]   = new TH1D("h_npe_0","N(pe)[0]",360,4000,40000);
  Hist->h_npe[1]   = new TH1D("h_npe_1","N(pe)[1]",360,4000,40000);

  Hist->h_decay[0] = new TH1D("h_dt_0","decay time[0]",1000,0,200);
  Hist->h_decay[1] = new TH1D("h_dt_1","decay time[1]",1000,0,200);

  Hist->h_te[0]   = new TH1D("h_te_0"  ,"electron time[0]",1024,0,204.8);
  Hist->h_te[1]   = new TH1D("h_te_1"  ,"electron time[1]",1024,0,204.8);

  Hist->h_te_ev[0] = new TH1D("h_te_ev_0"  ,"event electron time[0]",1000,-5,195);
  Hist->h_te_ev[1] = new TH1D("h_te_ev_1"  ,"event electron time[1]",1000,-5,195);

  Hist->h_tf[0] = new TH1D("h_tf_0","first electron time[0]",150,0,30);
  Hist->h_tf[1] = new TH1D("h_tf_1","first electron time[1]",150,0,30);

  Hist->h_t10[0] = new TH1D("h_t10_0","CFD 10% time[0]",500,50,150);
  Hist->h_t10[1] = new TH1D("h_t10_1","CFD 10% time[1]",500,50,150);

  Hist->h_dtpmt[0] = new TH1D("h_dtpmt_0","PMT term [0]",500,15,25);
  Hist->h_dtpmt[1] = new TH1D("h_dtpmt_1","PMT term [1]",500,15,25);

  Hist->h_prob[0] = new TH1D("h_prob_0","Prob[0]",100,0,1);
  Hist->h_prob[1] = new TH1D("h_prob_1","Prob[1]",100,0,1);

  Hist->h_delta_t   = new TH1D("h_delta_t"  ,"delta_t"  ,1000,-2.5,2.5);
  Hist->h_delta_t10 = new TH1D("h_delta_t10","delta_t10",1000,-2.5,2.5);
  
  return;
}


//-----------------------------------------------------------------------------
int TPmtSim::SimulateChannel(int Channel) {

  double t0, prob, dt_pmt, te, tse, npe;   //t0 = t when optical photon released, 

  int  ich, cell; 

  ich = Channel;
  
  npe = fScintillator-> GetMeanNpe(); //average light yield for given scintillator * e annihilation energy

  fNPe[ich] = fRn->Poisson(npe);     // N(pe) for a given event in a given channel

  fHist.h_npe[ich]->Fill(fNPe[ich]);

  fTFirst[ich] =  999.;              // first electron arrival time
  fTCfd10[ich] = -999.;              // 10% CFD
      
  fHist.h_te[ich]->Reset();
//-----------------------------------------------------------------------------
// simulate one channel, 
// 'npe[ich' excited atoms generate npe[ich] optical photons
//-----------------------------------------------------------------------------
  for (int i=0; i<fNPe[ich]; i++) {
//-----------------------------------------------------------------------------
// optical photon emission time
//-----------------------------------------------------------------------------
    double decay_time, wave_length;
    fScintillator->GetPhotonProperties(&decay_time,&wave_length);
    t0 = fRn->Exp(decay_time);

    fHist.h_decay[ich]->Fill(t0);
//-----------------------------------------------------------------------------
// simulate detection efficiency. Need to simulate it as lowering efficiency
// changes the simulated pulse.
//-----------------------------------------------------------------------------
     
    fPhotoEfficiency = fPmt->GetPDE(wave_length);
    prob = fRn->Rndm(i);
    fHist.h_prob[ich]->Fill(prob);
	
    if (prob < fPhotoEfficiency) { 

// photon makes a photoelectron, calculate timing of its pulse, fPhETravelTime;

      dt_pmt = fPhETravelTime ; // + fRn->Gaus(0,fPmtJitter);

      fHist.h_dtpmt[ich]->Fill(dt_pmt);

// spread of transit times from the cathode to the first stage

// so far ignore the spread of the photon travel times inside 
// the scintillator, take into account later

      te = t0 + dt_pmt;
      fHist.h_te_ev[ich]->Fill(te);

// first photon arrival time
      
      if (te < fTFirst[ich]) {
	fTFirst[ich] = te;
      }
    
//-----------------------------------------------------------------------------
// assuming each original photoelectron generates a cascade of 10^3:
//-----------------------------------------------------------------------------
      int ne = (int) fGain;
      for (int ie=0; ie<ne; ie++) {
       	double pmtser = fPmt->GetSPTR();   // "electron propagation time"
  	double jitter = fPmt->GetJitter(); // strawman single electron response
	tse = te+pmtser+jitter;
	fHist.h_te[ich]->Fill(tse);
					   // convert time into a bin (200ps)
	cell          = (int) (tse*5.);
	if ((cell >=0) && (cell < 1024)) { // otherwise - out of range
	  fV[ich][cell] += 1;	           // 1mv/electron, renormalize
	}
	else if ((cell)<0){
	  printf("%i %f \n",cell,jitter);
	}
      }
    }
  }
  return 0;
}

//-----------------------------------------------------------------------------




int TPmtSim::GenerateEvents(int NEvents=1000) {

  // generate number of photoelectrons

  int        npe[2];

  double     t0, tf[2], tse, dt_pmt, te, delta_t, t10[2], delta_t10;

  //  TRandom3   rn;

  float** branch;

  // 

  if (fWriteOutput) {

    fOutputFile = TFile::Open(fOutputFn.Data(),"recreate");
    
    fOutputTree = new TTree("pulse", Form("MC by TPmtSim: %s fPhotoefficiency = %7.4f",
					  fScintillator->GetName(),
					  fPhotoEfficiency));
    
    fOutputTree->Branch("event"   , &fEventNumber, "event/I"     );
    fOutputTree->Branch("tc1"     , &fTc1        , "tc1/I"       );
    fOutputTree->Branch("b1_t"    , &fT          , "b1_t[1024]/F");
    
// P.Murat: so far assume 4 channels at most..., but...

    fOutputTree->Branch("nch"     , &fNChannels  , "nch/I");
    fOutputTree->Branch("usedchan", &fUsedChan   , "usedchan[4]/I");

    fOutputTree->Branch("b1_c1", (float**) &fV[0], "b1_c1[1024]/F");
    fOutputTree->Branch("b1_c2", (float**) &fV[1], "b1_c2[1024]/F");
  }
					// do not really need times...
  for (int i=0; i<1024; i++) {
    fT[i] = i;
  }

  for (int iev=0; iev<NEvents; iev++) {
    fEventNumber = iev;
//-----------------------------------------------------------------------------
// simulate one event, we have 2 time measurement channels (PMT's)
//-----------------------------------------------------------------------------
    for (int ich=0; ich<2; ich++) {
      SimulateChannel(ich);
//-----------------------------------------------------------------------------
// done with the simulation of a single 'PMT' channel, calculate t(10%)
// make sure it is done only for 25% of the photons which produced the 
// photoelectrons
//-----------------------------------------------------------------------------
      if (fTFirst[ich] > -100) {
	double qmax = -1;
	for (int i=1; i<=1024; i++) {
	  double q = fV[ich][i];
	  if (q > qmax) qmax=q;
	}

	for (int i=0; i<=1024; i++) {
	  double q = fV[ich][i];
	  if (q > qmax*0.1) {
	    fTCfd10[ich] = i+0.5;
	    break;
	  }
	}
      }
      
      fHist.h_tf [ich]->Fill(fTFirst[ich] );
      fHist.h_t10[ich]->Fill(fTCfd10[ich]);
    }
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
    // fTFirst[0]=tf[0];
    // fTFirst[1]=tf[1];
    
    if ((fTFirst[0] > -100.) && (fTFirst[1] > -100.)) {
      delta_t   = fTFirst [0]-fTFirst [1];
      delta_t10 = fTCfd10[0]-fTCfd10[1];
      
     
      fHist.h_delta_t->Fill(delta_t);
      fHist.h_delta_t10->Fill(delta_t10);
    }
//-----------------------------------------------------------------------------
// if writing output is requested, do it
// renormalize pulse height
//-----------------------------------------------------------------------------
    if (fWriteOutput) {
      for (int ich=0; ich<2; ich++) {
	for (int i=0; i<1024; i++) {
	  fV[ich][i] = -fV[ich][i]/100.;
	}
      }
      fOutputTree->Fill();
    }
  }
  
  fHist.h_delta_t->Draw();

//-----------------------------------------------------------------------------
// if output requested, write the output file and close it
//-----------------------------------------------------------------------------
  if (fWriteOutput) {
    fOutputFile->Write();
    fOutputFile->Close();
    delete fOutputFile;
  }
  return 0;
}
