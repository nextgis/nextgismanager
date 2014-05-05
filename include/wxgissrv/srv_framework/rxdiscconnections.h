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
#pragma once
/*
#include "wxgissrv/srv_framework/framework.h"
#include "wxgis/catalog/gxdiscconnections.h"

/** \class wxRxDiscConnections rxdiscconnections.h
    \brief A Disc Connections RxRootObject.
*//*
class WXDLLIMPEXP_GIS_FRW wxRxDiscConnections :
    public wxGxDiscConnections,
	public IRxObjectServer,
	public INetMessageReceiver
{
   DECLARE_DYNAMIC_CLASS(wxRxDiscConnections)
public:
	wxRxDiscConnections(void);
	virtual ~wxRxDiscConnections(void);
	//wxGxDiscConnections
    virtual wxString GetFullName(void){return wxString(wxT("discconn"));};
	virtual void Detach(void);
	//IRxObjectServer
	virtual wxXmlNode* GetDescription(void);
	virtual void SetApplication(IServerApplication* pApp);
	//INetMessageReceiver
    virtual void ProcessMessage(WXGISMSG msg, wxXmlNode* pChildNode);
protected:
    IServerApplication* m_pApp;
};
*/