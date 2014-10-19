/******************************************************************************
 * Project:  wxGIS (GIS Server)
 * Purpose:  Main application class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
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

#include "wxgis/tskmngr_app/tskmngrapp.h"

#include <wx/socket.h>

#include <iostream>

#define QUIT_CHAR_DELAY 500

//---------------------------------------------
// wxGISTaskManagerApp
//---------------------------------------------
IMPLEMENT_APP_CONSOLE(wxGISTaskManagerApp)

wxGISTaskManagerApp::wxGISTaskManagerApp(void) : wxAppConsole(), wxGISThreadHelper(), wxGISService()
{
    m_vendorName = wxString(VENDOR);
    m_vendorDisplayName = wxString(wxT("wxGIS"));

    m_appName = wxString(wxT("wxGISTaskManager"));
    m_appDisplayName = wxString(wxT("NextGIS Task Manager"));
    m_className = wxString(wxT("wxGISTaskManagerApp"));

    m_sServiceName = m_appName + wxString(wxT("_service"));
    m_sServiceDisplayName = m_appDisplayName + wxString(_(" service"));

    m_pTaskManager = NULL;
    m_bService = false;
}

wxGISTaskManagerApp::~wxGISTaskManagerApp(void)
{
#ifdef _WIN32
    wxSocketBase::Shutdown();
#endif

#ifdef wxUSE_SNGLINST_CHECKER
    wxDELETE(m_pChecker);
#endif
}

int wxGISTaskManagerApp::OnRun()
{

    if (!m_pTaskManager)
        return EXIT_FAILURE;

    if (!m_pTaskManager->Init())
    {
        m_pTaskManager->Exit();
        return EXIT_FAILURE;
    }

    CreateAndRunThread();

    wxAppConsole::OnRun();

    return EXIT_SUCCESS;//success == true ? EXIT_SUCCESS : EXIT_FAILURE;//EXIT_FAILURE;//
}

bool wxGISTaskManagerApp::OnInit()
{
#ifdef _WIN32
	wxLogDebug(wxT("wxSocketBase::Initialize"));
    wxSocketBase::Initialize();
#endif

#ifdef wxUSE_SNGLINST_CHECKER
    m_pChecker = new wxSingleInstanceChecker();
    if ( m_pChecker->IsAnotherRunning() )
    {
        wxLogError(_("Another program instance is already running, aborting."));

        wxDELETE( m_pChecker ); // OnExit() won't be called if we return false

        return false;
    }
#endif

    //create application
    m_pTaskManager = new wxGISTaskManager();

    //send interesting things to console
    return wxAppConsole::OnInit();
}

void wxGISTaskManagerApp::Exit()
{
    wxDELETE(m_pTaskManager);

    Uninitialize();

    wxAppConsole::Exit();
}

// Loop until user enters q or Q
wxThread::ExitCode wxGISTaskManagerApp::Entry()
{
    if (m_bService)
    {
#ifdef _WIN32
        StartService();
#else
        Daemonize();
#endif
    }
    else
    {
        while (!TestDestroy())
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
    }

    return (wxThread::ExitCode)wxTHREAD_NO_ERROR;
}

void wxGISTaskManagerApp::OnInitCmdLine(wxCmdLineParser& pParser)
{
    wxAppConsole::OnInitCmdLine(pParser);
    pParser.AddSwitch(wxT( "v" ), wxT( "version" ),     _( "The version of this program" ));
    pParser.AddSwitch(wxT( "r" ), wxT( "run" ),         _( "Run the wxGIS task manager in standalone mode. Press 'q' to quit." ));
#ifdef _WIN32
    pParser.AddSwitch(wxT( "i" ), wxT( "install" ),     _( "Install wxGIS task manager as service" ));
    pParser.AddSwitch(wxT( "u" ), wxT( "uninstall" ),   _( "Uninstall wxGIS task manager service" ));
#endif //_WIN32
    pParser.AddSwitch(wxT( "s" ), wxT( "start" ),       _( "Start wxGIS task manager service" ));
    pParser.AddSwitch(wxT( "a" ), wxT( "app" ),         _( "Start wxGIS task manager from client application" ));

    pParser.SetLogo(wxString::Format(_("%s (%s)\nAuthor: Bishop (aka Baryshnikov Dmitriy), polimax@mail.ru\nCopyright (c) 2012-%d"), m_appDisplayName.c_str(), GetAppVersionString().c_str(), __YEAR__));
}

void wxGISTaskManagerApp::OnAppAbout(void)
{
	wxString out = wxString::Format(_("%s (%s)\nAuthor: Bishop (aka Baryshnikov Dmitriy), polimax@mail.ru\nCopyright (c) 2012-%d\n"), m_appDisplayName.c_str(), GetAppVersionString().c_str(), __YEAR__);
	wxFprintf(stdout, out);
}

void wxGISTaskManagerApp::OnAppOptions(void)
{
    wxCmdLineParser Parser;
    Parser.AddSwitch(wxT( "v" ), wxT( "version" ),     _( "The version of this program" ));
    Parser.AddSwitch(wxT( "r" ), wxT( "run" ),         _( "Run the wxGIS task manager in standalone mode. Press 'q' to quit." ));
#ifdef _WIN32
    Parser.AddSwitch(wxT("i"), wxT("install"), _("Install wxGIS task manager as service"));
    Parser.AddSwitch(wxT( "u" ), wxT( "uninstall" ),   _( "Uninstall wxGIS task manager service" ));
#endif //_WIN32
    Parser.AddSwitch(wxT("s"), wxT("start"), _("Start wxGIS task manager service"));
    Parser.AddSwitch(wxT("a"), wxT("app"), _("Start wxGIS task manager from client application"));

    Parser.SetLogo(wxString::Format(_("%s (%s)\nAuthor: Bishop (aka Baryshnikov Dmitriy), polimax@mail.ru\nCopyright (c) 2012-%d"),  m_appDisplayName.c_str(), GetAppVersionString().c_str(), __YEAR__));

    Parser.Usage();
}

bool wxGISTaskManagerApp::OnCmdLineParsed(wxCmdLineParser& pParser)
{
	if( pParser.Found( wxT( "v" ) ) )
	{
	    OnAppAbout();
        OnExit();
        exit(EXIT_SUCCESS);
        return true;
	}

	else if( pParser.Found( wxT( "r" ) ) || pParser.Found( wxT( "a" ) ))
	{
        if (!Initialize(wxT("wxGISTaskManager"), wxT("tskmngr_")))
            return false;

        if (!m_pTaskManager)
            return false;

        m_bService = false;        
	}
#ifdef _WIN32

	else if( pParser.Found( wxT( "i" ) ) )
	{
        bool bRes = Install(wxT("--start"));
        if (!bRes)
        {
            wxFprintf(stderr, _("install service failed"));
            return false;
        }
        OnExit();
        exit(EXIT_SUCCESS);
        return true;
    }

 	else if( pParser.Found( wxT( "u" ) ) )
	{
        bool bRes = Uninstall();
        if (!bRes)
        {
            wxFprintf(stderr, _("uninstall service failed"));
            return false;
        }
        OnExit();
        exit(EXIT_SUCCESS);
        return true;
    }

 	else if( pParser.Found( wxT( "h" ) ) )
	{
		pParser.Usage();
        OnExit();
        exit(EXIT_SUCCESS);
        return true;
    }
 	else if( pParser.Found( wxT( "s" ) ) )
	{
        if (!Initialize(wxT("wxGISTaskManager"), wxT("tskmngr_")))
            return false;

        if (!m_pTaskManager)
            return false;

        if (!m_pTaskManager->Init())
        {
            m_pTaskManager->Exit();
            return false;
        }
        m_pTaskManager->SetExitState(enumGISNetCmdStNoExit);

        wxLogMessage(wxT("Starting service..."));

        m_bService = true;
        CreateAndRunThread();
	}
    else
    {
		pParser.Usage();
        OnExit();
        exit(EXIT_SUCCESS);
        return true;
    }

#endif //_WIN32

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

#ifdef _WIN32
void wxGISTaskManagerApp::Run()
{
    wxLogMessage(_("%s %s run"), m_sServiceDisplayName.c_str(), GetAppVersionString().c_str());

    while (m_bServiceIsRunning)
    {
        wxThread::Sleep(QUIT_CHAR_DELAY);
    }

    wxThread::Sleep(QUIT_CHAR_DELAY);

    wxLogMessage(_("%s %s exit run state"), m_sServiceDisplayName.c_str(), GetAppVersionString().c_str());
}

bool wxGISTaskManagerApp::Initialize()
{
    wxLogMessage(_("%s %s initialize"), m_sServiceDisplayName.c_str(), GetAppVersionString().c_str());
    return true;
}

void wxGISTaskManagerApp::OnStop()
{
    wxLogMessage(_("%s %s stop"), m_sServiceDisplayName.c_str(), GetAppVersionString().c_str());
    wxDELETE(m_pTaskManager);

    wxGISService::OnStop();
}

void wxGISTaskManagerApp::OnPause()
{
    wxLogMessage(_("%s %s pause"), m_sServiceDisplayName.c_str(), GetAppVersionString().c_str());
    wxDELETE(m_pTaskManager);
    wxGISService::OnPause();
}

void wxGISTaskManagerApp::OnContinue()
{
    wxLogMessage(_("%s %s continue"), m_sServiceDisplayName.c_str(), GetAppVersionString().c_str());
    m_pTaskManager = new wxGISTaskManager();

    if (!m_pTaskManager->Init())
    {
        m_pTaskManager->Exit();
        return;
    }

    m_pTaskManager->SetExitState(enumGISNetCmdStNoExit);

    wxGISService::OnContinue();
}

void wxGISTaskManagerApp::OnInterrogate()
{
    wxLogMessage(_("%s %s interrogate"), m_sServiceDisplayName.c_str(), GetAppVersionString().c_str());
    //report current status of the service
    wxGISService::OnInterrogate();
}

void wxGISTaskManagerApp::OnShutdown()
{
    wxLogMessage(_("%s %s shutdown"), m_sServiceDisplayName.c_str(), GetAppVersionString().c_str());

    wxGISService::OnShutdown();
    wxExit();
}
#endif // _WIN32

