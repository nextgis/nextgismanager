/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxRasterFactoryUI class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010,2013 Bishop
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
#include "wxgis/catalog/gxrasterfactory.h"

/** \class wxGxRasterFactoryUI gxrasterfactoryui.h
    \brief A raster GxObject factory.
*/

class wxGxRasterFactoryUI :
	public wxGxRasterFactory
{
	DECLARE_DYNAMIC_CLASS(wxGxRasterFactoryUI)
public:
	wxGxRasterFactoryUI(void);
	virtual ~wxGxRasterFactoryUI(void);
    //wxGxRasterFactory
    virtual wxGxObject* GetGxObject(wxGxObject* pParent, const wxString &soName, const CPLString &szPath, wxGISEnumRasterDatasetType type);
protected:
    wxIcon m_icLargeIcon, m_icSmallIcon;
    wxIcon m_icBMPLargeIcon, m_icBMPSmallIcon;
    wxIcon m_icTIFLargeIcon, m_icTIFSmallIcon;
    wxIcon m_icTILLargeIcon, m_icTILSmallIcon;
    wxIcon m_icIMGLargeIcon, m_icIMGSmallIcon;
    wxIcon m_icJPGLargeIcon, m_icJPGSmallIcon;
    wxIcon m_icPNGLargeIcon, m_icPNGSmallIcon;
    wxIcon m_icGIFLargeIcon, m_icGIFSmallIcon;
    wxIcon m_icSAGALargeIcon, m_icSAGASmallIcon;
    wxIcon m_icVRTLargeIcon, m_icVRTSmallIcon;
};
