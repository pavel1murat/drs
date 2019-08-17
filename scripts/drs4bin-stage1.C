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
   TFile* ofile;
   TTree* tree;
   union Word4 {
      UInt_t word;
      Char_t sbyte[4];            // signed char: binary write and read need char (signed char)
      Float_t fword;
      UChar_t ubyte[4];   // unsigned char: for test purpose only
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
      if (!ifile) cout<< "File not found: " << ifname <<endl;
      else status = true;
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
   void Convert(Int_t entry1=0, Int_t entry2=0)
   //void Convert(Int_t nevents=0)
   //void Convert()
   {
      Int_t entry = 0;

      // read the first event to figure out used channels
      readDRS4event();

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

      // process the rest of the data
      ReadStatus res = data_read;
      while (res == data_read) {
         res = readDRS4event();
         if (res == data_read) {
            ++entry;
            bool in_range = (entry >= entry1);
            if (entry2 > 0) in_range = in_range and (entry <= entry2);
            if (in_range) tree->Fill();
         }
         if (res == data_bad) {
            cout<< "Convert: received data_bad return value from the event" <<endl;
         }
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
      Word4 word4;

      std::streampos pos = ifile.tellg();    // store current position in the file
      word4.word = 0;
      if (ifile.read(word4.sbyte, 4)) {
         // cout<< "EventHeader: ";
         // cout.write(word4.sbyte,4);
         // cout<<endl;
      }
      else {
         // read error: normally the end of file is expected
         ifile.seekg(pos);                   // restore position in the file
         return data_empty;
      }

      word4.word = 0;
      if (ifile.read(word4.sbyte, 4)) event = word4.word;
      else return data_bad;

      word4.word = 0;
      if (ifile.read(word4.sbyte, 2)) year = word4.word;
      else return data_bad;

      word4.word = 0;
      if (ifile.read(word4.sbyte, 2)) month = word4.word;
      else return data_bad;

      word4.word = 0;
      if (ifile.read(word4.sbyte, 2)) day = word4.word;
      else return data_bad;

      word4.word = 0;
      if (ifile.read(word4.sbyte, 2)) hour = word4.word;
      else return data_bad;

      word4.word = 0;
      if (ifile.read(word4.sbyte, 2)) minute = word4.word;
      else return data_bad;

      word4.word = 0;
      if (ifile.read(word4.sbyte, 2)) second = word4.word;
      else return data_bad;

      word4.word = 0;
      if (ifile.read(word4.sbyte, 2)) millisecond = word4.word;
      else return data_bad;

      word4.word = 0;
      if (ifile.read(word4.sbyte, 2));
      else return data_bad;

      for (int ipoint=0; ipoint<1024; ++ipoint) {
         word4.word = 0;
         if (ifile.read(word4.sbyte, 4)) t[ipoint] = word4.fword;
         else return data_bad;
      }

      ReadStatus res = data_read;
      while (res == data_read) {
         res = readDRS4channel();
         if (res == data_bad) {
            cout<< "readDRS4event: received data_bad return value from the channel" <<endl;
         }
      }

      return data_read;
   }

   //--       readDRS4channel
   ReadStatus readDRS4channel()
   {
      Word4 word4;

      Int_t channel = 0;
      Float_t* v = 0;

      // read channel header

      std::streampos pos = ifile.tellg();             // store current position in the file
      word4.word = 0;
      if (ifile.read(word4.sbyte, 4)) {
         if (word4.sbyte[3] == '1') {channel = 1; v = v1;}
         if (word4.sbyte[3] == '2') {channel = 2; v = v2;}
         if (word4.sbyte[3] == '3') {channel = 3; v = v3;}
         if (word4.sbyte[3] == '4') {channel = 4; v = v4;}
         if (channel == 0) {
            // no more channels
            ifile.seekg(pos);                         // restore position in the file
            return data_empty;
         }
         usedchan[channel-1] = kTRUE;
      }
      else {
         // read error: normally the end of file is expected
         ifile.seekg(pos);                            // restore position in the file
         return data_empty;
      }

      Int_t ipoint = 0;
      while (ipoint < 1024)
      {
         Int_t iy = 0;

         // first part of the 4 byte word
         word4.word = 0;
         if (ifile.read(word4.sbyte, 2)) {
            iy = word4.word;
            v[ipoint] = (iy - 32767.5)/65535.;
            // cout<< ipoint << "\t iy " << iy << " v[ipoint] " << v[ipoint] <<endl;
         }
         else {
            cout<< "readout error for channel " << ipoint <<endl;
            return data_bad;
         }
         ++ipoint;

         // first part of the 4 byte word
         word4.word = 0;
         if (ifile.read(word4.sbyte, 2)) {
            iy = word4.word;
            v[ipoint] = (iy - 32767.5)/65535.;
            // cout<< ipoint << "\t iy " << iy << " v[ipoint] " << v[ipoint] <<endl;
         }
         else {
            cout<< "readout error for channel " << ipoint <<endl;
            return data_bad;
         }
         ++ipoint;
      }
      return data_read;
   }
};

void drs4bin(const char* ifname="data_meander.bin", Int_t entry1=0, Int_t entry2=0)
{
   DRS4bin* drs4bin = new DRS4bin(ifname);
   drs4bin->Convert(entry1,entry2);
}
