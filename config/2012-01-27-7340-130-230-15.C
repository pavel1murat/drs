//-----------------------------------------------------------------------------
// configuration and fit results: 
// V = 73.4v
// x2 = 130mm  , x1 = 230mm , xsource = 180mm
//
// sigma(Q)/Q[0] = 5.6/96.5  FWHM    = 13.3% 
// sigma(Q)/Q[2] = 6.2/103.3 FWHM    = 13.8%
// <chi2 (shape)>[0] = 9.3
// <chi2 (shape)>[2] = 15.7
// sigma(dt2)        = 115.8ps chi2/Ndf = 44.9/47
// sigma(dt6)        = 
//-----------------------------------------------------------------------------
int init() {
  
  drsana* drs = drsana::fgInstance; 


  drs->GetChannel(0)->SetBaseOffset(4.0);
  drs->GetChannel(2)->SetBaseOffset(6.4);

  drs->GetChannel(0)->SetMinThreshold(20);
  drs->GetChannel(2)->SetMinThreshold(20);

  drs->GetChannel(0)->SetMinQ(80.);
  drs->GetChannel(2)->SetMinQ(80.);

  TKneeFitAlg* knee = drs->GetAlgorithm("knee");
  if (knee) {
    knee->SetNFitBins(0,4);
    knee->SetNFitBins(2,4);
  }

  TGausFitAlg* gaus = drs->GetAlgorithm("gaus");
  if (gaus) {
    // default: 0.07
    gaus->SetLevel(0.03);

    // default (compiled): 0.02, 0.5
    gaus->SetMinMaxFractions(0,0.02,0.5);
    gaus->SetMinMaxFractions(2,0.02,0.5);
  }

  return 0;
}
