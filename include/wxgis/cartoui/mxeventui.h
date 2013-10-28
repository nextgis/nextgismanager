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

#include "wxgis/cartoui/cartoui.h"
#include "wx/event.h"


class WXDLLIMPEXP_FWD_GIS_CTU wxMxMapViewUIEvent;

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_GIS_CTU, wxMXMAP_ROTATED, wxMxMapViewUIEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_GIS_CTU, wxMXMAP_DRAWING_START, wxMxMapViewUIEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_GIS_CTU, wxMXMAP_DRAWING_STOP, wxMxMapViewUIEvent);

//wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_GIS_CTU, wxMXMAP_REFRESHED, wxGxMapViewEvent);
 //AfterDraw Fired after the specified phase is drawn. 
 //AfterItemDraw Fired after an individual view item is drawn. Example: view items include layers in a map or elements in a page layout. 
 //ContentsChanged Fired when the contents of the view changes. 
 //ContentsCleared Fired when the contents of the view is cleared. 
 //FocusMapChanged Fired when a new map is made active. 
 //ItemAdded Fired when an item is added to the view. 
 //ItemDeleted Fired when an item is deleted from the view. 
 //ItemReordered Fired when a view item is reordered. 
 //SelectionChanged Call this function to fire the selection changed event. 
 //SpatialReferenceChanged Fired when the spatial reference is changed. 
 //ViewRefreshed Fired when view is refreshed before draw happens. 


/** \class wxGxMapViewEvent mxeventui.h
    \brief The MapView class event.
*/
class WXDLLIMPEXP_GIS_CTU wxMxMapViewUIEvent : public wxEvent
{
public:
    wxMxMapViewUIEvent(int nWinId = 0, wxEventType eventType = wxMXMAP_ROTATED, double dAngleRad = 0) : wxEvent(nWinId, eventType)
	{
        m_dRotation = dAngleRad;
	}
	wxMxMapViewUIEvent(const wxMxMapViewUIEvent& event) : wxEvent(event)
	{
        m_dRotation = event.m_dRotation;
	}

	void SetRotate(double dAngleRad){m_dRotation = dAngleRad;};
	double GetRotate(void){return m_dRotation;};

    virtual wxEvent *Clone() const { return new wxMxMapViewUIEvent(*this); }

protected:
    double m_dRotation;

private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxMxMapViewUIEvent)
};

typedef void (wxEvtHandler::*wxMxMapViewUIEventFunction)(wxMxMapViewUIEvent&);

#define wxMxMapViewUIEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxMxMapViewUIEventFunction, func)

#define EVT_MXMAP_ROTATED(func)  wx__DECLARE_EVT0(wxMXMAP_ROTATED, wxMxMapViewUIEventHandler(func))
#define EVT_MXMAP_DRAWING_START(func)  wx__DECLARE_EVT0(wxMXMAP_DRAWING_START, wxMxMapViewUIEventHandler(func))
#define EVT_MXMAP_DRAWING_STOP(func)  wx__DECLARE_EVT0(wxMXMAP_DRAWING_STOP, wxMxMapViewUIEventHandler(func))
