/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISNewMenu class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011,2012,2014 Bishop
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
#include "wxgis/catalogui/newmenu.h"

//#include "wxgis/catalogui/gxapplication.h"

//----------------------------------------------------------------------
// wxGISNewMenu
//----------------------------------------------------------------------

wxGISNewMenu::wxGISNewMenu(const wxString& sName, const wxString& sCaption, wxGISEnumCommandBars type, const wxString& title, long style) : wxGISMenu(sName, sCaption, type, title, style), wxGISCommand()
{
	m_pCatalog = NULL;
    SetId(NEWMENUID);
}

wxGISNewMenu::~wxGISNewMenu(void)
{
}

wxIcon wxGISNewMenu::GetBitmap(void)
{
	return wxNullIcon;
}

wxString wxGISNewMenu::GetCaption(void)
{
	return wxString(_("Create"));
}

wxString wxGISNewMenu::GetCategory(void)
{
	return wxString(_("[Menus]"));
}

bool wxGISNewMenu::GetChecked(void)
{
	return false;
}

bool wxGISNewMenu::GetEnabled(void)
{
	return true;
}

wxString wxGISNewMenu::GetMessage(void)
{
	return wxString(_("Create menu"));
}

wxGISEnumCommandKind wxGISNewMenu::GetKind(void)
{
	return enumGISCommandMenu;
}

void wxGISNewMenu::OnClick(void)
{
}

bool wxGISNewMenu::OnCreate(wxGISApplicationBase* pApp)
{
	m_pApp = pApp;
    m_pCatalog = dynamic_cast<wxGxCatalogUI*>(GetGxCatalog());
    if(!m_pCatalog)
        return false;

	//TODO: Advise ???
    return true;
}

wxString wxGISNewMenu::GetTooltip(void)
{
	return wxString(_("Create menu"));
}

unsigned char wxGISNewMenu::GetCount(void)
{
	return 1;
}

void wxGISNewMenu::Update(wxGxSelection* Selection)
{
    wxCHECK_RET(Selection && m_pApp && m_pCatalog, wxT("Null pointers"));

    for(int i = m_CommandArray.size() - 1; i >= 0; --i)
        RemoveCommand(i);

	for(size_t i = 0; i < m_SubmenuArray.size(); ++i)
	{
		Delete(m_SubmenuArray[i].pItem);
		wsDELETE(m_SubmenuArray[i].pBar);
	}

    wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(Selection->GetLastSelectedObjectId());
    IGxObjectUI* pGxObjUI = dynamic_cast<IGxObjectUI*>(pGxObject);
    if(pGxObjUI)
    {
        wxGISCommandBar* pCmdBar = m_pApp->GetCommandBar( pGxObjUI->NewMenu() );
        if(pCmdBar)
        {
            for(size_t i = 0; i < pCmdBar->GetCommandCount(); ++i)
            {
                wxGISCommand* pCmd = pCmdBar->GetCommand(i);
                AddCommand(pCmd);
            }
        }
	}
}

void wxGISNewMenu::AddCommand(wxGISCommand* pCmd)
{
	switch(pCmd->GetKind())
	{
	case enumGISCommandSeparator:
		AppendSeparator();
		break;
	case enumGISCommandMenu:
		{
			wxGISCommandBar* pGISCommandBar = dynamic_cast<wxGISCommandBar*>(pCmd);
			if(pGISCommandBar)
			{
				pGISCommandBar->Reference();
				SUBMENUDATA data = {AppendSubMenu(dynamic_cast<wxMenu*>(pCmd), pCmd->GetCaption(), pCmd->GetMessage()), pGISCommandBar};
				m_SubmenuArray.push_back(data);
			}
		}
		break;
	case enumGISCommandCheck:
	case enumGISCommandRadio:
    case enumGISCommandNormal:
		{
			wxMenuItem *item = new wxMenuItem(this, pCmd->GetId(), pCmd->GetCaption(), pCmd->GetMessage(), (wxItemKind)pCmd->GetKind());
//TODO: check if works in new wxWidgets release
			wxBitmap Bmp = pCmd->GetBitmap();
			if(Bmp.IsOk())
            {
#ifdef __WIN32__
                wxImage Img = Bmp.ConvertToImage();
				item->SetBitmaps(Bmp, Img.ConvertToGreyscale());
#else
                item->SetBitmap(Bmp);
#endif
            }
			Append(item);
		}
		break;
	case enumGISCommandDropDown:
	case enumGISCommandControl:
	default:
		return;
	}
	wxGISCommandBar::AddCommand(pCmd);
}

