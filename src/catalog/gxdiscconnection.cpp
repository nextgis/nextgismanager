/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxDiscConnection class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2010,2012-2014  Dmitry Baryshnikov
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
#include "wxgis/catalog/gxdiscconnection.h"
#include "wxgis/catalog/gxcatalog.h"
#include "wxgis/core/format.h"
#include "wxgis/catalog/gxmlconnstor.h"

//---------------------------------------------------------------------------
// wxGxDiscConnection
//---------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGxDiscConnection, wxGxFolder)

BEGIN_EVENT_TABLE(wxGxDiscConnection, wxGxFolder)
    EVT_FSWATCHER(wxID_ANY, wxGxDiscConnection::OnFileSystemEvent)
#ifdef __WXGTK__
    EVT_GXOBJECT_ADDED(wxGxDiscConnection::OnObjectAdded)
#endif
END_EVENT_TABLE()


wxGxDiscConnection::wxGxDiscConnection(void) : wxGxFolder()
{
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
}

wxGxDiscConnection::wxGxDiscConnection(wxGxObject *oParent, int nStoreId, const wxString &soName, const CPLString &soPath) : wxGxFolder(oParent, soName, soPath)
{
    m_nStoreId = nStoreId;
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
}

wxGxDiscConnection::~wxGxDiscConnection(void)
{
}

bool wxGxDiscConnection::Destroy(void)
{
#ifdef __WXGTK__
	if(m_ConnectionPointCatalogCookie != wxNOT_FOUND)
        m_pCatalog->Unadvise(m_ConnectionPointCatalogCookie);
#endif

    wxDELETE(m_pWatcher);
    return wxGxObjectContainer::Destroy();
}

bool wxGxDiscConnection::Delete(void)
{
    wxGxJSONConnectionStorage* pGxJSONConnectionStorage = wxDynamicCast( GetParent(), wxGxJSONConnectionStorage);
    if(pGxJSONConnectionStorage)
    {
        return pGxJSONConnectionStorage->DeleteItemById(m_nStoreId);
    }
    return false;
}

bool wxGxDiscConnection::Rename(const wxString& NewName)
{
    wxGxJSONConnectionStorage* pGxJSONConnectionStorage = wxDynamicCast( GetParent(), wxGxJSONConnectionStorage);
    if(pGxJSONConnectionStorage)
    {
        return pGxJSONConnectionStorage->RenameItemById(m_nStoreId, NewName);
    }
    return false;
}

void wxGxDiscConnection::StartWatcher(void)
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

void wxGxDiscConnection::OnFileSystemEvent(wxFileSystemWatcherEvent& event)
{
    //wxLogDebug(wxT("*** %s ***"), event.ToString().c_str());
    switch(event.GetChangeType())
    {
    case wxFSW_EVENT_CREATE:
        {
            //get object parent
            wxFileName oName = event.GetPath();
            wxGxObjectContainer *parent = wxDynamicCast(FindGxObjectByPath(oName.GetPath()), wxGxObjectContainer);
            if(!parent)
                return;
            //check doubles
            if(parent->IsNameExist(oName.GetFullName()))
                return;

            //wxCriticalSectionLocker locker(m_CritSect);

            if (m_pCatalog)
            {
                wxArrayLong ChildrenIds;
                char **papszFileList = NULL;
                CPLString szPath(oName.GetFullPath().ToUTF8());
                papszFileList = CSLAddString(papszFileList, szPath);

                m_pCatalog->CreateChildren(parent, papszFileList, ChildrenIds);
                for (size_t i = 0; i < ChildrenIds.GetCount(); ++i)
                    m_pCatalog->ObjectAdded(ChildrenIds[i]);

                CSLDestroy(papszFileList);
            }

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
                if(IsPathWatched(event.GetPath().GetFullPath()))
                {
                    m_pWatcher->Remove(event.GetPath());
                }

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

void wxGxDiscConnection::LoadChildren(void)
{
	if(m_bIsChildrenLoaded)
		return;

    wxGxFolder::LoadChildren();
    StartWatcher();
}

#ifdef __WXGTK__
void wxGxDiscConnection::OnObjectAdded(wxGxCatalogEvent& event)
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

bool wxGxDiscConnection::IsPathWatched(const wxString& sPath)
{
    wxArrayString sPaths;
    m_pWatcher->GetWatchedPaths(&sPaths);

    return sPaths.Index(sPath, false) != wxNOT_FOUND;
}

void wxGxDiscConnection::Refresh(void)
{
#ifdef __WXGTK__
    m_pWatcher->RemoveAll();
#endif
	DestroyChildren();
    m_bIsChildrenLoaded = false;
	LoadChildren();
    wxGIS_GXCATALOG_EVENT(ObjectRefreshed);
}
