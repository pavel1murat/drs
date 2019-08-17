//------------------------------------------------------------------------------
//  rootlogon.C: a sample ROOT logon macro allowing use of ROOT script 
//               compiler in CDF RunII environment. The name of this macro file
//               is defined by the .rootrc file
//
//  USESHLIBS variable has to be set to build Stntuple libraries locally:  
//
//  setenv USESHLIBS 1
//
//  Feb 12 2001 P.Murat
//------------------------------------------------------------------------------
{
#include <iomanip.h>
#include <time.h>
                                // the line below tells CINT where to look for 
				// the include files

  gInterpreter->AddIncludePath(Form("%s/include",
				    gSystem->Getenv("SRT_LOCAL")));

  gInterpreter->AddIncludePath(Form("%s/include",
				    gSystem->Getenv("CDFSOFT2_DIR")));

  gInterpreter->AddIncludePath(Form("%s/tex/cdfnotes",
				    gSystem->Getenv("HOME")));
  gSystem->SetMakeSharedLib("cd $BuildDir ; g++ -c -g $Opt -pipe -m32 -Wall -W -Woverloaded-virtual -fPIC -pthread $IncludePath $SourceFiles ;  g++ -g $ObjectFiles -shared -Wl,-soname,$LibName.so -m32 $LinkedLibs -o $SharedLib");
                                // load in ROOT physics vectors and event
                                // generator libraries

//   gSystem->Load("$ROOTSYS/lib/libPhysics.so");
//   gSystem->Load("$ROOTSYS/lib/libEG.so");
  gSystem->Load("$ROOTSYS/lib/libMinuit.so");
  gSystem->Load("$ROOTSYS/lib/libFumili.so");
//   gSystem->Load("$ROOTSYS/lib/libNet.so");
  gSystem->Load("$ROOTSYS/lib/libTree.so");
  //  gSystem->Load("$ROOTSYS/lib/libRuby.so");

  //  gSystem->Load("$ORACLE_HOME/lib/libclntsh.so");
  //  gSystem->Load("$ROOTSYS/lib/libSQL.so");
  //  gSystem->Load("$ROOTSYS/lib/libOracle.so");
  //  gSystem->Load("$ROOTSYS/lib/libDCache.so");

				// load a script with the macros
  char command[200];

  sprintf(command,"%s/Stntuple/scripts/global_init.C",
	  gSystem->Getenv("CDFSOFT2_DIR"));

  gInterpreter->LoadMacro(command);

//-----------------------------------------------------------------------------
//  check batch mode
//-----------------------------------------------------------------------------
  int batch_mode = 0;
  const char* opt;
  int nargs = gApplication->Argc();
  for (int i=1; i<nargs; i++) {
    opt = gApplication->Argv(i);
    if (strcmp(opt,"-b") == 0) {
      batch_mode = 1;
      break;
    }
  }

  printf("   batch_mode = %i\n",batch_mode);
//-----------------------------------------------------------------------------
// STNTUPLE shared libraries are assumed to be built in the private test 
// release area with USESHLIBS environment variable set 
// we always need libStntuple_loop, but the other 2 libs should be loaded in 
// only if we're running bare root
//-----------------------------------------------------------------------------
  const char* exec_name = gApplication->Argv(0);

  if (strstr(exec_name,"stnfit.exe") != 0) {

    //    gSystem->Load("$WORK_DIR/shlib/cern/libminicern.so");

    gSystem->Load("$ROOTSYS/lib/libminicern.so");
    //    gSystem->Load("$WORK_DIR/shlib/cern/libpdf.so");

    //    gSystem->Load("$WORK_DIR/shlib/$BFARCH/libElectroweak_mrst2003.so");  
    //    gSystem->Load("$WORK_DIR/shlib/$BFARCH/libElectroweak_cteq6.so");  
    //    gSystem->Load("$WORK_DIR/shlib/$BFARCH/libElectroweak_TCteq6.so");

    //    gSystem->Load("$WORK_DIR/shlib/$BFARCH/libElectroweak_pythia_6418.so");  
    //    gSystem->Load("$WORK_DIR/shlib/$BFARCH/libElectroweak_TPythia6.so");

    gSystem->Load("$WORK_DIR/shlib/$BFARCH/libTRn.so");
    gSystem->Load("$WORK_DIR/shlib/$BFARCH/libTGeant_gen.so");
    gSystem->Load("$WORK_DIR/shlib/$BFARCH/libTGeant.so");
  }

  if (strstr(exec_name,"root.exe") != 0) {
//    gSystem->Load("$WORK_DIR/shlib/$BFARCH/libStntuple_base.so");
//    gSystem->Load("$WORK_DIR/shlib/$BFARCH/libStntuple_obj.so");
//    gSystem->Load("$WORK_DIR/shlib/$BFARCH/libStntuple_loop.so");
//    gSystem->Load("$WORK_DIR/shlib/$BFARCH/libStntuple_geom.so");
//    gSystem->Load("$WORK_DIR/shlib/$BFARCH/libStntuple_alg.so");
//    gSystem->Load("$WORK_DIR/shlib/$BFARCH/libStntuple_ana.so");
//    gSystem->Load("$WORK_DIR/shlib/$BFARCH/libStntuple_val.so");
  }
  else if (strstr(exec_name,"stnfit.exe") != 0) {
//-----------------------------------------------------------------------------
//  running stnfit.exe
//-----------------------------------------------------------------------------
    gSystem->Load("$WORK_DIR/shlib/$BFARCH/libStntuple_base.so");
    // gSystem->Load("$WORK_DIR/shlib/$BFARCH/libStntuple_geom.so");
    gSystem->Load("$WORK_DIR/shlib/$BFARCH/libStntuple_obj.so");
    gSystem->Load("$WORK_DIR/shlib/$BFARCH/libStntuple_loop.so");
    //  gSystem->Load("./shlib/$BFARCH/libStntuple_run1.so");
    // gSystem->Load("$WORK_DIR/shlib/$BFARCH/libStntuple_alg.so");
    gSystem->Load("$WORK_DIR/shlib/$BFARCH/libStntuple_val.so");
//
//    gSystem->Load("$WORK_DIR/shlib/$BFARCH/libmurat_obj.so");
//    gSystem->Load("$WORK_DIR/shlib/$BFARCH/libmurat_geom.so");
    if (batch_mode == 1) {
      gSystem->Load("$ROOTSYS/lib/libGui.so");
    }
//    gSystem->Load("$WORK_DIR/shlib/$BFARCH/libmurat_gui.so");
//    gSystem->Load("$WORK_DIR/shlib/$BFARCH/libmurat_alg.so");
//
//    gSystem->Load("$WORK_DIR/shlib/$BFARCH/libStntuple_fit.so");
//    gSystem->Load("$WORK_DIR/shlib/$BFARCH/libmurat_ana.so");
  }

//  gSystem->Load("$WORK_DIR/shlib/$BFARCH/libmurat_plot.so");
//
//  gSystem->Load("$WORK_DIR/shlib/$BFARCH/libbq.so");
//   gSystem->Load("$WORK_DIR/shlib/$BFARCH/libOfflineMon.so");
//   gSystem->Load("$WORK_DIR/shlib/$BFARCH/libCompModel.so");

  gSystem->Load("/usr/local/lib/libwx_gtk_adv-2.8.so");
  gSystem->Load("/usr/local/lib/libwx_gtk_qa-2.8.so");
  gSystem->Load("$WORK_DIR/shlib/$BFARCH/libdrs_drs4.so");
  gSystem->Load("$WORK_DIR/shlib/$BFARCH/libdrs_base.so");
  gSystem->Load("$WORK_DIR/shlib/$BFARCH/libdrs_obj.so");
  gSystem->Load("$WORK_DIR/shlib/$BFARCH/libdrs_geom.so");
  gSystem->Load("$WORK_DIR/shlib/$BFARCH/libdrs_gui.so");
  gSystem->Load("$WORK_DIR/shlib/$BFARCH/libdrs_alg.so");
  gSystem->Load("$WORK_DIR/shlib/$BFARCH/libdrs_ana.so");

                                // print overflows/underflows in the stat box
  gStyle->SetOptStat(11111111);
                                // print fit results in the stat box
  gStyle->SetOptFit(1110);
//-----------------------------------------------------------------------------
//  databases
//-----------------------------------------------------------------------------
//   gSystem->Load("libStntuple_oracle.so");
//   TCdfOracle cdfofprd("cdfofpr2");
//   TCdfOracle cdfrep01("cdfrep01");

  if (gSystem->Exec("ls $HOME/root_macros/set_style.C &> /dev/null") == 0) {
    gInterpreter->ExecuteMacro("$HOME/root_macros/set_style.C");
  }

//   sprintf(command,"%s/drs/ana/scripts/drs4.C",gSystem->Getenv("WORK_DIR"));
//   gInterpreter->LoadMacro(command);

// sprintf(command,"%s/cdfopr/CompModel/scripts/model.C",
// 	  gSystem->Getenv("WORK_DIR"));
//   gInterpreter->LoadMacro(command);
//  init_model();
//-----------------------------------------------------------------------------
// this line reports the process ID which simplifies debugging
//-----------------------------------------------------------------------------
  printf(" process ID: %i\n",gSystem->GetPid());
  TAuthenticate::SetGlobalUser(gSystem->Getenv("USER"));
  gInterpreter->ProcessLine(".! ps | grep root");
}
