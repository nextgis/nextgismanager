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

#include "wxgis/catalog/gxngwconn.h"
#include "wxgis/datasource/sysop.h"
#include "wxgis/catalog/gxcatalog.h"
#include "wxgis/core/json/jsonreader.h"
#include "wxgis/core/json/jsonwriter.h"
#include "wxgis/core/crypt.h"
#include "wxgis/core/app.h"

#ifdef wxGIS_USE_CURL

//--------------------------------------------------------------
//class wxGxNGWWebService
//--------------------------------------------------------------
IMPLEMENT_CLASS(wxGxNGWService, wxGxObjectContainer)

wxGxNGWService::wxGxNGWService(wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxObjectContainer(oParent, soName, soPath)
{
    m_bChildrenLoaded = false;
    m_bIsConnected = false;
    m_bIsAuthorized = false;

    ReadConnectionFile();
}

void wxGxNGWService::ReadConnectionFile()
{
	wxXmlDocument doc(wxString::FromUTF8(m_sPath));
    if (doc.IsOk())
    {
        wxXmlNode* pRootNode = doc.GetRoot();
        if (NULL != pRootNode)
        {
            m_sURL = pRootNode->GetAttribute(wxT("url"));
			if (!m_sURL.StartsWith(wxT("http")))
			{
				m_sURL.Prepend(wxT("http://"));
			}		
			
            m_sLogin = pRootNode->GetAttribute(wxT("user"));
            Decrypt(pRootNode->GetAttribute(wxT("pass")), m_sPassword);
        }
    }
}

wxGxNGWService::~wxGxNGWService(void)
{
}

bool wxGxNGWService::Delete(void)
{
    Disconnect();

    bool bRet = DeleteFile(m_sPath);

    if (!bRet)
    {
		wxString sErr = wxString::Format(_("Operation '%s' failed!"), _("Delete"));   
		sErr += wxT("\n") + wxString::Format(wxT("%s '%s'"), GetCategory().c_str(), wxString::FromUTF8(m_sPath));
		wxGISLogError(sErr, wxString::FromUTF8(CPLGetLastErrorMsg()), wxEmptyString, NULL);	
        return false;
    }
    return true;
}

bool wxGxNGWService::Rename(const wxString &sNewName)
{
    CPLString szDirPath = CPLGetPath(m_sPath);
    CPLString szName = CPLGetBasename(m_sPath);
    CPLString szNewName(ClearExt(sNewName).ToUTF8());
    CPLString szNewPath(CPLFormFilename(szDirPath, szNewName, GetExtension(m_sPath, szName)));


    if (!RenameFile(m_sPath, szNewPath))
    {
		wxString sErr = wxString::Format(_("Operation '%s' failed!"), _("Rename"));   
		sErr += wxT("\n") + wxString::Format(wxT("%s '%s' - '%s'"), GetCategory().c_str(), wxString::FromUTF8(m_sPath), wxString::FromUTF8(szNewPath));
		wxGISLogError(sErr, wxString::FromUTF8(CPLGetLastErrorMsg()), wxEmptyString, NULL);	
        return false;
    }
    else
    {
        m_sPath = szNewPath;
        m_sName = sNewName;
        //change event
        wxGIS_GXCATALOG_EVENT(ObjectChanged);
    }
    return true;
}


bool wxGxNGWService::Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
    bool bRet = CopyFile(m_sPath, szDestPath, pTrackCancel);

    if (!bRet)
    {
		wxString sErr = wxString::Format(_("Operation '%s' failed!"), _("Copy"));   
		sErr += wxT("\n") + wxString::Format(wxT("%s '%s'"), GetCategory().c_str(), wxString::FromUTF8(m_sPath));
		wxGISLogError(sErr, wxString::FromUTF8(CPLGetLastErrorMsg()), wxEmptyString, pTrackCancel);	
        return false;
    }

    return true;
}

bool wxGxNGWService::Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
    Disconnect();
    bool bRet = MoveFile(m_sPath, szDestPath, pTrackCancel);

    if (!bRet)
    {
		wxString sErr = wxString::Format(_("Operation '%s' failed!"), _("Move"));   
		sErr += wxT("\n") + wxString::Format(wxT("%s '%s'"), GetCategory().c_str(), wxString::FromUTF8(m_sPath));
		wxGISLogError(sErr, wxString::FromUTF8(CPLGetLastErrorMsg()), wxEmptyString, pTrackCancel);	
        return false;
    }

    return true;
}

bool wxGxNGWService::ConnectToNGW()
{
    if (IsConnected())
    {
        return true;
    }

    wxGISCurl curl;
    if (!curl.IsOk())
    {
         return false;
    }

    wxString sURL = m_sURL + wxString(wxT("/login"));
    wxString sPostData = wxString::Format(wxT("login=%s&password=%s"), m_sLogin.ToUTF8(), m_sPassword.ToUTF8());

    PERFORMRESULT res = curl.Post(sURL, sPostData);

    if(!res.IsValid)
        return false;

    m_bIsConnected = true;

    int pos;
    if((pos = res.sHead.Find(wxT("Set-Cookie"))) != wxNOT_FOUND)
    {
        m_bIsAuthorized = true;
        m_sAuthCookie = res.sHead.Right(res.sHead.Len() - 11 - pos);
        pos = m_sAuthCookie.Find(wxT("\r\n"));
        if(pos != wxNOT_FOUND)
        {
            m_sAuthCookie = m_sAuthCookie.Left(pos);
        }
    }

    return true;
}

bool wxGxNGWService::Connect(void)
{
    if(!ConnectToNGW())
        return false;

    LoadChildren();
    wxGIS_GXCATALOG_EVENT(ObjectChanged);

    return true;
}

bool wxGxNGWService::Disconnect(void)
{
    if (!IsConnected())
    {
        return true;
    }

    DestroyChildren();
    wxGIS_GXCATALOG_EVENT(ObjectChanged);

    m_bChildrenLoaded = false;
    m_bIsConnected = false;

    return true;
}

