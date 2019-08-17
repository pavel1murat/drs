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
#include <cstring>

using std::cout;      using std::endl;

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

struct OsciEvent {
   OsciTime time;
   OsciChannel channel[4];
};

union OsciRecord {
   OsciEvent event;
   Char_t buffer[sizeof(OsciEvent)];
   OsciRecord() {for (unsigned i=0; i<sizeof(OsciEvent); ++i) buffer[i] = 0;}
};

class OsciBin {
private:
   std::string ifname;     // to use input file name for messages
   std::ifstream ifile;
   OsciRecord osciRecord;
public:
   // getters
   UInt_t Number() const {return osciRecord.event.time.number;}
   UInt_t Year() const {return osciRecord.event.time.year;}
   UInt_t Day() const {return osciRecord.event.time.day;}
   UInt_t Hour() const {return osciRecord.event.time.hour;}
   UInt_t Minute() const {return osciRecord.event.time.minute;}
   UInt_t Second() const {return osciRecord.event.time.second;}
   UInt_t Millisecond() const {return osciRecord.event.time.millisecond;}
   const Float_t* Time() const {return osciRecord.event.time.t;}
   const UShort_t* Voltage(Int_t ich) const {
      if (ich > 3) return 0;
      return osciRecord.event.channel[ich].voltage;
   }
   UInt_t Nchan() const {return nchan;}
   const Int_t* UsedChan() const {return usedchan;}

   // vars
   bool status;
   bool operator !() const {return status;}
   Long64_t ifsize;
   Int_t nchan;
   Int_t usedchan[4];

