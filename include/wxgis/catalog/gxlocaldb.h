/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  local db (sqlite, gdab, mdb) classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2014 Bishop
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
#include "wxgis/catalog/gxfolder.h"
#include "wxgis/catalog/gxdataset.h"

/** @class wxGxOpenFileGDB

    The Open FileGDB GxObject.

    @library{catalog}
*/

class WXDLLIMPEXP_GIS_CLT wxGxOpenFileGDB :
    public wxGxDatasetContainer
{
    DECLARE_CLASS(wxGxOpenFileGDB)
public:
    wxGxOpenFileGDB(wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
    virtual ~wxGxOpenFileGDB(void);
	//wxGxObject
	virtual wxString GetCategory(void) const {return wxString(_("File geodatabase"));};
	//wxGxObjectContainer
    virtual bool CanCreate(long nDataType, long DataSubtype);
    virtual bool AreChildrenViewable(void) const;
    //IGxDataset
    virtual wxGISDataset* const GetDataset(bool bCached = true, ITrackCancel* const pTrackCancel = NULL);
    virtual wxGISEnumDatasetType GetType(void) const;
    virtual int GetSubType(void) const;
    virtual wxULongLong GetSize(void) const { return m_nSize; };
    virtual wxDateTime GetModificationDate(void) const { return m_dtMod; };
    virtual void FillMetadata(bool bForce = false);
    virtual bool IsMetadataFilled() const;
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
	//wxGxOpenFileGDB
	virtual void LoadChildren(void);  
    virtual wxGISDataset* const GetDatasetFast(void);
protected:
    wxGISDataset* m_pwxGISDataset;
    wxULongLong m_nSize;
    wxDateTime m_dtMod;
    bool m_bIsMetadataFilled;
    bool m_bIsChildrenLoaded;
};

/** @class wxGxInitedFeatureDataset

    An already opened layer GxObject.

    @library{catalog}
*/

class WXDLLIMPEXP_GIS_CLT wxGxInitedFeatureDataset :
    public wxGxFeatureDataset
{
    DECLARE_CLASS(wxGxInitedFeatureDataset)
public:
    wxGxInitedFeatureDataset(wxGISDataset* pwxGISDataset, wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
    virtual ~wxGxInitedFeatureDataset(void);
};

/** @class wxGxInitedTableDataset

    An already opened table GxObject.

    @library{catalog}
*/

class WXDLLIMPEXP_GIS_CLT wxGxInitedTableDataset :
    public wxGxTableDataset
{
    DECLARE_CLASS(wxGxInitedTableDataset)
public:
    wxGxInitedTableDataset(wxGISDataset* pwxGISDataset, wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
    virtual ~wxGxInitedTableDataset(void);
};
