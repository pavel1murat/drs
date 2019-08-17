#ifndef __ConfigDialog__
#define __ConfigDialog__

// $Id: ConfigDialog.h,v 1.1.1.1 2012/03/25 23:06:55 camac Exp $

/**
@file
Subclass of ConfigDialog_fb, which is generated by wxFormBuilder.
*/

class DOFrame;
class Osci;

/** Implementing ConfigDialog_fb */
class ConfigDialog : public ConfigDialog_fb, DRSCallback
{
protected:
   // Handlers for ConfigDialog_fb events.
   void OnBoardSelect( wxCommandEvent& event );
   void OnRescan( wxCommandEvent& event );
   void OnInfo( wxCommandEvent& event );
   void OnChannelRange( wxCommandEvent& event );
   void OnInputRange( wxCommandEvent& event );
   void OnCalOn( wxCommandEvent& event );
   void OnCalEnter( wxCommandEvent& event );
   void OnCalSlider( wxScrollEvent& event );
   void OnClkOn( wxCommandEvent& event );
   void OnDateTime( wxCommandEvent& event );
   void OnShowGrid( wxCommandEvent& event );
   void OnDisplayWaveforms( wxCommandEvent& event );
   void OnButtonCalVolt( wxCommandEvent& event );
   void OnButtonSelect( wxCommandEvent& event );
   void UpdateCalVolt(int value);
   void OnButtonCalTime( wxCommandEvent& event );
   void OnRemoveSpikes( wxCommandEvent& event );
   void OnFreq( wxCommandEvent& event );
   void OnLock( wxCommandEvent& event );

   void OnClose( wxCommandEvent& event );
   
   int  fCalMode;

public:
   /** Constructor */
   ConfigDialog( wxWindow* parent );
   void Progress(int prog);
   void FreqChange();

private:
   DOFrame *m_frame;
   Osci    *m_osci;

   int      m_board, m_firstChannel, m_chnSection;

   void PopulateBoards(void);
   void EnableButtons(void);

};

#endif // __ConfigDialog__
