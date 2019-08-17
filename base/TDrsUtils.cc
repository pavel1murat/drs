//
#include <math.h>

#include "drs/base/TDrsUtils.hh"
#include "Stntuple/obj/TStnEvent.hh"
#include "Stntuple/obj/TStnDataBlock.hh"

//-----------------------------------------------------------------------------
TDrsUtils::TDrsUtils() {
}

//-----------------------------------------------------------------------------
TDrsUtils::~TDrsUtils() {
}

//-----------------------------------------------------------------------------
// 'Font' - ROOT font number
//-----------------------------------------------------------------------------
int TDrsUtils::DrawPaveLabelNDC(TPaveLabel*& Label, 
				const char*  Text , 
				double       XMin , 
				double       YMin , 
				double       XMax , 
				double       YMax ,
				int          Font ) {
  Label = new TPaveLabel();

  Label->SetLabel(Text);

  Label->SetTextFont(Font);
  Label->SetTextSize(0.8);
  Label->SetBorderSize(0);
  Label->SetFillStyle(0);

  Label->SetX1NDC(XMin); 
  Label->SetX2NDC(XMax); 
  Label->SetY1NDC(YMin);
  Label->SetY2NDC(YMax);

  Label->Draw();
}



//-----------------------------------------------------------------------------
// with weights equal to one
//-----------------------------------------------------------------------------
int TDrsUtils::FitPol0(const float* V, int IMin, int IMax, double* Par, double* Chi2) {

  int rc(0);

  double   n, sx, sx2, xm, x2m, sig2;

  n   = 0;
  sx  = 0;
  sx2 = 0;
  
  for (int i=IMin; i<=IMax; i+=1) {
    sx  += V[i];
    sx2 += V[i]*V[i];
    n   += 1;
  }

  xm   = sx/n;
  x2m  = sx2/n;
  sig2 = x2m-xm*xm;

  Par [0] = xm;
  *Chi2   = sig2*n/(n-1+1.e-12);

  return rc;
}

//-----------------------------------------------------------------------------
// with weights equal to one
//-----------------------------------------------------------------------------
int TDrsUtils::FitPol1(const float* V, int IMin, int IMax, double* Par, double* Chi2) {

  int rc(0);

  double   n, x, y, sx, sy, sx2, sxy, sy2, xm, ym, x2m, xym, y2m, sigxx, sigxy, sigyy, chi2;

  n   = 0;
  sx  = 0;
  sy  = 0;
  sx2 = 0;
  sxy = 0;
  sy2 = 0;
  
  for (int i=IMin; i<=IMax; i+=1 ) {
    x   = i+ 0.5;
    y   = V[i];
    sx  += x;
    sy  += y;
    sx2 += x*x;
    sxy += x*y;
    sy2 += y*y;
    n   += 1;
  }

  xm  = sx/n;
  ym  = sy/n;
  x2m = sx2/n;
  xym = sxy/n;
  y2m = sy2/n;

  sigxx = x2m -xm*xm;
  sigxy = xym -xm*ym;
  sigyy = y2m -ym*ym;

  Par[1] = sigxy/sigxx;
  Par[0] = ym-Par[1]*xm;
  *Chi2  = (sigyy-sigxy*sigxy/sigxx)*n/(n-2+1.e-12);


  return rc;
}

//-----------------------------------------------------------------------------
// convert DRS format to ROOT
//-----------------------------------------------------------------------------
// int TDrsUtils::ReadGroupData(FILE* InputFile, VmeGroup_t* Group) {

//   int      rc(0);

//   //  unsigned int samples[8][1024];
//   unsigned int evb[3*1024];
//   unsigned int trb[3*1024/8];
//   int          n_samples, read_trigger, freq, start_cell;

//   unsigned int i, j, loc;

//   fread(&Group->fHeader, sizeof(unsigned int), 1, InputFile);

//   n_samples = (Group->fHeader & 0xfff) / 3;

//     // readoutTR = 1 if trigger is read out
//   read_trigger = (Group->fHeader >> 12) & 0x1;
//   freq         = (Group->fHeader >> 16) & 0x3;
//   start_cell   = (Group->fHeader >> 20) & 0x3ff;

//   printf("group header: 0x%08x : n_samples=%4i read_trigger=%i freq=%i start_cell=%5i\n",
// 	 Group->fHeader,n_samples,read_trigger,freq,start_cell);


