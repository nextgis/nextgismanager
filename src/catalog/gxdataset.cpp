/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  GxDataset classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2013 Bishop
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

#include "wxgis/catalog/gxdataset.h"
#include "wxgis/catalog/gxcatalog.h"

#include "wxgis/datasource/featuredataset.h"
#include "wxgis/datasource/rasterdataset.h"
#include "wxgis/datasource/sysop.h"

//--------------------------------------------------------------
//class wxGxDataset
//--------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxGxDataset, wxGxObject)

wxGxDataset::wxGxDataset(wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxObject(oParent, soName, soPath)
{
    m_pwxGISDataset = NULL;
    m_nSize = 0;
    m_dtMod = wxInvalidDateTime;
    m_bIsMetadataFilled = false;
}

wxGxDataset::~wxGxDataset()
{
    wsDELETE(m_pwxGISDataset);
}

void wxGxDataset::FillMetadata(bool bForce)
{
    if(m_bIsMetadataFilled && !bForce)
        return;
    m_bIsMetadataFilled = true;

    //return for subdatasets
    if (m_sPath.find('?') != wxNOT_FOUND)
        return;

    wxGISDataset* pDSet = GetDatasetFast();
    if (NULL == pDSet)
    {
        return;
    }

    VSIStatBufL BufL;
    wxULongLong nSize(0);
    wxDateTime dt;
    int ret = VSIStatL(m_sPath, &BufL);
    if(ret == 0)
    {
        nSize += BufL.st_size;
        dt = wxDateTime(BufL.st_mtime);
    }    

    char** papszFileList = pDSet->GetFileList();
    if( papszFileList && CSLCount(papszFileList) > 0 )
    {
        for(int i = 0; papszFileList[i] != NULL; ++i )
	    {
            ret = VSIStatL(papszFileList[i], &BufL);
            if(ret == 0)
            {
                nSize += BufL.st_size;
                wxDateTime dtt(BufL.st_mtime);
                if(dtt > dt)
                    dt = dtt;
            }
	    }
    }
    CSLDestroy( papszFileList );
    
    m_nSize = nSize;
    m_dtMod = dt;

    wsDELETE(pDSet);
}

bool wxGxDataset::CanDelete(void)
{
	wxGISDataset* pDSet = GetDatasetFast();
    
    if (NULL == pDSet)
    {
        return false;
    }
    bool bRet = pDSet->CanDelete();
    wsDELETE(pDSet);
    return bRet;
}

bool wxGxDataset::CanRename(void)
{
	wxGISDataset* pDSet = GetDatasetFast();
    
    if (NULL == pDSet)
    {
        return false;
    }
    bool bRet = pDSet->CanRename();
    wsDELETE(pDSet);
    return bRet;
}

bool wxGxDataset::CanCopy(const CPLString &szDestPath)
{
	wxGISDataset* pDSet = GetDatasetFast();
    
    if (NULL == pDSet)
    {
        return false;
    }
    bool bRet = pDSet->CanCopy(szDestPath);
    wsDELETE(pDSet);
    return bRet;
}

bool wxGxDataset::CanMove(const CPLString &szDestPath) 
{
    return CanCopy(szDestPath) & CanDelete();
}

bool wxGxDataset::Delete(void)
{
	wxGISDataset* pDSet = GetDatasetFast();
    
    if (NULL == pDSet)
    {
        return false;
    }

    bool bRet = pDSet->Delete();
    wsDELETE(pDSet);

    if( !bRet )
    {
        const char* err = CPLGetLastErrorMsg();
		wxLogError(_("Operation '%s' failed! GDAL error: %s, %s '%s'"), _("Delete"), wxString(err, wxConvUTF8).c_str(), GetCategory().c_str(), wxString(m_sPath, wxConvUTF8).c_str());
		return false;	
    }
    return true;
}

bool wxGxDataset::Rename(const wxString &sNewName)
{
	wxGISDataset* pDSet = GetDatasetFast();
    
    if (NULL == pDSet)
    {
        return false;
    }

    if (pDSet->IsOpened())
    {
		pDSet->Close();
    }

    bool bRet = pDSet->Rename(sNewName);
    wsDELETE(pDSet);

	if( !bRet )
	{
		const char* err = CPLGetLastErrorMsg();
		wxLogError(_("Operation '%s' failed! GDAL error: %s, %s '%s'"), _("Rename"), wxString(err, wxConvUTF8).c_str(), GetCategory().c_str(), wxString(m_sPath, wxConvUTF8).c_str());
		return false;
	}	
    return true;
}


bool wxGxDataset::Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
    if(pTrackCancel)
        pTrackCancel->PutMessage(wxString::Format(_("%s %s %s"), _("Copy"), GetCategory().c_str(), m_sName.c_str()), wxNOT_FOUND, enumGISMessageInfo);

	wxGISDataset* pDSet = GetDatasetFast();
    
    if (NULL == pDSet)
    {
        if (pTrackCancel)
        {
            pTrackCancel->PutMessage(wxString::Format(_("%s %s %s failed!"), _("Copy"), GetCategory().c_str(), m_sName.c_str()), wxNOT_FOUND, enumGISMessageErr);
        }
        return false;
    }

    bool bRet = pDSet->Copy(szDestPath, pTrackCancel);
    wsDELETE(pDSet);

    if(!bRet)
    {
        const char* err = CPLGetLastErrorMsg();
        wxString sErr = wxString::Format(_("Operation '%s' failed! GDAL error: %s, %s '%s'"), _("Copy"), wxString(err, wxConvUTF8).c_str(), GetCategory().c_str(), wxString(m_sPath, wxConvUTF8).c_str());
		wxLogError(sErr);
        if(pTrackCancel)
            pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageErr);
		return false;	
    }
	
    return true;
}

