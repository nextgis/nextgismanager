/******************************************************************************
 * Project:  wxGIS (GIS Map)
 * Purpose:  wxMxMapView class.
 * Author:   Bishop (aka Baryshnikov Dmitriy), polimax@mail.ru
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

#include "wxgis/mapui/mapui.h"
#include "wxgis/cartoui/mapview.h"
#include "wxgis/framework/framework.h"


/** \class wxMxMapView mxmapview.h
    \brief This is the map view class in MxApplication.
*/
class WXDLLIMPEXP_GIS_MAPU wxMxMapView :
	public wxGISMapView
{
    DECLARE_DYNAMIC_CLASS(wxMxMapView)
public:
	wxMxMapView(void);
	wxMxMapView(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
						 const wxSize& size = wxDefaultSize);
	virtual ~wxMxMapView(void);
    //events
	void OnMouseMove(wxMouseEvent& event);
	void OnMouseDown(wxMouseEvent& event);
	void OnMouseUp(wxMouseEvent& event);
	void OnMouseDoubleClick(wxMouseEvent& event);
    virtual bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = wxT("GISMapView"));
	virtual bool Activate(IFrameApplication* application, wxXmlNode* pConf = NULL);
	virtual void Deactivate(void);
private:
	IStatusBar* m_pStatusBar;
	IFrameApplication* m_pApp;
	ITrackCancel *m_pTrackCancel;

	DECLARE_EVENT_TABLE()
};