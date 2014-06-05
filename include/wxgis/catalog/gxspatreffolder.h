/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxSpatialReferencesFolder class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011 Dmitry Baryshnikov
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

#include "wxgis/catalog/catalog.h"
#include "wxgis/catalog/gxarchfolder.h"
#include "wxgis/catalog/gxcatalog.h"

#define GEOGCSSTR _("Geographic Coordinate Systems")
#define PROJCSSTR _("Projected Coordinate Systems")

/** @class wxGxPrjFolder
    
    A projections folder root GxObject.

    @library{catalog}
*/


class WXDLLIMPEXP_GIS_CLT wxGxPrjFolder :
	public wxGxArchiveFolder
{
    DECLARE_CLASS(wxGxPrjFolder)
public:
    wxGxPrjFolder();
    wxGxPrjFolder(wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
	virtual ~wxGxPrjFolder(void);
	//wxGxObject
	virtual wxString GetCategory(void) const {return wxString(_("Coordinate Systems Folder"));};
protected:
    //wxGxArchiveFolder
    virtual wxGxObject* GetArchiveFolder(wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
};

/** @class wxGxSpatialReferencesFolder

    A spatial reference root GxObject.

    @library{catalog}
*/

class WXDLLIMPEXP_GIS_CLT wxGxSpatialReferencesFolder :
	public wxGxPrjFolder,
    public IGxRootObjectProperties
{
   DECLARE_DYNAMIC_CLASS(wxGxSpatialReferencesFolder)
public:
	wxGxSpatialReferencesFolder(void);//wxString Path, wxString Name, bool bShowHidden
	virtual ~wxGxSpatialReferencesFolder(void);
	//wxGxObject
	virtual wxString GetName(void) const {return wxString(_("Coordinate Systems"));};
	virtual wxString GetCategory(void) const {return wxString(_("Coordinate Systems Folder"));};
    virtual bool Create(wxGxObject *oParent = NULL, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
    //IGxRootObjectProperties
    virtual void Init(wxXmlNode* const pConfigNode);
    virtual void Serialize(wxXmlNode* const pConfigNode);
protected:
    virtual bool IsArchive(void) const;
protected:
    wxString m_sInternalPath;
    wxGxCatalog* m_pCatalog;
    bool m_bIsArchive;
};

