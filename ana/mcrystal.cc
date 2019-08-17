/////////////////////T//////////////////////////////////////////////////////////
// different sides - different inefficiencies
///////////////////////////////////////////////////////////////////////////////

#include "TMath.h"
#include "TROOT.h"
#include "TFile.h"
#include "ana/mcrystal.hh"
#include "murat/plot/smooth_new.hh"
#include "Stntuple/val/stntuple_val_functions.hh"

ClassImp(mcrystal)
//-----------------------------------------------------------------------------
mcrystal::mcrystal(double Dx, double Dy, double Dz, 
		   double SipmDx, double SipmDy, double Gap, double Yield): TNamed() 
{
  float par[10];

  int   np;

  fGm = TGeometryManager::Instance();

  double const pos[] = { 0., 0., 0. };

  double dim[3];

  dim[0] = Dx;
  dim[1] = Dy;
  dim[2] = Dz;

  fSipmDx = SipmDx;
  fSipmDy = SipmDy;
  fGap    = Gap;
  fYield = Yield;

  fPosMode = 0;


  double precision = 0.01;

  fGm->DeclareTrackingMedium(fMedium, "air", 
			     fGm->GetMaterial("AIR"), 
			     TG3Constants::kSensitiveVolume, 
			     TG3Constants::kNoMagneticField, 
			     0, 
			     TG3Constants::kAuto, 
			     TG3Constants::kAuto, 
			     TG3Constants::kAuto, 
			     precision,
			     TG3Constants::kAuto,
			     par,np);

  TG3Volume* vol;

  fGm->CreateVolume(vol,
		    "crystal ",
		    "BOX",
		    fMedium,
		    dim,
		    NULL,
		    pos,
		    0,
		    TG3Constants::kOnly);

  fCrystal = (TG3Box*) vol;

  fX0 = 0.;
  fY0 = 0.;
  fZ0 = 0.;

  fTracingMode = 2;

  fNPhMean  = fYield; // 13000  LYSO:Ce 26000 photons/MeV

  fReflEff  = 0.95;
  fPhotoEff = 0.25;

  fRn = new TRandom3();

  fFun = new TF1("fFun",mcrystal::Fun,-5,5,4);

  fFun->SetParameters(100,100,0.1,0.2);
//-----------------------------------------------------------------------------
// folders
//-----------------------------------------------------------------------------
  TObject* o = gROOT->GetRootFolder()->FindObject("Ana");
  if (o != 0) {
    gROOT->GetRootFolder()->Remove(o);
    o->Delete();
  }

  fAnaFolder  = gROOT->GetRootFolder()->AddFolder("Ana","TStnAna Folder");

  fFolder     = fAnaFolder->AddFolder("mcrystal","mcrystal");

  fHistFolder = fFolder->AddFolder("Hist","ListOfHistograms");

  TH1::AddDirectory(0);

  for (int i=0; i<kNEventHistSets; i++) {
    fHist.fEvent[i] = 0;
  }

  char ref_file_name[200];
  sprintf(ref_file_name,"hist/mcrystal/mcrystal_uniform_lyso_%2d_%2d_%2d_sipm_%02d_%02d_%02d_%002d.hist",
	  (int) (20*fCrystal->GetDx()),
	  (int) (20*fCrystal->GetDy()),
	  (int) (20*fCrystal->GetDz()),
	  (int) (10*fSipmDx), 
	  (int) (10*fSipmDy),
	  (int) (10*fGap),
	  (int) (fYield));

  

  fReferenceHistFile = TFile::Open(ref_file_name);

  TProfile  *hy(0), *hy4(0), *hz2(0), *hz3(0), *hzt_vs_s0(0), *hzt_vs_s1(0);
  if (fReferenceHistFile) {
    hy  = gh2(fReferenceHistFile->GetName(),"mcrystal","evt_0/dy2_vs_yr2")->ProfileX();
    hy4 = gh2(fReferenceHistFile->GetName(),"mcrystal","evt_0/dy4_vs_yr4")->ProfileX();
    hz2 = gh2(fReferenceHistFile->GetName(),"mcrystal","evt_0/dz2_vs_zr2")->ProfileX();
    hz3 = gh2(fReferenceHistFile->GetName(),"mcrystal","evt_0/dz3_vs_zr3")->ProfileX();

					// need at least 10000 events

    hzt_vs_s0 = gh2(fReferenceHistFile->GetName(),"mcrystal","evt_0/s0_vs_zt")->ProfileY("s0_vs_zt_py",1,-1,"s");
    hzt_vs_s0->Rebin(2);
    hzt_vs_s1 = gh2(fReferenceHistFile->GetName(),"mcrystal","evt_0/s1_vs_zt")->ProfileY("s1_vs_zt_py",1,-1,"s");
    hzt_vs_s1->Rebin(2);
  }

  fYCorrFunction     = new smooth_new(hy);
  fY4CorrFunction    = new smooth_new(hy4);
  fZCorrFunction[2]  = new smooth_new(hz2);
  fZCorrFunction[3]  = new smooth_new(hz3);

					// use for extracting Z4

  fZVsSigma[0] = new smooth_new(hzt_vs_s0);
  fZVsSigma[1] = new smooth_new(hzt_vs_s1);

  BookHistograms();
}


//-----------------------------------------------------------------------------
mcrystal::~mcrystal() {
}


//-----------------------------------------------------------------------------
double mcrystal::Fun(double* X , double* P) {
  double f, dx;

  dx = (X[0]-P[2])/P[3];

  f = P[0]+P[1]*TMath::Exp(-dx*dx/2.);

  return f;
}

