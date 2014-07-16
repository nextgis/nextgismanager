/******************************************************************************
 * Project:  wxGIS (GIS Server)
 * Purpose:  wxGISServer class. Main server class
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2008-2012 Bishop
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

#include "wxgissrv/srv_framework/server.h"
#include "wxgis/version.h"

#include <wx/cmdline.h>
#include <wx/dir.h>

#include "ogr_api.h"
#include "gdal_priv.h"

//----------------------------------------------------------------------------
// wxGISServer
//----------------------------------------------------------------------------
IMPLEMENT_CLASS(wxGISServer, IApplication)

wxGISServer::wxGISServer(void) : IApplication()
{
    //m_pszOldLocale = NULL;
    m_pLocale = NULL;
    m_pCatalog = NULL;
    m_pNetworkService = NULL;
}

wxGISServer::~wxGISServer(void)
{
    if(m_pNetworkService)
        Exit();

  //  if(m_pszOldLocale != NULL)
		//setlocale(LC_NUMERIC, m_pszOldLocale);
    wxDELETE(m_pLocale);
	//wxDELETE(m_pszOldLocale);
}

bool wxGISServer::SetupSys(const wxString &sSysPath)
{
	//setup sys dir
	if(!wxDirExists(sSysPath))
	{
		wxLogError(wxString::Format(_("wxGISServer: System folder is absent! Lookup path '%s'"), sSysPath.c_str()));
		return false;
	}

	wxString sGdalDataDir = sSysPath + wxFileName::GetPathSeparator() + wxString(wxT("gdal")) + wxFileName::GetPathSeparator();
	CPLSetConfigOption("GDAL_DATA", sGdalDataDir.mb_str(wxConvUTF8) );
#ifdef HAVE_PROJ
	sGdalDataDir = sSysPath + wxFileName::GetPathSeparator() + wxString(wxT("proj")) + wxFileName::GetPathSeparator();
	//CPLSetConfigOption("PROJ_LIB", sGdalDataDir.mb_str(wxConvUTF8) );
    CPLString pszPROJ_LIB(sGdalDataDir.mb_str(wxConvUTF8));
    const char *path = pszPROJ_LIB.c_str();
    pj_set_searchpath(1, &path);
#endif
    return true;
}

void wxGISServer::SetDebugMode(bool bDebugMode)
{
	CPLSetConfigOption("CPL_DEBUG", bDebugMode == true ? "ON" : "OFF");
	CPLSetConfigOption("CPL_TIMESTAMP", "ON");
	CPLSetConfigOption("CPL_LOG_ERRORS", bDebugMode == true ? "ON" : "OFF");
}

bool wxGISServer::SetupLog(const wxString &sLogPath)
{
	if(sLogPath.IsEmpty())
	{
		wxLogError(_("wxGISServer: Failed to get log folder"));
        return false;
	}

	if(!wxDirExists(sLogPath))
		wxFileName::Mkdir(sLogPath, 0777, wxPATH_MKDIR_FULL);


	wxDateTime dt(wxDateTime::Now());
	wxString logfilename = sLogPath + wxFileName::GetPathSeparator() + wxString::Format(wxT("srvlog_%.4d%.2d%.2d.log"),dt.GetYear(), dt.GetMonth() + 1, dt.GetDay());

    if(m_LogFile.IsOpened())
        m_LogFile.Close();

	if(!m_LogFile.Open(logfilename.GetData(), wxT("a+")))
		wxLogError(_("wxGISServer: Failed to open log file %s"), logfilename.c_str());

	wxLog::SetActiveTarget(new wxLogStderr(m_LogFile.fp()));

#ifdef WXGISPORTABLE
	wxLogMessage(wxT("Portable"));
#endif
	wxLogMessage(wxT(" "));
	wxLogMessage(wxT("####################################################################"));
	wxLogMessage(wxT("##                    %s                    ##"),wxNow().c_str());
	wxLogMessage(wxT("####################################################################"));
	long dFreeMem =  wxMemorySize(wxGetFreeMemory() / 1048576).ToLong();
	wxLogMessage(_("HOST '%s': OS desc - %s, free memory - %u Mb"), wxGetFullHostName().c_str(), wxGetOsDescription().c_str(), dFreeMem);
	wxLogMessage(_("wxGISServer: %s %s is initializing..."), GetAppName().c_str(), GetAppVersionString().c_str());
	wxLogMessage(_("wxGISServer: Log file: %s"), logfilename.c_str());

	wxString sCPLLogPath = sLogPath + wxFileName::GetPathSeparator() + wxString(wxT("gdal_log_cat.txt"));
	CPLString szCPLLogPath(sCPLLogPath.mb_str(wxConvUTF8));
	CPLSetConfigOption("CPL_LOG", szCPLLogPath );
    return true;
}

bool wxGISServer::SetupLoc(const wxString &sLoc, const wxString &sLocPath)
{
    wxLogVerbose(_("wxGISServer: Initialize locale"));

  //  if(m_pszOldLocale != NULL)
		//setlocale(LC_NUMERIC, m_pszOldLocale);
	//wxDELETE(m_pszOldLocale);
    wxDELETE(m_pLocale);

	//init locale
    if ( !sLoc.IsEmpty() )
    {
		int iLocale(0);
		const wxLanguageInfo* loc_info = wxLocale::FindLanguageInfo(sLoc);
		if(loc_info != NULL)
		{
			iLocale = loc_info->Language;
			wxLogMessage(_("wxGISServer: Language is set to %s"), loc_info->Description.c_str());
		}

        // don't use wxLOCALE_LOAD_DEFAULT flag so that Init() doesn't return
        // false just because it failed to load wxstd catalog

        m_pLocale = new wxLocale();
        if ( !m_pLocale->Init(iLocale) )
        {
            wxLogError(wxT("wxGISServer: This language is not supported by the system."));
            return false;
        }
    }

	//m_locale.Init(wxLANGUAGE_DEFAULT);

    // normally this wouldn't be necessary as the catalog files would be found
    // in the default locations, but when the program is not installed the
    // catalogs are in the build directory where we wouldn't find them by
    // default
	wxString sLocalePath = sLocPath + wxFileName::GetPathSeparator() + sLoc;
	if(wxDirExists(sLocalePath))
	{
		wxLocale::AddCatalogLookupPathPrefix(sLocalePath);

		// Initialize the catalogs we'll be using
		//load multicat from locale
		wxArrayString trans_arr;
		wxDir::GetAllFiles(sLocalePath, &trans_arr, wxT("*.mo"));

		for(size_t i = 0; i < trans_arr.size(); ++i)
		{
			wxFileName name(trans_arr[i]);
			m_pLocale->AddCatalog(name.GetName());
		}

		// this catalog is installed in standard location on Linux systems and
		// shows that you may make use of the standard message catalogs as well
		//
		// if it's not installed on your system, it is just silently ignored
	#ifdef __LINUX__
		{
			wxLogNull noLog;
			m_pLocale->AddCatalog(_T("fileutils"));
		}
	#endif
	}

	//support of dot in doubles and floats
	//m_pszOldLocale = strdup(setlocale(LC_NUMERIC, NULL));
 //   if( setlocale(LC_NUMERIC,"C") == NULL )
 //       m_pszOldLocale = NULL;

    return true;
}

void wxGISServer::Exit(void)
{
    //delete network service
    if(m_pNetworkService)
    {
        m_pNetworkService->Stop();
        SetNetworkService(NULL);
        m_pNetworkService = NULL;
    }

    //delete catalog
    if(m_pCatalog)
    {
        SetGxCatalog(NULL);
        m_pCatalog = NULL;
    }
}

bool wxGISServer::CreateApp(void)
{
    //create and init GxCatalog
    m_pCatalog = new wxRxCatalog();
    SetGxCatalog(m_pCatalog);    
    
    if(!m_pCatalog->Init())
    {
        SetGxCatalog(NULL);
        m_pCatalog = NULL;
        return false;
    }

    //create network service
    m_pNetworkService = new wxGISNetworkService(static_cast<INetEventProcessor*>(m_pCatalog));
    SetNetworkService(m_pNetworkService);

    if(!m_pNetworkService->Start())
    {
        SetNetworkService(NULL);
        m_pNetworkService = NULL;
        return false;
    }
    //
    return true;
}

void wxGISServer::OnAppOptions()
{
    wxCmdLineParser pParser;
    pParser.AddSwitch(wxT( "h" ), wxT( "help" ), _( "Show this help message" ), wxCMD_LINE_OPTION_HELP);
    pParser.AddSwitch(wxT( "v" ), wxT( "version" ), _( "The version of this program" ));
    pParser.AddSwitch(wxT( "r" ), wxT("run"), _( "Run the wxGIS Server in standalone mode. Press 'q' to quit." ));
    pParser.AddSwitch(wxT( "i" ), wxT("install"), _( "Install wxGIS Server as service" ));
    pParser.AddSwitch(wxT( "u" ), wxT("uninstall"), _( "Uninstall wxGIS Server service" ));
    pParser.AddSwitch(wxT( "s" ), wxT("start"), _( "Start wxGIS Server service" ));

    pParser.SetLogo(wxString::Format(_("The wxGIS Server (%s)\nAuthor: Bishop (aka Barishnikov Dmitriy), polimax@mail.ru\nCopyright (c) 2010-%d"), wxString(wxGIS_VERSION_NUM_DOT_STRING_T).c_str(), __YEAR__));

    pParser.Usage();
}

void wxGISServer::OnAppAbout(void)
{
    wxCmdLineParser pParser;
    pParser.SetLogo(wxString::Format(_("The wxGIS Server (%s)\nAuthor: Bishop (aka Barishnikov Dmitriy), polimax@mail.ru\nCopyright (c) 2010-%d"), wxString(wxGIS_VERSION_NUM_DOT_STRING_T).c_str(), __YEAR__));

    pParser.Usage();
}

wxString wxGISServer::GetAppName(void) const
{
    return wxString(wxT("wxGISServer"));
}

wxString wxGISServer::GetAppVersionString(void) const
{
    return wxString(wxGIS_VERSION_NUM_DOT_STRING_T);
}

/*static IServerApplication* m_pGlobalApp;
extern WXDLLIMPEXP_GIS_FRW IServerApplication* GetApplication()
{
    return m_pGlobalApp;
}

wxGISServer::wxGISServer(void) : wxGISWorkPlugin(), m_pNetService(NULL), m_pAuthService(NULL), m_pCatalogService(NULL)
{
}

wxGISServer::~wxGISServer(void)
{
}

bool wxGISServer::Start(wxString sAppName, wxString sConfigDir, wxCmdLineParser& parser)
{
	if(!wxGISWorkPlugin::Start(sAppName, sConfigDir, parser))
		return false;

	wxLogDebug(wxT("OnStartMessageThread"));
    bool bResult = OnStartMessageThread();
    if(!bResult)
        return bResult;

	//GDAL
    CPLSetConfigOption( "GDAL_CACHEMAX", "128" );

	OGRRegisterAll();
	GDALAllRegister();
	//END GDAL

	wxLogDebug(wxT("Start workers service"));
    //3. Start workers service
	//3a. Create catalog
	m_pCatalogService = new wxRxCatalog();
    bResult = m_pCatalogService->Start(this, NULL);
    if(!bResult)
        return bResult;

	wxLogDebug(wxT("Start Auth service"));
	//4. Start Auth service
    wxXmlNode* pAuthNode = m_pConfig->GetConfigNode(enumGISHKLM, wxString(wxT("auth")));
    if(pAuthNode == NULL)
        pAuthNode = m_pConfig->CreateConfigNode(enumGISHKLM, wxString(wxT("auth")));
	m_pAuthService = new wxGISAuthService();
    bResult = m_pAuthService->Start(this, pAuthNode);
    if(!bResult)
        return bResult;

    wxXmlNode* pNetworkNode = m_pConfig->GetConfigNode(enumGISHKLM, wxString(wxT("network")));
    if(pNetworkNode == NULL)
        pNetworkNode = m_pConfig->CreateConfigNode(enumGISHKLM, wxString(wxT("network")));
	wxLogDebug(wxT("Start network service"));
    //5. Start network service
    m_pNetService = new wxGISNetworkService();
    bResult = m_pNetService->Start(this, pNetworkNode);
    if(!bResult)
	{
		wxLogError(_("Start network service failed!"));
        return bResult;
	}
    m_pGlobalApp = this;

	wxLogDebug(wxT("Started..."));
	return true;
}

void wxGISServer::Stop(void)
{
    //1. Stop network service
    if(m_pNetService)
        m_pNetService->Stop();
    wxDELETE(m_pNetService);

    //2. Stop auth service
    if(m_pAuthService)
        m_pAuthService->Stop();
    wxDELETE(m_pAuthService);

	OnStopMessageThread();

    //3. Stop workers service
	//3a. Delete catalog
    if(m_pCatalogService)
        m_pCatalogService->Stop();
    wxDELETE(m_pCatalogService);

	//GDAL
	GDALDestroyDriverManager();
	OGRCleanupAll();
	//END GDAL

	wxGISWorkPlugin::Stop();
}

void wxGISServer::PutInMessage(WXGISMSG msg)
{
    m_MsgQueue.push(msg);
}

void wxGISServer::PutOutMessage(WXGISMSG msg)
{
	if(msg.pMsg->IsOk() && m_pNetService)
		m_pNetService->PutOutMessage(msg);
}

IGISConfig* wxGISServer::GetConfig(void)
{
    return m_pConfig;
}

void wxGISServer::SetAuth(AUTHRESPOND stUserInfo)
{
	if(m_pNetService)
		m_pNetService->SetAuth(stUserInfo);
}

AUTHRESPOND wxGISServer::GetAuth(long nID)
{
	AUTHRESPOND respond;
	respond.bIsValid = false;
	if(m_pNetService)
		respond = m_pNetService->GetAuth(nID);
	return respond;
}


bool wxGISServer::SetupSys(wxString sSysPath)
{
	if(!wxGISWorkPlugin::SetupSys(sSysPath))
		return false;

    CPLSetConfigOption("GDAL_DATA", wgWX2MB( (sSysPath + wxFileName::GetPathSeparator() + wxString(wxT("gdal")) + wxFileName::GetPathSeparator()).c_str() ) );
    return true;
}

void wxGISServer::SetDebugMode(bool bDebugMode)
{
	wxGISWorkPlugin::SetDebugMode(bDebugMode);		
	CPLSetConfigOption("CPL_DEBUG", bDebugMode == true ? "ON" : "OFF");
	CPLSetConfigOption("CPL_TIMESTAMP", "ON");
	CPLSetConfigOption("CPL_LOG_ERRORS", bDebugMode == true ? "ON" : "OFF");
}

bool wxGISServer::SetupLog(wxString sLogPath, wxString sNamePrefix)
{
	if(!wxGISWorkPlugin::SetupLog(sLogPath, wxT("srv")))
		return false;

	wxString sCPLLogPath = sLogPath + wxFileName::GetPathSeparator() + wxString(wxT("gdal_log_srv.txt"));
	CPLSetConfigOption("CPL_LOG", wgWX2MB(sCPLLogPath.c_str()) );
    return true;
}
*/