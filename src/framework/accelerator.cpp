/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  GIS application accelerator table header.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2012 Bishop
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
#include "wxgis/framework/accelerator.h"
#include "wx/tokenzr.h"

wxGISAcceleratorTable::wxGISAcceleratorTable(wxGISApplicationBase* pApp) : bHasChanges(true)
{
	m_AccelEntryArray.reserve(20);

	wxGISAppConfig oConfig = GetConfig();
    if(!oConfig.IsOk())
		return;

	wxXmlNode* pAcceleratorsNodeCU = oConfig.GetConfigNode(enumGISHKCU, pApp->GetAppName() + wxString(wxT("/accelerators")));
	wxXmlNode* pAcceleratorsNodeLM = oConfig.GetConfigNode(enumGISHKLM, pApp->GetAppName() + wxString(wxT("/accelerators")));
	//merge two tables
	m_pApp = pApp;
	if(!pApp)
		return;

	//TODO: merge acc tables
	//TODO: if user delete key - it must be mark as deleted to avoid adding it fron LM table

	if(pAcceleratorsNodeCU)
	{
		wxXmlNode *child = pAcceleratorsNodeCU->GetChildren();
		while(child)
		{
			wxString sCmdName = child->GetAttribute(wxT("cmd_name"), NON);
			unsigned char nSubtype = wxAtoi(child->GetAttribute(wxT("subtype"), wxT("0")));
			wxGISCommand* pCmd = m_pApp->GetCommand(sCmdName, nSubtype);
			if(pCmd)
			{
				wxString sFlags = child->GetAttribute(wxT("flags"), wxT("NORMAL"));
                wxDword Flags = GetFlags(sFlags);
				wxString sKey = child->GetAttribute(wxT("keycode"), wxT("A"));
				int nKey = GetKeyCode(sKey);
				Add(wxAcceleratorEntry(Flags, nKey, pCmd->GetId()));
			}
			child = child->GetNext();
		}
	}
	if(pAcceleratorsNodeLM)
	{
		wxXmlNode *child = pAcceleratorsNodeLM->GetChildren();
		while(child)
		{
			wxString sCmdName = child->GetAttribute(wxT("cmd_name"), NON);
			unsigned char nSubtype = wxAtoi(child->GetAttribute(wxT("subtype"), wxT("0")));
			wxGISCommand* pCmd = m_pApp->GetCommand(sCmdName, nSubtype);
			if(pCmd)
			{
				wxString sFlags = child->GetAttribute(wxT("flags"), wxT("NORMAL"));
                wxDword Flags = GetFlags(sFlags);
				wxString sKey = child->GetAttribute(wxT("keycode"), wxT("A"));
				int nKey = GetKeyCode(sKey);
				Add(wxAcceleratorEntry(Flags, nKey, pCmd->GetId()));
			}
			child = child->GetNext();
		}
	}
}

wxGISAcceleratorTable::~wxGISAcceleratorTable(void)
{
}

int wxGISAcceleratorTable::Add(wxAcceleratorEntry entry)
{
	int cmd(wxID_ANY);
	bool bAdd(true);
	for(size_t i = 0; i < m_AccelEntryArray.size(); ++i)
	{
		if(m_AccelEntryArray[i].GetKeyCode() == entry.GetKeyCode() && m_AccelEntryArray[i].GetFlags() == entry.GetFlags())
		{
			cmd = m_AccelEntryArray[i].GetCommand();
			m_AccelEntryArray[i] = entry;
			bAdd = false;
		}
		if(m_AccelEntryArray[i].GetCommand() == entry.GetCommand())
		{
			m_AccelEntryArray[i] = entry;
			bAdd = false;
		}
	}
	if(bAdd)
		m_AccelEntryArray.push_back(entry);
	bHasChanges = true;
	return cmd;
}

void wxGISAcceleratorTable::Remove(wxAcceleratorEntry entry)
{
	for(size_t i = 0; i < m_AccelEntryArray.size(); ++i)
	{
		if(m_AccelEntryArray[i] == entry)
		{
			m_AccelEntryArray.erase(m_AccelEntryArray.begin() + i);
			if(!bHasChanges)
				bHasChanges = true;
		}
	}
}

wxAcceleratorEntry wxGISAcceleratorTable::GetEntry(int cmd)
{
	for(size_t i = 0; i < m_AccelEntryArray.size(); ++i)
		if(m_AccelEntryArray[i].GetCommand() == cmd)
            return m_AccelEntryArray[i];
	return wxAcceleratorEntry();
}