//_____________________________________________________________________________
void     mcrystal::AddHistogram(TObject* hist, const char* FolderName) {
  TFolder* fol = (TFolder*) fFolder->FindObject(FolderName);
  fol->Add(hist); 
}

//_____________________________________________________________________________
void mcrystal::HBook1F(TH1F*& Hist, const char* Name, const char* Title,
			 Int_t Nx, Double_t XMin, Double_t XMax,
			 const char* FolderName)
{
  // book 2D histogram, add it to the module's list of histograms and 
  // return pointer to it to the user

  Hist = new TH1F(Name,Title,Nx,XMin,XMax);
  AddHistogram(Hist,FolderName);
}

//_____________________________________________________________________________
void mcrystal::HBook2F(TH2F*& Hist, const char* Name, const char* Title,
			 Int_t Nx, Double_t XMin, Double_t XMax,
			 Int_t Ny, Double_t YMin, Double_t YMax,
			 const char* FolderName)
{
  // book 2D histogram, add it to the module's list of histograms and 
  // return pointer to it to the user

  Hist = new TH2F(Name,Title,Nx,XMin,XMax,Ny,YMin,YMax);
  AddHistogram(Hist,FolderName);
}

//_____________________________________________________________________________
void mcrystal::HProf(TProfile*& Hist, const char* Name, const char* Title,
		       Int_t Nx, Double_t XMin, Double_t XMax,
		       Double_t YMin, Double_t YMax,
		       const char* FolderName)
{
  // book 2D histogram, add it to the module's list of histograms and 
  // return pointer to it to the user

  Hist = new TProfile(Name,Title,Nx,XMin,XMax,YMin,YMax);
  AddHistogram(Hist,FolderName);
}


//_____________________________________________________________________________
void mcrystal::ResetHistograms(TFolder* Folder, const char* Opt) {
  // internal method...

  TObject  *o1;

  if (Folder == 0) Folder = fFolder;

  if (strcmp(Opt,"all") == 0) {

    TIter    it1(Folder->GetListOfFolders());

    while ((o1 = it1.Next())) {
      if (o1->InheritsFrom("TFolder")) {
	ResetHistograms((TFolder*) o1,Opt);
      }
      else if (o1->InheritsFrom("TH1")) {
	((TH1*) o1)->Reset();
      }
    }
  }
  else {
//-----------------------------------------------------------------------------
// reset per-event histograms - occupancy profiles for a given event
//-----------------------------------------------------------------------------
    for (int i=0; i<kNEventHistSets; i++) {
      if (fHist.fEvent[i]) {
	fHist.fEvent[i]->fYVsX[0]->Reset();
	fHist.fEvent[i]->fYVsX[1]->Reset();
	fHist.fEvent[i]->fYVsX[2]->Reset();
	fHist.fEvent[i]->fYVsX[3]->Reset();
	fHist.fEvent[i]->fYVsX[4]->Reset();
      }
    }
  }
}


