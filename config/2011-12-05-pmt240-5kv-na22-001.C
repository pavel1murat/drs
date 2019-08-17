///
int init() {
  
  drsana* drs = drsana::fgInstance; 

  drs->GetChannel(0)->SetBaseOffset(3.94);
  drs->GetChannel(2)->SetBaseOffset(6.04);

					// non-scattered photon energy cutoffs
					// work in units of charge !
  drs->GetChannel(0)->SetMinQ(130.);
  drs->GetChannel(2)->SetMinQ(150.);
					// the linear fit range

  TKneeFitAlg* knee = drs->GetAlgorithm("knee");
  knee->SetMinMaxFractions(0,0.05,0.15);
  knee->SetMinMaxFractions(2,0.05,0.15);
					// the gaussian fit range

  TExpoFitAlg* expo = drs->GetAlgorithm("expo");
  expo->SetMinMaxFractions(2,0.1,0.7);
  expo->SetMinMaxFractions(3,0.1,0.7);
  
  return 0;
}

