//-----------------------------------------------------------------------------
// calibrations follow H/W convention: strips=0 wire=1
//-----------------------------------------------------------------------------
#ifndef TTofCalibData_hh
#define TTofCalibData_hh

#include "TGeant/TCalibData.hh"

class TTofModuleCalibData;
class TCalibManager;
class TCalibRunRange;

//-----------------------------------------------------------------------------
// module calib data
//-----------------------------------------------------------------------------
class TTofCalibData : public TCalibData {
public:
  int      fNChannels;
  Int_t    fNx;
  Int_t    fNy;
					// DRS4 cell-level amplitude calibration
  float    fGain    [32][1024];
  float    fOffset  [32][1024];
					// common pedestal, not used ?
  float    fPedestal[32];
  float    fPedSigma[32];

  int      fChannel[2][4][4];
  int      fModule [32];
  int      fIX     [32];
  int      fIY     [32];
  int      fUsed   [32];

  float    fChannelGain[32];            // photopeak gain, divide by [0]
  float    fPulseWindow[32];            // pulse width , for charge integration

  int      fRunNumber;

  TCalibRunRange* fCalibRunRange;       //! non-persistent
//-----------------------------------------------------------------------------
// constructors and destructor
//-----------------------------------------------------------------------------
public:
  TTofCalibData();
  virtual ~TTofCalibData();
//-----------------------------------------------------------------------------
// accessors
//-----------------------------------------------------------------------------
  int Init                (int RunNumber, TCalibManager* Manager);
  int InitAmplCalibrations(int RunNumber, TCalibManager* Manager);
  int InitPedestals       (int RunNumber, TCalibManager* Manager);
  int InitReadoutMap      (int RunNumber, TCalibManager* Manager);
  int InitChannelGains    (int RunNumber, TCalibManager* Manager);

  float  GetGain  (int Channel, int Cell) { return fGain  [Channel][Cell] ; }
  float  GetOffset(int Channel, int Cell) { return fOffset[Channel][Cell] ; }

  float  GetPedestal   (int Channel) { return fPedestal   [Channel]; }
  float  GetPedSigma   (int Channel) { return fPedSigma   [Channel]; }
  float  GetChannelGain(int Channel) { return fChannelGain[Channel]; }
  float  GetPulseWindow(int Channel) { return fPulseWindow[Channel]; }

//   int    GetChannelID(int Channel) { 
//     return (fModule[Channel]<<16) + (fIX[Channel]<<8) + fIY[Channel];
//   }

  int    GetChannel(int Module, int  IX, int IY) {
    return fChannel[Module][IX][IY];
  };

  int    GetModule(int Channel) { return fModule[Channel]; }
  int    GetIX    (int Channel) { return fIX    [Channel]; }
  int    GetIY    (int Channel) { return fIY    [Channel]; }
  int    GetUsed  (int Channel) { return fUsed  [Channel]; }
//-----------------------------------------------------------------------------
// overloaded methods of TCalibData
//-----------------------------------------------------------------------------
  virtual int Init(TObject* Data) { return 0; }
//-----------------------------------------------------------------------------
// overloaded methods of TObject
//-----------------------------------------------------------------------------
  void Clear(Option_t* Option = "");
  void Print(Option_t* Option = "") const ;

  ClassDef (TTofCalibData,1)
};

#endif