bool wxGxDataset::Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
    if (pTrackCancel)
    {
		pTrackCancel->PutMessage(wxString::Format(_("%s %s %s"), _("Move"), GetCategory().c_str(), m_sName.c_str()), wxNOT_FOUND, enumGISMessageInfo);
    }

	wxGISDataset* pDSet = GetDatasetFast();
    
    if(NULL == pDSet)
    {
        if (pTrackCancel)
        {
            pTrackCancel->PutMessage(wxString::Format(_("%s %s %s failed!"), _("Move"), GetCategory().c_str(), m_sName.c_str()), wxNOT_FOUND, enumGISMessageErr);
        }
        return false;
    }

    bool bRet = pDSet->Move(szDestPath, pTrackCancel);
    wsDELETE(pDSet);

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

void wxGxDataset::SetPath(const CPLString &soPath)
{
    wsDELETE(m_pwxGISDataset);
    wxGxObject::SetPath(soPath);
}

//--------------------------------------------------------------
//class wxGxDatasetContainer
//--------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxGxDatasetContainer, wxGxObjectContainer)

wxGxDatasetContainer::wxGxDatasetContainer(wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxObjectContainer(oParent, soName, soPath)
{
    m_pwxGISDataset = NULL;
    m_nSize = 0;
    m_dtMod = wxInvalidDateTime;
    m_bIsMetadataFilled = false;
}

wxGxDatasetContainer::~wxGxDatasetContainer()
{
    wsDELETE(m_pwxGISDataset);
}

void wxGxDatasetContainer::FillMetadata(bool bForce)
{
    if(m_bIsMetadataFilled && !bForce)
        return;
    m_bIsMetadataFilled = true;

    wxGISDataset* pDSet = GetDatasetFast();
    if (NULL == pDSet)
    {
        return;
    }

    VSIStatBufL BufL;
    wxULongLong nSize(0);
    wxDateTime dt;
    int ret = VSIStatL(m_sPath, &BufL);
    if(ret == 0)
    {
        nSize += BufL.st_size;
        dt = wxDateTime(BufL.st_mtime);
    }    

    char** papszFileList = pDSet->GetFileList();
    if( papszFileList && CSLCount(papszFileList) > 0 )
    {
        for(int i = 0; papszFileList[i] != NULL; ++i )
	    {
            ret = VSIStatL(papszFileList[i], &BufL);
            if(ret == 0)
            {
                nSize += BufL.st_size;
                wxDateTime dtt(BufL.st_mtime);
                if(dtt > dt)
                    dt = dtt;
            }
	    }
    }
    CSLDestroy( papszFileList );
    
    m_nSize = nSize;
    m_dtMod = dt;

    wsDELETE(pDSet);
}

bool wxGxDatasetContainer::CanDelete(void)
{
    return true;
}

bool wxGxDatasetContainer::CanRename(void)
{
    return true;
}

bool wxGxDatasetContainer::CanCopy(const CPLString &szDestPath)
{
    return true;
}

bool wxGxDatasetContainer::CanMove(const CPLString &szDestPath) 
{
    return CanCopy(szDestPath) & CanDelete();
}

bool wxGxDatasetContainer::Delete(void)
{
	wxGISDataset* pDSet = GetDatasetFast();
    
    if (NULL == pDSet)
    {
        return false;
    }

    bool bRet = pDSet->Delete();
    wsDELETE(pDSet);

    if( !bRet )
    {
        const char* err = CPLGetLastErrorMsg();
		wxLogError(_("Operation '%s' failed! GDAL error: %s, %s '%s'"), _("Delete"), wxString(err, wxConvUTF8).c_str(), GetCategory().c_str(), wxString(m_sPath, wxConvUTF8).c_str());
		return false;	
    }
    return true;
}

bool wxGxDatasetContainer::Rename(const wxString &sNewName)
{
	wxGISDataset* pDSet = GetDatasetFast();
    
    if (NULL == pDSet)
    {
        return false;
    }

    if (pDSet->IsOpened())
    {
		pDSet->Close();
    }

    bool bRet = pDSet->Rename(sNewName);
    wsDELETE(pDSet);

	if( !bRet )
	{
		const char* err = CPLGetLastErrorMsg();
		wxLogError(_("Operation '%s' failed! GDAL error: %s, %s '%s'"), _("Rename"), wxString(err, wxConvUTF8).c_str(), GetCategory().c_str(), wxString(m_sPath, wxConvUTF8).c_str());
		return false;
	}	
    return true;
}


bool wxGxDatasetContainer::Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
    if (pTrackCancel)
    {
        pTrackCancel->PutMessage(wxString::Format(_("%s %s %s"), _("Copy"), GetCategory().c_str(), m_sName.c_str()), wxNOT_FOUND, enumGISMessageInfo);
    }

	wxGISDataset* pDSet = GetDatasetFast();
    
    if(NULL == pDSet)
    {
        if (pTrackCancel)
        {
            pTrackCancel->PutMessage(wxString::Format(_("%s %s %s failed!"), _("Copy"), GetCategory().c_str(), m_sName.c_str()), wxNOT_FOUND, enumGISMessageErr);
        }
        return false;
    }

    bool bRet = pDSet->Copy(szDestPath, pTrackCancel);
    wsDELETE(pDSet);

    if(!bRet)
    {
        const char* err = CPLGetLastErrorMsg();
        wxString sErr = wxString::Format(_("Operation '%s' failed! GDAL error: %s, %s '%s'"), _("Copy"), wxString(err, wxConvUTF8).c_str(), GetCategory().c_str(), wxString(m_sPath, wxConvUTF8).c_str());
		wxLogError(sErr);
        if(pTrackCancel)
            pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageErr);
		return false;	
    }
	
    return true;
}

