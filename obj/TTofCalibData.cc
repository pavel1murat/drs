///////////////////////////////////////////////////////////////////////////////
// calibration: Ch[i] = Offset[i] + Gain[i]*V
///////////////////////////////////////////////////////////////////////////////

#include "obj/TTofCalibData.hh"
#include "TGeant/TCalibManager.hh"
#include "TGeant/TCalibRunRange.hh"

ClassImp(TTofCalibData)

//_____________________________________________________________________________
TTofCalibData::TTofCalibData() {
//-----------------------------------------------------------------------------
// initialize the look up tables to indicate all the channels are good
//-----------------------------------------------------------------------------
  fNChannels = 32;
  fNx        = 4;
  fNy        = 4;
  Clear();
}

//_____________________________________________________________________________
TTofCalibData::~TTofCalibData() {
}



//-----------------------------------------------------------------------------
// cell-level amplitude calibrations
//-----------------------------------------------------------------------------
int TTofCalibData::InitAmplCalibrations(int RunNumber, TCalibManager* Manager) {
  int   rc(0);

  FILE  *f;
  int    done = 0;
  char   c[1000], calib_name[100];

  int     channel, bad_code, cell;
  float   offset , gain, chi2;

  TCalibRunRange* crr(0);


  for (int ich=0; ich<32; ich++) {
    
    sprintf(calib_name,"ampl_channel_%03i",ich);

    crr = Manager->GetRunRange("tof",calib_name,RunNumber);
    if (crr == 0) {
      Error("Init",Form("missing TOF AMPL table for run number %8i\n",RunNumber));
      return -1;
    }
					// check if the same run range
    if (fCalibRunRange == crr) return 0;
    fCalibRunRange = crr;
					// new run range, need to read
    f  = fopen(crr->GetFilename(),"r");
    if (f == 0) {
      Error("Init",Form("missing file %s\n",crr->GetFilename()));
      return -2;
    }

    while ( ((c[0]=getc(f)) != EOF) && !done) {

					// check if it is a comment line
      if (c[0] != '#') {
	ungetc(c[0],f);
					// read channel number
	fscanf(f,"%i" ,&channel );
	fscanf(f,"%i" ,&cell    );
	fscanf(f,"%f" ,&offset  );
	fscanf(f,"%f" ,&gain    );
	fscanf(f,"%f", &chi2    );
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
	fOffset[channel][cell] = offset;
	fGain  [channel][cell] = gain  ;
      }
					// skip line
      fgets(c,100,f);
    }
  }

  fclose(f);

  return rc;
}


//-----------------------------------------------------------------------------
int TTofCalibData::InitPedestals(int RunNumber, TCalibManager* Manager) {
  int   rc(0);

  FILE  *f;
  int    done = 0;
  char   c[1000], calib_name[100];

  int     channel, bad_code, cell;
  float   offset , gain, chi2, pedestal, sigma;

  TCalibRunRange* crr(0);

  sprintf(calib_name,"pedestals");

  crr = Manager->GetRunRange("tof",calib_name,RunNumber);
  if (crr == 0) {
    Error("Init",Form("missing TOF %s table for run number %8i\n",
		      calib_name,RunNumber));
    return -1;
  }
					// check if the same run range
  if (fCalibRunRange == crr) return 0;
  fCalibRunRange = crr;
					// new run range, need to read
  f  = fopen(crr->GetFilename(),"r");
  if (f == 0) {
    Error("Init",Form("missing file %s\n",crr->GetFilename()));
    return -2;
  }

  while ( ((c[0]=getc(f)) != EOF) && !done) {

					// check if it is a comment line
    if (c[0] != '#') {
      ungetc(c[0],f);
					// read channel number
      fscanf(f,"%i" ,&channel );
      fscanf(f,"%f" ,&pedestal);
      fscanf(f,"%f" ,&sigma   );
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
      fPedestal[channel] = pedestal;
      fPedSigma[channel] = sigma;
    }
					// skip line
    fgets(c,100,f);
  }

  fclose(f);

  return rc;
}

