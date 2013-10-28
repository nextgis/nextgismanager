/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxMLFactory class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2011,2013 Bishop
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

#include "wxgis/catalog/gxmlfactory.h"
#include "wxgis/catalog/gxdataset.h"
#include "wxgis/catalog/gxmldataset.h"
#include "wxgis/datasource/sysop.h"

//---------------------------------------------------------------------------
// wxGxMLFactory
//---------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGxMLFactory, wxObject)

wxGxMLFactory::wxGxMLFactory(void)
{
    m_bHasKMLDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("KML");
    m_bHasLIBKMLDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("LIBKML");
    m_bHasDXFDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("DXF");
    m_bHasGMLDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("GML");
}

wxGxMLFactory::~wxGxMLFactory(void)
{
}

bool wxGxMLFactory::GetChildren(wxGxObject* pParent, char** &pFileNames, wxArrayLong & pChildrenIds)
{
    for(int i = CSLCount(pFileNames) - 1; i >= 0; i-- )
    {
        wxGxObject* pGxObj = NULL;
        CPLString szExt = CPLGetExtension(pFileNames[i]);
        CPLString szPath;

        if(wxGISEQUAL(szExt, "kml") && (m_bHasKMLDriver || m_bHasLIBKMLDriver) )
        {
            pGxObj = GetGxObject(pParent, GetConvName(pFileNames[i]), pFileNames[i], enumVecKML);
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        }
        else if(wxGISEQUAL(szExt, "kmz") && m_bHasLIBKMLDriver)
        {
            pGxObj = GetGxObject(pParent, GetConvName(pFileNames[i]), pFileNames[i], enumVecKMZ);
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        }
        else if(wxGISEQUAL(szExt, "dxf") && m_bHasDXFDriver)
        {
            pGxObj = GetGxObject(pParent, GetConvName(pFileNames[i]), pFileNames[i], enumVecDXF);
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        }
        else if(wxGISEQUAL(szExt, "gml") && m_bHasGMLDriver)
        {
            pGxObj = GetGxObject(pParent, GetConvName(pFileNames[i]), pFileNames[i], enumVecGML);
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        }

		if(pGxObj != NULL)
        {
			pChildrenIds.Add(pGxObj->GetId());
            pGxObj = NULL;
        }
    }

	return true;
}

wxGxObject* wxGxMLFactory::GetGxObject(wxGxObject* pParent, const wxString &soName, const CPLString &szPath, wxGISEnumVectorDatasetType type)
{
#ifdef CHECK_DUBLES
    if(IsNameExist(pParent, soName))
    {
        return NULL;
    }
#endif //CHECK_DUBLES

    if(type == enumVecKML)
    {
        wxGxMLDataset* pDataset = new wxGxMLDataset(type, pParent, soName, szPath);
        return wxStaticCast(pDataset, wxGxObject);
    }
    else if(type == enumVecKMZ)
    {
        wxGxMLDataset* pDataset = new wxGxMLDataset(type, pParent, soName, szPath);
        return wxStaticCast(pDataset, wxGxObject);
    }
    else if(type == enumVecGML)
    {
        wxGxMLDataset* pDataset = new wxGxMLDataset(type, pParent, soName, szPath);
        return wxStaticCast(pDataset, wxGxObject);
    }
    else
    {
	    wxGxFeatureDataset* pDataset = new wxGxFeatureDataset(type, pParent, soName, szPath);
        return wxStaticCast(pDataset, wxGxObject);
    }
    return NULL;
}
