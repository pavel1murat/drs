//
#include <math.h>

#include "drs/ana/TVme1742.hh"
#include "Stntuple/obj/TStnNode.hh"
#include "Stntuple/obj/TStnEvent.hh"
#include "Stntuple/obj/TStnDataBlock.hh"
#include "Stntuple/obj/TStnHeaderBlock.hh"
#include "drs/obj/TVme1742DataBlock.hh"

#include "TTree.h"
#include "TFile.h"

TStnEvent*       TVme1742::fgEvent = 0;
TTree*           TVme1742::fgTree  = 0;

TVme1742*        TVme1742::fgInstance = 0;

//-----------------------------------------------------------------------------
TVme1742::TVme1742() {
  fgEvent           = new TStnEvent();
  fCompressionLevel = 1;
  fBufferSize       = 64000;
}

//-----------------------------------------------------------------------------
TVme1742* TVme1742::Instance() {
  static Cleaner cleaner;
  return (fgInstance) ? fgInstance : (fgInstance = new TVme1742());
}

//-----------------------------------------------------------------------------
TVme1742::~TVme1742() {
  delete fgEvent;
}

//------------------------------------------------------------------------------
TVme1742::Cleaner::Cleaner() {
}

//------------------------------------------------------------------------------
TVme1742::Cleaner::~Cleaner() {
  if (TVme1742::fgInstance) {
    delete TVme1742::fgInstance;
    TVme1742::fgInstance = 0;
  }
}


//-----------------------------------------------------------------------------
// clone of StntupleModule::AddDataBlock
//-----------------------------------------------------------------------------
TStnDataBlock* TVme1742::AddDataBlock(const char* branch_name,
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
Int_t TVme1742::InitHeaderBlock(TStnDataBlock* Block, AbsEvent* Event, int Mode) 
{
  // Run II version, section number is defined

  static TNamed*          processName      = NULL;

  TStnHeaderBlock* header = (TStnHeaderBlock*) Block;

  TVme1742* vme = TVme1742::Instance();

  header->fEventNumber   = vme->EventNumber();
  header->fRunNumber     = vme->RunNumber ();
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
Int_t TVme1742::InitVme1742DataBlock(TStnDataBlock* Block, 
				     AbsEvent*      Event, 
				     int            Mode) 
{
  TVme1742* vme = TVme1742::Instance();

  TVme1742DataBlock* data = (TVme1742DataBlock*) Block;
  data->Clear();

  // initialization is happening from the already read event,
  // fgEvent 

  data->SetEventHeader(vme->EventHeader());
  data->SetGroupHeader(vme->GroupHeader());
  for (int i=0; i<kMaxGroups; i++) {
    if (vme->GroupPresent(i)) {
      data->SetGroupData(i,vme->EventData(i));
      if (vme->TriggerReadout(i)) {
	data->SetTriggerData(i,vme->TriggerData(i));
      }
    }
  }

  data->SetGroupTriggerTimeTag(vme->GroupTriggerTimeTag());
  return 0;
}

//-----------------------------------------------------------------------------
// read group 'Igr'
//-----------------------------------------------------------------------------
int TVme1742::ReadGroupData(FILE* InputFile, Int_t Igr) {

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

  printf("group time tag: 0x%08x ", fGroupTriggerTimeTag[Igr]);
  printf("header: 0x%08x : n_samples=%4i read_trigger=%i freq=%i start_cell=%5i\n",
	 fGroupHeader[Igr],n_samples,read_trigger,freq,start_cell);

  // Move the file handle past the trigger time tag
  // fseek(InputFile, sizeof(unsigned int), SEEK_CUR);
  return rc;
}



//-----------------------------------------------------------------------------
int TVme1742::ConvertToStntuple(const char* InputFile, const char* OutputFile, int RunNumber) {
  int rc(0);

  FILE         *input_file;
  unsigned int eventHeader[4];
  unsigned int group_header;
  unsigned int n_samples;
  unsigned int readoutTR;
  unsigned int groupNum;
  unsigned int groupMask;
  TFile*       stntuple_file;

  AbsEvent*    event(0);

  int          ievent(0), nevents(0), present, nbytes;

  //  VmeEvent_t   event;


  // learn how to read the input file, start from Steve's code...

  input_file = fopen(InputFile, "r");


  // if (fgEvent != 0) delete fgEvent;

  stntuple_file = new TFile(OutputFile,"RECREATE");

  if (! stntuple_file) {
    Error("beginJob","an attempt to open a new ROOT file %s failed",OutputFile);
    rc = -1;
  }

  fgTree        = new TTree("STNTUPLE", "STNTUPLE");
  fRunNumber    = RunNumber;

  fHeaderBlock = (TStnHeaderBlock*) AddDataBlock("HeaderBlock",
						 "TStnHeaderBlock",
						 TVme1742::InitHeaderBlock,
						 fBufferSize,
						 99,                // fSplitMode.value(), always split
						 fCompressionLevel);

  //  SetResolveLinksMethod("HeaderBlock",StntupleHeaderBlockLinks);

  fVme1742DataBlock = (TVme1742DataBlock*) AddDataBlock("Vme1742DataBlock",
							"TVme1742DataBlock",
							TVme1742::InitVme1742DataBlock,
							fBufferSize,
							-1,		// don't split the data
							fCompressionLevel);
//-----------------------------------------------------------------------------
// read an event from the input file
//-----------------------------------------------------------------------------
  while (fread(fEventHeader, sizeof(unsigned int), 4, input_file)) {
    nbytes = (fEventHeader[0] & 0x0fffffff) * 4;
    printf("------------- ievent %5i nbytes = %5i event number: %8i\n",
	   ievent,nbytes, EventNumber());
    printf("0x%08x 0x%08x 0x%08x 0x%08x\n",
	   fEventHeader[0],
	   fEventHeader[1],
	   fEventHeader[2],
	   fEventHeader[3]);

    groupMask = fEventHeader[1] & 0x0000000f;
//-----------------------------------------------------------------------------
// read groups
//-----------------------------------------------------------------------------
    for(int ig=0; ig<4; ig++) {
      present = (groupMask >> ig) & 0x1;
      if ( present == 0) goto READ_NEXT_GROUP;
      ReadGroupData(input_file, ig);
    READ_NEXT_GROUP:;
    }
//-----------------------------------------------------------------------------
// at this point the event has been read in, time to write it out
//-----------------------------------------------------------------------------
    ievent++;
    nevents++;

    fgEvent->Init(event,0);

    fgTree->Fill();
  }

  fclose(input_file);

  stntuple_file->Write();
  delete stntuple_file;
  stntuple_file = 0;

  return rc;
}

