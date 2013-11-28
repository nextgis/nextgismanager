/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxDBConnections class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011,2013 Bishop
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

#include "wxgis/catalog/gxdbconnections.h"
/*
#include "wxgis/core/config.h"
#include "wxgis/datasource/datasource.h"

//---------------------------------------------------------------------------
// wxGxDBConnections
//---------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGxDBConnections, wxGxFolder)

BEGIN_EVENT_TABLE(wxGxDBConnections, wxGxFolder)
    EVT_FSWATCHER(wxID_ANY, wxGxDBConnections::OnFileSystemEvent)
#ifdef __WXGTK__
    EVT_GXOBJECT_ADDED(wxGxDiscConnection::OnObjectAdded)
#endif
END_EVENT_TABLE()

wxGxDBConnections::wxGxDBConnections(void) : wxGxFolder()
{
    m_sName = wxString(_("DataBase connections"));
}

bool wxGxDBConnections::Create(wxGxObject *oParent, const wxString &soName, const CPLString &soPath)
{
    if( !wxGxFolder::Create(oParent, _("DataBase connections"), soPath) )
    {
        wxLogError(_("wxGxDBConnections::Create failed. GxObject %s"), wxString(_("DataBase connections")).c_str());
        return false;
    }

    m_pWatcher = new wxFileSystemWatcher();
    m_pWatcher->SetOwner(this);
    m_pCatalog = wxDynamicCast(GetGxCatalog(), wxGxCatalog);
#ifdef __WXGTK__
    m_ConnectionPointCatalogCookie = wxNOT_FOUND;

    if(m_pCatalog)
    {
		m_ConnectionPointCatalogCookie = m_pCatalog->Advise(this);
    }
#endif
    return true;
}

wxGxDBConnections::~wxGxDBConnections(void)
{
}

void wxGxDBConnections::Init(wxXmlNode* const pConfigNode)
{
    m_sInternalPath = pConfigNode->GetAttribute(wxT("path"), NON);
    if(m_sInternalPath.IsEmpty() || m_sInternalPath == wxString(NON))
    {
		wxGISAppConfig oConfig = GetConfig();
		if(!oConfig.IsOk())
			return;

		m_sInternalPath = oConfig.GetLocalConfigDir() + wxFileName::GetPathSeparator() + wxString(wxT("dbconnections"));
    }

    //m_sInternalPath.Replace(wxT("\\"), wxT("/"));
    wxLogMessage(_("wxGxDBConnections: The path is set to '%s'"), m_sInternalPath.c_str());
    CPLSetConfigOption("wxGxDBConnections", m_sInternalPath.mb_str(wxConvUTF8));

    m_sPath = CPLString(m_sInternalPath.mb_str(wxConvUTF8));

	if(!wxDirExists(m_sInternalPath))
		wxFileName::Mkdir(m_sInternalPath, 0755, wxPATH_MKDIR_FULL);
}

void wxGxDBConnections::Serialize(wxXmlNode* pConfigNode)
{
    pConfigNode->AddAttribute(wxT("path"), m_sInternalPath);
}

bool wxGxDBConnections::CanCreate(long nDataType, long DataSubtype)
{
	if(nDataType != enumGISContainer)
		return false;
	if(DataSubtype != enumContFolder && DataSubtype != enumContRemoteDBConnection)
		return false;
	return wxGxFolder::CanCreate(nDataType, DataSubtype);
}

bool wxGxDBConnections::Destroy(void)
{
#ifdef __WXGTK__
	if(m_ConnectionPointCatalogCookie != wxNOT_FOUND)
        m_pCatalog->Unadvise(m_ConnectionPointCatalogCookie);
#endif

    wxDELETE(m_pWatcher);
    return wxGxFolder::Destroy();
}

void wxGxDBConnections::StartWatcher(void)
{
    //add itself
    wxFileName oFileName = wxFileName::DirName(wxString(m_sPath, wxConvUTF8));
    wxString sPath = oFileName.GetFullPath();
    if(!IsPathWatched(sPath))
    {
#ifdef __WXGTK__
        if(!m_pWatcher->Add(oFileName, wxFSW_EVENT_ALL))
#else ifdefined __WXMSW__
        if(!m_pWatcher->AddTree(oFileName, wxFSW_EVENT_CREATE | wxFSW_EVENT_DELETE | wxFSW_EVENT_RENAME))
#endif
        {
            wxLogError(_("Add File system watcher failed"));
        }
    }
#ifdef __WXGTK__
    //add children
    wxGxObjectList::const_iterator iter;
    for(iter = GetChildren().begin(); iter != GetChildren().end(); ++iter)
    {
        wxGxObject *current = *iter;
        if(current)
        {
            oFileName = wxFileName::DirName(wxString(current->GetPath(), wxConvUTF8));
            if(!IsPathWatched(oFileName.GetFullPath()))
            {
               if(!m_pWatcher->Add(oFileName, wxFSW_EVENT_ALL))
                {
                    wxLogError(_("Add File system watcher failed"));
                }
            }
        }
    }
#endif
} 

void wxGxDBConnections::OnFileSystemEvent(wxFileSystemWatcherEvent& event)
{
    wxLogDebug(wxT("*** %s ***"), event.ToString().c_str());
    switch(event.GetChangeType())
    {
    case wxFSW_EVENT_CREATE:
        {
            //get object parent
            wxFileName oName = event.GetPath();
            wxString sPath = oName.GetPath();
            wxGxObjectContainer *parent = wxDynamicCast(FindGxObjectByPath(sPath), wxGxObjectContainer);
            if(!parent)
                return;
            //check doubles
            if(parent->IsNameExist(event.GetPath().GetFullName()))
                return;

            CPLString szPath(event.GetPath().GetFullPath().mb_str(wxConvUTF8));
            char **papszFileList = NULL;  
            papszFileList = CSLAddString( papszFileList, szPath );
	        if(m_pCatalog)
            {
                wxArrayLong ChildrenIds;
                m_pCatalog->CreateChildren(parent, papszFileList, ChildrenIds);
                for(size_t i = 0; i < ChildrenIds.GetCount(); ++i)
                    m_pCatalog->ObjectAdded(ChildrenIds[i]);
	        }
            CSLDestroy( papszFileList );
        }
        break;
    case wxFSW_EVENT_DELETE:
        {
            //search gxobject
            wxGxObject *current = FindGxObjectByPath(event.GetPath().GetFullPath());
            if(current)
            {
                current->Destroy();
                return;
            }
        }
        break;
    case wxFSW_EVENT_RENAME:
        {
            wxGxObject *current = FindGxObjectByPath(event.GetPath().GetFullPath());
            if(current)
            {
                current->SetName(event.GetNewPath().GetFullName());
                current->SetPath( CPLString( event.GetNewPath().GetFullPath().mb_str(wxConvUTF8) ) );
                wxGIS_GXCATALOG_EVENT_ID(ObjectChanged, current->GetId());

#ifdef __WXGTK__
                m_pWatcher->Remove(event.GetPath());
                if(!IsPathWatched(event.GetNewPath().GetFullPath()))
                {
                    m_pWatcher->Add(event.GetNewPath());

                }
#endif
                return;
            }
        }
        break;
    default:
        break;
    };
    event.Skip();
}

void wxGxDBConnections::LoadChildren(void)
{
	if(m_bIsChildrenLoaded)
		return;

    wxGxFolder::LoadChildren();
    StartWatcher();
}

#ifdef __WXGTK__
void wxGxDBConnections::OnObjectAdded(wxGxCatalogEvent& event)
{
    wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(event.GetObjectID());
	if(!pGxObject)
		return;
    wxString sPath(pGxObject->GetPath(), wxConvUTF8);
    wxString sConnPath(GetPath(), wxConvUTF8);
    if(sPath.StartsWith(sConnPath))
    {
        if(!IsPathWatched(sPath) && pGxObject->IsKindOf(wxCLASSINFO(wxGxFolder)))
        {
            wxFileName oFileName = wxFileName::DirName(sPath);
            m_pWatcher->Add(oFileName);
        }
    }
}
#endif

bool wxGxDBConnections::IsPathWatched(const wxString& sPath)
{
    wxArrayString sPaths;
    m_pWatcher->GetWatchedPaths(&sPaths);

    return sPaths.Index(sPath, false) != wxNOT_FOUND;
}

void wxGxDBConnections::Refresh(void)
{
#ifdef __WXGTK__
    m_pWatcher->RemoveAll();
#endif
	DestroyChildren();
    m_bIsChildrenLoaded = false;
	LoadChildren();
    wxGIS_GXCATALOG_EVENT(ObjectRefreshed);
}
*/