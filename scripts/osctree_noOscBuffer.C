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

struct OscTime {
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

struct OscChannel {
   Char_t header[4];
   UShort_t voltage[1024];
};

struct OscEvent {
   OscTime time;
   OscChannel channel[4];
};

union OscRecord {
   OscEvent event;
   Char_t buffer[sizeof(OscEvent)];
   OscRecord() {for (unsigned i=0; i<sizeof(OscEvent); ++i) buffer[i] = 0;}
};

class OscBin {
private:
   std::string ifname;     // to use input file name for messages
   std::ifstream ifile;
   OscRecord oscRecord;
public:
   // getters
   UInt_t Number() const {return oscRecord.event.time.number;}
   UInt_t Year() const {return oscRecord.event.time.year;}
   UInt_t Day() const {return oscRecord.event.time.day;}
   UInt_t Hour() const {return oscRecord.event.time.hour;}
   UInt_t Minute() const {return oscRecord.event.time.minute;}
   UInt_t Second() const {return oscRecord.event.time.second;}
   UInt_t Millisecond() const {return oscRecord.event.time.millisecond;}
   const Float_t* Time() const {return oscRecord.event.time.t;}
   const UShort_t* Voltage(Int_t ich) const {
      if (ich > 3) return 0;
      return oscRecord.event.channel[ich].voltage;
   }
   UInt_t Nchan() const {return nchan;}
   const Int_t* UsedChan() const {return usedchan;}

   // vars
   bool status;
   bool operator !() const {return !status;}
   Long64_t ifsize;
   Int_t nchan;
   Int_t usedchan[4];

