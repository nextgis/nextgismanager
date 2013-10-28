/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISMenuBar class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2012  Bishop
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
#include "wxgis/framework/menubar.h"
#include "wxgis/core/config.h"
#include "wxgis/framework/toolbarmenu.h"

//IMPLEMENT_DYNAMIC_CLASS(wxGISMenuBar, wxGISMenuBar)

wxGISMenuBar::wxGISMenuBar(long style, wxGISApplicationBase* pApp, wxXmlNode* pConf) : wxMenuBar(style)
{
	if(!pConf || pApp == NULL)
		return;
	wxXmlNode *child = pConf->GetChildren();
	while(child)
	{
		wxString sMenuName = child->GetAttribute(wxT("name"), wxT(""));
		AddMenu(pApp->GetCommandBar(sMenuName));
		child = child->GetNext();
	}
}

wxGISMenuBar::~wxGISMenuBar(void)
{
    for(size_t i = 0; i < m_MenubarArray.size(); ++i)
	{
        m_MenubarArray[i]->Release();
    }

	//while(GetMenuCount() > 0)
 //   {
	//	Remove(0);
 //   }

	m_menus.Clear();
}

bool wxGISMenuBar::IsMenuBarMenu(wxString sMenuName)
{
	for(size_t i = 0; i < m_MenubarArray.size(); ++i)
	{
		if(m_MenubarArray[i]->GetName() == sMenuName)
			return true;
	}
	return false;
}

wxGISCommandBarPtrArray wxGISMenuBar::GetMenuBarArray(void) const
{
	return m_MenubarArray;
}

void wxGISMenuBar::MoveLeft(int pos)
{
	if(pos == 0)
		return;

	//m_MenubarArray.swap(pos, pos - 1);
	wxGISCommandBar* val = m_MenubarArray[pos];
	m_MenubarArray[pos] = m_MenubarArray[pos - 1];
	m_MenubarArray[pos - 1] = val;
	Insert(pos - 1, Remove(pos), m_MenubarArray[pos - 1]->GetCaption());
}

void wxGISMenuBar::MoveRight(int pos)
{
	if(pos == m_MenubarArray.size() - 1)
		return;
//	m_MenubarArray.swap(pos, pos + 1);
	wxGISCommandBar* val = m_MenubarArray[pos];
	m_MenubarArray[pos] = m_MenubarArray[pos + 1];
	m_MenubarArray[pos + 1] = val;
	Insert(pos + 1, Remove(pos), m_MenubarArray[pos + 1]->GetCaption());
}

void wxGISMenuBar::MoveLeft(wxGISCommandBar* pBar)
{
	int nPos = GetMenuPos(pBar);
	if(nPos != wxNOT_FOUND)
		MoveLeft(nPos);
}

void wxGISMenuBar::MoveRight(wxGISCommandBar* pBar)
{
	int nPos = GetMenuPos(pBar);
	if(nPos != wxNOT_FOUND)
		MoveRight(nPos);
}

int wxGISMenuBar::GetMenuPos(wxGISCommandBar* pBar)
{
	if(pBar == NULL)
		return wxNOT_FOUND;
	for(size_t i = 0; i < m_MenubarArray.size(); ++i)
		if(m_MenubarArray[i] == pBar)
			return i;
	return wxNOT_FOUND;
}

void wxGISMenuBar::RemoveMenu(wxGISCommandBar* pBar)
{
	for(size_t i = 0; i < m_MenubarArray.size(); ++i)
	{
		if(m_MenubarArray[i] == pBar)
		{
			Remove(i);
            m_MenubarArray[i]->Release();
			m_MenubarArray.erase(m_MenubarArray.begin() + i);
		}
	}
}

bool wxGISMenuBar::AddMenu(wxGISCommandBar* pBar)
{
	wxMenu* pMenu = dynamic_cast<wxMenu*>(pBar);
	if(pMenu)
	{
        pBar->Reference();
		Append(pMenu, pBar->GetCaption());
		m_MenubarArray.push_back(pBar);
		return true;
	}
	return false;
}

void wxGISMenuBar::Serialize(wxXmlNode* pConf)
{
	if(!pConf)
		return;
	wxGISConfig::DeleteNodeChildren(pConf);
	for(size_t i = m_MenubarArray.size(); i > 0; i--)
	{
		wxString sName = m_MenubarArray[i - 1]->GetName();
		wxXmlNode* pNewNode = new wxXmlNode(pConf, wxXML_ELEMENT_NODE, wxString(wxT("menu")));
		pNewNode->AddAttribute(wxT("name"), sName);
	}
}
