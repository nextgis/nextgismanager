/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Markup language object classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2014 Dmitry Baryshnikov
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
#include "wxgis/catalog/gxgnm.h"
#include "wxgis/core/process.h"

#ifdef wxGIS_USE_GDAL_GNM

/** @class wxGxGNMConnectivityUI

    A geography network model GxObject UI.

    @library{catalogui}
*/

class WXDLLIMPEXP_GIS_CLU wxGxGNMConnectivityUI :
    public wxGxGNMConnectivity,
	public IGxObjectUI,
	public IGxObjectEditUI,
    public wxGISThreadHelper
{
    DECLARE_CLASS(wxGxMLDatasetUI)
public:
    wxGxGNMConnectivityUI(wxGISEnumVectorDatasetType eType, wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "", const wxIcon &LargeIcon = wxNullIcon, const wxIcon &SmallIcon = wxNullIcon, const wxIcon &SubLargeIcon = wxNullIcon, const wxIcon &SubSmallIcon = wxNullIcon);
    virtual ~wxGxGNMConnectivityUI(void);
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void) const {return wxString(wxT("wxGxGNMConnectivityUI.ContextMenu"));};
	virtual wxString NewMenu(void) const {return wxString(wxT("wxGxGNMConnectivityUI.NewMenu"));};
	//wxGxGNMConnectivity
    //virtual wxGISDataset* const GetDataset(bool bCache, ITrackCancel* const pTrackCancel = NULL);
	virtual void LoadChildren(void);
protected:
    virtual wxThread::ExitCode Entry();
protected:
    wxIcon m_LargeIcon, m_SmallIcon;
    wxIcon m_LargeSubIcon, m_SmallSubIcon;
    long m_nPendUId;
};

/** @class wxGxMLSubDatasetUI

    A kml layer GxObject.

    @library{catalogui}


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
protected:
    wxIcon m_LargeIcon, m_SmallIcon;
};*/
#endif
