// x2 = 8cm  , x1 = 28cm , xsource = 18cm
//
int init() {
  
  drsana* drs = drsana::fgInstance; 


  drs->GetChannel(0)->SetBaseOffset(2.4);
  drs->GetChannel(1)->SetBaseOffset(2.7);

  drs->GetChannel(0)->SetMinThreshold(20);
  drs->GetChannel(1)->SetMinThreshold(20);

  drs->GetChannel(0)->SetQPeak(147.0);  // sigma = 7.7
  drs->GetChannel(1)->SetQPeak(154.9);	// sigma = 8.15

  TKneeFitAlg* knee = drs->GetAlgorithm("knee");
  if (knee) {
    knee->SetNFitBins(0,4);
    knee->SetNFitBins(2,4);
  }

  TGausFitAlg* gaus = drs->GetAlgorithm("gaus");
  if (gaus) {
    //  0.07: dt2_rebin_5:  99 +/- 5, dt5_rebin_5=98: +/- 5,  dt6_rebin_5: 90 +/- 7 ps, dt7_rebin_5: 90 +/- 7
    //  0.08: dt2_rebin_5:  98 +/- 6, dt5_rebin_5=95: +/- 6,  dt6_rebin_5: 87 +/- 6 ps, dt7_rebin_5: 86 +/- 7
    //  0.10: dt2_rebin_5: 101 +/- 6, dt5_rebin_5=96: +/- 6,  dt6_rebin_5: 86 +/- 5 ps, dt7_rebin_5: 88 +/- 7
    //  0.15: dt2_rebin_5: 95       , dt5_rebin_5=94:      ,  dt6_rebin_5: 90 +/- 5 ps, dt7_rebin_5: 92

    gaus->SetLevel(0.07);

    // default (compiled): 0.02, 0.5
    gaus->SetMinMaxFractions(0,0.02,0.5);
    gaus->SetMinMaxFractions(2,0.02,0.5);
  }

  return 0;
}
