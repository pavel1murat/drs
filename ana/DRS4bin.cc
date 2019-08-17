///////////////////////////////////////////////////////////////////////////////
// 2011-10-25 P.Murat : write data out in mV
//-----------------------------------------------------------------------------
#include "ana/DRS4bin.hh"

ClassImp(DRS4bin)

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
DRS4bin::DRS4bin(const char* Filename) {

  status    = false;
  ifname    = Filename;
  ofile     = 0;
  tree      = 0;
  oscReader = new OscReader();

  if (oscReader->Open(Filename)) {
    status = true;
    printf ("Successfully opened Osci binary file %s\n",Filename);
    return;
  }
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
DRS4bin::~DRS4bin() {
  delete oscReader;
}



//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
TGraph* DRS4bin::plot(Int_t Channel) {
  if (!usedchan[Channel]) return 0;
  const Int_t color[] = {2, 4, 6, 8};
  
  Float_t* v = 0;
  if (Channel == 1) v = b1_c1;
  if (Channel == 2) v = b1_c2;
  if (Channel == 3) v = b1_c3;
  if (Channel == 4) v = b1_c4;

  TGraph* gr = new TGraph(1024,b1_t,v);

  gr->SetNameTitle(Form("gr_evt_%d_chan_%d",event,Channel), Form("gr_evt_%d_chan_%d",event,Channel));
  gr->SetMarkerStyle(6);
  gr->SetMarkerColor(color[Channel]);
  gr->Draw("ap");
  return gr;
}


//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void DRS4bin::Convert2root(Int_t entry1, Int_t entry2) {

  cout<< "Convert2root: entry1 = " << entry1 << " entry2 = " <<entry2 <<endl;

  for (int i=0; i<4; i++) {
    usedchan[i] = oscReader->UsedChan(i);
    printf("i=%i  usedchan[i]=%i\n", i, usedchan[i]);
  }

  printf("DRS4 channels: ");
  for (int i=0; i<4; i++) {
    int used = (usedchan[i] != 0);
    printf(" i+1 = %i used =  %i\n",i,used);
  }
  printf("\n");
  
  TString  output_fn = ifname.c_str();
  output_fn.Replace(output_fn.Index(".osc"),4,".root");
  
  ofile = TFile::Open(output_fn.Data(),"recreate");

  tree = new TTree("pulse", "produced from DRS4 binary file");

  tree->SetMarkerStyle(6);
  tree->SetMarkerColor(46);
  tree->SetLineColor  (46);

 
  tree->Branch("event"   , &event   , "event/I"     );
  tree->Branch("tc1"     , &tc1     , "tc1/I"       );
  tree->Branch("b1_t"    , &b1_t    , "b1_t[1024]/F");

  // P.Murat: so far assume 4 channels at most..., but...
  tree->Branch("nch"     , &fNChannels, "nch/I");
  tree->Branch("usedchan", &usedchan  , "usedchan[4]/I");

  // book used channels

  int       ich;
  char      branch_name[100], format[100];
  float**   branch;

  for (int i=0; i<4; i++) {
    ich = usedchan[i];
    if (ich > 0) {

      if (usedchan[i] == 1) branch = (float**) &b1_c1;
      if (usedchan[i] == 2) branch = (float**) &b1_c2;
      if (usedchan[i] == 3) branch = (float**) &b1_c3;
      if (usedchan[i] == 4) branch = (float**) &b1_c4;

      sprintf(branch_name,"b1_c%i",ich);
      sprintf(format,"%s[1024]/F",branch_name);
      tree->Branch(branch_name, branch, format);
    }
  }

  // // rest of the event
  // tree->Branch("year"       , &year       , "year/I");
  // tree->Branch("month"      , &month      , "month/I");
  // tree->Branch("day"        , &day        , "day/I");
  // tree->Branch("hour"       , &hour       , "hour/I");
  // tree->Branch("minute"     , &minute     , "minute/I");
  // tree->Branch("second"     , &second     , "second/I");
  // tree->Branch("millisecond", &millisecond, "millisecond/I");

  Int_t ientry = entry1;
  oscReader->SetEventPointer(ientry);

  // cout<< "Conver2root: oscReader->GetEventPointer() = " << oscReader->GetEventPointer() <<endl;

  // cout<< "--> start reading" <<endl;

  while (oscReader->ReadEvent()) {
    cout<< "processing ientry " << ientry << " oscReader->Number = " << oscReader->Number() <<endl;

    fNChannels = oscReader->Nchan();
    for (int i=0; i<fNChannels; i++) {
      usedchan[i] = oscReader->UsedChan(i);
    }

    // event       = oscReader->Number();
    // year        = oscReader->Year();
    // day         = oscReader->Day();
    // hour        = oscReader->Hour();
    // minute      = oscReader->Minute();
    // second      = oscReader->Second();
    // millisecond = oscReader->Millisecond();

    for (int ipoint=0; ipoint<1024; ++ipoint) {
      b1_t[ipoint] = oscReader->Time()[ipoint];
    }
					// figure out which branch corresponds to channel number 'i'
					// this is a quite an inefficient way

    for (unsigned ich=0; ich<fNChannels; ich++) {
      Float_t* v = 0;
      Int_t channel = oscReader->UsedChan(ich);
      if (channel > 0) {
	if (channel == 1) v = b1_c1;
	if (channel == 2) v = b1_c2;
	if (channel == 3) v = b1_c3;
	if (channel == 4) v = b1_c4;
      }
      if (!v) {
	cout<< "something wrong: v == 0" <<endl;
	return;
      }
      for (int ipoint=0; ipoint<1024; ++ipoint) {
	// convert data to mV
	v[ipoint] = (Float_t(oscReader->Voltage(ich)[ipoint]) - 32767.5) / 65536. * 1000;
      }
    }

    tree->Fill();
    if (entry2 > 0 && ientry >= entry2) break;
    ++ientry;
  }

  cout<< "write " << tree->GetEntries() << " entries into file " << ofile->GetName() <<endl;
  ofile->Write();

  new TCanvas;
  tree->Draw("b1_c1:b1_t","");
}

