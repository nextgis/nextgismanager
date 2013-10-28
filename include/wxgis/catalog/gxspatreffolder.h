/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxSpatialReferencesFolder class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011 Bishop
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

#include "wxgis/catalog/catalog.h"
#include "wxgis/catalog/gxarchfolder.h"

#define GEOGCSSTR _("Geographic Coordinate Systems")
#define PROJCSSTR _("Projected Coordinate Systems")

/** \class wxGxPrjFolder gxspatreffolder.h
    \brief A projections folder root GxObject.
*/
/*
class WXDLLIMPEXP_GIS_CLT wxGxPrjFolder :
	public wxGxArchiveFolder
{
public:
	wxGxPrjFolder(CPLString Path, wxString Name);
	virtual ~wxGxPrjFolder(void);
	//IGxObject
	virtual wxString GetCategory(void){return wxString(_("Coordinate Systems Folder"));};
    //wxGxArchiveFolder
    virtual IGxObject* GetArchiveFolder(CPLString szPath, wxString soName);
};
*/
/** \class wxGxSpatialReferencesFolder gxspatreffolder.h
    \brief A spatial reference root GxObject.
*/
/*
class WXDLLIMPEXP_GIS_CLT wxGxSpatialReferencesFolder :
	public wxGxPrjFolder,
    public IGxRootObjectProperties,
    public wxObject
{
   DECLARE_DYNAMIC_CLASS(wxGxSpatialReferencesFolder)
public:
	wxGxSpatialReferencesFolder(void);//wxString Path, wxString Name, bool bShowHidden
	virtual ~wxGxSpatialReferencesFolder(void);
	//IGxObject
	virtual wxString GetName(void){return wxString(_("Coordinate Systems"));};
    virtual wxString GetBaseName(void){return GetName();};
	virtual CPLString GetInternalName(void){return m_sPath;};
	virtual wxString GetCategory(void){return wxString(_("Coordinate Systems Folder"));};
    //IGxRootObjectProperties
    virtual void Init(wxXmlNode* const pConfigNode);
    virtual void Serialize(wxXmlNode* pConfigNode);
protected:
	wxString m_sInternalPath;
};
*/
