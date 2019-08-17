// x2 = 8cm  , x1 = 28cm , xsource = 18cm
//
int init() {
  
  drsana* drs = drsana::fgInstance; 


  drs->GetChannel(0)->SetBaseOffset(1.5);
  drs->GetChannel(1)->SetBaseOffset(3.1);

  drs->GetChannel(0)->SetMinThreshold(20);
  drs->GetChannel(1)->SetMinThreshold(20);

  drs->GetChannel(0)->SetQPeak(118.0);  // sigma = 6.3
  drs->GetChannel(1)->SetQPeak(124.6);	// sigma = 7.5

  TKneeFitAlg* knee = drs->GetAlgorithm("knee");
  if (knee) {
    knee->SetNFitBins(0,4);
    knee->SetNFitBins(1,4);
  }

  TGausFitAlg* gaus = drs->GetAlgorithm("gaus");
  if (gaus) {

    gaus->SetLevel(0.07);

    // default (compiled): 0.02, 0.5
    gaus->SetMinMaxFractions(0,0.02,0.5);
    gaus->SetMinMaxFractions(1,0.02,0.5);


    gaus->SetChi2Max(0,21.);
    gaus->SetChi2Max(1,36.);
  }

  return 0;
}
