/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxLocalDBFactoryUI class. Create new local databases GxObjectsUI
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

#include "wxgis/catalogui/catalogui.h"
#include "wxgis/catalog/gxlocaldbfactory.h"

/** @class wxGxLocalDBFactoryUI
    
    A local database (i.e. ESRI File GDB, spatialite db, ESRI personal GDB, geopackage, etc.) objects ui factory.

    @library{catalogui}
*/

class WXDLLIMPEXP_GIS_CLU wxGxLocalDBFactoryUI :
    public wxGxLocalDBFactory
{
    DECLARE_DYNAMIC_CLASS(wxGxLocalDBFactoryUI)
public:
    wxGxLocalDBFactoryUI(void);
    virtual ~wxGxLocalDBFactoryUI(void);
	//wxGxObjectFactory
    virtual wxGxObject* GetGxObject(wxGxObject* pParent, const wxString &soName, const CPLString &szPath, wxGISEnumContainerType eType, bool bCheckNames);
protected:
    wxIcon m_oLargeFolderIcon, m_oSmallFolderIcon;
};
