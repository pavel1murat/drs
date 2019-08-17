///////////////////////////////////////////////////////////////////////////////
// Drawing a PET-TOF prototype, using ROOT geometry classes
///////////////////////////////////////////////////////////////////////////////

#include "TGeoManager.h"

//-----------------------------------------------------------------------------
// brain imager - design based on Sergey's strip board 
// option 1: DRS4 boards displaced radially
//-----------------------------------------------------------------------------
void plot_sergey_geometry_v1() {

 TGeoManager *geom    = new TGeoManager("geom","TOF-PET geometry");

 TGeoMaterial *vacuum = new TGeoMaterial("vacuum",0,0,0);
 TGeoMaterial *Fe     = new TGeoMaterial("Fe",55.845,26,7.87);
 TGeoMaterial *Cu     = new TGeoMaterial("Cu",63.549,29,8.92);

 TGeoMedium *Air      = new TGeoMedium("Vacuum",0,vacuum);
 TGeoMedium *Iron     = new TGeoMedium("Iron",1,Fe);
 TGeoMedium *Copper   = new TGeoMedium("Copper",2,Cu);

 TGeoVolume *top      = geom->MakeBox("top",Air,1000,1000,1000);

 geom->SetTopVolume(top);
 geom->SetTopVisible(0);

 // If you want to see the boundary, please input the number, 1 instead of 0.
 // Like this, geom->SetTopVisible(1); 

 // TGeoVolume *box1=geom->MakeBox("Box1",Copper,2,2,4.5);
 // box1->SetFillColor(2);
 // box1->SetLineColor(2);
 // top->AddNodeOverlap(box1,1,new TGeoTranslation(0,0,0));

 TVector3 crystal(0.75, 0.25, 0.25);        // crystal dimensions
 TVector3 strip_board  (0.25, 1.20, 4.50);  // front-end board  half-dimensions
 TVector3 drs4_board   (0.25, 1.20, 7.50);  // DRS4 board  half-dimensions

 TGeoVolume   *box, *box_cr, *box_drs4b;

 double radius, offset, dx, dy, dz;
 int    nwedges;

 radius = 14.;     // D = 28cm - internal radius of the electronics

 // dx = 0.75;        // crystal length = 15 mm;
 // dy = 0.25;


 dz      = strip_board.Z();
 offset  = radius; // +dx;
 nwedges = 36;

 for (int i=0; i<nwedges; i++) {

   TGeoVolume *box=geom->MakeBox("Box2",Copper,strip_board.X(),strip_board.Y(),strip_board.Z());
   box->SetFillColor(4);
   box->SetLineColor(4);

   double phi     = 360./nwedges*i;

   double phi_rad = phi*TMath::Pi()/180;

   TGeoRotation* rot = new TGeoRotation(Form("r02i",i),0,0,phi);

   double dy1 = 2*strip_board.Y(); // dy+0.01;

   top->AddNodeOverlap(box,2*i+1,new TGeoCombiTrans(offset*cos(phi_rad), // +dy1*sin(phi_rad),
						    offset*sin(phi_rad), // -dy1*cos(phi_rad),
						    0,
						    rot));

   drs4_box = geom->MakeBox("BoxDRS4",Copper,drs4_board.X(),drs4_board.Y(),drs4_board.Z());
   drs4_box->SetFillColor(41);
   drs4_box->SetLineColor(41);

   drs4_offset = radius + 1;

   top->AddNodeOverlap(drs4_box,2*i+1,new TGeoCombiTrans(drs4_offset*cos(phi_rad), // +dy1*sin(phi_rad),
							 drs4_offset*sin(phi_rad), // -dy1*cos(phi_rad),
							 drs4_board.Z()-strip_board.Z(),
							 rot));
//-----------------------------------------------------------------------------
// now position the scintillating crystals
//-----------------------------------------------------------------------------
   for (int iy=0; iy<4; iy++) {
     TGeoVolume *box_cr = geom->MakeBox("box_cr",Copper,crystal.X(),crystal.Y(),crystal.Z());
     box_cr->SetFillColor(7);
     box_cr->SetLineColor(7);

     double off_x = radius-strip_board.X()-crystal.X();
     double off_y = (iy-1.5)*(2*crystal.Y()+0.02);

     for (int iz=0; iz<8; iz++) {
       double off_z = (iz-3.5)*(2*crystal.Z()+0.02);

       top->AddNodeOverlap(box_cr,
			   32*i+8*iy+iz+1,
			   new TGeoCombiTrans(off_x*cos(phi_rad)+off_y*sin(phi_rad),
					      off_x*sin(phi_rad)-off_y*cos(phi_rad),
					      off_z,
					      rot)
			   );
     }
   }
 }
   
 top->SetVisibility(0);
 geom->CloseGeometry();
 
 top->Draw("ogl");

}