bool wxGxNGWService::IsConnected()
{
    return m_bIsConnected;
}

void wxGxNGWService::Refresh(void)
{
    DestroyChildren();
    LoadChildren();
    wxGxObject::Refresh();
}

bool wxGxNGWService::HasChildren(void)
{
    LoadChildren();
    return wxGxObjectContainer::HasChildren();
}

void wxGxNGWService::LoadChildren(void)
{
    if (m_bChildrenLoaded)
        return;
    if(!m_bIsConnected)
    {
        ConnectToNGW();
        if(!m_bIsConnected)
            return;
    }


    new wxGxNGWRootResource(this, this, _("Resources"), CPLString(m_sURL.ToUTF8()));

/*    if(m_bIsAuthorized)
        new wxGxNGWRoot(this, _("Administration"), CPLString(m_sURL.ToUTF8()));*/

    m_bChildrenLoaded = true;
}

bool wxGxNGWService::CanCreate(long nDataType, long DataSubtype)
{
    return false;
}

wxGISCurl wxGxNGWService::GetCurl()
{
    wxGISCurl curl;
    curl.AppendHeader(wxT("Cookie:") + m_sAuthCookie);
    return curl;
}

wxString wxGxNGWService::GetLogin() const
{
	return m_sLogin;
}

wxString wxGxNGWService::GetPassword() const
{
	return m_sPassword;
}

wxString wxGxNGWService::GetURL() const
{
	return m_sURL;
}

//--------------------------------------------------------------
//class wxGxNGWRootResource
//--------------------------------------------------------------
IMPLEMENT_CLASS(wxGxNGWRootResource, wxGxNGWResourceGroup)