   OscBin(): status(false) {}
   bool Open(const char* ifname_)
   {
      // use like:
      //
      // OscBin oscBin;
      // oscBin.Open("osc.bin");
      // if (!oscBin) {
      //    cout<< "Input file does not seem to be the DRS4 oscilloscope application binary file" <<endl;
      //    return;
      // }

      ifname = ifname_;
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
         cout<< "OscBin: input file error: ifsize = " << ifsize <<endl;
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

      Long64_t size = sizeof(OscTime) + 4*sizeof(OscChannel);
      while (ifsize < size) size -= sizeof(OscChannel); // min file size: sizeof(OscTime) + 1*sizeof(OscChannel)

      if (ifsize < Long64_t(sizeof(OscTime) + sizeof(OscChannel))) {
         cout<< "OscBin: input file " << ifname << " is too short to contain even single DRS4 osclloscope event" <<endl;
         ifile.close();
         status = false;
         return false;
      }

      ifile.read(oscRecord.buffer, size);

      // check the event header
      if (std::strncmp(oscRecord.event.time.header, header_event, 4) != 0) {
         status = false;
         cout<< "OscBin: No event header found, the input file " << ifname << " does not seem to be the DRS4 osclloscope application binary file" <<endl;
         return false;
      }

      // read channels' headers to figure out how many them and their numbers
      for (nchan=0; nchan<4; ++nchan)
      {
         Int_t channel = 0;
         for (int ich=0; ich<4; ++ich) {
            if (std::strncmp(oscRecord.event.channel[nchan].header, header_channel[ich], 4)==0) {
               channel = ich+1;
               usedchan[ich] = channel;
               // cout<< "OscBin::Open: channel = " << channel << " usedchan[" << ich << "] = " << usedchan[ich] <<endl;
            }
         }
         if (channel == 0) {
            // this is not a channel header
            break;
         }
      }
      if (nchan == 0) {
         status = false;
         cout<< "OscBin: No channels found, the input file " << ifname << " does not seem to be the DRS4 osclloscope application binary file" <<endl;
         return false;
      }

      // cout<< "OscBin: oscilloscope event contains " << nchan << " channels" <<endl;

      // return to the beginning of the file
      ifile.seekg(0);

      status = true;
      return true;
   }
   bool SetEventPointer(Int_t record)
   {
      // set file get pointer to read record record (counting from 0)
      if (!status) return false;
      Long64_t record_size = sizeof(OscEvent);
      if (record_size*(record+1) <= ifsize) ifile.seekg(record*record_size);
      else {
         cout<< "File " << ifname << " has no event " << record <<endl;
         return false;
      }
      // cout<< "OscBin::SetEventPointer: ifile.tellg() = " << ifile.tellg() <<endl;
      return true;
   }
   Long64_t GetEventPointer()
   {
      // set file get pointer to read record record (counting from 0)
      if (!status) return false;
      Long64_t pos = ifile.tellg();
      // cout<< "OscBin::GetEventPointer: ifile.tellg() = " << pos <<endl;
      return pos;
   }
   bool ReadEvent()
   {
      // reads current event

      if (!status) return false;
      Long64_t record_size = sizeof(OscTime) + nchan*sizeof(OscChannel);
      Long64_t curr = ifile.tellg();
      if (curr < 0) {
         cout<< "OscBin::ReadNext: read error: curr = " << curr <<endl;
         return false;
      }
      if (ifsize < curr+record_size) return false;
      else {
         ifile.read(oscRecord.buffer, record_size);
         // check data consistency
         const Char_t header_event[] = {'E', 'H', 'D', 'R'};
         const Char_t header_channel[][4] = {
            {'C', '0', '0', '1'},
            {'C', '0', '0', '2'},
            {'C', '0', '0', '3'},
            {'C', '0', '0', '4'}
         };
         // event header
         if (std::strncmp(oscRecord.event.time.header, header_event, 4) != 0) {
            cout<< "OscBin::ReadNext: corrupted event header" <<endl;
            return false;
         }
         // channel headers
         for (int ich=0; ich<nchan; ++ich) {
            if (std::strncmp(oscRecord.event.channel[ich].header, header_channel[ich], 4) != 0) {
               cout<< "OscBin::ReadNext: corrupted channel header for ich = " << ich <<endl;
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

class Osc {
public:
   static Int_t event;
   static Int_t year;
   static Int_t month;
   static Int_t day;
   static Int_t hour;
   static Int_t minute;
   static Int_t second;
   static Int_t millisecond;
   static Float_t t[1024];
   static Float_t v1[1024];
   static Float_t v2[1024];
   static Float_t v3[1024];
   static Float_t v4[1024];
   static Int_t usedchan[4];
   void Book()
   {
      // books branches of the new tree
      tree->Branch("event", &event, "event/I");
      tree->Branch("year", &year, "year/I");
      tree->Branch("month", &month, "month/I");
      tree->Branch("day", &day, "day/I");
      tree->Branch("hour", &hour, "hour/I");
      tree->Branch("minute", &minute, "minute/I");
      tree->Branch("second", &second, "second/I");
      tree->Branch("millisecond", &millisecond, "millisecond/I");
      tree->Branch("t", &t, "t[1024]/F");
      // channel in use: from 1 to 4 (its number), not in use: 0
      tree->Branch("usedchan", &usedchan, "usedchan[4]/I");
      // book used channels only
      if (usedchan[0] > 0) tree->Branch("v1", &v1, "v1[1024]/F");
      if (usedchan[1] > 0) tree->Branch("v2", &v2, "v2[1024]/F");
      if (usedchan[2] > 0) tree->Branch("v3", &v3, "v3[1024]/F");
      if (usedchan[3] > 0) tree->Branch("v4", &v4, "v4[1024]/F");
   }
   void Connect()
   {
      // connects tree buffers with variables to use for event-by-event analysis
      // rest of the event
      tree->SetBranchAddress("event", &event);
      tree->SetBranchAddress("year", &year);
      tree->SetBranchAddress("month", &month);
      tree->SetBranchAddress("day", &day);
      tree->SetBranchAddress("hour", &hour);
      tree->SetBranchAddress("minute", &minute);
      tree->SetBranchAddress("second", &second);
      tree->SetBranchAddress("millisecond", &millisecond);
      tree->SetBranchAddress("t", &t);
      // channel in use: from 1 to 4 (its number), not in use: 0
      tree->SetBranchAddress("usedchan", &usedchan);
      // book used channels only
      if (usedchan[0] > 0) tree->SetBranchAddress("v1", &v1);
      if (usedchan[1] > 0) tree->SetBranchAddress("v2", &v2);
      if (usedchan[2] > 0) tree->SetBranchAddress("v3", &v3);
      if (usedchan[3] > 0) tree->SetBranchAddress("v4", &v4);
   }

   std::string ifname;
   TTree* tree;
   TTree* Tree() {return tree;}
   OscBin oscBin;          // oscilloscope binary file reader

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

   Osc(const char* ifname_): ifname(ifname_), tree(0)
   {
      if (oscBin.Open(ifname.c_str())) {
         cout<< "Successfully opened DRS4 oscilloscope binary file " << ifname.c_str() <<endl;
         return;
      }

      // try to open as a root file
      TFile* ifile = TFile::Open(ifname.c_str());
      if (!ifile) {
         cout<< "Not a root file: " << ifname <<endl;
         return;
      }
      tree = (TTree*) ifile->Get("osc");
      if (!tree) {
         cout<< "There is no DRS4 oscilloscope tree in file " << ifname <<endl;
         return;
      }
      cout<< "Found DRS4 oscilloscope tree \"osc\" in ROOT file " << ifname <<endl;

      Connect();                 // connect tree buffers with static fields of class Osc
      tree->GetEntry(0);

      cout<< "DRS4 channels: ";
      for (int ich=0; ich<4; ++ich) {
         usedchan[ich] = oscBin.UsedChan()[ich];
         if (usedchan[ich]) cout<< usedchan[ich] << "  ";
      }
      cout<<endl;
   }

   void Convert(Int_t entry1=0, Int_t entry2=0)
   {
      if (tree) return;
      if (!oscBin) return;

      cout<< "DRS4 channels: ";
      for (int ich=0; ich<4; ++ich) {
         usedchan[ich] = oscBin.UsedChan()[ich];
         if (usedchan[ich]) cout<< usedchan[ich] << "  ";
      }
      cout<<endl;

      TFile* ofile = TFile::Open(Form("%s.root",ifname.c_str()), "recreate");

      tree = new TTree("osc", "DRS4 oscilloscope data");
      tree->SetMarkerStyle(6);
      tree->SetMarkerColor(46);
      tree->SetLineColor(46);

      Book();                             // book tree

      Int_t ientry = entry1;
      oscBin.SetEventPointer(ientry);

      while (oscBin.ReadEvent())
      {
         if (tree->GetEntries() % 100 == 0) cout<< "processing ientry " << ientry << " oscBin.Number = " << oscBin.Number() <<endl;

         for (int ich=0; ich<4; ++ich) {
            usedchan[ich] = oscBin.UsedChan()[ich];
            // cout<< ich << "\t " << usedchan[ich] <<endl;
         }
         event = oscBin.Number();
         year = oscBin.Year();
         day = oscBin.Day();
         hour = oscBin.Hour();
         minute = oscBin.Minute();
         second = oscBin.Second();
         millisecond = oscBin.Millisecond();
         for (int ipoint=0; ipoint<1024; ++ipoint) t[ipoint] = oscBin.Time()[ipoint];
         for (unsigned ich=0; ich<oscBin.Nchan(); ++ich) {
            Float_t* v = 0;
            Int_t channel = oscBin.UsedChan()[ich];
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
               v[ipoint] = (Float_t(oscBin.Voltage(ich)[ipoint]) - 32767.5) / 65536.;
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
Int_t Osc::event;
Int_t Osc::year;
Int_t Osc::month;
Int_t Osc::day;
Int_t Osc::hour;
Int_t Osc::minute;
Int_t Osc::second;
Int_t Osc::millisecond;
Float_t Osc::t[1024];
Float_t Osc::v1[1024];
Float_t Osc::v2[1024];
Float_t Osc::v3[1024];
Float_t Osc::v4[1024];
Int_t Osc::usedchan[4];

TTree* osctree(const char* ifname, Int_t entry1=0, Int_t entry2=0)
{
   Osc* osc = new Osc(ifname);
   osc->Convert(entry1,entry2);
   return osc->Tree();
}
