/******************************************************************************
 * Project:  wxGIS (GIS Server)
 * Purpose:  wxGISService class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011 Bishop
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
#include "wxgissrv/srv_framework/server.h"

class ISysService
{
protected:
	virtual ~ISysService(void){};
    virtual bool Install() = 0;
    virtual bool Uninstall() = 0;
    virtual void Run() = 0;
    virtual void OnStop() = 0;
	virtual long GetExitCode() = 0;
};

#ifdef __WXMSW__ //Win

#define EVMSG_INSTALLED                  0x00000064L
#define EVMSG_REMOVED                    0x00000065L
#define EVMSG_NOTREMOVED                 0x00000066L
#define EVMSG_CTRLHANDLERNOTINSTALLED    0x00000067L
#define EVMSG_FAILEDINIT                 0x00000068L
#define EVMSG_STARTED                    0x00000069L
#define EVMSG_BADREQUEST                 0x0000006AL
#define EVMSG_DEBUG                      0x0000006BL
#define EVMSG_STOPPED                    0x0000006CL

#define SERVICE_CONTROL_USER 128

#include <windows.h>
#include <winnt.h>
#include <stdio.h>
#include <winsvc.h>
//#include "TCHAR.H"

class wxGISNTService : public ISysService
{
public:
    wxGISNTService(wxString sServiceName);
    virtual ~wxGISNTService();
	//IService
 	virtual bool IsInstalled();
    virtual bool Install();
    virtual bool Uninstall();
    virtual void Run();
    virtual void OnStop();
	//wxGISNTService
	virtual bool StartService();
    virtual void LogEvent(WORD wType, DWORD dwID, const TCHAR* pszS1 = NULL, const TCHAR* pszS2 = NULL, const TCHAR* pszS3 = NULL);
    virtual void SetStatus(DWORD dwState);
    virtual bool Initialize();
	virtual bool OnInit();
    virtual void OnInterrogate();
    virtual void OnPause();
    virtual void OnContinue();
    virtual void OnShutdown();
    virtual bool OnUserControl(DWORD dwOpcode);
	virtual long GetExitCode(){return m_Status.dwWin32ExitCode;};

	// static member functions
    static void WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);
    static void WINAPI Handler(DWORD dwOpcode);
protected:

   wxString m_sServiceName;
    int m_iMajorVersion;
    int m_iMinorVersion;
    SERVICE_STATUS_HANDLE m_hServiceStatus;
    SERVICE_STATUS m_Status;
    bool m_bIsRunning;
    static wxGISNTService* m_pThis; // nasty hack to get object ptr
    
	wxGISServer m_Server;
	HANDLE m_hEventSource;
};

#define wxGISService wxGISNTService

#else	//Lin

//#define wxGISService
#endif

