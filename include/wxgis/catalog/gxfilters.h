/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxObjectDialog filters of GxObjects to show.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011,2012 Bishop
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

#include "wxgis/datasource/datasource.h"
#include "wxgis/catalog/gxobject.h"

/** @class wxGxObjectFilter
   
    The base class GxObject Filters.

    @library {catalog}
 */
class WXDLLIMPEXP_GIS_CLT wxGxObjectFilter : public wxObject
{
    DECLARE_CLASS(wxGxObjectFilter)
public:
    wxGxObjectFilter(void);
	virtual ~wxGxObjectFilter(void);
	virtual bool CanChooseObject( wxGxObject* const pObject );
	virtual bool CanDisplayObject( wxGxObject* const pObject );
	virtual bool CanStoreToObject( wxGxObject* const pObject );
	virtual wxGISEnumSaveObjectResults CanSaveObject( wxGxObject* const pLocation, const wxString &sName );
	virtual wxString GetName(void) const;
    virtual wxString GetExt(void) const;
    virtual wxString GetDriver(void) const;
    virtual int GetSubType(void) const;
    virtual wxGISEnumDatasetType GetType(void) const;
};

WX_DEFINE_USER_EXPORTED_ARRAY(wxGxObjectFilter*, wxGxObjectFiltersArray, class WXDLLIMPEXP_GIS_CLT);

/** @class wxGxPrjFileFilter
 
    The Spatial reference GxObject Filters.

    @library {catalog}
 */
/*
class WXDLLIMPEXP_GIS_CLT wxGxPrjFileFilter : public wxGxObjectFilter
{
    DECLARE_CLASS(wxGxPrjFileFilter)
public:
	wxGxPrjFileFilter(void);
	virtual ~wxGxPrjFileFilter(void);
	virtual bool CanChooseObject( wxGxObject* const pObject );
	virtual bool CanDisplayObject( wxGxObject* const pObject );
	virtual wxString GetName(void) const;
    virtual wxString GetExt(void) const;
};
*/

/** @class wxGxRemoteDBFilter

    The database (postgis, mysql, etc.) schema GxObject Filters.
    
    @library {catalog}
*/

class WXDLLIMPEXP_GIS_CLT wxGxRemoteDBSchemaFilter : public wxGxObjectFilter
{
    DECLARE_CLASS(wxGxRemoteDBSchemaFilter)
public:
    wxGxRemoteDBSchemaFilter(void);
    virtual ~wxGxRemoteDBSchemaFilter(void);
    virtual bool CanChooseObject(wxGxObject* const pObject);
    virtual bool CanDisplayObject(wxGxObject* const pObject);
    virtual bool CanStoreToObject(wxGxObject* const pObject);
    virtual wxString GetName(void) const;
};

/** @class wxGxDatasetFilter

    The Dataset GxObject Filters.

    @library {catalog}
 */

class WXDLLIMPEXP_GIS_CLT wxGxDatasetFilter : public wxGxObjectFilter
{
    DECLARE_CLASS(wxGxDatasetFilter)
public:
	wxGxDatasetFilter(wxGISEnumDatasetType nType, int nSubType = wxNOT_FOUND);
	virtual ~wxGxDatasetFilter(void);
	virtual bool CanChooseObject( wxGxObject* const pObject );
	virtual bool CanDisplayObject( wxGxObject* const pObject );
    virtual bool CanStoreToObject(wxGxObject* const pObject);
    virtual wxString GetName(void) const;
    virtual wxGISEnumDatasetType GetType(void) const{return m_nType;};
    virtual int GetSubType(void) const{return m_nSubType;};
protected:
    wxGISEnumDatasetType m_nType;
	int m_nSubType;
};

/** @class wxGxFeatureDatasetFilter

    The FeatureDataset GxObject Filters.

    @library {catalog}
*/

