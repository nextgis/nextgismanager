/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Markup language object classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010,2011,2013,2014 Dmitry Baryshnikov
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
#include "wxgis/catalog/gxmldataset.h"

/** @class wxGxMLDatasetUI

    A markup language (kml, gml, etc.) dataset GxObject.

    @library{catalogui}
*/

class WXDLLIMPEXP_GIS_CLU wxGxMLDatasetUI :
    public wxGxMLDataset,
	public IGxObjectUI,
	public IGxObjectEditUI,
    public wxThreadHelper
{
    DECLARE_CLASS(wxGxMLDatasetUI)
public:
	wxGxMLDatasetUI(wxGISEnumVectorDatasetType eType, wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "", const wxIcon &LargeIcon = wxNullIcon, const wxIcon &SmallIcon = wxNullIcon, const wxIcon &SubLargeIcon = wxNullIcon, const wxIcon &SubSmallIcon = wxNullIcon);
	virtual ~wxGxMLDatasetUI(void);
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void) const {return wxString(wxT("wxGxFeatureDataset.ContextMenu"));};
	virtual wxString NewMenu(void) const {return wxString(wxT("wxGxKMLDataset.NewMenu"));};
	//IGxObjectEditUI
	virtual void EditProperties(wxWindow *parent);
	//wxGxKMLDataset
    virtual wxGISDataset* const GetDataset(bool bCache, ITrackCancel* const pTrackCancel = NULL);
	virtual void LoadChildren(void);
protected:
    virtual wxThread::ExitCode Entry();
    bool CreateAndRunCheckThread(void);
protected:
    wxIcon m_LargeIcon, m_SmallIcon;
    wxIcon m_LargeSubIcon, m_SmallSubIcon;
    long m_nPendUId;
};

/** @class wxGxMLSubDatasetUI

    A kml layer GxObject.

    @library{catalogui}
*/

class WXDLLIMPEXP_GIS_CLU wxGxMLSubDatasetUI :
	public wxGxMLSubDataset,
	public IGxObjectUI,
	public IGxObjectEditUI
{
    DECLARE_CLASS(wxGxMLSubDatasetUI)
public:
	wxGxMLSubDatasetUI(wxGISEnumVectorDatasetType nType, wxGISDataset* pwxGISDataset, wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "", const wxIcon &LargeIcon = wxNullIcon, const wxIcon &SmallIcon = wxNullIcon);
	virtual ~wxGxMLSubDatasetUI(void);
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void) const {return wxString(wxT("wxGxKMLSubDataset.ContextMenu"));};
	virtual wxString NewMenu(void) const {return wxEmptyString;};
	//IGxObjectEditUI
	virtual void EditProperties(wxWindow *parent);
protected:
    wxIcon m_LargeIcon, m_SmallIcon;
};
