///
int init() {
  drsana* drs = drsana::fgInstance; 

  drs->GetChannel(2)->SetBaseOffset(-10.);
  drs->GetChannel(3)->SetBaseOffset( 20.);

					// pulse height fraction used in the 
					// leading edge fit

  TKneeFitAlg* knee = drs->GetAlgorithm("knee");
  knee->SetMinMaxFractions(2,0.10,0.60);
  knee->SetMinMaxFractions(3,0.10,0.60);

  TExpoFitAlg* expo = drs->GetAlgorithm("expo");
  expo->SetMinMaxFractions(2,0.02,0.45);
  expo->SetMinMaxFractions(3,0.02,0.45);

  TExpoFitAlg* gaus = drs->GetAlgorithm("gaus");
  gaus->SetMinMaxFractions(2,0.02,0.45);
  gaus->SetMinMaxFractions(3,0.02,0.45);
					// even-odd shift , old DRS4 ?
  drs->GetChannel(2)->SetEvenShift(2.41);
  drs->GetChannel(3)->SetEvenShift(2.34);

  printf(" base_offset[2] = %10.3g\n",drs->GetChannel(2)->GetBaseOffset());
  printf(" even_shift [2] = %10.3g\n",drs->GetChannel(2)->fEvenShift);
//-----------------------------------------------------------------------------
// read offsets, they need to be subtracted - see FitCellOffsets
//-----------------------------------------------------------------------------
  drs->ReadCellOffsets(2,"drs/db/hm_ch2_offsets.txt");
  drs->ReadCellOffsets(3,"drs/db/hm_ch3_offsets.txt");

  return 0;
}
