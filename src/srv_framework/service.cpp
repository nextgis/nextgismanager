/******************************************************************************
 * Project:  wxGIS (GIS Server)
 * Purpose:  wxGISService class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011 Bishop
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
/*#include "wxgissrv/srv_app/service.h"

#include "wx/cmdline.h"

#ifdef __WXMSW__ //Win

// static variables
wxGISNTService* wxGISNTService::m_pThis = NULL;

wxGISNTService::wxGISNTService(wxString sServiceName)
{
    // copy the address of the current object so we can access it from
    // the static member callback functions. 
    // WARNING: This limits the application to only one CNTService object. 
    m_pThis = this;
    
    // Set the default service name and version
    m_sServiceName = sServiceName;

    m_iMajorVersion = 1;
    m_iMinorVersion = 0;
    m_hEventSource = NULL;

    // set up the initial service status 
    m_hServiceStatus = NULL;
    m_Status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    m_Status.dwCurrentState = SERVICE_STOPPED;
    m_Status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    m_Status.dwWin32ExitCode = 0;
    m_Status.dwServiceSpecificExitCode = 0;
    m_Status.dwCheckPoint = 0;
    m_Status.dwWaitHint = 0;
    m_bIsRunning = false;
}

wxGISNTService::~wxGISNTService()
{
	if (m_hEventSource)
		::DeregisterEventSource(m_hEventSource);
}

bool wxGISNTService::IsInstalled()
{
    bool bResult(false);

    // Open the Service Control Manager
    SC_HANDLE hSCM = ::OpenSCManager(NULL, // local machine
                                     NULL, // ServicesActive database
                                     SC_MANAGER_ALL_ACCESS); // full access
    if (hSCM)
	{
        // Try to open the service
        SC_HANDLE hService = ::OpenService(hSCM, m_sServiceName, SERVICE_QUERY_CONFIG);
        if (hService)
		{
            bResult = true;
            ::CloseServiceHandle(hService);
        }
        ::CloseServiceHandle(hSCM);
    }    
    return bResult;
}

bool wxGISNTService::Install()
{
	if (IsInstalled()) 
	{
		wxFprintf(stderr, wxString::Format(_("%s is already installed\n"), m_sServiceName.c_str()));
		return false;
	}
	else
	{
		// Open the Service Control Manager
		SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
		if (!hSCM) 
		{
			wxFprintf(stderr, wxString::Format(_("%s is not installed\n"), m_sServiceName.c_str()));
			return false;
		}
		// Get the executable file path
		wxStandardPaths stp;
		wxString sFilePath = stp.GetExecutablePath();

		// Create the service
		SC_HANDLE hService = ::CreateService(hSCM, m_sServiceName, wxT("wxGIS Server"), SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL, sFilePath + wxT(" -s"), NULL, NULL, NULL, NULL, NULL);//SERVICE_DEMAND_START

		if (!hService)
		{
			::CloseServiceHandle(hSCM);
			wxFprintf(stderr, wxString::Format(_("%s is not installed\n"), m_sServiceName.c_str()));
			return false;
		}

		// make registry entries to support logging messages
		// Add the source name as a subkey under the Application
		// key in the EventLog service portion of the registry.
		wxString szKey = wxString::Format(wxT("SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\%s"), m_sServiceName.c_str());
		HKEY hKey = NULL;
		if (::RegCreateKey(HKEY_LOCAL_MACHINE, szKey, &hKey) != ERROR_SUCCESS)
		{
			::CloseServiceHandle(hService);
			::CloseServiceHandle(hSCM);
			wxFprintf(stderr, wxString::Format(_("%s is not installed\n"), m_sServiceName.c_str()));
			return false;
		}

		// Add the Event ID message-file name to the 'EventMessageFile' subkey.
		::RegSetValueEx(hKey, wxT("EventMessageFile"), 0, REG_EXPAND_SZ, (CONST BYTE*)sFilePath.c_str(), sFilePath.Len() + 1);     

		// Set the supported types flags.
		DWORD dwData = EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE | EVENTLOG_INFORMATION_TYPE;
		::RegSetValueEx(hKey, wxT("TypesSupported"), 0, REG_DWORD, (CONST BYTE*)&dwData, sizeof(DWORD));
		::RegCloseKey(hKey);

		LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_INSTALLED, m_sServiceName);

		// tidy up
		::CloseServiceHandle(hService);
		::CloseServiceHandle(hSCM);
		wxFprintf(stdout, wxString::Format(_("%s installed\n"), m_sServiceName.c_str()));
		return true;
	}
}

bool wxGISNTService::Uninstall()
{
	// Open the Service Control Manager
    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (!hSCM) 
	{
		wxFprintf(stderr, wxString::Format(_("Could not remove %s. Error %d\n"), m_sServiceName.c_str(), GetLastError()));
		return false;
	}

    bool bResult(false);
    SC_HANDLE hService = ::OpenService(hSCM, m_sServiceName, DELETE);
    if (hService)
	{
		if (::DeleteService(hService))
		{
            LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_REMOVED, m_sServiceName);
            bResult = true;
        }
		else
		{
            LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_NOTREMOVED, m_sServiceName);
        }
        ::CloseServiceHandle(hService);
    }
    
    ::CloseServiceHandle(hSCM);
 
	if(bResult)
	{
		// Get the executable file path
		wxStandardPaths stp;
		wxString sFilePath = stp.GetExecutablePath();
		wxFprintf(stdout, wxString::Format(_("%s removed. (You must delete the file (%s) yourself.)\n"), m_sServiceName.c_str(), sFilePath.c_str()));	
	}
	return bResult;
}

// This function makes an entry into the application event log
void wxGISNTService::LogEvent(WORD wType, DWORD dwID, const TCHAR* pszS1, const TCHAR* pszS2, const TCHAR* pszS3)
{
    const TCHAR* ps[3];
    ps[0] = pszS1;
    ps[1] = pszS2;
    ps[2] = pszS3;

    int iStr(0);
    for (int i = 0; i < 3; i++)
        if(ps[i] != NULL) iStr++;
        
    // Check the event source has been registered and if
    // not then register it now
    if (!m_hEventSource)
		m_hEventSource = ::RegisterEventSource(NULL, m_sServiceName);

    if (m_hEventSource)
        ::ReportEvent(m_hEventSource, wType, 0, dwID, NULL, iStr, 0, ps, NULL);
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Service startup and registration

bool wxGISNTService::StartService()
{
    SERVICE_TABLE_ENTRY st[] = {
		{(LPWSTR)m_sServiceName.c_str(), ServiceMain},
        {NULL, NULL}
    };

	bool bResult = ::StartServiceCtrlDispatcher(st);
    return bResult;
}

// static member function (callback)
void wxGISNTService::ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv)
{
    // Get a pointer to the C++ object
    wxGISNTService* pService = m_pThis;
    
    // Register the control request handler
    pService->m_Status.dwCurrentState = SERVICE_START_PENDING;
    pService->m_hServiceStatus = RegisterServiceCtrlHandler(pService->m_sServiceName, Handler);
    if (pService->m_hServiceStatus == NULL)
	{
        pService->LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_CTRLHANDLERNOTINSTALLED);
        return;
    }

    // Start the initialisation
    if (pService->Initialize())
	{
		// Do the real work. 
        // When the Run function returns, the service has stopped.
        pService->m_bIsRunning = TRUE;
        pService->m_Status.dwWin32ExitCode = 0;
        pService->m_Status.dwCheckPoint = 0;
        pService->m_Status.dwWaitHint = 0;
        pService->Run();
    }

    // Tell the service manager we are stopped
    pService->SetStatus(SERVICE_STOPPED);
}

///////////////////////////////////////////////////////////////////////////////////////////
// status functions

void wxGISNTService::SetStatus(DWORD dwState)
{
    m_Status.dwCurrentState = dwState;
    ::SetServiceStatus(m_hServiceStatus, &m_Status);
}

///////////////////////////////////////////////////////////////////////////////////////////
// Service initialization

bool wxGISNTService::Initialize()
{
    // Start the initialization
    SetStatus(SERVICE_START_PENDING);
    
    // Perform the actual initialization
    bool bResult = OnInit(); 
    
    // Set final state
    m_Status.dwWin32ExitCode = GetLastError();
    m_Status.dwCheckPoint = 0;
    m_Status.dwWaitHint = 0;
    if (!bResult)
	{
        LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_FAILEDINIT);
        SetStatus(SERVICE_STOPPED);
        return false;    
    }
    
    LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_STARTED);
    SetStatus(SERVICE_RUNNING);

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// main function to do the real work of the service

// This function performs the main work of the service. 
// When this function returns the service has stopped.
void wxGISNTService::Run()
{
	wxCmdLineParser parser;
	if(!m_Server.Start(wxT("wxGISServer"), CONFIG_DIR, parser)) 
	{
        m_Server.Stop();
		return;
	}

    while (m_bIsRunning)
        wxMilliSleep(300);

	m_Server.Stop();
}

//////////////////////////////////////////////////////////////////////////////////////
// Control request handlers

// static member function (callback) to handle commands from the
// service control manager
void wxGISNTService::Handler(DWORD dwOpcode)
{
    // Get a pointer to the object
    wxGISNTService* pService = m_pThis;

	wxLogDebug(wxT("Handler %d"), dwOpcode);

    
    switch (dwOpcode)
	{
    case SERVICE_CONTROL_STOP: // 1
        pService->SetStatus(SERVICE_STOP_PENDING);
        pService->OnStop();
        pService->m_bIsRunning = FALSE;
        pService->LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_STOPPED);
        break;

    case SERVICE_CONTROL_PAUSE: // 2
        pService->OnPause();
        break;

    case SERVICE_CONTROL_CONTINUE: // 3
        pService->OnContinue();
        break;

    case SERVICE_CONTROL_INTERROGATE: // 4
        pService->OnInterrogate();
        break;

    case SERVICE_CONTROL_SHUTDOWN: // 5
        pService->SetStatus(SERVICE_STOP_PENDING);
        pService->OnShutdown();
        pService->m_bIsRunning = FALSE;
        pService->LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_STOPPED);
       break;

    default:
        if (dwOpcode >= SERVICE_CONTROL_USER)
            if (!pService->OnUserControl(dwOpcode))
                pService->LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_BADREQUEST);
        else
			pService->LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_BADREQUEST);
        break;
    }
    ::SetServiceStatus(pService->m_hServiceStatus, &pService->m_Status);
}
        
// Called when the service is first initialized
bool wxGISNTService::OnInit()
{
	return true;
}

// Called when the service control manager wants to stop the service
void wxGISNTService::OnStop()
{
}

// called when the service is interrogated
void wxGISNTService::OnInterrogate()
{
}

// called when the service is paused
void wxGISNTService::OnPause()
{
}

// called when the service is continued
void wxGISNTService::OnContinue()
{
}

// called when the service is shut down
void wxGISNTService::OnShutdown()
{
}

// called when the service gets a user control message
bool wxGISNTService::OnUserControl(DWORD dwOpcode)
{
    return false; // say not handled
}

#else	//Lin


#endif
*/
