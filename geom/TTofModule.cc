//------------------------------------------------------------------------------
//  Jan 1998 G.Velev: created
//  revision history:
//  -----------------
// *0001 Jan 27 1998 G Velev: created this version
//------------------------------------------------------------------------------
#include <string.h>

#include "TPad.h"
#include "TMath.h"

#include "TGeant/TGeometryManager.hh"

#include "drs/obj/TTofData.hh"

#include "drs/geom/TTofConstants.hh"
#include "drs/geom/TTofGeometry.hh"
#include "drs/geom/TPetDetector.hh"
#include "drs/geom/TTofDetector.hh"

ClassImp(TTofModule)

//_____________________________________________________________________________
TTofModule::TTofModule() {
  // describe module geometry
  fNx               = 4;
  fNy               = 4;
  fNChannels        = 16;
  fNTriggerChannels = 2;
  fNHitCrystals     = 0;
  fListOfClusters   = new TObjArray();
}

//_____________________________________________________________________________
TTofModule::~TTofModule() {
  delete fListOfClusters;
}

//_____________________________________________________________________________
int TTofModule::DeclareMaterials(TGeometryManager*  gm) {
  return 0;
}

//_____________________________________________________________________________
int TTofModule::DeclareTrackingMedia(TGeometryManager*  gm) {
  return 0;
}


//_____________________________________________________________________________
int TTofModule::DeclareGeometry(TGeometryManager* gm, TSubdetector* mother) {
  // initialize geometry of the CMP side
  // `mother' = CMP in this case. 
  // expect side number (fNumber) to be already defined

  TPetDetector* pet = TPetDetector::Instance();
  TTofDetector* tof = (TTofDetector*) mother;

//   char name  [] = "CMP.";
//   name[3]       = 49+fNumber;

//   char namepm[] = "CS..";
//   namepm[2]     = 48+fNumber;

//   float         phi[3], theta[3];

//   theta[0]     = 90.;
//   theta[1]     = 90.;
//   theta[2]     =  0.;
//   phi  [0]     = 90*(fNumber-1);
//   phi  [1]     = phi[0]+90.;
//   phi  [2]     = 0;

//   TG3RotMatrix* rot_matrix;

//   int rot_number = CMP_ROTATION[fNumber];

//   gm->DeclareRotation(rot_matrix,rot_number,
// 		      theta[0],phi[0],theta[1],phi[1],theta[2],phi[2]);

// 					// CMP sides are placed into 
// 					// the CDF volume, containment volumes
// 					// for different sides overlap

//   gm->CreateVolume(fContainment,name,
// 		   "BOX",
// 		   cdf->TmAir(),
// 		   CMP_DIMENSIONS[fNumber],
// 		   cdf->Containment(),
// 		   CMP_POSITION[fNumber], 
// 		   rot_number, 
// 		   TG3Constants::kMany);

//   fNChambers    = CMP_NUMBER_OF_PARTS[fNumber];
//   fChamber      = new TCmpChamber[fNChambers];
//   fPhi0         = TMath::Pi()/2*fNumber;

// 					// describe the chambers

//   for (int i=0; i<fNChambers; i++) {
//     TCmpChamber* chamber = Chamber(i);
//     chamber->SetDetCode(fNumber,i);
//     chamber->DeclareGeometry(gm,this);
//   }

  return 0;
}

//_____________________________________________________________________________
void TTofModule::Clear(Option_t* opt) {
  // for the moment reset CSP counters only

//   for (int ich=0; ich<fNChambers; ich++) {
//     Chamber(ich)->Clear();
//   }
}


//-----------------------------------------------------------------------------
// TTofModule should behave similarly to TCmpChamber - it has hits
//-----------------------------------------------------------------------------
void TTofModule::PaintXY(Option_t* option) {

//   for (int i=0; i<fNChambers; i++) {
//     Chamber(i)->PaintXY(option);
//   }

}


//_____________________________________________________________________________
void TTofModule::PaintRZ(Option_t* option) {
}

//_____________________________________________________________________________
Int_t TTofModule::DistancetoPrimitiveXY(Int_t px, Int_t py) {
  Int_t dist, min_dist;

  min_dist = 9999;

//   for (int i=0; i<fNChambers; i++) {
//     dist = Chamber(i)->DistancetoPrimitiveXY(px,py);
//     if (dist < min_dist) {
//       min_dist = dist;
//     }
//   }
  return min_dist;
}

//_____________________________________________________________________________
Int_t TTofModule::DistancetoPrimitiveRZ(Int_t px, Int_t py) {
  return 9999;
}
