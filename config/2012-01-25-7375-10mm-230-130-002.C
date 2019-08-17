// x2 = 130mm  , x1 = 230mm , xsource = 180mm  V=73.75v I = 82A  T(DRS4) = 49.2-49.7
//
int init() {
  
  drsana* drs = drsana::fgInstance; 


  drs->GetChannel(0)->SetBaseOffset(3.2);
  drs->GetChannel(2)->SetBaseOffset(4.1);

  drs->GetChannel(0)->SetMinThreshold(20);
  drs->GetChannel(2)->SetMinThreshold(20);

  drs->GetChannel(0)->SetMinQ(150.);
  drs->GetChannel(2)->SetMinQ(130.);

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
