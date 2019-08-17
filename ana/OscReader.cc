///////////////////////////////////////////////////////////////////////////////

#include "ana/OscReader.hh"

ClassImp(OscReader)

//-----------------------------------------------------------------------------
OscReader::OscReader() {
  fStatus = false;
}


//-----------------------------------------------------------------------------
OscReader::~OscReader() {
  if (ifile.is_open()) ifile.close();
}

//-----------------------------------------------------------------------------
bool OscReader::Open(const char* ifname_0) {
  // use like:
  //
  // OscReader oscReader("osc.bin");
  // if (!oscReadout) {
  //    cout<< "Input file does not seem to be the DRS4 oscilloscope application binary file" <<endl;
  //    return;
  // }

  const Char_t header_event[] = {'E', 'H', 'D', 'R'};
  const Char_t header_channel[][4] = {
    {'C', '0', '0', '1'},
    {'C', '0', '0', '2'},
    {'C', '0', '0', '3'},
    {'C', '0', '0', '4'}
  };

  ifname = ifname_0;
  fStatus = true;
  
  // try to open input file as the oscilloscope application binary file
  ifile.open(ifname.c_str(), std::ios::binary);
  if (!ifile) {
    cout<< "File not found: " << ifname <<endl;
    fStatus = false;
    return false;
  }
  cout<< "input file " << ifname <<endl;
  
  // file size
  ifile.seekg(0, std::ios::end);
  ifsize = ifile.tellg();
  ifile.seekg(0);
  
  // series of checks
  if (ifsize < 0) {
    cout<< "OscReader: input file error: ifsize = " << ifsize <<endl;
    ifile.close();
    fStatus = false;
    return false;
  }
  Long64_t sizeof_time_channel1 = sizeof(OscStructUnion::OscStruct::EventTime) + sizeof(OscStructUnion::OscStruct::Channel);
  if (ifsize < sizeof_time_channel1 ) {
    cout<< "OscReader: input file " << ifname << " is too short to contain even single DRS4 oscilloscope event" <<endl;
    ifile.close();
    fStatus = false;
    return false;
  }
  
  // read the first event to figure out the number of channels
  // read the time and the first channel
  ifile.read(oscStructUnion.buffer, sizeof(OscStructUnion::OscStruct::EventTime));
  
  // check the event header
  //cout.write(oscStructUnion.oscStruct.time.field.header, 4); cout<<endl;
  //cout<< "event header check: " << std::strncmp(oscStructUnion.oscStruct.time.field.header, header_event, 4) <<endl;
  if (std::strncmp(oscStructUnion.oscStruct.time.field.header, header_event, 4) != 0) {
    fStatus = false;
    cout<< "OscReader: No event header found, the input file " << ifname << " does not seem to be the DRS4 oscilloscope application binary file" <<endl;
    return false;
  }
  
  // read channels to figure out how many them and their numbers

  for (int i=0; i<4; i++) usedchan[i] = 0;
  
  nchan = 0;
  for (int nch=0; nch<4; ++nch) {
    // will the channel fit in the file?
    if (ifsize - ifile.tellg() < Long64_t(sizeof(OscStructUnion::OscStruct::Channel))) break;
    ifile.read(oscStructUnion.oscStruct.channel[nch].buffer, sizeof(OscStructUnion::OscStruct::Channel));
    //cout.write(oscStructUnion.oscStruct.channel[nch].field.header, 4); cout<<endl;
    //cout.write(header_channel[nch], 4); cout<<endl;
    //cout<< "channel header check: " << 
    // std::strncmp(oscStructUnion.oscStruct.channel[nch].field.header, header_channel[nch], 4) <<endl;
    
    Int_t channel = 0;
    for (int ich=0; ich<4; ++ich) {
      if (std::strncmp(oscStructUnion.oscStruct.channel[nch].field.header, header_channel[ich], 4)==0) {
	channel         = ich+1;
	usedchan[nchan] = channel;
	nchan           = nchan+1;
	cout<< "OscReader::Open: channel = " << channel << " usedchan[" << nchan << "] = " << usedchan[nchan] <<endl;
      }
    }

    if (channel == 0) {
      // this is not a channel header
      break;
    }
  }

  if (nchan == 0) {
    fStatus = false;
    cout<< "OscReader: No channels found, the input file " << ifname << " does not seem to be the DRS4 oscilloscope application binary file" <<endl;
    return false;
  }
  
  // return to the beginning of the file
  ifile.seekg(0);
  
  cout<< "OscReader: oscilloscope event contains " << nchan << " channels" <<endl;
  return true;
}



//-----------------------------------------------------------------------------
bool OscReader::SetEventPointer(Int_t record) {
  // set file get pointer to read record record (counting from 0)
  if (!fStatus) return false;
  Long64_t record_size = sizeof(OscStructUnion::OscStruct::EventTime) + nchan*sizeof(OscStructUnion::OscStruct::Channel);
  if (record_size*(record+1) <= ifsize) ifile.seekg(record*record_size);
  else {
    cout<< "File " << ifname << " has no event " << record <<endl;
    return false;
  }
  // cout<< "OscReader::SetEventPointer: ifile.tellg() = " << ifile.tellg() <<endl;
  return true;
}


//-----------------------------------------------------------------------------
Long64_t OscReader::GetEventPointer() {
  // set file get pointer to read record record (counting from 0)
  if (!fStatus) return false;
  Long64_t pos = ifile.tellg();
  // cout<< "OscReader::GetEventPointer: ifile.tellg() = " << pos <<endl;
  return pos;
}


//-----------------------------------------------------------------------------
bool OscReader::ReadEvent() {
  // reads current event

  if (!fStatus) return false;
  Long64_t record_size = sizeof(OscStructUnion::OscStruct::EventTime) + nchan*sizeof(OscStructUnion::OscStruct::Channel);
  Long64_t curr = ifile.tellg();
  if (curr < 0) {
    cout<< "OscReader::ReadEvent: read error: curr = " << curr <<endl;
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
      cout<< "OscReader::ReadEvent: corrupted event header" <<endl;
      return false;
    }
    // channel headers

    int ich;
    for (int i=0; i<nchan; ++i) {
      ich = usedchan[i]-1;
      if (std::strncmp(oscStructUnion.oscStruct.channel[i].field.header, header_channel[ich], 4) != 0) {
	cout<< "OscReader::ReadNext: corrupted header for ich = " << ich <<endl;
	return false;
      }
    }
  }
  return true;
}
