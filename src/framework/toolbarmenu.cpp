/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  toolbar check menu class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2012 Dmitry Baryshnikov
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
#include "wxgis/framework/toolbarmenu.h"
#include "wxgis/framework/commandbar.h"

//----------------------------------------------------------------------------
// wxGISToolBarMenu
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGISToolBarMenu, wxGISMenu)

BEGIN_EVENT_TABLE(wxGISToolBarMenu, wxMenu)
	EVT_MENU_RANGE(ID_TOOLBARCMD, ID_TOOLBARCMDMAX, wxGISToolBarMenu::OnCommand)
END_EVENT_TABLE()

wxGISToolBarMenu::wxGISToolBarMenu(const wxString& sName, const wxString& sCaption, wxGISEnumCommandBars type, const wxString& title, long style) : wxGISMenu(sName, sCaption, type, title, style)
{
}

wxGISToolBarMenu::~wxGISToolBarMenu(void)
{
}

void wxGISToolBarMenu::Update(void)
{
	wxVector<MENUITEM> current_items;
	wxGISCommandBarPtrArray CommandBars = m_pApp->GetCommandBars();
	for(size_t i = 0; i < CommandBars.GetCount(); ++i)
	{
		wxGISCommandBar* pCmdBar = CommandBars[i];
		if(pCmdBar->GetType() == enumGISCBToolbar)
		{
			wxWindow* pWnd = dynamic_cast<wxWindow*>(pCmdBar);
			bool bIsShown = m_pApp->IsApplicationWindowShown(pWnd);
			
			//TODO: sort panels in menu
			MENUITEM item = {ID_TOOLBARCMD + i, pCmdBar->GetCaption(), bIsShown}; 
			current_items.push_back(item);
		}
	}
		
	bool bSame = true;
	if(current_items.size() + 1 != m_menuitems.size())
		bSame = false;
		
	if(bSame)	
	{
		for ( size_t i = 0; i < current_items.size(); ++i ) 
		{    
			if(current_items[i].name != m_menuitems[i + 1].name){
				bSame = false;
				break;
			}
			
			if(current_items[i].checked != m_menuitems[i + 1].checked){
				bSame = false;
				break;
			}
		}
	}
	
	if(bSame){
		return;
	}
 	//return;

	//cleare contents
    for (size_t i = 0; i < m_menuitems.size(); ++i)
    {
		Destroy(m_menuitems[i].id);
    }

	m_menuitems.clear();

	if(current_items.size() > 0){
		MENUITEM item = {PrependSeparator()->GetId(), "", false};
		m_menuitems.push_back(item);
	}
	
	for ( size_t i = 0; i < current_items.size(); ++i ) 
	{
		m_menuitems.push_back(current_items[i]);
		wxMenuItem* pAddedItem = PrependCheckItem(current_items[i].id, current_items[i].name);
		pAddedItem->Check(current_items[i].checked);
	}
}

void wxGISToolBarMenu::OnCommand(wxCommandEvent& event)
{
	int pos = event.GetId() - (ID_TOOLBARCMD);
	wxGISCommandBarPtrArray CommandBars = m_pApp->GetCommandBars();
	wxWindow* pWnd = dynamic_cast<wxWindow*>(CommandBars[pos]);
	bool bIsShown = m_pApp->IsApplicationWindowShown(pWnd);
	m_pApp->ShowApplicationWindow(pWnd, !bIsShown);
}

wxIcon wxGISToolBarMenu::GetBitmap(void)
{
	return wxNullIcon;
}

wxString wxGISToolBarMenu::GetCaption(void)
{
	return wxString(_("Toolbars"));
}

wxString wxGISToolBarMenu::GetCategory(void)
{
	return wxString(_("[Menus]"));
}

bool wxGISToolBarMenu::GetChecked(void)
{
	return false;
}

bool wxGISToolBarMenu::GetEnabled(void)
{
	return true;
}

wxString wxGISToolBarMenu::GetMessage(void)
{
	return wxString(_("Toolbars"));
}

wxGISEnumCommandKind wxGISToolBarMenu::GetKind(void)
{
	return enumGISCommandMenu;
}

void wxGISToolBarMenu::OnClick(void)
{
}

bool wxGISToolBarMenu::OnCreate(wxGISApplicationBase* pApp)
{
	m_pApp = dynamic_cast<wxGISApplication*>(pApp);
	
	wxGISCommand* pCmd = m_pApp->GetCommand(wxT("wxGISCommonCmd"), enumGISCommonCmdCustomize);
    if (NULL != pCmd)
    {
		Append(pCmd->GetId(), pCmd->GetCaption(), pCmd->GetTooltip(), (wxItemKind)pCmd->GetKind());
    }
	pCmd = m_pApp->GetCommand(wxT("wxGISCommonCmd"), enumGISCommonCmdFitToolbars);
    if (NULL != pCmd)
    {
		Append(pCmd->GetId(), pCmd->GetCaption(), pCmd->GetTooltip(), (wxItemKind)pCmd->GetKind());
    }
	pCmd = m_pApp->GetCommand(wxT("wxGISCommonCmd"), enumGISCommonCmdOptimizeToolbars);
    if (NULL != pCmd)
    {
		Append(pCmd->GetId(), pCmd->GetCaption(), pCmd->GetTooltip(), (wxItemKind)pCmd->GetKind());
    }
	
	return true;
}

wxString wxGISToolBarMenu::GetTooltip(void)
{
	return wxString(_("Toolbars"));
}

unsigned char wxGISToolBarMenu::GetCount(void)
{
	return 1;
}
