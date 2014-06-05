/******************************************************************************
 * Project:  wxGIS (Task Manager)
 * Purpose:  Task manager application class.
 * Author:   Dmitry Barishnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2012,2014 Dmitry Baryshnikov
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

#pragma once

#include "wxgis/base.h"
#include "wxgis/core/config.h"
#include "wxgis/core/init.h"
#include "wxgis/tskmngr_app/tskmngr.h"
#include "wxgis/version.h"

#include <wx/app.h>
#include <wx/snglinst.h>
#include <wx/cmdline.h>

/** @class wxGISTaskManagerApp

    Main task manager application.
    This is an singleton application which manage tasks (geoprocessing or something else). Each application or different instances of the same application create, change, delete, start, stop, pause their tasks via this application. The application instance should try to start task manager application and connect to it via tcp ethernet protocol. While exiting - send exit command to task manager. If there are no any other connections, task manager should exit. In standalone mode task manager can be run as a service to execute by timer.

*/
class wxGISTaskManagerApp :
	public wxAppConsole,
    public wxThreadHelper,
    public wxGISInitializer,
    public wxGISService
{
public:
	wxGISTaskManagerApp(void);
	virtual ~wxGISTaskManagerApp(void);
    // wxAppConsole
    virtual bool OnInit();
    virtual int OnExit();
    void OnInitCmdLine(wxCmdLineParser& pParser);
    bool OnCmdLineParsed(wxCmdLineParser& pParser);
    // wxGISInitializer
	virtual bool Initialize(const wxString &sAppName, const wxString &sLogFilePrefix);//, wxCmdLineParser& parser
    // wxGISService
#ifdef _WIN32
    virtual void Run();
    virtual bool Initialize();
    virtual void OnStop();
    virtual void OnPause();
    virtual void OnContinue();
    virtual void OnInterrogate();
    virtual void OnShutdown();
#endif // _WIN32
    // IApplication
    virtual bool SetupSys(const wxString &sSysPath);
    virtual wxString GetAppName(void) const {return m_appName;};
	virtual wxString GetAppDisplayName(void) const{return m_appDisplayName;};
    virtual wxString GetAppDisplayNameShort(void) const {return wxString(_("Task Manager"));};
    virtual wxString GetAppVersionString(void) const {return wxString(wxGIS_VERSION_NUM_DOT_STRING_T);};
    virtual void OnAppAbout(void);
    virtual void OnAppOptions(void);
protected:
    virtual wxThread::ExitCode Entry();
    bool CreateAndRunExitThread(void);
    void DestroyExitThread(void);
protected:
    wxGISAppConfig m_oConfig;
#ifdef wxUSE_SNGLINST_CHECKER
    wxSingleInstanceChecker *m_pChecker;
#endif
    wxCriticalSection m_ExitLock;
    wxGISTaskManager* m_pTaskManager;
    bool m_bService;
};

DECLARE_APP(wxGISTaskManagerApp)


