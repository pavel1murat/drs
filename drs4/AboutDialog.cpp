/*
 * AboutDialog.cpp
 * About Dialog class
 * $Id: AboutDialog.cpp,v 1.1.1.1 2012/03/25 23:06:55 camac Exp $
 */

#include "DRSOscInc.h"

extern char svn_revision[];
extern char drsosc_version[];

AboutDialog::AboutDialog(wxWindow* parent)
:
AboutDialog_fb( parent )
{
   wxString str;

   str.Printf(_T("Version %s"), drsosc_version);
   m_stVersion->SetLabel(str);

   str.Printf(_T("Build %d"), atoi(svn_revision+17));
   m_stBuild->SetLabel(str);
}