class WXDLLIMPEXP_GIS_CLT wxGxFeatureDatasetFilter : public wxGxObjectFilter
{
    DECLARE_CLASS(wxGxFeatureDatasetFilter)
public:
	wxGxFeatureDatasetFilter(wxGISEnumVectorDatasetType nSubType);
	virtual ~wxGxFeatureDatasetFilter(void);
	virtual bool CanChooseObject( wxGxObject* const pObject );
	virtual bool CanDisplayObject( wxGxObject* const pObject );
    virtual bool CanStoreToObject(wxGxObject* const pObject);
    virtual wxString GetName(void) const;
    virtual wxString GetExt(void) const;
    virtual wxString GetDriver(void) const;
    virtual int GetSubType(void) const;
    virtual wxGISEnumDatasetType GetType(void) const {return enumGISFeatureDataset;};
protected:
    wxGISEnumVectorDatasetType m_nSubType;
};

/** @class wxGxFolderFilter 

    The Folder GxObject Filters.

    @library {catalog}
 */

class WXDLLIMPEXP_GIS_CLT wxGxFolderFilter : public wxGxObjectFilter
{
    DECLARE_CLASS(wxGxFolderFilter)
public:
	wxGxFolderFilter(void);
	virtual ~wxGxFolderFilter(void);
	virtual bool CanChooseObject( wxGxObject* const pObject );
	virtual bool CanDisplayObject( wxGxObject* const pObject );
    virtual bool CanStoreToObject(wxGxObject* const pObject);
    virtual wxString GetName(void) const;
};

/** @class wxGxRasterFilter

    The raster GxObject Filters.

    @library {catalog}
*/
/*
class WXDLLIMPEXP_GIS_CLT wxGxRasterFilter : public wxGxObjectFilter
{
public:
	wxGxRasterFilter(wxGISEnumRasterDatasetType nSubType);
	virtual ~wxGxRasterFilter(void);
	virtual bool CanChooseObject( IGxObject* pObject );
	virtual bool CanDisplayObject( IGxObject* pObject );
	virtual wxString GetName(void);
    virtual wxString GetExt(void);
    virtual wxString GetDriver(void);
    virtual int GetSubType(void);
    virtual wxGISEnumDatasetType GetType(void){return enumGISRasterDataset;};
protected:
    wxGISEnumRasterDatasetType m_nSubType;
};


/** @class wxGxTextFilter

    The Text file GxObject Filters.

    @library {catalog}
 */

class WXDLLIMPEXP_GIS_CLT wxGxTextFilter : public wxGxObjectFilter
{
    DECLARE_CLASS(wxGxTextFilter)
public:
	wxGxTextFilter(const wxString &soName, const wxString &soExt);
	virtual ~wxGxTextFilter(void);
	virtual bool CanChooseObject( wxGxObject* const pObject );
	virtual bool CanDisplayObject( wxGxObject* const pObject );
    virtual bool CanStoreToObject(wxGxObject* const pObject);
    virtual wxString GetName(void) const;
    virtual wxString GetExt(void) const;
protected:
    wxString m_soName;
    wxString m_soExt, m_soExtCmp;
};


/** @class wxGxTableDatasetFilter

    The TableDataset GxObject Filters.

    @library {catalog}
 */

class WXDLLIMPEXP_GIS_CLT wxGxTableDatasetFilter : public wxGxObjectFilter
{
    DECLARE_CLASS(wxGxTableDatasetFilter)
public:
	wxGxTableDatasetFilter(wxGISEnumTableDatasetType nSubType);
	virtual ~wxGxTableDatasetFilter(void);
	virtual bool CanChooseObject( wxGxObject* const pObject );
	virtual bool CanDisplayObject( wxGxObject* const pObject );
    virtual bool CanStoreToObject(wxGxObject* const pObject);
    virtual wxString GetName(void) const;
    virtual wxString GetExt(void) const;
    virtual wxString GetDriver(void) const;
    virtual int GetSubType(void) const;
    virtual wxGISEnumDatasetType GetType(void) const {return enumGISTableDataset;};
protected:
    wxGISEnumTableDatasetType m_nSubType;
};