//   fread(evb, sizeof(int), 3*n_samples, InputFile);
//   for(i = 0; i<n_samples; i++) {
//     loc = 3*i;
//     Group->fEvData[0][i] = (evb[loc  ]      ) & 0xfff;
//     Group->fEvData[1][i] = (evb[loc  ] >> 12) & 0xfff;
//     Group->fEvData[2][i] = (evb[loc  ] >> 24) | ((evb[loc+1]&0x0f) << 8);
//     Group->fEvData[3][i] = (evb[loc+1] >>  4) & 0xfff;
//     Group->fEvData[4][i] = (evb[loc+1] >> 16) & 0xfff;
//     Group->fEvData[5][i] = (evb[loc+1] >> 28) | ((evb[loc+2]&0xff) << 4);
//     Group->fEvData[6][i] = (evb[loc+2] >>  8) & 0xfff;
//     Group->fEvData[7][i] = (evb[loc+2] >> 20);
//   }

//   if (read_trigger != 0) {
// //-----------------------------------------------------------------------------
// // read trigger data in one go
// //-----------------------------------------------------------------------------
//     fread(trb, sizeof(unsigned int), 3*n_samples/8, InputFile);
//     for(i = 0; i < n_samples/8; i++) {
//       loc = 3*i;
//       Group->fTrData[i*8 + 0] = (trb[loc  ]      ) & 0xfff;
//       Group->fTrData[i*8 + 1] = (trb[loc  ] >> 12) & 0xfff;
//       Group->fTrData[i*8 + 2] = (trb[loc  ] >> 24) | ((trb[loc+1]&0x0f) << 8);
//       Group->fTrData[i*8 + 3] = (trb[loc+1] >>  4) & 0xfff;
//       Group->fTrData[i*8 + 4] = (trb[loc+1] >> 16) & 0xfff;
//       Group->fTrData[i*8 + 5] = (trb[loc+1] >> 28) | ((trb[loc+2]&0xff) << 4);
//       Group->fTrData[i*8 + 6] = (trb[loc+2] >>  8) & 0xfff;
//       Group->fTrData[i*8 + 7] = (trb[loc+2] >> 20);
//     }
//   }

//   // Move the file handle past the trigger time tag
//   fseek(InputFile, sizeof(unsigned int), SEEK_CUR);
//   return rc;
// }


// //-----------------------------------------------------------------------------
// int TDrsUtils::ConvertVmeToRoot(const char* InputFile, const char* OutputFile) {
//   int rc(0);

//   FILE         *input_file;
//   unsigned int eventHeader[4];
//   unsigned int group_header;
//   unsigned int n_samples;
//   unsigned int readoutTR;
//   unsigned int groupNum;
//   unsigned int groupMask;
//   int          ievent(0), nevents(0), present, nbytes;

//   VmeEvent_t   event;


//   // learn how to read the input file, start from Steve's code...

//   input_file = fopen(InputFile, "r");


//   if (fgEvent != 0) delete fgEvent;

//   fgFile = new TFile(Filename,"RECREATE");

//   if (! fgFile) {
//     Error("beginJob","an attempxt to open a new ROOT file %s failed",
// 	  fgFileName.Data());
//     rc = -1;
//   }

//   fgEvent       = new TStnEvent();
//   fgTree        = new TTree("STNTUPLE", "STNTUPLE");



//   AddDataBlock("HeaderBlock","TStnHeaderBlock",
// 	       StntupleInitHeaderBlock,
// 	       fBufferSize.value(),
// 	       99,                          // fSplitMode.value(), always split
// 	       fCompressionLevel.value());

//   SetResolveLinksMethod("HeaderBlock",StntupleHeaderBlockLinks);

//   AddDataBlock("TVmeDataBlock","TVmeDataBlock",
// 		 DrsInitVmeDataBlock,
// 		 fBufferSize.value(),
// 		 -1,			// don't split the data
// 		 fCompressionLevel.value());


//   while (fread(event.fHeader, sizeof(unsigned int), 4, input_file)) {
//     nbytes = (event.fHeader[0] & 0x0fffffff) * 4;
//     printf("------------- event number %5i nbytes = %5i\n",ievent,nbytes);
//     printf("0x%08x 0x%08x 0x%08x 0x%08x\n",
// 	   event.fHeader[0],
// 	   event.fHeader[1],
// 	   event.fHeader[2],
// 	   event.fHeader[3]);

//     groupMask = event.fHeader[1] & 0x0000000f;
// //-----------------------------------------------------------------------------
// // read groups
// //-----------------------------------------------------------------------------
//     for(int ig=0; ig<4; ig++) {
//       present = (groupMask >> ig) & 0x1;
//       if ( present == 0) goto READ_NEXT_GROUP;
//       ReadGroupData(input_file, &event.fGroup[ig]);
//     READ_NEXT_GROUP:;
//     }
// //-----------------------------------------------------------------------------
// // at this point event has been read, time to write it out
// //-----------------------------------------------------------------------------
//     ievent++;
//     nevents++;

//     fgEvent->Init(.....);
//     fgTree->Fill();
//   }

//   fclose(input_file);

//   fgFile->Write();
//   delete fgFile;
//   fgFile = 0;

//   return rc;
// }