wxGxNGWRootResource::wxGxNGWRootResource(wxGxNGWService *pService, wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxNGWResourceGroup(pService, wxJSONValue(), oParent, soName, soPath)
{
    m_nRemoteId = 0;
    m_sName = wxString(_("Resources"));
	m_sPath = CPLFormFilename(soPath, "resources", "");
}

wxGxNGWRootResource::~wxGxNGWRootResource(void)
{
}

bool wxGxNGWRootResource::CanDelete(void)
{
	return false;
}

bool wxGxNGWRootResource::CanRename(void)
{
	return false;
}

bool wxGxNGWRootResource::CanCopy(const CPLString &szDestPath)
{
	return false;
}

bool wxGxNGWRootResource::CanMove(const CPLString &szDestPath)
{
	return false;
}

//--------------------------------------------------------------
// wxGxNGWResource
//--------------------------------------------------------------
wxGxNGWResource::wxGxNGWResource(const wxJSONValue &Data)
{
    wxJSONValue JSONResource = Data[wxT("resource")];
    m_bHasChildren = JSONResource[wxT("children")].AsBool();
    m_sDescription = JSONResource[wxT("description")].AsString();
    m_sDisplayName = JSONResource[wxT("display_name")].AsString();
    SetRemoteId( JSONResource[wxT("id")].AsInt() );
    //wxArrayString m_aInterfaces;
    m_sKeyName = JSONResource[wxT("keyname")].AsString();
    m_nOwnerId = JSONResource[wxT("owner_user")].AsInt();
    const wxJSONInternalArray* pArr = JSONResource[wxT("permissions")].AsArray();
    if(pArr)
    {
        for(size_t i = 0; i < pArr->size(); ++i)
        {
            m_aPermissions.Add(pArr->operator[](i).AsString());
        }
    }
    pArr = JSONResource[wxT("scopes")].AsArray();
    if(pArr)
    {
        for(size_t i = 0; i < pArr->size(); ++i)
        {
            m_aScopes.Add(pArr->operator[](i).AsString());
        }
    }
}

wxGxNGWResource::~wxGxNGWResource()
{

}

bool wxGxNGWResource::DeleteResource()
{
	wxGISCurl curl = m_pService->GetCurl();
    if(!curl.IsOk())
        return false;
	
	// DELETE /resource/0/child/4 0 - parentid 4 - curren id
    wxString sURL = m_pService->GetURL() + wxString::Format(wxT("/resource/%d/child/%d"), GetParentResourceId(), m_nRemoteId);
    PERFORMRESULT res = curl.Delete(sURL);
	
	bool bResult = res.IsValid && res.nHTTPCode < 400;
	
	if(bResult)
		return true;
		
	ReportError(res.nHTTPCode, res.sBody);
	
	return false;
}

bool wxGxNGWResource::RenameResource(const wxString &sNewName)
{
	wxGISCurl curl = m_pService->GetCurl();
    if(!curl.IsOk())
        return false;
		
	wxJSONValue val;
	val["resource"]["display_name"] = sNewName;
	wxJSONWriter writer(wxJSONWRITER_NO_INDENTATION | wxJSONWRITER_NO_LINEFEEDS);
	wxString sPayload;
	writer.Write(val, sPayload);	
		
	//wxString sPayload = teat;//wxString::Format(wxT("{\"resource\":{\"display_name\":\"%s\"}}"), sNewName.ToUTF8());
    wxString sURL = m_pService->GetURL() + wxString::Format(wxT("/resource/%d/child/%d"), GetParentResourceId(), m_nRemoteId);
    PERFORMRESULT res = curl.PutData(sURL, sPayload);
	
	bool bResult = res.IsValid && res.nHTTPCode < 400;
	
	if(bResult)
		return true;
		
	ReportError(res.nHTTPCode, res.sBody);
		
	return false;
}

bool wxGxNGWResource::MoveResource(int nResourceId)
{
	wxGISCurl curl = m_pService->GetCurl();
    if(!curl.IsOk())
        return false;
		
	wxJSONValue val;
	val["resource"]["parent"]["id"] = nResourceId;
	wxJSONWriter writer(wxJSONWRITER_NO_INDENTATION | wxJSONWRITER_NO_LINEFEEDS);
	wxString sPayload;
	writer.Write(val, sPayload);	
	
	//wxString sPayload = wxString::Format(wxT("{\"resource\":{\"parent\":{\"id\":\"%d\"}}}"), nResourceId);
    wxString sURL = m_pService->GetURL() + wxString::Format(wxT("/resource/%d/child/%d"), GetParentResourceId(), m_nRemoteId);
    PERFORMRESULT res = curl.PutData(sURL, sPayload);
	
	bool bResult = res.IsValid && res.nHTTPCode < 400;
	
	if(bResult)
		return true;
		
	ReportError(res.nHTTPCode, res.sBody);
		
	return false;
}

bool wxGxNGWResource::CanCopyResource(const CPLString &szDestPath)
{
	wxGxCatalogBase* pCatalog = GetGxCatalog();
    if (NULL == pCatalog)
    {
        return false;
    }	
    wxGxNGWResource* pGxDestNGWResource = dynamic_cast<wxGxNGWResource*>(pCatalog->FindGxObjectByPath(szDestPath));
	return NULL != pGxDestNGWResource;
}

bool wxGxNGWResource::CanMoveResource(const CPLString &szDestPath)
{
	wxGxCatalogBase* pCatalog = GetGxCatalog();
    if (NULL == pCatalog)
    {
        return false;
    }
    wxGxNGWResource* pGxDestNGWResource = dynamic_cast<wxGxNGWResource*>(pCatalog->FindGxObjectByPath(szDestPath));
    bool bIsSameService = NULL != pGxDestNGWResource && NULL != pGxDestNGWResource->GetNGWService() && NULL != GetNGWService() && pGxDestNGWResource->GetNGWService()->GetURL().IsSameAs(GetNGWService()->GetURL(), false);
	
	return bIsSameService;
}

void wxGxNGWResource::ReportError(int nHTTPCode, const wxString& sBody)
{
	wxString sErrCode = wxString::Format(_("Error code %d"), nHTTPCode);
	wxString sErr;		
	wxJSONReader reader;
    wxJSONValue  JSONRoot;
    int numErrors = reader.Parse(sBody, &JSONRoot);
    if(numErrors > 0 || !JSONRoot.HasMember("message"))
	{
		sErr = wxString (_("Unexpected error"));
	}	
	else
	{
		sErr = JSONRoot[wxT("message")].AsString();
	}
	
	wxString sFullError = sErr + wxT(" (") + sErrCode + wxT(")");
	CPLError(CE_Failure, CPLE_AppDefined, sFullError.ToUTF8());
}

wxGxNGWService *wxGxNGWResource::GetNGWService() const
{
	return m_pService;
}

wxString wxGxNGWResource::MakeKey(const wxString& sInputStr)
{
	wxString sOut = sInputStr;
	sOut = sOut.Trim(true).Trim(false);
	sOut.Replace(' ', '_');
	if(wxIsdigit(sOut[0]))
		sOut.Prepend(wxT("tab_"));
	return sOut;
}

/*

forbidden err
PUT /resource/0/child/8 HTTP/1.1
Host: bishop.gis.to
User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:32.0) Gecko/20100101 Firefox/32.0
Accept: application/json
Accept-Language: ru-RU,ru;q=0.8,en-US;q=0.5,en;q=0.3
Accept-Encoding: gzip, deflate
Content-Type: application/json; charset=UTF-8
X-Requested-With: XMLHttpRequest
Referer: http://bishop.gis.to/resource/8/update
Content-Length: 221
Cookie: tkt="e26584afc516303874e1aae3770d9ce0b49e041eefb8006900d0871626d5b73a63a25482e0b0f81ba502b3a558bcf542b2b85af33ee3d147e7a1374806e5ef4a541e13104!userid_type:int"; tkt="e26584afc516303874e1aae3770d9ce0b49e041eefb8006900d0871626d5b73a63a25482e0b0f81ba502b3a558bcf542b2b85af33ee3d147e7a1374806e5ef4a541e13104!userid_type:int"
Connection: keep-alive

{"resource":{"display_name":"test3","keyname":"qw4","parent":{"id":0},"permissions":[{"action":"deny","principal":{"id":"2"},"scope":"resource","permission":"update","identity":"","propagate":true}],"description":"rrr5"}}HTTP/1.1 403 Forbidden
Server: nginx/1.4.6 (Ubuntu)
Date: Sun, 21 Sep 2014 00:05:31 GMT
Content-Type: application/json; charset=UTF-8
Content-Length: 44
Connection: keep-alive

{"message": "Attribute 'keyname' forbidden"}

//change permissions
PUT /resource/0/child/8 HTTP/1.1
Host: bishop.gis.to
User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:32.0) Gecko/20100101 Firefox/32.0
Accept: application/json
Accept-Language: ru-RU,ru;q=0.8,en-US;q=0.5,en;q=0.3
Accept-Encoding: gzip, deflate
Content-Type: application/json; charset=UTF-8
X-Requested-With: XMLHttpRequest
Referer: http://bishop.gis.to/resource/8/update
Content-Length: 220
Cookie: tkt="e26584afc516303874e1aae3770d9ce0b49e041eefb8006900d0871626d5b73a63a25482e0b0f81ba502b3a558bcf542b2b85af33ee3d147e7a1374806e5ef4a541e13104!userid_type:int"; tkt="e26584afc516303874e1aae3770d9ce0b49e041eefb8006900d0871626d5b73a63a25482e0b0f81ba502b3a558bcf542b2b85af33ee3d147e7a1374806e5ef4a541e13104!userid_type:int"
Connection: keep-alive

{"resource":{"display_name":"test3","keyname":null,"parent":{"id":0},"permissions":[{"action":"deny","principal":{"id":"1"},"scope":"resource","permission":"update","identity":"","propagate":true}
*/

//--------------------------------------------------------------
//class wxGxNGWResourceGroup
//--------------------------------------------------------------
IMPLEMENT_CLASS(wxGxNGWResourceGroup, wxGxObjectContainerUpdater)

wxGxNGWResourceGroup::wxGxNGWResourceGroup(wxGxNGWService *pService, const wxJSONValue &Data, wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxObjectContainerUpdater(oParent, soName, soPath), wxGxNGWResource(Data)
{
    m_eResourceType = enumNGWResourceTypeResourceGroup;
    m_pService = pService;
    m_sName = m_sDisplayName;
	m_sPath = CPLFormFilename(soPath, m_sName.ToUTF8(), "");
	m_bHasGeoJSON =  NULL != GetOGRCompatibleDriverByName(GetDriverByType(enumGISFeatureDataset, enumVecGeoJSON).mb_str());
}

wxGxNGWResourceGroup::~wxGxNGWResourceGroup()
{

}

wxGISEnumNGWResourcesType wxGxNGWResourceGroup::GetType(const wxJSONValue &Data) const
{
    wxString sType = Data[wxT("resource")][wxT("cls")].AsString();
    wxGISEnumNGWResourcesType eType = enumNGWResourceTypeNone;
    if(sType.IsSameAs(wxT("resource_group")))
        eType = enumNGWResourceTypeResourceGroup;
    else if(sType.IsSameAs(wxT("postgis_layer")))
        eType = enumNGWResourceTypePostgisLayer;
    else if(sType.IsSameAs(wxT("wmsserver_service")))
        eType = enumNGWResourceTypeWMSServerService;
    else if(sType.IsSameAs(wxT("baselayers")))
        eType = enumNGWResourceTypeBaseLayers;
    else if(sType.IsSameAs(wxT("postgis_connection")))
        eType = enumNGWResourceTypePostgisConnection;
    else if(sType.IsSameAs(wxT("webmap")))
        eType = enumNGWResourceTypeWebMap;
    else if(sType.IsSameAs(wxT("wfsserver_service")))
        eType = enumNGWResourceTypeWFSServerService;
	else if(sType.IsSameAs(wxT("vector_layer")))
		eType = enumNGWResourceTypeVectorLayer;

    return eType;
}

wxGxObject* wxGxNGWResourceGroup::AddResource(const wxJSONValue &Data)
{
    wxGISEnumNGWResourcesType eType = GetType(Data);
	wxGxObject* pReturnObj(NULL);
    switch(eType)
    {
    case enumNGWResourceTypeResourceGroup:
        pReturnObj = wxDynamicCast(new wxGxNGWResourceGroup(m_pService, Data, this, wxEmptyString, m_sPath), wxGxObject);
        break;
    case enumNGWResourceTypePostgisLayer:
		if(m_bHasGeoJSON)
			pReturnObj = wxDynamicCast(new wxGxNGWLayer(m_pService, enumNGWResourceTypePostgisLayer, Data, this, wxEmptyString, m_sPath), wxGxObject);
        break;
    case enumNGWResourceTypeVectorLayer:
        if(m_bHasGeoJSON)
			pReturnObj = wxDynamicCast(new wxGxNGWLayer(m_pService, enumNGWResourceTypeVectorLayer, Data, this, wxEmptyString, m_sPath), wxGxObject);
        break;
	case enumNGWResourceTypePostgisConnection:	
	if(m_bHasGeoJSON)
			pReturnObj = wxDynamicCast(new wxGxNGWPostGISConnection(m_pService, Data, this, wxEmptyString, m_sPath), wxGxObject);
        break;
    }
	
	return pReturnObj;
}

void wxGxNGWResourceGroup::Refresh(void)
{
    DestroyChildren();
	m_bChildrenLoaded = false;
    LoadChildren();
    wxGxObject::Refresh();
}

bool wxGxNGWResourceGroup::HasChildren(void)
{
    LoadChildren();

    CreateAndRunThread();

    return wxGxObjectContainer::HasChildren();
}

bool wxGxNGWResourceGroup::CanCreate(long nDataType, long DataSubtype)
{
    if (nDataType == enumGISContainer && DataSubtype == enumContNGWResourceGroup)
    {
        return true;
    } 

    if (nDataType == enumGISFeatureDataset)
    {
        return true;
    }
	
    if (nDataType == enumGISTable)
    {
        return false;
    }
	
    if (nDataType == enumGISRasterDataset)
    {
        return false;
    }
	
    return false;
}

bool wxGxNGWResourceGroup::CanDelete(void)
{
    //TODO: check permissions
    return m_pService != NULL;
}

bool wxGxNGWResourceGroup::CanRename(void)
{
    //TODO: check permissions
    return m_pService != NULL;
}

bool wxGxNGWResourceGroup::Delete(void)
{
    if( DeleteResource() )
	{
		IGxObjectNotifier *pNotify = dynamic_cast<IGxObjectNotifier*>(m_oParent);
		if(pNotify)
		{
			pNotify->OnGetUpdates();
		}
		return true;
	}
	return false;
}

bool wxGxNGWResourceGroup::Rename(const wxString &sNewName)
{
    if( RenameResource(sNewName) )
	{
		IGxObjectNotifier *pNotify = dynamic_cast<IGxObjectNotifier*>(m_oParent);
		if(pNotify)
		{
			pNotify->OnGetUpdates();
		}
		return true;
	}
	return false;
}

bool wxGxNGWResourceGroup::Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
    return false;
}

bool wxGxNGWResourceGroup::CanCopy(const CPLString &szDestPath)
{
	return CanCopyResource(szDestPath);
}

bool wxGxNGWResourceGroup::Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
    return false;
}

