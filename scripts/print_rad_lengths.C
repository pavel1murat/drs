///////////////////////////////////////////////////////////////////////////////
// calculate radiation lengths of materials and compounds
// call:
// -----
// .L print_rad_lengths.C++
// init()
// print_rad_lengths()
//
// element fe("Fe",26,55.845,7.874,-1.)
// element al("Al",26,55.845,7.874,-1.)

// Sidletskij: LuGdSiO5, Gd2Si2O7, Y3Al3Ga2O12, Lu3Al5O12.
///////////////////////////////////////////////////////////////////////////////

#include "TString.h" 
#include "TNamed.h"
#include "TObjArray.h"
#include "math.h" 

TObjArray* fListOfElements = 0;
TObjArray* fListOfMaterials = 0;

//-----------------------------------------------------------------------------
class element:public TNamed {
public:

  TString  fName;
  double   fA;
  double   fZ;
  double   fDensity;
  double   fX0;              // radiation length

  element() {};
  element(const char* Name, double M, double Z, double Density, double X0=-1.);
  ~element() {};

  const char* GetName   () const { return fName.Data(); }
  double      GetDensity() const { return fDensity; }
  double      GetX0     () const { return fX0; }
  double      GetA      () const { return fA ; }
  double      GetZ      () const { return fZ ; }
};

//-----------------------------------------------------------------------------
class xxx {
public: 
  xxx()  {};
  ~xxx() {};


  static double Re();              // electron classical radius

  static double Ksi(double Z);
  static double Fc (double Z);

  static double RadLength(element* E);
};


//-----------------------------------------------------------------------------
element::element(const char* Name, double Z, double A, 
		 double Density, double X0): TNamed(Name,Name) {
  fName    = Name;
  fZ       = Z;
  fA       = A;
  fDensity = Density;

  if (X0  > 0) fX0 = X0;
  else         fX0 = xxx::RadLength(this);
}



double kHPlanck      = 6.62606957e-27 ;  // [erg s]
double kHPlanckBar   = 1.05457266e-27 ;  // [erg s]
double kSpeedOfLight = 2.99792458e+10 ;  // [cm/sec]
double kAlpha        = 1/137.035999074;  // dimensionless
double kEleCharge    = 4.8032068e-10  ;  // [esu]
double kEleMass      = 9.1093897e-28  ;  // [g]
double kProMass      = 1.6726231e-24  ;  // [g]
double kNAv          = 6.0221367e+23  ;  // 


//-----------------------------------------------------------------------------
double xxx::Re() {
  double c2 = kSpeedOfLight*kSpeedOfLight;
  double e2 = kEleCharge*kEleCharge;
  double m  = kEleMass;

  double re = e2/(m*c2);

  return re;
}


//-----------------------------------------------------------------------------
double xxx::Fc(double Z) {
  double fc, x;

  x = (kAlpha*Z)*(kAlpha*Z);

  fc = x*(1./(1+x)+0.20206-0.0369*x+0.0083*x*x-0.0020*x*x*x);

  return fc;
}

//-----------------------------------------------------------------------------
double xxx::Ksi(double Z) {

  double ksi;

  ksi = log(1440./pow(Z,2./3.))/(log(183/pow(Z,1./3.))-xxx::Fc(Z));

  return ksi;
}

//-----------------------------------------------------------------------------
double xxx::RadLength(element* E) {
  double rl;

  double Z = E->fZ;
  double A = E->fA;

  double r = xxx::Re();
  double r2 = r*r;

  double a = 4*kAlpha*r2*kNAv*Z*(Z+xxx::Ksi(Z))/A*(log(183./pow(Z,1./3.))-xxx::Fc(Z));

  rl = 1/(a*E->fDensity);

  return rl;
}



//-----------------------------------------------------------------------------
class material: public TNamed {
public:
  TString   fName;
  int       fN;				// N elements
  element** fElement;
  double*   fNAtoms;
  double    fAMol;			// molar weight
  double    fDensity;
  double    fZEff;
  double    fAEff;
  double    fX0;

  material();
  material(const char* Name, int N, element** E, double* NAtoms, double Density);
  ~material();

  const char* GetName() const { return fName.Data(); }

  double    Weight(int I) { return fNAtoms[I]*fElement[I]->fA/fAMol ; }
};


