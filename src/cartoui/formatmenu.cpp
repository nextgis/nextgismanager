/******************************************************************************
 * Project:  wxGIS
 * Purpose:  menu to show format clases (for format coordinates etc.).
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011 Dmitry Baryshnikov
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
#include "wxgis/cartoui/formatmenu.h"

#include "wxgis/core/config.h"
//#include "wxgis/core/globalfn.h"

#include "wx/propgrid/propgrid.h"

#define POS_TO_INSERT 2

BEGIN_EVENT_TABLE(wxGISCoordinatesFormatMenu, wxMenu)
	EVT_MENU(wxGISCoordinatesFormatMenu::ID_SWAPCOORS, wxGISCoordinatesFormatMenu::OnSwapCoords)
	EVT_UPDATE_UI(wxGISCoordinatesFormatMenu::ID_SWAPCOORS, wxGISCoordinatesFormatMenu::OnSwapCoordsUI)
	EVT_MENU(wxGISCoordinatesFormatMenu::ID_CHANGEMASKS, wxGISCoordinatesFormatMenu::OnChangeMasks)
	EVT_MENU_RANGE(wxGISCoordinatesFormatMenu::ID_MASKBEG, wxGISCoordinatesFormatMenu::ID_MASKBEG + 255, wxGISCoordinatesFormatMenu::OnSetMask)
END_EVENT_TABLE()


wxGISCoordinatesFormatMenu::wxGISCoordinatesFormatMenu(const wxString& title, long style) : wxMenu(title, style), wxGISCoordinatesFormat()
{
	//preparations
	Serialize(false);
	//TODO:
	/*
	//add selectors m -> cm etc.
	Append(wxID_ANY, _("Kilometers"));
	Append(wxID_ANY, _("Meters"));
	Append(wxID_ANY, _("Decimeters"));
	Append(wxID_ANY, _("Centimeters"));
	Append(wxID_ANY, _("Millimeters"));
	AppendSeparator();
	Append(wxID_ANY, _("Nautical miles"));
	Append(wxID_ANY, _("Miles"));
	Append(wxID_ANY, _("Yards"));
	Append(wxID_ANY, _("Feet"));
	Append(wxID_ANY, _("Inches"));
	AppendSeparator();
	Append(wxID_ANY, _("Degrees"));
	*/
	//add swp coords check mark
	//TODO: AppendSeparator();
	AppendCheckItem(ID_SWAPCOORS, _("Swap coordinates"));
	//TODO: AppendCheckItem(wxID_ANY, _("Add decimal separator"));

	//add formats
	AppendSeparator();
    for(size_t i = 0; i < m_asCoordsMask.GetCount(); ++i)
		Append(ID_MASKBEG + i, m_asCoordsMask[i]);
	//add change format list dialog
	AppendSeparator();
	Append(ID_CHANGEMASKS, _("Change masks"));
}

wxGISCoordinatesFormatMenu::~wxGISCoordinatesFormatMenu(void)
{
}

void wxGISCoordinatesFormatMenu::OnSwapCoords(wxCommandEvent& event)
{
	m_bSwaped = !m_bSwaped;
}

void wxGISCoordinatesFormatMenu::Serialize(bool bStore)
{
	wxGISAppConfig oConfig = GetConfig();
	if(!oConfig.IsOk())
		return;
	if(bStore)
	{
		wxXmlNode* pToolsNode = oConfig.GetConfigNode(enumGISHKCU, wxString(wxT("wxGISCommon/coordinate_mask")));
		if(pToolsNode)
			oConfig.DeleteNodeChildren(pToolsNode);
		else
			pToolsNode = oConfig.CreateConfigNode(enumGISHKCU, wxString(wxT("wxGISCommon/coordinate_mask")));
		if(pToolsNode)
		{
			for(size_t i = 0; i < m_asCoordsMask.GetCount(); ++i)
			{
				wxXmlNode *pMskNode = new wxXmlNode(pToolsNode, wxXML_ELEMENT_NODE, wxString(wxT("mask")));
				pMskNode->AddAttribute(wxT("text"),  m_asCoordsMask[i]);
			}
		}
	}
	else
	{
		m_asCoordsMask.Clear();
		wxXmlNode* pToolsNode = oConfig.GetConfigNode(enumGISHKCU, wxString(wxT("wxGISCommon/coordinate_mask")));
		if(pToolsNode)
		{
			wxXmlNode* pChild = pToolsNode->GetChildren();
			while(pChild)
			{
				wxString sMask = pChild->GetAttribute(wxT("text"), wxEmptyString);
				if(!sMask.IsEmpty())
				{
					m_asCoordsMask.Add(sMask);
				}
				pChild = pChild->GetNext();
			}
		}

		if(m_asCoordsMask.GetCount() == 0)
		{
			m_asCoordsMask.Add(wxT("dd-mm-ss.ss [W][ ]dd-mm-ss.ss [W]"));
#ifdef __WXMSW__
			m_asCoordsMask.Add(wxT("dd°mm'ss.ss\" [W][ ]dd°mm'ss.ss\" [W]"));
#endif
			m_asCoordsMask.Add(wxT("dd-mm-ss.ss[ ]dd-mm-ss.ss"));
			m_asCoordsMask.Add(wxT("ddmmss[ ]ddmmss"));
			m_asCoordsMask.Add(wxT("d.ddd[tab]d.ddd"));
			m_asCoordsMask.Add(wxT("d.ddd[ ]d.ddd"));
		}
	}
}

void wxGISCoordinatesFormatMenu::OnSwapCoordsUI(wxUpdateUIEvent& event)
{
	event.Check(m_bSwaped);
}

void wxGISCoordinatesFormatMenu::OnChangeMasks(wxCommandEvent& event)
{
	//TODO: update m_asCoordsMask from config before show dialog
	wxPGArrayStringEditorDialog dlg;
    dlg.SetDialogValue( wxVariant(m_asCoordsMask) );
    dlg.Create(dynamic_cast<wxWindow*>(GetApplication()), _("Change value list"), _("Value list"));
    int res = dlg.ShowModal();
    if ( res == wxID_OK && dlg.IsModified() )
    {
        wxVariant value = dlg.GetDialogValue();
        if ( !value.IsNull() )
        {
			//remove menu items
			for(size_t i = 0; i < m_asCoordsMask.GetCount(); ++i)
				Delete(i + ID_MASKBEG);
			m_asCoordsMask = value.GetArrayString();
			for(size_t i = 0; i < m_asCoordsMask.GetCount(); ++i)
				Insert(POS_TO_INSERT + i, ID_MASKBEG + i, m_asCoordsMask[i]);

			Serialize(true);
        }
    }
}

void wxGISCoordinatesFormatMenu::OnSetMask(wxCommandEvent& event)
{
	int nMaskID = event.GetId() - ID_MASKBEG;
	Create(m_asCoordsMask[nMaskID], m_bSwaped);
	//update coordinates dialog
}

void wxGISCoordinatesFormatMenu::PrepareMenu(void)
{
	for(size_t i = 0; i < m_asCoordsMask.GetCount(); ++i)
		Delete(i + ID_MASKBEG);
	Serialize(false);
	for(size_t i = 0; i < m_asCoordsMask.GetCount(); ++i)
		Insert(POS_TO_INSERT + i, ID_MASKBEG + i, m_asCoordsMask[i]);
}
