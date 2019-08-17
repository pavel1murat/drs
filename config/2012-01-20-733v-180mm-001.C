///
int init() {
  
  drsana* drs = drsana::fgInstance; 


  drs->GetChannel(0)->SetBaseOffset(3.3);
  drs->GetChannel(2)->SetBaseOffset(4.0);

  drs->GetChannel(0)->SetMinThreshold(20);
  drs->GetChannel(2)->SetMinThreshold(20);

  drs->GetChannel(0)->SetMinQ(200.);
  drs->GetChannel(2)->SetMinQ(200.);

  TKneeFitAlg* knee = drs->GetAlgorithm("knee");
  if (knee) {
    knee->SetNFitBins(0,4);
    knee->SetNFitBins(2,4);
  }

  TGausFitAlg* gaus = drs->GetAlgorithm("gaus");
  if (gaus) {
    gaus->SetLevel(0.10);
    gaus->SetMinMaxFractions(0,0.01,0.50);
    gaus->SetMinMaxFractions(2,0.01,0.50);
  }
  return 0;
}

