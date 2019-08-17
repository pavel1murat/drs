#include <math.h>

#include "TMinuit.h"

#include "drs/geom/TPetDetector.hh"
#include "drs/geom/TPetGeometry.hh"
#include "drs/geom/TTofModule.hh"
#include "drs/geom/TTofDetector.hh"

#include "drs/obj/TTofCalibData.hh"
#include "drs/alg/TPetClusterFinder.hh"
#include "drs/obj/TVme1742DataBlock.hh"

#include "drs/base/TDrsUtils.hh"

ClassImp(TPetClusterFinder)

  namespace {
    struct FitPar_t {
      int    fView;			// 0: strips, 1: wires
      int    fSide;
      int    fWedge;
      int    fFirst;
      int    fLast;
      int    fPeak[128];
      int    fNMerged;
      int*   fAdc;
      double fChi2;			// calculated chi2
      int    fCharge;			// total charge in ADC counts
    };

    FitPar_t   fgFitPar;
    TPetClusterFinder::Parameters_t* fgPar;
  }

//_____________________________________________________________________________
TPetClusterFinder::TPetClusterFinder(): kLargeChi2(1.e10) {

  fFitter      = NULL;

  fRunNumber   = -1;
  fEventNumber = -1;
				// to be initialized from the outside
  fTofDetector = 0;
  fVerbose     = 0;

  for (int i=0; i<2; i++) {
    fListOfClusters[i] = new TObjArray();
  }

  fMinCell     =  50;
  fMaxCell     = 150;

  fMinThreshold =  20;  // above pedestal...
  fQScale       = 250.; // 1/250 = 1/50 [Ohm] x 0.2 [ns]
}



//_____________________________________________________________________________
TPetClusterFinder::~TPetClusterFinder() {
  delete [] fListOfClusters;
}




//-----------------------------------------------------------------------------
Int_t TPetClusterFinder::DoClustering(Int_t*      Data, 
				      Parameters_t* Par,
				      TClonesArray* ListOfClusters) {
  // `Data`: pulse heights
  // `X`   : coordinates of the strips
  // this routine doesn't clear list of clusters but assumes that it is 
  // properly initialized !

  return 0;
}


