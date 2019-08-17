#ifndef GEOMETRY_TCdfDetector
#define GEOMETRY_TCdfDetector

#include "TClonesArray.h"

#include "TGeant/TSubdetector.hh"

#include "TPetGeometry.hh"

class TG3Box;
class TTofDetector;
class AbsEvent;
class TG3Material;
class TTrackingMedium;
class TBField;

class TPetDetector : public TSubdetector {
protected:
					// magnetic field has to be initialized
					// separately and then passed to the 
					// CDF detector. This may seem 
					// artificial, because the field 
					// depends on the geometry. However the
					// it is not always direct dependence
					// on the constants describing the 
					// geometry, also one may want to get 
					// field w/o any detector...

  TBField*               fBField;	// magnetic field

					// pointers to the subdetectors (we 
					// have less than 2 dozens of those, so
					// keep each of them explicitly)
  TTofDetector*          fTof;	// !
					// some commonly used materials, which
					// are not in the default list

  TG3Material*             fSilicon;
  TG3Material*             fScintillator;
  TG3Material*             fArEth5050;
  TG3Material*             fEpoxy;
  TG3Material*             fG10;
					// available tracking media
				        // default tracking medium
					// non-uniform magnetic field ON
  TTrackingMedium*       fTmAir;
				        // aluminum and scintillators (CMU/CMP)
					// aluminum also used in BeamPipe

  TTrackingMedium*       fTmAluminumBfield;
  TTrackingMedium*       fTmAluminumNoBfield;
  TTrackingMedium*       fTmScintillatorBfield;
  TTrackingMedium*       fTmScintillatorNoBfield;

  static TPetDetector*  fgInstance;

					// memory cleanup stuff
  class  Cleaner {
  public: 
    Cleaner();
    ~Cleaner();
  };
  friend class Cleaner;
//------------------------------------------------------------------------------
// functions
//------------------------------------------------------------------------------
public:
					// ****** constructors and destructor
  TPetDetector();
  virtual ~TPetDetector();

  static TPetDetector* Instance();
					// ****** overloaded methods of 
					// TSubdetector

  int  DeclareMaterials    (TGeometryManager* gm);
  int  DeclareTrackingMedia(TGeometryManager* gm);
  int  DeclareGeometry     (TGeometryManager* gm, TSubdetector* mother);

					// ****** direct access to subdetectors

  TTofDetector*        GetTofDetector() { return fTof; }

					// geometry manager

  TBField*             GetBField      () { return fBField;   }
  
					// materials

  TG3Material*  Silicon         () { return fSilicon  ; }
  TG3Material*  Epoxy           () { return fEpoxy    ; }
  TG3Material*  G10             () { return fG10      ; }
  TG3Material*  ArEth5050       () { return fArEth5050; }
  TG3Material*  Scintillator    () { return fScintillator; }

					// tracking media

  TTrackingMedium* TmAir                 () { return fTmAir                 ; }
  TTrackingMedium* TmAluminumBfield      () { return fTmAluminumBfield      ; }
  TTrackingMedium* TmAluminumNoBfield    () { return fTmAluminumNoBfield    ; }
  TTrackingMedium* TmScintillatorBfield  () { return fTmScintillatorBfield  ; }
  TTrackingMedium* TmScintillatorNoBfield() { return fTmScintillatorNoBfield; }

  TG3Box*         Containment() { return (TG3Box*) fContainment; }

					// modifiers

  void SetBField          (TBField*          bf) { fBField          = bf; }

  ClassDef(TPetDetector,0)
};


#endif
