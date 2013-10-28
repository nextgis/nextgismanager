/******************************************************************************
 * Project:  wxGIS (GIS Map)
 * Purpose:  Main frame class.
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

#include "wxgis/framework/framework.h"
#include "wxgis/mapui/mxapplication.h"

/** \class wxGISMapFrame mapframe.h
    \brief The wxGISMapFrame class is a main frame of wxGISMap.
*/
class wxGISMapFrame :
	public wxMxApplication
{
public:
	wxGISMapFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE | wxSUNKEN_BORDER);//| wxWS_EX_VALIDATE_RECURSIVELY
	~wxGISMapFrame(void);
//IFrameApplication
	virtual void OnAppAbout(void);

	DECLARE_EVENT_TABLE()
};

