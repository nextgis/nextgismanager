/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxWebConnections class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2013 Dmitry Baryshnikov
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
#include "wxgis/catalog/gxfolder.h"

/** \class wxGxWebConnections gxwebconnections.h
    \brief The web services connections root item.

	This root item can held connections (*.wconn) and folders items
*/

class WXDLLIMPEXP_GIS_CLT wxGxWebConnections :
	public wxGxFolder,
    public IGxRootObjectProperties
{
   DECLARE_DYNAMIC_CLASS(wxGxWebConnections)
public:
	wxGxWebConnections(void);
	virtual ~wxGxWebConnections(void);
	//wxGxObject
    virtual bool Create(wxGxObject *oParent = NULL, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
	virtual wxString GetCategory(void) const {return wxString(_("Web services folder"));};
	//wxGxObjectContainer
    virtual bool CanCreate(long nDataType, long DataSubtype);     
    //wxGxObjectContainer
    virtual bool AreChildrenViewable(void) const {return true;};
    //IGxRootObjectProperties
    virtual void Init(wxXmlNode* const pConfigNode);
    virtual void Serialize(wxXmlNode* const pConfigNode);
//protected:
//    virtual void LoadChildren(void);
protected:
    wxString m_sInternalPath;
};


