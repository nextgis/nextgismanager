/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxDBConnectionFactory class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011,2013,2014 Dmitry Baryshnikov
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

#include "wxgis/catalog/gxobjectfactory.h"

/** \class wxGxDBConnectionFactory gxdbconnfactory.h
    \brief A DataBase connection (*.xconn) GxObject factory.
*/

class WXDLLIMPEXP_GIS_CLT wxGxDBConnectionFactory :
	public wxGxObjectFactory
{
	DECLARE_DYNAMIC_CLASS(wxGxDBConnectionFactory)
public:
	wxGxDBConnectionFactory(void);
	virtual ~wxGxDBConnectionFactory(void);
	//IGxObjectFactory
	virtual bool GetChildren(wxGxObject* pParent, char** &pFileNames, wxArrayLong & pChildrenIds);
    virtual wxString GetName(void) const {return wxString(_("DataBase connections"));};
    virtual wxGxObject* GetGxObject(wxGxObject* pParent, const wxString &soName, const CPLString &szPath, bool bCheckNames);
    virtual void Serialize(wxXmlNode* const pConfig, bool bStore);
    //wxGxDBConnectionFactory
    virtual bool GetLoadSystemTablesAndShemes(void){return m_bLoadSystemTablesAndSchemes;};
    virtual void SetLoadSystemTablesAndShemes(bool bLoadSystemTablesAndSchemes){m_bLoadSystemTablesAndSchemes = bLoadSystemTablesAndSchemes;};
protected:
    bool m_bHasDriver;
    bool m_bLoadSystemTablesAndSchemes;
};
