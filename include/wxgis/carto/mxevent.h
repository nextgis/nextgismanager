/******************************************************************************
 * Project:  wxGIS
 * Purpose:  event UI classes special for MapView events.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011,2013 Bishop
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

#include "wxgis/carto/carto.h"
#include "wx/event.h"


class WXDLLIMPEXP_GIS_CRT wxMxMapViewEvent;


wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_GIS_CRT, wxMXMAP_LAYER_CHANGED, wxMxMapViewEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_GIS_CRT, wxMXMAP_LAYER_LOADING, wxMxMapViewEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_GIS_CRT, wxMXMAP_LAYER_DS_CLOSED, wxMxMapViewEvent);

/** \class wxGxMapViewEvent mxeventui.h
    \brief The MapView class event.
*/
class WXDLLIMPEXP_GIS_CRT wxMxMapViewEvent : public wxEvent
{
public:
    wxMxMapViewEvent(wxEventType eventType = wxMXMAP_LAYER_CHANGED, size_t nLayer = 0) : wxEvent(0, eventType)
	{
        m_nLayer = nLayer;
	}
	wxMxMapViewEvent(const wxMxMapViewEvent& event) : wxEvent(event)
	{
        m_nLayer = event.m_nLayer;
	}

	void SetLayerCacheId(size_t nLayer){m_nLayer = nLayer;};
	size_t GetLayerCacheId(void){return m_nLayer;};

    virtual wxEvent *Clone() const { return new wxMxMapViewEvent(*this); }

protected:
    size_t m_nLayer;

private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxMxMapViewEvent)
};

typedef void (wxEvtHandler::*wxMxMapViewEventFunction)(wxMxMapViewEvent&);

#define wxMxMapViewEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxMxMapViewEventFunction, func)

#define EVT_MXMAP_LAYER_CHANGED(func)  wx__DECLARE_EVT0(wxMXMAP_LAYER_CHANGED, wxMxMapViewEventHandler(func))
#define EVT_MXMAP_LAYER_LOADING(func)  wx__DECLARE_EVT0(wxMXMAP_LAYER_LOADING, wxMxMapViewEventHandler(func))
#define EVT_MXMAP_LAYER_DS_CLOSED(func)  wx__DECLARE_EVT0(wxMXMAP_LAYER_DS_CLOSED, wxMxMapViewEventHandler(func))