//-----------------------------------------------------------------------------
material::material(): TNamed() {
  fN       = 0;
  fElement = 0;
  fNAtoms  = 0;
  fDensity = -1.;
  fX0      = -1.;
} 

//-----------------------------------------------------------------------------
material::material(const char* Name, int N, element** E, double* NAtoms, double Density):
TNamed(Name,Name) 
{

  element* e;
  double   s;

  fName    = Name;
  fN       = N;
  fElement = new element* [N];
  fNAtoms  = new double[N];

  for (int i=0; i<fN; i++) {
    fElement[i] = E[i];
    fNAtoms [i] = NAtoms[i];
  }

  fDensity = Density;

  fAMol  = 0;
  for (int i=0; i<fN; i++) {
    fAMol += fElement[i]->fA*fNAtoms[i];
  }

  s     = 0; 
  fAEff = 0;
  fZEff = 0;
  for (int i=0; i<fN; i++) {
    e = fElement[i];
    s     += Weight(i)/(e->fDensity*e->fX0);
    fAEff += Weight(i)*e->fA;
    fZEff += Weight(i)*e->fZ;
  }

  fX0 = 1./(fDensity*s);
} 

//-----------------------------------------------------------------------------
material::~material()  {
  if (fElement) {
    delete fElement;
    delete fNAtoms;
  }
}

//-----------------------------------------------------------------------------
// polyethylen : (CH_2)^n
//-----------------------------------------------------------------------------
void test4() {
  element* e[2];
  double   n[2];

  n[0] = 2;
  n[1] = 1;

  e[0] = new element ("H",  1,1.00794,0.0899e-3,-1.);
  e[1] = new element ("C",  6,12.0108,2.265    ,-1.);


  material* c = new material("Polyethylen",2,e,n,0.89);

  printf(" H     rad.len[g/cm2]: %f\n",e[0]->fX0*e[0]->fDensity);
  printf(" C     rad.len[g/cm2]: %f\n",e[1]->fX0*e[1]->fDensity);
  printf(" CH2n  rad.len[g/cm2]: %f\n",c->fX0*c->fDensity);

}


