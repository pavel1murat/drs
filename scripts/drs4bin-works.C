/*
g++ -Wall drs4bin.cpp
./a.out drs4bin.cpp
*/

#include <TFile.h>
#include <TTree.h>
#include <TGraph.h>
#include <TCanvas.h>

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cstdarg>
#include <cstdlib>
#include <cstdio>

using std::cout;      using std::endl;

/*
TODO: 
1) exect EHDR and C001,C002, etc.
2) read complete 4-bytes word and union with two 2-bytes words
3) exceptions for readout error
4) easy way to set brach addresses to read the tree (MakeClass may work)
   -- how to do with different number of channels?
*/

class DRS4bin {
public:
   bool status;
   bool operator !() {return !status;}
   Float_t t[1024];
   Float_t v1[1024];
   Float_t v2[1024];
   Float_t v3[1024];
   Float_t v4[1024];
   Bool_t usedchan[4];
   Int_t event;
   Int_t year;
   Int_t month;
   Int_t day;
   Int_t hour;
   Int_t minute;
   Int_t second;
   Int_t millisecond;
   std::string ifname;
   std::ifstream ifile;
   Long64_t ifsize;
   TFile* ofile;
   TTree* tree;

   // stuff for readout
   union Word4 {
      UInt_t word;
      Float_t fword;
      UShort_t word2[2];
      Char_t sbyte[4];     // signed char: binary write and read need char (signed char)
      UChar_t ubyte[4];    // unsigned char: for test purpose only
      Word4(): word(0) {}
      void print() const {
         cout<< "decimal word: " << word <<endl;
         cout<< "word: " << std::hex << word << std::dec <<endl;
         printf("sbyte: %02x %02x %02x %02x\n", (unsigned char) ubyte[0], (unsigned char) ubyte[1], (unsigned char) ubyte[2], (unsigned char) ubyte[3]);
         printf("ubyte: %02x %02x %02x %02x\n", ubyte[0], ubyte[1], ubyte[2], ubyte[3]);
      }
   };
   enum ReadStatus { data_read, data_empty, data_bad };
   DRS4bin(std::string ifname_0): status(false)
                                  , event(0)
                                  , year(0)
                                  , month(0)
                                  , day(0)
                                  , hour(0)
                                  , minute(0)
                                  , second(0)
                                  , millisecond(0)
                                  , ifname(ifname_0)
                                  , ofile(0)
                                  , tree(0)
   {
      ifile.open(ifname.c_str(), std::ios::binary);
      if (!ifile) {
         cout<< "File not found: " << ifname <<endl;
         return;
      }
      cout<< "processing file " << ifname <<endl;

      // file length
      ifile.seekg(0, std::ios::end);
      ifsize = ifile.tellg();
      ifile.seekg(0);
      if (ifsize < 0) {
         cout<< "input file error: ifsize = " << ifsize <<endl;
         return;
      }
      else cout<< "ifsize = " << ifsize <<endl;

      status = true;

      for (int ipoint=0; ipoint<1024; ++ipoint) {
         t[ipoint] = 0;
         v1[ipoint] = 0;
         v2[ipoint] = 0;
         v3[ipoint] = 0;
         v4[ipoint] = 0;
      }
      usedchan[0] = usedchan[1] = usedchan[2] = usedchan[3] = kFALSE;
      if (!status) return;
   }

   //-- Convert
   void Convert(Int_t entry1=0, Int_t entry2=0)
   {
      // set exception mask for ifile
      ifile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

      // read the first event to figure out used channels
      try {
         readDRS4event();
      }
      catch (std::ifstream::failure e) {
         //cout<< "read error: " << e.what() <<endl;
         cout<< "DRS4bin::Convert: exception caught: read error at the first event" <<endl;
         return;
      }

      cout<< "\n after the first event:" <<endl;

      for (int ich=0; ich<4; ++ich) {
         cout<< "usedchan[" << ich << "] = " << std::boolalpha << usedchan[ich] <<endl;
      }

      ofile = TFile::Open(Form("%s.root",ifname.c_str()),"recreate");
      tree = new TTree("drs", "produced from DRS4 binary file");
      tree->SetMarkerStyle(6);
      tree->SetMarkerColor(46);
      tree->SetLineColor(46);

      tree->Branch("t", &t, "t[1024]/F");
      // book used channels
      if (usedchan[0]) tree->Branch("v1", &v1, "v1[1024]/F");
      if (usedchan[1]) tree->Branch("v2", &v2, "v2[1024]/F");
      if (usedchan[2]) tree->Branch("v3", &v3, "v3[1024]/F");
      if (usedchan[3]) tree->Branch("v4", &v4, "v4[1024]/F");
      // rest of the event
      tree->Branch("usedchan", &usedchan, "usedchan[4]/b");
      tree->Branch("event", &event, "event/I");
      tree->Branch("year", &year, "year/I");
      tree->Branch("month", &month, "month/I");
      tree->Branch("day", &day, "day/I");
      tree->Branch("hour", &hour, "hour/I");
      tree->Branch("minute", &minute, "minute/I");
      tree->Branch("second", &second, "second/I");
      tree->Branch("millisecond", &millisecond, "millisecond/I");

      if (entry1 == 0) tree->Fill();

      // TODO calculate event size and seekg to proper position

      // process the rest of the data
      try {
         ReadStatus res = data_read;
         while (res == data_read) {
            res = readDRS4event();
            if (res == data_read) {
               if (tree->GetEntries() >= entry1) tree->Fill();
               if (entry2 > 0 && tree->GetEntries() >= entry2) break;
            }
         }
      }
      catch (std::ifstream::failure e) {
         //cout<< "read error: " << e.what() <<endl;
         cout<< "DRS4bin::Convert: exception caught: read error at event " << tree->GetEntries() <<endl;
         return;
      }

      ofile->Write();

      new TCanvas;
      tree->Draw("v1:t","");

      // TGraph* gr = new TGraph(1024,t,v1);
      // gr->SetMarkerStyle(6);
      // gr->SetMarkerColor(2);
      // new TCanvas;
      // gr->Draw("ap");
   }

