//------------------------------------------------------------------------------
//  revision history:
//  -----------------
// *0001 Mar 28 2012 P.Murat: light-weight description of TOF-PET detector geometry
//------------------------------------------------------------------------------

#include <cstring>
#include <cstdlib>
#include <cmath>

#include "TROOT.h"
#include "TClass.h"

#include "TGeant/TG3Volume.hh"
#include "TGeant/TG3Constants.hh"
#include "TGeant/TTrackingMedium.hh"
#include "TGeant/TGeometryManager.hh"

#include "drs/obj/TVme1742DataBlock.hh"
#include "drs/obj/TTofCalibData.hh"
#include "drs/obj/TTofData.hh"
// #include "drs/obj/TTofReadoutMap.hh"

#include "drs/geom/TPetDetector.hh"
#include "drs/geom/TPetGeometry.hh"
#include "drs/geom/TTofGeometry.hh"
#include "drs/geom/TTofDetector.hh"
#include "drs/geom/TTofModule.hh"

ClassImp(TTofDetector)
//------------------------------------------------------------------------------
// ********** class TTofDetector **********
//------------------------------------------------------------------------------
TTofDetector::TTofDetector() : TSubdetector("TOF",1) {
  TClass* cl;

  TTofModule*   m;

  int           ich;

  fContainment      = NULL;
  fNModules         = 2;
  fNChannels        = 32;
  //  fNReadoutGroups   = 4;
  fNTriggerChannels = 4;

  fModule           = new TTofModule[fNModules];
  fData             = new TTofData  [fNChannels];

  for (int i=0; i<fNChannels; i++) {
					// each 8 DRS4 signal channels have a common trigger channel
    fData[i].SetReadoutGroup(i/8);
  }

  fTriggerData  = new TTofData  [fNTriggerChannels];
  for (int i=0; i<fNTriggerChannels; i++) {
					// default: zero
    fTriggerData[i].SetTriggerChannel(1);
  }

  cl            = gROOT->GetClass("TTofCalibData");

  fCalibData    = (TTofCalibData*) cl->New();

}

//_____________________________________________________________________________
TTofDetector::~TTofDetector() {
  if (fContainment) delete fContainment;
  delete [] fModule;
  delete [] fData;
  delete fCalibData;
}


//_____________________________________________________________________________
Int_t TTofDetector::InitCalibrations(Int_t RunNumber, TCalibManager* Manager) {
  //

  int ich, loc, used;

  TTofModule*  m;
  TTofData*    channel;

  int rc = fCalibData->Init(RunNumber,Manager);

  // need to initialize module pointers to the data

  for (int im=0; im<fNModules; im++) {
    m = GetModule(im);
    for (int ix=0; ix<4; ix++) {
      for (int iy=0; iy<4; iy++) {
	ich     = fCalibData->GetChannel(im,ix,iy);
	used    = fCalibData->GetUsed(ich);
	channel = fData+ich;
	channel->SetChannelID(ich,im,ix,iy);
	channel->SetUsed(used);
	m->SetData(ix,iy,channel);
	loc = 4*ix+iy;
	m->SetChannel(loc,channel);
      }
    }
//-----------------------------------------------------------------------------
// straw-man initialization of the trigger channels, half-hack
//-----------------------------------------------------------------------------
    for (int i=0; i<2; i++) {
      ich = im*2+i;
      channel = fTriggerData+ich;
      channel->SetChannelID(ich,im,0,0);
      channel->SetUsed(1);
      m->SetTriggerChannel(i,channel);
    }
  }

  return rc;
}

//_____________________________________________________________________________
int TTofDetector::DeclareMaterials(TGeometryManager*  gm) {
  // declare CMU-specific materials to the geometry manager
  return 0;
}

