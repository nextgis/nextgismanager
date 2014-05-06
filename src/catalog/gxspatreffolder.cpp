/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxSpatialReferencesFolder class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011 Bishop
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
#include "wxgis/catalog/gxspatreffolder.h"

#include "cpl_vsi_virtual.h"

#include <wx/stdpaths.h>

//-----------------------------------------------------------------------------------
// wxGxSpatialReferencesFolder
//-----------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGxSpatialReferencesFolder, wxGxPrjFolder)

wxGxSpatialReferencesFolder::wxGxSpatialReferencesFolder(void) : wxGxPrjFolder()
{
}

wxGxSpatialReferencesFolder::~wxGxSpatialReferencesFolder(void)
{
}

bool wxGxSpatialReferencesFolder::Create(wxGxObject *oParent, const wxString &soName, const CPLString &soPath)
{
    if (!wxGxPrjFolder::Create(oParent, GetName(), soPath))
    {
        wxLogError(_("wxGxSpatialReferencesFolder::Create failed. GxObject %s"), GetName().c_str());
        return false;
    }

    m_pCatalog = wxDynamicCast(GetGxCatalog(), wxGxCatalog);
    return true;
}


void wxGxSpatialReferencesFolder::Init(wxXmlNode* const pConfigNode)
{
    m_sInternalPath = pConfigNode->GetAttribute(wxT("path"), NON);
    if(m_sInternalPath.IsEmpty() || m_sInternalPath == wxString(NON))
    {
        //example: /vsizip/c:/wxGIS/sys/cs.zip/cs
        wxGISAppConfig oConfig = GetConfig();
        if (oConfig.IsOk())
        {
            m_sInternalPath = wxT("/vsizip/") + oConfig.GetSysDir() + wxT("/cs.zip/cs");
        }
        else
        {
            wxString sExeDirPath = wxPathOnly(wxStandardPaths::Get().GetExecutablePath());
            m_sInternalPath = wxT("/vsizip/") + sExeDirPath + wxT("/sys/cs.zip/cs");
        }
    }

    m_sInternalPath.Replace(wxT("\\"), wxT("/"));
    wxLogMessage(_("wxGxSpatialReferencesFolder: The path is set to '%s'"), m_sInternalPath.c_str());
    CPLSetConfigOption("wxGxSpatialReferencesFolder", m_sInternalPath.mb_str(wxConvUTF8));

    m_sPath = CPLString(m_sInternalPath.ToUTF8());

    m_bIsArchive = m_sInternalPath.Find(wxT("/vsizip/")) != wxNOT_FOUND;
}

void wxGxSpatialReferencesFolder::Serialize(wxXmlNode* pConfigNode)
{
    pConfigNode->AddAttribute(wxT("path"), m_sInternalPath);
}

bool wxGxSpatialReferencesFolder::IsArchive(void) const
{
    return m_bIsArchive;
}

//-----------------------------------------------------------------------------------
// wxGxPrjFolder
//-----------------------------------------------------------------------------------
IMPLEMENT_CLASS(wxGxPrjFolder, wxGxArchiveFolder)

wxGxPrjFolder::wxGxPrjFolder() : wxGxArchiveFolder()
{
}

