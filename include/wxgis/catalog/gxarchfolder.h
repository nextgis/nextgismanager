/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxArchive classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011,2013 Bishop
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
#include "wxgis/catalog/gxfolder.h"

/** \class wxGxArchiveFolder gxarchfolder.h
    \brief The Archive Folder GxObject.
*/

class WXDLLIMPEXP_GIS_CLT wxGxArchiveFolder :
	public wxGxFolder
{
    DECLARE_CLASS(wxGxArchiveFolder)
public:
    wxGxArchiveFolder(void);
	wxGxArchiveFolder(wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
	virtual ~wxGxArchiveFolder(void);
	//wxGxObject
	virtual wxString GetCategory(void) const {return wxString(_("Archive folder"));};
	//IGxObjectEdit unsupported yet
	virtual bool CanDelete(void){return false;};
	virtual bool CanRename(void){return false;};
    //wxGxArchiveFolder
protected:
    virtual wxGxObject* GetArchiveFolder(wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
	virtual void LoadChildren(void);
    virtual bool IsArchive(void) const;
};

/** \class wxGxArchive gxarchfolder.h
    \brief The Archive GxObject.
*/

class WXDLLIMPEXP_GIS_CLT wxGxArchive :
	public wxGxArchiveFolder
{
    DECLARE_CLASS(wxGxArchive)
public:
	wxGxArchive(wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
	virtual ~wxGxArchive(void);
	//IGxObject
    virtual wxString GetBaseName(void) const;
	virtual wxString GetCategory(void) const {return wxString(_("Archive"));};
	//IGxObjectEdit
	virtual bool Delete(void);
	virtual bool CanDelete(void){return true;};
	virtual bool Rename(const wxString &sNewName);
	virtual bool CanRename(void){return true;};
};


