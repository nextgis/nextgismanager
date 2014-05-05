/******************************************************************************
 * Project:  wxGIS (GIS Server)
 * Purpose:  wxNetMessage class. Network message class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2008-2010  Bishop
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
/*
#include "wxgissrv/srv_framework/message.h"
#include <wx/stream.h>
#include <wx/sstream.h>

wxNetMessage::wxNetMessage(void) : m_bIsOk(false), m_pXmlDocument(NULL)
{
    m_nPriority = enumGISPriorityNormal;
    m_nState = enumGISMsgStUnk;

    m_pXmlDocument = new wxXmlDocument();
    wxXmlNode* pRootNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("msg"));
    pRootNode->AddProperty(wxT("ver"), wxString::Format(wxT("%d"), WXNETVER));
    pRootNode->AddProperty(wxT("priority"), wxString::Format(wxT("%d"), m_nPriority));
    pRootNode->AddProperty(wxT("state"), wxString::Format(wxT("%d"), m_nState));
    m_pXmlDocument->SetRoot(pRootNode);
    m_bIsOk = true;
}
 
wxNetMessage::wxNetMessage(long nID, wxGISMessageState nState, short nPriority, wxString sMessage, wxString sModuleSrc, wxString sModuleDst) : m_bIsOk(false), m_pXmlDocument(NULL)
{
    m_nPriority = nPriority;
    m_nID = nID;
    m_nState = nState;

    m_pXmlDocument = new wxXmlDocument();
    wxXmlNode* pRootNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("msg"));
    pRootNode->AddProperty(wxT("ver"), wxString::Format(wxT("%d"), WXNETVER));
    pRootNode->AddProperty(wxT("priority"), wxString::Format(wxT("%d"), m_nPriority));
    if(!sModuleSrc.IsEmpty())
        pRootNode->AddProperty(wxT("src"), sModuleSrc);
    if(!sModuleDst.IsEmpty())
        pRootNode->AddProperty(wxT("dst"), sModuleDst);
    pRootNode->AddProperty(wxT("state"), wxString::Format(wxT("%d"), m_nState));
    if(!sMessage.IsEmpty())
        pRootNode->AddProperty(wxT("message"), wxString::Format(wxT("%s"), sMessage.c_str()));
    m_pXmlDocument->SetRoot(pRootNode);
    m_bIsOk = true;
}

wxNetMessage::wxNetMessage(wxString sMsgData, long nID) : m_bIsOk(false)
{
    m_nID = nID;

	wxStringInputStream Stream(sMsgData);
    
    m_pXmlDocument = new wxXmlDocument();
	if(!m_pXmlDocument->Load(Stream) || !m_pXmlDocument->IsOk())
		return;
	
	wxXmlNode *pRoot = m_pXmlDocument->GetRoot();
	if(pRoot && pRoot->GetName() != wxT("msg"))
		return;

    if(wxAtoi(pRoot->GetPropVal(wxT("ver"), wxT("1"))) > WXNETVER)
		return;

    //header
	m_nState = (wxGISMessageState)wxAtoi(pRoot->GetPropVal(wxT("state"), wxT("0")));
	m_sMessage = pRoot->GetPropVal(wxT("message"), wxT(""));
	m_nPriority = wxAtoi(pRoot->GetPropVal(wxT("priority"), wxT("0")));
	m_bIsOk = true;
}

wxNetMessage::~wxNetMessage(void)
{
    wxDELETE(m_pXmlDocument);
}

short wxNetMessage::GetPriority(void)
{
    return m_nPriority;
}

void wxNetMessage::SetPriority(short nPriority)
{
    if(m_pXmlDocument)
    {
        wxXmlNode* pRootNode = m_pXmlDocument->GetRoot();
        if(pRootNode)
        {
            if(pRootNode->HasProp(wxT("priority")))
                pRootNode->DeleteProperty(wxT("priority"));
            pRootNode->AddProperty(wxT("priority"), wxString::Format(wxT("%d"), nPriority));
        }
    }
    m_nPriority = nPriority;
}

//
//bool wxNetMessage::operator< (const INetMessage& msg) const
//{
//    return m_nPriority < msg.m_nPriority;
//}
//
//INetMessage& wxNetMessage::operator= (const INetMessage& oSource)
//{
//    m_pXmlDocument = oSource.m_pXmlDocument;
//    m_nID = oSource.m_nID;
//	m_bIsOk = oSource.m_bIsOk;
//    m_nPriority = oSource.m_nPriority;
//
//    return *this;
//}

bool wxNetMessage::IsOk(void)
{
    if(!m_pXmlDocument)
        return false;
    return m_pXmlDocument->IsOk() && m_bIsOk;
}

wxGISMessageDirection wxNetMessage::GetDirection(void)
{
    return m_nDirection;
}

void wxNetMessage::SetDirection(wxGISMessageDirection nDirection)
{
    m_nDirection = nDirection;
}

wxXmlNode* wxNetMessage::GetRoot(void)
{
    if(m_pXmlDocument)
        return m_pXmlDocument->GetRoot();
    return NULL;
}

wxString wxNetMessage::GetData(void)
{
    wxString sData;
    wxStringOutputStream Stream(&sData);
	if(m_pXmlDocument && m_pXmlDocument->Save(Stream))
        return sData;
    return wxEmptyString;
}

*/