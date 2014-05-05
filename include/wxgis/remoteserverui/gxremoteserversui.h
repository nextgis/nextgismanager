/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  wxGxRemoteServersUI class.
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
#include "wxgis/catalogui/catalogui.h"
//#include "wxgis/remoteserverui/remoteserverui.h"
#include "wxgis/remoteserver/gxremoteservers.h"

/** \class wxGxRemoteServersUI gxremoteserversui.h
    \brief A Remote Servers UI GxRootObject.
*/
class WXDLLIMPEXP_GIS_RSU wxGxRemoteServersUI :
    public wxGxRemoteServers,
	public IGxObjectUI
{
   DECLARE_DYNAMIC_CLASS(wxGxRemoteServersUI)
public:
	wxGxRemoteServersUI(void);
	virtual ~wxGxRemoteServersUI(void);
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void) const {return wxString(wxT("wxGxRemoteServersUI.ContextMenu"));};
	virtual wxString NewMenu(void) const {return wxString(wxT("wxGxRemoteServersUI.NewMenu"));};
	//wxGxRemoteServersUI
    virtual void CreateConnection(wxWindow* pParent);
	/*virtual void LoadChildren();
	virtual void EmptyChildren(void);
    */
protected:
    //wxGxXMLConnectionStorage
    virtual wxGxObject* CreateChildGxObject(const wxXmlNode* pNode);
protected:
	wxIcon m_RemServs16, m_RemServs48;
	wxIcon m_RemServ16, m_RemServ48;
    wxIcon m_RemServDsbl16, m_RemServDsbl48;
    wxIcon m_RemServAuth16, m_RemServAuth48;
};