//_____________________________________________________________________________
int TTofDetector::DeclareTrackingMedia(TGeometryManager*  gm) 
{
  // declare CMU-specific tracking media to the geometry manager
  // CMU gas: Ar:Ethane (50:50). the material itself should be declared by
  // CDF node
  
  float     par[10];
  int       np        = 0;
  float     precision;

  precision = 0.1;

  gm->DeclareTrackingMedium(fTmedGas, "TOF Gas", 
			    gm->GetMaterial("ARETH_5050"), 
			    TG3Constants::kSensitiveVolume, 
			    TG3Constants::kNoMagneticField, 
			    0., // CDF_MAG_FIELD, 
			    TG3Constants::kAuto, 
			    TG3Constants::kAuto, 
			    TG3Constants::kAuto, 
			    precision,
			    TG3Constants::kAuto,
			    par,np);

  return 0;
}

//_____________________________________________________________________________
int TTofDetector::DeclareGeometry(TGeometryManager*  gm, TSubdetector* mother) 
{
  // initialize CMP geometry and simultaneously declare it to the geometry
  // manager

  if (! Used())                   return  0;
  if ((!gm) || gm->Initialized()) return -1;
  if (GeometryInitialized())      return  1;


  DeclareMaterials(gm);
  DeclareTrackingMedia(gm);

  //  SetDetectorType   (CMP_DETECTOR_TYPE);
  SetDetectorSetName(TOF_DETECTOR_SET_NAME);

					// TOF volumes which will be replicated
  char last;
  char name  [] = "TOF.";
//   char namec [] = "CMG.";
//   char namesl[] = "CSP.";

  Double_t pos[3]  = {0,0,0};
  Double_t dim[3];

  TPetDetector* pet = TPetDetector::Instance();
  TG3Box* drift_volume;

  for (int ic=0; ic<2; ic++){
					// volumes describing the chamber -
					// a box of air inside a box of Al 
					// ** so far doesn't make sense , just compile
    dim[0]    = TOF_MODULE_DIMX;
    dim[1]    = TOF_MODULE_DIMY;      
    dim[2]    = TOF_MODULE_DIMZ;

    gm->CreateShape(fChamberVolume[ic],
		    name,
		    "BOX",
		    pet->TmAluminumNoBfield(),
		    dim);
// 					// place volume of air inside the 
// 					// aluminum box - this is a drift
// 					// cell
//     dim[0]  = CMP_X_DIM_GAS[ic];
//     dim[1]  = CMP_Y_DIM_GAS[ic];      
//     dim[2]  = CMP_Z_DIM_GAS[ic];

//     gm->CreateSubVolume((TG3Volume*&) drift_volume,
// 			namec,
// 			"BOX",
// 			fTmedGas,
// 			dim,
// 			fChamberVolume[ic],
// 			pos,
// 			0,
// 			TG3Constants::kOnly);

  }
// 					// CMP doesn't have a single 
// 					// containment volume
// 					// I wish it did... it has 4 sides
// 					// instead
//   for (int is=0; is<kNSides; is++) {
//     fSide[is].SetNumber(is);
//     fSide[is].DeclareGeometry(gm,this);
//   }
  
  SetGeometryInitialized(1);
  return 0;
}


//_____________________________________________________________________________
Int_t TTofDetector::InitEvent(TObject* event, int read_available_input) {
  return 0;
}

//_____________________________________________________________________________
Int_t TTofDetector::GenerateHit() {
  printf(" dummy TTofDetector::GenerateHit() called\n");
  return 0;
}

//_____________________________________________________________________________
Int_t TTofDetector::GenerateHit(float* p1, float* p2, int mc_number) {
  printf(" dummy TTofDetector::GenerateHit(float*,float*,int) called\n");
  return 0;
}

//_____________________________________________________________________________
Int_t TTofDetector::InitEvent(TVme1742DataBlock* Data) 
{
  // initialize CMP and CSP with the event data. For the moment do only CSP

					// clear the data (later this piece of 
					// code can be reorganized
  Clear();

  InitTofModules(Data);

  return 0;
}