wxAcceleratorTable wxGISAcceleratorTable::GetAcceleratorTable(void)
{
	if(bHasChanges)
	{
		wxAcceleratorEntry* entries = new wxAcceleratorEntry[m_AccelEntryArray.size()];
		for(size_t i = 0; i < m_AccelEntryArray.size(); ++i)
			entries[i] = m_AccelEntryArray[i];

		wxAcceleratorTable ATab(m_AccelEntryArray.size(), entries);
		delete [] entries;
		m_ATab = ATab;
		bHasChanges = false;
	}
	if(m_ATab.IsOk())
		return m_ATab;
	else
		return wxNullAcceleratorTable;
}

wxString wxGISAcceleratorTable::GetText(int cmd)
{
	for(size_t i = 0; i < m_AccelEntryArray.size(); ++i)
	{
		if(m_AccelEntryArray[i].GetCommand() == cmd)
		{
			wxString res = m_AccelEntryArray[i].ToString();
			res.Replace(wxT("|"),wxT("+"));
			res.Replace(wxT("-"),wxT("+"));
			return res;
		}
	}
	return wxEmptyString;
}

void wxGISAcceleratorTable::Store(void)
{
	wxGISAppConfig oConfig = GetConfig();
    if(!oConfig.IsOk())
		return;

	wxXmlNode* pAcceleratorsNodeCU = oConfig.GetConfigNode(enumGISHKCU, m_pApp->GetAppName() + wxString(wxT("/accelerators")));
	if(pAcceleratorsNodeCU)
		oConfig.DeleteNodeChildren(pAcceleratorsNodeCU);
	else
		pAcceleratorsNodeCU = oConfig.CreateConfigNode(enumGISHKCU, m_pApp->GetAppName() + wxString(wxT("/accelerators")));
	for(size_t i = 0; i < m_AccelEntryArray.size(); ++i)
	{
		wxGISCommand* pCmd = m_pApp->GetCommand(m_AccelEntryArray[i].GetCommand());
		wxObject* pObj = dynamic_cast<wxObject*>(pCmd);
		if(pObj)
		{
			wxClassInfo* pInfo = pObj->GetClassInfo();
			wxString sClassName = pInfo->GetClassName();
			unsigned char nSubType = pCmd->GetSubType();
			int nKeyCode = m_AccelEntryArray[i].GetKeyCode();
			int nFlags = m_AccelEntryArray[i].GetFlags();
			wxString sFlags;

			if(nFlags & wxMOD_ALT)
				sFlags += wxT("ALT|");
			if(nFlags & wxMOD_CONTROL)
				sFlags += wxT("CTRL|");
			if(nFlags & wxMOD_SHIFT)
				sFlags += wxT("SHIFT|");
			if(nFlags == 0)
				sFlags += wxT("NORMAL");

			wxString sKeyCode;
			if(nKeyCode >= WXK_F1 && nKeyCode <= WXK_F12)
			{
				sKeyCode += wxString::Format(wxT("WXK_F%d"), nKeyCode - WXK_DIVIDE );
			}
			else if((nKeyCode >= (int)'A' && nKeyCode <= (int)'Z') || (nKeyCode >= (int)'0' && nKeyCode <= (int)'9') || (nKeyCode >= WXK_NUMPAD0 && nKeyCode <= WXK_NUMPAD9))
			{
				sKeyCode += wxString(wxChar(nKeyCode));
			}
			else if(nKeyCode == WXK_DELETE )
			{
				sKeyCode += wxString(wxT("WXK_DELETE"));
			}	
			else if(nKeyCode == WXK_NUMPAD_DELETE)
			{
				sKeyCode += wxString(wxT("WXK_NUMPAD_DELETE"));
			}
			else if(nKeyCode == WXK_ESCAPE)
			{
				sKeyCode += wxString(wxT("WXK_ESCAPE"));
			}
			else if(nKeyCode == WXK_SPACE)
			{
				sKeyCode += wxString(wxT("WXK_SPACE"));
			}
			else if(nKeyCode == WXK_NUMPAD_SPACE)
			{
				sKeyCode += wxString(wxT("WXK_NUMPAD_SPACE"));
			}
			else if(nKeyCode == WXK_RETURN)
			{
				sKeyCode += wxString(wxT("WXK_RETURN"));
			}
			else if(nKeyCode == WXK_EXECUTE)
			{
				sKeyCode += wxString(wxT("WXK_EXECUTE"));
			}
			else if(nKeyCode == WXK_PAUSE)
			{
				sKeyCode += wxString(wxT("WXK_PAUSE"));
			}
			else if(nKeyCode == WXK_END)
			{
				sKeyCode += wxString(wxT("WXK_END"));
			}
			else if(nKeyCode == WXK_NUMPAD_END)
			{
				sKeyCode += wxString(wxT("WXK_NUMPAD_END"));
			}
			else if(nKeyCode == WXK_HOME)
			{
				sKeyCode += wxString(wxT("WXK_HOME"));
			}
			else if(nKeyCode == WXK_NUMPAD_HOME)
			{
				sKeyCode += wxString(wxT("WXK_NUMPAD_HOME"));
			}
			else if(nKeyCode == WXK_INSERT)
			{
				sKeyCode += wxString(wxT("WXK_INSERT"));
			}
			else if(nKeyCode == WXK_NUMPAD_INSERT)
			{
				sKeyCode += wxString(wxT("WXK_NUMPAD_INSERT"));
			}

			wxXmlNode* pNewNode = new wxXmlNode(pAcceleratorsNodeCU, wxXML_ELEMENT_NODE, wxString(wxT("Entry")));
			pNewNode->AddAttribute(wxT("cmd_name"), sClassName);
			pNewNode->AddAttribute(wxT("subtype"), wxString::Format(wxT("%u"), nSubType));
			pNewNode->AddAttribute(wxT("flags"), sFlags);
			pNewNode->AddAttribute(wxT("keycode"), sKeyCode);
		}
	}
}

