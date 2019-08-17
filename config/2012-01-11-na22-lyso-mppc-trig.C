///
int init() {
  
  drsana* drs = drsana::fgInstance; 

  drs->GetChannel(0)->SetBaseOffset(3.8);
  drs->GetChannel(2)->SetBaseOffset(5.7);

// min threshold over the pedestal, having exceeded which one could start looking
// for a maximum (protects against spikes)

  drs->GetChannel(0)->SetMinThreshold(8);
  drs->GetChannel(2)->SetMinThreshold(8);

  TKneeFitAlg* knee = drs->GetAlgorithm("knee");
  knee->SetNFitBins(0,7);
  knee->SetNFitBins(2,7);
//-----------------------------------------------------------------------------
// read offsets, they need to be subtracted - see FitCellOffsets
//-----------------------------------------------------------------------------
  drs->ReadCellOffsets(0,"drs/db/2012-01-11-na22-lyso-mppc-auto.ch_0.offsets");
  //    drs->ReadCellOffsets(2,"drs/db/2012-01-11-na22-lyso-mppc-auto.ch_2.offsets");
  return 0;
}

