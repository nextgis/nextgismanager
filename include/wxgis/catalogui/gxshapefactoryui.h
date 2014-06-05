/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxShapeFactoryUI class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010,2012,2014 Dmitry Baryshnikov
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
#include "wxgis/catalog/gxshapefactory.h"

/** \class wxGxShapeFactoryUI gxshapefactoryui.h
    \brief A shape file GxObject factory.
*/

class wxGxShapeFactoryUI :
	public wxGxShapeFactory
{
	DECLARE_DYNAMIC_CLASS(wxGxShapeFactoryUI)
public:
	wxGxShapeFactoryUI(void);
	virtual ~wxGxShapeFactoryUI(void);
    //wxGxShapeFactory
    virtual wxGxObject* GetGxObject(wxGxObject* pParent, const wxString &soName, const CPLString &szPath, wxGISEnumDatasetType type, bool bCheckNames);
protected:
    wxIcon m_LargeSHPIcon, m_SmallSHPIcon;
    wxIcon m_LargeDBFIcon, m_SmallDBFIcon;
};