bool wxGxDatasetContainer::Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
    if(pTrackCancel)
		pTrackCancel->PutMessage(wxString::Format(_("%s %s %s"), _("Move"), GetCategory().c_str(), m_sName.c_str()), wxNOT_FOUND, enumGISMessageInfo);

	wxGISDataset* pDSet = GetDatasetFast();
    
    if(NULL == pDSet)
    {
        if(pTrackCancel)
            pTrackCancel->PutMessage(wxString::Format(_("%s %s %s failed!"), _("Move"), GetCategory().c_str(), m_sName.c_str()), wxNOT_FOUND, enumGISMessageErr);
        return false;
    }

    bool bRet = pDSet->Move(szDestPath, pTrackCancel);
    wsDELETE(pDSet);

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

void wxGxDatasetContainer::SetPath(const CPLString &soPath)
{
    wsDELETE(m_pwxGISDataset);
    wxGxObject::SetPath(soPath);
}

//--------------------------------------------------------------
//class wxGxTableDataset
//--------------------------------------------------------------

IMPLEMENT_CLASS(wxGxTableDataset, wxGxDataset)

wxGxTableDataset::wxGxTableDataset(wxGISEnumTableDatasetType eType, wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxDataset(oParent, soName, soPath)
{
    m_eType = eType;
}

wxGxTableDataset::~wxGxTableDataset(void)
{    
}

wxString wxGxTableDataset::GetCategory(void) const
{
	return wxString(_("Table"));
}

wxGISDataset* const wxGxTableDataset::GetDatasetFast(void)
{
 	if(m_pwxGISDataset == NULL)
    {
        wxGISTable* pDSet = new wxGISTable(m_sPath, m_eType);
        m_pwxGISDataset = wxStaticCast(pDSet, wxGISDataset);
        m_pwxGISDataset->Reference();
    }
    wsGET(m_pwxGISDataset);
}

wxGISDataset* const wxGxTableDataset::GetDataset(bool bCache, ITrackCancel* const pTrackCancel)
{
    wxGISTable* pwxGISTable(NULL);
	pwxGISTable = wxDynamicCast(GetDatasetFast(), wxGISTable);

    if(NULL != pwxGISTable && !pwxGISTable->IsOpened())
    {
        if(!pwxGISTable->Open(0, 0, bCache, pTrackCancel))
        {
            wsDELETE(pwxGISTable);
		    const char* err = CPLGetLastErrorMsg();
			wxString sErr = wxString::Format(_("Operation '%s' failed! GDAL error: %s"), _("Open"), wxString(err, wxConvUTF8).c_str());
            wxLogError(sErr);
			if(pTrackCancel)
				pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageErr);
			return NULL;
        }
        wxGIS_GXCATALOG_EVENT(ObjectChanged);
        wsDELETE(pwxGISTable);
	}

	wsGET(m_pwxGISDataset);
}

