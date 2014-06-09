/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  event classes special for GxCatalog events.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011-2012 Dmitry Baryshnikov
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

#include "wxgis/catalog/catalog.h"
#include "wx/event.h"

class WXDLLIMPEXP_FWD_GIS_CLT wxGxCatalogEvent;

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_GIS_CLT, wxGXOBJECT_ADDED, wxGxCatalogEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_GIS_CLT, wxGXOBJECT_CHANGED, wxGxCatalogEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_GIS_CLT, wxGXOBJECT_DELETED, wxGxCatalogEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_GIS_CLT, wxGXOBJECT_REFRESHED, wxGxCatalogEvent);

/** @class wxGxCatalogEvent

    The GxCatalog class event.

    @library{catalog}
*/
class WXDLLIMPEXP_GIS_CLT wxGxCatalogEvent : public wxEvent
{
public:
    wxGxCatalogEvent(wxEventType eventType = wxGXOBJECT_ADDED, long nObjectID = wxNOT_FOUND) : wxEvent(0, eventType)
	{
        m_nObjectID = nObjectID;
	}

    wxGxCatalogEvent(const wxGxCatalogEvent& event) : wxEvent(event)
	{
        m_nObjectID = event.m_nObjectID;
	}

    void SetObjectID(long nObjectID) { m_nObjectID = nObjectID; }
    long GetObjectID() const { return m_nObjectID; }

    virtual wxEvent *Clone() const { return new wxGxCatalogEvent(*this); }

protected:
    long m_nObjectID;

private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxGxCatalogEvent)
};

typedef void (wxEvtHandler::*wxGxCatalogEventFunction)(wxGxCatalogEvent&);

#define wxGxCatalogEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxGxCatalogEventFunction, func)

#define EVT_GXOBJECT_ADDED(func)  wx__DECLARE_EVT0(wxGXOBJECT_ADDED, wxGxCatalogEventHandler(func))
#define EVT_GXOBJECT_CHANGED(func)  wx__DECLARE_EVT0(wxGXOBJECT_CHANGED, wxGxCatalogEventHandler(func))
#define EVT_GXOBJECT_DELETED(func)  wx__DECLARE_EVT0(wxGXOBJECT_DELETED, wxGxCatalogEventHandler(func))
#define EVT_GXOBJECT_REFRESHED(func)  wx__DECLARE_EVT0(wxGXOBJECT_REFRESHED, wxGxCatalogEventHandler(func))

