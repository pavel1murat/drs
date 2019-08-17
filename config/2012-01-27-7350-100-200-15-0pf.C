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
  TExpoFitAlg* expo;
  
  drsana* drs = drsana::fgInstance; 
//-----------------------------------------------------------------------------
// channels: thresholds and charges
//-----------------------------------------------------------------------------
  drs->GetChannel(0)->SetBaseOffset(3.7);
  drs->GetChannel(2)->SetBaseOffset(5.8);

  drs->GetChannel(0)->SetMinThreshold(30);
  drs->GetChannel(2)->SetMinThreshold(30);

  drs->GetChannel(0)->SetMinQ(650.);
  drs->GetChannel(2)->SetMinQ(700.);
//-----------------------------------------------------------------------------
// algorithms
//-----------------------------------------------------------------------------
  expo = drs->GetAlgorithm("expo");

  if (expo == 0) {
    expo = new TExpoFitAlg("expo","expo fit",drs->GetListOfChannels());
    drs->AddAlgorithm(expo);
  }

  // default (compiled): 0.02, 0.5

  expo->SetMinMaxFractions(0,0.05,0.50);
  expo->SetMinMaxFractions(2,0.05,0.50);

  // TGausFitAlg* gaus = drs->GetAlgorithm("gaus");
  // if (gaus) {

  //   gaus->SetLevel(0.07);

  //   // default (compiled): 0.02, 0.5
  //   gaus->SetMinMaxFractions(0,0.02,0.5);
  //   gaus->SetMinMaxFractions(2,0.02,0.5);
  // }

  return 0;
}
