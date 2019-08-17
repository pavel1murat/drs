//_____________________________________________________________________________
// Feb 10 2001 P.Murat
//_____________________________________________________________________________
#include "TVirtualX.h"
#include "TPad.h"
#include "TStyle.h"

#include "Stntuple/geom/TTrajectory3D.hh"
#include "Stntuple/geom/TTrajectoryPoint.hh"
#include "TGeant/TG3Box.hh"
#include "TGeant/TG3Tube.hh"

#include "murat/alg/TExtrapolator.hh"
#include "drs/gui/TTofVisNode.hh"
//#include "murat/gui/TStnVisManager.hh"
#include "drs/geom/TTofDetector.hh"
#include "drs/geom/TPetDetector.hh"

ClassImp(TTofVisNode)

//_____________________________________________________________________________
TTofVisNode::TTofVisNode(const char* name): TVisNode(name) {
//   fStubColor[0] = 2;
//   fStubColor[1] = 4;
//   fMuonColor    = 3;
  fDisplayHits  = 1;
}

//_____________________________________________________________________________
TTofVisNode::~TTofVisNode() {
}


//_____________________________________________________________________________
void TTofVisNode::Paint(Option_t* option) {
  //
				// parse option list

  if (strstr(option,"XY") != 0) {
    PaintXY(option);
  }
  else if (strstr(option,"RZ") != 0) {
    PaintRZ(option);
  }
  else {
				// what is the default?
  }
}


