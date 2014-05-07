/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxLocalDBFactory class. Create new local databases GxObjects
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2014 Bishop
*   Copyright (C) 2014 NextGIS
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

/** @class wxGxLocalDBFactory

    A local database (i.e. ESRI File GDB, spatialite db, ESRI personal GDB, geopackage, etc.) objects factory.

    @library{catalog}
*/
class WXDLLIMPEXP_GIS_CLT wxGxLocalDBFactory :
    public wxGxObjectFactory
{
    DECLARE_DYNAMIC_CLASS(wxGxLocalDBFactory)
public:
    wxGxLocalDBFactory(void);
    virtual ~wxGxLocalDBFactory(void);
	//wxGxObjectFactory
	virtual bool GetChildren(wxGxObject* pParent, char** &pFileNames, wxArrayLong & pChildrenIds);
    virtual wxString GetName(void) const {return wxString(_("File databases"));};
    virtual wxGxObject* GetGxObject(wxGxObject* pParent, const wxString &soName, const CPLString &szPath, wxGISEnumContainerType eType, bool bCheckNames);
protected:
    bool m_bHasOFGDBDriver;
};
