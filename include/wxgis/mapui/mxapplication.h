/******************************************************************************
 * Project:  wxGIS (GIS Map)
 * Purpose:  wxMxApplication code.
 * Author:   Bishop (aka Baryshnikov Dmitriy), polimax@mail.ru
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

#include "wxgis/mapui/mapui.h"
#include "wxgis/mapui/mxmapview.h"
#include "wxgis/framework/applicationex.h"

#include "wx/aui/aui.h"
#include "wx/artprov.h"

/** \class wxMxApplication mxapplication.h
 *   \brief A map application framework class.
 */
class WXDLLIMPEXP_GIS_MAPU wxMxApplication :
	public wxGISApplicationEx,
    public IMxApplication
{
    DECLARE_CLASS(wxMxApplication)
public:	
	wxMxApplication(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE | wxSUNKEN_BORDER);
	virtual ~wxMxApplication(void);
	//IFrameApplication
    virtual wxString GetAppName(void){return wxString(wxT("wxGISMap"));};
    virtual bool Create(void);
	virtual wxIcon GetAppIcon(void){return m_pAppIcon;};
protected:
	wxIcon m_pAppIcon;
	wxMxMapView* m_pMapView;
};
