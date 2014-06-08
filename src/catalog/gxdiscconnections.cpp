/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxDiscConnections class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2014 Dmitry Baryshnikov
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
#include "wxgis/catalog/gxdiscconnections.h"
#include "wxgis/catalog/gxcatalog.h"
#include "wxgis/core/config.h"
#include "wxgis/core/format.h"
#include "wxgis/catalog/gxdiscconnection.h"

#include <wx/volume.h>
#include <wx/dir.h>


IMPLEMENT_DYNAMIC_CLASS(wxGxDiscConnections, wxGxJSONConnectionStorage)

BEGIN_EVENT_TABLE(wxGxDiscConnections, wxGxJSONConnectionStorage)
    EVT_FSWATCHER(wxID_ANY, wxGxJSONConnectionStorage::OnFileSystemEvent)
END_EVENT_TABLE()

wxGxDiscConnections::wxGxDiscConnections(void) : wxGxJSONConnectionStorage()
{
    m_sName = wxString(_("Folder connections"));
}

bool wxGxDiscConnections::Create(wxGxObject *oParent, const wxString &soName, const CPLString &soPath)
{
    if (!wxGxObjectContainer::Create(oParent, GetName(), soPath))
    {
        wxLogError(_("wxGxSpatialReferencesFolder::Create failed. GxObject %s"), GetName().c_str());
        return false;
    }
    //get config path
    m_sStorageName = wxString(CONNCONF);
	wxGISAppConfig oConfig = GetConfig();
	if(oConfig.IsOk())
	{
		m_sUserConfigDir = oConfig.GetLocalConfigDirNonPortable() + wxFileName::GetPathSeparator() + wxString(CONNDIR);
		m_sStoragePath = m_sUserConfigDir + wxFileName::GetPathSeparator() + m_sStorageName;
	}
    m_pWatcher = new wxFileSystemWatcher();
    m_pWatcher->SetOwner(this);

    wxFileName oFileName = wxFileName::DirName(m_sUserConfigDir);
    //if dir is not exist create it
    if(!wxDirExists(m_sUserConfigDir))
        wxFileName::Mkdir(m_sUserConfigDir, 0755, wxPATH_MKDIR_FULL);

    wxLogDebug(wxT("monitoring dir is: %s"), oFileName.GetFullPath().c_str());
    if(!m_pWatcher->Add(oFileName, wxFSW_EVENT_MODIFY))//bool bAdd = |wxFSW_EVENT_CREATE
    {
        wxLogError(_("Add File system watcher failed"));
        return false;
    }

    return true;
}

wxGxDiscConnections::~wxGxDiscConnections(void)
{
}

bool wxGxDiscConnections::Destroy(void)
{
    wxDELETE(m_pWatcher);
    return wxGxJSONConnectionStorage::Destroy();
}

void wxGxDiscConnections::Refresh(void)
{
    if(DestroyChildren())
        LoadConnectionsStorage();
    else
        wxGxJSONConnectionStorage::Refresh();
}

void wxGxDiscConnections::Init(wxXmlNode* const pConfigNode)
{
    LoadConnectionsStorage();
}

void wxGxDiscConnections::Serialize(wxXmlNode* const pConfigNode)
{
    //Nothing to do. We edit connections xml from other functions
    //StoreConnections();
}

bool wxGxDiscConnections::IsObjectExist(wxGxObject* const pObj, const wxJSONValue& GxObjConfValue)
{
    int nStoreId = GxObjConfValue[wxT("id")].AsInt();
    wxGxDiscConnection* pConn = wxDynamicCast(pObj, wxGxDiscConnection);
    if(!pConn)
        return false;

    if(pConn->GetStoreId() != nStoreId)
        return false;

    //if exist control name and path

    wxString sName = GxObjConfValue[wxT("name")].AsString();
	wxString sPath = GxObjConfValue[wxT("path")].AsString();
    CPLString szPath(sPath.mb_str(wxConvUTF8));

    if(pObj->GetPath() != szPath)
    {
        pObj->SetPath(szPath);
        //ObjectChanged event
        wxGIS_GXCATALOG_EVENT_ID(ObjectRefreshed, pObj->GetId());
    }

    if(!pObj->GetName().IsSameAs( sName, false))
    {
        pObj->SetName(sName);
        //ObjectChanged event
        wxGIS_GXCATALOG_EVENT_ID(ObjectChanged, pObj->GetId());
    }
    return true;
}

void wxGxDiscConnections::CreateConnectionsStorage(void)
{
    wxLogMessage(_("wxGxDiscConnections: Start scan folder connections"));
    wxArrayString arr;
#if defined(__WINDOWS__)
	arr = wxFSVolumeBase::GetVolumes(wxFS_VOL_MOUNTED, wxFS_VOL_REMOVABLE);//| wxFS_VOL_REMOTE
#elif defined(__UNIX__)
    //linux paths
    arr.Add(wxT("/"));
    arr.Add(wxStandardPaths::Get().GetUserConfigDir());
//      arr.Add(stp.GetDataDir());
#endif
    //create

    wxJSONValue  oStorageRoot;
    oStorageRoot[wxT("version")] = GetStorageVersion();
    for(size_t i = 0; i < arr.size(); ++i)
	{
        oStorageRoot[wxT("connections")][i][wxT("name")] = arr[i];
        oStorageRoot[wxT("connections")][i][wxT("path")] = arr[i];
        oStorageRoot[wxT("connections")][i][wxT("id")] = i;
	}

    wxJSONWriter writer( wxJSONWRITER_STYLED | wxJSONWRITER_WRITE_COMMENTS );
    wxString  sJSONText;

    writer.Write( oStorageRoot, sJSONText );
    wxFile oStorageFile(m_sStoragePath, wxFile::write);
    oStorageFile.Write(sJSONText);
    oStorageFile.Close();
}

wxGxObject* wxGxDiscConnections::CreateChildGxObject(const wxJSONValue& GxObjConfValue)
{
    wxString soName = GxObjConfValue[wxT("name")].AsString();
    wxString sPath = GxObjConfValue[wxT("path")].AsString();
    CPLString soPath(sPath.mb_str(wxConvUTF8));
    int nStorageId = GxObjConfValue[wxT("id")].AsInt();
    if(sPath.IsEmpty() || nStorageId == wxNOT_FOUND)
        return NULL;
    return new wxGxDiscConnection(this, nStorageId, soName, soPath);
}

bool wxGxDiscConnections::ConnectFolder(const wxString &sPath)
{
    wxCriticalSectionLocker locker(m_oCritSect);
    if(!wxDir::Exists(sPath))
        return false;
    //find max id
    int nMaxId(0);
	wxGxObjectList ObjectList = GetChildren();
    wxGxObjectList::iterator iter;
    for (iter = ObjectList.begin(); iter != ObjectList.end(); ++iter)
    {
        wxGxObject *current = *iter;
		wxGxDiscConnection* pConn = wxDynamicCast(current, wxGxDiscConnection);
        if(!pConn)
            continue;
        if(nMaxId < pConn->GetStoreId())
            nMaxId = pConn->GetStoreId();
    }
    //add
    return AddItem(nMaxId + 1, sPath, sPath);
}

bool wxGxDiscConnections::DisconnectFolder(int nStoreId)
{
    wxCriticalSectionLocker locker(m_oCritSect);

    return DeleteItemById(nStoreId);
}


