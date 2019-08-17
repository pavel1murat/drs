/********************************************************************
 Name:         main.c
 Created by:   Stefan Ritt

 Contents:     Main wxWidgets application for DRSOsc project

 $Id: main.cpp,v 1.1.1.1 2012/03/25 23:06:55 camac Exp $
\********************************************************************/

#include "DRSOscInc.h"

#include "drsosc.xpm"

class DOApp : public wxApp
{
public:
   virtual bool OnInit();
   virtual int OnExit();

private:
};

IMPLEMENT_APP(DOApp)

/*------------------------------------------------------------------*/
/*                  NaN's                                           */
/*------------------------------------------------------------------*/

#ifdef _MSC_VER
unsigned long _nan[2]={0xffffffff, 0x7fffffff};
#endif

double ss_nan()
{
#ifdef _MSC_VER
   return *(double *)_nan;
#else
   return nan(NULL);
#endif
}

#ifdef _MSC_VER
#include <float.h>
#ifndef isnan
#define isnan(x) _isnan(x)
#endif
#ifndef finite
#define finite(x) _finite(x)
#endif
#elif defined(__linux__)
#include <math.h>
#endif

int ss_isnan(double x)
{
   return isnan(x);
}

void ss_sleep(int ms)
{
   wxThread::Sleep(ms);
}

/*------------------------------------------------------------------*/
/*                            DOApp                                 */
/*------------------------------------------------------------------*/

bool DOApp::OnInit()
{
   DOFrame *frame = new DOFrame(NULL);
   frame->Show(TRUE);
   frame->SetIcon(wxICON(drsosc));
   SetTopWindow(frame);
   return true;
} 

/*------------------------------------------------------------------*/

int DOApp::OnExit()
{
   return wxApp::OnExit();
} 
