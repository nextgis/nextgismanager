/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Remote Connection classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011,2013,2014 Dmitry Baryshnikov
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

#include "wxgis/catalog/gxobject.h"
#include "wxgis/catalog/gxdataset.h"
#include "wxgis/core/json/jsonval.h"
#include "wxgis/catalog/contupdater.h"

#ifdef wxGIS_USE_POSTGRES

#include "wxgis/datasource/postgisdataset.h"

class wxGxRemoteDBSchema;

/**
    @class wxGxRemoteConnection gxremoteconn.h

    A Remote Connection GxObject.

    @library{catalog}
*/

class WXDLLIMPEXP_GIS_CLT wxGxRemoteConnection :
    public IGxObjectEdit,
    public IGxRemoteConnection,
    public IGxObjectNoFilter,
	public wxGxObjectContainerUpdater
{
    DECLARE_CLASS(wxGxRemoteConnection)
public:
	wxGxRemoteConnection(wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
	virtual ~wxGxRemoteConnection(void);
    //wxGxObject
    virtual wxString GetCategory(void) const { return wxString(_("Database Connection")); };
    virtual void Refresh(void);
    //IGxRemoteConnection
	virtual bool Connect(void);
	virtual bool Disconnect(void);
    virtual bool IsConnected(void);
	//IGxObjectEdit
	virtual bool Delete(void);
	virtual bool CanDelete(void) {return true;};
	virtual bool Rename(const wxString& NewName);
    virtual bool CanRename(void) { return true; };
	virtual bool Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel);
    virtual bool CanCopy(const CPLString &szDestPath) { return true; };
	virtual bool Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel);
    virtual bool CanMove(const CPLString &szDestPath) { return CanCopy(szDestPath) & CanDelete(); };
	//wxGxObjectContainer
	virtual bool AreChildrenViewable(void) const {return true;};
    virtual bool CanCreate(long nDataType, long DataSubtype);
    //wxGxRemoteConnection
    virtual bool CreateSchema(const wxString& sSchemaName);
    virtual wxString CheckUniqSchemaName(const wxString& sSchemaName, const wxString& sAdd = wxT(" "), int nCounter = 0) const;
protected:
	//wxGxRemoteConnection
	virtual void LoadChildren(void);
    virtual wxGxRemoteDBSchema* GetNewRemoteDBSchema(int nRemoteId, const wxString &sName, const CPLString &soPath, wxGISPostgresDataSource *pwxGISRemoteConn);
    //create wxGISDataset without openning it
    virtual wxGISDataset* const GetDatasetFast(void);
	//wxGxObjectContainerUpdater
	virtual wxGxObjectMap GetRemoteObjects();
	virtual void AddObject(int nRemoteId, const wxString &sName);
protected:
    wxGISDataset* m_pwxGISDataset;
    bool m_bIsConnected;
};

/** @class wxGxRemoteDBSchema

    A Remote Database schema GxObject.

    @library{catalog}
*/

class WXDLLIMPEXP_GIS_CLT wxGxRemoteDBSchema :
    public IGxObjectEdit,
    public IGxObjectNoFilter,
	public wxGxObjectContainerUpdater,
	public wxGxRemoteId
{
    DECLARE_CLASS(wxGxRemoteDBSchema)
public:
	wxGxRemoteDBSchema(int nRemoteId, wxGISPostgresDataSource* pwxGISRemoteConn, wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
	virtual ~wxGxRemoteDBSchema(void);
	//wxGxObject
    virtual wxString GetCategory(void) const { return wxString(_("Remote Database Schema")); };
    virtual void Refresh(void);
	//wxGxObjectContainer
	virtual bool AreChildrenViewable(void) const {return true;};
	virtual bool HasChildren(void);
    virtual bool CanCreate(long nDataType, long DataSubtype);
    //IGxObjectEdit
	virtual bool Delete(void);
    virtual bool CanDelete(void);
	virtual bool Rename(const wxString& NewName);
    virtual bool CanRename(void);
    virtual bool Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel);
    virtual bool CanCopy(const CPLString &szDestPath);
    virtual bool Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel);
    virtual bool CanMove(const CPLString &szDestPath);
    wxString CheckUniqTableName(const wxString& sTableName, const wxString& sAdd = wxT(" "), int nCounter = 0) const;
    //wxGxRemoteDBSchema
    virtual void LoadChildren(void);	
protected:
    //wxGxRemoteDBSchema
    virtual wxGxObject* GetNewTable(int nRemoteId, const wxString &sTableName, const wxGISEnumDatasetType eType);
	//wxGxObjectContainerUpdater
	virtual wxGxObjectMap GetRemoteObjects();
	virtual void AddObject(int nRemoteId, const wxString &sName);
protected:
    wxGISPostgresDataSource* m_pwxGISRemoteConn;
};

#endif //wxGIS_USE_POSTGRES


/** @class wxGxTMSWebService

    A TMS Web Service GxObject.

    @library {catalog}
*/

class WXDLLIMPEXP_GIS_CLT wxGxTMSWebService :
	public wxGxRasterDataset
{
    DECLARE_CLASS(wxGxTMSWebService)
public:
	wxGxTMSWebService(wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
	virtual ~wxGxTMSWebService(void);
	//wxGxObject
	virtual wxString GetCategory(void) const {return wxString(_("Tile Map service"));};
    //wxGxDataset
    virtual void FillMetadata(bool bForce = false);
};