//-----------------------------------------------------------------------------
// after pedestals are subtracted, work only with the RecoData
//-----------------------------------------------------------------------------
Int_t TPetClusterFinder::SubtractPedestals(TTofData* Data) {
  static int nreports(0);

  double    sum, qn, q, q1, q2, v, vmax, pedestal, slope, gain, t0, y, ey, mean, chi2;
  double    ped_mean, ped_sigma;
  int       ncells, cmax, ix, iy, ich, nch, found, min_cell, max_cell;
  int       pulse_integration_window;
  TTofData  *ch1, *ch2, *ch;
//-----------------------------------------------------------------------------
// determine pedestals
//-----------------------------------------------------------------------------
  Data->SetQ (0.);
  Data->SetQ1(0.);
  if (Data->GetUsed() == 0) return 0;
//-----------------------------------------------------------------------------
// get calibration constants
// 5 GHz: 5 channels / ns, pulse_integration_window - in units of channels
//-----------------------------------------------------------------------------
  ich                      = Data->GetChannelID()->GetNumber();
  gain                     = fCalibData->GetChannelGain(ich);
  pulse_integration_window = (int) (fCalibData->GetPulseWindow(ich)*5);
  ped_mean                 = fCalibData->GetPedestal(ich);
  ped_sigma                = fCalibData->GetPedSigma(ich);

  sum = 0;
  qn  = 0;
  for (int cell=fMinCell; cell<fMaxCell; cell++) {
    sum += Data->GetV0(cell);
    qn  += 1;
  }

  pedestal = sum/qn;

  Data->SetPedestal(pedestal);

  ncells = TVme1742DataBlock::kNCells;
//-----------------------------------------------------------------------------
// correct for event-based pedestals
// revert to positive numbers, calculate pulse height and the charge
// may need to recalculate the pedestals in a different range them during 
// the fitting...
//-----------------------------------------------------------------------------
  vmax = -999.;
  cmax = -1;
  for (int cell=0; cell<ncells; cell++) {
    v = -(Data->GetV0(cell)-pedestal);
    if (v > vmax) {
      vmax = v;
      cmax = cell;
    }
    
    Data->SetV1(cell,v);
  }

  Data->SetV1Max(vmax);
  Data->SetI1Max(cmax);
//-----------------------------------------------------------------------------
// look, approximately, for the first maximum
//-----------------------------------------------------------------------------
  found    = 0;
  min_cell = -1;
  max_cell = -1;

  for (int cell=0; cell<ncells; cell++) {
    if (Data->GetV1(cell) > fMinThreshold) {
      min_cell = cell; 
      found    = 1;
      break;
    }
  }
    
  if (found == 0) {
    if (nreports <= 20) {
      Error("SubtractPedestals",Form("Couldnt find MIN_CELL for channel %i",
				     Data->GetChannelID()->GetNumber()));
      nreports++;
    }
  }
//-----------------------------------------------------------------------------
// approximate T0, then find the first maximum ... which is not used so far...
//-----------------------------------------------------------------------------
  slope = (Data->GetV1(min_cell+2)-Data->GetV1(min_cell-2))/4.;
  t0    = min_cell-Data->GetV1(min_cell)/slope;

  Data->SetT0(t0);
                                                  
  for (int cell=min_cell; cell<ncells-1; cell++) {
    max_cell = cell;
    if (Data->GetV1(cell+1) < Data->GetV1(cell)) { 
      Data->SetMax1(cell,Data->GetV1(cell));
      break;
    }
  }
//-----------------------------------------------------------------------------
// recalculate pedestal using [T0-60,T0-10] interval
//-----------------------------------------------------------------------------
  min_cell = int(t0-60);
  if (min_cell <= 0) min_cell=0;
  max_cell = min_cell+50;
  if (max_cell > 1023) max_cell = 1023;
//-----------------------------------------------------------------------------
// FitPol0 returns chi2/ndof
//-----------------------------------------------------------------------------
  TDrsUtils::FitPol0(Data->GetV0(),min_cell,max_cell,&mean,&chi2);
  Data->SetPedestal(mean);
  Data->SetChi2Ped (chi2);
//-----------------------------------------------------------------------------
// redefine V1, using real pedestal - fitted for a given event or the mean
// 2012-12-27: use fitted value
//-----------------------------------------------------------------------------
  vmax = -999.;
  cmax = -1;
  for (int cell=0; cell<ncells; cell++) {
    v = -(Data->GetV0(cell)-mean);
    //    v = -(Data->GetV0(cell)-ped_mean);
    if (v > vmax) {
      vmax = v;
      cmax = cell;
    }
    
    Data->SetV1(cell,v);
  }

  Data->SetV1Max(vmax);
  Data->SetI1Max(cmax);
//-----------------------------------------------------------------------------
// fill histograms
//-----------------------------------------------------------------------------
  Data->fHistV0->Reset();
  Data->fHistV1->Reset();
  Data->fHistShape->Reset();

  for (int cell=0; cell<ncells; cell++) {
    y = Data->GetV0(cell);
    Data->fHistV0->SetBinContent(cell+1,y);
    
    y  = Data->GetV1(cell);
    ey = ped_sigma*1.5+0.05*y;

    Data->fHistV1->SetBinContent(cell+1,y);
    Data->fHistV1->SetBinError  (cell+1,ey);

    y = Data->GetV1(cell)/vmax;
    Data->fHistShape->SetBinContent(cell+1,y);
    Data->fHistShape->SetBinError  (cell+1,ey/vmax);
  }
//-----------------------------------------------------------------------------
// integrate charge (for non-trigger pulses)
//-----------------------------------------------------------------------------
  if (Data->GetTriggerChannel() == 0) {
    min_cell = (int) (t0-10);
    if (min_cell < 0) min_cell = 0;
    max_cell = (int) (t0+pulse_integration_window);
    if (max_cell > 1024) max_cell=1024;

    q = 0;
    for (int cell=min_cell; cell<max_cell; cell++) {
      v = Data->GetV1(cell);
      q += v;
    }
//-----------------------------------------------------------------------------
// account for calibrations and convert into picocoulombs
//-----------------------------------------------------------------------------
    q =  q/fQScale;
    Data->SetQ(q);
    q1 = q*gain;
    Data->SetQ1(q1);
  }

  return 0;
}


//_____________________________________________________________________________
int TPetClusterFinder::ProcessModule(TTofDetector* Tof, int ModuleNumber) {
//-----------------------------------------------------------------------------
// loop over the half-chambers, reconstructed clusters are defined in the 
// local coordinate system of the chamber
// on output covert their coordinates into the global coordinate system
//-----------------------------------------------------------------------------
  TClonesArray*    list_of_clusters;
  int              rc(0), nch, min_cell(100), max_cell(400);
  double           q1, q2;
  TTofData         *data, *ch1, *ch2;

  fTofDetector = Tof;
  fModule      = fTofDetector->GetModule(ModuleNumber);
  fCalibData   = fTofDetector->GetCalibData();

  fListOfClusters[ModuleNumber]->Clear();
//-----------------------------------------------------------------------------
// event-by-event pedestal subtraction, see it it is enough
//-----------------------------------------------------------------------------
  nch = fModule->GetNChannels();
  for (int i=0; i<nch; i++) {
    data = fModule->GetChannel(i);
    SubtractPedestals(data); 
  }
//-----------------------------------------------------------------------------
// order channels in the integrated charge
//-----------------------------------------------------------------------------
  for (int ich=0; ich<nch; ich++) {
    fModule->SetOrderedChannel(ich,fModule->GetChannel(ich));
  }

  for (int i1=0; i1<nch; i1++) {
    ch1 = fModule->GetOrderedChannel(i1);
    q1  = ch1->GetQ();

    for (int i2=i1+1; i2<nch; i2++) {
      ch2 = fModule->GetOrderedChannel(i2);
      q2  = ch2->GetQ();
      if (q2 > q1) {
	fModule->SetOrderedChannel(i2,ch1);
	fModule->SetOrderedChannel(i1,ch2);
	ch1 = ch2;
	q1  = q2;
      }
    }
  }
//-----------------------------------------------------------------------------
// process trigger cahnnels for this module
//-----------------------------------------------------------------------------
  nch = fModule->GetNTriggerChannels();
  for (int i=0; i<nch; i++) {
    data = fModule->GetTriggerChannel(i);
    SubtractPedestals(data); 
  }

  return rc;
}
