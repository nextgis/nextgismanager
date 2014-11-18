/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxFolder class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011-2014 Dmitry Baryshnikov
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
#include "wxgis/catalog/gxfolder.h"
#include "wxgis/catalog/gxcatalog.h"
#include "wxgis/datasource/sysop.h"
#include "wxgis/core/app.h"

//---------------------------------------------------------------------------
// wxGxFolder
//---------------------------------------------------------------------------
IMPLEMENT_CLASS(wxGxFolder, wxGxObjectContainer)

wxGxFolder::wxGxFolder(void) : wxGxObjectContainer()
{
    m_nDefaultCreateDirMode = 0775;
    m_bIsChildrenLoaded = false;
}

wxGxFolder::wxGxFolder(wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxObjectContainer(oParent, soName, soPath)
{
    m_nDefaultCreateDirMode = 0775;
    m_bIsChildrenLoaded = false;
}

wxGxFolder::~wxGxFolder(void)
{
    wxGxCatalog* pCatalog = wxDynamicCast(GetGxCatalog(), wxGxCatalog);
#ifdef __UNIX__
    if(pCatalog)
    {
        wxString sPath = wxString(m_sPath, wxConvUTF8);
        wxFileName oFileName = wxFileName::DirName(sPath);
        pCatalog->RemoveFSWatcherPath(oFileName);
    }
#endif // __UNIX__
}

wxString wxGxFolder::GetBaseName(void) const
{
    return GetName();
}

void wxGxFolder::Refresh(void)
{
	DestroyChildren();
    m_bIsChildrenLoaded = false;
	LoadChildren();
    wxGIS_GXCATALOG_EVENT(ObjectRefreshed);
}

void wxGxFolder::LoadChildren(void)
{
	if(m_bIsChildrenLoaded)
		return;

    wxGxCatalog* pCatalog = wxDynamicCast(GetGxCatalog(), wxGxCatalog);
#ifdef __UNIX__
    if(pCatalog)
    {
        wxString sPath = wxString(m_sPath, wxConvUTF8);
        wxFileName oFileName = wxFileName::DirName(sPath);
        pCatalog->AddFSWatcherPath(oFileName);
    }
#endif // __UNIX__

    char **papszItems = CPLReadDir(m_sPath);
    if(papszItems == NULL)
        return;

    char **papszFileList = NULL;

    //remove unused items
    for(int i = CSLCount(papszItems) - 1; i >= 0; i-- )
    {
        if( wxGISEQUAL(papszItems[i], ".") || wxGISEQUAL(papszItems[i], "..") )
            continue;
        CPLString szFileName = CPLFormFilename(m_sPath, papszItems[i], NULL);
        if(pCatalog && !pCatalog->GetShowHidden() && IsFileHidden(szFileName))
            continue;

        //if(CSLFindString(papszFileList, szFileName) == -1)
            papszFileList = CSLAddString( papszFileList, szFileName );

    }
    CSLDestroy( papszItems );

    //create children from path and load them

	if(pCatalog)
    {
        wxArrayLong ChildrenIds;
        pCatalog->CreateChildren(this, papszFileList, ChildrenIds);
	}
    CSLDestroy( papszFileList );

	m_bIsChildrenLoaded = true;
}

bool wxGxFolder::CanDelete(void)
{
    return true;
}

bool wxGxFolder::Delete(void)
{
 	if(DeleteDir(m_sPath))
	{
        return true;
	}
	else
    {
		wxString sErr = wxString::Format(_("Operation '%s' failed!"), _("Delete"));
		sErr += wxT("\n") + wxString::Format(wxT("%s '%s'"), GetCategory().c_str(), wxString::FromUTF8(m_sPath));
		wxGISLogError(sErr, wxString::FromUTF8(CPLGetLastErrorMsg()), wxEmptyString, NULL);
        return false;
    }
	return false;
}

bool wxGxFolder::CanRename(void)
{
    return true;
}

bool wxGxFolder::Rename(const wxString &sNewName)
{
	wxFileName PathName(wxString(m_sPath, wxConvUTF8));
	PathName.SetName(sNewName);

	wxString sNewPath = PathName.GetFullPath();

    CPLString szNewPath(sNewPath.mb_str(wxConvUTF8));
    if(RenameFile(m_sPath, szNewPath))
	{
        if (m_bIsChildrenLoaded)
            Refresh();
		return true;
	}
	else
    {
		wxString sErr = wxString::Format(_("Operation '%s' failed!"), _("Rename"));   
		sErr += wxT("\n") + wxString::Format(wxT("%s '%s' - '%s'"), GetCategory().c_str(), wxString::FromUTF8(m_sPath), wxString::FromUTF8(szNewPath));
		wxGISLogError(sErr, wxString::FromUTF8(CPLGetLastErrorMsg()), wxEmptyString, NULL);
		return false;
    }
	return false;
}

bool wxGxFolder::CanCreate(long nDataType, long DataSubtype)
{
	return wxIsWritable(wxString(m_sPath, wxConvUTF8));
}

bool wxGxFolder::HasChildren(bool bWaitLoading)
{
    LoadChildren();
    return wxGxObjectContainer::HasChildren(bWaitLoading);
}

bool wxGxFolder::CanCopy(const CPLString &szDestPath)
{
    return true;
}

bool wxGxFolder::Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
    if(pTrackCancel)
		pTrackCancel->PutMessage(wxString::Format(_("%s %s %s"), _("Copy"), GetCategory().c_str(), m_sName.c_str()), wxNOT_FOUND, enumGISMessageInformation);

    //CPLString szFullDestPath = CPLFormFilename(szDestPath, CPLGetFilename(m_sPath), NULL);
    CPLString szFullDestPath = CheckUniqPath(szDestPath, CPLGetFilename(m_sPath), true, " ");

    bool bRet = CopyDir(m_sPath, szFullDestPath, m_nDefaultCreateDirMode, pTrackCancel);
    if(!bRet)
    {
		wxString sErr = wxString::Format(_("Operation '%s' failed!"), _("Copy"));		  
		sErr += wxT("\n") + wxString::Format(wxT("%s '%s'"), GetCategory().c_str(), wxString::FromUTF8(m_sPath));
		wxGISLogError(sErr, wxString::FromUTF8(CPLGetLastErrorMsg()), wxEmptyString, pTrackCancel);
		return false;
    }

    return true;
}

bool wxGxFolder::CanMove(const CPLString &szDestPath)
{
    return CanCopy(szDestPath) && CanDelete();
}

bool wxGxFolder::Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
    if(pTrackCancel)
		pTrackCancel->PutMessage(wxString::Format(_("%s %s %s"), _("Move"), GetCategory().c_str(), m_sName.c_str()), wxNOT_FOUND, enumGISMessageInformation);

    //CPLString szFullDestPath = CPLFormFilename(szDestPath, CPLGetFilename(m_sPath), NULL);
    CPLString szFullDestPath = CheckUniqPath(szDestPath, CPLGetFilename(m_sPath), true, " ");

    bool bRet = MoveDir(m_sPath, szFullDestPath, m_nDefaultCreateDirMode, pTrackCancel);
    if(!bRet)
    {
		wxString sErr = wxString::Format(_("Operation '%s' failed!"), _("Move"));
		sErr += wxT("\n") + wxString::Format(wxT("%s '%s'"), GetCategory().c_str(), wxString::FromUTF8(m_sPath));  
		wxGISLogError(sErr, wxString::FromUTF8(CPLGetLastErrorMsg()), wxEmptyString, pTrackCancel);
		return false;
    }

    return true;
}

bool wxGxFolder::AreChildrenViewable(void) const
{
    return true;
}
