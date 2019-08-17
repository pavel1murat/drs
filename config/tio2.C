// x2 = 8cm  , x1 = 28cm , xsource = 18cm
//
int init() {
  
  drsana* drs = drsana::fgInstance; 


  drs->GetChannel(0)->SetBaseOffset(3.75);
  drs->GetChannel(2)->SetBaseOffset(5.45);

  drs->GetChannel(0)->SetMinThreshold(20);
  drs->GetChannel(2)->SetMinThreshold(20);

  drs->GetChannel(0)->SetQPeak(99.26);  // sigma = 7.00
  drs->GetChannel(2)->SetQPeak(107.4);	// sigma = 6.20

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


    gaus->SetChi2Max(0,21.);
    gaus->SetChi2Max(1,36.);
  }

  return 0;
}
