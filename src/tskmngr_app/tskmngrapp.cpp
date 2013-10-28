/******************************************************************************
 * Project:  wxGIS (GIS Server)
 * Purpose:  Main application class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2012 Bishop
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
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

#include "wxgis/tskmngr_app/tskmngrapp.h"

#include <wx/socket.h>

#include <iostream>

#define QUIT_CHAR_DELAY 500

//---------------------------------------------
// wxGISTaskManagerApp
//---------------------------------------------
IMPLEMENT_APP_CONSOLE(wxGISTaskManagerApp)

wxGISTaskManagerApp::wxGISTaskManagerApp(void) : wxAppConsole(), wxThreadHelper()
{
    m_vendorName = wxString(wxT("wxGIS"));
    m_vendorDisplayName = wxString(wxT("wxGIS"));

    m_appName = wxString(wxT("wxGISTaskManager"));
    m_appDisplayName = wxString(wxT("NextGIS Task Manager"));
    m_className = wxString(wxT("wxGISTaskManagerApp"));

    m_pTaskManager = NULL;
}

wxGISTaskManagerApp::~wxGISTaskManagerApp(void)
{
}

bool wxGISTaskManagerApp::OnInit()
{

#ifdef wxUSE_SNGLINST_CHECKER
    m_pChecker = new wxSingleInstanceChecker();
    if ( m_pChecker->IsAnotherRunning() )
    {
        wxLogError(_("Another program instance is already running, aborting."));

        wxDELETE( m_pChecker ); // OnExit() won't be called if we return false

        return false;
    }
#endif

 
#ifdef __WXMSW__
	wxLogDebug(wxT("wxSocketBase::Initialize"));
    wxSocketBase::Initialize();
#endif

    //create application
    m_pTaskManager = new wxGISTaskManager();

    //send interesting things to console
    return wxAppConsole::OnInit();
}

int wxGISTaskManagerApp::OnExit()
{
    wxDELETE(m_pTaskManager);

    Uninitialize();
#ifdef __WXMSW__
    wxSocketBase::Shutdown();
#endif

#ifdef wxUSE_SNGLINST_CHECKER
    wxDELETE( m_pChecker ); 
#endif

    return wxAppConsole::OnExit();//success == true ? EXIT_SUCCESS : EXIT_FAILURE;
}

// Loop until user enters q or Q
wxThread::ExitCode wxGISTaskManagerApp::Entry()
{
    // IMPORTANT:
    // this function gets executed in the secondary thread context!
    // here we do our long task, periodically calling TestDestroy():
    while (!GetThread()->TestDestroy())
    {
        wxFprintf(stdout, wxString(_("Press 'q' to quit.\n")));
        int nChar = getchar();
        if(nChar == 'q' || nChar == 'Q')
        {
            wxExit(); 
            break;
        }
        wxThread::Sleep(QUIT_CHAR_DELAY);
    }

    // TestDestroy() returned true (which means the main thread asked us
    // to terminate as soon as possible) or we ended the long task...
    return (wxThread::ExitCode)wxTHREAD_NO_ERROR;
}

bool wxGISTaskManagerApp::CreateAndRunExitThread(void)
{
    if (CreateThread(wxTHREAD_JOINABLE) != wxTHREAD_NO_ERROR)
    {
        wxLogError(_("Could not create the thread!"));
        return false;
    }

    // go!
    if (GetThread()->Run() != wxTHREAD_NO_ERROR)
    {
        wxLogError(_("Could not run the thread!"));
        return false;
    }
    return true;
}

void wxGISTaskManagerApp::DestroyExitThread(void)
{
    wxCriticalSectionLocker lock(m_ExitLock);
    if (GetThread() && GetThread()->IsRunning())
        GetThread()->Wait();//Delete();//
}

void wxGISTaskManagerApp::OnInitCmdLine(wxCmdLineParser& pParser)
{
    wxAppConsole::OnInitCmdLine(pParser);
    pParser.AddSwitch(wxT( "v" ), wxT( "version" ),     _( "The version of this program" ));
    pParser.AddSwitch(wxT( "i" ), wxT( "install" ),     _( "Install wxGIS task manager as service" ));
    pParser.AddSwitch(wxT( "r" ), wxT( "run" ),         _( "Run the wxGIS task manager in standalone mode. Press 'q' to quit." ));
    pParser.AddSwitch(wxT( "u" ), wxT( "uninstall" ),   _( "Uninstall wxGIS task manager service" ));
    pParser.AddSwitch(wxT( "s" ), wxT( "start" ),       _( "Start wxGIS task manager service" ));
    pParser.AddSwitch(wxT( "a" ), wxT( "app" ),         _( "Start wxGIS task manager from client application" ));

    pParser.SetLogo(wxString::Format(_("%s (%s)\nAuthor: Bishop (aka Barishnikov Dmitriy), polimax@mail.ru\nCopyright (c) 2012-%d"), m_appDisplayName.c_str(), GetAppVersionString().c_str(), __YEAR__));
}

void wxGISTaskManagerApp::OnAppAbout(void)
{
	wxString out = wxString::Format(_("%s (%s)\nAuthor: Bishop (aka Barishnikov Dmitriy), polimax@mail.ru\nCopyright (c) 2012-%d\n"), m_appDisplayName.c_str(), GetAppVersionString().c_str(), __YEAR__);
	wxFprintf(stdout, out);
}

void wxGISTaskManagerApp::OnAppOptions(void)
{
    wxCmdLineParser Parser;
    Parser.AddSwitch(wxT( "v" ), wxT( "version" ),     _( "The version of this program" ));
    Parser.AddSwitch(wxT( "i" ), wxT( "install" ),     _( "Install wxGIS task manager as service" ));
    Parser.AddSwitch(wxT( "r" ), wxT( "run" ),         _( "Run the wxGIS task manager in standalone mode. Press 'q' to quit." ));
    Parser.AddSwitch(wxT( "u" ), wxT( "uninstall" ),   _( "Uninstall wxGIS task manager service" ));
    Parser.AddSwitch(wxT( "s" ), wxT( "start" ),       _( "Start wxGIS task manager service" ));
    Parser.AddSwitch(wxT( "a" ), wxT( "app" ),         _( "Start wxGIS task manager from client application" ));

    Parser.SetLogo(wxString::Format(_("%s (%s)\nAuthor: Bishop (aka Barishnikov Dmitriy), polimax@mail.ru\nCopyright (c) 2012-%d"),  m_appDisplayName.c_str(), GetAppVersionString().c_str(), __YEAR__));

    Parser.Usage();
}

bool wxGISTaskManagerApp::OnCmdLineParsed(wxCmdLineParser& pParser)
{
	if( pParser.Found( wxT( "v" ) ) )
	{
	    OnAppAbout();

        return false;
	}

    if(!Initialize(wxT("wxGISTaskManager"), wxT("tskmngr_")))
        return false;

	if( pParser.Found( wxT( "r" ) ) || pParser.Found( wxT( "a" ) ))
	{
        if(!m_pTaskManager) 
            return true;

        CreateAndRunExitThread();
        //wxGISServer Server;
		if(!m_pTaskManager->Init())
		{
            m_pTaskManager->Exit();
			return true;
		}
	}

	//if( pParser.Found( wxT( "i" ) ) )
	//{
	//	//wxGISService Service(wxT("wxGISServer"));
	//	//Service.Install();
	//	return true;
	//}

 //	if( pParser.Found( wxT( "u" ) ) )
	//{
	//	//wxGISService Service(wxT("wxGISServer"));
	//	//Service.Uninstall();
	//	return true;
	//}
   
 //	if( pParser.Found( wxT( "h" ) ) )
	//{
	//	pParser.Usage();
	//	return true;
	//}

 //	if( pParser.Found( wxT( "s" ) ) )
	//{
	//	wxFprintf(stdout, wxString(_("Starting service...")));

	//	//wxGISService Service(wxT("wxGISServer"));
	//	//Service.StartService();

	//	//// When we get here, the service has been stopped
	//	//if( Service.GetExitCode() == EXIT_SUCCESS)
	//	//	return true;

	//	return false;
	//}

    return wxAppConsole::OnCmdLineParsed(pParser);
}

bool wxGISTaskManagerApp::Initialize(const wxString &sAppName, const wxString &sLogFilePrefix)
{
	wxGISAppConfig oConfig = GetConfig();
    if(!oConfig.IsOk())
        return false;

	wxString sLogDir = oConfig.GetLogDir();
	if(!SetupLog(sLogDir, sLogFilePrefix))
        return false;
	wxLogMessage(_("%s %s is initializing..."), sAppName.c_str(), GetAppVersionString().c_str());

	if(!SetupLoc(oConfig.GetLocale(), oConfig.GetLocaleDir()))
        return false;

	//setup sys dir
	wxString sSysDir = oConfig.GetSysDir();
	if(!SetupSys(sSysDir))
        return false;

	bool bDebugMode = oConfig.GetDebugMode();
	SetDebugMode(bDebugMode);

    if(!GetApplication())
        SetApplication(this);

    return true;
}

bool wxGISTaskManagerApp::SetupSys(const wxString &sSysPath)
{
    //no sys dir needed
    return true;
}



