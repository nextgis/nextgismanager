/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  geoprocessing tool parameters domains.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2012 Dmitry Baryshnikov
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

#include "wxgis/geoprocessing/geoprocessing.h"
#include "wxgis/catalog/catalog.h"
#include "wxgis/catalog/gxfilters.h"
#include "wxgis/datasource/sysop.h"

/** @class IGISGPDomainParent

    An abstract class. Each class whish need listen of values domain changes should inherited it

    @library{gp}
*/

class IGISGPDomainParent
{
public:
    /** Destructor */
    virtual ~IGISGPDomainParent(void){};
    /**
        Fires then value added to the domain
        @param Value
            The added value to the domain

        @param sName
            The name for the added value. This name will be used in lists or choices for indicating this value.
    */
    virtual void OnValueAdded(const wxVariant &Value, const wxString &sName) = 0;
    /** Fires then value domain cleared */
    virtual void OnCleared(void) = 0;
};



/** @class wxGISGPValueDomain

    The domain storing variant values

    @library{gp}
*/

class WXDLLIMPEXP_GIS_GP wxGISGPValueDomain :
    public wxObject,
    public wxGISConnectionPointContainer
{
    DECLARE_CLASS(wxGISGPValueDomain)
public:
	wxGISGPValueDomain(void);
	virtual ~wxGISGPValueDomain (void);
	virtual void AddValue(const wxVariant& Element, const wxString &soNameStr);
	virtual size_t GetCount(void) const;
	virtual wxVariant GetValue(size_t nIndex) const;
	virtual wxString GetName(size_t nIndex) const;
    virtual void Clear(void);
	virtual bool GetAltered(void) const {return m_bAltered;};
	virtual void SetAltered(bool bAltered){m_bAltered = bAltered;};
	//
    virtual wxVariant GetValueByName(const wxString &soNameStr) const;
	virtual int GetPosByName(const wxString &sName) const;
	virtual int GetPosByValue(const wxVariant &oVal) const;
    //
    virtual void SetParent(IGISGPDomainParent* const pDomParet){m_pDomParet = pDomParet;};
protected:
	wxArrayString m_asoNames;
	wxVector<wxVariant> m_asoData;
	bool m_bAltered;
    IGISGPDomainParent* m_pDomParet;
};

/** \class wxGISGPGxObjectDomain gpdomain.h
    \brief The domain storing GxObjectFilters
*/

class WXDLLIMPEXP_GIS_GP wxGISGPGxObjectDomain : public wxGISGPValueDomain
{
    DECLARE_CLASS(wxGISGPGxObjectDomain)
public:
    wxGISGPGxObjectDomain (void);
    virtual ~wxGISGPGxObjectDomain (void);
    virtual wxGxObjectFilter* const GetFilter(size_t nIndex) const;
	virtual void AddFilter(wxGxObjectFilter* pFilter);
	virtual int GetPosByValue(const wxVariant &oVal) const;
};

WXDLLIMPEXP_GIS_GP inline void AddAllVectorFilters(wxGISGPGxObjectDomain* pDomain)
{
    for (size_t i = enumVecUnknown + 1; i < enumVecMAX; ++i)
    {
        pDomain->AddFilter(new wxGxFeatureDatasetFilter(wxGISEnumVectorDatasetType(i)));
    }
}

WXDLLIMPEXP_GIS_GP inline void AddAllVectorFileFilters(wxGISGPGxObjectDomain* pDomain)
{
    for (size_t i = enumVecUnknown + 1; i < enumVecMAX; ++i)
    {
        if (IsFileDataset(enumGISFeatureDataset, wxGISEnumVectorDatasetType(i)))
        {
            pDomain->AddFilter(new wxGxFeatureDatasetFilter(wxGISEnumVectorDatasetType(i)));
        }
    }
}

WXDLLIMPEXP_GIS_GP inline void AddAllVectorDBFilters(wxGISGPGxObjectDomain* pDomain)
{
    pDomain->AddFilter(new wxGxFeatureDatasetFilter(enumVecPostGIS));
}


WXDLLIMPEXP_GIS_GP inline void AddAllRasterFilters(wxGISGPGxObjectDomain* pDomain)
{
//    for(size_t i = enumRasterUnknown + 1; i < enumRasterMAX; ++i)
//        pDomain->AddFilter(new wxGxRasterFilter(wxGISEnumRasterDatasetType(i)));
}


/** \class wxGISGPStringDomain gpdomain.h
    \brief The domain storing strings
*/
class WXDLLIMPEXP_GIS_GP wxGISGPStringDomain : public wxGISGPValueDomain
{
    DECLARE_CLASS(wxGISGPStringDomain)
public:
    wxGISGPStringDomain (void);
    virtual ~wxGISGPStringDomain (void);
	virtual void AddString(const wxString &soStr, const wxString &soNameStr = wxEmptyString);
    virtual wxString GetString(size_t nIndex) const;
};


