/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxShapeFactory class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2014 Dmitry Baryshnikov
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
#include "wxgis/datasource/datasource.h"

/** @class wxGxGNMFactory

    A Geography network model (GNM) GxObject factory.

    @library{catalog}
*/

#if GDAL_VERSION_NUM >= 2000000

class WXDLLIMPEXP_GIS_CLT wxGxGNMFactory :
	public wxGxObjectFactory
{
    DECLARE_DYNAMIC_CLASS(wxGxGNMFactory)
public:
    wxGxGNMFactory(void);
    virtual ~wxGxGNMFactory(void);
	//IGxObjectFactory
	virtual bool GetChildren(wxGxObject* pParent, char** &pFileNames, wxArrayLong & pChildrenIds);
    virtual wxString GetName(void) const {return wxString(_("Geography network models"));};
    //wxGxShapeFactory
    virtual wxGxObject* GetGxObject(wxGxObject* pParent, const wxString &soName, const CPLString &szPath, bool bCheckNames);
protected:
    bool m_bHasDriver;
};
#endif