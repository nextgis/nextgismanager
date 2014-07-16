/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  local db (sqlite, gdab, mdb) ui classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
 *   Copyright (C) 2014 Dmitry Baryshnikov
 *   Copyright (C) 2014 NextGIS
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
#include "wxgis/catalogui/catalogui.h"
#include "wxgis/catalog/gxlocaldb.h"
#include "wxgis/catalogui/gxview.h"
#include "wxgis/catalogui/gxdatasetui.h"

/** @class wxGxOpenFileGDBUI
    
    A Open FileGDB GxObjectUI.

    @library{catalogui}
*/

class WXDLLIMPEXP_GIS_CLU wxGxOpenFileGDBUI :
    public wxGxOpenFileGDB,
	public IGxObjectUI,
	public IGxObjectEditUI
{
    DECLARE_CLASS(wxGxOpenFileGDBUI)
public:
    wxGxOpenFileGDBUI(wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "", const wxIcon & LargeIcon = wxNullIcon, const wxIcon & SmallIcon = wxNullIcon);
    virtual ~wxGxOpenFileGDBUI(void);
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void) const {return wxString(wxT("wxGxOpenFileGDB.ContextMenu"));};
	virtual wxString NewMenu(void) const {return wxString(wxT("wxGxOpenFileGDB.NewMenu"));};
	//IGxObjectEditUI
	virtual void EditProperties(wxWindow *parent);
protected:
    //wxGxOpenFileGDB
    virtual void LoadChildren(void);
protected:
    wxIcon m_oLargeIcon, m_oSmallIcon;
    wxIcon m_LargeSHPIcon, m_SmallSHPIcon;
    wxIcon m_LargeDBFIcon, m_SmallDBFIcon;
};

/** @class wxGxInitedFeatureDatasetUI

    An already opened layer GxObjectUI.

    @library{catalogui}
*/

class WXDLLIMPEXP_GIS_CLU wxGxInitedFeatureDatasetUI :
    public wxGxFeatureDatasetUI
{
    DECLARE_CLASS(wxGxInitedFeatureDatasetUI)
public:
    wxGxInitedFeatureDatasetUI(wxGISDataset* pwxGISDataset, wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "", const wxIcon &LargeIcon = wxNullIcon, const wxIcon &SmallIcon = wxNullIcon);
    virtual ~wxGxInitedFeatureDatasetUI(void);
};

/** @class wxGxInitedTableDatasetUI

    An already opened table GxObjectUI.

    @library{catalogui}
*/

class WXDLLIMPEXP_GIS_CLU wxGxInitedTableDatasetUI :
    public wxGxTableDatasetUI
{
    DECLARE_CLASS(wxGxInitedTableDatasetUI)
public:
    wxGxInitedTableDatasetUI(wxGISDataset* pwxGISDataset, wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "", const wxIcon &LargeIcon = wxNullIcon, const wxIcon &SmallIcon = wxNullIcon);
    virtual ~wxGxInitedTableDatasetUI(void);
};