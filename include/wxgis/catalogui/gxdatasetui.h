/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxDatasetUI classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2011 Dmitry Baryshnikov
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
#include "wxgis/catalog/gxdataset.h"

/** @class wxGxTableUI
  * 
  * A Table GxObjectUI
  * 
  * @library{catalogui}
  */

class WXDLLIMPEXP_GIS_CLU wxGxTableUI :
	public wxGxTable,
	public IGxObjectUI,
	public IGxObjectEditUI,
    public IGxObjectWizard
{
    DECLARE_CLASS(wxGxTableUI)
public:
	wxGxTableUI(wxGISEnumTableType nType, wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "", const wxIcon &LargeIcon = wxNullIcon, const wxIcon &SmallIcon = wxNullIcon);
	virtual ~wxGxTableUI(void);
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void) const {return wxString(wxT("wxGxTable.ContextMenu"));};
	virtual wxString NewMenu(void) const {return wxEmptyString;};//{return wxString(wxT("wxGxDataset.NewMenu"));};
	//IGxObjectEditUI
	virtual void EditProperties(wxWindow *parent);
    //wxGxTable
    virtual wxGISDataset* const GetDataset(bool bCached = true, ITrackCancel* const pTrackCancel = NULL);
    //IGxObjectWizard
    virtual bool Invoke(wxWindow* pParentWnd);
protected:
    wxIcon m_LargeIcon, m_SmallIcon;
};

/** \class wxGxFeatureDataset gxdataset.h
    \brief A Feature Dataset GxObject.
*/

class WXDLLIMPEXP_GIS_CLU wxGxFeatureDatasetUI :
	public wxGxFeatureDataset,
	public IGxObjectUI,
	public IGxObjectEditUI,
    public IGxObjectWizard
{
    DECLARE_CLASS(wxGxFeatureDatasetUI)
public:
	wxGxFeatureDatasetUI(wxGISEnumVectorDatasetType nType, wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "", const wxIcon &LargeIcon = wxNullIcon, const wxIcon &SmallIcon = wxNullIcon);
	virtual ~wxGxFeatureDatasetUI(void);
 	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void) const {return wxString(wxT("wxGxFeatureDataset.ContextMenu"));};
	virtual wxString NewMenu(void) const {return wxEmptyString;};//wxString(wxT("wxGxShapefileDataset.NewMenu"))
	//IGxObjectEditUI
	virtual void EditProperties(wxWindow *parent);
    //IGxObjectWizard
    virtual bool Invoke(wxWindow* pParentWnd);
    //wxGxTable
    virtual wxGISDataset* const GetDataset(bool bCached = true, ITrackCancel* const pTrackCancel = NULL);
protected:
    wxIcon m_LargeIcon, m_SmallIcon;
};

/** @class wxGxRasterDataset
  * 
  * A Raster Dataset GxObject
  * 
  * @library{catalogui}
  */

class WXDLLIMPEXP_GIS_CLU wxGxRasterDatasetUI :
	public wxGxRasterDataset,
	public IGxObjectUI,
	public IGxObjectEditUI,
    public IGxObjectWizard
{
    DECLARE_CLASS(wxGxRasterDatasetUI)
public:
	wxGxRasterDatasetUI(wxGISEnumRasterDatasetType nType, wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "", const wxIcon & LargeIcon = wxNullIcon, const wxIcon & SmallIcon = wxNullIcon);
	virtual ~wxGxRasterDatasetUI(void);
    //wxGxRasterDataset
    virtual wxGISDataset* const GetDataset(bool bCached = true, ITrackCancel* const pTrackCancel = NULL);
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void) const {return wxString(wxT("wxGxRasterDataset.ContextMenu"));};
	virtual wxString NewMenu(void) const {return wxEmptyString;};//wxString(wxT("wxGxRasterDataset.NewMenu"));};
	//IGxObjectEditUI
	virtual void EditProperties(wxWindow *parent);
    //IGxObjectWizard
    virtual bool Invoke(wxWindow* pParentWnd);
protected:
    wxIcon m_LargeIcon, m_SmallIcon;
};
