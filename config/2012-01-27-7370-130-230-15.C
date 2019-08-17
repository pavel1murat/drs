//-----------------------------------------------------------------------------
// configuration and fit results: 
// V = 73.7v
// x2 = 130mm  , x1 = 230mm , xsource = 180mm
//
// sigma(Q)/Q[0] = 7.3/134.2 FWHM    = 12.5%
// sigma(Q)/Q[2] = 8.0/142.7 FWHM    = 12.9% 
// <chi2 (shape)>[0] = 17.0 
// <chi2 (shape)>[2] = 28.7 
// sigma(dt2)        = 115.2ps   chi2/Ndf = 47.5/50
// sigma(dt6)        = 
//-----------------------------------------------------------------------------
int init() {
  
  drsana* drs = drsana::fgInstance; 

  drs->GetChannel(0)->SetBaseOffset(3.9);
  drs->GetChannel(2)->SetBaseOffset(6.4);

  drs->GetChannel(0)->SetMinThreshold(20);
  drs->GetChannel(2)->SetMinThreshold(20);

  drs->GetChannel(0)->SetMinQ(110.);
  drs->GetChannel(2)->SetMinQ(120.);

  TGausFitAlg* gaus = drs->GetAlgorithm("gaus");
  if (gaus) {

    gaus->SetLevel(0.07);

    // default (compiled): 0.02, 0.5
    gaus->SetMinMaxFractions(0,0.02,0.5);
    gaus->SetMinMaxFractions(2,0.02,0.5);
  }

  return 0;
}
