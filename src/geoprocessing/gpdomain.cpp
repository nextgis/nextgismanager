/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  geoprocessing tool parameters domains.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2012 Bishop
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

#include "wxgis/geoprocessing/gpdomain.h"

//-----------------------------------------------------------------------------
// Class wxGISGPValueDomain
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGISGPValueDomain, wxObject)

wxGISGPValueDomain::wxGISGPValueDomain(void) 
{
	m_bAltered = false;
    m_pDomParet = NULL;
}

wxGISGPValueDomain::~wxGISGPValueDomain(void)
{
}

void wxGISGPValueDomain::AddValue(const wxVariant& Element, const wxString &soNameStr)
{
	m_asoData.push_back(Element);
	m_asoNames.Add(soNameStr);
    m_bAltered = true;
    if(m_pDomParet)
        m_pDomParet->OnValueAdded(Element, soNameStr);
}

size_t wxGISGPValueDomain::GetCount(void) const
{
	return m_asoData.size(); 
}

wxVariant wxGISGPValueDomain::GetValue(size_t nIndex) const
{ 
	wxASSERT(nIndex < m_asoData.size());
	return m_asoData[nIndex]; 
}

wxString wxGISGPValueDomain::GetName(size_t nIndex) const
{ 
	return m_asoNames[nIndex]; 
}

void wxGISGPValueDomain::Clear(void)
{
	m_asoData.clear();
	m_asoNames.Empty();
	m_bAltered = true;
    if(m_pDomParet)
        m_pDomParet->OnCleared();
}

wxVariant wxGISGPValueDomain::GetValueByName(const wxString &soNameStr) const
{
	int nPos = m_asoNames.Index(soNameStr);
	wxVariant result;
	if(nPos != wxNOT_FOUND)
		result = m_asoData[nPos];
	return result;
}

int wxGISGPValueDomain::GetPosByName(const wxString &sName) const
{
	return m_asoNames.Index(sName);
}

int wxGISGPValueDomain::GetPosByValue(const wxVariant &oVal) const
{
	int nPos = wxNOT_FOUND;
	for(size_t i = 0; i < m_asoData.size(); ++i)
	{
		if(m_asoData[i] == oVal)
		{
			nPos = i;
			break;
		}
	}
	return nPos;
}

//-----------------------------------------------------------------------------
// Class wxGISGPGxObjectDomain
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGISGPGxObjectDomain, wxGISGPValueDomain)

wxGISGPGxObjectDomain::wxGISGPGxObjectDomain(void) : wxGISGPValueDomain()
{
}

wxGISGPGxObjectDomain::~wxGISGPGxObjectDomain(void)
{
    for(size_t i = 0; i < m_asoData.size(); ++i)
	{
		wxGxObjectFilter* pFil = (wxGxObjectFilter*)m_asoData[i].GetVoidPtr();
		wxDELETE(pFil);
	}
}

void wxGISGPGxObjectDomain::AddFilter(wxGxObjectFilter* pFilter)
{
	AddValue(wxVariant((void*)pFilter), pFilter->GetName());
}

wxGxObjectFilter* const wxGISGPGxObjectDomain::GetFilter(size_t nIndex) const
{
	return (wxGxObjectFilter*)GetValue(nIndex).GetVoidPtr();
}

int wxGISGPGxObjectDomain::GetPosByValue(const wxVariant &oVal) const
{
    if(!oVal.IsNull())
    {
		wxFileName oName(oVal.GetString());
        for(size_t i = 0; i < m_asoData.size(); ++i)
        {
            wxGxObjectFilter* poFilter = GetFilter(i);
            if(poFilter)
            {
                if( oName.GetExt().IsSameAs(poFilter->GetExt(), false) || poFilter->GetExt().IsEmpty() )
                {
                    return i;
                }
            }
        }
    }
	return wxNOT_FOUND;
}

//-----------------------------------------------------------------------------
// Class wxGISGPStringDomain
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGISGPStringDomain, wxGISGPValueDomain)

wxGISGPStringDomain::wxGISGPStringDomain(void) : wxGISGPValueDomain()
{
}

wxGISGPStringDomain::~wxGISGPStringDomain(void)
{
}

void wxGISGPStringDomain::AddString(const wxString &soStr, const wxString &soName)
{
	if(soName.IsEmpty())
		AddValue(wxVariant(soStr), soStr);
	else
		AddValue(wxVariant(soStr), soName);
}

wxString wxGISGPStringDomain::GetString(size_t nIndex) const 
{
	return GetValue(nIndex).GetString();
}


