///
int init() {
  
  drsana* drs = drsana::fgInstance; 


  drs->GetChannel(0)->SetBaseOffset(3.7);
  drs->GetChannel(2)->SetBaseOffset(5.7);

  drs->GetChannel(0)->SetMinThreshold(20);
  drs->GetChannel(2)->SetMinThreshold(20);

  drs->GetChannel(0)->SetMinQ(350.);
  drs->GetChannel(2)->SetMinQ(400.);

  TKneeFitAlg* knee = drs->GetAlgorithm("knee");
  if (knee) {
    knee->SetNFitBins(0,4);
    knee->SetNFitBins(2,4);
  }

  TGausFitAlg* gaus = drs->GetAlgorithm("gaus");
  if (gaus) {
    gaus->SetLevel(0.10);
    gaus->SetMinMaxFractions(0,0.01,0.1);
    gaus->SetMinMaxFractions(2,0.01,0.1);
  }
  return 0;
}

