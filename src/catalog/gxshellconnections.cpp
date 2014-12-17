/******************************************************************************
* Project:  wxGIS (GIS Catalog)
* Purpose:  Shell Connections classes (ssh, ftp, etc.).
* Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
******************************************************************************
*   Copyright (C) 2014 Dmitry Baryshnikov
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

#include "wxgis/catalog/gxshellconnections.h"

#include "wxgis/core/config.h"

//---------------------------------------------------------------------------
// wxGxShellConnections
//---------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGxShellConnections, wxGxFolder)

wxGxShellConnections::wxGxShellConnections(void) : wxGxFolder()
{
    m_sName = wxString(_("Shell connections"));
}

bool wxGxShellConnections::Create(wxGxObject *oParent, const wxString &soName, const CPLString &soPath)
{
    if( !wxGxFolder::Create(oParent, _("Shell connections"), soPath) )
    {
        wxLogError(_("wxGxShellConnections::Create failed. GxObject %s"), wxString(_("Shell connections")).c_str());
        return false;
    }

    m_pCatalog = wxDynamicCast(GetGxCatalog(), wxGxCatalog);
    return true;
}

wxGxShellConnections::~wxGxShellConnections(void)
{
}

void wxGxShellConnections::Init(wxXmlNode* const pConfigNode)
{
    m_sInternalPath = pConfigNode->GetAttribute(wxT("path"), NON);
    if(m_sInternalPath.IsEmpty() || m_sInternalPath == wxString(NON))
    {
		wxGISAppConfig oConfig = GetConfig();
		if(!oConfig.IsOk())
			return;

		m_sInternalPath = oConfig.GetLocalConfigDir() + wxFileName::GetPathSeparator() + wxString(wxT("shconnections"));
    }

    //m_sInternalPath.Replace(wxT("\\"), wxT("/"));
    wxLogMessage(_("wxGxShellConnections: The path is set to '%s'"), m_sInternalPath.c_str());
    CPLSetConfigOption("wxGxShellConnections", m_sInternalPath.mb_str(wxConvUTF8));

    m_sPath = CPLString(m_sInternalPath.mb_str(wxConvUTF8));

	if(!wxDirExists(m_sInternalPath))
        wxFileName::Mkdir(m_sInternalPath, wxPOSIX_USER_READ | wxPOSIX_USER_WRITE | wxPOSIX_USER_EXECUTE | wxPOSIX_GROUP_READ | wxPOSIX_GROUP_EXECUTE | wxPOSIX_OTHERS_READ | wxPOSIX_OTHERS_EXECUTE, wxPATH_MKDIR_FULL); //0755
}

void wxGxShellConnections::Serialize(wxXmlNode* pConfigNode)
{
    pConfigNode->AddAttribute(wxT("path"), m_sInternalPath);
}

bool wxGxShellConnections::CanCreate(long nDataType, long DataSubtype)
{
	if(nDataType != enumGISContainer)
		return false;
	if(DataSubtype != enumContFolder && DataSubtype != enumContRemoteDBConnection)
		return false;
	return wxGxFolder::CanCreate(nDataType, DataSubtype);
}

void wxGxShellConnections::StartWatcher(void)
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

void wxGxShellConnections::LoadChildren(void)
{
	if(m_bIsChildrenLoaded)
		return;

    wxGxFolder::LoadChildren();
    StartWatcher();
}

void wxGxShellConnections::Refresh(void)
{
	DestroyChildren();
    m_bIsChildrenLoaded = false;
	LoadChildren();
    wxGIS_GXCATALOG_EVENT(ObjectRefreshed);
}
