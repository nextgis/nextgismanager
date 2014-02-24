/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Remote Connection classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011,2013,2014 Bishop
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

#include "wxgisdefs.h"

#include "wxgis/catalog/gxobject.h"
#include "wxgis/catalog/gxdataset.h"
#include "wxgis/core/json/jsonval.h"

#ifdef wxGIS_USE_POSTGRES

#include "wxgis/datasource/postgisdataset.h"

class wxGxRemoteDBSchema;

WX_DECLARE_HASH_MAP(int, wxString, wxIntegerHash, wxIntegerEqual, wxGISDBShemaMap);

/** 
    @class wxGxRemoteConnection gxremoteconn.h
    
    A Remote Connection GxObject.
    
    @library {catalog}
*/

class WXDLLIMPEXP_GIS_CLT wxGxRemoteConnection :
	public wxGxObjectContainer,
    public IGxObjectEdit,
    public IGxRemoteConnection,
    public wxThreadHelper,
    public IGxObjectNoFilter
{
    DECLARE_CLASS(wxGxRemoteConnection)
public:
	wxGxRemoteConnection(wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
	virtual ~wxGxRemoteConnection(void);
    //wxGxObject
    virtual bool Destroy(void);
    virtual wxString GetCategory(void) const { return wxString(_("Database Connection")); };
    virtual void Refresh(void);
    //IGxRemoteConnection
	virtual bool Connect(void);
	virtual bool Disconnect(void);
    virtual bool IsConnected(void);
	//IGxObjectEdit
	virtual bool Delete(void);
	virtual bool CanDelete(void){return true;};
	virtual bool Rename(const wxString& NewName);
	virtual bool CanRename(void){return true;};
	virtual bool Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel);
	virtual bool CanCopy(const CPLString &szDestPath){return true;};
	virtual bool Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel);
	virtual bool CanMove(const CPLString &szDestPath){return CanCopy(szDestPath) & CanDelete();};
	//wxGxObjectContainer
	virtual bool AreChildrenViewable(void) const {return true;};
	//virtual bool HasChildren(void);
    virtual bool CanCreate(long nDataType, long DataSubtype);
    //wxGxRemoteConnection
    bool CreateSchema(const wxString& sSchemaName);
    wxString CheckUniqSchemaName(const wxString& sSchemaName, const wxString& sAdd = wxT(" "), int nCounter = 0) const;
protected:
    enum
    {
        EXIT_EVENT = wxID_HIGHEST + 1,
        LOADED_EVENT
    };
protected:
	//wxGxRemoteConnection
	virtual void LoadChildren(void);
    virtual wxGxRemoteDBSchema* GetNewRemoteDBSchema(const wxString &sName, const CPLString &soPath, wxGISPostgresDataSource *pwxGISRemoteConn);
    //create wxGISDataset without openning it
    virtual wxGISDataset* const GetDatasetFast(void);
    wxGISDBShemaMap FillSchemaNames(wxGISTableCached* pInfoSchema);
    void DeleteSchema(const wxString& sSchemaName);
    void RenameSchema(const wxString& sSchemaName, const wxString& sNewSchemaName);
    virtual wxThread::ExitCode Entry();
    virtual bool CreateAndRunThread(void);
    virtual wxThread::ExitCode CheckChanges();
    //events
    virtual void OnThreadFinished(wxThreadEvent& event);
protected:
    wxGISDataset* m_pwxGISDataset;
    bool m_bIsConnected;
    wxGISDBShemaMap m_saSchemas;
    bool m_bHasGeom, m_bHasGeog, m_bHasRaster;
    bool m_bChildrenLoaded;
private:
    DECLARE_EVENT_TABLE()
};

/** @class wxGxRemoteDBSchema
    
    A Remote Database schema GxObject.

    @library {catalog}
*/

