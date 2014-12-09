/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Catalog Create New Commands class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
 *   Copyright (C) 2011-2012,2014 Dmitry Baryshnikov
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
#include "wxgis/catalogui/createnewcmd.h"

#include "wxgis/catalogui/gxcatalogui.h"
#include "wxgis/catalogui/remoteconndlgs.h"
#include "wxgis/datasource/sysop.h"
#include "wxgis/catalogui/gxselection.h"
#include "wxgis/catalogui/gxdbconnectionsui.h"
#include "wxgis/catalogui/gxremoteconnui.h"
#include "wxgis/catalogui/gxngwconnui.h"
#include "wxgis/catalogui/createremotedlgs.h"
#include "wxgis/framework/icon.h"

#include "../../art/rdb_conn_16.xpm"
#include "../../art/dbschema_16.xpm"
#include "../../art/folder_arch_16.xpm"
#include "../../art/web_conn_create.xpm"


IMPLEMENT_DYNAMIC_CLASS(wxGISCreateNewCmd, wxObject)

wxGISCreateNewCmd::wxGISCreateNewCmd(void)
{
}

wxGISCreateNewCmd::~wxGISCreateNewCmd(void)
{
}

wxIcon wxGISCreateNewCmd::GetBitmap(void)
{
	switch(m_subtype)
	{
        case enumGISCatalogCreateNewCmdDBAndConnection:
		case enumGISCatalogCreateNewCmdRemoteDBConnection:
		case enumGISCatalogCreateNewCmdNGWPostGISConnection:
			if(!m_IconCreateRemoteConn.IsOk())
                m_IconCreateRemoteConn = GetStateIcon(rdb_conn_16_xpm, wxGISEnumIconStateNew, false);
			return m_IconCreateRemoteConn;
		case enumGISCatalogCreateNewCmdTMSConnection:
        case enumGISCatalogCreateNewCmdNGWConnection:
			if(!m_IconCreateWebConn.IsOk())
				m_IconCreateWebConn = wxIcon(web_conn_create_xpm);
			return m_IconCreateWebConn;
		case enumGISCatalogCreateNewCmdDBSchema:
			if(!m_IconCreateSchema.IsOk())
                m_IconCreateSchema = GetStateIcon(dbschema_16_xpm, wxGISEnumIconStateNew, false);
            return m_IconCreateSchema;
		case enumGISCatalogCreateNewCmdNGWResourceGroup:	
			if(!m_IconCreateResourceGroup.IsOk())
                m_IconCreateResourceGroup = GetStateIcon(folder_arch_16_xpm, wxGISEnumIconStateNew, false);
            return m_IconCreateResourceGroup;
        default:
			return wxNullIcon;
	}
}

wxString wxGISCreateNewCmd::GetCaption(void)
{
	switch(m_subtype)
	{
		case enumGISCatalogCreateNewCmdRemoteDBConnection:
			return wxString(_("&Remote database connection"));
		case enumGISCatalogCreateNewCmdTMSConnection:
			return wxString(_("&TMS connection"));
        case enumGISCatalogCreateNewCmdNGWConnection:
			return wxString(_("&NGW connection"));
        case enumGISCatalogCreateNewCmdDBSchema:
            return wxString(_("&Database schema"));
        case enumGISCatalogCreateNewCmdDBAndConnection:
            return wxString(_("Rem&ote Database"));
		case enumGISCatalogCreateNewCmdNGWResourceGroup:	
			return wxString(_("NGW Resource Group"));
		case enumGISCatalogCreateNewCmdNGWPostGISConnection:
			return wxString(_("PostGIS Connection"));
        default:
			return wxEmptyString;
	}
}

wxString wxGISCreateNewCmd::GetCategory(void)
{
	switch(m_subtype)
	{
		case enumGISCatalogCreateNewCmdRemoteDBConnection:
		case enumGISCatalogCreateNewCmdTMSConnection:
		case enumGISCatalogCreateNewCmdNGWConnection:
		case enumGISCatalogCreateNewCmdDBSchema:
        case enumGISCatalogCreateNewCmdDBAndConnection:
		case enumGISCatalogCreateNewCmdNGWResourceGroup:
        case enumGISCatalogCreateNewCmdNGWPostGISConnection:
            return wxString(_("Create"));
		default:
			return NO_CATEGORY;
	}
}

