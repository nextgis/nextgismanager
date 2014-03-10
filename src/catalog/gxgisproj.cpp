/******************************************************************************
* Project:  wxGIS (GIS Catalog)
* Purpose:  GIS Prolect classes
* Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
******************************************************************************
*   Copyright (C) 2014 Bishop
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
#include "wxgis/catalog/gxgisproj.h"
#include "wxgis/catalog/gxcatalog.h"
#include "wxgis/datasource/sysop.h"

//---------------------------------------------------------------------------
// wxGxQGISProjFile
//---------------------------------------------------------------------------
IMPLEMENT_CLASS(wxGxQGISProjFile, wxGxObjectContainer)

wxGxQGISProjFile::wxGxQGISProjFile(wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxObjectContainer(oParent, soName, soPath), m_bIsChildrenLoaded(false)
{
}

wxGxQGISProjFile::~wxGxQGISProjFile(void)
{
}

wxString wxGxQGISProjFile::GetBaseName(void) const
{
    return GetName();
}

void wxGxQGISProjFile::Refresh(void)
{
	DestroyChildren();
    m_bIsChildrenLoaded = false;
	LoadChildren();
    wxGIS_GXCATALOG_EVENT(ObjectRefreshed);
}

void wxGxQGISProjFile::LoadChildren(void)
{
	if(m_bIsChildrenLoaded)
        return;

    struct _pg_data{
        wxString sName;
        wxString sPass;
        wxString sPort;
        wxString sAddres;
        wxString sDBName;
        wxArrayString saTables;
    };

    wxString sPath = wxString::FromUTF8(m_sPath);
    wxFileName oFProjPath(sPath);
    wxXmlDocument oProjectFile(sPath);
    if (oProjectFile.IsOk())
    {
        m_bIsChildrenLoaded = true;

        wxXmlNode* pRoot = oProjectFile.GetRoot();

        //TODO: link object with link add (->) to icon 

        wxXmlNode* pMainCategories = pRoot->GetChildren();
        wxXmlNode* pProjectLayers = NULL;

        char **papszFileList = NULL;

        //projectlayers
        while (pMainCategories != NULL)
        {
            if (pMainCategories->GetName().IsSameAs(wxT("projectlayers"), false))
            {
                pProjectLayers = pMainCategories;
            }
            pMainCategories = pMainCategories->GetNext();
        }

        if (pProjectLayers != NULL)
        {
            wxVector<struct _pg_data> staPG_Tables;

            wxXmlNode* pProjectLayer = pProjectLayers->GetChildren();
            while (pProjectLayer)
            {
                wxString sDatasource, sProvider;
                wxXmlNode* pProjectLayerProp = pProjectLayer->GetChildren();
                while (pProjectLayerProp)
                {
                    if (pProjectLayerProp->GetName().IsSameAs(wxT("datasource"), false))
                    {
                        sDatasource = pProjectLayerProp->GetNodeContent();
                    }
                    if (pProjectLayerProp->GetName().IsSameAs(wxT("provider"), false))
                    {
                        sProvider = pProjectLayerProp->GetNodeContent();
                    }

                    if (!sProvider.IsEmpty() && !sDatasource.IsEmpty())
                        break;
                    pProjectLayerProp = pProjectLayerProp->GetNext();
                }

                if (sProvider.IsSameAs(wxT("ogr"), false))
                {
                    bool bIsOk = true;
                    wxFileName oFName(sDatasource);
                    if (oFName.IsRelative())
                    {
                        bIsOk = oFName.MakeAbsolute(oFProjPath.GetPath());
                    }

                    if (bIsOk && oFName.Exists())
                    {
                        if (CSLFindString(papszFileList, oFName.GetFullPath().ToUTF8()) == -1)
                            papszFileList = CSLAddString(papszFileList, oFName.GetFullPath().ToUTF8());
                    }
                }
                else if (sProvider.IsSameAs(wxT("gdal"), false))
                {
                    bool bIsOk = true;
                    wxFileName oFName(sDatasource);
                    if (oFName.IsRelative())
                    {
                        bIsOk = oFName.MakeAbsolute(oFProjPath.GetPath());
                    }

                    if (bIsOk && oFName.Exists())
                    {
                        if (CSLFindString(papszFileList, oFName.GetFullPath().ToUTF8()) == -1)
                        {
                            papszFileList = CSLAddString(papszFileList, oFName.GetFullPath().ToUTF8());
                        }
                    }
                }
                else if (sProvider.IsSameAs(wxT("postgres"), false))
                {
                    //TODO: staPG_Tables
                }

                pProjectLayer = pProjectLayer->GetNext();
            }

            //create children from path and load them
            wxGxCatalog* pCatalog = wxDynamicCast(GetGxCatalog(), wxGxCatalog);
            if (pCatalog != NULL)
            {
                wxArrayLong ChildrenIds;
                pCatalog->CreateChildren(this, papszFileList, ChildrenIds);
            }
            CSLDestroy(papszFileList);

            //Fill PG Tables
            //TODO: in UI add pending and load tables in thread

       }
    }
}

bool wxGxQGISProjFile::CanDelete(void)
{ 
    return true; 
}

bool wxGxQGISProjFile::Delete(void)
{
    if (DeleteFile(m_sPath))
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

bool wxGxQGISProjFile::CanRename(void)
{ 
    return true;
}

bool wxGxQGISProjFile::Rename(const wxString &sNewName)
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
        const char* err = CPLGetLastErrorMsg();
		wxLogError(_("Operation '%s' failed! GDAL error: %s, file '%s'"), _("Rename"), wxString(err, wxConvUTF8).c_str(), wxString(m_sPath, wxConvUTF8).c_str());
		return false;
    }
	return false;
}

bool wxGxQGISProjFile::CanCreate(long nDataType, long DataSubtype)
{
    //we don't support project file modification
	return false;
}

bool wxGxQGISProjFile::HasChildren(void)
{
    LoadChildren();     
    return wxGxObjectContainer::HasChildren();
}

bool wxGxQGISProjFile::CanCopy(const CPLString &szDestPath)
{ 
    return true; 
}

bool wxGxQGISProjFile::Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
    if(pTrackCancel)
		pTrackCancel->PutMessage(wxString::Format(_("%s %s %s"), _("Copy"), GetCategory().c_str(), m_sName.c_str()), wxNOT_FOUND, enumGISMessageInfo);

    //CPLString szFullDestPath = CPLFormFilename(szDestPath, CPLGetFilename(m_sPath), NULL);
    CPLString szFullDestPath = CheckUniqPath(szDestPath, CPLGetFilename(m_sPath), true, " ");

    bool bRet = CopyFile(m_sPath, szFullDestPath, pTrackCancel);
    if(!bRet)
    {
        const char* err = CPLGetLastErrorMsg();
        wxString sErr = wxString::Format(_("Operation '%s' failed! GDAL error: %s, %s '%s'"), _("Copy"), GetCategory().c_str(), wxString(err, wxConvUTF8).c_str(), wxString(m_sPath, wxConvUTF8).c_str());
		wxLogError(sErr);
        if(pTrackCancel)
            pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageErr);
		return false;	
    } 

    return true;
}

bool wxGxQGISProjFile::CanMove(const CPLString &szDestPath)
{ 
    return CanCopy(szDestPath) & CanDelete(); 
}

bool wxGxQGISProjFile::Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
    if(pTrackCancel)
		pTrackCancel->PutMessage(wxString::Format(_("%s %s %s"), _("Move"), GetCategory().c_str(), m_sName.c_str()), wxNOT_FOUND, enumGISMessageInfo);
    
    //CPLString szFullDestPath = CPLFormFilename(szDestPath, CPLGetFilename(m_sPath), NULL);
    CPLString szFullDestPath = CheckUniqPath(szDestPath, CPLGetFilename(m_sPath), true, " ");

    bool bRet = MoveFile(m_sPath, szFullDestPath, pTrackCancel);
    if(!bRet)
    {
        const char* err = CPLGetLastErrorMsg();
        wxString sErr = wxString::Format(_("Operation '%s' failed! GDAL error: %s, %s '%s'"), _("Move"), GetCategory().c_str(), wxString(err, wxConvUTF8).c_str(), wxString(m_sPath, wxConvUTF8).c_str());
		wxLogError(sErr);
        if(pTrackCancel)
            pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageErr);
		return false;	
    } 

    return true;
}

bool wxGxQGISProjFile::AreChildrenViewable(void) const
{ 
    return true; 
}
