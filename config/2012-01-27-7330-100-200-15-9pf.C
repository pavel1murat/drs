//-----------------------------------------------------------------------------
// configuration and fit results: 
// V = 73.3v
// x2 = 100mm  , x1 = 150mm , xsource = 200mm
// sigma(Q)/Q[0] = 4.4/70.6  FWHM     = 14.3%
// sigma(Q)/Q[2] = 5.4/74.2  FWHM     = 16.7% 
// sigma(dt2)    = 131.4ps   chi2/Ndf = 41.5/50
// sigma(dt6)    = 
//-----------------------------------------------------------------------------
int init() {
  
  drsana* drs = drsana::fgInstance; 


  drs->GetChannel(0)->SetBaseOffset(4.0);
  drs->GetChannel(2)->SetBaseOffset(6.4);

  drs->GetChannel(0)->SetMinThreshold(20);
  drs->GetChannel(2)->SetMinThreshold(20);

  drs->GetChannel(0)->SetMinQ( 60.);
  drs->GetChannel(2)->SetMinQ( 60.);

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
