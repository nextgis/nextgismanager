/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Markup language Dataset classes.
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

#include "wxgisdefs.h"

#ifdef wxGIS_USE_GDAL_GNM

#include "wxgis/catalog/catalog.h"
#include "wxgis/catalog/gxobject.h"
#include "wxgis/catalog/gxdataset.h"
#include "wxgis/datasource/dataset.h"

/** @class wxGxGNMConnectivity

    A gnm connectivity GxObject.

    @library{catalog}
*/

class WXDLLIMPEXP_GIS_CLT wxGxGNMConnectivity :
    public wxGxDatasetContainer
{
    DECLARE_CLASS(wxGxGNMConnectivity)
public:
    wxGxGNMConnectivity(wxGISEnumVectorDatasetType eType, wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
    virtual ~wxGxGNMConnectivity(void);
    //wxGxObject
	virtual wxString GetCategory(void) const;
    virtual void Refresh(void);
    //wxGxDatasetContainer
	virtual wxGISDataset* const GetDataset(bool bCache = true, ITrackCancel* const pTrackCancel = NULL);
	virtual wxGISEnumDatasetType GetType(void) const;
    virtual int GetSubType(void) const {return m_eType;};
    //wxGxObjectContainer
    virtual bool DestroyChildren();
	virtual bool AreChildrenViewable(void) const {return true;};
	virtual bool HasChildren(void);
protected:
	//wxGxMLDataset
	virtual void LoadChildren(void);
    //create wxGISDataset without openning it
    virtual wxGISDataset* const GetDatasetFast(void);
protected:
    bool m_bIsChildrenLoaded;
//    wxGISDataset* m_pwxGISDataset;
	wxGISEnumVectorDatasetType m_eType;
};

/** @class wxGxMLSubDataset

    A markup language layer GxObject.

    @library{catalog}


class WXDLLIMPEXP_GIS_CLT wxGxMLSubDataset :
	public wxGxFeatureDataset
{
    DECLARE_CLASS(wxGxMLSubDataset)
public:
	wxGxMLSubDataset(wxGISEnumVectorDatasetType nType, wxGISDataset* pwxGISDataset, wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
	virtual ~wxGxMLSubDataset(void);
	//IGxObject
	virtual wxString GetCategory(void) const;
};
*/

#endif
