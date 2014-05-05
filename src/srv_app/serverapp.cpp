/******************************************************************************
 * Project:  wxGIS (GIS Server)
 * Purpose:  Main application class.
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

#include "wxgissrv/srv_app/serverapp.h"
#include "wxgis/version.h"

#include <wx/socket.h>

#include "ogr_api.h"
#include "gdal_priv.h"

#include <iostream>

#define QUIT_CHAR_DELAY 25

//---------------------------------------------
// wxGISServerApp
//---------------------------------------------
IMPLEMENT_APP_CONSOLE(wxGISServerApp)

wxGISServerApp::wxGISServerApp(void)
{
    m_vendorName = wxString(wxT("wxGIS"));
    m_vendorDisplayName = wxString(wxT("wxGIS"));
    m_appName = wxString(wxT("wxgisserver"));
    m_appDisplayName = wxString(wxT("wxGIS Server"));
    m_className = wxString(wxT("wxGISServerApp"));

    m_pServer = NULL;
}

wxGISServerApp::~wxGISServerApp(void)
{
//    OnExit();
    DestroyExitThread();
}

//int wxGISServerApp::OnRun()
//{
//    wxAppConsole::OnRun();
//
//    return EXIT_SUCCESS;
//}

//void wxGISServerApp::ProcessPendingEvents(void)
//{
//    wxAppConsole::ProcessPendingEvents();
//
//    //char c(' ');
//    //while (c != 'q' && c != 'Q')
//    //{
//    //    std::cout << "Press q then enter to quit: ";
//    //    std::cin >> c;
//    //}
//}

bool wxGISServerApp::OnInit()
{

#ifdef wxUSE_SNGLINST_CHECKER
    m_pChecker = new wxSingleInstanceChecker(wxT("wxgisserverapp"));
    if ( m_pChecker->IsAnotherRunning() )
    {
        wxLogError(_("Another program instance is already running, aborting."));

        wxDELETE( m_pChecker ); // OnExit() won't be called if we return false

        return false;
    }
#endif

    m_oConfig = GetConfig();
	if(!m_oConfig.IsOk())
		return false;

    //create application
    m_pServer = new wxGISServer();
    SetApplication(m_pServer);

	//setup loging
	wxString sLogDir = m_oConfig.GetLogDir();
    //if(!m_pServer->SetupLog(sLogDir))
    //    return;

	//setup locale
	wxString sLocale = m_oConfig.GetLocale();
	wxString sLocaleDir = m_oConfig.GetLocaleDir();
    if(!m_pServer->SetupLoc(sLocale, sLocaleDir))
        return false;

   	//setup sys
    wxString sSysDir = m_oConfig.GetSysDir();
    //if(!m_pServer->SetupSys(sSysDir))
    //    return;

   	//setup debug
	bool bDebugMode = m_oConfig.GetDebugMode();
    //m_pServer->SetDebugMode(bDebugMode);

    //some default GDAL
	wxString sGDALCacheMax = m_oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/GDAL/cachemax")), wxString(wxT("128")));
	CPLSetConfigOption( "GDAL_CACHEMAX", sGDALCacheMax.mb_str() );
    CPLSetConfigOption ( "LIBKML_USE_DOC.KML", "no" );
    //GDAL_MAX_DATASET_POOL_SIZE
    //OGR_ARC_STEPSIZE

	OGRRegisterAll();
	GDALAllRegister();

#ifdef __WXMSW__
	wxLogDebug(wxT("wxSocketBase::Initialize"));
    wxSocketBase::Initialize();
#endif

	//store values

	m_oConfig.SetLogDir(sLogDir);
	m_oConfig.SetLocale(sLocale);
	m_oConfig.SetLocaleDir(sLocaleDir);
	m_oConfig.SetSysDir(sSysDir);
	m_oConfig.SetDebugMode(bDebugMode);

	//gdal
	m_oConfig.Write(enumGISHKCU, wxString(wxT("wxGISCommon/GDAL/cachemax")), sGDALCacheMax);

    wxString sKey(wxT("wxGISCommon/libs"));
    //load libs
	wxXmlNode* pLibsNode = m_oConfig.GetConfigNode(enumGISHKCU, sKey);
	if(pLibsNode)
		LoadLibs(pLibsNode);
	pLibsNode = m_oConfig.GetConfigNode(enumGISHKLM, sKey);
	if(pLibsNode)
		LoadLibs(pLibsNode);

    //send interesting things to console
    return wxAppConsole::OnInit();
	//return true;
}

// Loop until user enters q or Q



int wxGISServerApp::OnExit()
{
    if(m_pServer)
        m_pServer->Exit();
    SetApplication(NULL);

#ifdef __WXMSW__
    wxSocketBase::Shutdown();
#endif

	SerializeLibs();

	GDALDestroyDriverManager();
	OGRCleanupAll();

	UnLoadLibs();

#ifdef wxUSE_SNGLINST_CHECKER
    wxDELETE( m_pChecker ); 
#endif

    return wxAppConsole::OnExit();//success == true ? EXIT_SUCCESS : EXIT_FAILURE;
}

wxThread::ExitCode wxGISServerApp::Entry()
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
            Exit(); //wxTheApp->
        //ProcessNetMessage();
            break;
        }
        wxThread::Sleep(QUIT_CHAR_DELAY);
    }

    // TestDestroy() returned true (which means the main thread asked us
    // to terminate as soon as possible) or we ended the long task...
    return (wxThread::ExitCode)0;
}

bool wxGISServerApp::CreateAndRunExitThread(void)
{
    if (CreateThread(wxTHREAD_JOINABLE) != wxTHREAD_NO_ERROR)////wxTHREAD_DETACHED
    {
        wxLogError(_("Could not create the exit thread!"));
        return false;
    }

    // go!
    if (GetThread()->Run() != wxTHREAD_NO_ERROR)
    {
        wxLogError(_("Could not run the exit thread!"));
        return false;
    }
    return true;
}

void wxGISServerApp::DestroyExitThread(void)
{
    wxCriticalSectionLocker lock(m_ExitLock);
    if (GetThread() && GetThread()->IsRunning())
        GetThread()->Wait();//Delete();//
}

void wxGISServerApp::OnInitCmdLine(wxCmdLineParser& pParser)
{
    wxAppConsole::OnInitCmdLine(pParser);
    pParser.AddSwitch(wxT( "v" ), wxT( "version" ),     _( "The version of this program" ));
    pParser.AddSwitch(wxT( "i" ), wxT( "install" ),     _( "Install wxGIS Server as service" ));
    pParser.AddSwitch(wxT( "r" ), wxT( "run" ),         _( "Run the wxGIS Server in standalone mode. Press 'q' to quit." ));
    pParser.AddSwitch(wxT( "u" ), wxT( "uninstall" ),   _( "Uninstall wxGIS Server service" ));
    pParser.AddSwitch(wxT( "s" ), wxT( "start" ),       _( "Start wxGIS Server service" ));

    pParser.SetLogo(wxString::Format(_("The wxGIS Server (%s)\nAuthor: Bishop (aka Barishnikov Dmitriy), polimax@mail.ru\nCopyright (c) 2010-%d"), wxString(wxGIS_VERSION_NUM_DOT_STRING_T).c_str(), __YEAR__));
}

bool wxGISServerApp::OnCmdLineParsed(wxCmdLineParser& pParser)
{
   	//print params to log
	for(size_t i = 0; i < pParser.GetParamCount(); i++)
		wxLogDebug(pParser.GetParam());

	if( pParser.Found( wxT( "v" ) ) )
	{
	    wxString out = wxString::Format(_("The wxGIS Server (%s)\nAuthor: Bishop (aka Barishnikov Dmitriy), polimax@mail.ru\nCopyright (c) 2010-%d\n"), wxString(wxGIS_VERSION_NUM_DOT_STRING_T).c_str(), __YEAR__);
	    wxFprintf(stdout, out);

        return false;
	}

	if( pParser.Found( wxT( "r" ) ) )
	{
        if(!m_pServer) 
            return true;

	    //setup loging
	    wxString sLogDir = m_oConfig.GetLogDir();
        if(!m_pServer->SetupLog(sLogDir))
            return true;

   	    //setup sys
        wxString sSysDir = m_oConfig.GetSysDir();
        if(!m_pServer->SetupSys(sSysDir))
            return true;

   	    //setup debug
	    bool bDebugMode = m_oConfig.GetDebugMode();
        m_pServer->SetDebugMode(bDebugMode);

        CreateAndRunExitThread();
        //wxGISServer Server;
		if(!m_pServer->CreateApp())//.Start(wxT("wxGISServer"), CONFIG_DIR, my_parser))
		{
            m_pServer->Exit();
			return true;
		}

  //      wxFprintf(stdout, wxString(_("Press 'q' to quit.\n")));
		//while (getchar() != 'q')
  //      {
  //          Yield();
  //      }

		//m_pServer->Exit();
		//return false;
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

/*#include "wxgissrv/srv_app/service.h"

int main(int argc, char **argv)
{

    wxApp::CheckBuildOptions(WX_BUILD_OPTIONS_SIGNATURE, "program");

    wxInitializer initializer;
    if ( !initializer )
    {
        wxFprintf(stderr, _("Failed to initialize the wxWidgets library, aborting."));//fprintf(
        return -1;
    }

#ifdef __WXMSW__
	wxLogDebug(wxT("wxSocketBase::Initialize"));
    wxSocketBase::Initialize();
#endif

	bool success( false );
    /*
    // Parse command line arguments
    success = parse_commandline_parameters( argc, argv );
*//*
#ifdef __WXMSW__
    wxSocketBase::Shutdown();
#endif

	return success == true ? EXIT_SUCCESS : EXIT_FAILURE;
}

/*
bool parse_commandline_parameters( int argc, char** argv )
{
    // Create the commandline parser
    static const wxCmdLineEntryDesc my_cmdline_desc[] =
    {
        { wxCMD_LINE_SWITCH, wxT( "h" ), wxT( "help" ), _( "Show this help message" ),
            wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
        { wxCMD_LINE_SWITCH, wxT( "v" ), wxT( "version" ), _( "The version of this program" ) },
		{ wxCMD_LINE_SWITCH, wxT( "r" ), wxT("run"), _( "Run the wxGIS Server in standalone mode. Press 'q' to quit." ) },
		{ wxCMD_LINE_SWITCH, wxT( "i" ), wxT("install"), _( "Install wxGIS Server as service" ) },
		{ wxCMD_LINE_SWITCH, wxT( "u" ), wxT("uninstall"), _( "Uninstall wxGIS Server service" ) },
		{ wxCMD_LINE_SWITCH, wxT( "s" ), wxT("start"), _( "Start wxGIS Server service" ) },
		//{ wxCMD_LINE_OPTION, wxT( "i" ), wxT("index"), _( "Index the locations set in DB with max count of nodes" ), wxCMD_LINE_VAL_NUMBER },
		//{ wxCMD_LINE_SWITCH, wxT( "u" ), wxT("update"), _( "Update the locations set in config file (add/remove paths & etc.)" ) },
  //      { wxCMD_LINE_SWITCH, wxT( "l" ), wxT("list"), _( "list path's" ) },
  //      { wxCMD_LINE_SWITCH, wxT( "L" ), wxT("listall"), _( "list all path's" ) },
  //      { wxCMD_LINE_OPTION, wxT( "d" ), wxT("detail"), _("show node details"), wxCMD_LINE_VAL_NUMBER },
		{ wxCMD_LINE_NONE }
    };

    wxCmdLineParser my_parser( my_cmdline_desc, argc, argv );
    my_parser.SetLogo(wxString::Format(_("The wxGIS Server (%s)\nAuthor: Bishop (aka Barishnikov Dmitriy), polimax@mail.ru\nCopyright (c) 2010-2011"), wxString(wxGIS_VERSION_NUM_DOT_STRING_T).c_str()));

	//print params to log
	for(size_t i = 0; i < my_parser.GetParamCount(); i++)
		wxLogDebug(my_parser.GetParam());

    // Parse the parameters
    int my_parse_success = my_parser.Parse( );
    // Print help if the specify /? or if a syntax error occured.
    if( my_parse_success != 0 )
    {
        return false;
    }

	if( my_parser.Found( wxT( "v" ) ) )
	{
	    wxString out = wxString::Format(_("The wxGIS Server (%s)\nAuthor: Bishop (aka Barishnikov Dmitriy), polimax@mail.ru\nCopyright (c) 2010-2011\n"), wxString(wxGIS_VERSION_NUM_DOT_STRING_T).c_str());
	    wxFprintf(stdout, out);
		return true;
	}

	if( my_parser.Found( wxT( "r" ) ) )
	{
        wxGISServer Server;
		if(!Server.Start(wxT("wxGISServer"), CONFIG_DIR, my_parser))
		{
            Server.Stop();
			return false;
		}

        wxFprintf(stdout, wxString(_("Press 'q' to quit.\n")));
		while (getchar() != 'q');

		Server.Stop();
		return true;
	}

	if( my_parser.Found( wxT( "i" ) ) )
	{
		wxGISService Service(wxT("wxGISServer"));
		Service.Install();
		return true;
	}

 	if( my_parser.Found( wxT( "u" ) ) )
	{
		wxGISService Service(wxT("wxGISServer"));
		Service.Uninstall();
		return true;
	}
   
 	if( my_parser.Found( wxT( "h" ) ) )
	{
		my_parser.Usage();
		return true;
	}

 	if( my_parser.Found( wxT( "s" ) ) )
	{
		wxFprintf(stdout, wxString(_("Starting service...")));

		wxGISService Service(wxT("wxGISServer"));
		Service.StartService();

		// When we get here, the service has been stopped
		if( Service.GetExitCode() == EXIT_SUCCESS)
			return true;

		return false;
	}

	my_parser.Usage();

    // Either we are using the defaults or the provided parameters were valid.

    return true;
} 
*/