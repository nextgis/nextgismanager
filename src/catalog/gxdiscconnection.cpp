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
#ifdef __UNIX__
    EVT_GXOBJECT_ADDED(wxGxDiscConnection::OnObjectAdded)
#endif
END_EVENT_TABLE()


wxGxDiscConnection::wxGxDiscConnection(void) : wxGxFolder()
{
    m_pCatalog = wxDynamicCast(GetGxCatalog(), wxGxCatalog);
    m_ConnectionPointCatalogCookie = wxNOT_FOUND;

    if(m_pCatalog)
    {
		m_ConnectionPointCatalogCookie = m_pCatalog->Advise(this);
    }
}

wxGxDiscConnection::wxGxDiscConnection(wxGxObject *oParent, int nStoreId, const wxString &soName, const CPLString &soPath) : wxGxFolder(oParent, soName, soPath)
{
    m_nStoreId = nStoreId;
    m_pCatalog = wxDynamicCast(GetGxCatalog(), wxGxCatalog);
    m_ConnectionPointCatalogCookie = wxNOT_FOUND;

    if(m_pCatalog)
    {
		m_ConnectionPointCatalogCookie = m_pCatalog->Advise(this);
    }
}

wxGxDiscConnection::~wxGxDiscConnection(void)
{
}

bool wxGxDiscConnection::Destroy(void)
{
	if(m_ConnectionPointCatalogCookie != wxNOT_FOUND)
        m_pCatalog->Unadvise(m_ConnectionPointCatalogCookie);

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
#if defined(__UNIX__)
    if(!m_pCatalog->AddFSWatcherPath(oFileName, wxFSW_EVENT_ALL))
#elif defined(__WINDOWS__)
    if(!m_pCatalog->AddFSWatcherTree(oFileName, wxFSW_EVENT_CREATE | wxFSW_EVENT_DELETE | wxFSW_EVENT_RENAME))
#endif
    {
        wxLogError(_("Add File system watcher failed"));
    }
#ifdef __UNIX__
    //add children
    wxGxObjectList::const_iterator iter;
    for(iter = GetChildren().begin(); iter != GetChildren().end(); ++iter)
    {
        wxGxObject *current = *iter;
        if(current)
        {
            oFileName = wxFileName::DirName(wxString(current->GetPath(), wxConvUTF8));
            if(!m_pCatalog->AddFSWatcherPath(oFileName, wxFSW_EVENT_ALL))
            {
                wxLogError(_("Add File system watcher failed"));
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
            wxLogDebug(wxT("parent %s; path %s; new path %s"), m_sName.c_str(), event.GetPath().GetFullPath().c_str(), event.GetNewPath().GetFullPath().c_str());
            wxGxObject *current = FindGxObjectByPath(event.GetPath().GetFullPath());
            if(current)
            {
                current->SetName(event.GetNewPath().GetFullName());
                current->SetPath( CPLString( event.GetNewPath().GetFullPath().mb_str(wxConvUTF8) ) );
                wxGIS_GXCATALOG_EVENT_ID(ObjectChanged, current->GetId());

#ifdef __UNIX__
                m_pCatalog->RemoveFSWatcherPath(event.GetPath());

                if(current->IsKindOf(wxCLASSINFO(wxGxFolder)))
                {
                    m_pCatalog->AddFSWatcherPath(event.GetNewPath());
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

#ifdef __UNIX__
void wxGxDiscConnection::OnObjectAdded(wxGxCatalogEvent& event)
{
    wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(event.GetObjectID());
	if(!pGxObject)
		return;
    wxString sPath(pGxObject->GetPath(), wxConvUTF8);
    wxString sConnPath(GetPath(), wxConvUTF8);
    if(sPath.StartsWith(sConnPath))
    {
        if(pGxObject->IsKindOf(wxCLASSINFO(wxGxFolder)))
        {
            wxFileName oFileName = wxFileName::DirName(sPath);
            m_pCatalog->AddFSWatcherPath(oFileName);
        }
    }
}
#endif

void wxGxDiscConnection::Refresh(void)
{
	DestroyChildren();
    m_bIsChildrenLoaded = false;
	LoadChildren();
    wxGIS_GXCATALOG_EVENT(ObjectRefreshed);
}
