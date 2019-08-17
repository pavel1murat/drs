///////////////////////////////////////////////////////////////////////////////
// DetCode: 
// --------
// bit  0: 
///////////////////////////////////////////////////////////////////////////////
#include "TString.h"

#include "drs/obj/TTofData.hh"


ClassImp(TTofData)

//_____________________________________________________________________________
TTofData::TTofData() : TObject() {
  fHistShape      = new TH1F("shape","shape",1024,0,1024.);
  fHistV0         = new TH1F("v0"   ,"v0"   ,1024,0,1024.);
  fHistV1         = new TH1F("v1"   ,"v1"   ,1024,0,1024.);
  fT0             = -999.;
  fReadoutGroup   = -1;
  fTriggerChannel = 0;
}

//_____________________________________________________________________________
TTofData::~TTofData() {
  delete fHistShape;
  delete fHistV0;
  delete fHistV1;
}

// //_____________________________________________________________________________
// int TTofData::Init(int Side , int Wedge, int View, 
// 			  int First, int N    , int NMerged, int Seed,
// 			  float Energy, float Coord, float Sigma) 
// {
//   // `View` = 0 or 1

//   fCode    = (Seed<<24)|(N << 15)|(First << 7)|(Wedge << 2)|(Side << 1)|(View & 0x1);
//   fNMerged = NMerged;
//   fEnergy  = Energy;
//   fCoord   = Coord;
//   fSigma   = Sigma;
//   fChiSq   = 1.e6;			// so far...
//   fTrack   = 0;
//   fTrackDx = 1e6;

//   for (int i=0; i<5; i++) fQx[i] = -1;
  
//   return 0;
// }

//_____________________________________________________________________________
void TTofData::Print(Option_t* Option) const {

  TString opt = Option;

  if (opt.Contains("banner")) {
    printf("----------------------------------------------------------------");
    printf("------------------------------------------\n");
    printf(" S  W  V Energy Ns  I1 Seed Nm   Coord   Dx ");
    printf("   FitE    FitX  FitDx  TrkDx   Chi2");
    printf("   Qx0   Qx1   Qx2   Qx3   Qx4\n");
    printf("----------------------------------------------------------------");
    printf("------------------------------------------\n");
  }

//   if (opt.Contains("data") || opt == "") {
//     printf ("%2i %2i %2i %6.2f %2i %3i %4i %2i ",
// 	    Side(),
// 	    Wedge(),
// 	    View(),
// 	    Energy(),
// 	    NHit(),
// 	    FirstHit(),
// 	    Seed(),
// 	    NMerged());

//     float track_dx = (TrackDx() < 999.99) ? TrackDx() : 999.99 ;
//     float chi2     = (ChiSq  () < 999.99) ? ChiSq  () : 999.99 ;

//     printf ("%7.2f %5.2f %6.2f %7.2f %6.2f %6.2f %6.2f",
// 	    LocalCoord(),
// 	    Dx(),
// 	    FitEnergy(),
// 	    FitX (),
// 	    FitDx(),
// 	    track_dx,
// 	    chi2);

//     for (int i=0; i<5; i++) printf("%6.2f",Qx(i));
//     printf("\n");
//  }  
}
  
