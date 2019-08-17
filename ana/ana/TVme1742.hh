#ifndef TVme1742_hh
#define TVme1742_hh

#include "TObject.h"
#include "TTree.h"

#include "Stntuple/obj/TStnEvent.hh"
#include "Stntuple/obj/TStnHeaderBlock.hh"
#include "drs/obj/TVme1742DataBlock.hh"


class TVme1742: public TObject {

public:
  enum { kMaxGroups = 4 };

  UInt_t   fEventHeader[4];                    // event header - 4 words
  UInt_t   fGroupHeader[kMaxGroups];
  UShort_t fGroupData  [kMaxGroups][8][1024];
  UShort_t fTriggerData[kMaxGroups][1024];     // not necessarily present
  UInt_t   fGroupTriggerTimeTag[kMaxGroups];  // added in V2

  int      fCompressionLevel;
  int      fBufferSize;
  int      fRunNumber;

  TStnHeaderBlock*   fHeaderBlock;
  TVme1742DataBlock* fVme1742DataBlock;

  static TStnEvent*  fgEvent;
  static TTree*      fgTree ;
  static TVme1742*   fgInstance ;

  class  Cleaner {
  public: 
    Cleaner ();
    ~Cleaner();
  };
  friend class Cleaner;

//-----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------
  TVme1742 ();
  ~TVme1742();

  int ReadGroupData    (FILE* InputFile, Int_t IGroup);
  int ConvertToStntuple(const char* InputFile, const char* OutputFile, int RunNumber = 1);

  TStnDataBlock* AddDataBlock(const char* branch_name,
			      const char* class_name ,
			      Int_t       (*f)(TStnDataBlock*,AbsEvent*,Int_t),
			      Int_t       buffer_size,
			      Int_t       split      ,
			      Int_t       compression);

  int       EventNumber() { return  fEventHeader[2] & 0x003fffff; }
  int       RunNumber  () { return  fRunNumber; }

  UInt_t*   EventHeader() { return fEventHeader; }
  UInt_t*   GroupHeader() { return fGroupHeader; }

  UInt_t*   GroupTriggerTimeTag() { return fGroupTriggerTimeTag; }

  int       GroupPresent  (Int_t I) { return (fEventHeader[1] >> I ) & 0x1; }
  int       TriggerReadout(Int_t I) { return (fGroupHeader[I] >> 12) & 0x1; }

  UShort_t* EventData  (Int_t I) { return &fGroupData  [I][0][0] ; }
  UShort_t* TriggerData(Int_t I) { return &fTriggerData[I][0] ; }
//-----------------------------------------------------------------------------
// modifiers
//-----------------------------------------------------------------------------
  void      SetRunNumber(int RunNumber) {
    fRunNumber = RunNumber;
  }

  static TVme1742* Instance();

  static Int_t InitHeaderBlock     (TStnDataBlock* block, AbsEvent* event, int Mode);
  static Int_t InitVme1742DataBlock(TStnDataBlock* block, AbsEvent* event, int Mode);

    
  ClassDef (TVme1742,0)
};



#endif
