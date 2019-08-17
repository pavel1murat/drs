// struct OsciTime {
//    Char_t header[4];
//    UInt_t number;
//    UShort_t year;
//    UShort_t month;
//    UShort_t day;
//    UShort_t hour;
//    UShort_t minute;
//    UShort_t second;
//    UShort_t millisecond;
//    UShort_t reserved;
//    Float_t t[1024];
// };
// 
// struct OsciChannel {
//    Char_t header[4];
//    UShort_t voltage[1024];
// };
// 
// struct OsciEvent {
//    OsciTime time;
//    OsciChannel channel[4];
// };
// 
// union OsciRecord {
//    OsciStruct osciStruct;
//    Char_t buffer[sizeof(OsciTime) + 4*sizeof(OsciChannel)];
// };
// 
// class OsciBin {
// private:
//    std::string ifname;     // to use input file name for messages
//    std::ifstream ifile;
//    OsciRecord osciRecord;
// public:
//    // getters
//    UInt_t Number() const {return oscStructUnion.oscStruct.time.field.number;}
//    UInt_t Year() const {return oscStructUnion.oscStruct.time.field.year;}
//    UInt_t Day() const {return oscStructUnion.oscStruct.time.field.day;}
//    UInt_t Hour() const {return oscStructUnion.oscStruct.time.field.hour;}
//    UInt_t Minute() const {return oscStructUnion.oscStruct.time.field.minute;}
//    UInt_t Second() const {return oscStructUnion.oscStruct.time.field.second;}
//    UInt_t Millisecond() const {return oscStructUnion.oscStruct.time.field.millisecond;}
//    const Float_t* Time() const {return oscStructUnion.oscStruct.time.field.t;}
//    const UShort_t* Voltage(Int_t ich) const {
//       if (ich > 3) return 0;
//       return oscStructUnion.oscStruct.channel[ich].field.voltage;
//    }
//    UInt_t Nchan() const {return nchan;}
//    const Int_t* UsedChan() const {return usedchan;}
// 
//    // vars
//    bool status;
//    bool operator !() const {return status;}
//    Long64_t ifsize;
//    Int_t nchan;
//    Int_t usedchan[4];
// 
//    OsciBin(): status(false) {}
//    bool Open(const char* ifname_0)
//    {
//       // use like:
//       //
//       // OsciBin oscReader("osc.bin");
//       // if (!oscReadout) {
//       //    cout<< "Input file does not seem to be the DRS4 oscilloscope application binary file" <<endl;
//       //    return;
//       // }
// 
//       ifname = ifname_0;
//       status = true;
// 
//       // try to open input file as the oscilloscope application binary file
//       ifile.open(ifname.c_str(), std::ios::binary);
//       if (!ifile) {
//          cout<< "File not found: " << ifname <<endl;
//          status = false;
//          return false;
//       }
//       cout<< "processing file " << ifname <<endl;
// 
//       // file size
//       ifile.seekg(0, std::ios::end);
//       ifsize = ifile.tellg();
//       ifile.seekg(0);
// 
//       // series of checks
//       if (ifsize < 0) {
//          cout<< "OsciBin: input file error: ifsize = " << ifsize <<endl;
//          ifile.close();
//          status = false;
//          return false;
//       }
//       Long64_t sizeof_time_channel1 = sizeof(OscStructUnion::OscStruct::EventTime) + sizeof(OscStructUnion::OscStruct::Channel);
//       if (ifsize < sizeof_time_channel1 ) {
//          cout<< "OsciBin: input file " << ifname << " is too short to contain even single DRS4 oscilloscope event" <<endl;
//          ifile.close();
//          status = false;
//          return false;
//       }
// 
//       // read the first event to figure out the number of channels
// 
//       const Char_t header_event[] = {'E', 'H', 'D', 'R'};
//       const Char_t header_channel[][4] = {
//          {'C', '0', '0', '1'},
//          {'C', '0', '0', '2'},
//          {'C', '0', '0', '3'},
//          {'C', '0', '0', '4'}
//       };
// 
//       // read the time and the first channel
//       //ifile.read(oscStructUnion.buffer, sizeof_time_channel1);
//       ifile.read(oscStructUnion.buffer, sizeof(OscStructUnion::OscStruct::EventTime));
// 
//       // check the event header
//       //cout.write(oscStructUnion.oscStruct.time.field.header, 4); cout<<endl;
//       //cout<< "event header check: " << std::strncmp(oscStructUnion.oscStruct.time.field.header, header_event, 4) <<endl;
//       if (std::strncmp(oscStructUnion.oscStruct.time.field.header, header_event, 4) != 0) {
//          status = false;
//          cout<< "OsciBin: No event header found, the input file " << ifname << " does not seem to be the DRS4 oscilloscope application binary file" <<endl;
//          return false;
//       }
// 
//       // read channels to figure out how many them and their numbers
//       for (nchan=0; nchan<4; ++nchan) {
//          // will the channel fit in the file?
//          if (ifsize - ifile.tellg() < Long64_t(sizeof(OscStructUnion::OscStruct::Channel))) break;
//          ifile.read(oscStructUnion.oscStruct.channel[nchan].buffer, sizeof(OscStructUnion::OscStruct::Channel));
//          //cout.write(oscStructUnion.oscStruct.channel[nchan].field.header, 4); cout<<endl;
//          //cout.write(header_channel[nchan], 4); cout<<endl;
//          //cout<< "channel header check: " << std::strncmp(oscStructUnion.oscStruct.channel[nchan].field.header, header_channel[nchan], 4) <<endl;
// 
//          Int_t channel = 0;
//          for (int ich=0; ich<4; ++ich) {
//             if (std::strncmp(oscStructUnion.oscStruct.channel[nchan].field.header, header_channel[ich], 4)==0) {
//                channel = ich+1;
//                usedchan[ich] = channel;
//                cout<< "OsciBin::Open: channel = " << channel << " usedchan[" << ich << "] = " << usedchan[ich] <<endl;
//             }
//          }
//          if (channel == 0) {
//             // this is not a channel header
//             break;
//          }
//       }
//       if (nchan == 0) {
//          status = false;
//          cout<< "OsciBin: No channels found, the input file " << ifname << " does not seem to be the DRS4 oscilloscope application binary file" <<endl;
//          return false;
//       }
// 
//       // return to the beginning of the file
//       ifile.seekg(0);
// 
//       cout<< "OsciBin: oscilloscope event contains " << nchan << " channels" <<endl;
//       return true;
//    }
//    bool SetEventPointer(Int_t record)
//    {
//       // set file get pointer to read record record (counting from 0)
//       if (!status) return false;
//       Long64_t record_size = sizeof(OscStructUnion::OscStruct::EventTime) + nchan*sizeof(OscStructUnion::OscStruct::Channel);
//       if (record_size*(record+1) <= ifsize) ifile.seekg(record*record_size);
//       else {
//          cout<< "File " << ifname << " has no event " << record <<endl;
//          return false;
//       }
//       // cout<< "OsciBin::SetEventPointer: ifile.tellg() = " << ifile.tellg() <<endl;
//       return true;
//    }
//    Long64_t GetEventPointer()
//    {
//       // set file get pointer to read record record (counting from 0)
//       if (!status) return false;
//       Long64_t pos = ifile.tellg();
//       // cout<< "OsciBin::GetEventPointer: ifile.tellg() = " << pos <<endl;
//       return pos;
//    }
//    bool ReadEvent()
//    {
//       // reads current event
// 
//       if (!status) return false;
//       Long64_t record_size = sizeof(OscStructUnion::OscStruct::EventTime) + nchan*sizeof(OscStructUnion::OscStruct::Channel);
//       Long64_t curr = ifile.tellg();
//       if (curr < 0) {
//          cout<< "OsciBin::ReadNext: read error: curr = " << curr <<endl;
//          return false;
//       }
//       if (ifsize < curr+record_size) return false;
//       else {
//          ifile.read(oscStructUnion.buffer, record_size);
//          // check data consistency
//          const Char_t header_event[] = {'E', 'H', 'D', 'R'};
//          const Char_t header_channel[][4] = {
//             {'C', '0', '0', '1'},
//             {'C', '0', '0', '2'},
//             {'C', '0', '0', '3'},
//             {'C', '0', '0', '4'}
//          };
//          // event header
//          if (std::strncmp(oscStructUnion.oscStruct.time.field.header, header_event, 4) != 0) {
//             cout<< "OsciBin::ReadNext: corrupted event header" <<endl;
//             return false;
//          }
//          // channel headers
//          for (int ich=0; ich<nchan; ++ich) {
//             if (std::strncmp(oscStructUnion.oscStruct.channel[ich].field.header, header_channel[ich], 4) != 0) {
//                cout<< "OsciBin::ReadNext: corrupted header for ich = " << ich <<endl;
//                return false;
//             }
//          }
//       }
//       return true;
//    }
// };
