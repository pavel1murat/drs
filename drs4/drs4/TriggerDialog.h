#ifndef __TriggerDialog__
#define __TriggerDialog__

/*
$Id: TriggerDialog.h,v 1.1.1.1 2012/03/25 23:06:55 camac Exp $
*/

class DOFrame;

#include "DRSOsc.h"

/** Implementing TriggerDialog_fb */
class TriggerDialog : public TriggerDialog_fb
{
protected:
   // Handlers for TriggerDialog_fb events.
   void OnClose( wxCommandEvent& event );
   void OnButton( wxCommandEvent& event );
   
public:
   /** Constructor */
   TriggerDialog( wxWindow* parent );

private:
   DOFrame *m_frame;
};

#endif // __TriggerDialog__
