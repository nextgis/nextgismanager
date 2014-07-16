/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxFile classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2011,2013,2014 Dmitry Baryshnikov
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
#include "wxgis/catalog/gxobject.h"
#include "wxgis/datasource/datasource.h"
#include "wxgis/datasource/gdalinh.h"

/** @class wxGxFile

    A file GxObject.

    @library{catalog}
*/

class WXDLLIMPEXP_GIS_CLT wxGxFile :
	public wxGxObject,
    public IGxObjectEdit
{
    DECLARE_CLASS(wxGxFile)
public:
	wxGxFile(wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
	virtual ~wxGxFile(void);
	//IGxObjectEdit
	virtual bool Delete(void);
    virtual bool CanDelete(void) { return true; };
	virtual bool Rename(const wxString& NewName);
    virtual bool CanRename(void) { return true; };
	virtual bool Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel);
    virtual bool CanCopy(const CPLString &szDestPath) { return true; };
	virtual bool Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel);
	virtual bool CanMove(const CPLString &szDestPath) {return CanCopy(szDestPath) & CanDelete();};
};

/** @class wxGxPrjFile

    A proj file GxObject.

    @library{catalog}
*/

class WXDLLIMPEXP_GIS_CLT wxGxPrjFile :
    public wxGxFile
{
    DECLARE_CLASS(wxGxPrjFile)
public:
	wxGxPrjFile(wxGISEnumPrjFileType eType, wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
	virtual ~wxGxPrjFile(void);
	//wxGxObject
	virtual wxString GetCategory(void) const {return wxString(_("Coordinate System"));};
	//wxGxPrjFile
	virtual wxGISSpatialReference GetSpatialReference(void);
protected:
    wxGISEnumPrjFileType m_eType;
	wxGISSpatialReference m_SpatialReference;
};

/** @class wxGxTextFile

    A text file GxObject.

    @library{catalog}
*/

class WXDLLIMPEXP_GIS_CLT wxGxTextFile :
    public wxGxFile
{
    DECLARE_CLASS(wxGxTextFile)
public:
	wxGxTextFile(wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
	virtual ~wxGxTextFile(void);
	//wxGxObject
	virtual wxString GetCategory(void) const {return wxString(_("Text file"));};
};
