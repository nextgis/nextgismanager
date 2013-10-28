/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxSpatialReferencesFolderUI classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2011 Bishop
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

#include "wxgis/catalog/gxspatreffolder.h"
#include "wxgis/catalogui/catalogui.h"
#include "wxgis/catalogui/gxarchfolderui.h"

/** \class wxGxSpatialReferencesFolderUI gxspatreffolderui.h
    \brief A spatial reference root GxObject.
*/
/*
class WXDLLIMPEXP_GIS_CLU wxGxSpatialReferencesFolderUI :
    public wxGxSpatialReferencesFolder,
	public IGxObjectUI
{
   DECLARE_DYNAMIC_CLASS(wxGxSpatialReferencesFolderUI)
public:
	wxGxSpatialReferencesFolderUI(void);//wxString Path, wxString Name, bool bShowHidden
	virtual ~wxGxSpatialReferencesFolderUI(void);
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void){return wxString(wxT("wxGxSpatialReferencesFolder.ContextMenu"));};
	virtual wxString NewMenu(void){return wxString(wxT("wxGxSpatialReferencesFolder.NewMenu"));};
    //wxGxArchiveFolder
    virtual IGxObject* GetArchiveFolder(CPLString szPath, wxString soName);
    virtual void EmptyChildren(void);
protected:
    wxIcon m_LargeIcon, m_SmallIcon;
};
*/
/** \class wxGxPrjFolderUI gxspatreffolderui.h
    \brief A projections folder root GxObject.
*/
/*
class WXDLLIMPEXP_GIS_CLU wxGxPrjFolderUI :
	public wxGxArchiveFolderUI
{
public:
	wxGxPrjFolderUI(CPLString Path, wxString Name, wxIcon LargeIcon = wxNullIcon, wxIcon SmallIcon = wxNullIcon);
	virtual ~wxGxPrjFolderUI(void);
	//IGxObject
	virtual wxString GetCategory(void){return wxString(_("Coordinate Systems Folder"));};
    //IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
    //wxGxArchiveFolder
    virtual IGxObject* GetArchiveFolder(CPLString szPath, wxString soName);
protected:
    wxIcon m_LargeIcon, m_SmallIcon;
};
*/