//-----------------------------------------------------------------------------
int mcrystal::BookEventHistograms(EventHist_t* Hist, const char* Folder) {

  int nbx, nby;

  double dx, dy, dz;

  dx = fCrystal->GetDx();
  dy = fCrystal->GetDy();
  dz = fCrystal->GetDz();

  nbx = (int) (fCrystal->GetDx()*2/fSipmDx);
  nby = (int) (fCrystal->GetDx()*2/fSipmDx);

  HBook2F(Hist->fYVsX[0],"y_vs_x_0",Form("%s: y vs x[0]",Folder),nbx,-dx,dy,nby,-dy,dy,Folder);
  HBook2F(Hist->fYVsX[1],"y_vs_x_1",Form("%s: y vs x[1]",Folder),nbx,-dx,dy,nby,-dy,dy,Folder);
  HBook2F(Hist->fYVsX[2],"y_vs_x_2",Form("%s: y vs x[2]",Folder),nbx,-dx,dy,nby,-dy,dy,Folder);
  HBook2F(Hist->fYVsX[3],"y_vs_x_3",Form("%s: y vs x[3]",Folder),nbx,-dx,dy,nby,-dy,dy,Folder);
  HBook2F(Hist->fYVsX[4],"y_vs_x_4",Form("%s: y vs x[4]",Folder),nbx,-dx,dy,nby,-dy,dy,Folder);

  HBook1F(Hist->fNRef[0],"nrefl_0" ,Form("%s: N reflections [0]",Folder),100,0,100,Folder);
  HBook1F(Hist->fNRef[1],"nrefl_1" ,Form("%s: N reflections [1]",Folder),100,0,100,Folder);
  HBook1F(Hist->fNRef[2],"nrefl_2" ,Form("%s: N reflections [2]",Folder),100,0,100,Folder);
  HBook1F(Hist->fNRef[3],"nrefl_3" ,Form("%s: N reflections [3]",Folder),100,0,100,Folder);
  HBook1F(Hist->fNRef[4],"nrefl_4" ,Form("%s: N reflections [4]",Folder),100,0,100,Folder);

  HBook1F(Hist->fNPhotons,"nph" ,Form("%s: N photons",Folder),200,0,20000,Folder);

  HBook1F(Hist->fDyRec[0],"dy0" ,Form("%s: DY rec[0]",Folder),100,-dy,dy,Folder);
  HBook1F(Hist->fDyRec[1],"dy1" ,Form("%s: DY rec[1]",Folder),100,-dy,dy,Folder);
  HBook1F(Hist->fDyRec[2],"dy2" ,Form("%s: DY rec[2]",Folder),100,-dy,dy,Folder);

  HBook1F(Hist->fSigY[0],"sigy_0" ,Form("%s: Sig Y[0]",Folder),100,-1.,1,Folder);
  HBook1F(Hist->fSigY[1],"sigy_1" ,Form("%s: Sig Y[1]",Folder),100,-1.,1,Folder);

  HBook2F(Hist->fSigVsZt[0],"s0_vs_zt" ,Form("%s: Sig [0] Vs Zt",Folder),100,-dz,dz,100,0.,2*dy,Folder);
  HBook2F(Hist->fSigVsZt[1],"s1_vs_zt" ,Form("%s: Sig [1] Vs Zt",Folder),100,-dz,dz,100,0.,2*dy,Folder);

  HBook2F(Hist->fSigVsZr2[0],"s0_vs_zr2" ,Form("%s: Sig [0] Vs ZR2",Folder),100, -dz,dz,100,0.,2*dy,Folder);
  HBook2F(Hist->fSigVsZr2[1],"s1_vs_zr2" ,Form("%s: Sig [1] Vs ZR2",Folder),100, -dz,dz,100,0.,2*dy,Folder);

  HBook1F(Hist->fDxRec[0],"dxr_0" ,Form("%s: DX rec[0]",Folder),100,-dy,dy,Folder);
  HBook1F(Hist->fDxRec[1],"dxr_1" ,Form("%s: DX rec[1]",Folder),100,-dy,dy,Folder);
  HBook1F(Hist->fDxRec[2],"dxr_2" ,Form("%s: DX rec[2]",Folder),100,-dy,dy,Folder);

  HBook1F(Hist->fSigX[0],"sigx_0" ,Form("%s: Sig X[0]",Folder),100,-1.,1,Folder);
  HBook1F(Hist->fSigX[1],"sigx_1" ,Form("%s: Sig X[1]",Folder),100,-1.,1,Folder);

  HBook1F(Hist->fDzRec[0],"dzr_0" ,Form("%s: Dz rec[0]",Folder),100,-dz,dz,Folder);
  HBook1F(Hist->fDzRec[1],"dzr_1" ,Form("%s: Dz rec[1]",Folder),100,-dz,dz,Folder);
  HBook1F(Hist->fDzRec[2],"dzr_2" ,Form("%s: Dz rec[2]",Folder),100,-dz,dz,Folder);

  HBook2F(Hist->fDy0VsZr2,"dy0_vs_zr2",Form("%s: dy0 vs ZRec[2]",Folder),100,-dz,dz,100,-dy,dy,Folder);
  HBook2F(Hist->fDy1VsZr2,"dy1_vs_zr2",Form("%s: dy1 vs ZRec[2]",Folder),100,-dz,dz,100,-dy,dy,Folder);

  HBook2F(Hist->fDy2VsYt,"dy2_vs_yt",Form("%s: dy2 vs y(true)",Folder),100,-dy,dy,100,-dy,dy,Folder);
  HBook2F(Hist->fDy2VsZt,"dy2_vs_zt",Form("%s: dy2 vs Z(true)",Folder),100,-dz,dz,100,-dy,dy,Folder);
  HBook2F(Hist->fDz2VsYt,"dz2_vs_yt",Form("%s: dz2 vs y(true)",Folder),100,-dy,dy,100,-dz,dz,Folder);
  HBook2F(Hist->fDz2VsZt,"dz2_vs_zt",Form("%s: dz2 vs Z(true)",Folder),100,-dz,dz,100,-dz,dz,Folder);

  HBook2F(Hist->fDz3VsZt ,"dz3_vs_zt" ,Form("%s: dz3 vs Z(true)",Folder),100,-dz,dz,100,-dz,dz,Folder);
  HBook2F(Hist->fDz3VsZr3,"dz3_vs_zr3",Form("%s: dz3 vs ZR3",Folder),100,-dz,dz,100,-dz,dz,Folder);
  HBook2F(Hist->fDzc3VsZr3,"dzc3_vs_zr3",Form("%s: dzc3 vs ZR3",Folder),100,-dz,dz,100,-dz,dz,Folder);

  HBook2F(Hist->fDz4VsZt,"dz4_vs_zt",Form("%s: dz4 vs Z(true)",Folder),100,-dz,dz,100,-dz,dz,Folder);

  HBook2F(Hist->fDy2VsYr2,"dy2_vs_yr2",Form("%s: dy2 vs Y2(rec)",Folder),100,-dy,dy,100,-dy,dy,Folder);
  HBook2F(Hist->fDz2VsZr2,"dz2_vs_zr2",Form("%s: dz2 vs Z2(rec)",Folder),100,-dz,dz,100,-dz,dz,Folder);

  HBook2F(Hist->fQmmyVsZ[0],"qmmy_vs_z_0",Form("%s: qmmy[0] vs Z",Folder),100,-dz,dz,100,0,1,Folder);
  HBook2F(Hist->fQmmyVsZ[1],"qmmy_vs_z_1",Form("%s: qmmy[1] vs Z",Folder),100,-dz,dz,100,0,1,Folder);

  HBook2F(Hist->fQmmxVsZ[0],"qmmx_vs_z_0",Form("%s: qmmx[0] vs Z",Folder),100,-dz,dz,100,0,1,Folder);
  HBook2F(Hist->fQmmxVsZ[1],"qmmx_vs_z_1",Form("%s: qmmx[1] vs Z",Folder),100,-dz,dz,100,0,1,Folder);


  HBook2F(Hist->fDx2cVsXr2,"dx2c_vs_xr2",Form("%s: dx2c vs XR2(rec)",Folder),100,-dx,dx,100,-dx,dx,Folder);
  HBook2F(Hist->fDy2cVsYr2,"dy2c_vs_yr2",Form("%s: dy2c vs YR2(rec)",Folder),100,-dy,dy,100,-dy,dy,Folder);
  HBook2F(Hist->fDz2cVsZr2,"dz2c_vs_zr2",Form("%s: dz2c vs ZR2(rec)",Folder),100,-dz,dz,100,-dz,dz,Folder);

  HBook2F(Hist->fDy3VsYt,"dy3_vs_yt",Form("%s: dy3 vs y(true)",Folder),100,-dy,dy,100,-dy,dy,Folder);
  HBook2F(Hist->fDy3VsZt,"dy3_vs_zt",Form("%s: dy3 vs Z(true)",Folder),100,-dz,dz,100,-dy,dy,Folder);

  HBook2F(Hist->fDy3VsY3,"dy3_vs_y3",Form("%s: dy3 vs y3",Folder),100,-dy,dy,100,-dy,dy,Folder);

  HBook2F(Hist->fDy4VsYt,"dy4_vs_yt",Form("%s: dy4 vs y(true)",Folder),100,-dy,dy,100,-dy,dy,Folder);
  HBook2F(Hist->fDy4VsZt,"dy4_vs_zt",Form("%s: dy4 vs Z(true)",Folder),100,-dz,dz,100,-dy,dy,Folder);

  HBook2F(Hist->fDy4VsYr4,"dy4_vs_yr4",Form("%s: dy4 vs Yr4",Folder),100,-dy,dy,100,-dy,dy,Folder);
  HBook2F(Hist->fDy4VsZc2,"dy4_vs_zc2",Form("%s: dy4 vs Zc2",Folder),100,-dz,dz,100,-dy,dy,Folder);

  HBook2F(Hist->fDy4cVsYr4,"dy4c_vs_yr4",Form("%s: dy4c vs Yr4",Folder),100,-dy,dy,100,-dy,dy,Folder);
  HBook2F(Hist->fDy4cVsZc2,"dy4c_vs_zc2",Form("%s: dy4c vs Zc2",Folder),100,-dz,dz,100,-dy,dy,Folder);

  HBook2F(Hist->fQmmy2VsY[0],"qmmy2_vs_y_0",Form("%s: qmmy2[0] vs Y",Folder),100,-dy,dy,100,0,1,Folder);
  HBook2F(Hist->fQmmy2VsY[1],"qmmy2_vs_y_1",Form("%s: qmmy2[3] vs Y",Folder),100,-dy,dy,100,0,1,Folder);

  HBook2F(Hist->fQmmy2VsZ[0],"qmmy2_vs_z_0",Form("%s: qmmy2[0] vs Z",Folder),100,-dz,dz,100,0,1,Folder);
  HBook2F(Hist->fQmmy2VsZ[1],"qmmy2_vs_z_1",Form("%s: qmmy2[1] vs Z",Folder),100,-dz,dz,100,0,1,Folder);

  HBook2F(Hist->fYpVsXp[0],"yp_vs_xp_0",Form("%s: Yp Vs Xp[0]",Folder),200,-dx,dx,200,-dy,dy,Folder);
  HBook2F(Hist->fYpVsXp[1],"yp_vs_xp_1",Form("%s: Yp Vs Xp[1]",Folder),200,-dx,dx,200,-dy,dy,Folder);

  return 0;
}