bool wxGISCreateNewCmd::GetChecked(void)
{
	return false;
}

bool wxGISCreateNewCmd::GetEnabled(void)
{
    wxCHECK_MSG(m_pGxApp && m_pApp, false, wxT("Application pointer is null"));

    wxGxSelection* pSel = m_pGxApp->GetGxSelection();
    wxGxCatalogBase* pCat = GetGxCatalog();

    switch(m_subtype)
	{
        case enumGISCatalogCreateNewCmdDBAndConnection:
        case enumGISCatalogCreateNewCmdRemoteDBConnection:
            if(pCat && pSel)
            {
                wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetFirstSelectedObjectId());
                wxGxObjectContainer* pGxObjectContainer = wxDynamicCast(pGxObject, wxGxObjectContainer);
                if(pGxObjectContainer && pGxObjectContainer->CanCreate(enumGISContainer, enumContRemoteDBConnection))
                {
                    return true;
                }
            }
            return false;
		case enumGISCatalogCreateNewCmdTMSConnection://Create new web connection
        case enumGISCatalogCreateNewCmdNGWConnection://Create new web connection
           if(pCat && pSel)
            {
                wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetFirstSelectedObjectId());
                wxGxObjectContainer* pGxObjectContainer = wxDynamicCast(pGxObject, wxGxObjectContainer);
                if(pGxObjectContainer && pGxObjectContainer->CanCreate(enumGISContainer, enumContWebServiceConnection))
                {
                    return true;
                }
            }
            return false;
		case enumGISCatalogCreateNewCmdDBSchema://Create database schema
            if(pCat && pSel)
            {
                wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetFirstSelectedObjectId());
                wxGxObjectContainer* pGxObjectContainer = wxDynamicCast(pGxObject, wxGxObjectContainer);
                if (pGxObjectContainer && pGxObjectContainer->CanCreate(enumGISContainer, enumContGDBFolder))
                {
                    return true;
                }
            }
            return false;
		case enumGISCatalogCreateNewCmdNGWResourceGroup:
		case enumGISCatalogCreateNewCmdNGWPostGISConnection:
            if(pCat && pSel)
            {
                wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetFirstSelectedObjectId());
                wxGxObjectContainer* pGxObjectContainer = wxDynamicCast(pGxObject, wxGxObjectContainer);
                if (pGxObjectContainer)
                {
                    return pGxObjectContainer->CanCreate(enumGISContainer, enumContNGWResourceGroup);
                }
            }
            return false;			
        default:
			return false;
	}
}

wxGISEnumCommandKind wxGISCreateNewCmd::GetKind(void)
{
	switch(m_subtype)
	{
		case enumGISCatalogCreateNewCmdRemoteDBConnection:
        case enumGISCatalogCreateNewCmdTMSConnection:
        case enumGISCatalogCreateNewCmdNGWConnection:
        case enumGISCatalogCreateNewCmdDBSchema:
        case enumGISCatalogCreateNewCmdDBAndConnection:
		case enumGISCatalogCreateNewCmdNGWResourceGroup:
		case enumGISCatalogCreateNewCmdNGWPostGISConnection:
		default:
			return enumGISCommandNormal;
	}
}

wxString wxGISCreateNewCmd::GetMessage(void)
{
	switch(m_subtype)
	{
		case enumGISCatalogCreateNewCmdRemoteDBConnection:
			return wxString(_("Create new remote DB connection"));
		case enumGISCatalogCreateNewCmdTMSConnection:
			return wxString(_("Create new TMS connection"));
        case enumGISCatalogCreateNewCmdNGWConnection:
			return wxString(_("Create new NextGIS Web connection"));
        case enumGISCatalogCreateNewCmdDBSchema:
			return wxString(_("Create new database schema"));
		case enumGISCatalogCreateNewCmdDBAndConnection:
            return wxString(_("Create new database"));
		case enumGISCatalogCreateNewCmdNGWResourceGroup:
            return wxString(_("Create new resource group"));	
		case enumGISCatalogCreateNewCmdNGWPostGISConnection:			
            return wxString(_("Create new PostGIS Connection"));
		default:
			return wxEmptyString;
	}
}

