#ifndef TDrsUtils_hh
#define TDrsUtils_hh

#include "TPaveLabel.h"


class TDrsUtils {

public:

  struct VmeGroup_t {
    unsigned int    fHeader;
    unsigned short  fEvData[8][1024];
    unsigned short  fTrData[1024];    // not necessarily present
  };

  struct VmeEvent_t {
    unsigned int fHeader[4];
    VmeGroup_t   fGroup[4];
  };

  TObjArray*  fCachedListOfChannels;		// ! cached, not owned

  TDrsUtils ();
  ~TDrsUtils();

  static int     DrawPaveLabelNDC(TPaveLabel*& Label, 
				  const char*  Text , 
				  double       XMin , 
				  double       YMin , 
				  double       XMax , 
				  double       YMax ,
				  int          Font = 52);

  static int FitPol0(const float* V, int IMin, int IMax, double* Par, double* Chi2);
  static int FitPol1(const float* V, int IMin, int IMax, double* Par, double* Chi2);

  ClassDef (TDrsUtils,0)
};



#endif
