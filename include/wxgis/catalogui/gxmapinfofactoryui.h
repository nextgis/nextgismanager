/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxMapInfoFactoryUI class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2011,2014 Bishop
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

#include "wxgis/catalogui/catalogui.h"
#include "wxgis/catalog/gxmapinfofactory.h"

/** \class wxGxMapInfoFactoryUI gxmapinfofactoryui.h
    \brief A MapInfo dataset GxObjectUI factory.
*/

class wxGxMapInfoFactoryUI :
	public wxGxMapInfoFactory
{
	DECLARE_DYNAMIC_CLASS(wxGxMapInfoFactoryUI)
public:
	wxGxMapInfoFactoryUI(void);
	virtual ~wxGxMapInfoFactoryUI(void);
    //wxGxMapInfoFactory
    virtual wxGxObject* GetGxObject(wxGxObject* pParent, const wxString &soName, const CPLString &szPath, wxGISEnumVectorDatasetType type, bool bCheckNames);
protected:
    wxIcon m_LargeTabIcon, m_SmallTabIcon;
    wxIcon m_LargeMifIcon, m_SmallMifIcon;
    wxIcon m_LargeTabTIcon, m_SmallTabTIcon;
};