void wxGISCreateNewCmd::OnClick(void)
{
    wxCHECK_RET(m_pGxApp && m_pApp, wxT("Application pointer is null"));

    wxGxSelection* pSel = m_pGxApp->GetGxSelection();
    wxGxCatalogBase* pCat = GetGxCatalog();

	switch(m_subtype)
	{
		case enumGISCatalogCreateNewCmdRemoteDBConnection:
#ifdef wxGIS_USE_POSTGRES
            if(pCat && pSel)
            {
                wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetFirstSelectedObjectId());
                wxGxAutoRenamer* pGxAR = dynamic_cast<wxGxAutoRenamer*>(pGxObject);
                if (NULL != pGxAR && pGxObject->IsKindOf(wxCLASSINFO(wxGxFolder)))
                {
                    CPLString pszConnFolder = pGxObject->GetPath();
                    wxString sConnName = CheckUniqName(pszConnFolder, wxString(_("new DB connection")), wxString(wxT("xconn")));
                    wxString sConnPath = pGxObject->GetFullName() + wxFileName::GetPathSeparator() + sConnName + wxString(wxT(".xconn"));


                    wxGxView* pGxView = dynamic_cast<wxGxView*>(wxWindow::FindFocus());
                    pGxAR->BeginRenameOnAdd(pGxView, sConnPath);

                    wxGISRemoteDBConnDlg dlg(CPLFormFilename(pszConnFolder, sConnName.ToUTF8(), "xconn"), dynamic_cast<wxWindow*>(m_pApp));
					if(dlg.ShowModal() != wxID_OK)
                    {
                        pGxAR->BeginRenameOnAdd(NULL);
                    }
                }
            }
#endif //wxGIS_USE_POSTGRES
            break;
		case enumGISCatalogCreateNewCmdDBAndConnection:
#ifdef wxGIS_USE_POSTGRES
            if(pCat && pSel)
            {
                wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetFirstSelectedObjectId());
                wxGxAutoRenamer* pGxAR = dynamic_cast<wxGxAutoRenamer*>(pGxObject);
                if (NULL != pGxAR && pGxObject->IsKindOf(wxCLASSINFO(wxGxFolder)))
                {
                    CPLString pszConnFolder = pGxObject->GetPath();
                    wxString sConnName = CheckUniqName(pszConnFolder, wxString(_("new DB connection")), wxString(wxT("xconn")));
                    wxString sConnPath = pGxObject->GetFullName() + wxFileName::GetPathSeparator() + sConnName + wxString(wxT(".xconn"));

                    wxGxView* pGxView = dynamic_cast<wxGxView*>(wxWindow::FindFocus());
                    pGxAR->BeginRenameOnAdd(pGxView, sConnPath);

                    wxGISCreateDBDlg dlg(CPLFormFilename(pszConnFolder, sConnName.ToUTF8(), "xconn"), dynamic_cast<wxWindow*>(m_pApp));
					if(dlg.ShowModal() != wxID_OK)
                    {
                        pGxAR->BeginRenameOnAdd(NULL);
                    }
                }
            }
#endif //wxGIS_USE_POSTGRES
            break;
        case enumGISCatalogCreateNewCmdDBSchema:
