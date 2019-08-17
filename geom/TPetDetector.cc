//-----------------------------------------------------------------------------
//    Dec 25 2000 P.Murat: CDF description: top-level subdetector 
//-----------------------------------------------------------------------------

#include "TGeant/TG3Constants.hh"
#include "TGeant/TG3Volume.hh"
#include "TGeant/TTrackingMedium.hh"
#include "TGeant/TGeometryManager.hh"

#include "geom/TPetDetector.hh"

ClassImp(TPetDetector)

TPetDetector*  TPetDetector::fgInstance = 0;

//------------------------------------------------------------------------------
TPetDetector::TPetDetector() : TSubdetector("CDF",1) {
}

//------------------------------------------------------------------------------
TPetDetector::~TPetDetector() {
  // destructor 
}

//------------------------------------------------------------------------------
int TPetDetector::DeclareMaterials(TGeometryManager* gm) 
{
  // declare some commonly used materials

  float par[10];

  gm->DeclareMaterial(fSilicon,"SILICON",28.09,14.,2.33, 9.36, 45.66, par, 0);
  gm->DeclareMaterial(fG10,"G10"    ,28.09,14.,1.70,19.4 , 53.1 , par, 0);

				// Argon-Etane mixture 50:50 

  float   aareth[3] = { 1.010, 12.010, 39.95 };
  float   zareth[3] = { 1.   ,  6.   , 18.   };
  float   wareth[3] = { 6.   ,  2.   , 1.    };
  float   dareth    = 0.0015;

  gm->DeclareMaterial(fArEth5050,"ARETH_5050",aareth,zareth,dareth,-3,wareth);

  gm->DeclareMaterial(fScintillator,"Scintillator",
				      13.02,8.,1.032,42.4,82.0,par,0);
//------------------------------------------------------------------------------
//  materials used by IMU simulation
//------------------------------------------------------------------------------
  float epoxy_a[3]  = { 12.01, 16.0, 1.01};
  float epoxy_z[3]  = {  6.  ,  8. , 1.  };
  float epoxy_num[] = { 18.  ,  3. , 12. };

  gm->DeclareMaterial(fEpoxy, "epoxy", epoxy_a, epoxy_z, 1.4, -3, epoxy_num);

  return 0;
}

//------------------------------------------------------------------------------
int TPetDetector::DeclareTrackingMedia(TGeometryManager* gm) 
{
  // initialize tracking media needed by CDF detector
  // as this routine needs knowledge of the magnetic field, it should 
  // be called after talk-to

  float   precision;
  float   par[10];
  int     np = 0;
//------------------------------------------------------------------------------
//  default tracking medium (air with non-uniform magnetic field)
//------------------------------------------------------------------------------
  precision = (BOUNDARY_PRECISION < 10.) ? BOUNDARY_PRECISION : 10.;
  gm->DeclareTrackingMedium(fTmAir, "air_with_magnetic_field", 
			    gm->GetMaterial("AIR"), 
			    TG3Constants::kSensitiveVolume, 
			    TG3Constants::kNoMagneticField, 
			    0, // CDF_MAG_FIELD, 
			    TG3Constants::kAuto, 
			    TG3Constants::kAuto, 
			    TG3Constants::kAuto, 
			    precision,
			    TG3Constants::kAuto,
			    par,np);
//------------------------------------------------------------------------------
//  tracking media used by muon system
//------------------------------------------------------------------------------
  gm->DeclareTrackingMedium(fTmAluminumBfield, "aluminum in magnetic field", 
			    gm->GetMaterial("ALUMINIUM"), 
			    TG3Constants::kPassiveVolume,
			    TG3Constants::kNoMagneticField, 
			    0., // CDF_MAG_FIELD, 
			    TG3Constants::kAuto, 
			    TG3Constants::kAuto, 
			    TG3Constants::kAuto, 
			    precision,
			    TG3Constants::kAuto,
			    par,np);

  gm->DeclareTrackingMedium(fTmAluminumNoBfield, "aluminum w/o mag. field", 
			    gm->GetMaterial("ALUMINIUM"), 
			    TG3Constants::kPassiveVolume, 
			    TG3Constants::kNoMagneticField, 0, 
			    TG3Constants::kAuto, 
			    TG3Constants::kAuto, 
			    TG3Constants::kAuto, 
			    precision,
			    TG3Constants::kAuto,
			    par,np);
					// scintillators

  gm->DeclareTrackingMedium(fTmScintillatorBfield, 
			    "scintillator in magnetic field",
			    fScintillator, 
			    TG3Constants::kSensitiveVolume, 
			    TG3Constants::kNoMagneticField, 
			    0., // CDF_MAG_FIELD, 
			    TG3Constants::kAuto, 
			    TG3Constants::kAuto, 
			    TG3Constants::kAuto, 
			    precision,
			    TG3Constants::kAuto,
			    par,np);

  gm->DeclareTrackingMedium(fTmScintillatorNoBfield, 
			    "scintillator w/o mag. field", 
			    fScintillator, 
			    TG3Constants::kSensitiveVolume, 
			    TG3Constants::kNoMagneticField, 0, 
			    TG3Constants::kAuto, 
			    TG3Constants::kAuto, 
			    TG3Constants::kAuto, 
			    precision,
			    TG3Constants::kAuto,
			    par,np);
  return 0;
}