bool wxGxNGWResourceGroup::CanMove(const CPLString &szDestPath)
{
	if(!CanMoveResource(szDestPath))
		return CanCopy(szDestPath) && CanDelete();
	return true;
}

wxGxObjectMap wxGxNGWResourceGroup::GetRemoteObjects()
{
	wxGxObjectMap ret;
    wxGISCurl curl = m_pService->GetCurl();
    if(!curl.IsOk())
	{
		CPLError(CE_Failure, CPLE_AppDefined, "libcurl initialize failed!");
        return ret;
	}

    wxString sURL = m_pService->GetURL() + wxString::Format(wxT("/resource/%d/child/"), m_nRemoteId);
    PERFORMRESULT res = curl.Get(sURL);
	
	bool bResult = res.IsValid && res.nHTTPCode < 400;  
	if(!bResult)
	{  
		ReportError(res.nHTTPCode, res.sBody);	
		return ret;
	}
	
    wxJSONReader reader;
    wxJSONValue  JSONRoot;
    int numErrors = reader.Parse(res.sBody, &JSONRoot);
    if (numErrors > 0)  {    
        return ret;
    }

    const wxJSONInternalArray* pArr = JSONRoot.AsArray();
    if(pArr)
    {
        for(size_t i = 0; i < pArr->size(); ++i)
        {
            wxJSONValue JSONVal = pArr->operator[](i);
			wxJSONValue JSONResource = JSONVal[wxT("resource")];
			wxString sName = JSONResource[wxT("display_name")].AsString();
			int nId = JSONResource[wxT("id")].AsInt();
			
			ret[nId] = sName;
			m_moJSONData[nId] = JSONVal;
        }
    }

	return ret; 	
}

