/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxDiscConnectionUI class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2013 Bishop
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

#include "wxgis/catalogui/gxfolderui.h"
#include "wxgis/catalog/gxdiscconnection.h"

/** \class wxGxDiscConnectionUI gxdiscconnectionui.h
    \brief A Disc Connection GxObject.
*/

class WXDLLIMPEXP_GIS_CLU wxGxDiscConnectionUI :
	public wxGxDiscConnection,
    public IGxObjectUI,
    public IGxObjectEditUI,
    public IGxDropTarget,
    public wxThreadHelper,
    public wxGxAutoRenamer
{
    DECLARE_DYNAMIC_CLASS(wxGxDiscConnectionUI)
public:
    wxGxDiscConnectionUI(void);
	wxGxDiscConnectionUI(wxGxObject *oParent, int nStoreId, const wxString &soName = wxEmptyString, const CPLString &soPath = "", const wxIcon &SmallIco = wxNullIcon, const wxIcon &LargeIco = wxNullIcon, const wxIcon &SmallIcoDsbl = wxNullIcon, const wxIcon &LargeIcoDsbl = wxNullIcon);
	virtual ~wxGxDiscConnectionUI(void);
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void) const {return wxString(wxT("wxGxDiscConnection.ContextMenu"));};
	virtual wxString NewMenu(void) const {return wxString(wxT("wxGxDiscConnection.NewMenu"));};
	//IGxObjectEditUI
	virtual void EditProperties(wxWindow *parent);
    //IGxDropTarget
    virtual wxDragResult CanDrop(wxDragResult def);
    virtual bool Drop(const wxArrayString& saGxObjectPaths, bool bMove);
protected:
    bool CheckReadable(void);
    virtual wxThread::ExitCode Entry();
    bool CreateAndRunCheckThread(void);
protected:
	wxIcon m_Conn16, m_Conn48;
	wxIcon m_ConnDsbld16, m_ConnDsbld48;
    char m_nIsReadable;
};