//-----------------------------------------------------------------------------
int mcrystal::BookHistograms() {

  TFolder* fol;
  TFolder* hist_folder;

  char folder_name[200];

  int  book_event_histset[kNEventHistSets];
  for (int i=0; i<kNEventHistSets; i++) book_event_histset[i] = 0;

  book_event_histset[0] = 1; // all 

  hist_folder = (TFolder*) fFolder->FindObject("Hist");

  for (int i=0; i<kNEventHistSets; i++) {
    if (book_event_histset[i] != 0) {
      sprintf(folder_name,"evt_%i",i);
      fol = (TFolder*) hist_folder->FindObject(folder_name);
      if (! fol) fol = hist_folder->AddFolder(folder_name,folder_name);
      if (fHist.fEvent[i] == 0) fHist.fEvent[i] = new EventHist_t;
      BookEventHistograms(fHist.fEvent[i],Form("Hist/%s",folder_name));
    }
  }
    
  return 0;
}

//-----------------------------------------------------------------------------
int mcrystal::FillProfileHistograms(EventHist_t* Hist) {

  float x, y, r, theta;

  r = fRn->Rndm();
  if (r > fPhotoEff) goto END;
  
  x = fLastPoint.GetPosition()->X();
  y = fLastPoint.GetPosition()->Y();

  theta = fLastPoint.GetDirection()->Theta()*360./TMath::TwoPi();
  if (theta > 90) theta = 180-theta;

  if (fStop == 1) {
					// photon reached Z=Zmax face
    Hist->fYVsX[0]->Fill(x,y);
    Hist->fNRef[0]->Fill(fNReflections);
    
    if (theta < 60.) {
      Hist->fYVsX[1]->Fill(x,y);
      Hist->fNRef[1]->Fill(fNReflections);
    }
  }
  else if (fStop == 2) {
    Hist->fYVsX[2]->Fill(x,y);
    Hist->fNRef[2]->Fill(fNReflections);
  }
  else if (fStop == 3) { 
					// photon reached the Z=Zmin face
    Hist->fYVsX[3]->Fill(x,y);
    Hist->fNRef[3]->Fill(fNReflections);
 
    if (theta < 60.) {
      Hist->fYVsX[4]->Fill(x,y);
      Hist->fNRef[4]->Fill(fNReflections);
    }
  }
					// X:Y distributions for photons
  if (fLastPoint.Z() < 0) {
    Hist->fYpVsXp[0]->Fill(x,y);
  }
  else {
    Hist->fYpVsXp[1]->Fill(x,y);
  }

 END:;

  return 0;
}

