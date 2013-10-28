/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  network events classes
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2012 Bishop
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

#include "wxgis/net/message.h"

#include "wx/event.h"

class WXDLLIMPEXP_GIS_NET wxGISNetEvent;

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_GIS_NET, wxGISNET_MSG, wxGISNetEvent);

/** \class wxGxCatalogEvent netevent.h
    \brief The Network event.
*/
class WXDLLIMPEXP_GIS_NET wxGISNetEvent : public wxEvent
{
public:
    wxGISNetEvent(int nUserId = 0, wxEventType eventType = wxGISNET_MSG, wxNetMessage msg = wxNetMessage()) : wxEvent(nUserId, eventType)
	{
        m_msg = msg;
	}
	wxGISNetEvent(const wxGISNetEvent& event) : wxEvent(event)
	{
        m_msg = event.m_msg;
	}

    void SetNetMessage(wxNetMessage msg) { m_msg = msg; }
    wxNetMessage GetNetMessage() const { return m_msg; }

    virtual wxEvent *Clone() const { return new wxGISNetEvent(*this); }

protected:
    wxNetMessage m_msg;

private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxGISNetEvent)
};

typedef void (wxEvtHandler::*wxGISNetEventFunction)(wxGISNetEvent&);

#define wxGISNetEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxGISNetEventFunction, func)

#define EVT_GISNET_MSG(func)  wx__DECLARE_EVT0(wxGISNET_MSG, wxGISNetEventHandler(func))

class INetEventProcessor
{
public:
    virtual ~INetEventProcessor(void){};
    virtual void ProcessNetEvent(wxGISNetEvent& event) = 0;
};
