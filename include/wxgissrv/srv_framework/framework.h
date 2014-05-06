/******************************************************************************
 * Project:  wxGIS (GIS Server)
 * Purpose:  wxGIS Server Framework header.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2008-2011 Bishop
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

#include "wxgis/base.h"

/*
#include "wxgis/core/config.h"
#include "wxgis/net/net.h"


typedef struct _userinfo
{
	wxString sPluginName;
	wxString sUser;
	wxString sCryptPass;
	//wxString sGroup;
	//wxString sMail;
	//security mask
	//etc.
} USERINFO;

typedef struct _authrespond
{
	bool bIsValid;
	long nUserID;
	USERINFO info;
}AUTHRESPOND;

class IServerApplication
{
public:
	virtual ~IServerApplication(void) {};
	//pure virtual
	virtual IGISConfig* GetConfig(void) = 0;
    virtual void PutInMessage(WXGISMSG msg) = 0;//message to server
    virtual void PutOutMessage(WXGISMSG msg) = 0;//message from server & server plugins
	virtual void SetAuth(AUTHRESPOND stUserInfo) = 0;
	virtual AUTHRESPOND GetAuth(long nID) = 0;
};

class IService
{
public:
    virtual ~IService(void){};
    virtual bool Start(IServerApplication* pApp, wxXmlNode* pConfig) = 0;
    virtual bool Stop(void) = 0;
};

typedef std::vector<IService*> SERVICEARRAY;

/** \class IRxObjectServer catalog.h
    \brief The main interface class for remote GxObjects.
*//*
class IRxObjectServer
{
public:
	virtual ~IRxObjectServer(void){};
	virtual wxXmlNode* GetDescription(void) = 0;
	virtual void SetApplication(IServerApplication* pApp) = 0;
};

*/
