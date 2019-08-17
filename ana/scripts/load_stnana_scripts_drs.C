//

int load_stnana_scripts_drs() {
  char        macro[200];
  const char* script[] = { 
    "drs4.C",
    "pet_init_geometry.C",
    0 
  };

  const char* work_dir = gSystem->Getenv("WORK_DIR");

  TInterpreter* cint = gROOT->GetInterpreter();
  
  for (int i=0; script[i] != 0; i++) {
    sprintf(macro,"%s/drs/ana/scripts/%s",work_dir,script[i]);
    if (! cint->IsLoaded(macro)) {
      cint->LoadMacro(macro);
    }
  }
  
  return 0;
}
