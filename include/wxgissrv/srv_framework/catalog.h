/******************************************************************************
 * Project:  wxGIS (GIS Server)
 * Purpose:  wxGxServerCatalog class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010 Bishop
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
#pragma once
/*
#include "wxgissrv/srv_framework/framework.h"
#include "wxgis/catalog/gxcatalog.h"
#include "wxgis/networking/message.h"

/** \class wxRxCatalog catalog.h
    \brief The main Server catalog class.

	The catalog calss stores root items of server. The server can sent to client the list of root items
*/
/*
class WXDLLIMPEXP_GIS_FRW wxRxCatalog :
	public wxGxCatalog,
	public IService,
	public INetMessageReceiver
{
public:
	wxRxCatalog(void);
	virtual ~wxRxCatalog(void);
	//wxGxCatalog
	void Init(void);
	virtual wxString GetName(void){return wxString(wxT("root"));};
	virtual void LoadChildren(wxXmlNode* pNode);
	//IService
    virtual bool Start(IServerApplication* pApp, wxXmlNode* pConfig);
    virtual bool Stop(void);
	//INetMessageReceiver
    virtual void ProcessMessage(WXGISMSG msg, wxXmlNode* pChildNode);
protected:
    IServerApplication* m_pApp;
};
*/