//-----------------------------------------------------------------------------
int mcrystal::FillRecoHistograms(EventHist_t* Hist) {

  float   x, y, theta, qmmx_0, qmmx_1, qmmy_0, qmmy_1;
  double  dxr[10], dyr[10], dzr[10], dyc[10], sig[2], dzc[10];

					// photon reached the Z=Zmax face
  dyr[0] = fRecY[0].fMean-fY0;
  dyr[1] = fRecY[1].fMean-fY0;
  dyr[2] = fYRec[2]-fY0;
  dyr[3] = fYRec[3]-fY0;
  dyr[4] = fYRec[4]-fY0;

  sig[0] = sqrt(fRecY[0].fSigma*fRecY[0].fSigma + fRecX[0].fSigma*fRecX[0].fSigma);
  sig[1] = sqrt(fRecY[1].fSigma*fRecY[1].fSigma + fRecX[1].fSigma*fRecX[1].fSigma);

  dyc[2] = fYCorr[2]-fY0;
  dyc[4] = fYCorr[4]-fY0;

  dxr[0] = fRecX[0].fMean-fX0;
  dxr[1] = fRecX[1].fMean-fX0;
  dxr[2] = fXRec[2]-fX0;

  dzr[0] = fZRec[0]-fZ0;
  dzr[1] = fZRec[1]-fZ0;
  dzr[2] = fZRec[2]-fZ0;
  dzr[3] = fZRec[3]-fZ0;
  dzr[4] = fZRec[4]-fZ0;

  dzc[2] = fZCorr[2]-fZ0;
  dzc[3] = fZCorr[3]-fZ0;
  dzc[4] = fZCorr[4]-fZ0;

  Hist->fDxRec[0]->Fill(dxr[0]);
  Hist->fSigX[0]->Fill(fRecX[0].fSigma);

  Hist->fDyRec[0]->Fill(dyr[0]);
  Hist->fSigY[0]->Fill(fRecY[0].fSigma);

  Hist->fDy0VsZr2->Fill(fZRec[2],dyr[0]);
  Hist->fDy1VsZr2->Fill(fZRec[2],dyr[1]);
					// photon reached the Z=Zmax face
  Hist->fDxRec[1]->Fill(dxr[1]);
  Hist->fSigX[1]->Fill(fRecX[1].fSigma);

  Hist->fDyRec[1]->Fill(dyr[1]);
  Hist->fSigY[1]->Fill(fRecY[1].fSigma);

  Hist->fSigVsZt[0]->Fill(fZ0,sig[0]);
  Hist->fSigVsZt[1]->Fill(fZ0,sig[1]);

  Hist->fSigVsZr2[0]->Fill(fZRec[2],sig[0]);
  Hist->fSigVsZr2[1]->Fill(fZRec[2],sig[1]);

  Hist->fDxRec[2]->Fill(dxr[2]);
  Hist->fDyRec[2]->Fill(dyr[2]);

  Hist->fDzRec[0]->Fill(dzr[0]);
  Hist->fDzRec[1]->Fill(dzr[1]);
  Hist->fDzRec[2]->Fill(dzr[2]);

  Hist->fDy2VsYt->Fill(fY0,dyr[2]);
  Hist->fDy2VsZt->Fill(fZ0,dyr[2]);

  Hist->fDz2VsYt->Fill(fY0,dzr[2]);
  Hist->fDz2VsZt->Fill(fZ0,dzr[2]);

  Hist->fDz3VsZt->Fill(fZ0,dzr[3]);
  Hist->fDz3VsZr3->Fill(fZRec[3],dzr[3]);
  Hist->fDzc3VsZr3->Fill(fZRec[3],dzc[3]);

  Hist->fDz4VsZt->Fill(fZ0,dzr[4]);

  Hist->fDy2VsYr2->Fill(fYRec[2],dyr[2]);
  Hist->fDz2VsZr2->Fill(fZRec[2],dzr[2]);

  // here also fill fQmmVsZ

  qmmy_0 = fRecY[0].fQMin/(fRecY[0].fQMax + 1e-12);
  qmmx_0 = fRecX[0].fQMin/(fRecY[0].fQMax + 1e-12);
  qmmy_1 = fRecY[1].fQMin/(fRecY[1].fQMax + 1e-12);
  qmmx_1 = fRecX[1].fQMin/(fRecY[1].fQMax + 1e-12);

  Hist->fQmmyVsZ[0]->Fill(fZ0,qmmy_0);
  Hist->fQmmyVsZ[1]->Fill(fZ0,qmmy_1);

  Hist->fQmmxVsZ[0]->Fill(fZ0,qmmx_0);
  Hist->fQmmxVsZ[1]->Fill(fZ0,qmmx_1);

  Hist->fDx2cVsXr2->Fill(fXRec[2],fXCorr[2]-fX0);
  Hist->fDy2cVsYr2->Fill(fYRec[2],dyc[2]);
  Hist->fDz2cVsZr2->Fill(fZRec[2],dzc[2]);

  Hist->fDy3VsYt->Fill(fY0,dyr[3]);
  Hist->fDy3VsZt->Fill(fZ0,dyr[3]);
  Hist->fDy3VsY3->Fill(fYRec[3],dyr[3]);
 
  Hist->fDy4VsYt->Fill(fY0,dyr[4]);
  Hist->fDy4VsZt->Fill(fZ0,dyr[4]);

  Hist->fDy4VsYr4->Fill(fYRec [4],dyr[4]);
  Hist->fDy4VsZc2->Fill(fZCorr[2],dyr[4]);

  Hist->fDy4cVsYr4->Fill(fYRec [4],dyc[4]);
  Hist->fDy4cVsZc2->Fill(fZCorr[2],dyc[4]);

  Hist->fQmmy2VsY[0]->Fill(fY0,fRecY[0].fQMin2/fRecY[0].fQMax2);
  Hist->fQmmy2VsY[1]->Fill(fY0,fRecY[1].fQMin2/fRecY[1].fQMax2);

  Hist->fQmmy2VsZ[0]->Fill(fZ0,fRecY[0].fQMin2/fRecY[0].fQMax2);
  Hist->fQmmy2VsZ[1]->Fill(fZ0,fRecY[1].fQMin2/fRecY[1].fQMax2);

  return 0;
}

