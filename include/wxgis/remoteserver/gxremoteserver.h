/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  wxGxRemoteServer class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2012 Bishop
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

#include "wxgis/remoteserver/rxobjectclient.h"

/** \class wxGxRemoteServer gxremoteserver.h
    \brief A Remote Server GxObject.
*/
class WXDLLIMPEXP_GIS_RS wxGxRemoteServer :
	public wxRxCatalog
{
   DECLARE_CLASS(wxGxRemoteServer)
public:
    wxGxRemoteServer(void);
	wxGxRemoteServer(wxGISNetClientConnection* const pNetConn, wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
	virtual ~wxGxRemoteServer(void);
	//wxGxObject
    virtual wxString GetBaseName(void) const {return GetName();};
    virtual wxString GetFullName(void) const {return wxEmptyString;};
	virtual wxString GetCategory(void) const {return wxString(_("Remote Server"));};
    virtual bool Destroy(void);
	//wxGxObjectContainer
	virtual bool AreChildrenViewable(void){return true;};
    //wxGxRemoteServer
    virtual bool Connect(void);
    virtual bool Disconnect(void);
    virtual bool IsConnected(void);
    //events
    virtual void OnGisNetEvent(wxGISNetEvent& event);
 protected:
    bool m_bAuth;
    long m_ConnectionPointNetCookie;
private:
    DECLARE_EVENT_TABLE()
};
