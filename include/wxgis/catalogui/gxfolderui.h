/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxFolderUI class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2013 Bishop
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
#include "wxgis/catalogui/catalogui.h"
#include "wxgis/catalog/gxfolder.h"
#include "wxgis/catalogui/gxview.h"

/** \class wxGxFolderUI gxfolderui.h
    \brief A folder GxObject.
*/

class WXDLLIMPEXP_GIS_CLU wxGxFolderUI :
    public wxGxFolder,
	public IGxObjectUI,
	public IGxObjectEditUI,
    public IGxDropTarget,
    public wxGxAutoRenamer
{
    DECLARE_CLASS(wxGxFolderUI)
public:
	wxGxFolderUI(wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "", const wxIcon & LargeIcon = wxNullIcon, const wxIcon & SmallIcon = wxNullIcon);
	virtual ~wxGxFolderUI(void);
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void) const {return wxString(wxT("wxGxFolder.ContextMenu"));};
	virtual wxString NewMenu(void) const {return wxString(wxT("wxGxFolder.NewMenu"));};
	//IGxObjectEditUI
	virtual void EditProperties(wxWindow *parent);
    //IGxDropTarget
    virtual wxDragResult CanDrop(wxDragResult def);
    virtual bool Drop(const wxArrayString& saGxObjectPaths, bool bMove);
protected:
    wxIcon m_oLargeIcon, m_oSmallIcon;
};
