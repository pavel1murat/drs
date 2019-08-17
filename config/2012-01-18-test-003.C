///
int init() {
  
  drsana* drs = drsana::fgInstance; 


  drs->GetChannel(0)->SetBaseOffset(3.3);
  drs->GetChannel(2)->SetBaseOffset(4.8);

  drs->SetMinThreshold(0,20);
  drs->SetMinThreshold(2,20);

  drs->GetChannel(0)->SetMinQ(200.);
  drs->GetChannel(2)->SetMinQ(200.);

  TKneeFitAlg* knee = drs->GetAlgorithm("knee");
  knee->SetNFitBins(0,8);
  knee->SetNFitBins(2,8);

  return 0;
}