   //--       readDRS4event()
   ReadStatus readDRS4event()
   {
      Long64_t pos = ifile.tellg();
      if (pos < 0) return data_bad;

      if (pos == ifsize) {return data_empty;}

      union {
         Char_t buffer[24];
         struct {
            char title[4];
            UInt_t event;        // serial number starting from 1
            UShort_t year;
            UShort_t month;
            UShort_t day;
            UShort_t hour;
            UShort_t minute;
            UShort_t second;
            UShort_t millisecond;
            UShort_t reserved;
         };
      } header;

      ifile.read(header.buffer, sizeof(header.buffer));

      // sanity check
      bool ok = true
         and (header.title[0] == 'E')
         and (header.title[1] == 'H')
         and (header.title[2] == 'D')
         and (header.title[3] == 'R')
         ;
      if (!ok) {
         cout<< "this is not an Event Header: ";
         cout.write(header.title,4);
         cout<<endl;
         return data_empty;
      }

      event = header.event;
      year = header.year;
      month = header.month;
      day = header.day;
      hour = header.hour;
      minute = header.minute;
      second = header.second;
      millisecond = header.millisecond;

      //cout<< "year " << year << " month " << month << " day " << day << " hour " << hour << " minute " << minute << " second " << second << " millisecond " << millisecond <<endl;

      // read time bins
      char* t_buffer = (char*) t;
      ifile.read(t_buffer, 1024*4);

      // read channels
      ReadStatus res = data_read;
      while (res == data_read) {
         res = readDRS4channel();
         // if (res == data_bad) {
         //    cout<< "readDRS4event: received data_bad return value from the channel" <<endl;
         // }
      }

      return data_read;
   }

   //--       readDRS4channel
   ReadStatus readDRS4channel()
   {
      Long64_t pos = ifile.tellg();
      if (pos < 0) return data_bad;

      if (pos == ifsize) {return data_empty;}

      char header[4];

      ifile.read(header, sizeof(header));

      // sanity check
      Float_t* v = 0;
      // if (header[3] == '1') v = v1;
      // if (header[3] == '2') v = v2;
      // if (header[3] == '3') v = v3;
      // if (header[3] == '4') v = v4;
      switch (header[3]) {
         case '1': v = v1; usedchan[0] = kTRUE; break;
         case '2': v = v2; usedchan[1] = kTRUE; break;
         case '3': v = v3; usedchan[2] = kTRUE; break;
         case '4': v = v4; usedchan[3] = kTRUE; break;
      }

      if (v == 0 or (header[0] != 'C') or (header[1] != '0') or (header[2] != '0'))
      {
         // this is not a channel header
         ifile.seekg(pos);    // return to previous position
         // cout<< "this is not a Channel Header: ";
         // cout.write(header,4);
         // cout<<endl;
         return data_empty;
      }

      union {
         Char_t byte[2048];
         UShort_t word2[1024];
      } voltage;

      ifile.read(voltage.byte, sizeof(voltage.byte));
      for (int ipoint=0; ipoint<1024; ++ipoint) {
         v[ipoint] = (Float_t(voltage.word2[ipoint]) - 32767.5) / 65536.;
      }

      return data_read;
   }
};

void drs4bin(const char* ifname="data_meander.bin", Int_t entry1=0, Int_t entry2=0)
{
   DRS4bin* drs4bin = new DRS4bin(ifname);
   drs4bin->Convert(entry1,entry2);
}