//--------------------------------------------------------------
//class wxGxFeatureDataset
//--------------------------------------------------------------

IMPLEMENT_CLASS(wxGxFeatureDataset, wxGxDataset)

wxGxFeatureDataset::wxGxFeatureDataset(wxGISEnumVectorDatasetType eType, wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxDataset(oParent, soName, soPath)
{
	m_eType = eType;  
}

wxGxFeatureDataset::~wxGxFeatureDataset(void)
{
}

wxString wxGxFeatureDataset::GetCategory(void) const
{
	return wxString(_("Feature class"));
}

wxGISDataset* const wxGxFeatureDataset::GetDatasetFast(void)
{
 	if(m_pwxGISDataset == NULL)
    {
        wxGISFeatureDataset* pDSet = NULL;
        if (m_eType == enumVecMapinfoMif || m_eType == enumVecDXF)
        {
            pDSet = new wxGISFeatureDatasetCached(m_sPath, m_eType);
        }
        else
        {
            pDSet = new wxGISFeatureDataset(m_sPath, m_eType);
        }
        m_pwxGISDataset = wxStaticCast(pDSet, wxGISDataset);
        m_pwxGISDataset->Reference();
    }
    wsGET(m_pwxGISDataset);
}

wxGISDataset* const wxGxFeatureDataset::GetDataset(bool bCache, ITrackCancel* const pTrackCancel)
{
    wxGISFeatureDataset* pwxGISFeatureDataset = wxDynamicCast(GetDatasetFast(), wxGISFeatureDataset);

    if(NULL != pwxGISFeatureDataset && !pwxGISFeatureDataset->IsOpened())
    {
        if(!pwxGISFeatureDataset->Open(0, 0, bCache, pTrackCancel))
        {
            wsDELETE(pwxGISFeatureDataset);
		    const char* err = CPLGetLastErrorMsg();
			wxString sErr = wxString::Format(_("Operation '%s' failed! GDAL error: %s"), _("Open"), wxString(err, wxConvUTF8).c_str());
            wxLogError(sErr);
			if(pTrackCancel)
				pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageErr);
			return NULL;
        }
        wxGIS_GXCATALOG_EVENT(ObjectChanged);
        wsDELETE(pwxGISFeatureDataset);
	}

	wsGET(m_pwxGISDataset);

       // //TODO: Spatial Index
       // if(!pwxGISFeatureDataset->HasSpatialIndex())
       // {
       // 	bool bAskSpaInd = true;
       //     wxString name, ext;
       //     wxFileName::SplitPath(m_sPath, NULL, NULL, &name, &ext);
       //     wxString sFileName = name + wxT(".") + ext;
       //     IGISConfig*  pConfig = m_pCatalog->GetConfig();
       //     bool bCreateSpaInd = true;
       //     if(pConfig)
       //     {
       //         wxXmlNode* pNode = pConfig->GetConfigNode(enumGISHKCU, wxString(wxT("catalog/vector")));
       //         if(pNode)
       //             bAskSpaInd = wxAtoi(pNode->GetAttribute(wxT("create_tree"), wxT("1")));
       //         else
       //         {
       //             pNode = pConfig->CreateConfigNode(enumGISHKCU, wxString(wxT("catalog/vector")), true);
       //             pNode->AddAttribute(wxT("create_tree"), wxT("1"));
       //         }
       //         if(bAskSpaInd)
       //         {
       //             //show ask dialog
       //             wxGISMessageDlg dlg(NULL, wxID_ANY, wxString::Format(_("Create spatial index for %s"), sFileName.c_str()), wxString(_("This vector datasource does not have spatial index. Spatial index allows rapid display at different resolutions.")), wxString(_("Spatial index building may take few moments.\nWould you like to create spatial index?")), wxDefaultPosition, wxSize( 400,160 ));
       //             if(dlg.ShowModal() == wxID_NO)
       //             {
       //                 bCreateSpaInd = false;
       //             }
       //             if(!dlg.GetShowInFuture())
       //             {
       //                 pNode->DeleteAttribute(wxT("create_tree"));
       //                 pNode->AddAttribute(wxT("create_tree"), wxT("0"));
       //             }
       //         }
       //     }

	      //  if(bCreateSpaInd)
	      //  {
       //         wxWindowDisabler disableAll;
       //         wxBusyInfo wait(_("Please wait, working..."));

       //         OGRErr err = pwxGISFeatureDataset->CreateSpatialIndex();
			    //if(err != OGRERR_NONE)
			    //{
       //             const char* pszerr = CPLGetLastErrorMsg();
       //             wxLogError(_("Build Spatial Index failed! OGR error: %s"), wgMB2WX(pszerr));
       //             int nRes = wxMessageBox(_("Build Spatial Index! Continue?"), _("Question"), wxICON_QUESTION | wxYES_NO);
       //             if(nRes == wxNO)
       //                 return NULL;
			    //}
	      //  }
       // }
}