//-----------------------------------------------------------------------------
// calculate rad. lenghts for elements
//-----------------------------------------------------------------------------
void init() {

  fListOfElements  = new TObjArray();

  fListOfElements->Add(new element("H" ,  1,  1.00794  , 0.0899e-3, -1.));
  fListOfElements->Add(new element("C" ,  6,  12.0108  , 2.265    , -1.));
  fListOfElements->Add(new element("O" ,  8,  15.999   , 1.141e-3 , -1.));
  fListOfElements->Add(new element("F" ,  9,  18.998403, 1.8*10-3 , -1.));
  fListOfElements->Add(new element("Al", 13,  26.981539, 2.70     , -1.));
  fListOfElements->Add(new element("Si", 14,  28.0855  , 2.33     , -1.));
  fListOfElements->Add(new element("Cl", 17,  35.463   , 3.2*10-3 , -1.));
  fListOfElements->Add(new element("Fe", 26,  55.845   , 7.874    , -1.));
  fListOfElements->Add(new element("Ga", 31,  69.72    , 5.1      , -1.));
  fListOfElements->Add(new element("Ge", 32,  72.59    , 5.3      , -1.));
  fListOfElements->Add(new element("Y" , 39, 88.9059   , 4.47     , -1.));
  fListOfElements->Add(new element("Ba", 56, 137.33    , 3.5      , -1.));

  fListOfElements->Add(new element("Gd", 64, 157.25    , 7.9      , -1.));
  fListOfElements->Add(new element("Lu", 71, 174.97    , 9.7      , -1.));
  fListOfElements->Add(new element("Pb", 82, 207.2     ,11.34     , -1.));
  fListOfElements->Add(new element("Bi", 83, 208.9804  , 9.80     , -1.));
  fListOfElements->Add(new element("U" , 92, 238.03    ,18.95     , -1.));

//-----------------------------------------------------------------------------
// initialize materials
//-----------------------------------------------------------------------------
  fListOfMaterials = new TObjArray();

  element*  e[100];
  material* mat;
  double    n[100];
  int       nel;
//-----------------------------------------------------------------------------
// first make a material out of each known element
//-----------------------------------------------------------------------------
  nel = fListOfElements->GetEntries();
  for (int i=0; i<nel; i++) {
    e[0] = (element*) fListOfElements->At(i);
    n[0] = 1;
    fListOfMaterials->Add(new material(e[0]->GetName(),1,e,n,e[0]->GetDensity()));
  }
//-----------------------------------------------------------------------------
// water, "H2O"
//-----------------------------------------------------------------------------
  n[0] = 2;
  n[1] = 1;

  e[0] = (element*) fListOfElements->FindObject("H");
  e[1] = (element*) fListOfElements->FindObject("O");

  fListOfMaterials->Add(new material("H2O",2,e,n,1.));
//-----------------------------------------------------------------------------
// BaF2
//-----------------------------------------------------------------------------
  n[0] = 1;
  n[1] = 2;

  e[0] = (element*) fListOfElements->FindObject("Ba");
  e[1] = (element*) fListOfElements->FindObject("F");

  fListOfMaterials->Add(new material("BaF2",2,e,n,4.89));
//-----------------------------------------------------------------------------
// BG0:  Bi4Ge3012
//-----------------------------------------------------------------------------
  n[0] = 4;
  n[1] = 3;
  n[2] = 12;

  e[0] = (element*) fListOfElements->FindObject("Bi");
  e[1] = (element*) fListOfElements->FindObject("Ge");
  e[2] = (element*) fListOfElements->FindObject("O");

  fListOfMaterials->Add(new material("BG0",3,e,n,7.13));
//-----------------------------------------------------------------------------
// Al2O3
//-----------------------------------------------------------------------------
  n[0] = 2;
  n[1] = 3;

  e[0] = (element*) fListOfElements->FindObject("Al");
  e[1] = (element*) fListOfElements->FindObject("O");

  fListOfMaterials->Add(new material("Al2O3",2,e,n,3.97));
//-----------------------------------------------------------------------------
// LuGdSiO5
//-----------------------------------------------------------------------------
  nel  = 4;

  n[0] = 1;				// Lu
  n[1] = 1;				// Gd
  n[2] = 1;				// Si
  n[3] = 5;				// O5

  e[0] = (element*) fListOfElements->FindObject("Lu");
  e[1] = (element*) fListOfElements->FindObject("Gd");
  e[2] = (element*) fListOfElements->FindObject("Si");
  e[3] = (element*) fListOfElements->FindObject("O" );

  mat = new material("LuGdSiO5",nel,e,n,6.71);

  fListOfMaterials->Add(mat);
//-----------------------------------------------------------------------------
// GSO: Gd_{2}SiO_{5}:Ce, assume Ce concentration is small
//-----------------------------------------------------------------------------
  nel  = 3;

  n[0] = 2;				// Gd
  n[1] = 1;				// Si
  n[2] = 5;				// O5

  e[0] = (element*) fListOfElements->FindObject("Gd");
  e[1] = (element*) fListOfElements->FindObject("Si");
  e[2] = (element*) fListOfElements->FindObject("O" );

  mat = new material("GSO",nel,e,n,6.71);

  fListOfMaterials->Add(mat);
//-----------------------------------------------------------------------------
// GPS - Gd{2}Si{2}O{7} 
//-----------------------------------------------------------------------------
  nel  = 3;

  n[0] = 2;				// Gd2
  n[1] = 2;				// Si2
  n[2] = 7;				// O7

  e[0] = (element*) fListOfElements->FindObject("Gd");
  e[1] = (element*) fListOfElements->FindObject("Si");
  e[2] = (element*) fListOfElements->FindObject("O" );

  mat  = new material("Gd2Si2O7",nel,e,n,5.5);

  fListOfMaterials->Add(mat);
//-----------------------------------------------------------------------------
// Y3Al3Ga2O12, assume density of GSO
//-----------------------------------------------------------------------------
  nel  =  4;

  n[0] =  3;				// Y3
  n[1] =  3;				// Al3
  n[2] =  2;				// Ga2
  n[3] = 12;				// O12

  e[0] = (element*) fListOfElements->FindObject("Y" );
  e[1] = (element*) fListOfElements->FindObject("Al");
  e[2] = (element*) fListOfElements->FindObject("Ga");
  e[3] = (element*) fListOfElements->FindObject("O" );

  fListOfMaterials->Add(new material("Y3Al3Ga2O12",nel,e,n,6.71));
//-----------------------------------------------------------------------------
// Lu3Al5O12 LuAG - 
//-----------------------------------------------------------------------------
  nel  =  3;

  n[0] =  3;				// Lu3
  n[1] =  5;				// Al5
  n[2] = 12;				// O12

  e[0] = (element*) fListOfElements->FindObject("Lu");
  e[1] = (element*) fListOfElements->FindObject("Al");
  e[2] = (element*) fListOfElements->FindObject("O" );

  fListOfMaterials->Add(new material("LuAG",nel,e,n,6.7));

//-----------------------------------------------------------------------------
// LSO: Lu2SiO5:Ce, assume Ce concentration small
//-----------------------------------------------------------------------------
  nel  =  3;
  
  n[0] =  2;				// Lu2
  n[1] =  1;				// Si
  n[2] =  5;				// O5

  e[0] = (element*) fListOfElements->FindObject("Lu");
  e[1] = (element*) fListOfElements->FindObject("Si");
  e[2] = (element*) fListOfElements->FindObject("O" );

  fListOfMaterials->Add(new material("LSO",nel,e,n,7.3));
//-----------------------------------------------------------------------------
// LYSO: Lu_{2-x} Y_x SiO5:Ce, assume x=0.2e-5
//-----------------------------------------------------------------------------
  nel  =  4;
  
  n[0] =  2 - (0.2e-5);			// Lu_{2-x}
  n[1] =  0.2e-5;			// Y_{x}
  n[2] =  1;				// Si
  n[3] =  5;				// O5

  e[0] = (element*) fListOfElements->FindObject("Lu");
  e[1] = (element*) fListOfElements->FindObject("Y" );
  e[2] = (element*) fListOfElements->FindObject("Si");
  e[3] = (element*) fListOfElements->FindObject("O" );

  fListOfMaterials->Add(new material("LYSO",nel,e,n,7.3));
//-----------------------------------------------------------------------------
// LuAP: LuAlO3 (lutetium aluminum perovskit)
//-----------------------------------------------------------------------------
  nel  =  3;
  
  n[0] =  1;			// Lu
  n[1] =  1;			// Al
  n[2] =  3;			// O3

  e[0] = (element*) fListOfElements->FindObject("Lu");
  e[1] = (element*) fListOfElements->FindObject("Al" );
  e[2] = (element*) fListOfElements->FindObject("O" );

  fListOfMaterials->Add(new material("LuAP",nel,e,n,8.34));
//-----------------------------------------------------------------------------
// PbFCl: 
//-----------------------------------------------------------------------------
  nel  =  3;
  
  n[0] =  1;			// Pb
  n[1] =  1;			// F
  n[2] =  1;			// Cl

  e[0] = (element*) fListOfElements->FindObject("Pb");
  e[1] = (element*) fListOfElements->FindObject("F" );
  e[2] = (element*) fListOfElements->FindObject("Cl" );

  fListOfMaterials->Add(new material("PbFCl",nel,e,n,7.11));
}