//_____________________________________________________________________________
Int_t TTofDetector::InitTofModules(TVme1742DataBlock* VmeData) {

  int          nhits, old_id, region, tdc_number, first_cell, hw_cell;
  int          ich, ix, iy, im, used, chid, drs_counts, max_cells(1024), cmax, eval_board;
  float        data, gain, offset, pedestal, vmax, v, q;

  TTofData     *ch_data, *tr_data; 

  int ngr = VmeData->GetMaxNGroups();
  int nch = VmeData->GetNChannelsPerGroup();

  eval_board = VmeData->IsEvalBoard();

  for (int i=0; i<ngr; i++) {
    if (VmeData->GroupPresent(i)) {
      first_cell = VmeData->FirstCell(i);

      for (int j=0; j<nch; j++) {
	ich      = nch*i+j;
	im       = fCalibData->GetModule(ich);
	ix       = fCalibData->GetIX    (ich);
	iy       = fCalibData->GetIX    (ich);
	used     = fCalibData->GetUsed  (ich);

	ch_data  = GetData(ich);

	ch_data->SetUsed(used);

	if (used == 1) {

	  pedestal = fCalibData->GetPedestal(ich);

	  for (int cell=0; cell<max_cells; cell++) {
	    drs_counts = VmeData->GroupData(i,j,cell);
					// correct for calibrations
	    hw_cell = cell+first_cell;
	    if (hw_cell > 1024) hw_cell -= 1024;
	    if (eval_board == 1) {
//-----------------------------------------------------------------------------
// in case the DRS4 evaluation board is read, no need to correct for calibrations - 
// they are accounted for in the 
// readings were offset by 1000 in TDrsEvalBoard::InitVme1742DataBlock
//-----------------------------------------------------------------------------
	      offset   = 32767.5;
	      gain     = 65536./1000.;
	    }
	    else {
//-----------------------------------------------------------------------------
// as derived, the calibrations constants
//-----------------------------------------------------------------------------
	      offset   = fCalibData->GetOffset(ich,hw_cell);
	      gain     = fCalibData->GetGain  (ich,hw_cell);
	    }

	    data     = (drs_counts-offset)/gain;
  
	    ch_data->SetV0(cell,data);
	  }
	}
      }
//-----------------------------------------------------------------------------
// handle trigger data, if they are present
// for the evaluation board VmeData->TriggerReadout() should be returning 0
//-----------------------------------------------------------------------------
      if (VmeData->TriggerReadout(i) == 1) {
					// read trigger data, one trigger channel per group
	tr_data  = GetTriggerData(i);
	for (int cell=0; cell<max_cells; cell++) {
	  drs_counts = VmeData->TriggerData(i,cell);

					// so far - no calibration of the trigger data, just 
					// copying them over
	  tr_data->SetV0(cell,drs_counts);
	}
      }
    }  
  }

  return 0;
}

//_____________________________________________________________________________
void TTofDetector::Clear(Option_t* opt) {

  for (int i=0; i<fNModules; i++) {
    GetModule(i)->Clear();
  }
}

//_____________________________________________________________________________
void TTofDetector::Print(Option_t* opt) const {
  printf(" dummy TTofDetector::Print called\n");
}



//_____________________________________________________________________________
void TTofDetector::PaintXY(Option_t* option) {
//   for (int is=0; is<kNSides; is++) {
//     fSide[is].PaintXY(option);
//   }
}


//_____________________________________________________________________________
void TTofDetector::PaintRZ(Option_t* option) {
}

//_____________________________________________________________________________
Int_t TTofDetector::DistancetoPrimitiveXY(Int_t px, Int_t py) {
  Int_t dist, min_dist;
  min_dist = 9999;

//   for (int is=0; is<kNSides; is++) {
//     dist = fSide[is].DistancetoPrimitiveXY(px,py);
//     if (dist < min_dist) {
//       min_dist = dist;
//     }
//   }

  return min_dist;
}

//_____________________________________________________________________________
Int_t TTofDetector::DistancetoPrimitiveRZ(Int_t px, Int_t py) {
  return 9999;
}
