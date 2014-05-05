/******************************************************************************
 * Project:  wxGIS (GIS Map)
 * Purpose:  Main application class.
 * Author:   Bishop (aka Baryshnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 20011 Bishop
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
#include "wxgis/core/init.h"

#include <wx/app.h>

#include <map>

class wxGISMapApp :
	public wxApp,
	public wxGISAppWithLibs
{
public:
	wxGISMapApp(void);
	~wxGISMapApp(void);
	virtual bool OnInit();
};

DECLARE_APP(wxGISMapApp);
