/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Remote Connection classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2013,2014 NextGIS
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

#ifdef wxGIS_USE_CURL

#include "wxgis/net/curl.h"

/** @class wxGxNGWService

    A NextGIS Web Service GxObject.

    @library{catalog}
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
    virtual bool CanDelete(void) { return true; };
    virtual bool Rename(const wxString& NewName);
    virtual bool CanRename(void) { return true; };
    virtual bool Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel);
    virtual bool CanCopy(const CPLString &szDestPath) { return true; };
    virtual bool Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel);
    virtual bool CanMove(const CPLString &szDestPath) { return CanCopy(szDestPath) & CanDelete(); };
    //wxGxObjectContainer
    virtual bool AreChildrenViewable(void) const { return true; };
    virtual bool HasChildren(void);
    virtual bool CanCreate(long nDataType, long DataSubtype);
    //wxGxNGWService
    virtual wxGISCurl GetCurl();
	virtual wxString GetLogin() const;
	virtual wxString GetPassword() const;
protected:
    virtual void LoadChildren(void);
    virtual bool ConnectToNGW();
protected:
    wxString m_sLogin;
    wxString m_sPassword;
    wxString m_sURL;
    bool m_bChildrenLoaded, m_bIsConnected, m_bIsAuthorized;
    wxString m_sAuthCookie;
};

/** @enum wxGISEnumNGWResourcesType

    The NGW resource type.

    @library{catalog}
 */
 
enum wxGISEnumNGWResourcesType
{
	enumNGWResourceTypeNone,
	enumNGWResourceTypeResourceGroup,
	enumNGWResourceTypePostgisLayer,
	enumNGWResourceTypePostgisConnection,
	enumNGWResourceTypeWMSServerService,
	enumNGWResourceTypeBaseLayers,
	enumNGWResourceTypeWebMap,
	enumNGWResourceTypeWFSServerService,
	enumNGWResourceTypeVectorLayer
};


/** @class wxGxNGWResource

    A NextGIS Web wxGxNGWResource.

    @library{catalog}
*/
class wxGxNGWResource
{
public:
    wxGxNGWResource(const wxJSONValue &Data);
    virtual ~wxGxNGWResource(void);
	virtual int GetResourceId() const;
protected:
	virtual bool DeleteResource();
	virtual int GetParentResourceId() const = 0;
protected:
    wxGISEnumNGWResourcesType m_eResourceType;
    bool m_bHasChildren;
    wxString m_sDescription;
    wxString m_sDisplayName;
    int m_nResourceId;
    //wxArrayString m_aInterfaces;
    wxString m_sKeyName;
    int m_nOwnerId;
    wxArrayString m_aPermissions;
    wxArrayString m_aScopes;
    wxGxNGWService *m_pService;
};

/** @class wxGxNGWResourceGroup

    A NextGIS Web Service Resource Group GxObject.

    @library{catalog}
*/

class WXDLLIMPEXP_GIS_CLT wxGxNGWResourceGroup :
    public wxGxObjectContainer,
    /*public IGxObjectEdit,
    public wxThreadHelper,
    public IGxObjectNoFilter,
	public IGxObjectNotifier,*/
    public wxGxNGWResource
{
    DECLARE_CLASS(wxGxNGWResourceGroup)
public:
    wxGxNGWResourceGroup(wxGxNGWService *pService, const wxJSONValue &Data, wxGxObject *oParent, const wxString &soName = _("Resource Group"), const CPLString &soPath = "");
    virtual ~wxGxNGWResourceGroup(void);
    //wxGxObject
    virtual wxString GetCategory(void) const { return wxString(_("Resource Group")); };
    //wxGxObjectContainer
    //virtual void LoadChildren(const wxJSONValue &Data);
    virtual void Refresh(void);
    //wxGxObjectContainer
    virtual bool AreChildrenViewable(void) const { return true; };
    virtual bool HasChildren(void);
    //IGxObjectEdit
	/*virtual bool Delete(void);
    virtual bool CanDelete(void);
	virtual bool Rename(const wxString& NewName);
    virtual bool CanRename(void);
    virtual bool Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel);
    virtual bool CanCopy(const CPLString &szDestPath);
    virtual bool Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel);
    virtual bool CanMove(const CPLString &szDestPath);
    wxString CheckUniqTableName(const wxString& sTableName, const wxString& sAdd = wxT(" "), int nCounter = 0) const;
	//IGxObjectNotifier
	virtual void OnGetUpdates();	*/
protected:
    virtual void LoadChildren(void);
    virtual void AddResource(const wxJSONValue &Data);
    virtual wxGISEnumNGWResourcesType GetType(const wxJSONValue &Data) const;
	virtual int GetParentResourceId() const;    
	/*virtual bool CreateAndRunThread(void);
    virtual bool CheckChanges();
    virtual wxThread::ExitCode Entry();
    //events
    virtual void OnThreadFinished(wxThreadEvent& event);*/
protected:
    bool m_bChildrenLoaded;
	bool m_bHasGeoJSON;
	int m_nLongWait, m_nShortWait;
	bool m_bProcessUpdates;
};


/** @class wxGxNGWRootResource

    A NextGIS Web Service root resource GxObject.

    @library{catalog}
*/
class WXDLLIMPEXP_GIS_CLT wxGxNGWRootResource :
    public wxGxNGWResourceGroup
{
    DECLARE_CLASS(wxGxNGWRootResource)
public:
    wxGxNGWRootResource(wxGxNGWService *pService, wxGxObject *oParent, const wxString &soName = _("Resources"), const CPLString &soPath = "");
    virtual ~wxGxNGWRootResource(void);
    //wxGxObject
    virtual wxString GetCategory(void) const { return wxString(_("NGW resources")); };
	//IGxObjectEdit
	virtual bool CanDelete(void);
    virtual bool CanRename(void);
    virtual bool CanCopy(const CPLString &szDestPath);
    virtual bool CanMove(const CPLString &szDestPath);
};

/** @class wxGxNGWLayer

    A NextGIS Web Service Layer GxObject.

    @library{catalog}
*/

class WXDLLIMPEXP_GIS_CLT wxGxNGWLayer :
    public wxGxNGWResource,
	public wxGxFeatureDataset,
    public IGxObjectEdit
{
    DECLARE_CLASS(wxGxNGWLayer)
public:
    wxGxNGWLayer(wxGxNGWService *pService, wxGISEnumNGWResourcesType eType, const wxJSONValue &Data, wxGxObject *oParent, const wxString &soName, const CPLString &soPath = "");
    virtual ~wxGxNGWLayer(void);
    //wxGxObject
    virtual wxString GetCategory(void) const;
protected:
    //create wxGISDataset without openning it
    virtual wxGISDataset* const GetDatasetFast(void);
	virtual int GetParentResourceId() const;
};

#endif // wxGIS_USE_CURL
