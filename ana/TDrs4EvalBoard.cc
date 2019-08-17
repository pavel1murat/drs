//
#include <math.h>

#include "drs/ana/TDrs4EvalBoard.hh"
#include "Stntuple/obj/TStnNode.hh"
#include "Stntuple/obj/TStnEvent.hh"
#include "Stntuple/obj/TStnDataBlock.hh"
#include "Stntuple/obj/TStnHeaderBlock.hh"
#include "drs/obj/TVme1742DataBlock.hh"

#include "TTree.h"
#include "TFile.h"

TStnEvent*       TDrs4EvalBoard::fgEvent = 0;
TTree*           TDrs4EvalBoard::fgTree  = 0;

TDrs4EvalBoard*        TDrs4EvalBoard::fgInstance = 0;

//-----------------------------------------------------------------------------
TDrs4EvalBoard::TDrs4EvalBoard() {
  fgEvent           = new TStnEvent();
  fCompressionLevel = 1;
  fBufferSize       = 64000;
  fOscReader        = new OscReader();
}

//-----------------------------------------------------------------------------
TDrs4EvalBoard* TDrs4EvalBoard::Instance() {
  static Cleaner cleaner;
  return (fgInstance) ? fgInstance : (fgInstance = new TDrs4EvalBoard());
}

//-----------------------------------------------------------------------------
TDrs4EvalBoard::~TDrs4EvalBoard() {
  delete fgEvent;
  delete fOscReader;
}


//------------------------------------------------------------------------------
TDrs4EvalBoard::Cleaner::Cleaner() {
}

//------------------------------------------------------------------------------
TDrs4EvalBoard::Cleaner::~Cleaner() {
  if (TDrs4EvalBoard::fgInstance) {
    delete TDrs4EvalBoard::fgInstance;
    TDrs4EvalBoard::fgInstance = 0;
  }
}


//-----------------------------------------------------------------------------
// clone of StntupleModule::AddDataBlock
//-----------------------------------------------------------------------------
TStnDataBlock* TDrs4EvalBoard::AddDataBlock(const char* branch_name,
				      const char* class_name,
				      Int_t       (*f)(TStnDataBlock*,AbsEvent*,Int_t),
				      Int_t       buffer_size,
				      Int_t       split,
				      Int_t       compression) 
{
  // adds new branch to fgTree and registers a data block corresponding to it

  TBranch*       branch;
  TStnDataBlock* block;

  int            rc;

  TStnNode*      node;
  node = 0;

  rc  = fgEvent->AddDataBlock(branch_name,class_name,node);

  if (rc == 0) {
				// everything is OK

    branch = fgTree->Branch(branch_name,class_name,
			    node->GetDataBlockAddress(),
			    buffer_size,
			    split);
    branch->SetCompressionLevel(compression);
    block = node->GetDataBlock();
    block->SetExternalInit(f);
    block->SetNode(node);
  }
  else if (rc > 0) {
				// already existing branch

    printf(" StntupleModule::AddDataBlock: ");
    printf(" an attempt to redefine the existing block made for");
    printf(" branch %s and class %s\n",branch_name,class_name);
    block = node->GetDataBlock();
  }
  else {
				// can't create branch

    printf(" StntupleModule::AddDataBlock : can\'t add block for");
    printf(" branch %s and class %s\n",branch_name,class_name);
    block = NULL;
  }
  return block;
}

//_____________________________________________________________________________
Int_t TDrs4EvalBoard::InitHeaderBlock(TStnDataBlock* Block, AbsEvent* Event, int Mode) 
{
  // Run II version, section number is defined

  static TNamed*          processName      = NULL;

  TStnHeaderBlock* header = (TStnHeaderBlock*) Block;

  TDrs4EvalBoard* drs4 = TDrs4EvalBoard::Instance();

  header->fEventNumber   = drs4->EventNumber();
  header->fRunNumber     = drs4->RunNumber  ();
  header->fSectionNumber =  1;
  header->fMcFlag        =  0;
  header->fNLCent        = -1;
  header->fNLPlug        = -1;
  header->fNLepton       = -1;
  header->fInstLum       = -1;

  header->fNTracks       = -1;
  header->fNCdfMuons     = -1;
  header->fNCdfEmObjects = -1;
  header->fNCdfTaus      = -1;
  header->fEOut          = TStnDataBlock::kUndefined;
  header->fPressure      = TStnDataBlock::kUndefined;
  header->fEmFraction    = TStnDataBlock::kUndefined;
  header->fChFraction    = TStnDataBlock::kUndefined;
  header->fNTowersOot    = -1;
  header->fNTracks       = -1;
  header->fNCdfMuons     = -1;
  header->fNCdfEmObjects = -1;
  header->fNCdfTaus      = -1;
  header->fEOut          = -1;
  header->fEmFraction    = -1;
  header->fChFraction    = -1;
  header->fNTowersOot    = -1;
  header->fInstLum       = -1;
  header->fPressure      = -1;
  header->fStnVersion    = "dev_701";

  return 0;
}

