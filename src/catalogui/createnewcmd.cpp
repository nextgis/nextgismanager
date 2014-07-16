/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Catalog Create New Commands class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
 *   Copyright (C) 2011-2012,2014 Dmitry Baryshnikov
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

#include "../../art/rdb_create.xpm"
#include "../../art/web_conn_create.xpm"
#include "../../art/dbschema_create.xpm"

#ifdef wxGIS_USE_POSTGRES
    #include "wxgis/catalogui/createremotedlgs.h"
#endif // wxGIS_USE_POSTGRES


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
			if(!m_IconCreateRemoteConn.IsOk())
				m_IconCreateRemoteConn = wxIcon(rdb_create_xpm);
			return m_IconCreateRemoteConn;
		case enumGISCatalogCreateNewCmdWebServiceConnection:
			if(!m_IconCreateWebConn.IsOk())
				m_IconCreateWebConn = wxIcon(web_conn_create_xpm);
			return m_IconCreateWebConn;
		case enumGISCatalogCreateNewCmdDBSchema:
			if(!m_IconCreateSchema.IsOk())
                m_IconCreateSchema = wxIcon(dbschema_create_xpm);
            return m_IconCreateSchema;
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
		case enumGISCatalogCreateNewCmdWebServiceConnection:
			return wxString(_("&Web service connection"));
        case enumGISCatalogCreateNewCmdDBSchema:
            return wxString(_("&Database schema"));
        case enumGISCatalogCreateNewCmdDBAndConnection:
            return wxString(_("Rem&ote Database"));
        default:
			return wxEmptyString;
	}
}

wxString wxGISCreateNewCmd::GetCategory(void)
{
	switch(m_subtype)
	{
		case enumGISCatalogCreateNewCmdRemoteDBConnection:
		case enumGISCatalogCreateNewCmdWebServiceConnection:
		case enumGISCatalogCreateNewCmdDBSchema:
        case enumGISCatalogCreateNewCmdDBAndConnection:
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
		case enumGISCatalogCreateNewCmdWebServiceConnection://Create new web connection
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
        default:
			return false;
	}
}

wxGISEnumCommandKind wxGISCreateNewCmd::GetKind(void)
{
	switch(m_subtype)
	{
		case enumGISCatalogCreateNewCmdRemoteDBConnection:
        case enumGISCatalogCreateNewCmdWebServiceConnection:
        case enumGISCatalogCreateNewCmdDBSchema:
        case enumGISCatalogCreateNewCmdDBAndConnection:
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
		case enumGISCatalogCreateNewCmdWebServiceConnection:
			return wxString(_("Create new web service connection"));
		case enumGISCatalogCreateNewCmdDBSchema:
			return wxString(_("Create new database schema"));
		case enumGISCatalogCreateNewCmdDBAndConnection:
            return wxString(_("Create new database"));
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
                    CPLString pszConnName(CheckUniqName(pszConnFolder, wxString(_("new DB connection")), wxString(wxT("xconn"))).mb_str(wxConvUTF8));

                    wxGxView* pGxView = dynamic_cast<wxGxView*>(wxWindow::FindFocus());
                    pGxAR->BeginRenameOnAdd(pGxView, pszConnName);

					wxGISRemoteConnDlg dlg(CPLFormFilename(pszConnFolder, pszConnName, "xconn"), dynamic_cast<wxWindow*>(m_pApp));
					if(dlg.ShowModal() != wxID_OK)
                    {
                        pGxAR->BeginRenameOnAdd(NULL, "");
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
                    CPLString pszConnName(CheckUniqName(pszConnFolder, wxString(_("new DB connection")), wxString(wxT("xconn"))).mb_str(wxConvUTF8));

                    wxGxView* pGxView = dynamic_cast<wxGxView*>(wxWindow::FindFocus());
                    pGxAR->BeginRenameOnAdd(pGxView, pszConnName);

					wxGISCreateDBDlg dlg(CPLFormFilename(pszConnFolder, pszConnName, "xconn"), dynamic_cast<wxWindow*>(m_pApp));
					if(dlg.ShowModal() != wxID_OK)
                    {
                        pGxAR->BeginRenameOnAdd(NULL, "");
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
                    //CPLString pszConnFolder = pGxDBConnectionUI->GetPath();
                    //CPLString pszConnName(CheckUniqName(pszConnFolder, wxString(_("new DB connection")), wxString(wxT("xconn"))).mb_str(wxConvUTF8));

                    //wxGxView* pGxView = dynamic_cast<wxGxView*>(wxWindow::FindFocus());
                    //pGxDBConnectionsUI->BeginRenameOnAdd(pGxView, pszConnName);

                    //wxGISRemoteConnDlg dlg(CPLFormFilename(pszConnFolder, pszConnName, "xconn"), dynamic_cast<wxWindow*>(m_pApp));

                    wxGxAutoRenamer* pGxDBConnectionUIAR = dynamic_cast<wxGxAutoRenamer*>(pGxObject);
                    if (!pGxDBConnectionUIAR)
                        return;

                    wxGxView* pGxView = dynamic_cast<wxGxView*>(wxWindow::FindFocus());

                    wxString sSchemaName = pGxDBConnectionUI->CheckUniqSchemaName(_("new_schema"));
                    pGxDBConnectionUIAR->BeginRenameOnAdd(pGxView, CPLString(CPLFormFilename(pGxDBConnectionUI->GetPath(), sSchemaName.mb_str(wxConvUTF8), "")));
                    if (!pGxDBConnectionUI->CreateSchema(sSchemaName))
                    {
                        wxMessageBox(_("Create schema failed!"), _("Error"), wxICON_ERROR | wxOK);
                        pGxDBConnectionUIAR->BeginRenameOnAdd(NULL, "");
                        return;
                    }
                }
            }
#endif //wxGIS_USE_POSTGRES
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
		case enumGISCatalogCreateNewCmdWebServiceConnection:
			return wxString(_("Create new web service connection"));
		case enumGISCatalogCreateNewCmdDBSchema:
			return wxString(_("Create new database schema"));
		case enumGISCatalogCreateNewCmdDBAndConnection:
			return wxString(_("Create new database"));
        default:
			return wxEmptyString;
	}
}

unsigned char wxGISCreateNewCmd::GetCount(void)
{
	return enumGISCatalogCreateNewCmdMax;
}