//-----------------------------------------------------------------------------
int mcrystal::FillHistograms() {

  FillRecoHistograms(fHist.fEvent[0]);
  
  return 0;
}

//-----------------------------------------------------------------------------
  int mcrystal::TracePhoton(TTrajectoryPoint* Start, TG3Box* Vol, int Mode) {

  double            x, y, z, nx, ny, nz, r, s, sx, sy, sz, smin, p;
  int               reflection;
  TTrajectoryPoint* tp;

  tp = &fLastPoint;

  tp->SetPoint(Start->X(),Start->Y(),Start->Z(),
	       Start->Nx(),Start->Ny(),Start->Nz(),
	       Start->S(),Start->GetPTotal());

  fStop = 0;

  fNReflections = 0;

  while (fStop == 0) {
					// find next relection
					// calculate path till the X-wall
    nx = tp->Nx();
    ny = tp->Ny();
    nz = tp->Nz();

    if (nx > 0) sx = ( Vol->GetDx()-tp->X())/nx;
    else        sx = (-Vol->GetDx()-tp->X())/nx;

					// calculate path till the Y-wall

    if (ny > 0) sy = ( Vol->GetDy()-tp->Y())/ny;
    else        sy = (-Vol->GetDy()-tp->Y())/ny;

					// calculate path till the Z-wall

    if (nz > 0) sz = ( Vol->GetDz()-tp->Z())/nz;
    else        sz = (-Vol->GetDz()-tp->Z())/nz;

					// flag the wall reached first
    if (sx < sy) {
      if (sx < sz) reflection = 1;
      else         reflection = 3;
    }
    else {
      if (sy < sz) reflection = 2;
      else         reflection = 3;
    }

    if (reflection == 1) {
					// reflection from X-wall
      smin = sx;
      r    = fRn->Rndm();
      if (r < fReflEff) nx   = -nx; 
      else              fStop = 2;
    }
    else if (reflection == 2) {
					// reflection from X-wall
      smin = sy;
      r    = fRn->Rndm();
      if (r < fReflEff) ny = -ny;
      else              fStop = 2;
    }
    else if (reflection == 3) {
					// Z-boundary
      smin = sz;
      if (Mode == 1) {
	if (tp->Nz() > 0) fStop = 1;
	else {
					// reflection
	  r  = fRn->Rndm();
	  if (r < fReflEff) nz    = -nz;
	  else              fStop = 2;
	}
      }
      else if (Mode == 2) {
					// reached Z-face, stop anyway
	if (tp->Nz() > 0) fStop = 1;
	else              fStop = 3;
      }
    }
					// update the point coordinates
    x  = tp->X()+smin*tp->Nx();
    y  = tp->Y()+smin*tp->Ny();
    z  = tp->Z()+smin*tp->Nz();
    s  = tp->S()+smin;
    p  = tp->GetPTotal();
    
    //  struct TracePhoton_Result {
    // int x;
    // int y;
    // }
    // ;

    tp->SetPoint(x,y,z,nx,ny,nz,s,p);



    if (fStop == 0) fNReflections++;
  }
//-----------------------------------------------------------------------------
// reached the right Z-end, exit
//-----------------------------------------------------------------------------
  return 0;
}


// //-----------------------------------------------------------------------------
// // zcorr = Z(rec)-Z(true),  so it needs to be subtracted from Z(rec)
// //-----------------------------------------------------------------------------
// double mcrystal::CorrZ(double ZRec){
//   double zcorr;

//   zcorr = fZCorr->Eval(ZRec);
//   return zcorr;
// }

//-----------------------------------------------------------------------------
//struct TracePhoton_Result {
// int x;
// int y;
//}
//  ;
//-----------------------------------------------------------------------------

int mcrystal::Reconstruct(TH1D* Hist, RecoRes_t* R) { 

  int nb = Hist->GetNbinsX();

  float qs[100], x[100], qso[100], xo[100], sx(0), sx2(0), sw(0), xx;

  float qmin = 1.e12;
  float qmax = -1;
//-----------------------------------------------------------------------------
// order SiPM signals 
//-----------------------------------------------------------------------------
  
  for (int i=1; i<=nb; i++) {
    x  [i-1] = Hist->GetBinCenter (i);
    qs [i-1] = Hist->GetBinContent(i);
    qso[i-1] = qs[i-1];
  }

  for (int i=0; i<=nb-1; i++) {
    for (int j=i+1; j<=nb-1; j++) {
      if (qso[j] > qso[i]) {
	xx     = qso[i];
	qso[i] = qso[j];
	qso[j] = xx;
      }
    }
  }

  qmax = qso[0];
  qmin = qso[nb-1];
					// subtract minimum
  for (int i=0; i<=nb-1; i++) {
    qs[i] = qs[i]-qmin;

    sx    = sx +x[i]*qs[i];
    sx2   = sx2+x[i]*x [i]*qs[i];
    sw    = sw+qs[i];
  }
					// calculate mean and sigma
  R->fMean  = sx/sw;
  double sig2 = sx2/sw - (sx/sw)*(sx/sw);

  if (sig2 <= 0) {
    //    printf(" ERROR: Event = %10i sig2=%g, set it to zero\n", fEventNumber, sig2);
    sig2 = 0;
    R->fSigma = 1.e-6;
  }
  else {
    R->fSigma = TMath::Sqrt(sig2);
  }

  R->fQMin   = qso[nb-1];
  R->fQMax   = qso[0];
  R->fQMin2  = qso[nb-1]+qso[nb-2];
  R->fQMax2  = qso[0]+qso[1];

  return 0;
}