//-----------------------------------------------------------------------------
// initialize Vme1742 data block
//-----------------------------------------------------------------------------
Int_t TDrs4EvalBoard::InitVme1742DataBlock(TStnDataBlock* Block, 
					   AbsEvent*      Event, 
					   int            Mode) 
{
  TDrs4EvalBoard* drs4 = TDrs4EvalBoard::Instance();

  TVme1742DataBlock* data = (TVme1742DataBlock*) Block;

  data->Clear();

  drs4->fEventHeader[0] = 0xc0000000;
  drs4->fEventHeader[1] = 0x000000f1;  // only group 0 is present, plus eval board code = 0xf0
  drs4->fEventHeader[2] = 0x00000000;
  drs4->fEventHeader[3] = 0x00000000;
  data->SetEventHeader(drs4->fEventHeader);

  drs4->fGroupHeader[0] = 0x00000c00;
  for (int i=1; i<8; i++) drs4->fGroupHeader[i] = 0;
  data->SetGroupHeader(drs4->fGroupHeader);

  // initialization is happening from the already read event, fgEvent 

  int nchan, channel, used_channel[8]; 

  UShort_t v;

  nchan = drs4->fOscReader->Nchan();

  for (int i=0; i<nchan; i++) {
    used_channel[i] = drs4->fOscReader->UsedChan(i)-1;
  }

  memset(data->GroupData(0),0,8*1024*sizeof(UShort_t));

  for (int ich=0; ich<nchan; ich++) {
    channel = used_channel[ich]; // -1;
    if (channel >= 0) {
      for (int i=0; i<1024; i++) {
					// convert data to mV, shift by 1000.

	//	v = (UShort_t) ((Float_t(drs4->fOscReader->Voltage(ich)[i])-32767.5)/65536.*1000 +0.5 + 1000);
	v = drs4->fOscReader->Voltage(ich)[i];

					// the group is always zero... 
	data->SetCellData(0,channel,i,v); 
      }
    }
  }
//-----------------------------------------------------------------------------
// for DRS4 eval board there is no trigger data
//-----------------------------------------------------------------------------
  for (int i=0; i<4; i++) {
    if (drs4->GroupPresent(i)) {
      if (drs4->TriggerReadout(i)) {
	data->SetTriggerData(i,drs4->TriggerData(i));
      }
    }
  }

  data->SetGroupTriggerTimeTag(drs4->GroupTriggerTimeTag());
  return 0;
}

