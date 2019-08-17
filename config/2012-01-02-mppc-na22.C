///
int init() {
  
  drsana* drs = drsana::fgInstance; 

  drs->GetChannel(0)->SetBaseOffset(3.9);
  drs->GetChannel(1)->SetBaseOffset(5.2);

  drs->GetChannel(0)->SetMinQ(850.);
  drs->GetChannel(1)->SetMinQ(850.);

  TKneeFitAlg* knee = drs->GetAlgorithm("knee");
  knee->SetMinMaxFractions(0,0.03,0.10); 
  knee->SetMinMaxFractions(1,0.03,0.10); 

  TKneeFitAlg* expo = drs->GetAlgorithm("expo");
  expo->SetMinMaxFractions(0,0.04,0.40); 
  expo->SetMinMaxFractions(1,0.04,0.40); 

  return 0;
}

