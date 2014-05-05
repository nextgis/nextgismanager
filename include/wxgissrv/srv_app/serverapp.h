/******************************************************************************
 * Project:  wxGIS (GIS Server)
 * Purpose:  Main server application class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2012 Bishop
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 2 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/
/*#pragma once

#include "wxgis/base.h"

#include <wx/app.h>
#include <wx/defs.h>
#include <wx/cmdline.h>
#include <wx/string.h>

//#include "wxgis/core/config.h"
//#include "wx/file.h"
//#include <wx/ffile.h>
//
////#include "wx/log.h"
//#include "wx/apptrait.h"
//#include "wx/platinfo.h"
//#include <wx/dir.h>
//#include <wx/log.h>
//#include <wx/xml/xml.h>
//#include <wx/stdpaths.h>
//#include <wx/filename.h>
//#include <wx/dynload.h>
//#include <wx/dynlib.h>
//#include <wx/intl.h>

//#include <iostream>
//#include <stdio.h>
//#include <ctime>
//#include <vector>
*//*
// without this pragma, the stupid compiler precompiles #defines below so that
// changing them doesn't "take place" later!
#ifdef __VISUALC__
    #pragma hdrstop
#endif

#if !wxUSE_THREADS
    #error "This program requires thread support!"
#endif // wxUSE_THREADS


//bool parse_commandline_parameters( int argc, char** argv );
//int main(int argc, char** argv);

DECLARE_APP(Server);
*/

#pragma once

#include "wxgis/base.h"
#include "wxgis/core/init.h"
#include "wxgis/core/config.h"
#include "wxgissrv/srv_framework/server.h"

#include <wx/app.h>
#include <wx/snglinst.h>
#include <wx/thread.h>

/** \class wxGISServerApp serverapp.h
    \brief Main Server class.
*/
class wxGISServerApp :
	public wxAppConsole,
	public wxGISAppWithLibs,
    public wxThreadHelper
{
public:
	wxGISServerApp(void);
	virtual ~wxGISServerApp(void);
    virtual bool OnInit();
    virtual int OnExit();
    //virtual int OnRun();
    //virtual void ProcessPendingEvents(void);


    void OnInitCmdLine(wxCmdLineParser& pParser);
    bool OnCmdLineParsed(wxCmdLineParser& pParser);
protected:
    virtual wxThread::ExitCode Entry();
    bool CreateAndRunExitThread(void);
    void DestroyExitThread(void);
private:
    wxGISAppConfig m_oConfig;
    wxGISServer* m_pServer;
#ifdef wxUSE_SNGLINST_CHECKER
    wxSingleInstanceChecker *m_pChecker;
#endif    
    wxCriticalSection m_ExitLock;
};

DECLARE_APP(wxGISServerApp)