//-----------------------------------------------------------------------------
// read group 'Igr'
//-----------------------------------------------------------------------------
int TDrs4EvalBoard::ReadGroupData(FILE* InputFile, Int_t Igr) {

  int      rc(0);

  unsigned int evb[3*1024];
  unsigned int trb[3*1024/8];
  int          n_samples, read_trigger, freq, start_cell;

  unsigned int i, j, loc;

  fread(&fGroupHeader[Igr], sizeof(unsigned int), 1, InputFile);

  n_samples = (fGroupHeader[Igr] & 0xfff) / 3;

    // readoutTR = 1 if trigger is read out
  read_trigger = (fGroupHeader[Igr] >> 12) & 0x1;
  freq         = (fGroupHeader[Igr] >> 16) & 0x3;
  start_cell   = (fGroupHeader[Igr] >> 20) & 0x3ff;

  printf("group header: 0x%08x : n_samples=%4i read_trigger=%i freq=%i start_cell=%5i\n",
	 fGroupHeader[Igr],n_samples,read_trigger,freq,start_cell);


  fread(evb, sizeof(int), 3*n_samples, InputFile);
  for(i = 0; i<n_samples; i++) {
    loc = 3*i;
    fGroupData[Igr][0][i] = (evb[loc  ]      ) & 0xfff;
    fGroupData[Igr][1][i] = (evb[loc  ] >> 12) & 0xfff;
    fGroupData[Igr][2][i] = (evb[loc  ] >> 24) | ((evb[loc+1]&0x0f) << 8);
    fGroupData[Igr][3][i] = (evb[loc+1] >>  4) & 0xfff;
    fGroupData[Igr][4][i] = (evb[loc+1] >> 16) & 0xfff;
    fGroupData[Igr][5][i] = (evb[loc+1] >> 28) | ((evb[loc+2]&0xff) << 4);
    fGroupData[Igr][6][i] = (evb[loc+2] >>  8) & 0xfff;
    fGroupData[Igr][7][i] = (evb[loc+2] >> 20);
  }

  if (read_trigger != 0) {
//-----------------------------------------------------------------------------
// read trigger data in one go
//-----------------------------------------------------------------------------
    fread(trb, sizeof(unsigned int), 3*n_samples/8, InputFile);
    for(i = 0; i < n_samples/8; i++) {
      loc = 3*i;
      fTriggerData[Igr][i*8 + 0] = (trb[loc  ]      ) & 0xfff;
      fTriggerData[Igr][i*8 + 1] = (trb[loc  ] >> 12) & 0xfff;
      fTriggerData[Igr][i*8 + 2] = (trb[loc  ] >> 24) | ((trb[loc+1]&0x0f) << 8);
      fTriggerData[Igr][i*8 + 3] = (trb[loc+1] >>  4) & 0xfff;
      fTriggerData[Igr][i*8 + 4] = (trb[loc+1] >> 16) & 0xfff;
      fTriggerData[Igr][i*8 + 5] = (trb[loc+1] >> 28) | ((trb[loc+2]&0xff) << 4);
      fTriggerData[Igr][i*8 + 6] = (trb[loc+2] >>  8) & 0xfff;
      fTriggerData[Igr][i*8 + 7] = (trb[loc+2] >> 20);
    }
  }

  fread(&fGroupTriggerTimeTag[Igr], sizeof(unsigned int), 1, InputFile);

  // Move the file handle past the trigger time tag
  // fseek(InputFile, sizeof(unsigned int), SEEK_CUR);
  return rc;
}



//-----------------------------------------------------------------------------
// converts 'InputFile', written by drsosc, into STNTUPLE format
//-----------------------------------------------------------------------------
int TDrs4EvalBoard::ConvertToStntuple(const char* InputFile, const char* OutputFile, int RunNumber) {
  int rc(0);

  unsigned int n_samples;
  unsigned int readoutTR;
  unsigned int groupNum;
  unsigned int groupMask;
  TFile*       stntuple_file;
  AbsEvent*    event(0);

  int          ievent(0), present, nbytes;

  // open an input file

  if (fOscReader->Open(InputFile)) {
    printf ("Successfully opened Osci binary file %s\n",InputFile);
  }
  else {
    Error("ConvertToStntuple","failed to open OSC binary file %s, EXIT",InputFile);
    return -1;
  }

  stntuple_file = new TFile(OutputFile,"RECREATE");

  if (! stntuple_file) {
    Error("beginJob","an attempt to open a new ROOT file %s failed",OutputFile);
    rc = -1;
  }

  fgTree       = new TTree("STNTUPLE", "STNTUPLE");
//-----------------------------------------------------------------------------
// define data blocks
//-----------------------------------------------------------------------------
  fHeaderBlock = (TStnHeaderBlock*) AddDataBlock("HeaderBlock",
						 "TStnHeaderBlock",
						 TDrs4EvalBoard::InitHeaderBlock,
						 fBufferSize,
						 99,                // fSplitMode.value(), always split
						 fCompressionLevel);

  fVme1742DataBlock = (TVme1742DataBlock*) AddDataBlock("Vme1742DataBlock",
							"TVme1742DataBlock",
							TDrs4EvalBoard::InitVme1742DataBlock,
							fBufferSize,
							-1,		// don't split the data
							fCompressionLevel);
  fRunNumber = RunNumber;
//-----------------------------------------------------------------------------
// read input files
//-----------------------------------------------------------------------------
  Int_t ientry = 0;
  fOscReader->SetEventPointer(ientry);

  while (fOscReader->ReadEvent()) {
    cout << "processing ientry " << ientry << " fOscReader->Number = " << fOscReader->Number() <<endl;
    ientry++;

    fEventNumber = ientry;

    fgEvent->Init(event,0);

    fgTree->Fill();
  }

  stntuple_file->Write();
  delete stntuple_file;
  stntuple_file = 0;

  return rc;
}

