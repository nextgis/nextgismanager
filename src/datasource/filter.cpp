/******************************************************************************
 * Project:  wxGIS
 * Purpose:  Filter classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011 Bishop
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
#include "wxgis/datasource/filter.h"

//----------------------------------------------------------------------------
// wxGISQueryFilter
//----------------------------------------------------------------------------
wxGISQueryFilter wxGISNullQueryFilter;

wxGISQueryFilter::wxGISQueryFilter(void)
{
}
    
wxGISQueryFilter::wxGISQueryFilter(const wxString &sWhereClause)
{
	m_sWhereClause = sWhereClause;
}

wxGISQueryFilter::wxGISQueryFilter( const wxGISQueryFilter& obj )
{
   m_sWhereClause = obj.m_sWhereClause;
}

wxGISQueryFilter::~wxGISQueryFilter(void)
{
}

void wxGISQueryFilter::SetWhereClause(wxString sWhereClause)
{
	m_sWhereClause = sWhereClause;
}
    
wxString wxGISQueryFilter::GetWhereClause(void) const
{
	return m_sWhereClause;
}

wxGISQueryFilter& wxGISQueryFilter::operator = ( const wxGISQueryFilter& obj )
{
    m_sWhereClause = obj.m_sWhereClause;
    return *this;
}

//----------------------------------------------------------------------------
// wxGISSpatialFilter
//----------------------------------------------------------------------------
wxGISSpatialFilter wxGISNullSpatialFilter;

wxGISSpatialFilter::wxGISSpatialFilter(const wxGISGeometry &Geom, const wxString &sWhereClause) : wxGISQueryFilter(sWhereClause)
{
	m_Geom = Geom;
}
	
wxGISSpatialFilter::wxGISSpatialFilter() : wxGISQueryFilter()
{
}

wxGISSpatialFilter::~wxGISSpatialFilter()
{
}

wxGISSpatialFilter& wxGISSpatialFilter::operator = ( const wxGISSpatialFilter& obj )
{
    m_Geom = obj.m_Geom;
    m_sWhereClause = obj.m_sWhereClause;
    return *this;
}

wxGISSpatialFilter::wxGISSpatialFilter( const wxGISSpatialFilter& obj )
{
    m_Geom = obj.m_Geom;
    m_sWhereClause = obj.m_sWhereClause;
}

void wxGISSpatialFilter::SetEnvelope(double dfMinX, double dfMinY, double dfMaxX, double dfMaxY)
{
	OGREnvelope Envelope;
	Envelope.MinX = dfMinX;
	Envelope.MaxX = dfMaxX;
	Envelope.MinY = dfMinY;
	Envelope.MaxY = dfMaxY;
	m_Geom = EnvelopeToGeometry(Envelope);
}

void wxGISSpatialFilter::SetEnvelope(const OGREnvelope &Env)
{
	m_Geom = EnvelopeToGeometry(Env);
}

void wxGISSpatialFilter::SetGeometry(const wxGISGeometry &Geom)
{
	m_Geom = Geom;
}
	
OGREnvelope wxGISSpatialFilter::GetEnvelope(void)
{
	if(m_Geom.IsOk())
	{
		return m_Geom.GetEnvelope();
	}
	else
    {
		return OGREnvelope();
    }
}
	
wxGISGeometry wxGISSpatialFilter::GetGeometry(void) const
{
	return m_Geom;
}