void wxGxNGWResourceGroup::LoadChildren(void)
{
    if (m_bChildrenLoaded)
        return;
		
	m_bChildrenLoaded = true;
	m_smObjects = GetRemoteObjects();
	if (m_smObjects.empty())
        return;
		
	
	for (wxNGWResourceDataMap::const_iterator it = m_moJSONData.begin(); it != m_moJSONData.end(); ++it)
	{
		AddResource(it->second);
    }
}

void wxGxNGWResourceGroup::AddObject(int nRemoteId, const wxString &sName)
{
	wxGxObject* pObj = AddResource(m_moJSONData[nRemoteId]);
	if(NULL != pObj)
	{
		wxGIS_GXCATALOG_EVENT_ID(ObjectAdded, pObj->GetId());
	}
}

int wxGxNGWResourceGroup::GetParentResourceId() const
{
	wxGxNGWResource* pParentResource = dynamic_cast<wxGxNGWResource*>(m_oParent);
	if(NULL == pParentResource)
		return wxNOT_FOUND;
	return pParentResource->GetRemoteId();
}

wxString wxGxNGWResourceGroup::CheckUniqName(const wxString& sTableName, const wxString& sAdd, int nCounter) const
{
    wxString sResultName;
    if (nCounter > 0)
    {
        sResultName = sTableName + wxString::Format(wxT("%s%d"), sAdd.c_str(), nCounter);
    }
    else
    {
        sResultName = sTableName;
    }

	for (wxGxObjectMap::const_iterator it = m_smObjects.begin(); it != m_smObjects.end(); ++it)
	{
		if(it->second.IsSameAs(sResultName, false))
		{
			return CheckUniqName(sTableName, sAdd, nCounter + 1);
		}
	}
	return sResultName;
}

bool wxGxNGWResourceGroup::CreateResource(const wxString &sName, wxGISEnumNGWResourcesType eType)
{
	switch(eType)
	{
		case enumNGWResourceTypeResourceGroup:
			return CreateResourceGroup(sName);
		default:
			break;	
	}
	return false;
}

bool wxGxNGWResourceGroup::CreateResourceGroup(const wxString &sName)
{
	wxGISCurl curl = m_pService->GetCurl();
    if(!curl.IsOk())
        return false;
	
	// {"resource":{"cls":"resource_group","parent":{"id":0},"display_name":"test","keyname":"test_key","description":"qqq"}}
	wxJSONValue val;
	val["resource"]["cls"] = wxString(wxT("resource_group"));
	val["resource"]["parent"]["id"] = m_nRemoteId;
	val["resource"]["display_name"] = sName;
	wxJSONWriter writer(wxJSONWRITER_NO_INDENTATION | wxJSONWRITER_NO_LINEFEEDS);
	wxString sPayload;
	writer.Write(val, sPayload);
	
    //wxString sPayload = wxString::Format(wxT("{\"resource\":{\"cls\":\"resource_group\",\"parent\":{\"id\":%d},\"display_name\":\"%s\"}}"), m_nRemoteId, sName.ToUTF8());
    wxString sURL = m_pService->GetURL() + wxString::Format(wxT("/resource/%d/child/"), m_nRemoteId);
    PERFORMRESULT res = curl.Post(sURL, sPayload);
	bool bResult = res.IsValid && res.nHTTPCode < 400;
	
	if(bResult)
	{
		OnGetUpdates();
		return true;		
	}
		
	ReportError(res.nHTTPCode, res.sBody);	
		
	return false;	
}

