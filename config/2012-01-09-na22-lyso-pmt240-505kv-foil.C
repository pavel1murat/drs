///
int init() {
  
  drsana* drs = drsana::fgInstance; 

  drs->GetChannel(0)->SetBaseOffset(-3.7);
  drs->GetChannel(2)->SetBaseOffset(-5.4);

  return 0;
}

