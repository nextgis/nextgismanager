/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxArchiveFactoryUI class. Create new GxFolderUI objects
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011,2013 Bishop
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
#include "wxgis/catalog/gxarchivefactory.h"

/** \class wxGxArchiveFactoryUI gxarchivefactoryui.h
    \brief A GxArchiveUI factory.
*/

class WXDLLIMPEXP_GIS_CLU wxGxArchiveFactoryUI :
	public wxGxArchiveFactory
{
	DECLARE_DYNAMIC_CLASS(wxGxArchiveFactoryUI)
public:
	wxGxArchiveFactoryUI(void);
	virtual ~wxGxArchiveFactoryUI(void);
	//IGxObjectFactory
    virtual wxGxObject* GetGxObject(wxGxObject* pParent, const wxString &soName, const CPLString &szPath, bool bCheckNames);
protected:
    wxIcon m_oLargeAFolderIcon, m_oSmallAFolderIcon;
};