//-----------------------------------------------------------------------------
int TTofCalibData::InitChannelGains(int RunNumber, TCalibManager* Manager) {
  int   rc(0);

  FILE  *f;
  int    done = 0;
  char   c[1000], calib_name[100];

  int     channel, bad_code, cell;
  float   gain0, chi2, mean, sigm, pulse_int_window;

  TCalibRunRange* crr(0);

  sprintf(calib_name,"channel_gain");

  crr = Manager->GetRunRange("tof",calib_name,RunNumber);
  if (crr == 0) {
    Error("Init",Form("missing TOF %s table for run number %8i\n",
		      calib_name,RunNumber));
    return -1;
  }
					// check if the same run range
  if (fCalibRunRange == crr) return 0;
  fCalibRunRange = crr;
					// new run range, need to read
  f  = fopen(crr->GetFilename(),"r");
  if (f == 0) {
    Error("Init",Form("missing file %s\n",crr->GetFilename()));
    return -2;
  }

  while ( ((c[0]=getc(f)) != EOF) && !done) {

					// check if it is a comment line
    if (c[0] != '#') {
      ungetc(c[0],f);
					// read channel number
      fscanf(f,"%i" ,&channel );
      fscanf(f,"%f" ,&mean);
      fscanf(f,"%f" ,&sigm);
      fscanf(f,"%f" ,&chi2);
      fscanf(f,"%f" ,&pulse_int_window);
//-----------------------------------------------------------------------------
// normalize the photopeak to 511 MeV
//-----------------------------------------------------------------------------
      fChannelGain[channel] = 511./mean;
      fPulseWindow[channel] = pulse_int_window;
    }
					// skip line
    fgets(c,100,f);
  }

  fclose(f);

  return rc;
}


//-----------------------------------------------------------------------------
int TTofCalibData::InitReadoutMap(int RunNumber, TCalibManager* Manager) {
  int   rc(0);

  FILE  *f;
  int    done = 0;
  char   c[1000], calib_name[100];

  int     channel, used, module, ix, iy, loc;

  TCalibRunRange* crr(0);

  sprintf(calib_name,"readout_map");

  crr = Manager->GetRunRange("tof",calib_name,RunNumber);
  if (crr == 0) {
    Error("Init",Form("missing TOF %s table for run number %8i\n",
		      calib_name,RunNumber));
    return -1;
  }
					// check if the same run range
  if (fCalibRunRange == crr) return 0;
  fCalibRunRange = crr;
					// new run range, need to read
  f  = fopen(crr->GetFilename(),"r");
  if (f == 0) {
    Error("Init",Form("missing file %s\n",crr->GetFilename()));
    return -2;
  }

  while ( ((c[0]=getc(f)) != EOF) && !done) {

					// check if it is a comment line
    if (c[0] != '#') {
      ungetc(c[0],f);
					// read channel number
      fscanf(f,"%i" ,&channel);
      fscanf(f,"%i" ,&used   );
      fscanf(f,"%i" ,&module );
      fscanf(f,"%i" ,&ix     );
      fscanf(f,"%i" ,&iy     );
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
//      loc = module*16+ix*4+iy;
      fChannel[module][ix][iy] = channel;
      fModule [channel]        = module;
      fIX     [channel]        = ix;
      fIY     [channel]        = iy;
      fUsed   [channel]        = used;
    }
					// skip line
    fgets(c,100,f);
  }

  fclose(f);

  return rc;
}

//-----------------------------------------------------------------------------
int TTofCalibData::Init(int RunNumber, TCalibManager* Manager) {
//-----------------------------------------------------------------------------
// read calibrations for a given run
//-----------------------------------------------------------------------------
  int rc(0);

  rc = InitAmplCalibrations(RunNumber,Manager);
  rc = InitPedestals       (RunNumber,Manager);
  rc = InitReadoutMap      (RunNumber,Manager);
  rc = InitChannelGains    (RunNumber,Manager);

  return 0;
}

//_____________________________________________________________________________
void TTofCalibData::Clear(Option_t* Opt) {
  memset(fOffset  ,0,32*1024*sizeof(float));
  memset(fGain    ,0,32*1024*sizeof(float));
  memset(fPedestal,0,32*sizeof(float));
  fCalibRunRange = 0;
}

//_____________________________________________________________________________
void TTofCalibData::Print(Option_t* opt) const {
  Error("Print","not implemented yet");
}
  

