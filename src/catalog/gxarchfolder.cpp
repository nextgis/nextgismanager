/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxArchive classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011,2013 Dmitry Baryshnikov
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

#include "cpl_vsi_virtual.h"

//----------------------------------------------------------------------------
// wxGxArchive
//----------------------------------------------------------------------------
IMPLEMENT_CLASS(wxGxArchive, wxGxFolder)

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

bool wxGxArchive::Delete(void)
{
	int nCount = 0;
    for(size_t i = 1; i < m_sPath.length(); ++i)
    {
        nCount++;
        if(m_sPath[i] == '/')
            break;
    }

    m_sPath.erase(0, nCount + 1);

    if(DeleteFile(m_sPath))
	{
        return true;
	}
	else
    {
        const char* err = CPLGetLastErrorMsg();
		wxLogError(_("Operation '%s' failed! GDAL error: %s, file '%s'"), _("Delete"), wxString(err, wxConvUTF8).c_str(), wxString(m_sPath, wxConvUTF8).c_str());
        return false;
    }
    return false;
}

bool wxGxArchive::Rename(const wxString &sNewName)
{
    CPLString szType("/");
    int nCount = 0;
    for(size_t i = 1; i < m_sPath.length(); ++i)
    {
        nCount++;
        szType += m_sPath[i];
        if(m_sPath[i] == '/')
            break;
    }

    m_sPath.erase(0, nCount + 1);

	wxFileName PathName(wxString(m_sPath, wxConvUTF8));
	PathName.SetName(ClearExt(sNewName));

	wxString sNewPath = PathName.GetFullPath();

    CPLString szNewPath(sNewPath.mb_str(wxConvUTF8));
    if(RenameFile(m_sPath, szNewPath))
	{
		return true;
	}
	else
    {
        const char* err = CPLGetLastErrorMsg();
		wxLogError(_("Operation '%s' failed! GDAL error: %s, file '%s'"), _("Rename"), wxString(err, wxConvUTF8).c_str(), wxString(m_sPath, wxConvUTF8).c_str());
		return false;
    }
	return false;
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
                wxString sFileName;
                if (IsArchive())
                {
		            wxString sCharset(wxT("cp-866"));
		            wxGISAppConfig oConfig = GetConfig();
                    if(oConfig.IsOk())
			            sCharset = oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/zip/charset")), sCharset);
                    sFileName = wxString(papszItems[i], wxCSConv(sCharset));
                }
                else
                {
                    sFileName = wxString::FromUTF8(papszItems[i]);
                }

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

	m_bIsChildrenLoaded = true;
}

wxGxObject* wxGxArchiveFolder::GetArchiveFolder(wxGxObject *oParent, const wxString &soName, const CPLString &soPath)
{
	wxGxArchiveFolder* pFolder = new wxGxArchiveFolder(oParent, soName, soPath);
	return wxStaticCast(pFolder, wxGxObject);
}

