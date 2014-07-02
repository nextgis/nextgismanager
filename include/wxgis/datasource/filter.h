/******************************************************************************
 * Project:  wxGIS
 * Purpose:  Filter classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011 Dmitry Baryshnikov
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

#include "wxgis/datasource/vectorop.h"

/** \class wxGISQueryFilter filter.h
    \brief Attributes query filter.
*/

class WXDLLIMPEXP_GIS_DS wxGISQueryFilter
{
public:
    wxGISQueryFilter(void);
    wxGISQueryFilter(const wxString &sWhereClause);
    wxGISQueryFilter& operator = ( const wxGISQueryFilter& obj );
    wxGISQueryFilter( const wxGISQueryFilter& obj );
	virtual ~wxGISQueryFilter(void);
    virtual void SetWhereClause(wxString sWhereClause);
    virtual wxString GetWhereClause(void) const;
protected:
    wxString m_sWhereClause;
};

extern WXDLLIMPEXP_DATA_GIS_DS(wxGISQueryFilter) wxGISNullQueryFilter;

/** \class wxGISQueryFilter filter.h
    \brief Spatial (by geometry) query filter.
*/

class WXDLLIMPEXP_GIS_DS wxGISSpatialFilter :
	public wxGISQueryFilter
{
public:
	wxGISSpatialFilter(const wxGISGeometry &Geom, const wxString &sWhereClause = wxEmptyString);
	wxGISSpatialFilter(void);
    wxGISSpatialFilter& operator = ( const wxGISSpatialFilter& obj );
    wxGISSpatialFilter( const wxGISSpatialFilter& obj );
	virtual ~wxGISSpatialFilter(void);
	virtual void SetEnvelope(double dfMinX, double dfMinY, double dfMaxX, double dfMaxY);
	virtual void SetEnvelope(const OGREnvelope &Env);
	virtual void SetGeometry(const wxGISGeometry &Geom);
	virtual OGREnvelope GetEnvelope(void);
	virtual wxGISGeometry GetGeometry(void) const;
protected:
	wxGISGeometry m_Geom;
};

extern WXDLLIMPEXP_DATA_GIS_DS(wxGISSpatialFilter) wxGISNullSpatialFilter;
