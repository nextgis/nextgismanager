/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  Catalog Main Commands class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2012 Bishop
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

#include "wxgis/remoteserverui/remoteservercmd.h"

#include "wxgis/catalogui/gxselection.h"
#include "wxgis/catalog/gxobject.h"
#include "wxgis/remoteserverui/gxremoteserversui.h"
#include "wxgis/remoteserverui/gxremoteserverui.h"

//#include "wxgis/catalogui/gxcatalogui.h"
#include "wxgis/remoteserverui/serversearchdlg.h"

#include "../../art/remoteserver_16.xpm"
#include "../../art/remoteservers_16.xpm"
#include "../../art/remoteserver_discon.xpm"

//	0	Server connection
//	1	Connect
//	2	Disconnect


IMPLEMENT_DYNAMIC_CLASS(wxGISRemoteCmd, wxObject)

wxGISRemoteCmd::wxGISRemoteCmd(void) : ICommand()
{
}

wxGISRemoteCmd::~wxGISRemoteCmd(void)
{
}

wxIcon wxGISRemoteCmd::GetBitmap(void)
{
	switch(m_subtype)
	{
		case 0:
			if(!m_IconRemServ.IsOk())
				m_IconRemServ = wxIcon(remoteserver_16_xpm);
			return m_IconRemServ;
		case 1:
			if(!m_IconRemServ.IsOk())
				m_IconRemServ = wxIcon(remoteserver_16_xpm);
			return m_IconRemServ;
		case 2:
			if(!m_IconRemServDiscon.IsOk())
				m_IconRemServDiscon = wxIcon(remoteserver_discon_xpm);
			return m_IconRemServDiscon;
		default:
			return wxNullIcon;
	}
}

wxString wxGISRemoteCmd::GetCaption(void)
{
	switch(m_subtype)
	{
		case 0:	
			return wxString(_("Server c&onnection"));
		case 1:	
			return wxString(_("&Connect"));
		case 2:	
			return wxString(_("&Disconnect"));
		default:
			return wxEmptyString;
	}
}

wxString wxGISRemoteCmd::GetCategory(void)
{
	switch(m_subtype)
	{
		case 0:	
		case 1:	
		case 2:	
			return wxString(_("Remote Server"));
		default:
			return wxString(_("[No category]"));
	}
}

bool wxGISRemoteCmd::GetChecked(void)
{
	return false;
}

bool wxGISRemoteCmd::GetEnabled(void)
{
    wxCHECK_MSG(m_pApp, false, wxT("Application pointer is null"));

    wxGxSelection* pSel = m_pApp->GetGxSelection();
    wxGxCatalogBase* pCat = GetGxCatalog();
	switch(m_subtype)
	{
		case 0://server connection
            if(pCat && pSel)
            {
                wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetLastSelectedObjectId());
                return pGxObject != NULL && pGxObject->IsKindOf(wxCLASSINFO(wxGxRemoteServersUI));
            }
		case 1://connect
		case 2://disconnect
            if(pCat && pSel)
            {
                wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetLastSelectedObjectId());
                wxGxRemoteServerUI* pGxRemoteServerUI = wxDynamicCast(pGxObject, wxGxRemoteServerUI);
                if(!pGxRemoteServerUI)
                    return false;
                if(m_subtype == 1)
                    return !pGxRemoteServerUI->IsConnected();
                else if(m_subtype == 2)
                    return pGxRemoteServerUI->IsConnected();
                return false;
            }
		default:
			return false;
	}
}

wxGISEnumCommandKind wxGISRemoteCmd::GetKind(void)
{
	switch(m_subtype)
	{
		case 0://server connection
		case 1://connect
		case 2://disconnect
			return enumGISCommandNormal;
		default:
			return enumGISCommandNormal;
	}
}

wxString wxGISRemoteCmd::GetMessage(void)
{
	switch(m_subtype)
	{
		case 0:	
			return wxString(_("Create connection to remote server"));
		case 1:	
			return wxString(_("Connect to remote server"));
		case 2:	
			return wxString(_("Disconnect from remote server"));
		default:
			return wxEmptyString;
	}
}

void wxGISRemoteCmd::OnClick(void)
{
    wxCHECK_RET(m_pApp, wxT("Application pointer is null"));

    wxGxSelection* pSel = m_pApp->GetGxSelection();
    wxGxCatalogBase* pCat = GetGxCatalog();

	switch(m_subtype)
	{
		case 0:	
			{
                wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetLastSelectedObjectId());
                wxGxRemoteServersUI* pGxRemoteServersUI = wxDynamicCast(pGxObject, wxGxRemoteServersUI);
				if(pGxRemoteServersUI)
				{
					wxWindow* pParentWnd = dynamic_cast<wxWindow*>(m_pApp);
					return pGxRemoteServersUI->CreateConnection(pParentWnd);
				}
			}
			return;
		case 1:	//connect
		case 2:	//disconnect
			{
                wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetLastSelectedObjectId());
                wxGxRemoteServerUI* pGxRemoteServerUI = wxDynamicCast(pGxObject, wxGxRemoteServerUI);
                if(pGxRemoteServerUI)
                {
					if(m_subtype == 1)
						pGxRemoteServerUI->Connect();
					else
						pGxRemoteServerUI->Disconnect();
                }
			}
			return;
		default:
			return;
	}
}

bool wxGISRemoteCmd::OnCreate(wxGISApplicationBase* pApp)
{
    m_pApp = dynamic_cast<wxGxApplication*>(pApp);
	return true;
}

wxString wxGISRemoteCmd::GetTooltip(void)
{
	switch(m_subtype)
	{
		case 0:	
			return wxString(_("Create connection to server"));
		case 1:	
			return wxString(_("Connect to server"));
		case 2:	
			return wxString(_("Disconnect from server"));
		default:
			return wxEmptyString;
	}
}

unsigned char wxGISRemoteCmd::GetCount(void)
{
	return 3;
}

