///
// ch#0 : FWHM=10.3%, ch#2:10.6%
// sigma(T)
int init() {
  
  drsana* drs = drsana::fgInstance; 


  drs->GetChannel(0)->SetBaseOffset(3.5);
  drs->GetChannel(2)->SetBaseOffset(4.7);

  drs->GetChannel(0)->SetMinThreshold(20);
  drs->GetChannel(2)->SetMinThreshold(20);

  drs->GetChannel(0)->SetMinQ(240.);
  drs->GetChannel(2)->SetMinQ(220.);

  TKneeFitAlg* knee = drs->GetAlgorithm("knee");
  if (knee) {
    knee->SetNFitBins(0,4);
    knee->SetNFitBins(2,4);
  }

  return 0;
}