wxGxPrjFolder::wxGxPrjFolder(wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxArchiveFolder(oParent, soName, soPath)
{
}

wxGxPrjFolder::~wxGxPrjFolder(void)
{
}

wxGxObject* wxGxPrjFolder::GetArchiveFolder(wxGxObject *oParent, const wxString &soName, const CPLString &soPath)
{
    wxGxPrjFolder* pFolder = new wxGxPrjFolder(oParent, soName, soPath);
    return wxStaticCast(pFolder, wxGxObject);
}


/*
                //cast ctrl

    //            wxWindow* pWnd = dynamic_cast<wxWindow*>(m_pApp);
    //            wxGxObjectDialog dlg(pWnd, wxID_ANY, _("Select projection"));
				//dlg.SetAllowMultiSelect(false);
				//dlg.AddFilter(new wxGxPrjFileFilter(), true);
				//dlg.SetButtonCaption(_("Select"));
				//dlg.SetStartingLocation(_("Coordinate Systems"));
    //            if(dlg.ShowModalOpen() == wxID_OK)
    //            {
    //                GxObjectArray* pArr = dlg.GetSelectedObjects();
    //                if(!pArr)
    //                    return;
    //                if(pArr->size() < 0)
    //                     return;
				////	for(size_t i = 0; i < pArr->size(); ++i)
				////	{
				////		wxGxPrjFile* pGxPrjFile = dynamic_cast<wxGxPrjFile*>(pArr->at(i));
				////		if(!pGxPrjFile)
				////			return;
				////		OGRSpatialReference* pRef = pGxPrjFile->GetSpatialReference();
				////		if(pRef)
				////		{
				////			wxString sProjDir = wxString(wxT("d:\\temp\\srs\\Projected Coordinate Systems"));
				////			if(!wxDirExists(sProjDir))
				////				wxFileName::Mkdir(sProjDir, 0755, wxPATH_MKDIR_FULL);
				////			wxString sGeogDir = wxString(wxT("d:\\temp\\srs\\Geographic Coordinate Systems"));
				////			if(!wxDirExists(sGeogDir))
				////				wxFileName::Mkdir(sGeogDir, 0755, wxPATH_MKDIR_FULL);
				////			wxString sLoclDir = wxString(wxT("d:\\temp\\srs\\Vertical Coordinate Systems"));
				////			if(!wxDirExists(sLoclDir))
				////				wxFileName::Mkdir(sLoclDir, 0755, wxPATH_MKDIR_FULL);

				////			const char *pszProjection = pRef->GetAttrValue("PROJECTION");
				////			wxString sProjection;
				////			if(pszProjection)
				////				sProjection = wgMB2WX(pszProjection);
				////			if(pRef->IsProjected())
				////			{
				////				const char *pszProjcs = pRef->GetAttrValue("PROJCS");
				////				wxString sName = wgMB2WX(pszProjcs);
				////				wxString sFileName;
				////				int pos = sName.Find('/');
				////				if(pos != wxNOT_FOUND)
				////				{
				////					wxString sSubFldr = sName.Right(sName.Len() - pos - 1);
				////					sSubFldr.Trim(true); sSubFldr.Trim(false);
				////					wxString sStorePath = sProjDir + wxFileName::GetPathSeparator() + sSubFldr;
				////					if(!wxDirExists(sStorePath))
				////						wxFileName::Mkdir(sStorePath, 0755, wxPATH_MKDIR_FULL);

				////					sName.Replace(wxString(wxT("/")), wxString(wxT("")));
				////					sName.Replace(wxString(wxT("  ")), wxString(wxT(" ")));
				////					sFileName = sStorePath + wxFileName::GetPathSeparator() + sName + wxT(".spr");
				////				}
				////				else
				////				{
				////					sFileName = sProjDir + wxFileName::GetPathSeparator() + sName + wxT(".spr");
				////				}
				////				FILE *fp = VSIFOpenL( wgWX2MB(sFileName), "w");
				////				if( fp != NULL )
				////				{
				////					char* pData(NULL);
				////					pRef->exportToWkt(&pData);
				////					VSIFWriteL( pData, 1, strlen(pData), fp );
				////					CPLFree(pData);
				////					VSIFCloseL(fp);
				////				}
				////			}
				////			else if(pRef->IsGeographic())
				////			{
				////				const char *pszProjcs = pRef->GetAttrValue("GEOGCS");
				////				wxString sName = wgMB2WX(pszProjcs);
				////				if(sName.Find(wxT("depre")) != wxNOT_FOUND)
				////					continue;
				////				wxString sFileName;
				////				int pos = sName.Find('/');
				////				if(pos != wxNOT_FOUND)
				////				{
				////					wxString sSubFldr = sName.Right(sName.Len() - pos - 1);
				////					sSubFldr.Trim(true); sSubFldr.Trim(false);
				////					wxString sStorePath = sGeogDir + wxFileName::GetPathSeparator() + sSubFldr;
				////					if(!wxDirExists(sStorePath))
				////						wxFileName::Mkdir(sStorePath, 0755, wxPATH_MKDIR_FULL);

				////					sName.Replace(wxString(wxT("/")), wxString(wxT("")));
				////					sName.Replace(wxString(wxT("  ")), wxString(wxT(" ")));
				////					sFileName = sStorePath + wxFileName::GetPathSeparator() + sName + wxT(".spr");
				////				}
				////				else
				////				{
				////					sFileName = sGeogDir + wxFileName::GetPathSeparator() + sName + wxT(".spr");
				////				}
				////				FILE *fp = VSIFOpenL( wgWX2MB(sFileName), "w");
				////				if( fp != NULL )
				////				{
				////					char* pData(NULL);
				////					pRef->exportToWkt(&pData);
				////					VSIFWriteL( pData, 1, strlen(pData), fp );
				////					CPLFree(pData);
				////					VSIFCloseL(fp);
				////				}
				////			}
				////		}

				////		//const char *pszProjcs = pRef->GetAttrValue("PROJCS");
				////		//wxString sName = wgMB2WX(pszProjcs);
				////		//wxString sFileName = sStorePath + wxFileName::GetPathSeparator() + sName + wxT(".spr");
				////		//FILE *fp = VSIFOpenL( wgWX2MB(sFileName), "w");
				////		//if( fp != NULL )
				////		//{
				////		//	char* pData(NULL);
				////		//	SpaRef.exportToWkt(&pData);
				////		//	VSIFWriteL( pData, 1, strlen(pData), fp );

				////		//	CPLFree(pData);
				////		//	VSIFCloseL(fp);
				////		//}
				////	}

				//	wxGISMapView* pMapView(NULL);
				//	WINDOWARRAY* pWinArr = m_pApp->GetChildWindows();
				//	if(pWinArr)
				//	{
				//		for(size_t i = 0; i < pWinArr->size(); ++i)
				//		{
				//			pMapView = dynamic_cast<wxGISMapView*>(pWinArr->at(i));
				//			if(pMapView)
				//				break;
				//		}
				//	}
				//	if(pMapView)
    //                {
				//		wxGxPrjFile* pGxPrjFile = dynamic_cast<wxGxPrjFile*>(pArr->at(0));
				//		if(!pGxPrjFile)
				//			return;
    //                    OGRSpatialReference* pRef = pGxPrjFile->GetSpatialReference();
    //                    if(pRef)
    //                    {
    //                        pMapView->SetSpatialReference(pRef);
    //                        pMapView->SetFullExtent();
    //                    }
    //                }
    //            }


    //    //        wxString sProjDir = wxString(wxT("d:\\temp\\srs\\Projected Coordinate Systems"));
    //    //        if(!wxDirExists(sProjDir))
		  //    //      wxFileName::Mkdir(sProjDir, 0755, wxPATH_MKDIR_FULL);
    //    //        wxString sGeogDir = wxString(wxT("d:\\temp\\srs\\Geographic Coordinate Systems"));
    //    //        if(!wxDirExists(sGeogDir))
		  //    //      wxFileName::Mkdir(sGeogDir, 0755, wxPATH_MKDIR_FULL);
    //    //        wxString sLoclDir = wxString(wxT("d:\\temp\\srs\\Vertical Coordinate Systems"));
    //    //        if(!wxDirExists(sLoclDir))
		  //    //      wxFileName::Mkdir(sLoclDir, 0755, wxPATH_MKDIR_FULL);

    //    //        IStatusBar* pStatusBar = m_pApp->GetStatusBar();
    //    //        wxGISProgressor* pProgressor = dynamic_cast<wxGISProgressor*>(pStatusBar->GetProgressor());
    //    //        if(pProgressor)
    //    //        {
    //    //            pProgressor->Show(true);
    //    //            pProgressor->SetRange(70000);

    //    //            wxString sDirPath;

    //    //            for(size_t i = 2213; i < 2214; ++i)
    //    //            {
    //    //                OGRSpatialReference SpaRef;
    //    //                OGRErr err = SpaRef.importFromEPSG(i);
    //    //                if(err == OGRERR_NONE)
    //    //                {
    //    //                   const char *pszProjection = SpaRef.GetAttrValue("PROJECTION");
    //    //                   wxString sProjection;
    //    //                   if(pszProjection)
    //    //                       sProjection = wgMB2WX(pszProjection);
    //    //                   if(SpaRef.IsProjected())
    //    //                    {
    //    //                        const char *pszProjcs = SpaRef.GetAttrValue("PROJCS");
    //    //                        wxString sName = wgMB2WX(pszProjcs);
    //    //                        if(sName.Find(wxT("depre")) != wxNOT_FOUND)
    //    //                            continue;
    //    //                        wxString sFileName;
    //    //                        int pos = sName.Find('/');
    //    //                        if(pos != wxNOT_FOUND)
    //    //                        {
    //    //                            wxString sSubFldr = sName.Right(sName.Len() - pos - 1);
    //    //                            sSubFldr.Trim(true); sSubFldr.Trim(false);
    //    //                            wxString sStorePath = sProjDir + wxFileName::GetPathSeparator() + sSubFldr;
    //    //                            if(!wxDirExists(sStorePath))
		  //    //                          wxFileName::Mkdir(sStorePath, 0755, wxPATH_MKDIR_FULL);

    //    //                            sName.Replace(wxString(wxT("/")), wxString(wxT("")));
    //    //                            sName.Replace(wxString(wxT("  ")), wxString(wxT(" ")));
    //    //                            sFileName = sStorePath + wxFileName::GetPathSeparator() + sName + wxT(".spr");
    //    //                        }
    //    //                        else
    //    //                        {
    //    //                            sFileName = sProjDir + wxFileName::GetPathSeparator() + sName + wxT(".spr");
    //    //                        }
				//				//FILE *fp = VSIFOpenL( wgWX2MB(sFileName), "w");
				//				//if( fp != NULL )
    //    //                        //wxFile file;
    //    //                        //if(file.Create(sFileName))
    //    //                        {
    //    //                            char* pData(NULL);
    //    //                            SpaRef.exportToWkt(&pData);
    //    //                            //wxString Data = wgMB2WX(pData);
    //    //                            //file.Write(Data);
				//				//    VSIFWriteL( pData, 1, strlen(pData), fp );

    //    //                            CPLFree(pData);
				//				//	VSIFCloseL(fp);
    //    //                        }
    //    //                    }
    //    //                    else if(SpaRef.IsGeographic())
    //    //                    {
    //    //                        const char *pszProjcs = SpaRef.GetAttrValue("GEOGCS");
    //    //                        wxString sName = wgMB2WX(pszProjcs);
    //    //                        if(sName.Find(wxT("depre")) != wxNOT_FOUND)
    //    //                            continue;
    //    //                        wxString sFileName;
    //    //                        int pos = sName.Find('/');
    //    //                        if(pos != wxNOT_FOUND)
    //    //                        {
    //    //                            wxString sSubFldr = sName.Right(sName.Len() - pos - 1);
    //    //                            sSubFldr.Trim(true); sSubFldr.Trim(false);
    //    //                            wxString sStorePath = sGeogDir + wxFileName::GetPathSeparator() + sSubFldr;
    //    //                            if(!wxDirExists(sStorePath))
		  //    //                          wxFileName::Mkdir(sStorePath, 0755, wxPATH_MKDIR_FULL);

    //    //                            sName.Replace(wxString(wxT("/")), wxString(wxT("")));
    //    //                            sName.Replace(wxString(wxT("  ")), wxString(wxT(" ")));
    //    //                            sFileName = sStorePath + wxFileName::GetPathSeparator() + sName + wxT(".spr");
    //    //                        }
    //    //                        else
    //    //                        {
    //    //                            sFileName = sGeogDir + wxFileName::GetPathSeparator() + sName + wxT(".spr");
    //    //                        }
    //    //                        //wxFile file;
    //    //                        //if(file.Create(sFileName))
				//				//FILE *fp = VSIFOpenL( wgWX2MB(sFileName), "w");
				//				//if( fp != NULL )
    //    //                        {
    //    //                            char* pData(NULL);
    //    //                            SpaRef.exportToWkt(&pData);
    //    //                            //wxString Data = wgMB2WX(pData);
    //    //                            //file.Write(Data);
				//				//    VSIFWriteL( pData, 1, strlen(pData), fp );
    //    //                            CPLFree(pData);
				//				//	VSIFCloseL(fp);
    //    //                        }
    //    //                    }
    //    //                    else
    //    //                    {
    //    //                        sDirPath = wxString(wxT("d:\\temp\\srs\\Vertical Coordinate Systems"));
    //    //                    //bool bLoc = SpaRef.IsLocal();
    //    //                    }
    //    //                }
    //    //                pProgressor->SetValue(i);
    //    //            }
    //    //            pProgressor->Show(false);
    //    //        }
    //    //        pStatusBar->SetMessage(_("Done"));


    */