class WXDLLIMPEXP_GIS_CLT wxGxRemoteDBSchema :	
	public wxGxObjectContainer,
    public IGxObjectEdit,
    public wxThreadHelper,
    public IGxObjectNoFilter
{
    DECLARE_CLASS(wxGxRemoteDBSchema)
public:
	wxGxRemoteDBSchema(bool bHasGeom, bool bHasGeog, bool bHasRaster, wxGISPostgresDataSource* pwxGISRemoteConn, wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
	virtual ~wxGxRemoteDBSchema(void);
	//wxGxObject
    virtual bool Destroy();
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
protected:
    enum
    {
        EXIT_EVENT = wxID_HIGHEST + 1,
        LOADED_EVENT
    };
protected:
    //wxGxRemoteDBSchema
    virtual void LoadChildren(void);
    virtual wxArrayString FillTableNames(void);
    virtual wxGxObject* AddTable(const wxString &sTableName, const wxGISEnumDatasetType eType);
    virtual void DeleteTable(const wxString& sSchemaName);

    virtual bool CreateAndRunThread(void);
    virtual void CheckChanges();
    virtual wxThread::ExitCode Entry();
    //events
    virtual void OnThreadFinished(wxThreadEvent& event);
protected:
    wxGISPostgresDataSource* m_pwxGISRemoteConn;
    bool m_bChildrenLoaded;
    wxCriticalSection m_CritSect;
    bool m_bHasGeom, m_bHasGeog, m_bHasRaster;
    wxArrayString m_saTables;
private:
    DECLARE_EVENT_TABLE()
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


/** @class wxGxNGWService

    A NextGIS Web Service GxObject.

    @library {catalog}
*/

class WXDLLIMPEXP_GIS_CLT wxGxNGWService :
    public wxGxObjectContainer, 
    public IGxObjectEdit,
    public IGxRemoteConnection,
    public IGxObjectNoFilter
{
    DECLARE_CLASS(wxGxNGWService)
public:
    wxGxNGWService(wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
    virtual ~wxGxNGWService(void);
    //wxGxObject
    virtual wxString GetCategory(void) const { return wxString(_("NextGIS Web service")); };
    virtual void Refresh(void);
    //IGxRemoteConnection
    virtual bool Connect(void);
    virtual bool Disconnect(void);
    virtual bool IsConnected(void);
    //IGxObjectEdit
    virtual bool Delete(void);
    virtual bool CanDelete(void){ return true; };
    virtual bool Rename(const wxString& NewName);
    virtual bool CanRename(void){ return true; };
    virtual bool Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel);
    virtual bool CanCopy(const CPLString &szDestPath){ return true; };
    virtual bool Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel);
    virtual bool CanMove(const CPLString &szDestPath){ return CanCopy(szDestPath) & CanDelete(); };
    //wxGxObjectContainer
    virtual bool AreChildrenViewable(void) const { return true; };
    virtual bool HasChildren(void);
    virtual bool CanCreate(long nDataType, long DataSubtype);
protected:
    virtual void LoadChildren(void);
protected:
    wxString m_sLogin;
    wxString m_sPassword;
    wxString m_sURL;
    bool m_bChildrenLoaded, m_bIsConnected;
};

/** @class wxGxNGWRoot

    A NextGIS Web Service Root Layers GxObject.

    @library {catalog}
*/
class WXDLLIMPEXP_GIS_CLT wxGxNGWRoot :
    public wxGxObjectContainer,
    public IGxObjectNoFilter
{
    DECLARE_CLASS(wxGxNGWRoot)
public:
    wxGxNGWRoot(wxGxObject *oParent, const wxString &soName = _("Layers"), const CPLString &soPath = "");
    virtual ~wxGxNGWRoot(void);
    //wxGxObject
    virtual wxString GetCategory(void) const { return wxString(_("NGW service layers")); };
    virtual void Refresh(void);
    //wxGxObjectContainer
    virtual bool AreChildrenViewable(void) const { return true; };
    virtual bool HasChildren(void);
protected:
    virtual void LoadChildren(void);
    virtual wxGxObject* AddLayer(const wxString &sName, int nId);
    virtual wxGxObject* AddLayerGroup(const wxJSONValue &Data, const wxString &sName, int nId);
protected:
    bool m_bChildrenLoaded;
    wxString m_sProxy;
    wxString m_sHeaders;
    int m_nDNSCacheTimeout;
    int m_nTimeout;
    int m_nConnTimeout;
};

/** @class wxGxNGWLayers

    A NextGIS Web Service Layers GxObject.

    @library{ catalog }
*/
class WXDLLIMPEXP_GIS_CLT wxGxNGWLayers :
    public wxGxObjectContainer,
    public IGxObjectNoFilter
{
    DECLARE_CLASS(wxGxNGWLayers)
public:
    wxGxNGWLayers(wxGxObject *oParent, const wxString &soName = _("Layers"), const CPLString &soPath = "");
    virtual ~wxGxNGWLayers(void);
    //wxGxObject
    virtual wxString GetCategory(void) const { return wxString(_("NGW service layers")); };
    //wxGxObjectContainer
    virtual bool AreChildrenViewable(void) const { return true; };
    virtual void LoadChildren(const wxJSONValue &Data);
protected:
    virtual wxGxObject* AddLayer(const wxString &sName, int nId);
    virtual wxGxObject* AddLayerGroup(const wxJSONValue &Data, const wxString &sName, int nId);
};

/** @class wxGxNGWLayer

    A NextGIS Web Service Layer GxObject.

    @library {catalog}
*/
class WXDLLIMPEXP_GIS_CLT wxGxNGWLayer :
    public wxGxObject
{
    DECLARE_CLASS(wxGxNGWLayer)
public:
    wxGxNGWLayer(wxGxObject *oParent, const wxString &soName, const CPLString &soPath = "");
    virtual ~wxGxNGWLayer(void);
    //wxGxObject
    virtual wxString GetCategory(void) const { return wxString(_("NGW service layer")); };
};
