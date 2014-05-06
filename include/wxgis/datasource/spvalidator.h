/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISSpatialReferenceValidator class. Return SpatialReference limits
 * Author:   Baryshnikov Dmitry (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011 Bishop
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

#include "wxgis/datasource/datasource.h"
/*
class WXDLLIMPEXP_GIS_DS wxGISSpatialReferenceValidator
{
public:
	wxGISSpatialReferenceValidator(void);
	virtual ~wxGISSpatialReferenceValidator(void);
    virtual bool IsOk(void){return m_bIsOK;}
    virtual LIMITS GetLimits(wxString sProj){return m_DataMap[sProj];};
    virtual bool IsLimitsSet(wxString sProj);
protected:
    virtual bool LoadData(wxString sPath);
    wxString GetPath(wxXmlNode* pNode);
protected:
    bool m_bIsOK;
    wxString m_sPath;
    std::map<wxString, LIMITS> m_DataMap;
};

//use hashmap
//use EPSG ID
//save changes
//may be not in zip but still to save SP to zip should be there
*/