//-----------------------------------------------------------------------------
int mcrystal::ReconstructEvent() {
  // start from subtracting minimum

  TH1D* h;

  double  wy0, wy1, wx0, wx1, z0, z1, ez0, ez1, wz0, wz1;
  double  sy0_2, sy1_2, sx0_2, sx1_2, sig[2];

					// Z=Zmax face

  h = fHist.fEvent[0]->fYVsX[1]->ProjectionX();
  Reconstruct(h,&fRecX[0]);
  delete h;

  h = fHist.fEvent[0]->fYVsX[1]->ProjectionY();
  Reconstruct(h,&fRecY[0]);
  delete h;
					// Z=Zmin face

  h = fHist.fEvent[0]->fYVsX[4]->ProjectionX();
  Reconstruct(h,&fRecX[1]);
  delete h;

  h = fHist.fEvent[0]->fYVsX[4]->ProjectionY();
  Reconstruct(h,&fRecY[1]);
  delete h;

  sy0_2 = fRecY[0].fSigma*fRecY[0].fSigma;
  sy1_2 = fRecY[1].fSigma*fRecY[1].fSigma;

  wy0 = 1./(sy0_2+1.e-6);
  wy1 = 1./(sy1_2+1.e-6);

  fYRec[2] = (fRecY[0].fMean*wy0+fRecY[1].fMean*wy1)/(wy0+wy1);

  fYCorr[2] = fYRec[2]-fYCorrFunction->Eval(&fYRec[2]);

  sx0_2 = fRecX[0].fSigma*fRecX[0].fSigma;
  sx1_2 = fRecX[1].fSigma*fRecX[1].fSigma;

  sig[0] = TMath::Sqrt(sx0_2+sy0_2);
  sig[1] = TMath::Sqrt(sx1_2+sy1_2);

  z0     = fZVsSigma[0]->Eval(&sig[0]);
  z1     = fZVsSigma[1]->Eval(&sig[1]);

  ez0    = fZVsSigma[0]->GetError(&z0) + 1.e-12;
  ez1    = fZVsSigma[1]->GetError(&z1) + 1.e-12;

  // wz0    = 1./(ez0*ez0);
  // wz1    = 1./(ez1*ez1);

  wz0    = 1./(sig[0]*sig[0]+1.e-12);
  wz1    = 1./(sig[1]*sig[1]+1.e-12);

  fZRec[4] =  (z0*wz0+z1*wz1)/(wz0+wz1);

  wx0 = 1./(sx0_2+1.e-6);
  wx1 = 1./(sx1_2+1.e-6);

  fXRec[2] = (fRecX[0].fMean*wx0+fRecX[1].fMean*wx1)/(wx0+wx1);

  // X- and Y-corrections should be the same

  fXCorr[2] = fXRec[2]-fYCorrFunction->Eval(&fXRec[2]);
//-----------------------------------------------------------------------------
// determine Z using Y side only
//-----------------------------------------------------------------------------
  if (fRecY[0].fSigma < fRecY[1].fSigma) {
    fZRec[0] =  fCrystal->GetDz()*(1-2*sy0_2/(sy0_2+sy1_2));
  }
  else {
    fZRec[0] = -fCrystal->GetDz()*(1-2*sy1_2/(sy0_2+sy1_2));
  }
//-----------------------------------------------------------------------------
// determine Z using X side only
//-----------------------------------------------------------------------------
  if (fSigX[0] < fSigX[1]) {
    fZRec[1] =  fCrystal->GetDz()*(1-2*sx0_2/(sx0_2+sx1_2));
  }
  else {
    fZRec[1] = -fCrystal->GetDz()*(1-2*sx1_2/(sx0_2+sx1_2));
  }
//-----------------------------------------------------------------------------
// simply average X- and Y-based solutions
//-----------------------------------------------------------------------------
  fZRec [2] = (fZRec[0]+fZRec[1])/2.;
  fZCorr[2] = fZRec[2]-fZCorrFunction[2]->Eval(&fZRec[2]);

  double wy01, wy11;

  // use Z-weighting

  wy01 = 1/((1-fZRec[2]/fDz)*(1-fZRec[2]/fDz)+1.e-12);
  wy11 = 1/((1+fZRec[2]/fDz)*(1+fZRec[2]/fDz)+1.e-12);

  fYRec[3] = (fRecY[0].fMean*wy01+fRecY[1].fMean*wy11)/(wy01+wy11);

//-----------------------------------------------------------------------------
// determine Z using X side only
//-----------------------------------------------------------------------------
  fZRec [3] = fCrystal->GetDz()*(sig[1]*sig[1]-sig[0]*sig[0])/(sig[0]*sig[0]+sig[1]*sig[1]);
  fZCorr[3] = fZRec[3]-fZCorrFunction[3]->Eval(&fZRec[3]);
//-----------------------------------------------------------------------------
// Y(4)
//-----------------------------------------------------------------------------
  if      (fZRec[2] >  0.3) {
    fYRec [4] = fRecY[0].fMean;
    fYCorr[4] = fYRec[4];
  }
  else if (fZRec[2] < -0.3) {
    fYRec [4] = fRecY[1].fMean;
    fYCorr[4] = fYRec[4];
  }
  else {
    fYRec [4] = fYCorr[2];
    fYCorr[4] = fYRec[4]-fY4CorrFunction->Eval(&fYRec[4]);
  }

  return 0;
}

