/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Spreadsheet object ui classes.
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
#include "wxgis/catalog/gxssdataset.h"
#include "wxgis/core/process.h"

/** @class wxGxSpreadsheetDatasetUI

    A Spreadsheet (ods, xls, etc.) dataset GxObject.

    @library{catalogui}
*/

class WXDLLIMPEXP_GIS_CLU wxGxSpreadsheetDatasetUI :
    public wxGxSpreadsheetDataset,
	public IGxObjectUI,
	public IGxObjectEditUI,
    public wxGISThreadHelper
{
    DECLARE_CLASS(wxGxSpreadsheetDatasetUI)
public:
	wxGxSpreadsheetDatasetUI(wxGISEnumTableType eType, wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "", const wxIcon &LargeIcon = wxNullIcon, const wxIcon &SmallIcon = wxNullIcon, const wxIcon &SubLargeIcon = wxNullIcon, const wxIcon &SubSmallIcon = wxNullIcon);
	virtual ~wxGxSpreadsheetDatasetUI(void);
	virtual bool HasChildren(bool bWaitLoading = false);
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void) const {return wxString(wxT("wxGxFeatureDataset.ContextMenu"));};
	virtual wxString NewMenu(void) const {return wxString(wxT("wxGxSpreadsheetDataset.NewMenu"));};
	//IGxObjectEditUI
	virtual wxArrayString GetPropertyPages() const;
	virtual bool HasPropertyPages(void) const;
	//wxGxKMLDataset
    virtual wxGISDataset* const GetDataset(bool bCache, ITrackCancel* const pTrackCancel = NULL);
	virtual void LoadChildren(void);
protected:
    virtual wxThread::ExitCode Entry();
protected:
    wxIcon m_LargeIcon, m_SmallIcon;
    wxIcon m_LargeSubIcon, m_SmallSubIcon;
    long m_nPendUId;
};

/** @class wxGxSpreadsheetSubDatasetUI

    A Spreadsheet page GxObject.

    @library{catalogui}
*/

class WXDLLIMPEXP_GIS_CLU wxGxSpreadsheetSubDatasetUI :
	public wxGxSpreadsheetSubDataset,
	public IGxObjectUI,
	public IGxObjectEditUI
{
    DECLARE_CLASS(wxGxSpreadsheetSubDatasetUI)
public:
	wxGxSpreadsheetSubDatasetUI(wxGISEnumTableType nType, wxGISDataset* pwxGISDataset, wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "", const wxIcon &LargeIcon = wxNullIcon, const wxIcon &SmallIcon = wxNullIcon);
	virtual ~wxGxSpreadsheetSubDatasetUI(void);
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void) const {return wxString(wxT("wxGxSpreadsheetSubDatasetUI.ContextMenu"));};
	virtual wxString NewMenu(void) const {return wxEmptyString;};
	//IGxObjectEditUI
	virtual wxArrayString GetPropertyPages() const;
	virtual bool HasPropertyPages(void) const;
protected:
    wxIcon m_LargeIcon, m_SmallIcon;
};
