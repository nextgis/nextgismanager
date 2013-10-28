/******************************************************************************
 * Project:  wxGIS
 * Purpose:  Cursor class.
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

#include "wxgis/datasource/cursor.h"

//----------------------------------------------------------------------------
// wxFeatureCursor
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxFeatureCursor, wxObject);

wxFeatureCursor::wxFeatureCursor(void)
{
    m_refData = new wxFeatureCursorRefData();
}

wxFeatureCursor::~wxFeatureCursor()
{
}

wxObjectRefData *wxFeatureCursor::CreateRefData() const
{
    return new wxFeatureCursorRefData();
}

wxObjectRefData *wxFeatureCursor::CloneRefData(const wxObjectRefData *data) const
{
    return new wxFeatureCursorRefData(*(wxFeatureCursorRefData *)data);
}

bool wxFeatureCursor::IsOk() const
{ 
    return m_refData != NULL; 
}

bool wxFeatureCursor::operator == ( const wxFeatureCursor& obj ) const
{
    if (m_refData == obj.m_refData)
        return true;
    if (!m_refData || !obj.m_refData)
        return false;

    return ( *(wxFeatureCursorRefData*)m_refData == *(wxFeatureCursorRefData*)obj.m_refData );
}

void wxFeatureCursor::Reset()
{
    wxFeatureCursorRefData* pRefData = dynamic_cast<wxFeatureCursorRefData*>(m_refData);
    if( pRefData )
    {
        m_Iterator = pRefData->Begin();
    }
}

void wxFeatureCursor::Add(wxGISFeature Feature)
{
	((wxFeatureCursorRefData *)m_refData)->m_olFeatures.push_back(Feature);
}

wxGISFeature wxFeatureCursor::Next()
{
	if(((wxFeatureCursorRefData *)m_refData)->m_olFeatures.empty() || m_Iterator == ((wxFeatureCursorRefData *)m_refData)->m_olFeatures.end())
		return wxGISFeature();
	wxGISFeature Feature = *m_Iterator;
	++m_Iterator;
	return Feature;
}

void wxFeatureCursor::Clear()
{
	((wxFeatureCursorRefData *)m_refData)->m_olFeatures.clear();
}

size_t wxFeatureCursor::GetCount() const
{
    return ((wxFeatureCursorRefData *)m_refData)->m_olFeatures.size();
}