int wxGISAcceleratorTable::GetKeyCode(wxString sKeyCode)
{
	sKeyCode.MakeUpper();
	int Key;
	if(sKeyCode.Len() == 1)
		Key = (int)sKeyCode.GetChar(0);
	else
	{
		if(sKeyCode.Find(wxT("WXK_F")) != wxNOT_FOUND)
		{
			int num = wxAtoi(sKeyCode.Right(sKeyCode.Len() - 5)) - 1;
			Key = WXK_F1 + num;
		}
		else if(sKeyCode == wxString(wxT("WXK_DELETE")))
		{
			Key = WXK_DELETE;
		}	
		else if(sKeyCode == wxString(wxT("WXK_NUMPAD_DELETE")))
		{
			Key = WXK_NUMPAD_DELETE;
		}
		else if(sKeyCode == wxString(wxT("WXK_ESCAPE")))
		{
			Key = WXK_ESCAPE;
		}
		else if(sKeyCode == wxString(wxT("WXK_SPACE")))
		{
			Key = WXK_SPACE;
		}
		else if(sKeyCode == wxString(wxT("WXK_NUMPAD_SPACE")))
		{
			Key = WXK_NUMPAD_SPACE;
		}
		else if(sKeyCode == wxString(wxT("WXK_RETURN")))
		{
			Key = WXK_RETURN;
		}
		else if(sKeyCode == wxString(wxT("WXK_EXECUTE")))
		{
			Key = WXK_EXECUTE;
		}
		else if(sKeyCode == wxString(wxT("WXK_PAUSE")))
		{
			Key = WXK_PAUSE;
		}
		else if(sKeyCode == wxString(wxT("WXK_END")))
		{
			Key = WXK_END;
		}
		else if(sKeyCode == wxString(wxT("WXK_NUMPAD_END")))
		{
			Key = WXK_NUMPAD_END;
		}
		else if(sKeyCode == wxString(wxT("WXK_HOME")))
		{
			Key = WXK_HOME;
		}
		else if(sKeyCode == wxString(wxT("WXK_NUMPAD_HOME")))
		{
			Key = WXK_NUMPAD_HOME;
		}
		else if(sKeyCode == wxString(wxT("WXK_INSERT")))
		{
			Key = WXK_INSERT;
		}
		else if(sKeyCode == wxString(wxT("WXK_NUMPAD_INSERT")))
		{
			Key = WXK_NUMPAD_INSERT;
		}
	}
	return Key; 
}

wxDword wxGISAcceleratorTable::GetFlags(wxString sFlags)
{
    wxDword Flags(0);
	wxStringTokenizer tkz(sFlags, wxString(wxT("|")), wxTOKEN_RET_EMPTY );
	while ( tkz.HasMoreTokens() )
	{
		wxString token = tkz.GetNextToken();
		//token.Replace(wxT("|"), wxT(""));	
		token.MakeUpper();
		// process token here
		if(token == wxString(wxT("NORMAL")))
			Flags |= wxACCEL_NORMAL;
		else if(token == wxString(wxT("ALT")))
			Flags |= wxACCEL_ALT;
		else if(token == wxString(wxT("CTRL")))
			Flags |= wxACCEL_CTRL;
		else if(token == wxString(wxT("SHIFT")))
			Flags |= wxACCEL_SHIFT;
	}
	return Flags;
}
