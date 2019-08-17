/********************************************************************\

  Name:         drs_exam.cpp
  Created by:   Stefan Ritt

  Contents:     Simple example application to read out a DRS4
                evaluation board

  $Id: drs_exam.cpp,v 1.1.1.1 2012/03/25 23:06:55 camac Exp $

\********************************************************************/

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

int main()
{
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
   f = fopen("data.txt", "w");
   if (f == NULL) {
      perror("ERROR: Cannot open file \"data.txt\"");
      return 1;
   }

   /* repeat ten times */
   for (j=0 ; j<10 ; j++) {

      /* start board (activate domino wave) */
      b->StartDomino();

      /* wait for trigger */
      printf("Waiting for trigger...");
      while (b->IsBusy());

      /* read all waveforms */
      b->TransferWaves(0, 8);

      /* read time (X) array in ns */
      b->GetTime(0, b->GetTriggerCell(0), time_array);

      /* decode waveform (Y) array first channel in mV */
      b->GetWave(0, 0, wave_array[0]);

      /* decode waveform (Y) array second channel in mV
         Note: On the evaluation board input #1 is connected to channel 0 and 1 of
         the DRS chip, input #2 is connected to channel 2 and 3 and so on. So to
         get the input #2 we have to read DRS channel #2, not #1 */
      b->GetWave(0, 2, wave_array[1]);

      /* Save waveform: X=time_array[i], Yn=wave_array[n][i] */
      fprintf(f, "Event #%d: t y1 y2\n", j);
      for (i=0 ; i<1024 ; i++)
         //fprintf(f, "%1.2f %1.2f %1.2f\n", time_array[i], wave_array[0][i], wave_array[1][i]);
         fprintf(f, "%5.2f %6.2f\n", time_array[i], wave_array[0][i]);

      /* print some progress indication */
      printf("\rEvent #%d read successfully\n", j);
   }

   fclose(f);
   
   /* delete DRS object -> close USB connection */
   delete drs;
}
