/********************************************************************\

  Name:         drs_exam.cpp
  Created by:   Stefan Ritt

  Contents:     Simple example application to read out a DRS4
                evaluation board

  $Id: drs_ext1-testbeam-mar2011.cpp,v 1.1.1.1 2012/03/25 23:06:55 camac Exp $

\********************************************************************/
#include <fstream>
#include <iomanip>
#include <string>
#include <iostream>

#include <TFile.h>
#include <TTree.h>
#include <TROOT.h>
#include <TRandom.h>
#include <TApplication.h>
#include <TCanvas.h>
#include <TPostScript.h>
#include <TAxis.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TGraph.h>
#include <TLegend.h>
#include <TF1.h>
#include <TStyle.h>
#include <TProfile.h>
#include <TMapFile.h>




#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>


#include <math.h>

#ifdef _MSC_VER

#include <windows.h>

#elif defined(OS_LINUX)

#define O_BINARY 0

#include <unistd.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <errno.h>

#define DIR_SEPARATOR '/'

#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "strlcpy.h"
#include "DRS.h"

/*------------------------------------------------------------------*/

// void do_CalibrateVolt(DRSBoard*);

int main( int argc, char** argv)
{

  char title[100];  sprintf( title, "%s_%s_%s.root", argv[1], argv[2], argv[3]);
  TFile* file = new TFile( title, "RECREATE", "MCP Test");
  
  TTree* tree = new TTree("pulse", "Wave Form");

  float b1_t[1024], b1_c1[1024], b1_c2[1024], b1_c3[1024], b1_c4[1024];
  
  int event, tc1;

  tree->Branch("event", &event, "event/I");
  tree->Branch("tc1", &tc1, "tc1/I");
  tree->Branch("b1_t",  b1_t, "b1_t[1024]/F");
  tree->Branch("b1_c1", b1_c1, "b1_c1[1024]/F");
  tree->Branch("b1_c2", b1_c2, "b1_c2[1024]/F");
  tree->Branch("b1_c3", b1_c3, "b1_c3[1024]/F");
  tree->Branch("b1_c4", b1_c4, "b1_c4[1024]/F");


  int nBoards;
  DRS *drs;
  DRSBoard *b;

  /* do initial scan */
  drs = new DRS();

  /* show any found board(s) */
  for( int i=0; i<drs->GetNumberOfBoards(); i++) {
    b = drs->GetBoard(i);
    printf("Found DRS4 evaluation board, serial #%d, firmware revision %d\n", 
	   b->GetBoardSerialNumber(), b->GetFirmwareVersion());
  }


  /* exit if no board found */
  nBoards = drs->GetNumberOfBoards();
  if (nBoards == 0) {
    printf("No DRS4 evaluation board found\n");
    return 0;
  }

  /* continue working with first board only */
  b = drs->GetBoard(0);


  if(1){
  printf("GetBoardSerialNumber() = %d\n",b->GetBoardSerialNumber());
  printf("GetFirmwareVersion() = %d\n",b->GetFirmwareVersion());
  printf("GetRequiredFirmwareVersion() = %d\n",b->GetRequiredFirmwareVersion());
  printf("GetDRSType() = %d\n",b->GetDRSType());
  printf("GetBoardType() = %d\n",b->GetBoardType());
  printf("GetFrequency() = %f\n",b->GetFrequency());
  }

  /* initialize board */
  b->Init();

  if(0){
    printf("Before VoltageCalibration\n");

    // do_CalibrateVolt( b);
    
    sleep(1);
    printf("After VoltageCalibration\n");
  }

  /* set sampling frequency */
  //   b->SetFrequency(1.6, true);
  b->SetFrequency(5, true);

  /* enable transparent mode needed for analog trigger */
  b->SetTranspMode(1);

  /* set input range to -0.5V ... +0.5V */
  b->SetInputRange(0);

  /* use following line to enable external hardware trigger (Lemo) */
  b->EnableTrigger(1, 0);
  b->SetDelayedTrigger(1);
  // b->SetTriggerDelay(40);    
  // b->SetTriggerDelay(-40);    
  b->SetTriggerDelay(-80);    
  
  /* repeat ten times */
  for (int i=0; i < atoi(argv[4]); i++) {

    if (i % 100 == 0)
      printf("i = %6d\n", i);

    b->StartClearCycle();
    b->FinishClearCycle();
    
    /* start board (activate domino wave) */
    b->StartDomino();
    
    /* wait for trigger */    
    while( b->IsBusy());
        
    /* read all waveforms */
    b->TransferWaves(0, 8);
    
    tc1 = drs->GetBoard(0)->GetTriggerCell(0);

     if( 0) printf("Trigger cell =  %4d, Stop cell = %4d\n", tc1,  b->GetStopCell(0));
    
    /* read time (X) array in ns */
    b->GetTime(0, b1_t);
    
    /* decode waveform (Y) array first channel in mV */
    b->GetWave(0, 0, b1_c1);
    b->GetWave(0, 2, b1_c2);
    b->GetWave(0, 4, b1_c3);
    b->GetWave(0, 6, b1_c4);
    
    event = i;
    
    tree->Fill();      
  }
  
  file->Write();
  file->Close();

  /* delete DRS object -> close USB connection */
  delete drs;

  return 0;
}