//-----------------------------------------------------------------------------
int mcrystal::Run(int NEvents) {

    float  rn[2];  // to generate phi and theta

    double nx, ny, nz, theta, phi, x,y,z;


  ResetHistograms(0,"all");

  // trace photons

  for (int ievent=0; ievent<NEvents; ievent++) {
    fEventNumber = ievent;
//-----------------------------------------------------------------------------
// reset profile histograms in the beginning
//-----------------------------------------------------------------------------
    ResetHistograms(0,"event");

    fRn->RndmArray(3,rn);

    if (fPosMode == 1) {
					// randomly within the volume...
      fX0 = 2*(rn[0]-0.5)*fCrystal->GetDx();
      fY0 = 2*(rn[1]-0.5)*fCrystal->GetDy();
      fZ0 = 2*(rn[2]-0.5)*fCrystal->GetDz();
    }
      					// number of photons in this event
    fNPhotons = fRn->Poisson(fNPhMean);
					// generate parameters of i-th photo::n

    for (int iph=0; iph<fNPhotons; iph++) {

      fRn->RndmArray(2,rn);

      phi   = TMath::TwoPi()*rn[0];
      theta = TMath::ASin(2*(rn[1]-0.5));
    
      nx  = TMath::Cos(theta)*TMath::Cos(phi);
      ny  = TMath::Cos(theta)*TMath::Sin(phi);
      nz  = TMath::Sin(theta);     
      // TracePhoton_Result TracePhoton(int x, int y);   


					// trace photon till it reaches Z-face of the crystal, 
					// reflecting it from the other faces


      fPoint.SetPoint(fX0,fY0,fZ0,nx,ny,nz,0,1);

      TracePhoton(&fPoint,fCrystal,fTracingMode);
    
      x = fLastPoint.GetPosition()->X();
      y = fLastPoint.GetPosition()->Y();

      //  TracePhoton(x,y);
      
      /*      if (((((( -1.5 *fGap)-(2*fSipmDx))<x)&&(x<(-1.5*fGap)-(fSipmDx))) || 
	  ((((-.5*fGap)-(fSipmDx))<x)&&(x<(-.5*fGap))) ||
	  ((((.5*fGap)+(fSipmDx))>x)&&(x>(.5*fGap))) ||
	   (((( 1.5 *fGap)+(2*fSipmDx))>x)&&(x>(1.5*fGap)+(fSipmDx)))) &&
	  ((((( -1.5 *fGap)-(2*fSipmDy))<y)&&(y<(-1.5*fGap)-(fSipmDy))) || 
	  ((((-.5*fGap)-(fSipmDy))<y)&&(y<(-.5*fGap))) ||
	  ((((.5*fGap)+(fSipmDy))>y)&&(y>(.5*fGap))) ||
	  (((( 1.5 *fGap)+(2*fSipmDy))>y)&&(y>(1.5*fGap)+(fSipmDy))))) { */

	
      FillProfileHistograms(fHist.fEvent[0]);
      
    // else {
	//	int x = 0;
    }    

    ReconstructEvent();

    FillRecoHistograms(fHist.fEvent[0]);
  }

  return 0;
}
  
  //ReconstructEvent();

  //FillRecoHistograms(fHist.fEvent[0]);
  // return 0;



//_____________________________________________________________________________
int  mcrystal::SaveFolder(TFolder* Folder, TDirectory* Dir) {
  // save Folder into a subdirectory
  // do not write TStnModule's - for each TStnModule save contents of its
  // fFolder

  TFolder*     fol;
  TDirectory*  dir;
  TObject*     o;
//-----------------------------------------------------------------------------
// create new subdirectory in Dir to save Folder
//-----------------------------------------------------------------------------
  Dir->cd();
  //  dir = new TDirectory(Folder->GetName(),Folder->GetName(),"");
  dir = Dir->mkdir(Folder->GetName(),Folder->GetName());
  dir->cd();

//   printf(" ------------------- Dir: %s, new dir: %s\n",
// 	 Dir->GetName(),dir->GetName());


  TIter  it(Folder->GetListOfFolders());
  while ((o = it.Next())) {
//     printf(" o->GetName, o->ClassName : %-20s %-20s\n",
// 	   o->GetName(),
// 	   o->ClassName());

    if (strcmp(o->ClassName(),"TFolder") == 0) {
      SaveFolder((TFolder*) o, dir);
      //      dir->cd();
    }
    else if (! o->InheritsFrom("TStnModule")) {
      //      printf("gDirectory->GetPath = %s\n",gDirectory->GetPath());
      o->Write();
      //      gDirectory->GetListOfKeys()->Print();
    }
  }

  Dir->cd();
  return 0;
}

//_____________________________________________________________________________
void mcrystal::SaveHist(const char* Filename) {
  // save histograms booked by all the modules into a file with the given name
  // Mode = 2: save directories

  TFile* f = new TFile(Filename,"recreate");

  SaveFolder(fAnaFolder,f);

  f->Close();
  delete f;
  }


