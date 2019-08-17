//-----------------------------------------------------------------------------
// configuration and fit results: 
// V = 73.3v
// x2 = 8cm  , x1 = 28cm , xsource = 18cm
// sigma(Q)/Q[0] = 5.3/97         FWHM = 12.6%
// sigma(Q)/Q[2] = 5.8/101.2      FWHM = 13.2%
// sigma(dt2) = 113.6ps   (rebin 2) chi2 = 38.2/34
// sigma(dt6) = 114.9ps   (rebin 2) chi2 = 32.6/33
//-----------------------------------------------------------------------------
int init() {
  
  drsana* drs = drsana::fgInstance; 


  drs->GetChannel(0)->SetBaseOffset(4.0);
  drs->GetChannel(2)->SetBaseOffset(6.4);

  drs->GetChannel(0)->SetMinThreshold(20);
  drs->GetChannel(2)->SetMinThreshold(20);

  drs->GetChannel(0)->SetMinQ( 80.);
  drs->GetChannel(2)->SetMinQ( 80.);

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
