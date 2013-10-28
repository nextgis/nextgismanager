/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  GxPostGISDatasetUI classes.
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

#include "wxgisdefs.h"
#include "wxgis/catalogui/catalogui.h"

#ifdef wxGIS_USE_POSTGRES

#include "wxgis/catalog/gxpostgisdataset.h"

/** \class wxGxPostGISTableDatasetUI gxpostgisdatasetui.h
    \brief A PostGIS Table Dataset GxObjectUI.
*/

class WXDLLIMPEXP_GIS_CLU wxGxPostGISTableDatasetUI :
	public wxGxPostGISTableDataset,
	public IGxObjectUI,
	public IGxObjectEditUI
{
    DECLARE_CLASS(wxGxPostGISTableDatasetUI)
public:
	wxGxPostGISTableDatasetUI(const wxString &sSchema, wxGISPostgresDataSource* pwxGISRemoteConn, wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "", const wxIcon &LargeIcon = wxNullIcon, const wxIcon &SmallIcon = wxNullIcon);
	virtual ~wxGxPostGISTableDatasetUI(void);
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void) const {return wxString(wxT("wxGxPostGISTableDataset.ContextMenu"));};
	virtual wxString NewMenu(void) const {return wxEmptyString;};
	//IGxObjectEditUI
	virtual void EditProperties(wxWindow *parent);
protected:
    wxIcon m_LargeIcon, m_SmallIcon;
};

/** \class wxGxPostGISFeatureDatasetUI gxpostgisdatasetui.h
    \brief A PostGIS Table Dataset GxObjectUI.
*/

class WXDLLIMPEXP_GIS_CLU wxGxPostGISFeatureDatasetUI :
	public wxGxPostGISFeatureDataset,
	public IGxObjectUI,
	public IGxObjectEditUI
{
    DECLARE_CLASS(wxGxPostGISFeatureDatasetUI)
public:
	wxGxPostGISFeatureDatasetUI(const wxString &sSchema, wxGISPostgresDataSource* pwxGISRemoteConn, wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "", const wxIcon &LargeIcon = wxNullIcon, const wxIcon &SmallIcon = wxNullIcon);
	virtual ~wxGxPostGISFeatureDatasetUI(void);
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void) const {return wxString(wxT("wxGxPostGISFeatureDataset.ContextMenu"));};
	virtual wxString NewMenu(void) const {return wxEmptyString;};
	//IGxObjectEditUI
	virtual void EditProperties(wxWindow *parent);
protected:
    wxIcon m_LargeIcon, m_SmallIcon;
};

#endif //wxGIS_USE_POSTGRES


