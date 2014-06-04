/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISTaskManager - class to communicate with task manager server
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2012-2013 Bishop
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

#include "wxgis/net/netfactory.h"
#include "wxgis/net/netevent.h"

class WXDLLIMPEXP_GIS_GP wxGISTaskManagerEvent;

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_GIS_GP, wxGISTASKMNGR_CONNECT, wxGISTaskManagerEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_GIS_GP, wxGISTASKMNGR_DISCONNECT, wxGISTaskManagerEvent);

/** \class wxGISTaskEvent task.h
    \brief The Task event.
*/
class WXDLLIMPEXP_GIS_GP wxGISTaskManagerEvent : public wxEvent
{
public:
    wxGISTaskManagerEvent(wxEventType eventType = wxGISTASKMNGR_CONNECT) : wxEvent(wxNOT_FOUND, eventType)
	{
	}
	//wxGISTaskManagerEvent(const wxGISTaskManagerEvent& event) : wxEvent(event)
	//{
	//}
    virtual wxEvent *Clone() const { return new wxGISTaskManagerEvent(*this); }
private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxGISTaskManagerEvent)
};

typedef void (wxEvtHandler::*wxGISTaskManagerEventFunction)(wxGISTaskManagerEvent&);

#define wxGISTaskManagerEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxGISTaskManagerEventFunction, func)

#define EVT_GISTASKMNGR_CONNECT(func)  wx__DECLARE_EVT0(wxGISTASKMNGR_CONNECT, wxGISTaskManagerEventHandler(func))
#define EVT_GISTASKMNGR_DISCONNECT(func)  wx__DECLARE_EVT0(wxGISTASKMNGR_DISCONNECT, wxGISTaskManagerEventHandler(func))


/** \class wxGISLocalClientConnection tskmngr.h
    \brief The connection to communicate with local Task Manager Server.
*/

class wxGISLocalClientConnection :
	public INetConnection
{
    DECLARE_CLASS(wxGISLocalClientConnection)
    enum{
        // id for sockets
        SOCKET_ID = 111
    };
public:
	wxGISLocalClientConnection(void);
	virtual ~wxGISLocalClientConnection(void);
	//INetConnection
	virtual bool Connect(void);
	virtual bool Disconnect(void);
    virtual wxString GetLastError(void) const;
protected:
    //events
    virtual void OnSocketEvent(wxSocketEvent& event);
private:
    DECLARE_EVENT_TABLE()
};

class wxGISTaskCategory;
WX_DECLARE_STRING_HASH_MAP(wxGISTaskCategory*, wxGISTaskCategoryMap);

/** \class wxGISTaskManager tskmngr.h
    \brief Task Manager Server communicate class.
*/
class WXDLLIMPEXP_GIS_GP wxGISTaskManager :
    public wxEvtHandler,
	public wxGISConnectionPointContainer,
    public wxGISPointer
{
    DECLARE_CLASS(wxGISTaskManager)
    enum
    {
        TIMER_ID = 1015
    };
    friend wxGISTaskCategory;
public:
    wxGISTaskManager(void);
    virtual ~wxGISTaskManager(void);
    virtual wxGISTaskCategory* const GetCategory(const wxString& sName);
    virtual bool CreateCategory(const wxString& sName);
    virtual bool DeleteCategory(const wxString& sName);
    virtual wxString GetLastError(void) const;
    virtual bool SetMaxTaskExec(int nMaxExecTaskCount);
    virtual bool SetExecState(wxGISNetCommandState eExitState);
    virtual bool IsValid(void) const;//the manager app is started and connected via tcp/ip
protected:
    virtual void StartTaskManagerServer(void);
    virtual void ClearCategories(void);
    virtual void DeleteCategory(wxGISTaskCategory* pCategory);
    virtual void NetNote(const wxNetMessage &msg);
    virtual void NetCommand(const wxNetMessage &msg);
    virtual void FillDetails(const wxJSONValue &val);
    virtual void SendNetMessageAsync(const wxNetMessage& msg);
    virtual wxNetMessage SendNetMessageSync(const wxNetMessage& msg);
    //events
    virtual void OnGISNetEvent(wxGISNetEvent & event);
    virtual void OnTimer( wxTimerEvent & event);
protected:
    wxGISTaskCategoryMap m_omCategories;
    wxGISLocalClientConnection* m_pConn;
    long m_nConnectionPointConnCookie;
    wxTimer m_timer;
    int m_nMaxExecTasks;
    wxGISNetCommandState m_eExitState;
    bool m_bDetailesFilled;
    wxString m_sLastError;
private:
    DECLARE_EVENT_TABLE()
};

/** \fn wxGISTaskManager* const GetTaskManager(void)
 *  \brief Global wxGISTaskManager getter.
 */

WXDLLIMPEXP_GIS_GP wxGISTaskManager* const GetTaskManager(void);