//-----------------------------------------------------------------------------
// brain imager - design based on Sergey's strip board 
// option 1: DRS4 boards displaced in Z
//-----------------------------------------------------------------------------
void plot_sergey_geometry_v2() {

 TGeoManager *geom    = new TGeoManager("geom","TOF-PET geometry");

 TGeoMaterial *vacuum = new TGeoMaterial("vacuum",0,0,0);
 TGeoMaterial *Fe     = new TGeoMaterial("Fe",55.845,26,7.87);
 TGeoMaterial *Cu     = new TGeoMaterial("Cu",63.549,29,8.92);

 TGeoMedium *Air      = new TGeoMedium("Vacuum",0,vacuum);
 TGeoMedium *Iron     = new TGeoMedium("Iron",1,Fe);
 TGeoMedium *Copper   = new TGeoMedium("Copper",2,Cu);

 TGeoVolume *top      = geom->MakeBox("top",Air,1000,1000,1000);

 geom->SetTopVolume(top);
 geom->SetTopVisible(0);

 // If you want to see the boundary, please input the number, 1 instead of 0.
 // Like this, geom->SetTopVisible(1); 

 // TGeoVolume *box1=geom->MakeBox("Box1",Copper,2,2,4.5);
 // box1->SetFillColor(2);
 // box1->SetLineColor(2);
 // top->AddNodeOverlap(box1,1,new TGeoTranslation(0,0,0));

 TVector3 crystal(0.75, 0.25, 0.25);        // crystal dimensions
 TVector3 strip_board  (0.25, 1.20, 4.50);  // front-end board  half-dimensions
 TVector3 drs4_board   (0.25, 1.20, 7.50);  // DRS4 board  half-dimensions

 TGeoVolume   *box, *box_cr, *box_drs4b;

 double radius, offset, dx, dy, dz;
 int    nwedges;

 radius = 14.;     // D = 28cm - internal radius of the electronics

 // dx = 0.75;        // crystal length = 15 mm;
 // dy = 0.25;


 dz      = strip_board.Z();
 offset  = radius; // +dx;
 nwedges = 36;

 for (int i=0; i<nwedges; i++) {

   TGeoVolume *box=geom->MakeBox("Box2",Copper,strip_board.X(),strip_board.Y(),strip_board.Z());
   box->SetFillColor(4);
   box->SetLineColor(4);

   double phi     = 360./nwedges*i;

   double phi_rad = phi*TMath::Pi()/180;

   TGeoRotation* rot = new TGeoRotation(Form("r02i",i),0,0,phi);

   double dy1 = 2*strip_board.Y(); // dy+0.01;

   top->AddNodeOverlap(box,2*i+1,new TGeoCombiTrans(offset*cos(phi_rad), // +dy1*sin(phi_rad),
						    offset*sin(phi_rad), // -dy1*cos(phi_rad),
						    0,
						    rot));

   drs4_box = geom->MakeBox("BoxDRS4",Copper,drs4_board.X(),drs4_board.Y(),drs4_board.Z());
   drs4_box->SetFillColor(42);
   drs4_box->SetLineColor(42);

   top->AddNodeOverlap(drs4_box,2*i+1,new TGeoCombiTrans(offset*cos(phi_rad), // +dy1*sin(phi_rad),
							 offset*sin(phi_rad), // -dy1*cos(phi_rad),
							 drs4_board.Z()+strip_board.Z()+0.5,
							 rot));
//-----------------------------------------------------------------------------
// now position the scintillating crystals
//-----------------------------------------------------------------------------
   for (int iy=0; iy<4; iy++) {
     TGeoVolume *box_cr = geom->MakeBox("box_cr",Copper,crystal.X(),crystal.Y(),crystal.Z());
     box_cr->SetFillColor(7);
     box_cr->SetLineColor(7);

     double off_x = radius-strip_board.X()-crystal.X();
     double off_y = (iy-1.5)*(2*crystal.Y()+0.02);

     for (int iz=0; iz<8; iz++) {
       double off_z = (iz-3.5)*(2*crystal.Z()+0.02);

       top->AddNodeOverlap(box_cr,
			   32*i+8*iy+iz+1,
			   new TGeoCombiTrans(off_x*cos(phi_rad)+off_y*sin(phi_rad),
					      off_x*sin(phi_rad)-off_y*cos(phi_rad),
					      off_z,
					      rot)
			   );
     }
   }
 }
   
 top->SetVisibility(0);
 geom->CloseGeometry();
 
 top->Draw("ogl");

}


