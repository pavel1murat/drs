///////////////////////////////////////////////////////////////////////////////
// uses classes from murat/TGeant
///////////////////////////////////////////////////////////////////////////////
#include "Stntuple/scripts/global_vars.h"

int pet_init_geometry() {
//-----------------------------------------------------------------------------
// calibration pass  00    : no calibrations, all scales=1, corrections=0 
// calibration pass  02    : new CES alignment added
// calibration pass  03    : 02+E(ces)/P scale of 0.87 added to "wp_vs_z"
// calibration pass  04    : move to new channel-to-channel calibration scheme
// calibration pass  05    : final calibrations
// calibration pass  05.mc : final calibrations for MC
// force reinitialization
//-----------------------------------------------------------------------------
  TCalibManager* cm = TCalibManager::Instance();
  int force = 1;

  cm->SetPass(g.CalibPass.Data(),force);
  printf(" CALIBRATION PASS  = %s\n",cm->fPass.Data());

  /* DEBUG */     printf("[pet_init_geometry.C]: instantiate TGeometryManager\n");    
  TGeometryManager* gm = TGeometryManager::Instance();
  /* DEBUG */     printf("[pet_init_geometry.C]: TGeometryManager instantiated\n");    

  TPetDetector* pet = TPetDetector::Instance();
  if (pet->GetSubdetectorList() == 0) {
    pet->AddSubdetector(new TTofDetector());
    pet->DeclareGeometry(gm,0);
  }

//   TStnVisManager* vm = TStnVisManager::Instance();
//   vm->SetExtrapolator(new TSimpleExtrapolator);
//   vm->AddNode(new TDetectorVisNode(cdf));

//   SetVisManagerCallbacks(vm,g.x);

}
