/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxMLFactory class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2011,2013,2014 Dmitry Baryshnikov
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

#include "wxgis/catalog/gxmlfactory.h"
#include "wxgis/catalog/gxdataset.h"
#include "wxgis/catalog/gxmldataset.h"
#include "wxgis/datasource/sysop.h"

//---------------------------------------------------------------------------
// wxGxMLFactory
//---------------------------------------------------------------------------
static const char *ml_add_exts[] = {
    "rsc", "sda", "shd", "sit", "sit.ini", "sse", NULL
};

IMPLEMENT_DYNAMIC_CLASS(wxGxMLFactory, wxObject)

wxGxMLFactory::wxGxMLFactory(void)
{
    m_bHasKMLDriver = NULL != GetOGRCompatibleDriverByName("KML");
    m_bHasLIBKMLDriver = NULL != GetOGRCompatibleDriverByName("LIBKML");
    m_bHasDXFDriver = NULL != GetOGRCompatibleDriverByName(GetDriverByType(enumGISFeatureDataset, enumVecDXF).mb_str());
    m_bHasGMLDriver = NULL != GetOGRCompatibleDriverByName(GetDriverByType(enumGISFeatureDataset, enumVecGML).mb_str());
    m_bHasJsonDriver = NULL != GetOGRCompatibleDriverByName(GetDriverByType(enumGISFeatureDataset, enumVecGeoJSON).mb_str());

    CPLSetConfigOption("SXF_LAYER_FULLNAME", "YES");
    m_bHasSXFDriver = NULL != GetOGRCompatibleDriverByName(GetDriverByType(enumGISFeatureDataset, enumVecSXF).mb_str());
}

wxGxMLFactory::~wxGxMLFactory(void)
{
}

bool wxGxMLFactory::GetChildren(wxGxObject* pParent, char** &pFileNames, wxArrayLong & pChildrenIds)
{
    bool bCheckNames = CSLCount(pFileNames) < CHECK_DUBLES_MAX_COUNT;
    for(int i = CSLCount(pFileNames) - 1; i >= 0; i-- )
    {
        wxGxObject* pGxObj = NULL;
        CPLString szExt = CPLGetExtension(pFileNames[i]);
        CPLString szPath;

        if(wxGISEQUAL(szExt, "kml") && (m_bHasKMLDriver || m_bHasLIBKMLDriver) )
        {
            pGxObj = GetGxObject(pParent, GetConvName(pFileNames[i]), pFileNames[i], enumVecKML, bCheckNames);
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        }
        else if(wxGISEQUAL(szExt, "kmz") && m_bHasLIBKMLDriver)
        {
            pGxObj = GetGxObject(pParent, GetConvName(pFileNames[i]), pFileNames[i], enumVecKMZ, bCheckNames);
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        }
        else if(wxGISEQUAL(szExt, "dxf") && m_bHasDXFDriver)
        {
            pGxObj = GetGxObject(pParent, GetConvName(pFileNames[i]), pFileNames[i], enumVecDXF, bCheckNames);
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        }
        else if(wxGISEQUAL(szExt, "gml") && m_bHasGMLDriver)
        {
            pGxObj = GetGxObject(pParent, GetConvName(pFileNames[i]), pFileNames[i], enumVecGML, bCheckNames);
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        }
        else if ( (wxGISEQUAL(szExt, "geojson") || wxGISEQUAL(szExt, "json")) && m_bHasJsonDriver)
        {
            pGxObj = GetGxObject(pParent, GetConvName(pFileNames[i]), pFileNames[i], enumVecGeoJSON, bCheckNames);
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        }
        else if (wxGISEQUAL(szExt, "sxf") && m_bHasSXFDriver)
        {
            pGxObj = GetGxObject(pParent, GetConvName(pFileNames[i]), pFileNames[i], enumVecSXF, bCheckNames);
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        }

        for (int j = 0; ml_add_exts[j] != NULL; ++j)
        {
            if (wxGISEQUAL(szExt, ml_add_exts[j]))
            {
                pFileNames = CSLRemoveStrings(pFileNames, i, 1, NULL);
                break;
            }
        }


		if(pGxObj != NULL)
        {
			pChildrenIds.Add(pGxObj->GetId());
            pGxObj = NULL;
        }
    }

	return true;
}

wxGxObject* wxGxMLFactory::GetGxObject(wxGxObject* pParent, const wxString &soName, const CPLString &szPath, wxGISEnumVectorDatasetType type, bool bCheckNames)
{
    if(bCheckNames && IsNameExist(pParent, soName))
    {
        return NULL;
    }

    switch (type)
    {
    case enumVecKML:
    case enumVecKMZ:
    case enumVecSXF:
    case enumVecGML:
    {
        wxGxMLDataset* pDataset = new wxGxMLDataset(type, pParent, soName, szPath);
        return wxStaticCast(pDataset, wxGxObject);
    }

    default:
    {
               wxGxFeatureDataset* pDataset = new wxGxFeatureDataset(type, pParent, soName, szPath);
               return wxStaticCast(pDataset, wxGxObject);
    }
    };
    return NULL;
}
