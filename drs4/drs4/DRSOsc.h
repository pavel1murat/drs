///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __DRSOsc__
#define __DRSOsc__

#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/panel.h>
#include <wx/slider.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/radiobut.h>
#include <wx/bmpbuttn.h>
#include <wx/stattext.h>
#include <wx/statbox.h>
#include <wx/tglbtn.h>
#include <wx/checkbox.h>
#include <wx/frame.h>
#include <wx/choice.h>
#include <wx/textctrl.h>
#include <wx/gauge.h>
#include <wx/dialog.h>
#include <wx/combobox.h>
#include <wx/statline.h>
#include <wx/statbmp.h>
#include <wx/hyperlink.h>

///////////////////////////////////////////////////////////////////////////

#define ID_CURSORA 1000
#define ID_CURSORB 1001
#define ID_TR_LEVEL 1002
#define ID_RUN 1003
#define ID_SINGLE 1004
#define ID_TR_NORMAL 1005
#define ID_TR_AUTO 1006
#define ID_TR_POLARITY 1007
#define ID_TRGCFG 1008
#define ID_TRG1 1009
#define ID_TRG2 1010
#define ID_TRG3 1011
#define ID_TRG4 1012
#define ID_TRG5 1013
#define ID_TR_DELAY 1014
#define ID_HSCALEDOWN 1015
#define ID_HSCALEUP 1016
#define ID_CHON1 1017
#define ID_POS1 1018
#define ID_SCALEUP1 1019
#define ID_SCALEDN1 1020
#define ID_CHON2 1021
#define ID_POS2 1022
#define ID_SCALEUP2 1023
#define ID_SCALEDN2 1024
#define ID_CHON3 1025
#define ID_POS3 1026
#define ID_SCALEUP3 1027
#define ID_SCALEDN3 1028
#define ID_CHON4 1029
#define ID_POS4 1030
#define ID_SCALEUP4 1031
#define ID_SCALEDN4 1032
#define ID_CONFIG 1033
#define ID_SAVE 1034
#define ID_MEASURE 1035
#define ID_PRINT 1036
#define ID_ABOUT 1037
#define ID_EXIT 1038
#define ID_1_4 1039
#define ID_5_8 1040
#define ID_9_12 1041
#define ID_13_16 1042
#define ID_LOWER 1043
#define ID_UPPER 1044
#define ID_COMBINED 1045
#define ID_R0 1046
#define ID_R045 1047
#define ID_R05 1048
#define ID_DISP_CALIBRATED 1049
#define ID_DISP_CALIBRATED2 1050
#define ID_DISP_ROTATED 1051
#define ID_DISP_TCALIBRATED 1052
#define ID_DISP_TRGCORR 1053
#define ID_REFCLK 1054
#define ID_DISPSAMPLE 1055
#define ID_DISPAVERAGE 1056
#define ID_DISPPERSIST 1057
#define ID_DISPNUMBER 1058
#define ID_FFT1 1059
#define ID_FFT2 1060
#define ID_FFT3 1061
#define ID_FFT4 1062
#define ID_PJ1 1063
#define ID_PJ2 1064
#define ID_PJ3 1065
#define ID_PJ4 1066
#define ID_LEVEL1 1067
#define ID_LEVEL2 1068
#define ID_LEVEL3 1069
#define ID_LEVEL4 1070
#define ID_PKPK1 1071
#define ID_PKPK2 1072
#define ID_PKPK3 1073
#define ID_PKPK4 1074
#define ID_RMS1 1075
#define ID_RMS2 1076
#define ID_RMS3 1077
#define ID_RMS4 1078
#define ID_FREQ1 1079
#define ID_FREQ2 1080
#define ID_FREQ3 1081
#define ID_FREQ4 1082
#define ID_PERIOD1 1083
#define ID_PERIOD2 1084
#define ID_PERIOD3 1085
#define ID_PERIOD4 1086
#define ID_RISE1 1087
#define ID_RISE2 1088
#define ID_RISE3 1089
#define ID_RISE4 1090
#define ID_FALL1 1091
#define ID_FALL2 1092
#define ID_FALL3 1093
#define ID_FALL4 1094
#define ID_POSWIDTH1 1095
#define ID_POSWIDTH2 1096
#define ID_POSWIDTH3 1097
#define ID_POSWIDTH4 1098
#define ID_NEGWIDTH1 1099
#define ID_NEGWIDTH2 1100
#define ID_NEGWIDTH3 1101
#define ID_NEGWIDTH4 1102
#define ID_CHNDELAY1 1103
#define ID_CHNDELAY2 1104
#define ID_CHNDELAY3 1105
#define ID_CHNDELAY4 1106
#define ID_OR1 1107
#define ID_OR2 1108
#define ID_OR3 1109
#define ID_OR4 1110
#define ID_OREXT 1111
#define ID_AND1 1112
#define ID_AND2 1113
#define ID_AND3 1114
#define ID_AND4 1115
#define ID_ANDEXT 1116

