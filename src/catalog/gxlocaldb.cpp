/******************************************************************************
* Project:  wxGIS (GIS Catalog)
* Purpose:  local db (sqlite, gdab, mdb) classes.
* Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
******************************************************************************
*   Copyright (C) 2014 Dmitry Baryshnikov
*   Copyright (C) 2014 NextGIS
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
#include "wxgis/catalog/gxlocaldb.h"
#include "wxgis/catalog/gxcatalog.h"
#include "wxgis/datasource/sysop.h"
#include "wxgis/datasource/datacontainer.h"

//---------------------------------------------------------------------------
// wxGxFolder
//---------------------------------------------------------------------------
IMPLEMENT_CLASS(wxGxOpenFileGDB, wxGxDatasetContainer)

wxGxOpenFileGDB::wxGxOpenFileGDB(wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxDatasetContainer(oParent, soName, soPath)
{
    m_pwxGISDataset = NULL;
    m_bIsMetadataFilled = false;
    m_bIsChildrenLoaded = false;
}

wxGxOpenFileGDB::~wxGxOpenFileGDB(void)
{
}

wxGISDataset* const wxGxOpenFileGDB::GetDatasetFast(void)
{
    if (m_pwxGISDataset == NULL)
    {
        wxGISDataSource* pDSet = new wxGISDataSource(m_sPath);
        m_pwxGISDataset = wxStaticCast(pDSet, wxGISDataset);
        m_pwxGISDataset->Reference();
    }
    wsGET(m_pwxGISDataset);
}

void wxGxOpenFileGDB::LoadChildren(void)
{
	if(m_bIsChildrenLoaded)
		return;

    wxGISDataSource* pDSource = wxDynamicCast(GetDatasetFast(), wxGISDataSource);

    if (NULL == pDSource)
    {
        return;
    }

    pDSource->Open();

    for (size_t i = 0; i < pDSource->GetSubsetsCount(); ++i)
    {
        wxGISDataset* pDSet = pDSource->GetSubset(i);
        if (NULL == pDSet)
            continue;
        switch (pDSet->GetType())
        {
        case enumGISFeatureDataset:
            new wxGxInitedFeatureDataset(pDSet, wxStaticCast(this, wxGxObject), pDSet->GetName(), wxGxObjectContainer::GetPath());
            break;
        case enumGISTableDataset:
            new wxGxInitedTableDataset(pDSet, wxStaticCast(this, wxGxObject), pDSet->GetName(), wxGxObjectContainer::GetPath());
            break;
        case enumGISRasterDataset:
            //TODO:
            break;
        default:
            break;
        }
    }
    wsDELETE(pDSource);

	m_bIsChildrenLoaded = true;
}

bool wxGxOpenFileGDB::CanCreate(long nDataType, long DataSubtype)
{
	return false;
}

bool wxGxOpenFileGDB::AreChildrenViewable(void) const
{
    return true;
}

void wxGxOpenFileGDB::FillMetadata(bool bForce)
{
    if (m_bIsMetadataFilled && !bForce)
        return;
    m_bIsMetadataFilled = true;

    wxGISDataset* pDSet = GetDatasetFast();
    if (NULL == pDSet)
    {
        return;
    }

    VSIStatBufL BufL;
    wxULongLong nSize(0);
    wxDateTime dt(1,wxDateTime::Jan, 1900);
    int ret = VSIStatL(m_sPath, &BufL);
    if (ret == 0)
    {
        //nSize += BufL.st_size;
        //dt = wxDateTime(BufL.st_mtime);

        if (VSI_ISDIR(BufL.st_mode))
        {
            char **papszItems = CPLReadDir(wxGxObjectContainer::GetPath());
            if (papszItems && CSLCount(papszItems) > 0)
            {
                for (int i = 0; papszItems[i] != NULL; ++i)
                {
                    if (wxGISEQUAL(papszItems[i], ".") || wxGISEQUAL(papszItems[i], ".."))
                        continue;
                    CPLString szFullPathFrom = CPLFormFilename(wxGxObjectContainer::GetPath(), papszItems[i], NULL);

                    ret = VSIStatL(szFullPathFrom, &BufL);
                    if (ret == 0)
                    {
                        nSize += BufL.st_size;
                        wxDateTime dtt(BufL.st_mtime);
                        if (dtt > dt)
                            dt = dtt;
                    }
                }
            }
            CSLDestroy(papszItems);
        }
    }


    m_nSize = nSize;
    m_dtMod = dt;

    wsDELETE(pDSet);
}

wxGISDataset* const wxGxOpenFileGDB::GetDataset(bool bCached, ITrackCancel* const pTrackCancel)
{
    wxGISDataSource* pwxGISDataSource = wxDynamicCast(GetDatasetFast(), wxGISDataSource);

    if (NULL != pwxGISDataSource && !pwxGISDataSource->IsOpened())
    {
        if (!pwxGISDataSource->Open(TRUE))
        {
            wsDELETE(pwxGISDataSource);
            const char* err = CPLGetLastErrorMsg();
            wxString sErr = wxString::Format(_("Operation '%s' failed! GDAL error: %s"), _("Open"), wxString(err, wxConvUTF8).c_str());
            wxLogError(sErr);
            if (pTrackCancel)
                pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageErr);
            return NULL;
        }
        wxGIS_GXCATALOG_EVENT(ObjectChanged);
        wsDELETE(pwxGISDataSource);
    }

    wsGET(m_pwxGISDataset);

}

wxGISEnumDatasetType wxGxOpenFileGDB::GetType(void) const
{
    return enumGISContainer;
}

int wxGxOpenFileGDB::GetSubType(void) const
{
    return enumContGDBFolder;
}


bool wxGxOpenFileGDB::IsMetadataFilled() const
{
    return m_bIsMetadataFilled;
}

bool wxGxOpenFileGDB::CanDelete(void)
{
    return true;
}

bool wxGxOpenFileGDB::Delete(void)
{
    wxGISDataset* pDSet = GetDatasetFast();

    if (NULL != pDSet)
    {
        pDSet->Close();
        wsDELETE(pDSet);
    }

    if (DeleteDir(m_sPath))
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

bool wxGxOpenFileGDB::CanRename(void)
{
    return true;
}

bool wxGxOpenFileGDB::Rename(const wxString &sNewName)
{
    wxGISDataset* pDSet = GetDatasetFast();

    if (NULL != pDSet)
    {
        pDSet->Close();
        wsDELETE(pDSet);
    }

    wxFileName PathName(wxString(m_sPath, wxConvUTF8));
    PathName.SetName(ClearExt(sNewName));

    wxString sNewPath = PathName.GetFullPath();

    CPLString szNewPath(sNewPath.mb_str(wxConvUTF8));
    if (RenameFile(m_sPath, szNewPath))
    {
        if (m_bIsChildrenLoaded)
            Refresh();
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

bool wxGxOpenFileGDB::CanCopy(const CPLString &szDestPath)
{
    return true;
}

bool wxGxOpenFileGDB::Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
    wxGISDataset* pDSet = GetDatasetFast();

    if (NULL != pDSet)
    {
        pDSet->Close();
        wsDELETE(pDSet);
    }

    if (pTrackCancel)
        pTrackCancel->PutMessage(wxString::Format(_("%s %s %s"), _("Copy"), GetCategory().c_str(), m_sName.c_str()), wxNOT_FOUND, enumGISMessageInfo);

    //CPLString szFullDestPath = CPLFormFilename(szDestPath, CPLGetFilename(m_sPath), NULL);
    CPLString szFullDestPath = CheckUniqPath(szDestPath, CPLGetFilename(m_sPath), true, " ");

    bool bRet = CopyDir(m_sPath, szFullDestPath, 777, pTrackCancel);
    if (!bRet)
    {
        const char* err = CPLGetLastErrorMsg();
        wxString sErr = wxString::Format(_("Operation '%s' failed! GDAL error: %s, %s '%s'"), _("Copy"), GetCategory().c_str(), wxString(err, wxConvUTF8).c_str(), wxString(m_sPath, wxConvUTF8).c_str());
        wxLogError(sErr);
        if (pTrackCancel)
            pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageErr);
        return false;
    }

    return true;
}

bool wxGxOpenFileGDB::CanMove(const CPLString &szDestPath)
{
    return CanCopy(szDestPath) & CanDelete();
}

bool wxGxOpenFileGDB::Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
    wxGISDataset* pDSet = GetDatasetFast();

    if (NULL != pDSet)
    {
        pDSet->Close();
        wsDELETE(pDSet);
    }

    if (pTrackCancel)
        pTrackCancel->PutMessage(wxString::Format(_("%s %s %s"), _("Move"), GetCategory().c_str(), m_sName.c_str()), wxNOT_FOUND, enumGISMessageInfo);

    //CPLString szFullDestPath = CPLFormFilename(szDestPath, CPLGetFilename(m_sPath), NULL);
    CPLString szFullDestPath = CheckUniqPath(szDestPath, CPLGetFilename(m_sPath), true, " ");

    bool bRet = MoveDir(m_sPath, szFullDestPath, 777, pTrackCancel);
    if (!bRet)
    {
        const char* err = CPLGetLastErrorMsg();
        wxString sErr = wxString::Format(_("Operation '%s' failed! GDAL error: %s, %s '%s'"), _("Move"), GetCategory().c_str(), wxString(err, wxConvUTF8).c_str(), wxString(m_sPath, wxConvUTF8).c_str());
        wxLogError(sErr);
        if (pTrackCancel)
            pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageErr);
        return false;
    }

    return true;
}

bool wxGxOpenFileGDB::HasChildren(void)
{
    LoadChildren();
    return wxGxDatasetContainer::HasChildren();
}

//--------------------------------------------------------------
// wxGxInitedFeatureDataset
//--------------------------------------------------------------
IMPLEMENT_CLASS(wxGxInitedFeatureDataset, wxGxFeatureDataset)

wxGxInitedFeatureDataset::wxGxInitedFeatureDataset(wxGISDataset* pwxGISDataset, wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxFeatureDataset(enumVecFileDBLayer, oParent, soName, soPath)
{
    wsSET(m_pwxGISDataset, pwxGISDataset);

    m_sPath = CPLString(CPLFormFilename(soPath, soName.mb_str(wxConvUTF8), ""));
}

wxGxInitedFeatureDataset::~wxGxInitedFeatureDataset(void)
{
    wsDELETE(m_pwxGISDataset);
}

//--------------------------------------------------------------
// wxGxInitedTableDataset
//--------------------------------------------------------------
IMPLEMENT_CLASS(wxGxInitedTableDataset, wxGxTableDataset)

wxGxInitedTableDataset::wxGxInitedTableDataset(wxGISDataset* pwxGISDataset, wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxTableDataset(enumTableFileDBLayer, oParent, soName, soPath)
{
    wsSET(m_pwxGISDataset, pwxGISDataset);

    m_sPath = CPLString(CPLFormFilename(soPath, soName.mb_str(wxConvUTF8), ""));
}

wxGxInitedTableDataset::~wxGxInitedTableDataset(void)
{
    wsDELETE(m_pwxGISDataset);
}