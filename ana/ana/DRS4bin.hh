#ifndef drs4bin_hh
#define drs4bin_hh

#include "OscReader.hh"

class DRS4bin : public TNamed {
//-----------------------------------------------------------------------------
// data members
//-----------------------------------------------------------------------------
public:
  enum filetype {bin, root};

  bool    status;

  Float_t b1_t [1024];
  Float_t b1_c1[1024];
  Float_t b1_c2[1024];
  Float_t b1_c3[1024];
  Float_t b1_c4[1024];

  int     fNChannels;			// number of channels (sparsified)
  Int_t   usedchan[4];
  Int_t   event;
  Int_t   tc1;

  // Int_t   year;
  // Int_t   month;
  // Int_t   day;
  // Int_t   hour;
  // Int_t   minute;
  // Int_t   second;
  // Int_t   millisecond;

  std::string   ifname;
  std::ifstream ifile;

  Long64_t      ifsize;

  TFile*        ofile;
  TTree*        tree;

  OscReader*    oscReader;    // create an instance of the OscReader
//-----------------------------------------------------------------------------
// methods
//-----------------------------------------------------------------------------
  DRS4bin(const char* Filename = 0);
  ~DRS4bin();

  bool operator !() {return !status;}

  TGraph* plot(int Channel) ; 

  void  Convert2root   (Int_t entry1=0, Int_t entry2=0);
  //  void  Convert        (Int_t entry1=0, Int_t entry2=0);
  bool  readDRS4event  ();
  bool  readDRS4channel();

  ClassDef(DRS4bin,0)

};


#endif


