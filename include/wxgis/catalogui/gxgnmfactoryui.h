/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxShapeFactoryUI class.
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

#include "wxgis/catalogui/catalogui.h"
#include "wxgis/catalog/gxgnmfactory.h"

#ifdef wxGIS_USE_GDAL_GNM

/** @class wxGxGNMFactoryUI

    A geography network GxObject factory.

    @library{catalogui}
*/

class wxGxGNMFactoryUI :
    public wxGxGNMFactory
{
    DECLARE_DYNAMIC_CLASS(wxGxGNMFactoryUI)
public:
    wxGxGNMFactoryUI(void);
    virtual ~wxGxGNMFactoryUI(void);
    //wxGxShapeFactory
    virtual wxGxObject* GetGxObject(wxGxObject* pParent, const wxString &soName, const CPLString &szPath, bool bCheckNames);
protected:
    wxIcon m_LargeGNMIcon, m_SmallGNMIcon;
};

#endif