//_____________________________________________________________________________
void TTofVisNode::PaintXY(Option_t* option) {
  // draw stubs and muons
 
  const Int_t kMIN_WEDGE_SIZE = 60;
  int nch;

  TVector3 p1;
  TVector3 p2;
  TVector3 p3;
  TVector3 p4;

//   TExtrapolator* extrapolator;

//   TCdfDetector* cdf = TCdfDetector::Instance();
//   TTofDetector* cmp = cdf->GetCmpDetector();

//   TStnVisManager* vm   = TStnVisManager::Instance();
//   extrapolator      = vm->GetExtrapolator();

//   TTofChamber* w0 = cmp->GetSide(0)->Chamber(0);
//   TG3Box*    box;

//   box = w0->Box();
// 					// wedge size in pixels (estimate)

//   Int_t wedge_size = gPad->XtoAbsPixel(box->GetDx())-gPad->XtoAbsPixel(0);

//   if (fDisplayHits) {
// //-----------------------------------------------------------------------------
// //  draw hits
// //-----------------------------------------------------------------------------
//     for (int is=0; is<4; is++) {
//       TTofSide* side = cmp->GetSide(is);
//       nch = side->NChambers();
//       for (int ich=0; ich<nch; ich++) {
// 	TTofChamber* chamber = side->Chamber(ich);
// 	if (chamber->NHits()) {
// 	  box = chamber->Box();
// 					// there are hits in this wedge, loop 
// 					// over them

// 	  for (int il=0; il<chamber->NLayers(); il++) {
// 	    int nh = chamber->NHitsPerLayer(il);
// 	    for (int ih=0; ih<nh; ih++) {
// 	      TTofHit* hit = chamber->Hit(il,ih);
// 	      int iwire    = hit->WireNumber();
// 	      Double_t xw  = chamber->WireX(il,iwire);
// 	      Double_t yw  = chamber->WireY(il,iwire);

// 	      Double_t delta = chamber->DyCell()/4.;
// 	      Double_t dx1   = hit->DriftDistance();
// 	      Double_t dx2   = dx1+delta;

// 	      for (Double_t sx=-1; sx<2; sx+=2) {

// 		p1.SetXYZ(xw+dx1*sx,yw,0);
// 		p2.SetXYZ(xw+dx2*sx,yw,0);

// 					// transform points into global 
// 					// coordinate system,
// 					// set right color and draw the line

// 		box->TransformLocal2Global(p1,p1);
// 		box->TransformLocal2Global(p2,p2);
// 		gVirtualX->SetLineColor(fStubColor[0]);
// 		gPad->PaintLine(p1.X(),p1.Y(),p2.X(),p2.Y());

// 					// draw line in Y

// 		p3.SetXYZ(xw+dx1*sx,yw+delta,0);
// 		p4.SetXYZ(xw+dx1*sx,yw-delta,0);
// 		box->TransformLocal2Global(p3,p3);
// 		box->TransformLocal2Global(p4,p4);
// 		gPad->PaintLine(p3.X(),p3.Y(),p4.X(),p4.Y());

// 	      }

// 	    }
// 	  }
// 	}
//       }
//     }
//   }
// //-----------------------------------------------------------------------------
// //  draw stubs
// //-----------------------------------------------------------------------------
//   int          ir, ich;
//   double       dy;
//   TTofChamber* chamber;
//   int nstubs = (*fListOfStubs)->GetEntriesFast();
//   for (int i=0; i<nstubs; i++) {
//     TTofStub* stub = (TTofStub*) (*fListOfStubs)->UncheckedAt(i);
//     if (stub == 0) goto NEXT_STUB;
//     ir      = stub->Region();
//     ich     = stub->Chamber();
//     chamber = cmp->GetSide(ir)->Chamber(ich);
//     box     = chamber->Box();
// 					// ends of the stub in the local 
// 					// coordinate system
//     dy = box->GetDy();
//     p1.SetXYZ(stub->Y0()-stub->DyDx()*dy, -dy, 0);
//     p2.SetXYZ(stub->Y0()+stub->DyDx()*dy,  dy, 0);

// 					// transform points into global 
// 					// coordinate system
//     box->TransformLocal2Global(p1,p1);
//     box->TransformLocal2Global(p2,p2);
// 					// set right color

//     gVirtualX->SetLineColor(fStubColor[0]);
    
// 					// and draw the line
//     if (wedge_size <= kMIN_WEDGE_SIZE) {
//       gVirtualX->SetLineWidth(2);
//       gPad->PaintLine(p1.X(),p1.Y(),p2.X(),p2.Y());
//       gVirtualX->SetLineWidth(1);
//     }

// 					// one also may think what to do with
// 					// the hits, but not right now
// 					// time has come

//     if (fDisplayHits && (wedge_size > kMIN_WEDGE_SIZE)) {

//       Double_t x1[10], y1[10];
//       Int_t    nhits = 0;

//       for (int il=0; il<=stub->MaxLayer(); il++) {
// 	TMuonHit* hit = stub->Hit(il);
// 	if (hit) {
// 					// display polymarker
	  
// 	  int iwire    = hit->WireNumber();
// 	  Double_t xw  = chamber->WireX(il,iwire);
// 	  Double_t yw  = chamber->WireY(il,iwire);

// 	  p1.SetXYZ(xw+hit->DriftDistance()*stub->DriftSign(il),yw, 0);
// 	  box->TransformLocal2Global(p1,p1);

// 	  x1[nhits] = p1.X();
// 	  y1[nhits] = p1.Y();
// 	  nhits++;
// 	}
//       }

//       gStyle->SetMarkerColor(1);
//       gStyle->SetMarkerStyle(24);
//       gStyle->SetMarkerSize(0.5);

//       gPad->PaintPolyMarker(nhits,x1,y1);
//       gPad->PaintPolyLine  (nhits,x1,y1);
//     }
//   NEXT_STUB:;
//   }

  gPad->Modified();
}


//_____________________________________________________________________________
void TTofVisNode::PaintRZ(Option_t* option) {
}

//_____________________________________________________________________________
Int_t TTofVisNode::DistancetoPrimitive(Int_t px, Int_t py) {
  return 9999;
}

//_____________________________________________________________________________
Int_t TTofVisNode::DistancetoPrimitiveXY(Int_t px, Int_t py) {

  Int_t dist = 9999;

  static TVector3 global;
  static TVector3 local;

  Double_t    dx1, dx2, dy1, dy2, dx_min, dy_min, dr;

  global.SetXYZ(gPad->AbsPixeltoX(px),gPad->AbsPixeltoY(py),0);

  return dist;
}

//_____________________________________________________________________________
Int_t TTofVisNode::DistancetoPrimitiveRZ(Int_t px, Int_t py) {
  return 9999;
}

