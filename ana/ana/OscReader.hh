///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
#ifndef OscReader_hh
#define OscReader_hh

#include "TNamed.h"
#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TCanvas.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cstdarg>
#include <cstdlib>
#include <cstdio>
#include <cstring>

using std::cout;      using std::endl;

/*
  TODO: 
  1) exect EHDR and C001,C002, etc.
  2) read complete 4-bytes word and union with two 2-bytes words
  3) exceptions for readout error
  4) easy way to set brach addresses to read the tree (MakeClass may work)
  -- how to do with different number of channels?
*/


class OscReader: public TNamed {
private:

  union OscStructUnion {
    struct OscStruct {

      union EventTime {
	struct Field {
	  Char_t    header[4];
	  UInt_t    number;
	  UShort_t  year;
	  UShort_t  month;
	  UShort_t  day;
	  UShort_t  hour;
	  UShort_t  minute;
	  UShort_t  second;
	  UShort_t  millisecond;
	  UShort_t  reserved;
	  Float_t   t[1024];
	} field;
	// char buffer[sizeof(Field)];
	char buffer[1*4 + 1*4 + 2*8 + 1024*4];
      } time;

      union Channel {
	struct Field {
	  Char_t header[4];
	  UShort_t voltage[1024];
	} field;
	// Char_t buffer[sizeof(Field)];
	Char_t buffer[1*4 + 1024*2];
      } channel[4];
    } oscStruct;


      // Char_t buffer[sizeof(OscStruct::EventTime) + 4*sizeof(OscStruct::Channel)];
    Char_t buffer[1*4 + 1*4 + 2*8 + 1024*4 + 4*(1*4 + 1024*2)];
  } oscStructUnion;

  // vars

  std::string   ifname;     // to use input file name for messages
  std::ifstream ifile;

  bool          fStatus;
  Long64_t      ifsize;
  Int_t         nchan;
  Int_t         usedchan[4];
//-----------------------------------------------------------------------------
// methods
//-----------------------------------------------------------------------------
public:

  OscReader();
  ~OscReader();

      // getters

  bool operator !() const {return fStatus;}
  
  UInt_t Number     () const {return oscStructUnion.oscStruct.time.field.number;}
  UInt_t Year       () const {return oscStructUnion.oscStruct.time.field.year;}
  UInt_t Day        () const {return oscStructUnion.oscStruct.time.field.day;}
  UInt_t Hour       () const {return oscStructUnion.oscStruct.time.field.hour;}
  UInt_t Minute     () const {return oscStructUnion.oscStruct.time.field.minute;}
  UInt_t Second     () const {return oscStructUnion.oscStruct.time.field.second;}
  UInt_t Millisecond() const {return oscStructUnion.oscStruct.time.field.millisecond;}

  const Float_t* Time() const {return oscStructUnion.oscStruct.time.field.t;}
  
  const UShort_t* Voltage(Int_t ich) const {
    if (ich > 3) return 0;
    return oscStructUnion.oscStruct.channel[ich].field.voltage;
  }

  UInt_t   Nchan() const {return nchan;}

  Int_t    UsedChan(int I) const {return usedchan[I]; }

  bool     Open           (const char* ifname_0);
  Long64_t GetEventPointer();
  bool     ReadEvent      ();
  bool     SetEventPointer(Int_t record);

  ClassDef(OscReader,0)
      
};

#endif
