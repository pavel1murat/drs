///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
#include "obj/TVme1742DataBlock.hh"

ClassImp(TVme1742DataBlock)
//______________________________________________________________________________
void TVme1742DataBlock::Streamer(TBuffer &R__b) {

  // Stream an object of class TVme1742DataBlock - this is the only 
  // non-trivial method of this class
  // current version: V2

  if (R__b.IsReading()) {
    Version_t R__v = R__b.ReadVersion(); if (R__v) { }
    R__b.ReadFastArray((UInt_t*) fEventHeader,4);

    for (int ig=0; ig<kMaxNGroups; ig++) {
      if (GroupPresent(ig)) {
	R__b >> fGroupHeader[ig];
	R__b.ReadFastArray(fGroupData[ig][0],8*kNCells);
	if (TriggerReadout(ig) != 0) {
	  R__b.ReadFastArray(fTriggerData[ig],kNCells);
	}

	if (R__v == 1) {
	  fGroupTriggerTimeTag[ig] = 0;
	}
	else {
					       // in V2 added fGroupTriggerTimeTag
	  R__b >> fGroupTriggerTimeTag[ig];
	}
      }
    }
  } 
  else {
    R__b.WriteVersion(TVme1742DataBlock::IsA());
    R__b.WriteFastArray((UInt_t*) fEventHeader,4);
    
    for (int ig=0; ig<kMaxNGroups; ig++) {
      if (GroupPresent(ig)) {
	R__b << fGroupHeader[ig];
	R__b.WriteFastArray(fGroupData[ig][0],8*kNCells);
	if (TriggerReadout(ig) != 0) {
	  R__b.WriteFastArray(fTriggerData[ig],kNCells);
	}
	R__b << fGroupTriggerTimeTag[ig];
      }
    }
  }
}


//_____________________________________________________________________________
TVme1742DataBlock::TVme1742DataBlock() {
}

//_____________________________________________________________________________
TVme1742DataBlock::~TVme1742DataBlock() {
}

//_____________________________________________________________________________
void  TVme1742DataBlock::Clear(Option_t* opt)  {

  memset(fEventHeader        ,0,4*sizeof(UInt_t));
  memset(fGroupHeader        ,0,kMaxNGroups          *sizeof(UInt_t  ));
  memset(fGroupData          ,0,kMaxNGroups*8*kNCells*sizeof(UShort_t));
  memset(fTriggerData        ,0,kMaxNGroups*kNCells  *sizeof(UShort_t));
  memset(fGroupTriggerTimeTag,0,kMaxNGroups          *sizeof(UInt_t  ));
}


//_____________________________________________________________________________
void  TVme1742DataBlock::Print(Option_t* opt) const {
}

  


