/******************************************************************************
 * Project:  wxGIS (GIS Server)
 * Purpose:  wxRxDiscConnections class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010 Bishop
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
#include "wxgissrv/srv_framework/rxdiscconnections.h"
/*
#include "wxgis/networking/message.h"

IMPLEMENT_DYNAMIC_CLASS(wxRxDiscConnections, wxGxDiscConnections)

wxRxDiscConnections::wxRxDiscConnections(void) : wxGxDiscConnections()
{
}

wxRxDiscConnections::~wxRxDiscConnections(void)
{
}

wxXmlNode* wxRxDiscConnections::GetDescription(void)
{
    wxXmlNode* pNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("child"));
	pNode->AddProperty(wxT("class"), wxT("wxRxDiscConnections"));
	//wxString sDst = wxNetMessage::FormatXmlString(GetFullName());
	pNode->AddProperty(wxT("dst"), GetFullName());
	return pNode;
}

void wxRxDiscConnections::SetApplication(IServerApplication* pApp)
{
	m_pApp = pApp;
	INetMessageProcessor* pNetMessageProcessor = dynamic_cast<INetMessageProcessor*>(m_pApp);
	if(pNetMessageProcessor)
		pNetMessageProcessor->AddMessageReceiver(GetFullName(), static_cast<INetMessageReceiver*>(this));
}

void wxRxDiscConnections::ProcessMessage(WXGISMSG msg, wxXmlNode* pChildNode)
{

}

void wxRxDiscConnections::Detach()
{
	INetMessageProcessor* pNetMessageProcessor = dynamic_cast<INetMessageProcessor*>(m_pApp);
	if(pNetMessageProcessor)
		pNetMessageProcessor->DelMessageReceiver(GetFullName(), static_cast<INetMessageReceiver*>(this));
	wxGxDiscConnections::Detach();
}

*/

