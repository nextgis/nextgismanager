/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxFileFactoryUI class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2011 Bishop
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
#include "wxgis/catalog/gxfilefactory.h"

/** \class wxGxCSVFileFactoryUI gxcsvfactoryui.h
    \brief A GxCSVFile UI object factory.
*/

class wxGxFileFactoryUI :
	public wxGxFileFactory
{
	DECLARE_DYNAMIC_CLASS(wxGxFileFactoryUI)
public:
	wxGxFileFactoryUI(void);
	virtual ~wxGxFileFactoryUI(void);
    //wxGxFileFactory
    virtual wxGxObject* GetGxObject(wxGxObject* pParent, const wxString &soName, const CPLString &szPath);
//protected:
//    wxIcon m_LargePRJIcon, m_SmallPRJIcon;
};