bool wxGxNGWResourceGroup::CreatePostGISLayer(const wxString &sName, int nPGConnId, const wxString &sTable, const wxString &sSchema, const wxString &sFid, const wxString &sGeom)
{
	wxGISCurl curl = m_pService->GetCurl();
    if(!curl.IsOk())
        return false;	
		
		//{"resource":{"cls":"postgis_layer","parent":{"id":0},"display_name":"test","keyname":null,"description":null},"postgis_layer":{"connection":{"id":31},"table":"roads","schema":"thematic","column_id":"ogc_fid","column_geom":"wkb_geometry","geometry_type":null,"fields":"update","srs":{"id":3857}}}
	wxJSONValue val;
	val["resource"]["cls"] = wxString(wxT("postgis_layer"));
	val["resource"]["parent"]["id"] = m_nRemoteId;
	val["resource"]["display_name"] = sName;
	val["postgis_layer"]["connection"]["id"] = nPGConnId;
	val["postgis_layer"]["table"] = sTable;
	val["postgis_layer"]["schema"] = sSchema;
	val["postgis_layer"]["column_id"] = sFid;
	val["postgis_layer"]["column_geom"] = sGeom;
	val["postgis_layer"]["fields"] = wxString(wxT("update"));
	val["postgis_layer"]["srs"]["id"] = 3857;
	wxJSONWriter writer(wxJSONWRITER_NO_INDENTATION | wxJSONWRITER_NO_LINEFEEDS);
	wxString sPayload;
	writer.Write(val, sPayload);
	
	//wxString sPayload = wxString::Format(wxT("{\"resource\":{\"cls\":\"postgis_layer\",\"parent\":{\"id\":%d},\"display_name\":\"%s\"}, \"postgis_layer\":{\"connection\":{\"id\":%d},\"table\":\"%s\",\"schema\":\"%s\",\"column_id\":\"%s\", \"column_geom\":\"%s\",\"fields\":\"update\",\"srs\":{\"id\":3857}}}"), m_nRemoteId, sName.ToUTF8(), nPGConnId, sTable.ToUTF8(), sSchema.ToUTF8(), sFid.ToUTF8(), sGeom.ToUTF8());
	wxString sURL = m_pService->GetURL() + wxString::Format(wxT("/resource/%d/child/"), m_nRemoteId);
    PERFORMRESULT res = curl.Post(sURL, sPayload);
	bool bResult = res.IsValid && res.nHTTPCode < 400;
	
	if(bResult)
	{
		OnGetUpdates();
		return true;		
	}
		
	ReportError(res.nHTTPCode, res.sBody);	
		
	return false;		
}

bool wxGxNGWResourceGroup::CreatePostGISConnection(const wxString &sName, const wxString &sServer, const wxString &sDatabase, const wxString &sUser, const wxString &sPassword)
{
	wxGISCurl curl = m_pService->GetCurl();
    if(!curl.IsOk())
        return false;
	
	// {"resource":{"cls":"postgis_connection","parent":{"id":0},"display_name":"gis lab info","keyname":"gis-lab","description":"gis-lab PostGIS Connection"},"postgis_connection":{"hostname":"gis-lab.info","database":"rosavto","username":"user","password":"secret"}}
	
	wxJSONValue val;
	val["resource"]["cls"] = wxString(wxT("postgis_connection"));
	val["resource"]["parent"]["id"] = m_nRemoteId;
	val["resource"]["display_name"] = sName;
	val["postgis_connection"]["hostname"] = sServer;
	val["postgis_connection"]["database"] = sDatabase;
	val["postgis_connection"]["username"] = sUser;
	val["postgis_connection"]["password"] = sPassword;
	wxJSONWriter writer(wxJSONWRITER_NO_INDENTATION | wxJSONWRITER_NO_LINEFEEDS);
	wxString sPayload;
	writer.Write(val, sPayload);
	
    //wxString sPayload = wxString::Format(wxT("{\"resource\":{\"cls\":\"postgis_connection\",\"parent\":{\"id\":%d},\"display_name\":\"%s\"}, \"postgis_connection\":{\"hostname\":\"%s\",\"database\":\"%s\",\"username\":\"%s\",\"password\":\"%s\"}}"), m_nRemoteId, sName.ToUTF8(), sServer.ToUTF8(), sDatabase.ToUTF8(), sUser.ToUTF8(), sPassword.ToUTF8());
    wxString sURL = m_pService->GetURL() + wxString::Format(wxT("/resource/%d/child/"), m_nRemoteId);
    PERFORMRESULT res = curl.Post(sURL, sPayload);
	bool bResult = res.IsValid && res.nHTTPCode < 400;
	
	if(bResult)
	{
		OnGetUpdates();
		return true;		
	}
		
	ReportError(res.nHTTPCode, res.sBody);	
		
	return false;	
}

void wxGxNGWResourceGroup::ValidateDataset( wxGISFeatureDataset* const pSrcDataSet, OGRwkbGeometryType eFilterGeomType, bool bToMulti, ITrackCancel* const pTrackCancel )
{
	IProgressor *pProgressor(NULL);
	if(pTrackCancel)
	{
		pProgressor = pTrackCancel->GetProgressor();
	}
	
	
	//1. check fields
	
	OGRFeatureDefn* pDef = pSrcDataSet->GetDefinition();
	if(NULL == pDef)
    {
        if (pTrackCancel)
        {
            pTrackCancel->PutMessage(_("Error read dataset definition"), wxNOT_FOUND, enumGISMessageError);
        }
        return;
    }
	
	wxString sWrongFields;
	
	for (size_t i = 0; i < pDef->GetFieldCount(); ++i)
	{
		OGRFieldDefn* pFieldDefn = pDef->GetFieldDefn(i);
		if (NULL != pFieldDefn)
		{
			wxString sFieldName(pFieldDefn->GetNameRef(), wxConvUTF8);
			if (IsFieldNameForbidden(sFieldName))
			{
				sWrongFields += sFieldName + wxString(wxT(", "));
			}
		}
	}
	
	if(!sWrongFields.IsEmpty())
	{
        if (pTrackCancel)
        {
			wxString sWarn = wxString(_("The exist field name(s) '%s' are forbidden. They will be renamed or deleted"), sWrongFields.c_str());
            pTrackCancel->PutMessage(sWarn, wxNOT_FOUND, enumGISMessageWarning);
        }		
	}
	
	//2. check spatial reference
	wxGISSpatialReference SpaRef = pSrcDataSet->GetSpatialReference();
	OGRCoordinateTransformation *poCT = OGRCreateCoordinateTransformation( SpaRef, new OGRSpatialReference(SRS_WKT_WGS84) );
	if(NULL == poCT)
	{
		if (pTrackCancel)
        {
			wxString sErr = wxString(_("Unsupported spatial reference '%s'"), SpaRef.GetName().c_str());
            pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageError);
        }
		OCTDestroyCoordinateTransformation(poCT);
		return;
	}
	else
	{
		OCTDestroyCoordinateTransformation(poCT);
	}
	
	//3. check topology
	wxGISFeature Feature;
	size_t nCounter = 0;
    OGRwkbGeometryType eGeoFieldtype = pSrcDataSet->GetGeometryType();
    while ((Feature = pSrcDataSet->Next()).IsOk())
    {
        if(pTrackCancel && !pTrackCancel->Continue())
        {
            wxString sErr(_("Interrupted by user"));
            if (pTrackCancel)
            {
                pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageError);
            }

            return;
        }

        //set geometry
        wxGISGeometry Geom = Feature.GetGeometry();
        if (Geom.IsOk())
        {
            OGRwkbGeometryType eGeomType = Geom.GetType();
            if (eFilterGeomType != wkbUnknown)
            {
                OGRwkbGeometryType eTestGeomType = eGeomType;
                if (bToMulti && wkbFlatten(eGeomType) > 1 && wkbFlatten(eGeomType) < 4)
                {
                    eTestGeomType = (OGRwkbGeometryType)(eGeomType + 3);
                }

                if (eFilterGeomType != eTestGeomType)
                {
                    if (pProgressor)
                    {
                        pProgressor->SetValue(nCounter++);
                    }
                    continue;
                }
            }

            if (eGeoFieldtype != eGeomType)
            {                
				wxString sType(OGRGeometryTypeToName(wkbFlatten(eGeoFieldtype)), wxConvUTF8);
				wxString sWarn = wxString::Format(_("Force geom type to %s"), sType.c_str());
				if (pTrackCancel)
				{
					pTrackCancel->PutMessage(sWarn, wxNOT_FOUND, enumGISMessageWarning);
				}

            }
			
			OGRGeometry* pGeom = Geom;
			if(!pGeom->IsValid())
			{
				wxString sErr = wxString::Format(_("The geometry #%d is not valid"), nCounter);
				if (pTrackCancel)
				{
					pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageError);
				}
				return;
			}
			nCounter++;        
		}
	}
}