//--------------------------------------------------------------
//class wxGxRasterDataset
//--------------------------------------------------------------

IMPLEMENT_CLASS(wxGxRasterDataset, wxGxDataset)

wxGxRasterDataset::wxGxRasterDataset(wxGISEnumRasterDatasetType eType, wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxDataset(oParent, soName, soPath)
{
	m_eType = eType;
}

wxGxRasterDataset::~wxGxRasterDataset(void)
{
}

wxString wxGxRasterDataset::GetCategory(void) const
{
	return wxString(_("Raster"));
}

wxGISDataset* const wxGxRasterDataset::GetDatasetFast(void)
{
 	if(m_pwxGISDataset == NULL)
    {
        wxGISRasterDataset* pDSet = new wxGISRasterDataset(m_sPath, m_eType);
        m_pwxGISDataset = wxStaticCast(pDSet, wxGISDataset);
        m_pwxGISDataset->Reference();
    }
    m_pwxGISDataset->Reference();
    return m_pwxGISDataset;
}

wxGISDataset* const wxGxRasterDataset::GetDataset(bool bCache, ITrackCancel* const pTrackCancel)
{
    wxGISRasterDataset* pwxGISRasterDataset(NULL);
	pwxGISRasterDataset = wxDynamicCast(GetDatasetFast(), wxGISRasterDataset);

    if(pwxGISRasterDataset && !pwxGISRasterDataset->IsOpened())
    {
        if(!pwxGISRasterDataset->Open(true))
        {
            wsDELETE(pwxGISRasterDataset);
		    const char* err = CPLGetLastErrorMsg();
			wxString sErr = wxString::Format(_("Operation '%s' failed! GDAL error: %s"), _("Open"), wxString(err, wxConvUTF8).c_str());
            wxLogError(sErr);
			if(pTrackCancel)
				pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageErr);
			return NULL;
        }
        wxGIS_GXCATALOG_EVENT(ObjectChanged);
        wsDELETE(pwxGISRasterDataset);
	}

	wsGET(m_pwxGISDataset);
}