#ifdef wxGIS_USE_POSTGRES
            if (pCat && pSel)
            {
                wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetFirstSelectedObjectId());
                wxGxRemoteConnectionUI* pGxDBConnectionUI = wxDynamicCast(pGxObject, wxGxRemoteConnectionUI);
                if (pGxDBConnectionUI)
                {
                    wxGxAutoRenamer* pGxDBConnectionUIAR = dynamic_cast<wxGxAutoRenamer*>(pGxObject);
                    if (!pGxDBConnectionUIAR)
                        return;

                    wxGxView* pGxView = dynamic_cast<wxGxView*>(wxWindow::FindFocus());

                    wxString sSchemaName = pGxDBConnectionUI->CheckUniqSchemaName(_("new_schema"));
                    wxString sSchemaPath = pGxObject->GetFullName() + wxFileName::GetPathSeparator() + sSchemaName;

                    pGxDBConnectionUIAR->BeginRenameOnAdd(pGxView, sSchemaPath);
                    if (!pGxDBConnectionUI->CreateSchema(sSchemaName))
                    {
                        wxGISErrorMessageBox(_("Create schema failed!"), wxString::FromUTF8(CPLGetLastErrorMsg()));
                        pGxDBConnectionUIAR->BeginRenameOnAdd(NULL);
                        return;
                    }
                }
            }
#endif //wxGIS_USE_POSTGRES
            break;
        case enumGISCatalogCreateNewCmdTMSConnection:
#ifdef wxGIS_USE_CURL
			if(pCat && pSel)
            {
                wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetFirstSelectedObjectId());
                wxGxAutoRenamer* pGxAR = dynamic_cast<wxGxAutoRenamer*>(pGxObject);
                if (NULL != pGxAR && pGxObject->IsKindOf(wxCLASSINFO(wxGxFolder)))
                {
                    CPLString pszConnFolder = pGxObject->GetPath();
                    wxString sConnName = CheckUniqName(pszConnFolder, wxString(_("new TMS connection")), wxString(wxT("wconn")));
                    wxString sConnPath = pGxObject->GetFullName() + wxFileName::GetPathSeparator() + sConnName + wxString(wxT(".wconn"));

                    wxGxView* pGxView = dynamic_cast<wxGxView*>(wxWindow::FindFocus());
                    pGxAR->BeginRenameOnAdd(pGxView, sConnPath);

                    wxGISTMSConnDlg dlg(CPLFormFilename(pszConnFolder, sConnName.ToUTF8(), "wconn"), dynamic_cast<wxWindow*>(m_pApp));
					if(dlg.ShowModal() != wxID_OK)
                    {
                        pGxAR->BeginRenameOnAdd(NULL);
                    }
                }
            }
#endif // wxGIS_USE_CURL
            break;
        case enumGISCatalogCreateNewCmdNGWConnection:
#ifdef wxGIS_USE_CURL
			if(pCat && pSel)
            {
                wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetFirstSelectedObjectId());
                wxGxAutoRenamer* pGxAR = dynamic_cast<wxGxAutoRenamer*>(pGxObject);
                if (NULL != pGxAR && pGxObject->IsKindOf(wxCLASSINFO(wxGxFolder)))
                {
                    CPLString pszConnFolder = pGxObject->GetPath();
                    wxString sConnName = CheckUniqName(pszConnFolder, wxString(_("new NGW connection")), wxString(wxT("wconn")));
                    wxString sConnPath = pGxObject->GetFullName() + wxFileName::GetPathSeparator() + sConnName + wxString(wxT(".wconn"));

                    wxGxView* pGxView = dynamic_cast<wxGxView*>(wxWindow::FindFocus());
                    pGxAR->BeginRenameOnAdd(pGxView, sConnPath);

                    wxGISNGWConnDlg dlg(CPLFormFilename(pszConnFolder, sConnName.ToUTF8(), "wconn"), dynamic_cast<wxWindow*>(m_pApp));
					if(dlg.ShowModal() != wxID_OK)
                    {
                        pGxAR->BeginRenameOnAdd(NULL);
                    }
                }
            }
#endif // wxGIS_USE_CURL
            break;
		case enumGISCatalogCreateNewCmdNGWResourceGroup:
