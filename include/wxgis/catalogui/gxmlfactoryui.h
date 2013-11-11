/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxMLFactoryUI class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2011,2013 Bishop
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
#include "wxgis/catalog/gxmlfactory.h"

/** \class wxGxMLFactoryUI gxmlfactory.h
    \brief A markup lang GxObject factory.
*/

class wxGxMLFactoryUI :
	public wxGxMLFactory
{
	DECLARE_DYNAMIC_CLASS(wxGxMLFactoryUI)
public:
	wxGxMLFactoryUI(void);
	virtual ~wxGxMLFactoryUI(void);
	//wxGxMLFactory
    virtual wxGxObject* GetGxObject(wxGxObject* pParent, const wxString &soName, const CPLString &szPath, wxGISEnumVectorDatasetType type);
protected:
    wxIcon m_LargeDXFIcon, m_SmallDXFIcon;
    wxIcon m_LargeKMLIcon, m_SmallKMLIcon;
    wxIcon m_LargeKMZIcon, m_SmallKMZIcon;
    wxIcon m_LargeGMLIcon, m_SmallGMLIcon;
    wxIcon m_LargeSubKMLIcon, m_SmallSubKMLIcon;
    wxIcon m_LargeJsonIcon, m_SmallJsonIcon;
    wxIcon m_LargeSXFIcon, m_SmallSXFIcon, m_LargeSubSXFIcon, m_SmallSubSXFIcon;
};