//-----------------------------------------------------------------------------
// brain imager - design based on Sergey's strip board 
// option 2: DRS4 boards displaced radially and in Z, one digital board per 2 analog ones
//-----------------------------------------------------------------------------
void plot_sergey_geometry_v3() {

  TGeoManager *geom    = new TGeoManager("geom","TOF-PET geometry");

  TGeoMaterial *vacuum = new TGeoMaterial("vacuum",0,0,0);
  TGeoMaterial *Fe     = new TGeoMaterial("Fe",55.845,26,7.87);
  TGeoMaterial *Cu     = new TGeoMaterial("Cu",63.549,29,8.92);

  TGeoMedium *Air      = new TGeoMedium("Vacuum",0,vacuum);
  TGeoMedium *Iron     = new TGeoMedium("Iron",1,Fe);
  TGeoMedium *Copper   = new TGeoMedium("Copper",2,Cu);

  TGeoVolume *top      = geom->MakeBox("top",Air,1000,1000,1000);
  
  geom->SetTopVolume(top);
  geom->SetTopVisible(0);
  
  // If you want to see the boundary, please input the number, 1 instead of 0.
  // Like this, geom->SetTopVisible(1); 
  
  // TGeoVolume *box1=geom->MakeBox("Box1",Copper,2,2,4.5);
  // box1->SetFillColor(2);
  // box1->SetLineColor(2);
  // top->AddNodeOverlap(box1,1,new TGeoTranslation(0,0,0));
  
  TVector3 crystal(0.75, 0.25, 0.25);        // crystal dimensions
  TVector3 strip_board  (0.25, 1.20, 4.50);  // front-end board  half-dimensions
  TVector3 drs4_board   (0.25, 2.50, 7.50);  // DRS4 board  half-dimensions
  
  TGeoVolume   *box, *box_cr, *box_drs4b;
  
  double radius, offset, dx, dy, dz;
  int    nwedges;

  radius = 14.;     // D = 28cm - internal radius of the electronics

  // dx = 0.75;        // crystal length = 15 mm;
  // dy = 0.25;
  

  dz      = strip_board.Z();
  offset  = radius; // +dx;
  nwedges = 36;

  for (int i=0; i<nwedges; i++) {
//-----------------------------------------------------------------------------
// position the analog boards
//-----------------------------------------------------------------------------
    TGeoVolume *box=geom->MakeBox("Box2",Copper,strip_board.X(),strip_board.Y(),strip_board.Z());
    box->SetFillColor(4);
    box->SetLineColor(4);
    
    double phi     = 360./nwedges*i;
    
    double phi_rad = phi*TMath::Pi()/180;
    
    TGeoRotation* rot = new TGeoRotation(Form("r02i",i),0,0,phi);
    
    double dy1 = 2*strip_board.Y(); // dy+0.01;
   
    top->AddNodeOverlap(box,2*i+1,new TGeoCombiTrans(offset*cos(phi_rad), // +dy1*sin(phi_rad),
						    offset*sin(phi_rad), // -dy1*cos(phi_rad),
						    0,
						    rot));
//-----------------------------------------------------------------------------
// now position the scintillating crystals
//-----------------------------------------------------------------------------
    for (int iy=0; iy<4; iy++) {
      TGeoVolume *box_cr = geom->MakeBox("box_cr",Copper,crystal.X(),crystal.Y(),crystal.Z());
      box_cr->SetFillColor(7);
      box_cr->SetLineColor(7);
      
      double off_x = radius-strip_board.X()-crystal.X();
      double off_y = (iy-1.5)*(2*crystal.Y()+0.02);
      
      for (int iz=0; iz<8; iz++) {
	double off_z = (iz-3.5)*(2*crystal.Z()+0.02);
	
	top->AddNodeOverlap(box_cr,
			    32*i+8*iy+iz+1,
			    new TGeoCombiTrans(off_x*cos(phi_rad)+off_y*sin(phi_rad),
					       off_x*sin(phi_rad)-off_y*cos(phi_rad),
					       off_z,
					       rot)
			    );
      }
    }
  }
//-----------------------------------------------------------------------------
// position digital boards
//-----------------------------------------------------------------------------
  int nw2 = nwedges/2;
  for (int i=0; i<nwedges; i++) {
    
    double phi     = 360./nw2*(i+0.25);
    
    double phi_rad = phi*TMath::Pi()/180;
    
    TGeoRotation* rot = new TGeoRotation(Form("r02i",i),0,0,phi);
    
    // double dy1 = 2*strip_board.Y(); // dy+0.01;

    // top->AddNodeOverlap(box,2*i+1,new TGeoCombiTrans(offset*cos(phi_rad), // +dy1*sin(phi_rad),
    // 						    offset*sin(phi_rad), // -dy1*cos(phi_rad),
    // 						    0,
    // 						    rot));
    
    
    drs4_box = geom->MakeBox("BoxDRS4",Copper,drs4_board.X(),drs4_board.Y(),drs4_board.Z());
    drs4_box->SetFillColor(kBlue-10);
    drs4_box->SetLineColor(kBlue-10);

    drs4_offset = radius ; // + 1;

    top->AddNodeOverlap(drs4_box,2*i+1,new TGeoCombiTrans(drs4_offset*cos(phi_rad), // +dy1*sin(phi_rad),
							  drs4_offset*sin(phi_rad), // -dy1*cos(phi_rad),
							  drs4_board.Z()+strip_board.Z()+0.5,
							  rot));
  }
   
  top->SetVisibility(0);
  geom->CloseGeometry();
  
  top->Draw("ogl");
}

