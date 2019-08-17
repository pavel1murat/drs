// x2 = 8cm  , x1 = 28cm , xsource = 18cm
//
int init() {
  
  drsana* drs = drsana::fgInstance; 


  drs->GetChannel(0)->SetBaseOffset(3.9);
  drs->GetChannel(2)->SetBaseOffset(6.1);

  drs->GetChannel(0)->SetMinThreshold(20);
  drs->GetChannel(2)->SetMinThreshold(20);

  drs->GetChannel(0)->SetQPeak(143.9);  // sigma = 7.97
  drs->GetChannel(2)->SetQPeak(137.3);	// sigma = 9.15

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
