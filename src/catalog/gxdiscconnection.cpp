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

wxGxDiscConnection::wxGxDiscConnection(void) : wxGxFolder()
{
    m_pCatalog = wxDynamicCast(GetGxCatalog(), wxGxCatalog);
}

wxGxDiscConnection::wxGxDiscConnection(wxGxObject *oParent, int nStoreId, const wxString &soName, const CPLString &soPath) : wxGxFolder(oParent, soName, soPath)
{
    m_nStoreId = nStoreId;
    m_pCatalog = wxDynamicCast(GetGxCatalog(), wxGxCatalog);
}

wxGxDiscConnection::~wxGxDiscConnection(void)
{
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

void wxGxDiscConnection::LoadChildren(void)
{
	if(m_bIsChildrenLoaded)
		return;

    wxGxFolder::LoadChildren();
    StartWatcher();
}

void wxGxDiscConnection::Refresh(void)
{
	DestroyChildren();
    m_bIsChildrenLoaded = false;
	LoadChildren();
    wxGIS_GXCATALOG_EVENT(ObjectRefreshed);
}