///////////////////////////////////////////////////////////////////////////////
/// Class DOFrame_fb
///////////////////////////////////////////////////////////////////////////////
class DOFrame_fb : public wxFrame 
{
	private:
	
	protected:
		wxMenuBar* m_menubar1;
		wxMenu* m_menu1;
		wxMenu* m_menu4;
		wxMenu* m_menu3;
		wxMenu* m_menu2;
		wxPanel* m_pnScreen;
		wxPanel* m_pnControls;
		wxSlider* m_slTrgLevel;
		wxButton* m_btRun;
		wxButton* m_btSingle;
		wxRadioButton* m_rbNormal;
		wxRadioButton* m_rbAuto;
		wxBitmapButton* m_bpPolarity;
		wxButton* m_btTrgCfg;
		wxRadioButton* m_rbS0;
		wxRadioButton* m_rbS1;
		wxRadioButton* m_rbS2;
		wxRadioButton* m_rbS3;
		wxRadioButton* m_rbS4;
		wxStaticText* m_staticText59;
		wxStaticText* m_staticText60;
		wxStaticText* m_staticText61;
		wxSlider* m_slTrgDelay;
		wxBitmapButton* m_bpButton2;
		wxStaticText* m_stHScale;
		wxBitmapButton* m_bpButton3;
		wxSlider* m_slHOffset;
		wxToggleButton* m_btCh1;
		wxSlider* m_slPos1;
		wxBitmapButton* m_bpButton4;
		wxStaticText* m_stScale1;
		wxBitmapButton* m_bpButton5;
		wxToggleButton* m_btCh2;
		wxSlider* m_slPos2;
		wxBitmapButton* m_bpButton6;
		wxStaticText* m_stScale2;
		wxBitmapButton* m_bpButton7;
		wxToggleButton* m_btCh3;
		wxSlider* m_slPos3;
		wxBitmapButton* m_bpButton8;
		wxStaticText* m_stScale3;
		wxBitmapButton* m_bpButton9;
		wxToggleButton* m_btCh4;
		wxSlider* m_slPos4;
		wxBitmapButton* m_bpButton10;
		wxStaticText* m_stScale4;
		wxBitmapButton* m_bpButton11;
		wxToggleButton* m_toggleCursorA;
		wxToggleButton* m_toggleCursorB;
		wxCheckBox* m_checkBox8;
		wxButton* m_btConfig;
		wxButton* m_btSave;
		wxButton* m_btMeasure;
		wxButton* m_btDisplay;
		wxButton* m_btPrint;
		wxButton* m_btAbout;
		wxButton* m_btExit;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnSave( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnPrint( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnExit( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCursor( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSnap( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnConfig( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnMeasure( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDisplay( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAbout( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnTrgLevelChange( wxScrollEvent& event ) { event.Skip(); }
		virtual void OnTrigger( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnTrgButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnTrgDelayChange( wxScrollEvent& event ) { event.Skip(); }
		virtual void OnHScaleChange( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnHOffsetChange( wxScrollEvent& event ) { event.Skip(); }
		virtual void OnChnOn( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnPosChange( wxScrollEvent& event ) { event.Skip(); }
		virtual void OnScaleChange( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		DOFrame_fb( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("DRS Oscilloscope"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1024,768 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		~DOFrame_fb();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class ConfigDialog_fb
///////////////////////////////////////////////////////////////////////////////
class ConfigDialog_fb : public wxDialog 
{
	private:
	
	protected:
		wxChoice* m_cbBoard;
		wxButton* m_btScan;
		wxButton* m_btInfo;
		wxRadioButton* m_rb1_4;
		wxRadioButton* m_rb5_8;
		wxRadioButton* m_rb9_12;
		wxRadioButton* m_rb13_16;
		wxRadioButton* m_rbLower;
		wxRadioButton* m_rbUpper;
		wxRadioButton* m_rbCombined;
		wxCheckBox* m_cbClkOn;
		wxRadioButton* m_rbR0;
		wxRadioButton* m_rbR045;
		wxRadioButton* m_rbR05;
		wxTextCtrl* m_tbFreq;
		wxStaticText* m_staticText26;
		wxCheckBox* m_cbLocked;
		wxStaticText* m_staticText261;
		wxStaticText* m_stActFreq;
		wxCheckBox* m_cbCalOn;
		wxTextCtrl* m_teCal;
		wxSlider* m_slCal;
		wxStaticText* m_staticText10;
		wxCheckBox* m_cbCalibrated;
		wxCheckBox* m_cbCalibrated2;
		wxCheckBox* m_cbSpikes;
		wxButton* m_button13;
		wxGauge* m_gaugeCalVolt;
		wxCheckBox* m_cbRotated;
		wxCheckBox* m_cbTCalOn;
		wxCheckBox* m_cbTrgCorr;
		wxCheckBox* m_cbExtRefclk;
		wxButton* m_button14;
		wxGauge* m_gaugeCalTime;
		wxButton* m_button10;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnBoardSelect( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRescan( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnInfo( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnChannelRange( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClkOn( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnInputRange( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnFreq( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnLock( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCalOn( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCalEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCalSlider( wxScrollEvent& event ) { event.Skip(); }
		virtual void OnDisplayWaveforms( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRemoveSpikes( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonCalVolt( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonCalTime( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClose( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		ConfigDialog_fb( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Configuration"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE );
		~ConfigDialog_fb();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class DisplayDialog_fb
///////////////////////////////////////////////////////////////////////////////
class DisplayDialog_fb : public wxDialog 
{
	private:
	
	protected:
		wxCheckBox* m_checkBox7;
		wxCheckBox* m_checkBox71;
		wxCheckBox* m_checkBox88;
		wxRadioButton* m_rbShowSample;
		wxRadioButton* m_rbShowAverage;
		wxRadioButton* m_rbShowPersist;
		wxStaticText* m_staticText59;
		wxComboBox* m_cbNumber;
		wxStaticText* m_staticText11;
		wxStaticText* m_staticText12;
		wxStaticText* m_staticText13;
		wxStaticText* m_staticText14;
		wxStaticText* m_staticText15;
		wxStaticText* m_staticText16;
		wxCheckBox* m_checkBox9;
		wxCheckBox* m_checkBox10;
		wxCheckBox* m_checkBox11;
		wxCheckBox* m_checkBox12;
		wxStaticText* m_staticText17;
		wxCheckBox* m_checkBox13;
		wxCheckBox* m_checkBox14;
		wxCheckBox* m_checkBox15;
		wxCheckBox* m_checkBox16;
		wxButton* m_button10;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnDateTime( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnShowGrid( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnLines( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDisplayMode( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClose( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		DisplayDialog_fb( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Display"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE );
		~DisplayDialog_fb();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class MeasureDialog_fb
///////////////////////////////////////////////////////////////////////////////
class MeasureDialog_fb : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText11;
		wxStaticText* m_staticText12;
		wxStaticText* m_staticText13;
		wxStaticText* m_staticText14;
		wxStaticText* m_staticText15;
		wxStaticText* m_staticText16;
		wxCheckBox* m_checkBox9;
		wxCheckBox* m_checkBox10;
		wxCheckBox* m_checkBox11;
		wxCheckBox* m_checkBox12;
		wxStaticText* m_staticText17;
		wxCheckBox* m_checkBox13;
		wxCheckBox* m_checkBox14;
		wxCheckBox* m_checkBox15;
		wxCheckBox* m_checkBox16;
		wxStaticText* m_staticText19;
		wxCheckBox* m_checkBox17;
		wxCheckBox* m_checkBox18;
		wxCheckBox* m_checkBox19;
		wxCheckBox* m_checkBox20;
		wxStaticLine* m_staticline4;
		wxStaticLine* m_staticline41;
		wxStaticLine* m_staticline42;
		wxStaticLine* m_staticline43;
		wxStaticLine* m_staticline44;
		wxStaticText* m_staticText20;
		wxCheckBox* m_checkBox21;
		wxCheckBox* m_checkBox22;
		wxCheckBox* m_checkBox23;
		wxCheckBox* m_checkBox24;
		wxStaticText* m_staticText21;
		wxCheckBox* m_checkBox25;
		wxCheckBox* m_checkBox26;
		wxCheckBox* m_checkBox27;
		wxCheckBox* m_checkBox28;
		wxStaticText* m_staticText22;
		wxCheckBox* m_checkBox29;
		wxCheckBox* m_checkBox30;
		wxCheckBox* m_checkBox31;
		wxCheckBox* m_checkBox32;
		wxStaticText* m_staticText23;
		wxCheckBox* m_checkBox33;
		wxCheckBox* m_checkBox34;
		wxCheckBox* m_checkBox35;
		wxCheckBox* m_checkBox36;
		wxStaticText* m_staticText221;
		wxCheckBox* m_checkBox291;
		wxCheckBox* m_checkBox2911;
		wxCheckBox* m_checkBox2912;
		wxCheckBox* m_checkBox2913;
		wxStaticText* m_staticText2211;
		wxCheckBox* m_checkBox2914;
		wxCheckBox* m_checkBox2915;
		wxCheckBox* m_checkBox2916;
		wxCheckBox* m_checkBox2917;
		wxStaticText* m_staticText231;
		wxCheckBox* m_checkBox37;
		wxCheckBox* m_checkBox38;
		wxCheckBox* m_checkBox39;
		wxCheckBox* m_checkBox40;
		wxStaticLine* m_staticline1;
		wxCheckBox* m_cbStat;
		wxButton* m_button15;
		wxStaticText* m_staticText27;
		wxComboBox* m_cbNAverage;
		wxStaticText* m_staticText271;
		wxStaticLine* m_staticline2;
		wxCheckBox* m_cbIndicator;
		wxStaticLine* m_staticline21;
		wxButton* m_button11;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnStat( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnStatReset( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnStatNAverage( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnIndicator( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClose( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		MeasureDialog_fb( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Select Measurements"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~MeasureDialog_fb();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class TriggerDialog_fb
///////////////////////////////////////////////////////////////////////////////
class TriggerDialog_fb : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText12;
		wxStaticText* m_staticText13;
		wxStaticText* m_staticText14;
		wxStaticText* m_staticText15;
		wxStaticText* m_staticText16;
		wxCheckBox* m_cbOR1;
		wxStaticText* m_staticText17;
		wxCheckBox* m_cbOR2;
		wxStaticText* m_staticText171;
		wxCheckBox* m_cbOR3;
		wxStaticText* m_staticText172;
		wxCheckBox* m_cbOR4;
		wxStaticText* m_staticText173;
		wxCheckBox* m_cbOREXT;
		wxStaticText* m_staticText84;
		wxCheckBox* m_cbAND1;
		wxStaticText* m_staticText18;
		wxCheckBox* m_cbAND2;
		wxStaticText* m_staticText181;
		wxCheckBox* m_cbAND3;
		wxStaticText* m_staticText182;
		wxCheckBox* m_cbAND4;
		wxStaticText* m_staticText183;
		wxCheckBox* m_cbANDEXT;
		wxStaticLine* m_staticline1;
		wxCheckBox* m_cbMultiBoard;
		wxStaticLine* m_staticline25;
		wxButton* m_button11;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClose( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		TriggerDialog_fb( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Configure Trigger"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~TriggerDialog_fb();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class AboutDialog_fb
///////////////////////////////////////////////////////////////////////////////
class AboutDialog_fb : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText18;
		wxStaticText* m_stVersion;
		wxStaticText* m_stBuild;
		wxStaticText* m_staticText20;
		wxStaticText* m_staticText21;
		wxStaticBitmap* m_bitmap1;
		wxStaticText* m_staticText23;
		wxHyperlinkCtrl* m_hyperlink1;
		wxButton* m_button12;
	
	public:
		
		AboutDialog_fb( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("About"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE );
		~AboutDialog_fb();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class InfoDialog_fb
///////////////////////////////////////////////////////////////////////////////
class InfoDialog_fb : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText45;
		wxStaticText* m_stBoardType;
		wxStaticText* m_staticText47;
		wxStaticText* m_stDRSType;
		wxStaticText* m_staticText49;
		wxStaticText* m_stSerialNumber;
		wxStaticText* m_staticText51;
		wxStaticText* m_stFirmwareRevision;
		wxStaticText* m_staticText53;
		wxStaticText* m_stTemperature;
		wxStaticText* m_staticText55;
		wxStaticText* m_stInputRange;
		wxStaticText* m_staticText57;
		wxStaticText* m_stCalibratedRange;
		wxStaticText* m_staticText59;
		wxStaticText* m_stCalibratedFrequency;
		wxStaticText* m_staticText61;
		wxStaticText* m_stFrequency;
		wxButton* m_button12;
	
	public:
		
		InfoDialog_fb( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Info"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE );
		~InfoDialog_fb();
	
};

#endif //__DRSOsc__