   OsciBin(): status(false) {}
   bool Open(const char* ifname_0)
   {
      // use like:
      //
      // OsciBin osciBin;
      // osciBin.Open("osc.bin");
      // if (!osciBin) {
      //    cout<< "Input file does not seem to be the DRS4 oscilloscope application binary file" <<endl;
      //    return;
      // }

      ifname = ifname_0;
      status = true;

      // try to open input file as the oscilloscope application binary file
      ifile.open(ifname.c_str(), std::ios::binary);
      if (!ifile) {
         cout<< "File not found: " << ifname <<endl;
         status = false;
         return false;
      }
      cout<< "processing file " << ifname <<endl;

      // file size
      ifile.seekg(0, std::ios::end);
      ifsize = ifile.tellg();
      ifile.seekg(0);

      if (ifsize < 0) {
         cout<< "OsciBin: input file error: ifsize = " << ifsize <<endl;
         ifile.close();
         status = false;
         return false;
      }

      // read the first event to figure out the number of channels

      const Char_t header_event[] = {'E', 'H', 'D', 'R'};
      const Char_t header_channel[][4] = {
         {'C', '0', '0', '1'},
         {'C', '0', '0', '2'},
         {'C', '0', '0', '3'},
         {'C', '0', '0', '4'}
      };

      Long64_t size = sizeof(OsciTime) + 4*sizeof(OsciChannel);
      while (ifsize < size) size -= sizeof(OsciChannel); // min file size: sizeof(OsciTime) + 1*sizeof(OsciChannel)

      if (ifsize < Long64_t(sizeof(OsciTime) + sizeof(OsciChannel))) {
         cout<< "OsciBin: input file " << ifname << " is too short to contain even single DRS4 oscilloscope event" <<endl;
         ifile.close();
         status = false;
         return false;
      }

      ifile.read(osciRecord.buffer, size);

      // check the event header
      if (std::strncmp(osciRecord.event.time.header, header_event, 4) != 0) {
         status = false;
         cout<< "OsciBin: No event header found, the input file " << ifname << " does not seem to be the DRS4 oscilloscope application binary file" <<endl;
         return false;
      }

      // read channels' headers to figure out how many them and their numbers
      for (nchan=0; nchan<4; ++nchan)
      {
         Int_t channel = 0;
         for (int ich=0; ich<4; ++ich) {
            if (std::strncmp(osciRecord.event.channel[nchan].header, header_channel[ich], 4)==0) {
               channel = ich+1;
               usedchan[ich] = channel;
               cout<< "OsciBin::Open: channel = " << channel << " usedchan[" << ich << "] = " << usedchan[ich] <<endl;
            }
         }
         if (channel == 0) {
            // this is not a channel header
            break;
         }
      }
      if (nchan == 0) {
         status = false;
         cout<< "OsciBin: No channels found, the input file " << ifname << " does not seem to be the DRS4 oscilloscope application binary file" <<endl;
         return false;
      }

      // return to the beginning of the file
      ifile.seekg(0);

      cout<< "OsciBin: oscilloscope event contains " << nchan << " channels" <<endl;
      return true;
   }
   bool SetEventPointer(Int_t record)
   {
      // set file get pointer to read record record (counting from 0)
      if (!status) return false;
      Long64_t record_size = sizeof(OsciEvent);
      if (record_size*(record+1) <= ifsize) ifile.seekg(record*record_size);
      else {
         cout<< "File " << ifname << " has no event " << record <<endl;
         return false;
      }
      // cout<< "OsciBin::SetEventPointer: ifile.tellg() = " << ifile.tellg() <<endl;
      return true;
   }
   Long64_t GetEventPointer()
   {
      // set file get pointer to read record record (counting from 0)
      if (!status) return false;
      Long64_t pos = ifile.tellg();
      // cout<< "OsciBin::GetEventPointer: ifile.tellg() = " << pos <<endl;
      return pos;
   }
   bool ReadEvent()
   {
      // reads current event

      if (!status) return false;
      Long64_t record_size = sizeof(OsciTime) + nchan*sizeof(OsciChannel);
      Long64_t curr = ifile.tellg();
      if (curr < 0) {
         cout<< "OsciBin::ReadNext: read error: curr = " << curr <<endl;
         return false;
      }
      if (ifsize < curr+record_size) return false;
      else {
         ifile.read(osciRecord.buffer, record_size);
         // check data consistency
         const Char_t header_event[] = {'E', 'H', 'D', 'R'};
         const Char_t header_channel[][4] = {
            {'C', '0', '0', '1'},
            {'C', '0', '0', '2'},
            {'C', '0', '0', '3'},
            {'C', '0', '0', '4'}
         };
         // event header
         if (std::strncmp(osciRecord.event.time.header, header_event, 4) != 0) {
            cout<< "OsciBin::ReadNext: corrupted event header" <<endl;
            return false;
         }
         // channel headers
         for (int ich=0; ich<nchan; ++ich) {
            if (std::strncmp(osciRecord.event.channel[ich].header, header_channel[ich], 4) != 0) {
               cout<< "OsciBin::ReadNext: corrupted channel header for ich = " << ich <<endl;
               return false;
            }
         }
      }
      return true;
   }
};

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
   static Float_t t[1024];
   static Float_t v1[1024];
   static Float_t v2[1024];
   static Float_t v3[1024];
   static Float_t v4[1024];
   static Int_t usedchan[4];
   static Int_t event;
   static Int_t year;
   static Int_t month;
   static Int_t day;
   static Int_t hour;
   static Int_t minute;
   static Int_t second;
   static Int_t millisecond;
   enum filetype {bin, root};
   std::string ifname;
   Long64_t ifsize;
   TFile* ofile;
   TTree* tree;

   OsciBin osciBin;

   static TGraph* plot(Int_t ch)
   {
      if (!usedchan[ch]) return 0;
      const Int_t color[] = {2, 4, 6, 8};

      Float_t* v = 0;
      if (ch == 1) v = v1;
      if (ch == 2) v = v2;
      if (ch == 3) v = v3;
      if (ch == 4) v = v4;
      TGraph* gr = new TGraph(1024,t,v);
      gr->SetNameTitle(Form("gr_evt_%d_chan_%d",event,ch), Form("gr_evt_%d_chan_%d",event,ch));
      gr->SetMarkerStyle(6);
      gr->SetMarkerColor(color[ch]);
      gr->Draw("ap");
      return gr;
   }

   DRS4bin(std::string ifname_0): status(false)
                                  , ifname(ifname_0)
                                  , ofile(0)
                                  , tree(0)
   {
      if (osciBin.Open(ifname.c_str())) {
         cout<< "Successfully opened Osci binary file " << ifname.c_str() <<endl;
         return;
      }
   }

   void Convert(Int_t entry1=0, Int_t entry2=0)
   {
      cout<< "Convert2root: entry1 = " << entry1 << " entry2 = " <<entry2 <<endl;

      for (int ich=0; ich<4; ++ich) {
         usedchan[ich] = osciBin.UsedChan()[ich];
         cout<< ich << "\t " << usedchan[ich] <<endl;
      }
      cout<< "DRS4 channels: ";
      for (int ich=0; ich<4; ++ich) cout<< ich+1 << (usedchan[ich]? ": yes ": ": no ");
      cout<<endl;

      ofile = TFile::Open(Form("%s.root",ifname.c_str()),"recreate");
      tree = new TTree("drs", "produced from DRS4 binary file");
      tree->SetMarkerStyle(6);
      tree->SetMarkerColor(46);
      tree->SetLineColor(46);

      tree->Branch("t", &t, "t[1024]/F");
      // book used channels
      if (usedchan[0] > 0) tree->Branch("v1", &v1, "v1[1024]/F");
      if (usedchan[1] > 0) tree->Branch("v2", &v2, "v2[1024]/F");
      if (usedchan[2] > 0) tree->Branch("v3", &v3, "v3[1024]/F");
      if (usedchan[3] > 0) tree->Branch("v4", &v4, "v4[1024]/F");
      // rest of the event
      tree->Branch("usedchan", &usedchan, "usedchan[4]/I");
      tree->Branch("event", &event, "event/I");
      tree->Branch("year", &year, "year/I");
      tree->Branch("month", &month, "month/I");
      tree->Branch("day", &day, "day/I");
      tree->Branch("hour", &hour, "hour/I");
      tree->Branch("minute", &minute, "minute/I");
      tree->Branch("second", &second, "second/I");
      tree->Branch("millisecond", &millisecond, "millisecond/I");

      Int_t ientry = entry1;
      osciBin.SetEventPointer(ientry);

      while (osciBin.ReadEvent())
      {
         cout<< "processing ientry " << ientry << " osciBin.Number = " << osciBin.Number() <<endl;

         for (int ich=0; ich<4; ++ich) {
            usedchan[ich] = osciBin.UsedChan()[ich];
            // cout<< ich << "\t " << usedchan[ich] <<endl;
         }
         event = osciBin.Number();
         year = osciBin.Year();
         day = osciBin.Day();
         hour = osciBin.Hour();
         minute = osciBin.Minute();
         second = osciBin.Second();
         millisecond = osciBin.Millisecond();
         for (int ipoint=0; ipoint<1024; ++ipoint) t[ipoint] = osciBin.Time()[ipoint];
         for (unsigned ich=0; ich<osciBin.Nchan(); ++ich) {
            Float_t* v = 0;
            Int_t channel = osciBin.UsedChan()[ich];
            if (channel > 0) {
               if (channel == 1) v = v1;
               if (channel == 2) v = v2;
               if (channel == 3) v = v3;
               if (channel == 4) v = v4;
            }
            if (!v) {
               cout<< "something wrong: v == 0" <<endl;
               return;
            }
            for (int ipoint=0; ipoint<1024; ++ipoint) {
               v[ipoint] = (Float_t(osciBin.Voltage(ich)[ipoint]) - 32767.5) / 65536.;
            }
         }

         tree->Fill();
         if (entry2 > 0 && ientry >= entry2) break;
         ++ientry;
      }

      cout<< "write " << tree->GetEntries() << " entries into file " << ofile->GetName() <<endl;
      ofile->Write();

      new TCanvas;
      tree->Draw("v1:t","");
   }
};

// define static members
Float_t DRS4bin::t[1024];
Float_t DRS4bin::v1[1024];
Float_t DRS4bin::v2[1024];
Float_t DRS4bin::v3[1024];
Float_t DRS4bin::v4[1024];
Int_t DRS4bin::usedchan[4];
Int_t DRS4bin::event;
Int_t DRS4bin::year;
Int_t DRS4bin::month;
Int_t DRS4bin::day;
Int_t DRS4bin::hour;
Int_t DRS4bin::minute;
Int_t DRS4bin::second;
Int_t DRS4bin::millisecond;

void osci(const char* ifname, Int_t entry1=0, Int_t entry2=0)
{
   DRS4bin* drs4bin = new DRS4bin(ifname);
   drs4bin->Convert(entry1,entry2);
}
