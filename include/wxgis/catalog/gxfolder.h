/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxFolder class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011-2013 Bishop
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
#include "wxgis/catalog/gxobject.h"

#include <wx/dir.h>

/** \class wxGxFolder gxfolder.h
    \brief The Folder GxObject.
*/

class WXDLLIMPEXP_GIS_CLT wxGxFolder :
	public wxGxObjectContainer,
	public IGxObjectEdit
{
    DECLARE_CLASS(wxGxFolder)
public:
    wxGxFolder(void);
	wxGxFolder(wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
	virtual ~wxGxFolder(void);
	//wxGxObject
    virtual wxString GetBaseName(void) const;
	virtual wxString GetCategory(void) const {return wxString(_("Folder"));};
	virtual void Refresh(void);
	//IGxObjectEdit
	virtual bool Delete(void);
	virtual bool CanDelete(void);
	virtual bool Rename(const wxString &sNewName);
	virtual bool CanRename(void);
    virtual bool Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel);
	virtual bool CanCopy(const CPLString &szDestPath);
	virtual bool Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel);
	virtual bool CanMove(const CPLString &szDestPath);
	//wxGxObjectContainer
	virtual bool AreChildrenViewable(void) const;
	virtual bool HasChildren(void);
    virtual bool CanCreate(long nDataType, long DataSubtype);
protected:
	//wxGxFolder
	virtual void LoadChildren(void);    
protected:
	bool m_bIsChildrenLoaded;
};