//_____________________________________________________________________________
int TPetDetector::DeclareGeometry(TGeometryManager* gm, TSubdetector* mother) 
{
  // initialize CDF geometry and declare it to geometry manager. 
  // Only explicitly created detectors will be initialized. This gives an 
  // important degree of freedom of working with the subdetectors separately
  // (if necessary)
					// make sure geometry has not been 
					// initialized yet

  if ((!gm) || gm->Initialized()) return -1;

					// the same instance can't be declared
					// to 2 geometries (geometry managers)
  if (GeometryInitialized()) return 0;
					// declare materials and tracking media
					// and proceed with the declaration of
					// geometry
  DeclareMaterials(gm);
  DeclareTrackingMedia(gm);
					// cache pointers to the CDF 
					// subdetectors

  fTof = (TTofDetector*) GetSubdetector("TOF");

					// create the collision hall volume
  gm->CreateVolume(fContainment,
		   "PET ",
		   "BOX",
		   fTmAir,
		   PET_DIMENSIONS,
		   NULL,
		   NULL,
		   0,
		   TG3Constants::kOnly);

  fContainment->SetVisibility(1);
					// ************************************
					// init default magnetic field ???
					// ************************************
  //  TBfield::Instance();
					// this will create a geometry hierarchy
					// with the structure parallel to the
					// detector tree. A geometry manager
					// (in principle) may maintain several
					// parallel hierarchies... But what for?
					// need to keep backward pointers from
					// the volumes to the detectors...

					// now loop over the subdetectors
  TSubdetector* det;
  int ndet = fSubdetectorList->GetEntriesFast();
  for (int i=0; i<ndet; i++) {
    det = (TSubdetector*) fSubdetectorList->UncheckedAt(i);
    if (det->Used()) {
      det->DeclareGeometry(gm,this);
    }
  }
					// optimize geometry ogranization
					// (in case of G3-based implementation
					// this translated into call to GSORD)
  gm->OptimizeGeometry(fContainment,3);

  fGeometryInitialized = 1;

  return 0;
}

//------------------------------------------------------------------------------
TPetDetector* TPetDetector::Instance() {
  static Cleaner cleaner;
  return fgInstance ? fgInstance : (fgInstance = new TPetDetector());
}


//------------------------------------------------------------------------------
TPetDetector::Cleaner::Cleaner() {
}

//------------------------------------------------------------------------------
TPetDetector::Cleaner::~Cleaner() {
  if (TPetDetector::fgInstance) {
    delete TPetDetector::fgInstance;
    TPetDetector::fgInstance = 0;
  }
}


