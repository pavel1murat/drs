//-----------------------------------------------------------------------------
// configuration and fit results: 
// V = 73.5v
// x2 = 130mm  , x1 = 230mm , xsource = 180mm
//
// sigma(Q)/Q[0] = 6.4/117.5 FWHM    = 12.5% 
// sigma(Q)/Q[2] = 6.8/125.0 FWHM    = 12.5%
// <chi2 (shape)>[0] = 13.1
// <chi2 (shape)>[2] = 22.4 
// sigma(dt2)        = 115.3ps chi2/Ndf = 45.9/43
// sigma(dt6)        = 
//-----------------------------------------------------------------------------
int init() {
  
  drsana* drs = drsana::fgInstance; 


  drs->GetChannel(0)->SetBaseOffset(4.0);
  drs->GetChannel(2)->SetBaseOffset(6.4);

  drs->GetChannel(0)->SetMinThreshold(20);
  drs->GetChannel(2)->SetMinThreshold(20);

  drs->GetChannel(0)->SetMinQ(100.);
  drs->GetChannel(2)->SetMinQ(100.);

  TKneeFitAlg* knee = drs->GetAlgorithm("knee");
  if (knee) {
    knee->SetNFitBins(0,4);
    knee->SetNFitBins(2,4);
  }

  TGausFitAlg* gaus = drs->GetAlgorithm("gaus");
  if (gaus) {

    gaus->SetLevel(0.07);

    // default (compiled): 0.02, 0.5
    gaus->SetMinMaxFractions(0,0.02,0.5);
    gaus->SetMinMaxFractions(2,0.02,0.5);
  }

  return 0;
}
