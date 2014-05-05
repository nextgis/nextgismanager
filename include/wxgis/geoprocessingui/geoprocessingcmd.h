/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  Geoprocessing Main Commands class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2011,2013 Bishop
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

#include "wxgis/framework/command.h"
#include "wxgis/catalogui/gxapplication.h"

//#include "wxgis/datasource/featuredataset.h"

/** @class wxGISGeoprocessingCmd

    The main geoprocessing commands: "Export", ...

    @library{gpui}
*/


class WXDLLIMPEXP_GIS_GPU wxGISGeoprocessingCmd :
    public wxGISCommand
{
    DECLARE_DYNAMIC_CLASS(wxGISGeoprocessingCmd)
    
public:
	wxGISGeoprocessingCmd(void);
	virtual ~wxGISGeoprocessingCmd(void);
	//wxGISCommand
	virtual wxIcon GetBitmap(void);
	virtual wxString GetCaption(void);
	virtual wxString GetCategory(void);
	virtual bool GetChecked(void);
	virtual bool GetEnabled(void);
	virtual wxString GetMessage(void);
	virtual wxGISEnumCommandKind GetKind(void);
	virtual void OnClick(void);
	virtual bool OnCreate(wxGISApplicationBase* pApp);
	virtual wxString GetTooltip(void);
	virtual unsigned char GetCount(void);
protected:
	wxGISApplicationBase* m_pApp;
	wxGxApplicationBase* m_pGxApp;
	wxWindow* m_pToolboxView;
	wxIcon m_IconGPMenu, m_IconToolView;
};
