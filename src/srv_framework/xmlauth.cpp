/******************************************************************************
 * Project:  wxGIS (GIS Server)
 * Purpose:  XML Auth plugin class. Store user info in cofig
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
/*#include "wxgissrv/srv_auth/xmlauth.h"

IMPLEMENT_DYNAMIC_CLASS(wxGISXmlAuthPlugin, wxObject)

wxGISXmlAuthPlugin::wxGISXmlAuthPlugin()
{
}

wxGISXmlAuthPlugin::~wxGISXmlAuthPlugin()
{
}

AUTHRESPOND wxGISXmlAuthPlugin::GetAuth(wxString sLogin, wxString sCryptPass)
{
	AUTHRESPOND respond;
	respond.bIsValid = false;
	for(size_t i = 0; i < m_UserArray.size(); i++)
	{
		if(m_UserArray[i].sUser == sLogin && m_UserArray[i].sCryptPass == sCryptPass)
		{
			respond.bIsValid = true;
			respond.info = m_UserArray[i];
			return respond;
		}
	}
	return respond;
}

bool wxGISXmlAuthPlugin::Init(wxXmlNode* pConfig)
{
	m_pConfig = pConfig;
	wxXmlNode* pNode = m_pConfig->GetChildren();
	while(pNode)
	{
		USERINFO info;
		info.sPluginName = wxString(wxT("XmlAuthPlugin"));
		info.sUser = pNode->GetPropVal(wxT("user"), NONAME);
		info.sCryptPass = pNode->GetPropVal(wxT("pass"), wxT(""));
		m_UserArray.push_back(info);
		pNode = pNode->GetNext();
	}	
	return true;
}
*/
