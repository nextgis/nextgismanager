/******************************************************************************
 * Project:  wxGIS (GIS Server)
 * Purpose:  wxGISAuthService class. Auth main service class. 
 *           It adds plug-ins of different types of authenticate (file, LDAP, OS etc.)
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2008-2010 Bishop
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
#pragma once
/*
#include "wxgissrv/srv_framework/framework.h"
#include "wxgis/networking/message.h"

/** \class IServerAuthPlugin auth.h
    \brief The base interface class for auth plugins.
*//*
class IServerAuthPlugin
{
public:
	virtual ~IServerAuthPlugin(void){};
	virtual AUTHRESPOND GetAuth(wxString sLogin, wxString sCryptPass) = 0;
	virtual bool Init(wxXmlNode* pConfig) = 0;
};

/** \class wxGISAuthService auth.h
    \brief A Server side Auth Service.
*//*
class WXDLLIMPEXP_GIS_FRW wxGISAuthService : 
	public IService,
	public INetMessageReceiver
{
public:
    wxGISAuthService(void);
    virtual ~wxGISAuthService(void);
	//IService
    virtual bool Start(IServerApplication* pApp, wxXmlNode* pConfig);
    virtual bool Stop(void);
	//INetMessageReceiver
    virtual void ProcessMessage(WXGISMSG msg, wxXmlNode* pChildNode);
protected:
    wxXmlNode* m_pConfig;
    IServerApplication* m_pApp;
	std::map<short, IServerAuthPlugin*> m_AuthPluginMap;// priority | plugin
};
*/
