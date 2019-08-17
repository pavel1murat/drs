/********************************************************************\

  Name:         drs_exam.cpp
  Created by:   Stefan Ritt

  Contents:     Simple example application to read out a DRS4
                evaluation board

  $Id: drs_pet.cpp,v 1.1.1.1 2012/03/25 23:06:55 camac Exp $

  2011-10-19 P.Murat : created starting from drs_exam.cpp

\********************************************************************/

#include <math.h>

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

#include <sys/mman.h>
#include <time.h>

#define O_BINARY 0

#include <unistd.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <errno.h>

#define DIR_SEPARATOR '/'

#include "strlcpy.h"
#include "DRS.h"

//-----------------------------------------------------------------------------
// call: drs_pet filename nevents 
//-----------------------------------------------------------------------------
int main( int argc, char** argv)
{

  char title[100];  sprintf( title, "%s.root", argv[1]);
  TFile* file = new TFile( title, "RECREATE", "MCP Test");

  TTree* tree = new TTree("pulse", "Wave Form");

  float b1_t[1024], b1_c1[1024], b1_c2[1024], b1_c3[1024], b1_c4[1024];
  float b2_t[1024], b2_c1[1024], b2_c2[1024], b2_c3[1024], b2_c4[1024];
  
  int event, tc1, tc2;


  time_t start, end;
  double tdiff;


  tree->Branch("event", &event, "event/I");
  tree->Branch("tc1", &tc1, "tc1/I");
  tree->Branch("b1_t",  b1_t, "b1_t[1024]/F");
  tree->Branch("b1_c1", b1_c1, "b1_c1[1024]/F");
  tree->Branch("b1_c2", b1_c2, "b1_c2[1024]/F");
  tree->Branch("b1_c3", b1_c3, "b1_c3[1024]/F");
  tree->Branch("b1_c4", b1_c4, "b1_c4[1024]/F");

  tree->Branch("tc2", &tc2, "tc2/I");
  tree->Branch("b2_t",  b2_t,  "b2_t[1024]/F");
  tree->Branch("b2_c1", b2_c1, "b2_c1[1024]/F");
  tree->Branch("b2_c2", b2_c2, "b2_c2[1024]/F");
  tree->Branch("b2_c3", b2_c3, "b2_c3[1024]/F");
  tree->Branch("b2_c4", b2_c4, "b2_c4[1024]/F");




   int i, j, nBoards;
   DRS *drs;
   DRSBoard *b;
   float time_array[1024];
   float wave_array[8][1024];
   FILE  *f;

   /* do initial scan */
   drs = new DRS();

   /* show any found board(s) */
   for (i=0 ; i<drs->GetNumberOfBoards() ; i++) {
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

   /* initialize board */
   b->Init();

   /* set sampling frequency */
   b->SetFrequency(5, true);

   /* enable transparent mode needed for analog trigger */
   b->SetTranspMode(1);

   /* set input range to -0.5V ... +0.5V */
   b->SetInputRange(0);

   /* use following line to set range to 0..1V */
   //b->SetInputRange(0.5);

   /* use following line to disable hardware trigger */
   //b->EnableTrigger(0, 0);

   /* use following line to enable external hardware trigger (Lemo) */
   //b->EnableTrigger(1, 0);

   /* use following lines to enable hardware trigger on CH1 at 250 mV positive edge */
   b->EnableTrigger(0, 1);              // lemo off, analog trigger on
   b->SetTriggerSource(0);              // use CH1 as source
   b->SetTriggerLevel(0.25, false);     // 0.25 V, positive edge
   b->SetTriggerDelayNs(0);             // zero ns trigger delay

   /* open file to save waveforms */
   if( 0) printf("Hello 8\n");


   time( &start);




   /* repeat ten times */
   for (j =0 ; j < atoi( argv[2]) ; j++) {

     if( 0) printf("Hello 9\n");
     /* clear chip (necessary for DRS4 to reduce noise) */
     //     b->ClearCycle();
     //     b2->ClearCycle();
     
     if( 0) printf("Hello 10\n");
     /* start board (activate domino wave) */
     b->StartDomino();
     //     b2->StartDomino();
     
     /* wait for trigger */
     if( 0) printf("Waiting for trigger...\n");
     while (b->IsBusy());
     
     
     /* read all waveforms */
     b->TransferWaves(0, 8);
     //     b2->TransferWaves(0, 8);
     
     tc1 = drs->GetBoard(0)->GetTriggerCell(0);
     //     tc2 = drs->GetBoard(1)->GetTriggerCell(0);
     
     if( 0) printf("trigger cell =  %d,   %d\n", tc1, tc2);

     /* read time (X) array in ns */ 

     // 2011-10-19 P.Murat: ::GetTime signature has changed, need to figure out meaning of new parameters
     b->GetTime(0, b->GetTriggerCell(0), b1_t);
     //     b2->GetTime(0, b2_t);
     
     /* decode waveform (Y) array first channel in mV */
     b->GetWave(0, 0, b1_c1);
     b->GetWave(0, 2, b1_c2);
     b->GetWave(0, 4, b1_c3);
     b->GetWave(0, 6, b1_c4);

     //     b2->GetWave(0, 0, b2_c1);
     //     b2->GetWave(0, 2, b2_c2);
     //     b2->GetWave(0, 4, b2_c3);
     //     b2->GetWave(0, 6, b2_c4 );
     
     /* decode waveform (Y) array second channel in mV
	Note: On the evaluation board input #1 is connected to channel 0 and 1 of
	the DRS chip, input #2 is connected to channel 2 and 3 and so on. So to
	get the input #2 we have to read DRS channel #2, not #1 */
     //      b->GetWave(0, 6, wave_array[1]);
     // b2->GetWave(0, 6, wave_array2[1]);
     
     /* Save waveform: X=time_array[i], Yn=wave_array[n][i] */

     if( j%100 == 99){
       time( &end);
       tdiff = difftime( end, start);
       printf("Event#    %5d.  DAQ time = %6.2f(Second), Rate = %6.2f(Hz)\n", 
	      j+1, tdiff, (float)(j+1)/tdiff);
     }

     event = j;

     tree->Fill();

     /* print some progress indication */
     if( 0) printf("\rEvent #%d read successfully\n", j);
   }


   file->Write();
   file->Close();


  time( &end);

  tdiff = difftime( end, start);

  //  printf("DAQ time = %6.2f(Second),  Rate = %6.2f(Hz)\n", tdiff, atof(argv[4])/tdiff);


   
   /* delete DRS object -> close USB connection */
   delete drs;
}
