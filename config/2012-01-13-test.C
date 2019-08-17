///
int init() {
  
  drsana* drs = drsana::fgInstance; 

  drs->GetChannel(0)->SetBaseOffset(1.0);
  drs->GetChannel(2)->SetBaseOffset(1.8);

  drs->SetMinQ(0,60.);
  drs->SetMinQ(2,60.);

  TKneeFitAlg* knee = drs->GetAlgorithm("knee");
  knee->SetNFitBins(0,5);
  knee->SetNFitBins(2,5);

  return 0;
}

