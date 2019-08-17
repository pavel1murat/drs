// 
#include "TMath.h"
#include "Math/Integrator.h"
#include "TF1.h"
#include "Math/WrappedTF1.h"
#include "TVirtualFitter.h"

TVirtualFitter         *fgMinuit;
TF1                    *gF1;
ROOT::Math::WrappedTF1 *gWF1;
ROOT::Math::Integrator *gIG;

//-----------------------------------------------------------------------------
double  func(double* x, double* p) {

  double sigma = p[0];

  double f = TMath::Exp(-x[0]*x[0]/(2.*sigma*sigma))/sigma/sqrt(2*TMath::Pi());
  return f;
}


//-----------------------------------------------------------------------------
double integral(double X, double Sigma) {
 
  // Create the function and wrap it

  gF1->SetParameter(0,Sigma);

  ROOT::Math::WrappedTF1 wf1(*gF1);

  // Create the Integrator
  //   ROOT::Math::GSLIntegrator ig(ROOT::Math::IntegrationOneDim::kADAPTIVE);

  ROOT::Math::IntegratorOneDim ig(ROOT::Math::IntegrationOneDim::kADAPTIVE);

  ig.SetFunction(wf1, false);
  ig.SetRelTolerance(0.0001);

  // Set integration parameters

  double val1 = ig.Integral(-10.,X);
  double val2 = ig.Integral(X,10.);

  double r = val2/val1;

  // printf(" X = %12.6f Sigma = %12.6f val1 = %12.6e, val2 = %12.6e r = %12.6e\n",X,Sigma,val1,val2,r);

  return r;
}


//-----------------------------------------------------------------------------
void Fcn (int& NPar, double *Gin, double &Chi2 , double *Par, int IFlag) {

// root [ 3] test_bg("srcs01.000220.drs_pet2.hist") ...   4   3   5   3  155.000   74.000    0.000    7.667    0.450   -1.000
// root [ 4] test_bg("srcs01.000221.drs_pet2.hist") ...   3   4   2   3  167.000   86.000    6.000    9.636    0.485   -0.023
// root [ 5] test_bg("srcs01.000222.drs_pet2.hist") ...   3   4   2   3  211.000   54.000   21.000    6.818    0.231    0.069
// root [ 6] test_bg("srcs01.000223.drs_pet2.hist") ...   3   2   4   3  234.000   46.000   28.000    7.545    0.170    0.090
// root [ 7] test_bg("srcs01.000224.drs_pet2.hist") ...   3   2   4   3  171.000   98.000    9.000    9.818    0.547   -0.005
// root [ 8] test_bg("srcs01.000225.drs_pet2.hist") ...   2   3   1   3  135.000  118.000   15.000   10.818    0.863    0.034
// root [ 9] test_bg("srcs01.000226.drs_pet2.hist") ...   2   3   1   3  187.000   85.000   25.000    8.636    0.428    0.092
// root [10] test_bg("srcs01.000227.drs_pet2.hist") ...   2   1   3   3  224.000   37.000   37.000    7.273    0.137    0.137
// root [11] test_bg("srcs01.000228.drs_pet2.hist") ...   2   3   1   3  239.000   46.000   42.000    8.091    0.164    0.147
// root [12] test_bg("srcs01.000230.drs_pet2.hist") ...   2   1   3   3  156.000  131.000   10.000    8.364    0.831    0.011
// root [13] test_bg("srcs01.000231.drs_pet2.hist") ...   1   2   0   3  195.000   74.000    0.000    6.500    0.358   -1.000
// root [14] test_bg("srcs01.000232.drs_pet2.hist") ...   1   2   0   3  223.000   24.000    0.000    5.750    0.084   -1.000

  double data [] = {
    0.5, -1., 155.000,  74.000,  7.667,
    0.5, +1., 167.000,  86.000,  9.636,
    1.5, +1., 211.000,  54.000,  6.818,
    2.0, -1., 234.000,  46.000,  7.545,
    1.0, -1., 171.000,  98.000,  9.818,
    0.0,  1., 135.000, 118.000, 10.818,
    1.0,  1., 187.000,  85.000,  8.636,
    2.5, -1., 224.000,  37.000,  7.273,
    2.5, -1., 239.000,  46.000,  8.091,
    0.5, -1., 156.000, 131.000,  8.364,
    0.5,  1., 195.000,  74.000,  6.500,
    -1.
  };

  double bgr, x, y, ey, f;

  Chi2  = 0;

  for (int i=0; data[5*i] >= 0; i++) {
    bgr = data[5*i+4];

    y   = (data[5*i+3]-bgr)/(data[5*i+2]-bgr);

    ey = y*sqrt(data[5*i+3]/((data[5*i+3]-bgr)*(data[5*i+3]-bgr)) + 
		data[5*i+2]/((data[5*i+2]-bgr)*(data[5*i+2]-bgr))   );

    x = data[5*i]/10.+data[5*i+1]*Par[0];

    f = integral(x,Par[1]);

    Chi2 += (f-y)*(f-y)/(ey*ey);

    printf("i = %2i x = %8.2f Par[0] = %10.3f Par[1] = %10.3f f = %10.3f y = %10.3f chi2 = %10.3f\n",
	   i,x,Par[0],Par[1],f, y,Chi2);
  }

  // done with the chi2 calculation
}



//-----------------------------------------------------------------------------
int main() {
  double arglist[2];

  const char* pname[2] = {"x0" ,"sigma"};
  double      p0   [2] = {0.05 , 0.1 };
  double      step [2] = {0.001, 0.01};
  double      pmin [2] = {0.0  , 0.0 };
  double      pmax [2] = {0.1  , 0.5 };
  
  gF1  = new TF1("gF1",func,-10,10,1);
 
  fgMinuit   = TVirtualFitter::Fitter(0,2);
  fgMinuit->SetFCN(Fcn);

  arglist[0] = 1;
  fgMinuit->ExecuteCommand("SET ERR",arglist ,1);

  for (int i=0; i<2; i++) {
    fgMinuit->SetParameter(i,pname[i],p0[i],step[i],pmin[i],pmax[i]);
  }

  arglist[0] = 3;
  fgMinuit->ExecuteCommand("CALL FCN", arglist, 1);

  // arglist[0] = 0.1;
  // fgMinuit->ExecuteCommand("SET ERR", arglist ,1);

  // fgMinuit->ExecuteCommand("MINOS"   , arglist, 0);

  arglist[0] = 500;
  arglist[1] = 1.;
  fgMinuit->ExecuteCommand("MIGRAD"  , arglist, 0);

  // fgMinuit->ExecuteCommand("MINOS"   , arglist, 0);
  // // Print results

  arglist[0] = 3;
  fgMinuit->ExecuteCommand("CALL FCN", arglist, 1);

  return 0;
}


//-----------------------------------------------------------------------------
void test(double x0 = 0, double sigma = 0.1) {
  double chi2, p[2], g[2];
  int np, iflag;

  p[0] = x0;
  p[1] = sigma;
  
  Fcn(np, g, chi2, p, iflag);

}
