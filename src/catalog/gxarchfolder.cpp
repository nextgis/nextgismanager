/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxArchive classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011,2013,2014 Dmitry Baryshnikov
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
#include "wxgis/catalog/gxarchfolder.h"
#include "wxgis/catalog/gxcatalog.h"
#include "wxgis/datasource/sysop.h"
#include "wxgis/core/app.h"

#include "cpl_vsi_virtual.h"

//----------------------------------------------------------------------------
// wxGxArchive
//----------------------------------------------------------------------------
IMPLEMENT_CLASS(wxGxArchive, wxGxArchiveFolder)

wxGxArchive::wxGxArchive(wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxArchiveFolder(oParent, soName, soPath)
{
}

wxGxArchive::~wxGxArchive(void)
{
}

wxString wxGxArchive::GetBaseName(void) const
{
    wxFileName FileName(m_sName);
    FileName.SetEmptyExt();
    return FileName.GetName();
}

CPLString wxGxArchive::GetRealPath() const
{
    wxString sThisPath(m_sPath, wxConvUTF8);
    wxString sRealPath;
    if(sThisPath.StartsWith(wxT("/vsizip/"), &sRealPath))
    {
        sThisPath = sRealPath;
    }
    return  CPLString(sThisPath.ToUTF8());
}

bool wxGxArchive::Delete(void)
{
    CPLString szOldPath = GetRealPath();

    if(DeleteFile(szOldPath))
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

bool wxGxArchive::Rename(const wxString &sNewName)
{
    wxString sThisPath(m_sPath, wxConvUTF8);
    wxString sRealPath;
    if(sThisPath.StartsWith(wxT("/vsizip/"), &sRealPath))
    {
        sThisPath = sRealPath;
    }

	wxFileName PathName(sThisPath);
	PathName.SetName(ClearExt(sNewName));

	wxString sNewPath = PathName.GetFullPath();

    CPLString szNewPath(sNewPath.ToUTF8());
    CPLString szOldPath(sThisPath.ToUTF8());
    if(RenameFile(szOldPath, szNewPath))
	{
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

wxGxObject *wxGxArchive::FindGxObjectByPath(const wxString &sPath)
{
    wxString sThisPath(m_sPath, wxConvUTF8);

    if(sThisPath.IsSameAs(sPath, false))
        return (wxGxObject *)this;
    wxString sTestPath = wxT("/vsizip/") + sPath;
    if(sThisPath.IsSameAs(sTestPath, false))
        return (wxGxObject *)this;
    return NULL;
}

bool wxGxArchive::Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
    if(pTrackCancel)
        pTrackCancel->PutMessage(wxString(_("Copy file ")) + m_sName, -1, enumGISMessageInfo);

    CPLString szRealPath = GetRealPath();
	CPLString szFileName = CPLGetBasename(szRealPath);
	CPLString szNewDestFileName = GetUniqPath(szRealPath, szDestPath, szFileName);
    return CopyFile(szRealPath, szNewDestFileName, pTrackCancel);
}

bool wxGxArchive::Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
    if(pTrackCancel)
        pTrackCancel->PutMessage(wxString(_("Move file ")) + m_sName, -1, enumGISMessageInfo);

    CPLString szRealPath = GetRealPath();
	CPLString szFileName = CPLGetBasename(szRealPath);
	CPLString szNewDestFileName = GetUniqPath(szRealPath, szDestPath, szFileName);
    return MoveFile(szRealPath, szNewDestFileName, pTrackCancel);
}

//----------------------------------------------------------------------------
// wxGxArchiveFolder
//----------------------------------------------------------------------------
IMPLEMENT_CLASS(wxGxArchiveFolder, wxGxFolder)

wxGxArchiveFolder::wxGxArchiveFolder() : wxGxFolder()
{
}

wxGxArchiveFolder::wxGxArchiveFolder(wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxFolder(oParent, soName, soPath)
{
}

wxGxArchiveFolder::~wxGxArchiveFolder(void)
{
}

bool wxGxArchiveFolder::IsArchive(void) const
{
    return true;
}

void wxGxArchiveFolder::LoadChildren(void)
{
	if(m_bIsChildrenLoaded)
		return;


	m_bIsChildrenLoaded = true;
#ifdef __WINDOWS__
    wxString sCharset(wxT("cp-866"));
#endif // __WINDOWS__

#ifdef __UNIX__
    wxString sCharset(wxT("CP866"));
#endif // __UNIX__

    wxGISAppConfig oConfig = GetConfig();
    if(oConfig.IsOk())
        sCharset = oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/zip/charset")), sCharset);
    CPLSetConfigOption("CPL_ZIP_ENCODING", sCharset.mb_str());

    char **papszItems = CPLReadDir(m_sPath);
    if(papszItems == NULL)
        return;

    char **papszFileList = NULL;
    //remove unused items
    for(int i = CSLCount(papszItems) - 1; i >= 0; i-- )
    {
        if( wxGISEQUAL(papszItems[i],".") || wxGISEQUAL(papszItems[i],"..") )
            continue;
        CPLString szFileName = m_sPath;
        szFileName += "/";
        szFileName += papszItems[i];

        VSIStatBufL BufL;
        int ret = VSIStatL(szFileName, &BufL);
        if(ret == 0)
        {
            if(VSI_ISDIR(BufL.st_mode))
            {
                wxString sFileName = wxString::FromUTF8(papszItems[i]);
				GetArchiveFolder(this, sFileName, szFileName);
            }
            else
            {
                papszFileList = CSLAddString( papszFileList, szFileName );
            }
        }
    }
    CSLDestroy( papszItems );

    //load names
    wxGxCatalog *pCatalog = wxDynamicCast(GetGxCatalog(), wxGxCatalog);

	if(pCatalog)
    {
        wxArrayLong ChildrenIds;
        pCatalog->CreateChildren(this, papszFileList, ChildrenIds);
        //for(size_t i = 0; i < ChildrenIds.GetCount(); ++i)
        //    pCatalog->ObjectAdded(ChildrenIds[i]);
	}

    CSLDestroy( papszFileList );
}

wxGxObject* wxGxArchiveFolder::GetArchiveFolder(wxGxObject *oParent, const wxString &soName, const CPLString &soPath)
{
	wxGxArchiveFolder* pFolder = new wxGxArchiveFolder(oParent, soName, soPath);
	return wxStaticCast(pFolder, wxGxObject);
}

