/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxWebConnectionFactoryUI class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2013,2014 Bishop
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
#include "wxgis/catalog/gxwebconnfactory.h"

/** \class wxGxWebConnectionFactoryUI gxwebconnfactoryui.h
    \brief A Web service connection (*.wconn) GxObjectUI factory.
*/

class wxGxWebConnectionFactoryUI :
	public wxGxWebConnectionFactory
{
	DECLARE_DYNAMIC_CLASS(wxGxWebConnectionFactoryUI)
public:
	wxGxWebConnectionFactoryUI(void);
	virtual ~wxGxWebConnectionFactoryUI(void);
	//wxGxDBConnectionFactory
    virtual wxGxObject* GetGxObject(wxGxObject* pParent, const wxString &soName, const CPLString &szPath, bool bCheckNames);
protected:
    wxIcon m_LargeIconConn, m_SmallIconConn;
    wxIcon m_LargeIconDisconn, m_SmallIconDisconn;
    wxIcon m_LargeIconNGWConn, m_SmallIconNGWConn;
    wxIcon m_LargeIconNGWDisconn, m_SmallIconNGWDisconn;
};
