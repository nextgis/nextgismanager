/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISDataset class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2013 Bishop
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

#include "wxgis/datasource/dataset.h"
#include "wxgis/datasource/sysop.h"

//------------------------------------------------------------------------------
// wxGISDataset
//------------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxGISDataset, wxObject)

wxGISDataset::wxGISDataset(const CPLString &sPath) : wxGISPointer()
{
    m_sPath = sPath;
    m_bIsOpened = false;
    m_bIsReadOnly = true;
    m_bIsCached = true;
}

wxGISDataset::~wxGISDataset()
{
}

wxGISEnumDatasetType wxGISDataset::GetType(void) const
{
    return m_nType;
}

int wxGISDataset::GetSubType(void) const
{
    return m_nSubType;
}

void wxGISDataset::SetSubType(int nSubType)
{
    m_nSubType = nSubType;
}

CPLString wxGISDataset::GetPath(void) const
{
    return m_sPath;
}

size_t wxGISDataset::GetSubsetsCount(void) const
{
    return 0;
}

wxString wxGISDataset::GetName(void) const
{
    return wxEmptyString;
}

void wxGISDataset::Close(void)
{
    if(IsOpened())
    {
        m_bIsOpened = false;
        m_bIsReadOnly = true;
        m_bIsCached = true;
        m_SpatialReference = wxNullSpatialReference;
    }
}

bool wxGISDataset::IsOpened(void) const
{
    return m_bIsOpened;
}

bool wxGISDataset::IsReadOnly(void) const
{
    return m_bIsReadOnly;
}

bool wxGISDataset::IsCached(void) const
{
    return m_bIsCached;
}

bool wxGISDataset::IsCaching(void) const
{
    return false;
}

void wxGISDataset::StopCaching(void)
{
}

wxGISDataset* wxGISDataset::GetSubset(size_t nIndex)
{
    return NULL;
}

CPLString wxGISDataset::FixPathSeparator(CPLString &szPath)
{
	for(size_t i = 0; i < szPath.size(); ++i)
		if(szPath[i] == '/')
			szPath[i] = '\\';
    return szPath;
}

const wxGISSpatialReference wxGISDataset::GetSpatialReference(void)
{
    return m_SpatialReference;
}


bool wxGISDataset::CanDelete(void)
{
    return true;
}

bool wxGISDataset::CanRename(void)
{
    return true;
}

bool wxGISDataset::CanCopy(const CPLString &szDestPath)
{
    return true;
}

bool wxGISDataset::CanMove(const CPLString &szDestPath) 
{
    return CanCopy(szDestPath) & CanDelete();
}


bool wxGISDataset::Delete(ITrackCancel* const pTrackCancel)
{
	wxCriticalSectionLocker locker(m_CritSect);

    Close();

    if(!DeleteFile(m_sPath))
        return false;

    char** papszFileList = GetFileList();
    if(papszFileList)
    {
		IProgressor* pProgressor(NULL);
		if(pTrackCancel)
		{
			pProgressor = pTrackCancel->GetProgressor();
			if(pProgressor)
				pProgressor->SetRange(CSLCount(papszFileList));
		}
        for(int i = 0; papszFileList[i] != NULL; ++i )
		{
			DeleteFile(papszFileList[i]);
			if(pProgressor)
				pProgressor->SetValue(i);
		}
        CSLDestroy( papszFileList );
    }
	return true;
}

bool wxGISDataset::Rename(const wxString &sNewName, ITrackCancel* const pTrackCancel)
{
	wxCriticalSectionLocker locker(m_CritSect);

    Close();

    CPLString szDirPath = CPLGetPath(m_sPath);
    CPLString szName = CPLGetBasename(m_sPath);
	CPLString szNewName(ClearExt(sNewName).mb_str(wxConvUTF8));

    char** papszFileList = GetFileList();
    papszFileList = CSLAddString( papszFileList, m_sPath );
    if(!papszFileList)    
    {
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("No files to rename"), wxNOT_FOUND, enumGISMessageErr);
        return false;
    }

    char **papszNewFileList = NULL;

    for(int i = 0; papszFileList[i] != NULL; ++i )
    {
        CPLString szNewPath(CPLFormFilename(szDirPath, szNewName, GetExtension(papszFileList[i], szName)));
        papszNewFileList = CSLAddString(papszNewFileList, szNewPath);
        if(!RenameFile(papszFileList[i], papszNewFileList[i], pTrackCancel))
        {
            // Try to put the ones we moved back.
            for( --i; i >= 0; i-- )
                RenameFile( papszNewFileList[i], papszFileList[i]);

 			CSLDestroy( papszFileList );
			CSLDestroy( papszNewFileList );
            return false;
        }
    }

	m_sPath = CPLString(CPLFormFilename(szDirPath, szNewName, CPLGetExtension(m_sPath)));

	CSLDestroy( papszFileList );
	CSLDestroy( papszNewFileList );
	return true;
}

bool wxGISDataset::Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
	wxCriticalSectionLocker locker(m_CritSect);

    Close();

    char** papszFileList = GetFileList();
    papszFileList = CSLAddString( papszFileList, m_sPath );
    if(!papszFileList)    
    {
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("No files to move"), wxNOT_FOUND, enumGISMessageErr);
        return false;
    }

    CPLString szFileName = CPLGetBasename(GetUniqPath(m_sPath, szDestPath, CPLGetBasename(m_sPath)));

    char** papszMovedFileList = NULL;

	for(int i = 0; papszFileList[i] != NULL; ++i )
    {
		CPLString szNewDestFileName(CPLFormFilename(szDestPath, szFileName, GetExtension(papszFileList[i], szFileName)));
        papszMovedFileList = CSLAddString(papszMovedFileList, szNewDestFileName);
        if(!MoveFile(papszFileList[i], szNewDestFileName, pTrackCancel))
		{
            // Try to put the ones we moved back.
            pTrackCancel->Reset();
            for( --i; i >= 0; i-- )
                MoveFile( papszMovedFileList[i], papszFileList[i] );

			CSLDestroy( papszFileList );
			CSLDestroy( papszMovedFileList );
            return false;
		}
    }

    m_sPath = CPLFormFilename(szDestPath, CPLGetFilename(m_sPath), NULL);

	CSLDestroy( papszFileList );
	CSLDestroy( papszMovedFileList );
    return true;
}


bool wxGISDataset::Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
	wxCriticalSectionLocker locker(m_CritSect);

    Close();

    char** papszFileList = GetFileList();
    papszFileList = CSLAddString( papszFileList, m_sPath );
    if(!papszFileList)    
    {
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("No files to copy"), wxNOT_FOUND, enumGISMessageErr);
        return false;
    }

    CPLString szFileName = CPLGetBasename(GetUniqPath(m_sPath, szDestPath, CPLGetBasename(m_sPath)));

    char** papszFileCopiedList = NULL;

    for(int i = 0; papszFileList[i] != NULL; ++i )
    {
        CPLString szNewDestFileName(CPLFormFilename(szDestPath, szFileName, GetExtension(papszFileList[i], szFileName)));
        papszFileCopiedList = CSLAddString(papszFileCopiedList, szNewDestFileName);
        if(!CopyFile(papszFileList[i], szNewDestFileName, pTrackCancel))
		{
            // Try to put the ones we moved back.
            for( --i; i >= 0; i-- )
                DeleteFile( papszFileCopiedList[i] );

			CSLDestroy( papszFileList );
			CSLDestroy( papszFileCopiedList );
            return false;
		}
    }

    CSLDestroy( papszFileList );
	CSLDestroy( papszFileCopiedList );
	return true;
}
