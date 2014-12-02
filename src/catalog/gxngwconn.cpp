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
#include "wxgis/catalog/gxfolder.h"

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
	m_staCustomMetadata.clear();
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
			
			
			wxXmlNode* pMetadataNode = pRootNode->GetChildren();
			while(pMetadataNode)
			{
				wxString sName = pMetadataNode->GetName();
				if(sName.IsSameAs(wxT("metadata")))
					break;
				pMetadataNode = pMetadataNode->GetNext();
			}
			
			if(pMetadataNode)
			{
				wxXmlNode* pMetadataItemNode = pMetadataNode->GetChildren();
				while(pMetadataItemNode)
				{
					wxString sType = pMetadataItemNode->GetAttribute(wxT("type"));
					wxString sName = pMetadataItemNode->GetAttribute(wxT("name"));
					wxString sValue = pMetadataItemNode->GetAttribute(wxT("value"));
					CUSTOM_METADATA_ITEM item = {sName, sType, sValue};
					m_staCustomMetadata.push_back(item);
					pMetadataItemNode = pMetadataItemNode->GetNext();
				}
			}
        }
    }
}

wxGxNGWService::~wxGxNGWService(void)
{
}

const wxVector<wxGxNGWService::CUSTOM_METADATA_ITEM>& wxGxNGWService::GetCustomMetadata() const
{
	return m_staCustomMetadata;
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
	
	ReadConnectionFile();

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

bool wxGxNGWService::HasChildren(bool bWaitLoading)
{
    LoadChildren();
    return wxGxObjectContainer::HasChildren(bWaitLoading);
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


    wxGxNGWRootResource* pGxNGWRootResource = new wxGxNGWRootResource(this, this, _("Resources"), CPLString(m_sURL.ToUTF8()));
	pGxNGWRootResource->FillPermissions();

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
	wxString sURL = m_pService->GetURL() + wxString::Format(wxT("/resource/%d"), 0);
	m_sPath = CPLString(sURL.ToUTF8());
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
    wxJSONValue JSONResource = Data["resource"];
    m_bHasChildren = JSONResource["children"].AsBool();
    m_sDescription = JSONResource["description"].AsString();
    m_sDisplayName = JSONResource["display_name"].AsString();
    SetRemoteId( JSONResource["id"].AsInt() );
    //wxArrayString m_aInterfaces;
    m_sKeyName = JSONResource["keyname"].AsString();
    m_nOwnerId = JSONResource["owner_user"].AsInt();
	/*
    const wxJSONInternalArray* pArr = JSONResource["permissions"].AsArray();
    if(pArr)
    {
        for(size_t i = 0; i < pArr->size(); ++i)
        {
            m_aPermissions.Add(pArr->operator[](i).AsString());
        }
    }
    pArr = JSONResource["scopes"].AsArray();
    if(pArr)
    {
        for(size_t i = 0; i < pArr->size(); ++i)
        {
            m_aScopes.Add(pArr->operator[](i).AsString());
        }
    }*/
	
	m_oMetadata = Data["resmeta"]["items"];
	
	m_oPermissions = wxJSONValue(wxJSONTYPE_INVALID);
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
	{
		m_sDisplayName = sNewName;
		return true;
	}
		
	ReportError(res.nHTTPCode, res.sBody);
		
	return false;
}

bool wxGxNGWResource::UpdateResource(const wxString &sNewName, const wxString &sNewKey, const wxString &sNewDescription)
{
	wxGISCurl curl = m_pService->GetCurl();
    if(!curl.IsOk())
        return false;
		
	wxJSONValue val;
	if(!sNewName.IsEmpty())
		val["resource"]["display_name"] = sNewName;
	val["resource"]["keyname"] = sNewKey;
	val["resource"]["description"] = sNewDescription;
	wxJSONWriter writer(wxJSONWRITER_NO_INDENTATION | wxJSONWRITER_NO_LINEFEEDS);
	wxString sPayload;
	writer.Write(val, sPayload);	
		
	//wxString sPayload = teat;//wxString::Format(wxT("{\"resource\":{\"display_name\":\"%s\"}}"), sNewName.ToUTF8());
    wxString sURL = m_pService->GetURL() + wxString::Format(wxT("/resource/%d/child/%d"), GetParentResourceId(), m_nRemoteId);
    PERFORMRESULT res = curl.PutData(sURL, sPayload);
	
	bool bResult = res.IsValid && res.nHTTPCode < 400;
	
	if(bResult)
	{
		if(!sNewName.IsEmpty())
			m_sDisplayName = sNewName;
		m_sKeyName = sNewKey;
		m_sDescription = sNewDescription;
		return true;
	}
		
	ReportError(res.nHTTPCode, res.sBody);
		
	return false;
}

bool wxGxNGWResource::UpdateResourceDescritpion(const wxString &sNewDescription)
{
	wxGISCurl curl = m_pService->GetCurl();
    if(!curl.IsOk())
        return false;
		
	wxJSONValue val;
	val["resource"]["description"] = sNewDescription;
	wxJSONWriter writer(wxJSONWRITER_NO_INDENTATION | wxJSONWRITER_NO_LINEFEEDS);
	wxString sPayload;
	writer.Write(val, sPayload);	
		
	//wxString sPayload = teat;//wxString::Format(wxT("{\"resource\":{\"display_name\":\"%s\"}}"), sNewName.ToUTF8());
    wxString sURL = m_pService->GetURL() + wxString::Format(wxT("/resource/%d/child/%d"), GetParentResourceId(), m_nRemoteId);
    PERFORMRESULT res = curl.PutData(sURL, sPayload);
	
	bool bResult = res.IsValid && res.nHTTPCode < 400;
	
	if(bResult)
	{
		m_sDescription = sNewDescription;
		return true;
	}
		
	ReportError(res.nHTTPCode, res.sBody);
		
	return false;
}


bool wxGxNGWResource::UpdateResourceMetadata(const wxJSONValue &oNewMetadata)
{
	wxGISCurl curl = m_pService->GetCurl();
    if(!curl.IsOk())
        return false;
		
	//m_oMetadata
	wxArrayString saKeys = oNewMetadata.GetMemberNames();
	for ( size_t i = 0; i < saKeys.GetCount(); ++i ) 
	{    
		m_oMetadata.Remove(saKeys[i]);
		m_oMetadata[saKeys[i]] = oNewMetadata[saKeys[i]];
	}

		
	wxJSONValue val;
	val["resmeta"]["items"] = m_oMetadata;
	wxJSONWriter writer(wxJSONWRITER_NO_INDENTATION | wxJSONWRITER_NO_LINEFEEDS);
	wxString sPayload;
	writer.Write(val, sPayload);	
		
	//wxString sPayload = teat;//wxString::Format(wxT("{\"resource\":{\"display_name\":\"%s\"}}"), sNewName.ToUTF8());
    wxString sURL = m_pService->GetURL() + wxString::Format(wxT("/resource/%d/child/%d"), GetParentResourceId(), m_nRemoteId);
    PERFORMRESULT res = curl.PutData(sURL, sPayload);
	
	bool bResult = res.IsValid && res.nHTTPCode < 400;
	
	if(bResult)
	{
		return true;
	}
		
	ReportError(res.nHTTPCode, res.sBody);
		
	return false;	
}

bool wxGxNGWResource::FillPermissions()
{
	wxGISCurl curl = m_pService->GetCurl();
    if(!curl.IsOk())
        return false;
		
    wxString sURL = m_pService->GetURL() + wxString::Format(wxT("/api/resource/%d/permission"), m_nRemoteId);
    PERFORMRESULT res = curl.Get(sURL);
	
	bool bResult = res.IsValid && res.nHTTPCode < 400;
	
	if(bResult)
	{
		wxJSONReader reader;
		int numErrors = reader.Parse(res.sBody, &m_oPermissions);
	
		return numErrors == 0;
	}
		
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
	
	wxGxObject* pGxObj = pCatalog->FindGxObjectByPath(szDestPath);
	
	//1. CanCopy to File System
	//return  pGxObj && pGxObj->IsKindOf(wxCLASSINFO(wxGxFolder));
	
	//2. CanCopy to other NGW resource group
	//return pGxObj && pGxObj->IsKindOf(wxCLASSINFO(wxGxNGWResource));
	
	return false;
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
		sErr = JSONRoot["message"].AsString();
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

wxJSONValue wxGxNGWResource::MakeMetadata(wxGISDataset* const pDSet)
{
	wxJSONValue out;
	if(NULL == pDSet)
		return out;
		
	switch(pDSet->GetType())
	{
		case enumGISFeatureDataset:
		{
			wxGISFeatureDataset *pFeatureDataset = wxDynamicCast(pDSet, wxGISFeatureDataset);
			if(pFeatureDataset)
			{
				OGRCompatibleDataSource *pDataSource = pFeatureDataset->GetDataSourceRef();
				if(pDataSource)
				{
					OGRCompatibleDriver* pDrv = dynamic_cast<OGRCompatibleDriver*>(pDataSource->GetDriver());
					if(pDrv)
					{
						out["Driver"] = wxString::FromUTF8(pDrv->GetOGRCompatibleDriverName());
					}
					
					OGRLayer *poLayer = pDataSource->GetLayer(0);
					
					wxString sOut = GetConvName(poLayer->GetName(), false);
					if(!sOut.IsEmpty())
					{						
						out["Name"]	= sOut;
					}
					out["Geometry type"] = wxString(OGRGeometryTypeToName( poLayer->GetGeomType() ), wxConvLocal);
					out["Feature count"] = poLayer->GetFeatureCount();
					
					//fields
					wxString sFields;
					OGRFeatureDefn* const poDefn = poLayer->GetLayerDefn();
					if(poDefn)
					{						
						for( int iAttr = 0; iAttr < poDefn->GetFieldCount(); iAttr++ )
						{
							OGRFieldDefn    *poField = poDefn->GetFieldDefn( iAttr );
							wxString sFieldTypeName = wxString( poField->GetFieldTypeName( poField->GetType() ), wxConvLocal );
							wxString sFieldName = wxString(poField->GetNameRef(), wxConvLocal);
							if(sFields.IsEmpty())
								sFields += sFieldName + wxT(" (") +  sFieldTypeName + wxT(")");
							else	
								sFields += wxT(", ") + sFieldName + wxT(" (") +  sFieldTypeName + wxT(")");
						}
					}
					out["Fields"] = sFields;
					
					//SRS
					wxGISSpatialReference SpaRef = poLayer->GetSpatialRef();
					bool bProjected(false);
					if(SpaRef.IsOk())
					{
						out["CRS"] = SpaRef.GetName();
						if(SpaRef->IsProjected())
							bProjected = true;
					}
					
					//Extent
					OGREnvelope Extent;
					OGRErr eErr = poLayer->GetExtent(&Extent);
					if(eErr == OGRERR_NONE && Extent.IsInit())
					{
						out["Extent"] = wxString::Format(wxT("%f, %f, %f, %f"), Extent.MinX, Extent.MinY, Extent.MaxX, Extent.MaxY);
						out["Center"] = wxString::Format(wxT("%f, %f"), (Extent.MaxX + Extent.MinX) / 2.0, (Extent.MaxY + Extent.MinY) / 2.0);
						if(bProjected)
						{
							wxGISSpatialReference GSSpaRef(SpaRef->CloneGeogCS());
							if(GSSpaRef.IsOk())
							{
								OGRCoordinateTransformation *poCT = OGRCreateCoordinateTransformation( SpaRef, GSSpaRef );
								if(poCT)
								{
									poCT->Transform(1, &Extent.MaxX, &Extent.MaxY);
									poCT->Transform(1, &Extent.MinX, &Extent.MinY);
									
									out["Extent (geo)"] = wxString::Format(wxT("%f, %f, %f, %f"), Extent.MinX, Extent.MinY, Extent.MaxX, Extent.MaxY);
									out["Center (geo)"] = wxString::Format(wxT("%f, %f"), (Extent.MaxX + Extent.MinX) / 2.0, (Extent.MaxY + Extent.MinY) / 2.0);
									OCTDestroyCoordinateTransformation(poCT);
								}
							}
						}
					}
				}
			}
		}
			break;
		case enumGISRasterDataset:
		{
			wxGISRasterDataset *pRasterDataset = wxDynamicCast(pDSet, wxGISRasterDataset);
			if(pRasterDataset)
			{
				GDALDataset* poGDALDataset = pRasterDataset->GetMainRaster();
				if(!poGDALDataset)
					poGDALDataset = pRasterDataset->GetRaster();
				if(poGDALDataset)
				{
					GDALDriver* pDrv = poGDALDataset->GetDriver();
					out["Driver"] = wxString::Format(wxT("%s(%s)"), wxString::FromUTF8(pDrv->GetMetadataItem( GDAL_DMD_LONGNAME )).c_str(), wxString::FromUTF8(pDrv->GetDescription()).c_str() );					
					
					int nW = pRasterDataset->GetWidth();
					int nH = pRasterDataset->GetHeight();
					
					out["Columns"] = nW;
					out["Rows"] = nH;
					out["Number of bands"] = pRasterDataset->GetBandCount();
					double adfGeoTransform[6] = {0};
					if( poGDALDataset->GetGeoTransform( adfGeoTransform ) == CE_None )
					{
						out["Cell size (X, Y)"] = wxString::Format(wxT("%.6g, %.6g"), fabs(adfGeoTransform[1]), fabs(adfGeoTransform[5]) );
						//out["GeoTransform"] = wxString::Format(wxT("%.16g, %.16g, %.16g | %.16g, %.16g, %.16g"), adfGeoTransform[0], adfGeoTransform[1], adfGeoTransform[2], adfGeoTransform[3], adfGeoTransform[4], adfGeoTransform[5]);
					}
					
					//SRS
					wxGISSpatialReference SpaRef = pRasterDataset->GetSpatialReference();
					bool bProjected(false);
					if(SpaRef.IsOk())
					{
						out["CRS"] = SpaRef.GetName();
						if(SpaRef->IsProjected())
							bProjected = true;
					}
					
					//Extent
					
					OGREnvelope Extent = pRasterDataset->GetEnvelope();
					if(Extent.IsInit())
					{
						out["Extent"] = wxString::Format(wxT("%f, %f, %f, %f"), Extent.MinX, Extent.MinY, Extent.MaxX, Extent.MaxY);
						out["Center"] = wxString::Format(wxT("%f, %f"), (Extent.MaxX + Extent.MinX) / 2.0, (Extent.MaxY + Extent.MinY) / 2.0);
						if(bProjected)
						{
							wxGISSpatialReference GSSpaRef(SpaRef->CloneGeogCS());
							if(GSSpaRef.IsOk())
							{
								OGRCoordinateTransformation *poCT = OGRCreateCoordinateTransformation( SpaRef, GSSpaRef );
								if(poCT)
								{
									poCT->Transform(1, &Extent.MaxX, &Extent.MaxY);
									poCT->Transform(1, &Extent.MinX, &Extent.MinY);
									
									out["Extent (geo)"] = wxString::Format(wxT("%f, %f, %f, %f"), Extent.MinX, Extent.MinY, Extent.MaxX, Extent.MaxY);
									out["Center (geo)"] = wxString::Format(wxT("%f, %f"), (Extent.MaxX + Extent.MinX) / 2.0, (Extent.MaxY + Extent.MinY) / 2.0);
									OCTDestroyCoordinateTransformation(poCT);
								}
							}
						}
					}

					char** papszMetadata = poGDALDataset->GetMetadata();
					if( CSLCount(papszMetadata) > 0 )
					{
						for(int i = 0; papszMetadata[i] != NULL; ++i )
						{
							char* Key = NULL;
							const char* Value = CPLParseNameValue(papszMetadata[i], &Key);
							wxString sKey = wxString::FromUTF8(Key);
							if(!sKey.IsEmpty())
								out["IMAGE_METADATA." + sKey] = wxString::FromUTF8(Value);
						}
					}
					
					papszMetadata = poGDALDataset->GetMetadata("IMAGERY");
					if( CSLCount(papszMetadata) > 0 )
					{
						for(int i = 0; papszMetadata[i] != NULL; ++i )
						{
							char* Key = NULL;
							const char* Value = CPLParseNameValue(papszMetadata[i], &Key);
							wxString sKey = wxString::FromUTF8(Key);
							if(!sKey.IsEmpty())
								out["IMAGERY." + sKey] = wxString::FromUTF8(Value);
						}
					}		
					
					
					papszMetadata = poGDALDataset->GetMetadata("IMAGE_STRUCTURE");
					if( CSLCount(papszMetadata) > 0 )
					{
						for(int i = 0; papszMetadata[i] != NULL; ++i )
						{
							char* Key = NULL;
							const char* Value = CPLParseNameValue(papszMetadata[i], &Key);
							wxString sKey = wxString::FromUTF8(Key);
							if(!sKey.IsEmpty())
								out["IMAGE_STRUCTURE." + sKey] = wxString::FromUTF8(Value);
						}
					}

					papszMetadata = poGDALDataset->GetMetadata("SUBDATASETS");
					if( CSLCount(papszMetadata) > 0 )
					{
						for(int i = 0; papszMetadata[i] != NULL; ++i )
						{
							char* Key = NULL;
							const char* Value = CPLParseNameValue(papszMetadata[i], &Key);
							wxString sKey = wxString::FromUTF8(Key);
							if(!sKey.IsEmpty())
								out["SUBDATASETS." + sKey] = wxString::FromUTF8(Value);
						}
					}

					papszMetadata = poGDALDataset->GetMetadata("IMD");
					if( CSLCount(papszMetadata) > 0 )
					{
						for(int i = 0; papszMetadata[i] != NULL; ++i )
						{
							char* Key = NULL;
							const char* Value = CPLParseNameValue(papszMetadata[i], &Key);
							wxString sKey = wxString::FromUTF8(Key);
							if(!sKey.IsEmpty())
								out["IMD." + sKey] = wxString::FromUTF8(Value);
						}
					}

					papszMetadata = poGDALDataset->GetMetadata("GEOLOCATION");
					if( CSLCount(papszMetadata) > 0 )
					{
						for(int i = 0; papszMetadata[i] != NULL; ++i )
						{
							char* Key = NULL;
							const char* Value = CPLParseNameValue(papszMetadata[i], &Key);
							wxString sKey = wxString::FromUTF8(Key);
							if(!sKey.IsEmpty())
								out["GEOLOCATION." + sKey] = wxString::FromUTF8(Value);
						}
					}

					papszMetadata = poGDALDataset->GetMetadata("RPC");
					if( CSLCount(papszMetadata) > 0 )
					{
						out["RPC"] = wxString(_("present"));
						/*for(int i = 0; papszMetadata[i] != NULL; ++i )
						{
							char* Key = NULL;
							const char* Value = CPLParseNameValue(papszMetadata[i], &Key);
							wxString sKey = wxString::FromUTF8(Key);
							if(!sKey.IsEmpty())
								out["RPC." + sKey] = wxString::FromUTF8(Value);
						}*/
					}
					
					if(poGDALDataset->GetGCPCount() > 0)
					{
						out["GCP Count"] = poGDALDataset->GetGCPCount();
					}
				}
			}
		}
			break;
		case enumGISContainer:
		wxGISFeatureDataset *pFeatureDataset = wxDynamicCast(pDSet, wxGISFeatureDataset);
			if(pFeatureDataset)
			{
				OGRCompatibleDataSource *pDataSource = pFeatureDataset->GetDataSourceRef();
				if(pDataSource)
				{
					OGRCompatibleDriver* pDrv = dynamic_cast<OGRCompatibleDriver*>(pDataSource->GetDriver());
					if(pDrv)
					{
						out["Driver"] = wxString::FromUTF8(pDrv->GetOGRCompatibleDriverName());
					}
					out["Layer Count"]	= pDataSource->GetLayerCount();	
					
					wxString sLayers;
					for( int iLayer = 0; iLayer < pDataSource->GetLayerCount(); ++iLayer )
					{
						OGRLayer *poLayer = pDataSource->GetLayer(iLayer);
						if(poLayer)
						{
							if(sLayers.IsEmpty())
								sLayers += wxString::FromUTF8(poLayer->GetName());
							else	
								sLayers += wxT(", ") + wxString::FromUTF8(poLayer->GetName());
						}
					}
					
					if(!sLayers.IsEmpty())
						out["Layers"] = sLayers;	
				}
			}
			break;
	};
	
	return out;
}

const wxString& wxGxNGWResource::GetResourceName() const
{
	return m_sDisplayName;
}

const wxString& wxGxNGWResource::GetResourceKey() const
{
	return m_sKeyName;
}

const wxString& wxGxNGWResource::GetResourceDescription() const
{
	return m_sDescription;
}

const wxJSONValue& wxGxNGWResource::GetMetadata() const
{
	return m_oMetadata;
}

/*
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
// wxGxNGWResourceWithStyles
//--------------------------------------------------------------
wxGxNGWResourceWithStyles::wxGxNGWResourceWithStyles(const wxJSONValue &Data) : wxGxNGWResource(Data)
{
}

wxGxNGWResourceWithStyles::~wxGxNGWResourceWithStyles()
{
	ClearStyles();
}

void wxGxNGWResourceWithStyles::ClearStyles()
{
	for ( size_t i = 0; i < m_apStyles.size(); ++i ) 
	{    
		wxDELETE(m_apStyles[i]);
	}	
}

bool wxGxNGWResourceWithStyles::GetStyles()
{
    wxGISCurl curl = m_pService->GetCurl();
    if(!curl.IsOk())
	{
		CPLError(CE_Failure, CPLE_AppDefined, "libcurl initialize failed!");
        return false;
	}

    wxString sURL = m_pService->GetURL() + wxString::Format(wxT("/resource/%d/child/"), m_nRemoteId);
    PERFORMRESULT res = curl.Get(sURL);
	
	bool bResult = res.IsValid && res.nHTTPCode < 400;  
	if(!bResult)
	{  
		ReportError(res.nHTTPCode, res.sBody);	
		return false;
	}
	
    wxJSONReader reader;
    wxJSONValue  JSONRoot;
    int numErrors = reader.Parse(res.sBody, &JSONRoot);
    if (numErrors > 0)  {    
        return false;
    }

	ClearStyles();
    const wxJSONInternalArray* pArr = JSONRoot.AsArray();
    if(pArr)
    {
        for(size_t i = 0; i < pArr->size(); ++i)
        {
            wxJSONValue JSONVal = pArr->operator[](i);
			
			wxGxNGWStyle* pStyle = new wxGxNGWStyle(JSONVal, m_nRemoteId);
			m_apStyles.push_back(pStyle);
        }
    }

	return true;
}

//--------------------------------------------------------------
// wxGxNGWStyle
//--------------------------------------------------------------
wxGxNGWStyle::wxGxNGWStyle(const wxJSONValue &Data, int nParentRemoteId) : wxGxNGWResource(Data)
{
	m_nParentRemoteId = nParentRemoteId;
}

wxGxNGWStyle::~wxGxNGWStyle()
{
	
}

int wxGxNGWStyle::GetParentResourceId() const
{
	return m_nParentRemoteId;
}

//--------------------------------------------------------------
//class wxGxNGWResourceGroup
//--------------------------------------------------------------
IMPLEMENT_CLASS(wxGxNGWResourceGroup, wxGxObjectContainerUpdater)

wxGxNGWResourceGroup::wxGxNGWResourceGroup(wxGxNGWService *pService, const wxJSONValue &Data, wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxObjectContainerUpdater(oParent, soName, soPath), wxGxNGWResource(Data)
{
    m_eResourceType = enumNGWResourceTypeResourceGroup;
    m_pService = pService;
    m_sName = m_sDisplayName;
	wxString sURL = m_pService->GetURL() + wxString::Format(wxT("/resource/%d"), m_nRemoteId);
	m_sPath = CPLString(sURL.ToUTF8());
	m_bHasGeoJSON =  NULL != GetOGRCompatibleDriverByName(GetDriverByType(enumGISFeatureDataset, enumVecGeoJSON).mb_str());
	m_bHasWMS = NULL != GDALGetDriverByName(GetDriverByType(enumGISRasterDataset, enumRasterWMSTMS).mb_str());
	m_bHasPostGIS = NULL != GetOGRCompatibleDriverByName(GetDriverByType(enumGISTable, enumTablePostgres).mb_str());
}

wxGxNGWResourceGroup::~wxGxNGWResourceGroup()
{

}

void wxGxNGWResourceGroup::RenameObject(int nRemoteId, const wxString &sNewName)
{
	wxGxObject *pObj = GetChildByRemoteId(nRemoteId);
	if(NULL != pObj)
	{
		pObj->SetName(sNewName);
		CPLString szNewSchemaName(pObj->GetName().ToUTF8());
		wxGIS_GXCATALOG_EVENT_ID(ObjectChanged, pObj->GetId());
	}
}

wxGISEnumNGWResourcesType wxGxNGWResourceGroup::GetType(const wxJSONValue &Data) const
{
    wxString sType = Data["resource"]["cls"].AsString();
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
	else if(sType.IsSameAs(wxT("raster_layer")))
		eType = enumNGWResourceTypeRasterLayer;
    else if (sType.IsSameAs(wxT("file_bucket")))
        eType = enumNGWResourceTypeFileSet;
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
	    if(m_bHasPostGIS)
			pReturnObj = wxDynamicCast(new wxGxNGWPostGISConnection(m_pService, Data, this, wxEmptyString, m_sPath), wxGxObject);
        break;
	case enumNGWResourceTypeRasterLayer:	
	    if(m_bHasWMS)
			pReturnObj = wxDynamicCast(new wxGxNGWRaster(m_pService, Data, this, wxEmptyString, m_sPath), wxGxObject);
        break;
    case enumNGWResourceTypeFileSet:
        pReturnObj = wxDynamicCast(new wxGxNGWFileSet(m_pService, Data, this, wxEmptyString, m_sPath), wxGxObject);
        break;	
    }
	
	wxGxNGWResource* pGxNGWResource = dynamic_cast<wxGxNGWResource*>(pReturnObj);
	if(pGxNGWResource)
		pGxNGWResource->FillPermissions();
	
	return pReturnObj;
}

void wxGxNGWResourceGroup::Refresh(void)
{
    DestroyChildren();
	m_bChildrenLoaded = false;
    LoadChildren();
    wxGxObject::Refresh();
}

bool wxGxNGWResourceGroup::HasChildren(bool bWaitLoading)
{
    LoadChildren();

    CreateAndRunThread();

    return wxGxObjectContainer::HasChildren(bWaitLoading);
}

bool wxGxNGWResourceGroup::CanCreate(long nDataType, long DataSubtype)
{
	
	if(m_oPermissions.IsValid())
	{
		bool bCanCreate = m_oPermissions["resource"]["create"].AsBool();
		if(!bCanCreate)
			return false;
	}
	
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
    //check permissions
	if(m_oPermissions.IsValid())
	{
		bool bCanDelete = m_oPermissions["resource"]["delete"].AsBool();
		return bCanDelete && m_pService != NULL;
	}
    return m_pService != NULL;
}

bool wxGxNGWResourceGroup::CanRename(void)
{
    //check permissions
	if(m_oPermissions.IsValid())
	{
		bool bCanRename = m_oPermissions["resource"]["update"].AsBool();
		return bCanRename && m_pService != NULL;
	}
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
	return true;
	//return CanCopyResource(szDestPath);
}

bool wxGxNGWResourceGroup::Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
	wxGxCatalogBase* pCatalog = GetGxCatalog();
    if (NULL == pCatalog)
    {
        return false;
    }    
	
	wxGxObject* pGxObj = pCatalog->FindGxObjectByPath(szDestPath);
	if (CanMoveResource(szDestPath))
	{
		//move to resource
		wxGxNGWResource* pGxNGWResource = dynamic_cast<wxGxNGWResource*>(pGxObj);
		bool bRes = MoveResource(pGxNGWResource->GetRemoteId());
		//report error
		if(!bRes)
		{
			wxString sErr = wxString::Format(_("Operation '%s' failed!"), _("Move"));  
			sErr += wxT("\n") + wxString::Format(wxT("%s '%s'"), GetCategory().c_str(), wxString::FromUTF8(m_sPath));
			wxGISLogError(sErr, wxString::FromUTF8(CPLGetLastErrorMsg()), wxEmptyString, pTrackCancel);
			return false;
		}
		else
		{
			wxGxObject* pObj = GetParent();
			if(NULL != pObj)
			{
				IGxObjectNotifier *pNotify = dynamic_cast<IGxObjectNotifier*>(pObj->GetParent());
				if(pNotify)
				{	
					pNotify->OnGetUpdates();
				}						
			}
			
			if(NULL != pGxObj)
			{
				IGxObjectNotifier *pNotify = dynamic_cast<IGxObjectNotifier*>(pGxObj->GetParent());
				if(pNotify)
				{	
					pNotify->OnGetUpdates();
				}						
			}
			return true;	
		}
	}	
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
    wxCriticalSectionLocker lock(m_CritSect);
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
			wxJSONValue JSONResource = JSONVal["resource"];
			wxString sName = JSONResource["display_name"].AsString();
			int nId = JSONResource["id"].AsInt();
			
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

bool wxGxNGWResourceGroup::CreateVectorLayer(const wxString &sName, wxGISDataset * const pInputDataset, OGRwkbGeometryType eFilterGeomType, ITrackCancel* const pTrackCancel)
{
	
	return false;		
}

bool wxGxNGWResourceGroup::CreateRasterLayer(const wxString &sName, wxGISDataset * const pInputDataset, unsigned char R, unsigned char G, unsigned char B, unsigned char A, bool bAutoCrop, ITrackCancel* const pTrackCancel)
{

    return false;

}

bool wxGxNGWResourceGroup::CreatePostGISLayer(const wxString &sName, int nPGConnId, const wxString &sTable, const wxString &sSchema, const wxString &sFid, const wxString &sGeom)
{
	wxGISCurl curl = m_pService->GetCurl();
    if(!curl.IsOk())
	{
		return false;
	}
		
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
	{
        return false;
	}
	
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

bool wxGxNGWResourceGroup::CreateFileBucket(const wxString &sName, const wxArrayString& asPaths, const wxJSONValue& oMetadata, ITrackCancel* const pTrackCancel)
{
	wxGISCurl curl = m_pService->GetCurl();
    if(!curl.IsOk())
	{
		if (pTrackCancel)
        {
            pTrackCancel->PutMessage(_("cURL initialize failed."), wxNOT_FOUND, enumGISMessageError);
        }
        return false;
	}
	
	//1. Upload files
	//POST /rekod/file_upload/upload
	//{"upload_meta": [{"id": "b5c02d94-e1d7-40cf-b9c7-79bc9cca429d", "name": "grunt_area_2_multipolygon.cpg", "mime_type": "application/octet-stream", "size": 5}, {"id": "d8457f14-39cb-4f9d-bb00-452a381fa62e", "name": "grunt_area_2_multipolygon.dbf", "mime_type": "application/x-dbf", "size": 36607}, {"id": "1b0754f8-079d-4675-9367-36531da247e1", "name": "grunt_area_2_multipolygon.prj", "mime_type": "application/octet-stream", "size": 138}, {"id": "a34b5ab3-f3a5-4a60-835d-318e601d34df", "name": "grunt_area_2_multipolygon.shp", "mime_type": "application/x-esri-shape", "size": 65132}, {"id": "fb439bfa-1a63-4384-957d-ae57bb5eb67b", "name": "grunt_area_2_multipolygon.shx", "mime_type": "application/x-esri-shape", "size": 1324}]}
	
	if (pTrackCancel)
    {
        pTrackCancel->PutMessage(_("Upload files"), wxNOT_FOUND, enumGISMessageTitle);
    }
	
	wxString sURL = m_pService->GetURL() + wxString(wxT("/file_upload/upload"));
    PERFORMRESULT res = curl.UploadFiles(sURL, asPaths, pTrackCancel);
	bool bResult = res.IsValid && res.nHTTPCode < 400;
	
	if(bResult)
	{
		wxJSONReader reader;
		wxJSONValue  JSONRoot;
		int numErrors = reader.Parse(res.sBody, &JSONRoot);
		if (numErrors > 0)  {    
			if (pTrackCancel)
			{
				pTrackCancel->PutMessage(_("Unexpected error"), wxNOT_FOUND, enumGISMessageError);
			}
			return false;
		}
		
		if (pTrackCancel)
        {
            pTrackCancel->PutMessage(_("Create files set"), wxNOT_FOUND, enumGISMessageTitle);
        }
	
	
		//2. Create bucket
		//POST /rekod/resource/0/child/
		//{"resource":{"cls":"file_bucket","parent":{"id":0},"display_name":"grunt_area","keyname":null,"description":null},"file_bucket":{"files":[{"id":"b5c02d94-e1d7-40cf-b9c7-79bc9cca429d","name":"grunt_area_2_multipolygon.cpg","mime_type":"application/octet-stream","size":5},{"id":"d8457f14-39cb-4f9d-bb00-452a381fa62e","name":"grunt_area_2_multipolygon.dbf","mime_type":"application/x-dbf","size":36607},{"id":"1b0754f8-079d-4675-9367-36531da247e1","name":"grunt_area_2_multipolygon.prj","mime_type":"application/octet-stream","size":138},{"id":"a34b5ab3-f3a5-4a60-835d-318e601d34df","name":"grunt_area_2_multipolygon.shp","mime_type":"application/x-esri-shape","size":65132},{"id":"fb439bfa-1a63-4384-957d-ae57bb5eb67b","name":"grunt_area_2_multipolygon.shx","mime_type":"application/x-esri-shape","size":1324}]}}
		
		wxJSONValue val;
		val["resource"]["cls"] = wxString(wxT("file_bucket"));
		val["resource"]["parent"]["id"] = m_nRemoteId;
		val["resource"]["display_name"] = sName;
		val["file_bucket"]["files"] = JSONRoot["upload_meta"];
		if(oMetadata.IsValid())
			val["resmeta"]["items"] = oMetadata;
		
		wxJSONWriter writer(wxJSONWRITER_NO_INDENTATION | wxJSONWRITER_NO_LINEFEEDS);
		wxString sPayload;
		writer.Write(val, sPayload);
		
		sURL = m_pService->GetURL() + wxString::Format(wxT("/resource/%d/child/"), m_nRemoteId);
        res = curl.Post(sURL, sPayload, pTrackCancel);
		bResult = res.IsValid && res.nHTTPCode < 400;
		
		if(bResult)
		{
			OnGetUpdates();
			return true;		
		}		
	}

	wxString sErrCode = wxString::Format(_("Error code %ld"), res.nHTTPCode);
	wxString sErr;		
	wxJSONReader reader;
    wxJSONValue  JSONRoot;
    int numErrors = reader.Parse(res.sBody, &JSONRoot);
    if(numErrors > 0 || !JSONRoot.HasMember("message"))
	{
		sErr = wxString (_("Unexpected error"));
	}	
	else
	{
		sErr = JSONRoot["message"].AsString();
	}
	
	wxString sFullError = sErr + wxT(" (") + sErrCode + wxT(")");
	if (pTrackCancel)
	{
		pTrackCancel->PutMessage(sFullError, wxNOT_FOUND, enumGISMessageError);
	}
	
	return false;		
}

bool wxGxNGWResourceGroup::ValidateDataset( wxGISRasterDataset* const pSrcDataSet, ITrackCancel* const pTrackCancel )
{
	//0. check if warped
	if(!pSrcDataSet->IsOpened())
		pSrcDataSet->Open();
	if(pSrcDataSet->IsWarped())
	{
		if (pTrackCancel)
        {
            pTrackCancel->PutMessage(_("The warped images is not supported!"), wxNOT_FOUND, enumGISMessageError);
        }
		return false;
	}
	//1. check spatial reference
	wxGISSpatialReference SpaRef = pSrcDataSet->GetSpatialReference();
	wxGISSpatialReference SpaRefWGS(new OGRSpatialReference(SRS_WKT_WGS84));
	OGRCoordinateTransformation *poCT = OGRCreateCoordinateTransformation( SpaRef, SpaRefWGS);
	if(NULL == poCT)
	{
		if (pTrackCancel)
        {
			wxString sErr = wxString::Format(_("Unsupported spatial reference '%s'"), SpaRef.GetName().c_str());
            pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageError);
        }
		OCTDestroyCoordinateTransformation(poCT);
		return false;
	}
	else
	{
		OCTDestroyCoordinateTransformation(poCT);
	}
	
	//2. check towgs
	if(!SpaRef.IsSame(SpaRefWGS))
	{
		double adfPArams[7];
		if(SpaRef->GetTOWGS84(adfPArams) != OGRERR_NONE)
		{
			if (pTrackCancel)
			{
				wxString sLastMsg = pTrackCancel->GetLastMessage();
				wxString sWarn;
				if(sLastMsg.IsEmpty())
					sWarn = wxString(_("The ToWGS is not defined. There may by an error while project"));
				else
					sWarn = sLastMsg + wxString(wxT("\n")) + wxString(_("The ToWGS is not defined. There may by an error while project"));
				pTrackCancel->PutMessage(sWarn, wxNOT_FOUND, enumGISMessageWarning);
			}		
		}
	}
	
	//3. check channels
	if(pSrcDataSet->GetBandCount() < 3)
	{
		pTrackCancel->PutMessage(_("The band count is less than 3. The additional bands will form from first band"), wxNOT_FOUND, enumGISMessageWarning);
	}
	else if(pSrcDataSet->GetBandCount() > 4)
	{
		pTrackCancel->PutMessage(_("The band count is more than 4. You have to select RGB band and additional Apha band"), wxNOT_FOUND, enumGISMessageWarning);
	}
	
	return true;
}

bool wxGxNGWResourceGroup::ValidateDataset( wxGISFeatureDataset* const pSrcDataSet, OGRwkbGeometryType eFilterGeomType, ITrackCancel* const pTrackCancel )
{
	bool bToMulti = false;
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
        return bToMulti;
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
		sWrongFields = sWrongFields.Left(sWrongFields.Len() - 2);
        if (pTrackCancel)
        {
			wxString sWarn = wxString::Format(_("The exist field name(s) '%s' are forbidden. They will be renamed or deleted"), sWrongFields.c_str());
            pTrackCancel->PutMessage(sWarn, wxNOT_FOUND, enumGISMessageWarning);
        }		
	}
	
	//2. check spatial reference
	wxGISSpatialReference SpaRef = pSrcDataSet->GetSpatialReference();
	wxGISSpatialReference SpaRefWGS(new OGRSpatialReference(SRS_WKT_WGS84));
	OGRCoordinateTransformation *poCT = OGRCreateCoordinateTransformation( SpaRef, SpaRefWGS);
	if(NULL == poCT)
	{
		if (pTrackCancel)
        {
			wxString sErr = wxString::Format(_("Unsupported spatial reference '%s'"), SpaRef.GetName().c_str());
            pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageError);
        }
		OCTDestroyCoordinateTransformation(poCT);
		return bToMulti;
	}
	else
	{
		OCTDestroyCoordinateTransformation(poCT);
	}
	
	//3. check towgs
	if(!SpaRef.IsSame(SpaRefWGS))
	{
		double adfPArams[7];
		if(SpaRef->GetTOWGS84(adfPArams) != OGRERR_NONE)
		{
			if (pTrackCancel)
			{
				wxString sLastMsg = pTrackCancel->GetLastMessage();
				wxString sWarn;
				if(sLastMsg.IsEmpty())
					sWarn = wxString(_("The ToWGS is not defined. There may by an error while project"));
				else
					sWarn = sLastMsg + wxString(wxT("\n")) + wxString(_("The ToWGS is not defined. There may by an error while project"));
				pTrackCancel->PutMessage(sWarn, wxNOT_FOUND, enumGISMessageWarning);
			}		
		}
	}
	
	//4. check topology
	wxGISFeature Feature;
    OGRwkbGeometryType eGeoFieldtype = pSrcDataSet->GetGeometryType();
	
	wxArrayString saIgnoredFields = pSrcDataSet->GetFieldNames();
	saIgnoredFields.Add(wxT("OGR_STYLE"));
	pSrcDataSet->SetIgnoredFields(saIgnoredFields);
	pSrcDataSet->Reset();
			
    while ((Feature = pSrcDataSet->Next()).IsOk())
    {
        if(pTrackCancel && !pTrackCancel->Continue())
        {
            wxString sErr(_("Interrupted by user"));
            if (pTrackCancel)
            {
                pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageError);
            }
			saIgnoredFields.Clear();
            pSrcDataSet->SetIgnoredFields(saIgnoredFields);
			
            return bToMulti;
        }

		size_t nCounter(0);
        //set geometry
        wxGISGeometry Geom = Feature.GetGeometry();
        if (Geom.IsOk())
        {
			if(!bToMulti && eGeoFieldtype < Geom.GetType())
				bToMulti = true;
			if(!Geom.IsValid())
			{
				if (pTrackCancel)
				{
					wxString sWarnGeom = wxString::Format(_("The geometry in feature %ld is not valid. The feature will be ignored"), Feature.GetFID());
					wxString sLastMsg = pTrackCancel->GetLastMessage();
					wxString sWarn;
					if(sLastMsg.IsEmpty())
						sWarn = sWarnGeom;
					else
						sWarn = sLastMsg + wxString(wxT("\n")) + sWarnGeom;
					pTrackCancel->PutMessage(sWarn, wxNOT_FOUND, enumGISMessageWarning);
				}
			}
			
			if (pProgressor)
			{
				pProgressor->SetValue(nCounter++);
			}
		}
		else
		{
			if (pTrackCancel)
			{
				wxString sWarnGeom = wxString::Format(_("There is no geometry in feature %ld. The feature will be ignored"), Feature.GetFID());
				wxString sLastMsg = pTrackCancel->GetLastMessage();
				wxString sWarn;
				if(sLastMsg.IsEmpty())
					sWarn = sWarnGeom;
				else
					sWarn = sLastMsg + wxString(wxT("\n")) + sWarnGeom;
				pTrackCancel->PutMessage(sWarn, wxNOT_FOUND, enumGISMessageWarning);
			}			
		}
	}
	saIgnoredFields.Clear();
	pSrcDataSet->SetIgnoredFields(saIgnoredFields);
	
	return bToMulti;
}

bool wxGxNGWResourceGroup::IsFieldNameForbidden(const wxString& sTestFieldName) const
{
	if(sTestFieldName.IsEmpty())
		return true;
	if(sTestFieldName.Len() > 10)
		return true;
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

wxGxNGWLayer::wxGxNGWLayer(wxGxNGWService *pService, wxGISEnumNGWResourcesType eType, const wxJSONValue &Data, wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxFeatureDataset(enumVecGeoJSON, oParent, soName, soPath), wxGxNGWResourceWithStyles(Data)
{
    m_eResourceType = eType;
    m_pService = pService;
    m_sName = m_sDisplayName;
	wxString sURL = m_pService->GetURL() + wxString::Format(wxT("/resource/%d"), m_nRemoteId);
	m_sPath = CPLString(sURL.ToUTF8());
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
    //check permissions
	if(m_oPermissions.IsValid())
	{
		bool bCanDelete = m_oPermissions["resource"]["delete"].AsBool();
		return bCanDelete && m_pService != NULL;
	}
    return m_pService != NULL;
}

bool wxGxNGWLayer::CanRename(void)
{
    //check permissions
	if(m_oPermissions.IsValid())
	{
		bool bCanRename = m_oPermissions["resource"]["update"].AsBool();
		return bCanRename && m_pService != NULL;
	}
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
	return true;//CanCopyResource(szDestPath);
}

bool wxGxNGWLayer::Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
	wxGxCatalogBase* pCatalog = GetGxCatalog();
    if (NULL == pCatalog)
    {
        return false;
    }    
	
	wxGxObject* pGxObj = pCatalog->FindGxObjectByPath(szDestPath);
	if (CanMoveResource(szDestPath))
	{
		//move to resource
		wxGxNGWResource* pGxNGWResource = dynamic_cast<wxGxNGWResource*>(pGxObj);
		bool bRes = MoveResource(pGxNGWResource->GetRemoteId());
		//report error
		if(!bRes)
		{
			wxString sErr = wxString::Format(_("Operation '%s' failed!"), _("Move"));  
			sErr += wxT("\n") + wxString::Format(wxT("%s '%s'"), GetCategory().c_str(), wxString::FromUTF8(m_sPath));
			wxGISLogError(sErr, wxString::FromUTF8(CPLGetLastErrorMsg()), wxEmptyString, pTrackCancel);
			return false;
		}
		else
		{
			wxGxObject* pObj = GetParent();
			if(NULL != pObj)
			{
				IGxObjectNotifier *pNotify = dynamic_cast<IGxObjectNotifier*>(pObj->GetParent());
				if(pNotify)
				{	
					pNotify->OnGetUpdates();
				}						
			}
			
			if(NULL != pGxObj)
			{
				IGxObjectNotifier *pNotify = dynamic_cast<IGxObjectNotifier*>(pGxObj->GetParent());
				if(pNotify)
				{	
					pNotify->OnGetUpdates();
				}						
			}
			return true;	
		}
	}	
    return false;	
}

bool wxGxNGWLayer::CanMove(const CPLString &szDestPath)
{
	if(!CanMoveResource(szDestPath))
		return CanCopy(szDestPath) && CanDelete();
	return true;	
}

bool wxGxNGWLayer::CreateDefaultStyle(wxGxNGWService * const pService, int nParentId, const wxString & sStyleName, wxGISEnumNGWResourcesType eType, int nSubType, ITrackCancel* const pTrackCancel)
{
	wxGISCurl curl = pService->GetCurl();
    if(!curl.IsOk())
	{
		if (pTrackCancel)
        {
            pTrackCancel->PutMessage(_("cURL initialize failed."), wxNOT_FOUND, enumGISMessageError);
        }
        return false;
	}
	
	if (pTrackCancel)
	{
		pTrackCancel->PutMessage(wxString::Format(_("Create layer style '%s'"), sStyleName.c_str()), wxNOT_FOUND, enumGISMessageTitle);
	}

	wxJSONValue val;
	
	switch(eType)
	{
		case enumNGWResourceTypeRasterLayerStyle:
	//{"resource":{"cls":"raster_style","parent":{"id":392},"display_name":"test raster style","keyname":null,"description":null}}
			val["resource"]["cls"] = wxString(wxT("raster_style"));
			val["resource"]["parent"]["id"] = nParentId;
			val["resource"]["display_name"] = sStyleName;
			break;
		case enumNGWResourceTypeVectorLayerStyle:
			val["resource"]["cls"] = wxString(wxT("mapserver_style"));
			val["resource"]["parent"]["id"] = nParentId;
			val["resource"]["display_name"] = sStyleName;
			switch(nSubType)
			{
				case wkbPoint:
				case wkbMultiPoint:
					val["mapserver_style"]["xml"] = wxString(wxT("<map><symbol><type>ellipse</type><name>circle</name><points>1 1</points><filled>true</filled></symbol><layer><class><style><color blue=\"189\" green=\"128\" red=\"188\"/><outlinecolor blue=\"64\" green=\"64\" red=\"64\"/><symbol>circle</symbol><size>6</size></style></class></layer></map>"));
					break;
				case wkbLineString:
				case wkbMultiLineString:
				case wkbPolygon:
				case wkbMultiPolygon:
					val["mapserver_style"]["xml"] = wxString(wxT("<map><layer><class><style><color blue=\"218\" green=\"186\" red=\"190\"/><outlinecolor blue=\"64\" green=\"64\" red=\"64\"/></style></class></layer></map>"));
					break;
				default:
					return true;
			}
			break;
		default:
			return true;
	};
	
	wxJSONWriter writer(wxJSONWRITER_NO_INDENTATION | wxJSONWRITER_NO_LINEFEEDS);
	wxString sPayload;
	writer.Write(val, sPayload);
		
	wxString sURL = pService->GetURL() + wxString::Format(wxT("/resource/%d/child/"), nParentId);
	PERFORMRESULT res = curl.Post(sURL, sPayload, pTrackCancel);
	bool bResult = res.IsValid && res.nHTTPCode < 400;
	if(bResult)
		return true;
	
	wxString sErrCode = wxString::Format(_("Error code %ld"), res.nHTTPCode);
	wxString sErr;		
	wxJSONReader reader;
    wxJSONValue  JSONRoot;
    int numErrors = reader.Parse(res.sBody, &JSONRoot);
    if(numErrors > 0 || !JSONRoot.HasMember("message"))
	{
		sErr = wxString (_("Unexpected error"));
	}	
	else
	{
		sErr = JSONRoot[wxT("message")].AsString();
	}
	
	wxString sFullError = sErr + wxT(" (") + sErrCode + wxT(")");
	if (pTrackCancel)
	{
		pTrackCancel->PutMessage(sFullError, wxNOT_FOUND, enumGISMessageError);
	}
	
	return false;		
}

//change fields
//PUT /resource/37/child/127
//
//{"resource":{"display_name":"water_polygon","keyname":"water_polygon","parent":{"id":37},"permissions":[],"description":null},"feature_layer":{"fields":[{"id":140,"keyname":"OSM_ID2","datatype":"REAL","typemod":null,"display_name":"OSM_ID 3","label_field":false,"grid_visibility":true},{"id":141,"keyname":"NAME2","datatype":"STRING","typemod":null,"display_name":"NAME 3","label_field":false,"grid_visibility":true},{"id":142,"keyname":"NATURAL","datatype":"STRING","typemod":null,"display_name":"NATURAL","label_field":false,"grid_visibility":true},{"id":143,"keyname":"WATERWAY","datatype":"STRING","typemod":null,"display_name":"WATERWAY","label_field":false,"grid_visibility":true},{"id":144,"keyname":"WETLAND","datatype":"STRING","typemod":null,"display_name":"WETLAND","label_field":false,"grid_visibility":true}]},"postgis_layer":{"connection":{"id":14,"parent":{"id":0}},"table":"water_polygon","schema":"topo_osm","column_id":"ogc_fid","column_geom":"wkb_geometry","geometry_type":"POLYGON","fields":"keep","srs":{"id":3857}}}
//
//http://xxx.xxx.xxx.xxx/resource/127/field/
//
//[{"display_name": "OSM_ID 3", "idx": 0, "datatype": "REAL", "layer_id": 127, "grid_visibility": true, "keyname": "OSM_ID2", "id": 140, "cls": "postgis_layer"}, {"display_name": "NAME 3", "idx": 1, "datatype": "STRING", "layer_id": 127, "grid_visibility": true, "keyname": "NAME2", "id": 141, "cls": "postgis_layer"}, {"display_name": "NATURAL", "idx": 2, "datatype": "STRING", "layer_id": 127, "grid_visibility": true, "keyname": "NATURAL", "id": 142, "cls": "postgis_layer"}, {"display_name": "WATERWAY", "idx": 3, "datatype": "STRING", "layer_id": 127, "grid_visibility": true, "keyname": "WATERWAY", "id": 143, "cls": "postgis_layer"}, {"display_name": "WETLAND", "idx": 4, "datatype": "STRING", "layer_id": 127, "grid_visibility": true, "keyname": "WETLAND", "id": 144, "cls": "postgis_layer"}]

//--------------------------------------------------------------
//class wxGxNGWRaster
//--------------------------------------------------------------

IMPLEMENT_CLASS(wxGxNGWRaster, wxGxRasterDataset)

wxGxNGWRaster::wxGxNGWRaster(wxGxNGWService *pService, const wxJSONValue &Data, wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxRasterDataset(enumRasterWMSTMS, oParent, soName, soPath), wxGxNGWResourceWithStyles(Data)
{
    m_eResourceType = enumNGWResourceTypeRasterLayer;
    m_pService = pService;
    m_sName = m_sDisplayName;
	wxString sURL = m_pService->GetURL() + wxString::Format(wxT("/resource/%d"), m_nRemoteId);
	m_sPath = CPLString(sURL.ToUTF8());
	
	m_nEPSGCode = Data["raster_layer"]["srs"]["id"].AsInt();
	m_nBandCount = Data["raster_layer"]["band_count"].AsInt();
}

wxGxNGWRaster::~wxGxNGWRaster()
{

}

wxString wxGxNGWRaster::GetCategory(void) const
{ 
	return wxString(_("NGW raster")); 
}

wxGISDataset* const wxGxNGWRaster::GetDatasetFast(void)
{
 	if(m_pwxGISDataset == NULL)
    {
		if(m_apStyles.empty())
		{
			GetStyles();
		}
		
		if(m_apStyles.empty())
		{
			return NULL;
		}
		
		wxGxNGWResource* pStyle = m_apStyles[0];
		if(pStyle == NULL)
		{
			return NULL;
		}
		
		//http://xxx.xxx.xxx.xxx/resource/234/tms?z=7&x=84&y=42
		wxString sURL = m_pService->GetURL() + wxString::Format(wxT("/resource/%d/tms?z=${z}&amp;x=${x}&amp;y=${y}"), pStyle->GetRemoteId());
		
		wxString sULX(wxT("-20037508.34"));
		wxString sULY(wxT("20037508.34"));
		wxString sLRX(wxT("20037508.34"));
		wxString sLRY(wxT("-20037508.34"));
		int nMaxLevel = 20;
		
		wxString sCachePath;
		wxString sCacheDepth(wxT("2"));
		//cache path
		wxGISAppConfig oConfig = GetConfig();
		if(oConfig.IsOk())
		{
			sCachePath = oConfig.GetLocalConfigDirNonPortable() + wxFileName::GetPathSeparator() + wxString(wxT("cache"));
			sCachePath = oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/GDAL/WMS/cache_path")), sCachePath);
			sCacheDepth = oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/GDAL/WMS/cache_depth")), sCacheDepth);
		}
		
		//NGW TMS template
		wxString sPath = wxString::Format(wxT("<GDAL_WMS><Service name=\"TMS\"><ServerUrl>%s</ServerUrl></Service><DataWindow><UpperLeftX>%s</UpperLeftX><UpperLeftY>%s</UpperLeftY><LowerRightX>%s</LowerRightX><LowerRightY>%s</LowerRightY><TileLevel>%d</TileLevel><TileCountX>1</TileCountX><TileCountY>1</TileCountY><YOrigin>top</YOrigin></DataWindow><Projection>EPSG:%d</Projection><BlockSizeX>256</BlockSizeX><BlockSizeY>256</BlockSizeY><BandsCount>%d</BandsCount><Cache><Path>%s</Path><Depth>%s</Depth></Cache><UserPwd>%s:%s</UserPwd><MaxConnections>7</MaxConnections><ZeroBlockOnServerException>false</ZeroBlockOnServerException><ZeroBlockHttpCodes>204,404</ZeroBlockHttpCodes></GDAL_WMS>"), sURL.c_str(), sULX.c_str(), sULY.c_str(), sLRX.c_str(), sLRY.c_str(), nMaxLevel, m_nEPSGCode, m_nBandCount,sCachePath.c_str(), sCacheDepth.c_str(), m_pService->GetLogin().c_str(), m_pService->GetPassword().c_str() );
		
        wxGISRasterDataset* pDSet = new wxGISRasterDataset(CPLString(sPath.ToUTF8()), enumRasterWMSTMS);
        m_pwxGISDataset = wxStaticCast(pDSet, wxGISDataset);
        m_pwxGISDataset->Reference();
    }
    wsGET(m_pwxGISDataset);
}


int wxGxNGWRaster::GetParentResourceId() const
{
	wxGxNGWResource* pParentResource = dynamic_cast<wxGxNGWResource*>(m_oParent);
	if(NULL == pParentResource)
		return wxNOT_FOUND;
	return pParentResource->GetRemoteId();
}


bool wxGxNGWRaster::CanDelete(void)
{
    //check permissions
	if(m_oPermissions.IsValid())
	{
		bool bCanDelete = m_oPermissions["resource"]["delete"].AsBool();
		return bCanDelete && m_pService != NULL;
	}
    return m_pService != NULL;
}

bool wxGxNGWRaster::CanRename(void)
{
    //check permissions
	if(m_oPermissions.IsValid())
	{
		bool bCanRename = m_oPermissions["resource"]["update"].AsBool();
		return bCanRename && m_pService != NULL;
	}
    return m_pService != NULL;
}

bool wxGxNGWRaster::Delete(void)
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

bool wxGxNGWRaster::Rename(const wxString &sNewName)
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

bool wxGxNGWRaster::Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{	
    return false;
}

bool wxGxNGWRaster::CanCopy(const CPLString &szDestPath)
{
	return true;//CanCopyResource(szDestPath);
}

bool wxGxNGWRaster::Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
	wxGxCatalogBase* pCatalog = GetGxCatalog();
    if (NULL == pCatalog)
    {
        return false;
    }    
	
	wxGxObject* pGxObj = pCatalog->FindGxObjectByPath(szDestPath);
	if (CanMoveResource(szDestPath))
	{
		//move to resource
		wxGxNGWResource* pGxNGWResource = dynamic_cast<wxGxNGWResource*>(pGxObj);
		bool bRes = MoveResource(pGxNGWResource->GetRemoteId());
		//report error
		if(!bRes)
		{
			wxString sErr = wxString::Format(_("Operation '%s' failed!"), _("Move"));  
			sErr += wxT("\n") + wxString::Format(wxT("%s '%s'"), GetCategory().c_str(), wxString::FromUTF8(m_sPath));
			wxGISLogError(sErr, wxString::FromUTF8(CPLGetLastErrorMsg()), wxEmptyString, pTrackCancel);
			return false;
		}
		else
		{
			wxGxObject* pObj = GetParent();
			if(NULL != pObj)
			{
				IGxObjectNotifier *pNotify = dynamic_cast<IGxObjectNotifier*>(pObj->GetParent());
				if(pNotify)
				{	
					pNotify->OnGetUpdates();
				}						
			}
			
			if(NULL != pGxObj)
			{
				IGxObjectNotifier *pNotify = dynamic_cast<IGxObjectNotifier*>(pGxObj->GetParent());
				if(pNotify)
				{	
					pNotify->OnGetUpdates();
				}						
			}
			return true;	
		}
	}	
    return false;	
}

bool wxGxNGWRaster::CanMove(const CPLString &szDestPath)
{
	if(!CanMoveResource(szDestPath))
		return CanCopy(szDestPath) && CanDelete();
	return true;	
}

//--------------------------------------------------------------
// wxGxNGWPostGISConnection
//--------------------------------------------------------------

IMPLEMENT_CLASS(wxGxNGWPostGISConnection, wxGxRemoteConnection)

wxGxNGWPostGISConnection::wxGxNGWPostGISConnection(wxGxNGWService *pService, const wxJSONValue &Data, wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxRemoteConnection(oParent, soName, soPath), wxGxNGWResource(Data)
{
    m_eResourceType = enumNGWResourceTypePostgisConnection;
    m_pService = pService;
    m_sName = m_sDisplayName;	
	wxString sURL = m_pService->GetURL() + wxString::Format(wxT("/resource/%d"), m_nRemoteId);
	m_sPath = CPLString(sURL.ToUTF8());
	
	wxJSONValue JSONConn = Data["postgis_connection"];
	m_sUser =  JSONConn["username"].AsString();
	m_sPass =  JSONConn["password"].AsString();
	m_sDatabase =  JSONConn["database"].AsString();
	m_sHost =  JSONConn["hostname"].AsString();
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
    //check permissions
	if(m_oPermissions.IsValid())
	{
		bool bCanDelete = m_oPermissions["resource"]["delete"].AsBool();
		return bCanDelete && m_pService != NULL;
	}
    return m_pService != NULL;
}

bool wxGxNGWPostGISConnection::CanRename(void)
{
    //check permissions
	if(m_oPermissions.IsValid())
	{
		bool bCanRename = m_oPermissions["resource"]["update"].AsBool();
		return bCanRename && m_pService != NULL;
	}
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
	return true;//CanCopyResource(szDestPath);
}

bool wxGxNGWPostGISConnection::Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
	wxGxCatalogBase* pCatalog = GetGxCatalog();
    if (NULL == pCatalog)
    {
        return false;
    }    
	
	wxGxObject* pGxObj = pCatalog->FindGxObjectByPath(szDestPath);
	if (CanMoveResource(szDestPath))
	{
		//move to resource
		wxGxNGWResource* pGxNGWResource = dynamic_cast<wxGxNGWResource*>(pGxObj);
		bool bRes = MoveResource(pGxNGWResource->GetRemoteId());
		//report error
		if(!bRes)
		{
			wxString sErr = wxString::Format(_("Operation '%s' failed!"), _("Move"));  
			sErr += wxT("\n") + wxString::Format(wxT("%s '%s'"), GetCategory().c_str(), wxString::FromUTF8(m_sPath));
			wxGISLogError(sErr, wxString::FromUTF8(CPLGetLastErrorMsg()), wxEmptyString, pTrackCancel);
			return false;
		}
		else
		{
			wxGxObject* pObj = GetParent();
			if(NULL != pObj)
			{
				IGxObjectNotifier *pNotify = dynamic_cast<IGxObjectNotifier*>(pObj->GetParent());
				if(pNotify)
				{	
					pNotify->OnGetUpdates();
				}						
			}
			
			if(NULL != pGxObj)
			{
				IGxObjectNotifier *pNotify = dynamic_cast<IGxObjectNotifier*>(pGxObj->GetParent());
				if(pNotify)
				{	
					pNotify->OnGetUpdates();
				}						
			}
			return true;	
		}
	}	
    return false;	
}

bool wxGxNGWPostGISConnection::CanMove(const CPLString &szDestPath)
{
	if(!CanMoveResource(szDestPath))
		return CanCopy(szDestPath) && CanDelete();
	return true;	
}


//--------------------------------------------------------------
//class wxGxNGWFileSet
//--------------------------------------------------------------

IMPLEMENT_CLASS(wxGxNGWFileSet, wxGxDataset)

wxGxNGWFileSet::wxGxNGWFileSet(wxGxNGWService *pService, const wxJSONValue &Data, wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxDataset(oParent, soName, soPath), wxGxNGWResource(Data)
{
    m_eResourceType = enumNGWResourceTypeFileSet;
    m_pService = pService;
    m_sName = m_sDisplayName;
    wxString sURL = m_pService->GetURL() + wxString::Format(wxT("/resource/%d"), m_nRemoteId);
    m_sPath = CPLString(sURL.ToUTF8());

    m_nType = enumGISRasterDataset;
    m_nSubType = enumRasterPng;

    wxJSONValue JSONFilesBucket = Data["file_bucket"];
    wxString sTimeStamp = JSONFilesBucket["tstamp"].AsString();
    m_dtMod.ParseISOCombined(sTimeStamp);

    wxJSONValue JSONFiles = JSONFilesBucket["files"];
    FillFilesArray(JSONFiles);
}

wxGxNGWFileSet::~wxGxNGWFileSet()
{

}

void wxGxNGWFileSet::FillFilesArray(const wxJSONValue &files)
{
    for (size_t i = 0; i < files.Size(); ++i)
    {
        wxString sMime = files[i]["mime"].AsString();
        wxString sName = files[i]["name"].AsString();
        wxULongLong nSize = files[i]["size"].AsULong();

        NGWFILEDESCRIPTION desc = { sName, sMime, nSize };
        m_asFiles.push_back(desc);
    }

    FillMetadata(true);
}

wxGISEnumDatasetType wxGxNGWFileSet::GetType(void) const
{
    return m_nType;
}

int wxGxNGWFileSet::GetSubType(void) const
{
    return m_nSubType;
}

void wxGxNGWFileSet::FillMetadata(bool bForce)
{
    if (m_bIsMetadataFilled && !bForce)
        return;

    m_bIsMetadataFilled = true;
    m_nSize = 0;
    for (size_t i = 0; i < m_asFiles.size(); ++i)
    {
        m_nSize += m_asFiles[i].nSize;
    }
}

wxString wxGxNGWFileSet::GetCategory(void) const
{
    return wxString(_("File set"));
}

int wxGxNGWFileSet::GetParentResourceId() const
{
    wxGxNGWResource* pParentResource = dynamic_cast<wxGxNGWResource*>(m_oParent);
    if (NULL == pParentResource)
        return wxNOT_FOUND;
    return pParentResource->GetRemoteId();
}


bool wxGxNGWFileSet::CanDelete(void)
{
    //check permissions
	if(m_oPermissions.IsValid())
	{
		bool bCanDelete = m_oPermissions["resource"]["delete"].AsBool();
		return bCanDelete && m_pService != NULL;
	}
    return m_pService != NULL;
}

bool wxGxNGWFileSet::CanRename(void)
{
    //check permissions
	if(m_oPermissions.IsValid())
	{
		bool bCanRename = m_oPermissions["resource"]["update"].AsBool();
		return bCanRename && m_pService != NULL;
	}
    return m_pService != NULL;
}

bool wxGxNGWFileSet::Delete(void)
{
    if (DeleteResource())
    {
        IGxObjectNotifier *pNotify = dynamic_cast<IGxObjectNotifier*>(m_oParent);
        if (pNotify)
        {
            pNotify->OnGetUpdates();
        }
        return true;
    }
    return false;
}

bool wxGxNGWFileSet::Rename(const wxString &sNewName)
{
    if (RenameResource(sNewName))
    {
        IGxObjectNotifier *pNotify = dynamic_cast<IGxObjectNotifier*>(m_oParent);
        if (pNotify)
        {
            pNotify->OnGetUpdates();
        }
        return true;
    }
    return false;
}

bool wxGxNGWFileSet::Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
	//copy to folder the contents of bucket
	wxGxCatalogBase* pCatalog = GetGxCatalog();
    if (NULL == pCatalog)
    {
        return false;
    }    
	
	wxGxObject* pGxObj = pCatalog->FindGxObjectByPath(szDestPath);
	if(pGxObj && pGxObj->IsKindOf(wxCLASSINFO(wxGxFolder)))
	{
		//copy to folder
		return CopyToFolder(pGxObj->GetPath(), pTrackCancel);	
	}
    return false;
}

bool wxGxNGWFileSet::CanCopy(const CPLString &szDestPath)
{
	return true;
}

bool wxGxNGWFileSet::Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
	//copy to folder the contents of bucket
	//and delete the bucket
	wxGxCatalogBase* pCatalog = GetGxCatalog();
    if (NULL == pCatalog)
    {
        return false;
    }    
	
	wxGxObject* pGxObj = pCatalog->FindGxObjectByPath(szDestPath);
	if(pGxObj && pGxObj->IsKindOf(wxCLASSINFO(wxGxFolder)))
	{
		//copy to folder
		if(CopyToFolder(pGxObj->GetPath(), pTrackCancel))
			return Delete();
		return false;	
	}
	
    if (CanMoveResource(szDestPath))
	{
		//move to resource
		wxGxNGWResource* pGxNGWResource = dynamic_cast<wxGxNGWResource*>(pGxObj);
		bool bRes = MoveResource(pGxNGWResource->GetRemoteId());
		//report error
		if(!bRes)
		{
			wxString sErr = wxString::Format(_("Operation '%s' failed!"), _("Move"));  
			sErr += wxT("\n") + wxString::Format(wxT("%s '%s'"), GetCategory().c_str(), wxString::FromUTF8(m_sPath));
			wxGISLogError(sErr, wxString::FromUTF8(CPLGetLastErrorMsg()), wxEmptyString, pTrackCancel);
			return false;
		}
		else
		{
			wxGxObject* pObj = GetParent();
			if(NULL != pObj)
			{
				IGxObjectNotifier *pNotify = dynamic_cast<IGxObjectNotifier*>(pObj->GetParent());
				if(pNotify)
				{	
					pNotify->OnGetUpdates();
				}						
			}
			
			if(NULL != pGxObj)
			{
				IGxObjectNotifier *pNotify = dynamic_cast<IGxObjectNotifier*>(pGxObj->GetParent());
				if(pNotify)
				{	
					pNotify->OnGetUpdates();
				}						
			}
			return true;	
		}
	}	
    return false;
}

bool wxGxNGWFileSet::CanMove(const CPLString &szDestPath)
{
	if(!CanMoveResource(szDestPath))
		return CanCopy(szDestPath) && CanDelete();
	return true;
}

wxGISDataset* const wxGxNGWFileSet::GetDatasetFast(void)
{
	if(m_pwxGISDataset == NULL)
    {
		wxString sURL = m_pService->GetURL() + wxString::Format(wxT("/resource/%d/file/%s.%s"), m_nRemoteId, PREVIEW_FILE_NAME, PREVIEW_FILE_NAME_EXT);
		wxString sAuth = m_pService->GetLogin() + wxT(":") + m_pService->GetPassword();
		
		CPLSetConfigOption("GDAL_HTTP_USERPWD", sAuth.mb_str());
		CPLString szURL = CPLString("/vsicurl/") + CPLString(sURL.ToUTF8());
        wxGISRasterDataset* pDSet = new wxGISRasterDataset(szURL, enumRasterPng);
        m_pwxGISDataset = wxStaticCast(pDSet, wxGISDataset);
        m_pwxGISDataset->Reference();
    }
    wsGET(m_pwxGISDataset);	
}

wxGISDataset* const wxGxNGWFileSet::GetDataset(bool bCached, ITrackCancel* const pTrackCancel)
{
	wxGISRasterDataset* pwxGISRasterDataset(NULL);
	pwxGISRasterDataset = wxDynamicCast(GetDatasetFast(), wxGISRasterDataset);

    if(pwxGISRasterDataset && !pwxGISRasterDataset->IsOpened())
    {
        if(!pwxGISRasterDataset->Open())
        {
            wsDELETE(pwxGISRasterDataset);
		    const char* err = CPLGetLastErrorMsg();
			wxString sErr = wxString::Format(_("Operation '%s' failed! GDAL error: %s"), _("Open"), wxString(err, wxConvUTF8).c_str());
            wxLogError(sErr);
			if(pTrackCancel)
				pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageError);
			return NULL;
        }
        wxGIS_GXCATALOG_EVENT(ObjectChanged);
        wsDELETE(pwxGISRasterDataset);
	}

	wsGET(m_pwxGISDataset);
}

bool wxGxNGWFileSet::CopyToFolder(const CPLString &szPath, ITrackCancel * const pTrackCancel)
{
	wxGISCurl curl = m_pService->GetCurl();
    if(!curl.IsOk())
        return false;	
		
	IProgressor *pProgress = NULL;
	if(pTrackCancel)
	{
		pTrackCancel->PutMessage(wxString::Format(_("Start copy %s"), GetCategory().c_str()), wxNOT_FOUND, enumGISMessageInformation);
		pProgress = pTrackCancel->GetProgressor();
		if(pProgress)
			pProgress->SetRange(m_asFiles.size());
	}
	
	wxString sPreviewName = wxString::Format(wxT("%s.%s"), PREVIEW_FILE_NAME, PREVIEW_FILE_NAME_EXT);
	
	for ( size_t i = 0; i < m_asFiles.size(); ++i ) 
	{    
		if(pProgress)
			pProgress->SetValue(i);
		
		if(m_asFiles[i].sName.IsSameAs(sPreviewName, false))
			continue;
		
		wxString sURL = m_pService->GetURL() + wxString::Format(wxT("/resource/%d/file/%s"), m_nRemoteId, m_asFiles[i].sName.c_str());
			
		if(!curl.GetFile(sURL, wxString::FromUTF8(szPath) + wxFileName::GetPathSeparator() + m_asFiles[i].sName, pTrackCancel))
		{
			if(pTrackCancel)
				pTrackCancel->PutMessage(wxString::Format(_("File %s download failed"), m_asFiles[i].sName.c_str()), wxNOT_FOUND, enumGISMessageError);
			return false;	
		}
	}	
	
	return true;
}

#endif // wxGIS_USE_CURL
