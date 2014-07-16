/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Main application class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2012,2013 Dmitry Baryshnikov
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
#include "wxgis/cat_app/catalogframe.h"

#include <wx/app.h>

/** \class wxGISCatalogApp catalogapp.h
    \brief Main Catalog class.
*/
class wxGISCatalogApp :
	public wxApp,
	public wxGISAppWithLibs
{
public:
	wxGISCatalogApp(void);
	virtual ~wxGISCatalogApp(void);
	virtual bool OnInit();
    virtual int OnExit();
	virtual void OnEventLoopEnter(wxEventLoopBase* loop);
protected:
    wxGISCatalogFrame* m_pMainFrame;
    bool m_bMainFrameCreated;
};

wxDECLARE_APP(wxGISCatalogApp);


