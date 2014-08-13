/******************************************************************************
 * Project:  wxGIS
 * Purpose:  Initializer class for logs, locale, libs and etc.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2011 Dmitry Baryshnikov
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

#include "wxgis/core/init.h"

#include "wxgis/core/config.h"
#include "wxgis/version.h"

#include <wx/config.h>

#ifdef _WIN32
    #include <process.h>
#endif //_WIN32

//-----------------------------------------------------------------------------
// wxGISAppWithLibs
//-----------------------------------------------------------------------------

wxGISAppWithLibs::wxGISAppWithLibs(void)
{
}

wxGISAppWithLibs::~wxGISAppWithLibs(void)
{
}


void wxGISAppWithLibs::LoadLibs(const wxXmlNode* pRootNode)
{
	wxXmlNode *child = pRootNode->GetChildren();
	while(child)
	{
		wxString sPath = child->GetAttribute(wxT("path"), wxEmptyString);
        if(!sPath.IsEmpty())
		{
			//check for doubles
			if(m_LibMap[sPath] != NULL)
			{
				child = child->GetNext();
				continue;
			}

			LoadLib(sPath);
		}
		child = child->GetNext();
	}
}

void wxGISAppWithLibs::LoadLib(const wxString &sPath, bool bStore)
{
	wxDynamicLibrary* pLib = new wxDynamicLibrary(sPath);
	if(pLib != NULL && pLib->IsLoaded())
	{
		wxLogMessage(_("wxGISAppWithLibs: Library %s loaded"), sPath.c_str());
        if(!bStore)
            m_asNoStore.Add(sPath);
		m_LibMap[sPath] = pLib;
	}
	else
		wxLogError(_("wxGISAppWithLibs: Error loading library %s"), sPath.c_str());
}

void wxGISAppWithLibs::SerializeLibs()
{
    //TODO: Think about need of this
	wxGISAppConfig oConfig = GetConfig();
    if(!oConfig.IsOk())
		return;
	wxXmlNode* pLibsNode(NULL);
	pLibsNode = oConfig.GetConfigNode(enumGISHKCU, wxString(wxT("wxGISCommon/libs")));
	if(pLibsNode)
		oConfig.DeleteNodeChildren(pLibsNode);
	else
		pLibsNode = oConfig.CreateConfigNode(enumGISHKCU, wxString(wxT("wxGISCommon/libs")));

	if(!pLibsNode)
		return;

    for(LIBMAP::iterator item = m_LibMap.begin(); item != m_LibMap.end(); ++item)
	{
        if(m_asNoStore.Index(item->first, false) != wxNOT_FOUND)
            continue;
		wxXmlNode* pNewNode = new wxXmlNode(pLibsNode, wxXML_ELEMENT_NODE, wxString(wxT("lib")));
		pNewNode->AddAttribute(wxT("path"), item->first);

		wxFileName FName(item->first);
		wxString sName = FName.GetName();

		pNewNode->AddAttribute(wxT("name"), sName);
	}
}


void wxGISAppWithLibs::UnLoadLibs()
{
    for(LIBMAP::iterator item = m_LibMap.begin(); item != m_LibMap.end(); ++item)
		wxDELETE(item->second);
}

//-----------------------------------------------------------------------------
// wxGISInitializer
//-----------------------------------------------------------------------------

wxGISInitializer::wxGISInitializer(void) : m_pLocale(NULL), m_sDecimalPoint(wxT("."))
{
}

wxGISInitializer::~wxGISInitializer(void)
{
}

bool wxGISInitializer::Initialize(const wxString &sAppName, const wxString &sLogFilePrefix)
{
	wxGISAppConfig oConfig = GetConfig();
    if(!oConfig.IsOk())
        return false;

	wxString sLogDir = oConfig.GetLogDir();
	if(!SetupLog(sLogDir, sLogFilePrefix))
        return false;
    oConfig.ReportPaths();
	wxLogMessage(_("wxGISInitializer: %s %s is initializing..."), sAppName.c_str(), wxString(wxGIS_VERSION_NUM_DOT_STRING_T).c_str());

	if(!SetupLoc(oConfig.GetLocale(), oConfig.GetLocaleDir()))
        return false;

	//setup sys dir
	wxString sSysDir = oConfig.GetSysDir();
	if(!SetupSys(sSysDir))
        return false;

	bool bDebugMode = oConfig.GetDebugMode();
	SetDebugMode(bDebugMode);

	//store values
	//m_pConfig->SetLogDir(sLogDir);
	//m_pConfig->SetLocale(sLocale);
	//m_pConfig->SetLocaleDir(sLocaleDir);
	//m_pConfig->SetSysDir(sSysDir);
	//m_pConfig->SetDebugMode(bDebugMode);

    //load libs
	wxXmlNode* pLibsNode = oConfig.GetConfigNode(enumGISHKCU, wxString(wxT("wxGISCommon/libs")));
	if(pLibsNode)
		LoadLibs(pLibsNode);
	pLibsNode = oConfig.GetConfigNode(enumGISHKLM, wxString(wxT("wxGISCommon/libs")));
	if(pLibsNode)
		LoadLibs(pLibsNode);

    if(!GetApplication())
        SetApplication(this);

	return true;
}

void wxGISInitializer::Uninitialize()
{
    wxDELETE(m_pLocale);
	UnLoadLibs();
}

bool wxGISInitializer::SetupSys(const wxString &sSysPath)
{
	if(!wxDirExists(sSysPath))
	{
		wxLogError(wxString::Format(_("wxGISInitializer: System dir is absent! Lookup path '%s'"), sSysPath.c_str()));
		return false;
	}
    return true;
}

void wxGISInitializer::SetDebugMode(bool bDebugMode)
{
}

bool wxGISInitializer::SetupLog(const wxString &sLogPath, const wxString &sNamePrefix)
{
	if(sLogPath.IsEmpty())
	{
		wxLogError(_("wxGISInitializer: Failed to get log dir"));
        return false;
	}
	if(!wxDirExists(sLogPath))
		wxFileName::Mkdir(sLogPath, 0777, wxPATH_MKDIR_FULL);

	wxDateTime dt(wxDateTime::Now());
    wxString logfilename = sLogPath + wxFileName::GetPathSeparator() + wxString::Format(wxT("%slog_%.4d%.2d%.2d.log"), sNamePrefix.c_str(), dt.GetYear(), dt.GetMonth() + 1, dt.GetDay());

	if(!m_LogFile.Open(logfilename.GetData(), wxT("a+")))
		wxLogError(_("wxGISInitializer: Failed to open log file %s"), logfilename.c_str());

	delete wxLog::SetActiveTarget(new wxLogStderr(m_LogFile.fp()));

#ifdef wxGIS_PORTABLE
	wxLogMessage(wxT("Portable"));
#endif
	wxLogMessage(wxT(" "));
	wxLogMessage(wxT("####################################################################"));
	wxLogMessage(wxT("##                    %s                    ##"),wxNow().c_str());
	wxLogMessage(wxT("####################################################################"));
    const wxLongLong nSize = wxGetFreeMemory();
    wxULongLong nuSize;
    nuSize.operator=(nSize);
    const wxString sSize = wxFileName::GetHumanReadableSize(nuSize);
	wxLogMessage(_("HOST '%s': OS desc - %s, free memory - %s"), wxGetFullHostName().c_str(), wxGetOsDescription().c_str(), sSize.c_str());
	wxLogMessage(_("wxGISInitializer: Log file: %s"), logfilename.c_str());


    return true;
}

bool wxGISInitializer::SetupLoc(const wxString &sLoc, const wxString &sLocPath)
{
    wxLogMessage(_("wxGISInitializer: Initialize locale"));

    wxDELETE(m_pLocale);

	//init locale
    if ( !sLoc.IsEmpty() )
    {
		int iLocale(0);
		const wxLanguageInfo* loc_info = wxLocale::FindLanguageInfo(sLoc);
		if(loc_info != NULL)
		{
			iLocale = loc_info->Language;
			wxLogMessage(_("wxGISInitializer: Language is set to %s"), loc_info->Description.c_str());
            wxLogMessage(_("wxGISInitializer: Language locale files path '%s'"), sLocPath.c_str());

		}

        // don't use wxLOCALE_LOAD_DEFAULT flag so that Init() doesn't return
        // false just because it failed to load wxstd catalog

        m_pLocale = new wxLocale();
        if ( !m_pLocale->Init(iLocale) )
        {
            wxLogError(wxT("wxGISInitializer: This language is not supported by the system."));
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
        wxFileName oParent(sLocalePath);
        wxLocale::AddCatalogLookupPathPrefix(oParent.GetPath());

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

    m_sDecimalPoint = wxLocale::GetInfo(wxLOCALE_DECIMAL_POINT, wxLOCALE_CAT_NUMBER);

    return true;
}

//-----------------------------------------------------------------------------
// wxGISService
//-----------------------------------------------------------------------------
wxGISService* wxGISService::m_pThis = NULL;

wxGISService::wxGISService()
{
    m_pThis = this;

    m_hEventSource = 0;
    m_bServiceIsRunning = false;
    m_nMinLogLevel = LOG_WARNING;
}

wxGISService::~wxGISService()
{
#ifdef _WIN32
    if (m_hEventSource)
    {
        DeregisterEventSource(m_hEventSource);
    }
#endif // _WIN32
}

void wxGISService::LogMessage(wxString msg, int level)
{
#ifdef _WIN32
    if (m_hEventSource)
    {
        LPCTSTR *tmp;

        tmp = (LPCTSTR *)malloc(sizeof(LPCTSTR));
        tmp[0] = _wcsdup(msg.wc_str());

        switch (level)
        {
        case LOG_DEBUG:
            if (m_nMinLogLevel >= LOG_DEBUG)
                ReportEvent(m_hEventSource, EVENTLOG_INFORMATION_TYPE, 0, 0, NULL, 1, 0, tmp, NULL);
            break;

        case LOG_WARNING:
            if (m_nMinLogLevel >= LOG_WARNING)
                ReportEvent(m_hEventSource, EVENTLOG_WARNING_TYPE, 0, 0, NULL, 1, 0, tmp, NULL);
            break;

        case LOG_ERROR:
            ReportEvent(m_hEventSource, EVENTLOG_ERROR_TYPE, 0, 0, NULL, 1, 0, tmp, NULL);

            break;

            // Log startup/connection warnings (valid for any log level)
        case LOG_STARTUP:
            ReportEvent(m_hEventSource, EVENTLOG_WARNING_TYPE, 0, 0, NULL, 1, 0, tmp, NULL);
            break;
        }
    }
#endif//_WIN32

    switch (level)
    {
    case LOG_DEBUG:
        wxLogDebug(msg);
        break;
    case LOG_WARNING:
        wxLogMessage(msg);
        break;
    case LOG_ERROR:
        wxLogError(msg);
        break;
    case LOG_STARTUP:
        wxLogMessage(msg);
        break;
    };
}

#ifdef _WIN32

bool wxGISService::StartService()
{
    SERVICE_TABLE_ENTRY st[] = {
        { m_sServiceName.wchar_str(), ServiceMain },
        { NULL, NULL }
    };

    return StartServiceCtrlDispatcher(st) == TRUE ? true : false;
}

void wxGISService::ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv)
{
    // Get a pointer to the C++ object
    wxGISService* pService = m_pThis;

    // Register the control request handler
    pService->m_ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
    pService->m_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    pService->m_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE;
    pService->m_ServiceStatus.dwWin32ExitCode = 0;
    pService->m_ServiceStatus.dwCheckPoint = 0;
    pService->m_ServiceStatus.dwWaitHint = 15000;
    pService->m_hServiceStatusHandle = RegisterServiceCtrlHandler(pService->m_sServiceName.c_str(), Handler);
    if (pService->m_hServiceStatusHandle == NULL)
    {
        pService->LogMessage(_("Register service handler failed"), LOG_ERROR);
        return;
    }

    // Start the initialisation
    if (pService->Initialize())
    {
        // Do the real work.
        // When the Run function returns, the service has stopped.
        pService->m_bServiceIsRunning = true;
        pService->m_ServiceStatus.dwWaitHint = 1000;
        pService->SetStatus(SERVICE_RUNNING);
        pService->LogMessage(_("Service started successfuly"), LOG_STARTUP);
        pService->Run();
    }

    // Tell the service manager we are stopped
    pService->SetStatus(SERVICE_STOPPED);

    wxExit();
}

void wxGISService::Handler(DWORD dwOpcode)
{
    // Get a pointer to the C++ object
    wxGISService* pService = m_pThis;

    switch (dwOpcode)
    {
    case SERVICE_CONTROL_STOP: // 1
        pService->m_ServiceStatus.dwCheckPoint++;
        pService->SetStatus(SERVICE_STOP_PENDING);
        pService->OnStop();
        pService->m_bServiceIsRunning = false;
        pService->LogMessage(_("Service stopped"), LOG_STARTUP);
        break;

    case SERVICE_CONTROL_PAUSE: // 2
        pService->SetStatus(SERVICE_PAUSE_PENDING);
        pService->OnPause();
        pService->m_bServiceIsRunning = true;// false;
        pService->LogMessage(_("Service paused"), LOG_STARTUP);
        break;

    case SERVICE_CONTROL_CONTINUE: // 3
        pService->SetStatus(SERVICE_CONTINUE_PENDING);
        pService->OnContinue();
        pService->m_bServiceIsRunning = true;
        pService->LogMessage(_("Service continue"), LOG_STARTUP);
        break;

    case SERVICE_CONTROL_INTERROGATE: // 4
        pService->OnInterrogate();
        break;

    case SERVICE_CONTROL_SHUTDOWN: // 5
        pService->OnShutdown();
        break;

    default:
        pService->LogMessage(_("Bad request"), LOG_ERROR);
        break;
    }

    SetServiceStatus(pService->m_hServiceStatusHandle, &pService->m_ServiceStatus);
}

void wxGISService::SetStatus(DWORD dwState)
{
    m_ServiceStatus.dwCurrentState = dwState;
    SetServiceStatus(m_hServiceStatusHandle, &m_ServiceStatus);
}

bool wxGISService::IsInstalled()
{
    bool bResult = false;

    SC_HANDLE hSCM = OpenSCManager(
        NULL, // local machine
        NULL, // ServicesActive database
        SC_MANAGER_ALL_ACCESS); // full access
    if (hSCM)
    {
        // Try to open the service
        SC_HANDLE hService = OpenService(hSCM, m_sServiceName.c_str(), SERVICE_QUERY_CONFIG);
        if (hService)
        {
            bResult = true;
            CloseServiceHandle(hService);
        }
        CloseServiceHandle(hSCM);
    }

    return bResult;
}

bool wxGISService::Install(const wxString &args, const wxString &user, const wxString &password)
{
    bool bResult = false;
    SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
    if (!hSCM)
    {
        return bResult;
    }

    wxString sPath = wxStandardPaths::Get().GetExecutablePath();
    wxString sCmd = sPath + wxT(" ") + args;
    SC_HANDLE hService = NULL;
    if (user.IsEmpty())
    {
        hService = CreateService(hSCM, m_sServiceName.c_str(), m_sServiceDisplayName.c_str(), SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL, sCmd.c_str(), NULL, NULL, NULL, NULL, NULL);
    }
    else
    {
        wxString quser;
        if (!user.Contains(wxT("\\")))
            quser = wxT(".\\") + user;
        else
            quser = user;

        // Create the service
        hService = CreateService(hSCM, m_sServiceName.c_str(), m_sServiceDisplayName.c_str(), SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL, sCmd.c_str(), NULL, NULL, NULL, quser.c_str(), password.c_str());
    }
    if (hService)
    {
        CloseServiceHandle(hService);
        bResult = true;
    }
    else
    {
        WCHAR buffer[1024];
        DWORD dw = GetLastError();

        FormatMessage(
            FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            dw,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            buffer,
            1024, NULL
            );
        wxString error(buffer);
        LogMessage(error, LOG_ERROR);
    }

    CloseServiceHandle(hSCM);

    if (bResult)
    {
        // Setup the event message DLL
        wxRegKey *msgKey = new wxRegKey(wxT("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\") + m_sServiceName);
        if (!msgKey->Exists())
        {
            if (!msgKey->Create())
                LogMessage(_("Could not open the message source registry key."), LOG_WARNING);
        }

        if (!msgKey->SetValue(wxT("EventMessageFile"), sPath))
            LogMessage(_("Could not set the event message file registry value."), LOG_WARNING);

        DWORD dwData = EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE | EVENTLOG_INFORMATION_TYPE;

        if (!msgKey->SetValue(wxT("TypesSupported"), dwData))
            LogMessage(_("Could not set the supported types."), LOG_WARNING);

        LogMessage(_("Service installed"), LOG_STARTUP);

        //add to config

        wxGISAppConfig oConfig = GetConfig();
        if (oConfig.IsOk())
        {
            if (oConfig.Write(enumGISHKLM, wxString(wxT("wxGISCommon/taskmngr/is_service")), true))
            {
                oConfig.Save();
            }
        }
    }
    return bResult;
}

bool wxGISService::Uninstall()
{
    // Open the Service Control Manager
    SC_HANDLE hSCM = OpenSCManager( NULL, NULL, SC_MANAGER_CONNECT);
    if (!hSCM)
    {
        return false;
    }

    bool bResult = false;
    SC_HANDLE hService = OpenService(hSCM, m_sServiceName.c_str(), SERVICE_QUERY_STATUS | DELETE);

    if (hService) {
        SERVICE_STATUS serviceStatus;
        ControlService(hService, SERVICE_CONTROL_STOP, &serviceStatus);

        int retries;
        for (retries = 0; retries < 5; retries++)
        {
            if (QueryServiceStatus(hService, &serviceStatus))
            {
                if (serviceStatus.dwCurrentState == SERVICE_STOPPED)
                {
                    DeleteService(hService);
                    LogMessage(_("Service uninstalled"), LOG_STARTUP);
                    bResult = true;
                    break;
                }
                Sleep(1000L);
            }
        }
        CloseServiceHandle(hService);
    }

    CloseServiceHandle(hSCM);

    // Remove the event message DLL
    wxRegKey *msgKey = new wxRegKey(wxT("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\") + m_sServiceName);
    msgKey->DeleteSelf();

    wxGISAppConfig oConfig = GetConfig();
    if (oConfig.IsOk())
    {
        if(oConfig.Write(enumGISHKLM, wxString(wxT("wxGISCommon/taskmngr/is_service")), false))
        {
            oConfig.Save();
        }
    }


    return bResult;
}

void wxGISService::OnStop()
{
    m_ServiceStatus.dwCheckPoint = 0;
    m_bServiceIsRunning = false;
}

void wxGISService::OnPause()
{
    SetStatus(SERVICE_PAUSED);
}

void wxGISService::OnContinue()
{
    SetStatus(SERVICE_RUNNING);
}

void wxGISService::OnInterrogate()
{
    SetStatus(SERVICE_INTERROGATE);
}

void wxGISService::OnShutdown()
{
}

#else //_WIN32
void wxGISService::Daemonize(void)
{
    pid_t pid;

    pid = fork();
    if (pid == (pid_t)-1)
    {
        LogMessage(_("Cannot disassociate from controlling TTY"), LOG_ERROR);
        exit(1);
    }
    else if (pid)
        exit(0);

#ifdef HAVE_SETSID
    if (setsid() < 0)
    {
        LogMessage(_("Cannot disassociate from controlling TTY"), LOG_ERROR);
        exit(1);
    }
#endif

}
#endif //_WIN32