//-----------------------------------------------------------------------------
//-- readDRS4channel
//-----------------------------------------------------------------------------
bool DRS4bin::readDRS4channel() {

  char header[4];
  Long64_t pos = ifile.tellg();
  if (pos < 0) return false;
  
  if (pos == ifsize) {return false;}
  
  ifile.read(header, sizeof(header));

  Float_t* v = 0;
  switch (header[3]) {
  case '1': v = b1_c1; usedchan[0] = kTRUE; break;
  case '2': v = b1_c2; usedchan[1] = kTRUE; break;
  case '3': v = b1_c3; usedchan[2] = kTRUE; break;
  case '4': v = b1_c4; usedchan[3] = kTRUE; break;
  }
  
  if (v == 0 or (header[0] != 'C') or (header[1] != '0') or (header[2] != '0')) {
    // this is not a channel header
    ifile.seekg(pos);    // return to previous position
    // cout<< "this is not a Channel Header: ";
    // cout.write(header,4);
    // cout<<endl;
    return false;
  }

  union {
    Char_t   byte [2048];
    UShort_t word2[1024];
  } voltage;

  ifile.read(voltage.byte, sizeof(voltage.byte));
  for (int ipoint=0; ipoint<1024; ++ipoint) {
    // convert data into mV
    v[ipoint] = (Float_t(voltage.word2[ipoint]) - 32767.5) / 65536. * 1000;
  }

  return true;
}

//-----------------------------------------------------------------------------
// readDRS4event()
//-----------------------------------------------------------------------------
bool DRS4bin::readDRS4event() {

  Long64_t pos = ifile.tellg();
  if (pos < 0) return false;

  if (pos == ifsize) return false;

  union {
    Char_t buffer[24];
    struct {
      char title[4];
      UInt_t   event;        // serial number starting from 1
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

  event       = header.event;
  // year        = header.year;
  // month       = header.month;
  // day         = header.day;
  // hour        = header.hour;
  // minute      = header.minute;
  // second      = header.second;
  // millisecond = header.millisecond;

  // read time bins

  char* t_buffer = (char*) b1_t;
  ifile.read(t_buffer, 1024*4);
  
  // read channels

  bool res = true;
  while (res) {
    res = readDRS4channel();
  }

  return true;
}
