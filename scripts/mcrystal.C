//

int mcrystal_z_scan(int Ix = 0, int Iy = 0) {

  solid_crystal* s = new solid_crystal();

  s->fX0 = 0.1*Ix;
  s->fY0 = 0.1*Iy;

  for (int iz=0; iz<10; iz++) {
    s->fZ0 = iz*0.1;
    s->Run(500);
    s->SaveHist(Form("mcrystal_zscan_%02i_%02i_%02i.hist",Ix,Iy,iz));
  }
  
}

int mcrystal_y_scan(int Ix = 0, int Iz = 0) {

  solid_crystal* s = new solid_crystal();

  s->fX0 = 0.1*Ix;
  s->fZ0 = 0.1*Iz;

  for (int iy=0; iy<10; iy++) {
    s->fY0 = iy*0.1;
    s->Run(500);
    s->SaveHist(Form("mcrystal_yscan_%02i_%02i_%02i.hist",Ix,iy,Iz));
  }
}

//-----------------------------------------------------------------------------
int mcrystal_uniform(double Dx, double Dy, double Dz, double SipmDx, double SipmDy,int NEvents, double Gap, double Yield) {
  char fn[200];

  mcrystal* s = new mcrystal(Dx,Dy,Dz,SipmDx,SipmDy,Gap,Yield);

  s->fPosMode = 1;
  s->Run(NEvents);
  sprintf(fn,"mcrystal_uniform_lyso_%2d_%2d_%2d_sipm_%02d_%02d_%02d_%002d.hist",
	  (int) (20*Dx), 
	  (int) (20*Dy), 
	  (int) (20*Dz),
	  (int) (10*SipmDx), 
	  (int) (10*SipmDy),
	  (int) (10*Gap),
	  (int) (Yield)
	  );
	  
  s->SaveHist(fn);
  return 0;
}

//-----------------------------------------------------------------------------
int mcrystal_uniform_20_20_20_02_02_00_31500(int NEvents = 1000) {
  return mcrystal_uniform(1.,1.,1,0.2,0.2,NEvents, 0.0, 31500);
}

//-----------------------------------------------------------------------------
int mcrystal_uniform_20_20_20_02_02_00_1000(int NEvents = 1000) {
  return mcrystal_uniform(1.,1.,1,0.2,0.2,NEvents,0.0, 1000);
}

//-----------------------------------------------------------------------------
int mcrystal_uniform_20_20_20_02_02_00_13000(int NEvents = 1000) {
  return mcrystal_uniform(1.,1.,1.,0.2,0.2,NEvents, 0.0, 13000);
}

//-----------------------------------------------------------------------------
int mcrystal_uniform_20_20_20_04_04_01(int NEvents = 1000) {
  return mcrystal_uniform(1.,1.,1.,0.4,0.4,NEvents,0.1);
}

//-----------------------------------------------------------------------------
int mcrystal_uniform_20_20_30_02_02(int NEvents = 1000) {
  return mcrystal_uniform(1.,1.,1.5,0.2,0.2,NEvents);
}

//-----------------------------------------------------------------------------
int mcrystal_uniform_20_20_30_04_04(int NEvents = 1000) {
  return mcrystal_uniform(1.,1.,1.5,0.4,0.4,NEvents);
}

//-----------------------------------------------------------------------------
int mcrystal_uniform_40_40_30_02_02(int NEvents = 1000) {
  return mcrystal_uniform(2.,2.,1.5,0.2,0.2,NEvents);
}

//-----------------------------------------------------------------------------
int mcrystal_uniform_40_40_30_04_04(int NEvents = 1000) {
  return mcrystal_uniform(2.,2.,1.5,0.4,0.4,NEvents);
}
//-----------------------------------------------------------------------------i
