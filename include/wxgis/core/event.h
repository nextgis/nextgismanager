/******************************************************************************
 * Project:  wxGIS
 * Purpose:  event classes special for Process events.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
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
#pragma once

#include "wxgis/core/core.h"
#include "wx/event.h"

class WXDLLIMPEXP_FWD_GIS_CORE wxGISProcessEvent;

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_GIS_CORE, wxPROCESS_START, wxGISProcessEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_GIS_CORE, wxPROCESS_FINISH, wxGISProcessEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_GIS_CORE, wxPROCESS_CANCELED, wxGISProcessEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_GIS_CORE, wxPROCESS_STATE_CHANGED, wxGISProcessEvent);

/** \class wxGISProcessEvent gxevent.h
    \brief The wxGISProcess class event.
*/
class WXDLLIMPEXP_GIS_CORE wxGISProcessEvent : public wxEvent
{
public:
    wxGISProcessEvent(wxEventType eventType = wxPROCESS_START, long nProcessID = wxNOT_FOUND, bool bHasErrors = false) : wxEvent(0, eventType), m_nProcessID(nProcessID), m_bHasErrors(bHasErrors)
	{		
	}
	wxGISProcessEvent(const wxGISProcessEvent& event) : wxEvent(event), m_nProcessID(event.m_nProcessID), m_bHasErrors(event.m_bHasErrors)
	{
	}

    void SetProcessID(long nProcessID) { m_nProcessID = nProcessID; }
    long GetProcessID() const { return m_nProcessID; }
    void SetHasErrors(bool bHasErrors) { m_bHasErrors = bHasErrors; }
    bool GetHasErrors() const { return m_bHasErrors; }

    virtual wxEvent *Clone() const { return new wxGISProcessEvent(*this); }

protected:
    long m_nProcessID;
	bool m_bHasErrors;

private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxGISProcessEvent)
};

typedef void (wxEvtHandler::*wxGISProcessEventFunction)(wxGISProcessEvent&);

#define wxGISProcessEventHandler(func) wxEVENT_HANDLER_CAST(wxGISProcessEventFunction, func)

#define EVT_PROCESS_START(func)  wx__DECLARE_EVT0(wxPROCESS_START, wxGISProcessEventHandler(func))
#define EVT_PROCESS_FINISH(func)  wx__DECLARE_EVT0(wxPROCESS_FINISH, wxGISProcessEventHandler(func))
#define EVT_PROCESS_CANCELED(func)  wx__DECLARE_EVT0(wxPROCESS_CANCELED, wxGISProcessEventHandler(func))
#define EVT_PROCESS_STATE_CHANGED(func)  wx__DECLARE_EVT0(wxPROCESS_STATE_CHANGED, wxGISProcessEventHandler(func))