#ifdef wxGIS_USE_CURL
			if(pCat && pSel)
            {
                wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetFirstSelectedObjectId());
                wxGxNGWResourceGroupUI* pGxNGWResourceGroupUI = wxDynamicCast(pGxObject, wxGxNGWResourceGroupUI);
                if (pGxNGWResourceGroupUI)
                {
                    wxGxAutoRenamer* pGxNGWResourceGroupUIAR = dynamic_cast<wxGxAutoRenamer*>(pGxObject);
                    if (!pGxNGWResourceGroupUIAR)
                        return;

                    wxGxView* pGxView = dynamic_cast<wxGxView*>(wxWindow::FindFocus());

                    wxString sGroupName = pGxNGWResourceGroupUI->CheckUniqName(_("new group"));
                    wxString sFullGroupName = pGxNGWResourceGroupUI->GetFullName() + wxFileName::GetPathSeparator() + sGroupName;
                    pGxNGWResourceGroupUIAR->BeginRenameOnAdd(pGxView, sFullGroupName);
					CPLErrorReset();
                    if (!pGxNGWResourceGroupUI->CreateResource(sGroupName, enumNGWResourceTypeResourceGroup))
                    {						
						wxString sErrMsg = wxString::Format(_("Create '%s' failed"), sGroupName.c_str());
						wxGISErrorMessageBox(sErrMsg, wxString::FromUTF8(CPLGetLastErrorMsg()));
						
						pGxNGWResourceGroupUIAR->BeginRenameOnAdd(NULL);
                        return;
                    }
                }
            }
#endif // wxGIS_USE_CURL		
			break;
		case enumGISCatalogCreateNewCmdNGWPostGISConnection:
#ifdef wxGIS_USE_CURL
            if(pCat && pSel)
            {
                wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetFirstSelectedObjectId());
                wxGxNGWResourceGroupUI* pGxNGWResourceGroupUI = wxDynamicCast(pGxObject, wxGxNGWResourceGroupUI);
                if (pGxNGWResourceGroupUI)
				{
                    CPLString pszConnFolder = pGxObject->GetPath();
                    CPLString pszConnName(CheckUniqName(pszConnFolder, wxString(_("PostGIS Connection")), "").ToUTF8());

					wxGISRemoteDBConnDlg dlg(wxString::FromUTF8(pszConnName), wxT("localhost"), wxT("postgres"), wxT(""), wxT(""), dynamic_cast<wxWindow*>(m_pApp));
					if(dlg.ShowModal() == wxID_OK)
                    {
                        //do the work
						CPLErrorReset();
						
						wxString sName = dlg.GetName();
						wxString sUser = dlg.GetUser();
						wxString sPass = dlg.GetPassword();
						wxString sHost = dlg.GetHost();
						wxString sDB = dlg.GetDatabase();						
						
						if (!pGxNGWResourceGroupUI->CreatePostGISConnection(sName, sHost, sDB, sUser, sPass))
						{						
							wxString sErrMsg = wxString::Format(_("Create '%s' failed"), sName.c_str());
							wxGISErrorMessageBox(sErrMsg, wxString::FromUTF8(CPLGetLastErrorMsg()));
							
							return;
						}
					}
                }
            }			
#endif // wxGIS_USE_CURL		
			break;	
        default:
			return;
	}
}

bool wxGISCreateNewCmd::OnCreate(wxGISApplicationBase* pApp)
{
	m_pApp = pApp;
    m_pGxApp = dynamic_cast<wxGxApplicationBase*>(pApp);
	return true;
}

wxString wxGISCreateNewCmd::GetTooltip(void)
{
	switch(m_subtype)
	{
		case enumGISCatalogCreateNewCmdRemoteDBConnection:
			return wxString(_("Create new remote DB connection"));
		case enumGISCatalogCreateNewCmdTMSConnection:
			return wxString(_("Create new TMS connection"));
		case enumGISCatalogCreateNewCmdNGWConnection:
			return wxString(_("Create new NGW connection"));
		case enumGISCatalogCreateNewCmdDBSchema:
			return wxString(_("Create new database schema"));
		case enumGISCatalogCreateNewCmdDBAndConnection:
			return wxString(_("Create new database"));
		case enumGISCatalogCreateNewCmdNGWResourceGroup:
			return wxString(_("Create new resource group"));
		case enumGISCatalogCreateNewCmdNGWPostGISConnection	:
			return wxString(_("Create new PostGIS Connection"));
        default:
			return wxEmptyString;
	}
}

unsigned char wxGISCreateNewCmd::GetCount(void)
{
	return enumGISCatalogCreateNewCmdMax;
}
