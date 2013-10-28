/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  wxGxRemoteServerUI class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2012 Bishop
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

#include "wxgis/remoteserverui/remoteserverui.h"
#include "wxgis/remoteserver/gxremoteserver.h"
#include "wxgis/catalogui/catalogui.h"

/** \class wxGxRemoteServerUI gxremoteserverui.h
    \brief A Remote Server UI GxObject.
*/
class WXDLLIMPEXP_GIS_RSU wxGxRemoteServerUI :
	public wxGxRemoteServer,
	public IGxObjectUI,
	public IGxObjectWizard
{
    DECLARE_CLASS(wxGxRemoteServerUI)
public:
    wxGxRemoteServerUI(void);
	wxGxRemoteServerUI(wxGISNetClientConnection* const pNetConn, wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "", const wxIcon &SmallIco = wxNullIcon, const wxIcon &LargeIco = wxNullIcon, const wxIcon &SmalDsbIco = wxNullIcon, const wxIcon &LargeDsbIco = wxNullIcon, const wxIcon &SmallAuthIco = wxNullIcon, const wxIcon &LargeAuthIco = wxNullIcon);
	virtual ~wxGxRemoteServerUI(void);
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void) const {return wxString(wxT("wxGxRemoteServerUI.ContextMenu"));};
	virtual wxString NewMenu(void) const {return wxEmptyString;};
    //IGxObjectWizard
    virtual bool Invoke(wxWindow* pParentWnd);
    //wxGxRemoteServer
    virtual bool Connect(void);
    virtual bool Disconnect(void);
protected:    
    //wxGxRemoteServer events
    virtual void OnGisNetEvent(wxGISNetEvent& event);
protected:
    wxIcon m_SmallIcon, m_LargeIcon;
    wxIcon m_SmallDsblIcon, m_LargeDsblIcon;
    wxIcon m_SmallAuthIcon, m_LargeAuthIcon;
    long m_nGxPendingId;
};
