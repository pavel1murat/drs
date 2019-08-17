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
   std::ifstream ifile;
   Long64_t ifsize;
   TFile* ofile;
   TTree* tree;

   class OscReader
   {
   private:
      std::string ifname;     // to use input file name for messages
      std::ifstream ifile;
      union OscStructUnion {
         struct OscStruct {
            union EventTime {
               struct Field {
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
   public:
      // getters
      UInt_t Number() const {return oscStructUnion.oscStruct.time.field.number;}
      UInt_t Year() const {return oscStructUnion.oscStruct.time.field.year;}
      UInt_t Day() const {return oscStructUnion.oscStruct.time.field.day;}
      UInt_t Hour() const {return oscStructUnion.oscStruct.time.field.hour;}
      UInt_t Minute() const {return oscStructUnion.oscStruct.time.field.minute;}
      UInt_t Second() const {return oscStructUnion.oscStruct.time.field.second;}
      UInt_t Millisecond() const {return oscStructUnion.oscStruct.time.field.millisecond;}
      const Float_t* Time() const {return oscStructUnion.oscStruct.time.field.t;}
      const UShort_t* Voltage(Int_t ich) const {
         if (ich > 3) return 0;
         return oscStructUnion.oscStruct.channel[ich].field.voltage;
      }
      UInt_t Nchan() const {return nchan;}
      const Int_t* UsedChan() const {return usedchan;}

      // vars
      bool status;
      bool operator !() const {return status;}
      Long64_t ifsize;
      Int_t nchan;
      Int_t usedchan[4];

      OscReader(): status(false) {}
      bool Open(const char* ifname_0)
      {
         // use like:
         //
         // OscReader oscReader("osc.bin");
         // if (!oscReadout) {
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

         // series of checks
         if (ifsize < 0) {
            cout<< "OscReader: input file error: ifsize = " << ifsize <<endl;
            ifile.close();
            status = false;
            return false;
         }
         Long64_t sizeof_time_channel1 = sizeof(OscStructUnion::OscStruct::EventTime) + sizeof(OscStructUnion::OscStruct::Channel);
         if (ifsize < sizeof_time_channel1 ) {
            cout<< "OscReader: input file " << ifname << " is too short to contain even single DRS4 oscilloscope event" <<endl;
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

         // read the time and the first channel
         //ifile.read(oscStructUnion.buffer, sizeof_time_channel1);
         ifile.read(oscStructUnion.buffer, sizeof(OscStructUnion::OscStruct::EventTime));

         // check the event header
         //cout.write(oscStructUnion.oscStruct.time.field.header, 4); cout<<endl;
         //cout<< "event header check: " << std::strncmp(oscStructUnion.oscStruct.time.field.header, header_event, 4) <<endl;
         if (std::strncmp(oscStructUnion.oscStruct.time.field.header, header_event, 4) != 0) {
            status = false;
            cout<< "OscReader: No event header found, the input file " << ifname << " does not seem to be the DRS4 oscilloscope application binary file" <<endl;
            return false;
         }
         
         // read channels to figure out how many them and their numbers
         for (nchan=0; nchan<4; ++nchan) {
            // will the channel fit in the file?
            if (ifsize - ifile.tellg() < Long64_t(sizeof(OscStructUnion::OscStruct::Channel))) break;
            ifile.read(oscStructUnion.oscStruct.channel[nchan].buffer, sizeof(OscStructUnion::OscStruct::Channel));
            //cout.write(oscStructUnion.oscStruct.channel[nchan].field.header, 4); cout<<endl;
            //cout.write(header_channel[nchan], 4); cout<<endl;
            //cout<< "channel header check: " << std::strncmp(oscStructUnion.oscStruct.channel[nchan].field.header, header_channel[nchan], 4) <<endl;

            Int_t channel = 0;
            for (int ich=0; ich<4; ++ich) {
               if (std::strncmp(oscStructUnion.oscStruct.channel[nchan].field.header, header_channel[ich], 4)==0) {
                  channel = ich+1;
                  usedchan[ich] = channel;
                  cout<< "OscReader::Open: channel = " << channel << " usedchan[" << ich << "] = " << usedchan[ich] <<endl;
               }
            }
            if (channel == 0) {
               // this is not a channel header
               break;
            }
         }
         if (nchan == 0) {
            status = false;
            cout<< "OscReader: No channels found, the input file " << ifname << " does not seem to be the DRS4 oscilloscope application binary file" <<endl;
            return false;
         }

         // return to the beginning of the file
         ifile.seekg(0);

         cout<< "OscReader: oscilloscope event contains " << nchan << " channels" <<endl;
         return true;
      }
      bool SetEventPointer(Int_t record)
      {
         // set file get pointer to read record record (counting from 0)
         if (!status) return false;
         Long64_t record_size = sizeof(OscStructUnion::OscStruct::EventTime) + nchan*sizeof(OscStructUnion::OscStruct::Channel);
         if (record_size*(record+1) <= ifsize) ifile.seekg(record*record_size);
         else {
            cout<< "File " << ifname << " has no event " << record <<endl;
            return false;
         }
         // cout<< "OscReader::SetEventPointer: ifile.tellg() = " << ifile.tellg() <<endl;
         return true;
      }
      Long64_t GetEventPointer()
      {
         // set file get pointer to read record record (counting from 0)
         if (!status) return false;
         Long64_t pos = ifile.tellg();
         // cout<< "OscReader::GetEventPointer: ifile.tellg() = " << pos <<endl;
         return pos;
      }
      bool ReadEvent()
      {
         // reads current event

         if (!status) return false;
         Long64_t record_size = sizeof(OscStructUnion::OscStruct::EventTime) + nchan*sizeof(OscStructUnion::OscStruct::Channel);
         Long64_t curr = ifile.tellg();
         if (curr < 0) {
            cout<< "OscReader::ReadNext: read error: curr = " << curr <<endl;
            return false;
         }
         if (ifsize < curr+record_size) return false;
         else {
            ifile.read(oscStructUnion.buffer, record_size);
            // check data consistency
            const Char_t header_event[] = {'E', 'H', 'D', 'R'};
            const Char_t header_channel[][4] = {
               {'C', '0', '0', '1'},
               {'C', '0', '0', '2'},
               {'C', '0', '0', '3'},
               {'C', '0', '0', '4'}
            };
            // event header
            if (std::strncmp(oscStructUnion.oscStruct.time.field.header, header_event, 4) != 0) {
               cout<< "OscReader::ReadNext: corrupted event header" <<endl;
               return false;
            }
            // channel headers
            for (int ich=0; ich<nchan; ++ich) {
               if (std::strncmp(oscStructUnion.oscStruct.channel[ich].field.header, header_channel[ich], 4) != 0) {
                  cout<< "OscReader::ReadNext: corrupted header for ich = " << ich <<endl;
                  return false;
               }
            }
         }
         return true;
      }
   };
   OscReader oscReader;    // create an instance of the OscReader

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
      if (oscReader.Open(ifname.c_str())) {
         cout<< "Successfully opened Osci binary file " << ifname.c_str() <<endl;
         //Convert2root();
         return;
      }

      ifile.open(ifname.c_str(), std::ios::binary);
      if (!ifile) {
         cout<< "File not found: " << ifname <<endl;
         return;
      }
      cout<< "processing file " << ifname <<endl;

      //-- file length
      // TODO 
      ifile.seekg(0, std::ios::end);
      ifsize = ifile.tellg();
      ifile.seekg(0);
      if (ifsize < 0) {
         cout<< "input file error: ifsize = " << ifsize <<endl;
         return;
      }
      // else cout<< "ifsize = " << ifsize <<endl;

      status = true;
      usedchan[0] = usedchan[1] = usedchan[2] = usedchan[3] = kFALSE;
   }

   //-- Convert2root
   void Convert2root(Int_t entry1=0, Int_t entry2=0)
   {
      cout<< "Convert2root: entry1 = " << entry1 << " entry2 = " <<entry2 <<endl;

      for (int ich=0; ich<4; ++ich) {
         usedchan[ich] = oscReader.UsedChan()[ich];
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
      oscReader.SetEventPointer(ientry);

      // cout<< "Conver2root: oscReader.GetEventPointer() = " << oscReader.GetEventPointer() <<endl;

      // cout<< "--> start reading" <<endl;

      while (oscReader.ReadEvent())
      {
         cout<< "processing ientry " << ientry << " oscReader.Number = " << oscReader.Number() <<endl;

         for (int ich=0; ich<4; ++ich) {
            usedchan[ich] = oscReader.UsedChan()[ich];
            // cout<< ich << "\t " << usedchan[ich] <<endl;
         }
         event = oscReader.Number();
         year = oscReader.Year();
         day = oscReader.Day();
         hour = oscReader.Hour();
         minute = oscReader.Minute();
         second = oscReader.Second();
         millisecond = oscReader.Millisecond();
         for (int ipoint=0; ipoint<1024; ++ipoint) t[ipoint] = oscReader.Time()[ipoint];
         for (unsigned ich=0; ich<oscReader.Nchan(); ++ich) {
            Float_t* v = 0;
            Int_t channel = oscReader.UsedChan()[ich];
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
               v[ipoint] = (Float_t(oscReader.Voltage(ich)[ipoint]) - 32767.5) / 65536.;
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
         Int_t ientry = 0;
         bool res = true;
         while (res) {
            res = readDRS4event();
            if (res) {
               ++ientry;
               if (ientry >= entry1) tree->Fill();
               if (entry2 > 0 && ientry >= entry2) break;
            }
         }
      }
      catch (std::ifstream::failure e) {
         //cout<< "read error: " << e.what() <<endl;
         cout<< "DRS4bin::Convert: exception caught: read error at event " << tree->GetEntries() <<endl;
      }

      cout<< "write " << tree->GetEntries() << " entries into file " << ofile->GetName() <<endl;
      ofile->Write();

      new TCanvas;
      tree->Draw("v1:t","");
   }

   //-- readDRS4event()
   bool readDRS4event()
   {
      Long64_t pos = ifile.tellg();
      if (pos < 0) return false;

      if (pos == ifsize) return false;

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
      if (false
         or (header.title[0] != 'E')
         or (header.title[1] != 'H')
         or (header.title[2] != 'D')
         or (header.title[3] != 'R')
         )
      {
         // this is not an Event Header
         ifile.seekg(pos);
         // cout<< "this is not an Event Header: ";
         // cout.write(header.title,4);
         // cout<<endl;
         return false;
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
      bool res = true;
      while (res) {
         res = readDRS4channel();
      }

      return true;
   }

   //-- readDRS4channel
   bool readDRS4channel()
   {
      Long64_t pos = ifile.tellg();
      if (pos < 0) return false;

      if (pos == ifsize) {return false;}

      char header[4];

      ifile.read(header, sizeof(header));

      Float_t* v = 0;
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
         return false;
      }

      union {
         Char_t byte[2048];
         UShort_t word2[1024];
      } voltage;

      ifile.read(voltage.byte, sizeof(voltage.byte));
      for (int ipoint=0; ipoint<1024; ++ipoint) {
         v[ipoint] = (Float_t(voltage.word2[ipoint]) - 32767.5) / 65536.;
      }

      return true;
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

//void drs4bin(const char* ifname="data_meander.bin", Int_t entry1=0, Int_t entry2=0)
void oscbin(const char* ifname, Int_t entry1=0, Int_t entry2=0)
{
   DRS4bin* drs4bin = new DRS4bin(ifname);
   drs4bin->Convert2root(entry1,entry2);
}