bool wxGxNGWResourceGroup::IsFieldNameForbidden(const wxString& sTestFieldName) const
{
	if(sTestFieldName.IsSameAs(wxT("id"), false))
		return true;
	for(size_t i = 0; i < sTestFieldName.size(); ++i)
	{
		if(sTestFieldName[i] > 127)
			return true;
	}
	return false;	
}

bool wxGxNGWResourceGroup::CanStoreMultipleGeometryTypes() const
{
	return false;
}
	
//--------------------------------------------------------------
//class wxGxNGWLayer
//--------------------------------------------------------------

IMPLEMENT_CLASS(wxGxNGWLayer, wxGxFeatureDataset)

wxGxNGWLayer::wxGxNGWLayer(wxGxNGWService *pService, wxGISEnumNGWResourcesType eType, const wxJSONValue &Data, wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxFeatureDataset(enumVecGeoJSON, oParent, soName, soPath), wxGxNGWResource(Data)
{
    m_eResourceType = eType;
    m_pService = pService;
    m_sName = m_sDisplayName;
	m_sPath = CPLFormFilename(soPath, m_sName.ToUTF8(), "");
}

wxGxNGWLayer::~wxGxNGWLayer()
{

}

wxString wxGxNGWLayer::GetCategory(void) const
{ 
	switch(m_eResourceType)
	{
		case enumNGWResourceTypeVectorLayer:
			return wxString(_("NGW vector layer")); 
		case enumNGWResourceTypePostgisLayer:
			return wxString(_("NGW PostGIS layer")); 
		default:
			return wxEmptyString;
	}
}

wxGISDataset* const wxGxNGWLayer::GetDatasetFast(void)
{
 	if(m_pwxGISDataset == NULL)
    {
		wxString sURL = m_pService->GetURL() + wxString::Format(wxT("/resource/%d/geojson/"), m_nRemoteId);
		wxString sAuth = m_pService->GetLogin() + wxT(":") + m_pService->GetPassword();
		CPLSetConfigOption("GDAL_HTTP_USERPWD", sAuth.mb_str());
        wxGISFeatureDataset* pDSet = new wxGISFeatureDatasetCached(CPLString(sURL.ToUTF8()), m_eType);
        m_pwxGISDataset = wxStaticCast(pDSet, wxGISDataset);
        m_pwxGISDataset->Reference();
    }
    wsGET(m_pwxGISDataset);
}


int wxGxNGWLayer::GetParentResourceId() const
{
	wxGxNGWResource* pParentResource = dynamic_cast<wxGxNGWResource*>(m_oParent);
	if(NULL == pParentResource)
		return wxNOT_FOUND;
	return pParentResource->GetRemoteId();
}


bool wxGxNGWLayer::CanDelete(void)
{
    //TODO: check permissions
    return m_pService != NULL;
}

bool wxGxNGWLayer::CanRename(void)
{
    //TODO: check permissions
    return m_pService != NULL;
}

bool wxGxNGWLayer::Delete(void)
{
    if( DeleteResource() )
	{
		IGxObjectNotifier *pNotify = dynamic_cast<IGxObjectNotifier*>(m_oParent);
		if(pNotify)
		{
			pNotify->OnGetUpdates();
		}
		return true;
	}
	return false;
}

bool wxGxNGWLayer::Rename(const wxString &sNewName)
{
    if( RenameResource(sNewName) )
	{
		IGxObjectNotifier *pNotify = dynamic_cast<IGxObjectNotifier*>(m_oParent);
		if(pNotify)
		{
			pNotify->OnGetUpdates();
		}
		return true;
	}
	return false;
}

bool wxGxNGWLayer::Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{	
    return false;
}

bool wxGxNGWLayer::CanCopy(const CPLString &szDestPath)
{
	return CanCopyResource(szDestPath);
}

bool wxGxNGWLayer::Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
    return false;
}

bool wxGxNGWLayer::CanMove(const CPLString &szDestPath)
{
	if(!CanMoveResource(szDestPath))
		return CanCopy(szDestPath) && CanDelete();
	return true;	
}

