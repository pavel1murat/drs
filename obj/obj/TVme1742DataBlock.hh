#ifndef TVme1742DataBlock_hh
#define TVme1742DataBlock_hh

#include "Stntuple/obj/TStnDataBlock.hh"

class AbsEvent;
class TVme1742;

class TVme1742DataBlock: public TStnDataBlock {
public:
  enum { kNCells     = 1024 };
  enum { kMaxNGroups =    4 };
  
//   friend Int_t TVme1742::InitVme1742DataBlock(TStnDataBlock* block, 
// 					      AbsEvent*      event, 
// 					      int            mode);

protected:

  UInt_t      fEventHeader[4];
  UInt_t      fGroupHeader[kMaxNGroups];
  UShort_t    fGroupData  [kMaxNGroups][8][kNCells]; // a group has 8 channels...
  UShort_t    fTriggerData[kMaxNGroups][kNCells];    // and , may be, the trigger data
  UInt_t      fGroupTriggerTimeTag[kMaxNGroups];     // added in V2
//-----------------------------------------------------------------------------
//  functions
//-----------------------------------------------------------------------------
public:
					// ****** constructors and destructor
  TVme1742DataBlock();
  virtual ~TVme1742DataBlock();
					// ****** accessors

  Int_t     GetEventHeader(int I) { return fEventHeader[I]; }
  Int_t     GetGroupHeader(int I) { return fGroupHeader[I]; }

  Int_t     GetMaxNGroups() { return kMaxNGroups; }

  Int_t     GetNChannelsPerGroup() { return 8 ;}

					// 2012-10-24: to be fixed, make sure the default - VME-1742 - is correct
//   Int_t     GetEvalBoard() { 
//     //    Warning("GetEvalBoard","TO BE WRITTEN"); 
//     return 0; 
//   }

  UInt_t*   EventHeader() { return fEventHeader; }
  UInt_t*   GroupHeader() { return fGroupHeader; }

  UInt_t*   GroupTriggerTimeTag() { return fGroupTriggerTimeTag; }

  Int_t     GroupPresent  (Int_t I) { return (fEventHeader[1] >> I ) & 0x1; }
  Int_t     IsEvalBoard   ()        { return (((fEventHeader[1] >> 4 ) & 0xf) == 0xf); }

  Int_t     TriggerReadout(Int_t Igr) { return (fGroupHeader[Igr] >> 12) & 0x1  ;}
  Int_t     FirstCell     (Int_t Igr) { return (fGroupHeader[Igr] >> 20) & 0x3ff;}

  UShort_t* GroupData  (Int_t I) { return &fGroupData  [I][0][0] ; }
  UShort_t* TriggerData(Int_t I) { return &fTriggerData[I][0] ; }

  UShort_t  GroupData  (Int_t Igr, Int_t Ich, Int_t Cell) { 
    return fGroupData  [Igr][Ich][Cell] ; 
  }

  UShort_t  TriggerData(Int_t Igr, Int_t Cell) { 
    return fTriggerData[Igr][Cell] ; 
  }

//-----------------------------------------------------------------------------
// modifiers
//-----------------------------------------------------------------------------
					// use free space to save the run number
					// not implemented yet
  void    SetRunNumber(int RunNumber) { 
  }

  void    SetEventHeader(UInt_t* Data) { 
    memcpy(fEventHeader,Data,4*sizeof(UInt_t));
  }

  void    SetGroupHeader(UInt_t* Data) { 
    memcpy(fGroupHeader,Data,kMaxNGroups*sizeof(UInt_t));
  }
					// set bits 4-7 to 1 in case of evaluation board

  void    SetEvalBoardCode() { fEventHeader[1] |= 0x000000f0; }

  void    SetGroupTriggerTimeTag(UInt_t* Data) { 
    memcpy(fGroupTriggerTimeTag,Data,kMaxNGroups*sizeof(UInt_t));
  }

  void    SetGroupData(Int_t I, UShort_t* Data) { 
    memcpy(fGroupData[I][0],Data,8*1024*sizeof(UShort_t));
  }

  void    SetCellData(int Group,Int_t Channel,Int_t I, UShort_t Data) { 
    fGroupData[Group][Channel][I] = Data;
  }

  void    SetTriggerData(Int_t I, UShort_t* Data) { 
    memcpy(fTriggerData[I],Data,1024*sizeof(UShort_t));
  }
//-----------------------------------------------------------------------------
// overloaded methods of TObject
//-----------------------------------------------------------------------------
  void    Clear(Option_t* opt = "");
  void    Print(Option_t* opt = "") const;

  ClassDef(TVme1742DataBlock,2)	// VME1742 data block
};

#endif
