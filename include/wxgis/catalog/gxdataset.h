/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  GxDataset classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2014 Dmitry Baryshnikov
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

#include "wxgis/catalog/catalog.h"
#include "wxgis/catalog/gxobject.h"
#include "wxgis/datasource/dataset.h"

/** @class wxGxDataset

    A base GxDataset class.

    @library{catalog}
*/

class WXDLLIMPEXP_GIS_CLT wxGxDataset :
	public wxGxObject,
    public IGxObjectEdit,
    public IGxDataset
{
    DECLARE_ABSTRACT_CLASS(wxGxDataset)
public:
    wxGxDataset(wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
	virtual ~wxGxDataset(void);
	virtual wxGISDataset* const GetDataset(bool bCached = true, ITrackCancel* const pTrackCancel = NULL) {return NULL;};
	virtual wxGISEnumDatasetType GetType(void) const = 0;
	virtual int GetSubType(void) const = 0;
    virtual wxULongLong GetSize(void) const {return m_nSize;};
    virtual wxDateTime GetModificationDate(void) const {return m_dtMod;};
    virtual void FillMetadata(bool bForce = false);
    virtual bool IsMetadataFilled() const;
    //wxGxObject
    virtual void SetPath(const CPLString &soPath);
    //IGxObjectEdit
    virtual bool CanDelete(void);
    virtual bool CanRename(void);
    virtual bool CanCopy(const CPLString &szDestPath);
    virtual bool CanMove(const CPLString &szDestPath);
	virtual bool Delete(void);
	virtual bool Rename(const wxString &sNewName);
	virtual bool Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel);
	virtual bool Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel);
protected:
    //create wxGISDataset without openning it
    virtual wxGISDataset* const GetDatasetFast(void){ return NULL;};
protected:
	wxGISDataset* m_pwxGISDataset;
    wxULongLong m_nSize;
    wxDateTime m_dtMod;    
    bool m_bIsMetadataFilled;
};

/** @class wxGxContainerDataset

    A base container GxDataset class.

    @library{catalog}
*/

class WXDLLIMPEXP_GIS_CLT wxGxDatasetContainer :
	public wxGxObjectContainer,
    public IGxObjectEdit,
    public IGxDataset
{
    DECLARE_ABSTRACT_CLASS(wxGxDatasetContainer)
public:
    wxGxDatasetContainer(wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
	virtual ~wxGxDatasetContainer(void);
	virtual wxGISDataset* const GetDataset(bool bCached = true, ITrackCancel* const pTrackCancel = NULL) {return NULL;};
	virtual wxGISEnumDatasetType GetType(void) const = 0;
	virtual int GetSubType(void) const = 0;
    virtual wxULongLong GetSize(void) const {return m_nSize;};
    virtual wxDateTime GetModificationDate(void) const {return m_dtMod;};
    virtual void FillMetadata(bool bForce = false);
    virtual bool IsMetadataFilled() const;
    //wxGxObject
    virtual void SetPath(const CPLString &soPath);
    //IGxObjectEdit
	virtual bool CanDelete(void);
    virtual bool CanRename(void);
    virtual bool CanCopy(const CPLString &szDestPath);
    virtual bool CanMove(const CPLString &szDestPath);
	virtual bool Delete(void);
	virtual bool Rename(const wxString &sNewName);
	virtual bool Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel);
	virtual bool Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel);
protected:
    //create wxGISDataset without openning it
    virtual wxGISDataset* const GetDatasetFast(void) {return NULL;};
protected:
	wxGISDataset* m_pwxGISDataset;
    wxULongLong m_nSize;
    wxDateTime m_dtMod;    
    bool m_bIsMetadataFilled;
};

/** @class wxGxTable

    A Table GxObject

    @library{catalog}
*/

class WXDLLIMPEXP_GIS_CLT wxGxTable :
	public wxGxDataset
{
    DECLARE_CLASS(wxGxTable)
public:
	wxGxTable(wxGISEnumTableType eType, wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
	virtual ~wxGxTable(void);
	//wGxObject
	virtual wxString GetCategory(void) const;
	//wxGxDataset
	virtual wxGISDataset* const GetDataset(bool bCache = true, ITrackCancel* const pTrackCancel = NULL);
	virtual wxGISEnumDatasetType GetType(void) const {return enumGISTable;};
    virtual int GetSubType(void) const { return m_eType; };
protected:
    //create wxGISDataset without openning it
    virtual wxGISDataset* const GetDatasetFast(void);
protected:
    wxGISEnumTableType m_eType;
};

/** @class wxGxFeatureDataset

    A Feature Dataset GxObject.

    @library{catalog}
*/

class WXDLLIMPEXP_GIS_CLT wxGxFeatureDataset :
	public wxGxDataset
{
    DECLARE_CLASS(wxGxFeatureDataset)
public:
	wxGxFeatureDataset(wxGISEnumVectorDatasetType eType, wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
	virtual ~wxGxFeatureDataset(void);
	//wGxObject
	virtual wxString GetCategory(void) const;
	//wxGxDataset
	virtual wxGISDataset* const GetDataset(bool bCache = true, ITrackCancel* const pTrackCancel = NULL);
	virtual wxGISEnumDatasetType GetType(void) const {return enumGISFeatureDataset;};
    virtual int GetSubType(void) const {return m_eType;};
protected:
    virtual wxGISDataset* const GetDatasetFast(void);
protected:
	wxGISEnumVectorDatasetType m_eType;
};

/** @class wxGxRasterDataset

    A Raster Dataset GxObject.

    @library{catalog}
*/

class WXDLLIMPEXP_GIS_CLT wxGxRasterDataset :
	public wxGxDataset
{
    DECLARE_CLASS(wxGxRasterDataset)
public:
	wxGxRasterDataset(wxGISEnumRasterDatasetType eType, wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
	virtual ~wxGxRasterDataset(void);
	//wGxObject
	virtual wxString GetCategory(void) const;
	//wxGxDataset
	virtual wxGISDataset* const GetDataset(bool bCache = true, ITrackCancel* const pTrackCancel = NULL);
	virtual wxGISEnumDatasetType GetType(void) const {return enumGISRasterDataset;};
    virtual int GetSubType(void) const {return m_eType;};
protected:
    virtual wxGISDataset* const GetDatasetFast(void);
protected:
    wxGISEnumRasterDatasetType m_eType;
};