//change fields
//PUT /ore/resource/37/child/127
//
//{"resource":{"display_name":"water_polygon","keyname":"water_polygon","parent":{"id":37},"permissions":[],"description":null},"feature_layer":{"fields":[{"id":140,"keyname":"OSM_ID2","datatype":"REAL","typemod":null,"display_name":"OSM_ID 3","label_field":false,"grid_visibility":true},{"id":141,"keyname":"NAME2","datatype":"STRING","typemod":null,"display_name":"NAME 3","label_field":false,"grid_visibility":true},{"id":142,"keyname":"NATURAL","datatype":"STRING","typemod":null,"display_name":"NATURAL","label_field":false,"grid_visibility":true},{"id":143,"keyname":"WATERWAY","datatype":"STRING","typemod":null,"display_name":"WATERWAY","label_field":false,"grid_visibility":true},{"id":144,"keyname":"WETLAND","datatype":"STRING","typemod":null,"display_name":"WETLAND","label_field":false,"grid_visibility":true}]},"postgis_layer":{"connection":{"id":14,"parent":{"id":0}},"table":"water_polygon","schema":"topo_osm","column_id":"ogc_fid","column_geom":"wkb_geometry","geometry_type":"POLYGON","fields":"keep","srs":{"id":3857}}}
//
//http://176.9.38.120/ore/resource/127/field/
//
//[{"display_name": "OSM_ID 3", "idx": 0, "datatype": "REAL", "layer_id": 127, "grid_visibility": true, "keyname": "OSM_ID2", "id": 140, "cls": "postgis_layer"}, {"display_name": "NAME 3", "idx": 1, "datatype": "STRING", "layer_id": 127, "grid_visibility": true, "keyname": "NAME2", "id": 141, "cls": "postgis_layer"}, {"display_name": "NATURAL", "idx": 2, "datatype": "STRING", "layer_id": 127, "grid_visibility": true, "keyname": "NATURAL", "id": 142, "cls": "postgis_layer"}, {"display_name": "WATERWAY", "idx": 3, "datatype": "STRING", "layer_id": 127, "grid_visibility": true, "keyname": "WATERWAY", "id": 143, "cls": "postgis_layer"}, {"display_name": "WETLAND", "idx": 4, "datatype": "STRING", "layer_id": 127, "grid_visibility": true, "keyname": "WETLAND", "id": 144, "cls": "postgis_layer"}]


//--------------------------------------------------------------
// wxGxNGWPostGISConnection
//--------------------------------------------------------------

IMPLEMENT_CLASS(wxGxNGWPostGISConnection, wxGxRemoteConnection)

wxGxNGWPostGISConnection::wxGxNGWPostGISConnection(wxGxNGWService *pService, const wxJSONValue &Data, wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxRemoteConnection(oParent, soName, soPath), wxGxNGWResource(Data)
{
    m_eResourceType = enumNGWResourceTypePostgisConnection;
    m_pService = pService;
    m_sName = m_sDisplayName;
	m_sPath = CPLFormFilename(soPath, m_sName.ToUTF8(), "");
	
	wxJSONValue JSONConn = Data[wxT("postgis_connection")];
	m_sUser =  JSONConn[wxT("username")].AsString();
	m_sPass =  JSONConn[wxT("password")].AsString();
	m_sDatabase =  JSONConn[wxT("database")].AsString();
	m_sHost =  JSONConn[wxT("hostname")].AsString();
}

wxGxNGWPostGISConnection::~wxGxNGWPostGISConnection()
{

}

wxString wxGxNGWPostGISConnection::GetCategory(void) const
{ 
	return wxString(_("NGW PostGIS Connection")); 
}

wxGISDataset* const wxGxNGWPostGISConnection::GetDatasetFast(void)
{
 	if(m_pwxGISDataset == NULL)
    {
        wxGISPostgresDataSource* pDSet = new wxGISPostgresDataSource(m_sUser, m_sPass, wxT("5432"), m_sHost, m_sDatabase);
        m_pwxGISDataset = wxStaticCast(pDSet, wxGISDataset);
        m_pwxGISDataset->Reference();
    }
    wsGET(m_pwxGISDataset);
}

int wxGxNGWPostGISConnection::GetParentResourceId() const
{
	wxGxNGWResource* pParentResource = dynamic_cast<wxGxNGWResource*>(m_oParent);
	if(NULL == pParentResource)
		return wxNOT_FOUND;
	return pParentResource->GetRemoteId();
}


bool wxGxNGWPostGISConnection::CanDelete(void)
{
    //TODO: check permissions
    return m_pService != NULL;
}

bool wxGxNGWPostGISConnection::CanRename(void)
{
    //TODO: check permissions
    return m_pService != NULL;
}

bool wxGxNGWPostGISConnection::Delete(void)
{
    if( DeleteResource() )
	{
		IGxObjectNotifier *pNotify = dynamic_cast<IGxObjectNotifier*>(m_oParent);
		if(pNotify)
		{
			pNotify->OnGetUpdates();
		}
		return true;
	}
	return false;
}

bool wxGxNGWPostGISConnection::Rename(const wxString &sNewName)
{
    if( RenameResource(sNewName) )
	{
		IGxObjectNotifier *pNotify = dynamic_cast<IGxObjectNotifier*>(m_oParent);
		if(pNotify)
		{
			pNotify->OnGetUpdates();
		}
		return true;
	}
	return false;
}

bool wxGxNGWPostGISConnection::Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
	
    return false;
}

bool wxGxNGWPostGISConnection::CanCopy(const CPLString &szDestPath)
{
	return CanCopyResource(szDestPath);
}

bool wxGxNGWPostGISConnection::Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
    return false;
}

bool wxGxNGWPostGISConnection::CanMove(const CPLString &szDestPath)
{
	if(!CanMoveResource(szDestPath))
		return CanCopy(szDestPath) && CanDelete();
	return true;	
}

#endif // wxGIS_USE_CURL