//-----------------------------------------------------------------------------
void print_rad_lengths() {

  material* m;

  printf("--------------------------------------------------\n");
  printf("     material   density[g/cm3] X0[g/cm2] X0[cm]   \n");
  printf("--------------------------------------------------\n");

  int nmat = fListOfMaterials->GetEntries();

  for (int i=0; i<nmat; i++) {
    m = (material*) fListOfMaterials->At(i);

    printf(" %-12s  %10.2f %10.2f  %10.2f \n",
	   m->GetName(),m->fDensity,m->fX0*m->fDensity,m->fX0);
  }
}

//-----------------------------------------------------------------------------
void print_material(const char* Name) {
  element*  e;
  material* m = (material*) fListOfMaterials->FindObject(Name);

  printf("Material      Element   NA  Density[g/cm3]   X0[cm]    X0[g/cm2]\n");
  printf("%-19s         %10.2f   %10.2f  %10.2f \n",m->GetName(),
	 m->fDensity, m->fX0, m->fX0*m->fDensity);

  for (int i=0; i<m->fN; i++) {
    e = m->fElement[i];
    printf("%15s %-4s %6.3f %10.2f   %10.2f  %10.2f \n","",e->GetName(), m->fNAtoms[i], 
	   e->fDensity, e->fX0, e->fX0*e->fDensity);
  }
}