//-----------------------------------------------------------------------------
// breast imager 
//-----------------------------------------------------------------------------
void breast_imager() {

 TGeoManager *geom    = new TGeoManager("geom","TOF-PET geometry");

 TGeoMaterial *vacuum = new TGeoMaterial("vacuum",0,0,0);
 TGeoMaterial *Fe     = new TGeoMaterial("Fe",55.845,26,7.87);
 TGeoMaterial *Cu     = new TGeoMaterial("Cu",63.549,29,8.92);

 TGeoMedium *Air      = new TGeoMedium("Vacuum",0,vacuum);
 TGeoMedium *Iron     = new TGeoMedium("Iron",1,Fe);
 TGeoMedium *Copper   = new TGeoMedium("Copper",2,Cu);

 TGeoVolume *top      = geom->MakeBox("top",Air,1000,1000,1000);

 geom->SetTopVolume(top);
 geom->SetTopVisible(0);

 // If you want to see the boundary, please input the number, 1 instead of 0.
 // Like this, geom->SetTopVisible(1); 

 // TGeoVolume *box1=geom->MakeBox("Box1",Copper,2,2,4.5);
 // box1->SetFillColor(2);
 // box1->SetLineColor(2);
 // top->AddNodeOverlap(box1,1,new TGeoTranslation(0,0,0));

 double radius, offset, dx, dy, dz;

 radius = 7.5;  // 

 dx = 1.;     // crystal thickness = 2cm;
 dy = 1.5;    // 4cm=8 crystals, 8cm - 16 5mm crystals
 dz = 15./2;

 offset = radius+dx;

 int nw = 8;
 for (int iw=0; iw<nw; iw++) { 
   // 6 wedges 

   double phi = 360./nw*iw;
   double phi_rad = phi*TMath::Pi()/180;

   TGeoVolume *box=geom->MakeBox("Box2",Copper,1,dy,dz);
   box->SetFillColor(4);
   box->SetLineColor(4);

   TGeoRotation* rot = new TGeoRotation(Form("r02i",i),0,0,phi);

   double dy1 = dy+0.01;
   top->AddNodeOverlap(box,2*i+1,new TGeoCombiTrans(offset*cos(phi_rad)-dy1*sin(phi_rad),
						    offset*sin(phi_rad)+dy1*cos(phi_rad),
						    dz,
						    rot));
   top->AddNodeOverlap(box,2*i+2,new TGeoCombiTrans(offset*cos(phi_rad)+dy1*sin(phi_rad),
						    offset*sin(phi_rad)-dy1*cos(phi_rad),
						    dz,
						    rot));

   double dx1 = 0.5;
   double dz1 = dz+3;

   TGeoVolume *box_ele = geom->MakeBox("box_ele",Copper,dx1,2*dy1,dz1);
   box_ele->SetFillColor(7);
   box_ele->SetLineColor(7);

   double off1 = radius+2*dx+dx1;

   top->AddNodeOverlap(box_ele,i+1,new TGeoCombiTrans(off1*cos(phi_rad),
						      off1*sin(phi_rad),
						      dz1,
						      rot));
 }
 
 top->SetVisibility(0);
 geom->CloseGeometry();
 
 top->Draw("ogl");

}

