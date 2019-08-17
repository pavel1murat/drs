#ifdef __MAKECINT__
#include <TFile.h>
#include <TTree.h>
#include <TGraph.h>
#include <TCanvas.h>
#endif

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cstdarg>
#include <cstdlib>
#include <cstdio>
#include <cstring>

using std::cout;      using std::endl;

typedef int Int_t;
typedef unsigned int UInt_t;
typedef char Char_t;
typedef unsigned short UShort_t;
typedef float Float_t;

// union EventTime {
//    struct Field {
//       Char_t header[4];
//       UInt_t number;
//       UShort_t year;
//       UShort_t month;
//       UShort_t day;
//       UShort_t hour;
//       UShort_t minute;
//       UShort_t second;
//       UShort_t millisecond;
//       UShort_t reserved;
//       Float_t t[1024];
//    };
//    // char buffer[sizeof(Field)];
//    char buffer[1*4 + 1*4 + 2*8 + 1024*4];
// };
// 
// union Channel {
//    struct Field {
//       Char_t header[4];
//       UShort_t voltage[1024];
//    };
//    // Char_t buffer[sizeof(Field)];
//    Char_t buffer[1*4 + 1024*2];
// };

struct OsciTime {
   Char_t header[4];
   UInt_t number;
   UShort_t year;
   UShort_t month;
   UShort_t day;
   UShort_t hour;
   UShort_t minute;
   UShort_t second;
   UShort_t millisecond;
   UShort_t reserved;
   Float_t t[1024];
};

struct OsciChannel {
   Char_t header[4];
   UShort_t voltage[1024];
};

struct OsciStruct {
   OsciTime time;
   OsciChannel channel[4];
};

union OsciStructUnion {
   OsciStruct osciStruct;
   Char_t buffer[sizeof(OsciTime) + 4*sizeof(OsciChannel)];
};

// union OsciStructUnion {
//    struct {
//       OsciTime time;
//       OsciChannel channel[4];
//    };
//    Char_t buffer[sizeof(OsciTime) + 4*sizeof(OsciChannel)];
// };

void unionroot()
{
   // OsciStructUnion osciStructUnion;
   // cout<< "sizeof(osciStructUnion) = " << sizeof(osciStructUnion) <<endl;

   cout<< "sizeof(OsciStructUnion) = " << sizeof(OsciStructUnion) <<endl;
}
