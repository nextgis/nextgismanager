/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxShapeFactory class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2013 Bishop
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
#include "wxgis/catalog/gxshapefactory.h"
#include "wxgis/catalog/gxdataset.h"
#include "wxgis/datasource/sysop.h"

#include <wx/ffile.h>

//------------------------------------------------------------------------------
// wxGxShapeFactory
//------------------------------------------------------------------------------
static const char *shape_add_exts[] = {
    "sbn", "sbx", "shx", "cpg", "qix", "osf", NULL
};

IMPLEMENT_DYNAMIC_CLASS(wxGxShapeFactory, wxGxObjectFactory)

wxGxShapeFactory::wxGxShapeFactory(void)
{
    m_bHasDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("ESRI Shapefile");
}

wxGxShapeFactory::~wxGxShapeFactory(void)
{
}

bool wxGxShapeFactory::GetChildren(wxGxObject* pParent, char** &pFileNames, wxArrayLong & pChildrenIds)
{
    for(int i = CSLCount(pFileNames) - 1; i >= 0; i-- )
    {
        wxGxObject* pGxObj = NULL;
        CPLString szExt = CPLGetExtension(pFileNames[i]);
        CPLString szPath;

        if(wxGISEQUAL(szExt, "shp"))
        {
            bool bHasDbf(false);
            szPath = (char*)CPLResetExtension(pFileNames[i], "dbf");
            if(CPLCheckForFile((char*)szPath.c_str(), NULL))
                bHasDbf = true;
            if(!bHasDbf)
            {
                szPath = (char*)CPLResetExtension(pFileNames[i], "DBF");
                if(CPLCheckForFile((char*)szPath.c_str(), NULL))
                    bHasDbf = true;
            }
            //szPath = (char*)CPLResetExtension(pFileNames[i], "prj");
            //if(CPLCheckForFile((char*)szPath.c_str(), NULL))
            //    bHasPrj = true;
            if(bHasDbf && m_bHasDriver)
            {
                pGxObj = GetGxObject(pParent, GetConvName(pFileNames[i]), pFileNames[i], enumGISFeatureDataset);
                if(pGxObj)
                    pChildrenIds.Add(pGxObj->GetId());
            }
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        }
        else if(wxGISEQUAL(szExt, "dbf"))
        {
            bool bHasShp(false);
            szPath = (char*)CPLResetExtension(pFileNames[i], "shp");
            if(CPLCheckForFile((char*)szPath.c_str(), NULL))
                bHasShp = true;
            if(!bHasShp)
            {
                szPath = (char*)CPLResetExtension(pFileNames[i], "SHP");
                if(CPLCheckForFile((char*)szPath.c_str(), NULL))
                    bHasShp = true;
            }
            if(!bHasShp && m_bHasDriver)
            {
                pGxObj = GetGxObject(pParent, GetConvName(pFileNames[i]), pFileNames[i], enumGISTableDataset);
                if(pGxObj)
                    pChildrenIds.Add(pGxObj->GetId());
            }
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        }
        else if(wxGISEQUAL(szExt, "prj") || wxGISEQUAL(szExt, "qrj"))
        {
            bool bHasShp(false);
			if(pFileNames)
			{
				szPath = (char*)CPLResetExtension(pFileNames[i], "shp");
				if(CPLCheckForFile((char*)szPath.c_str(), NULL))
					bHasShp = true;
				if(bHasShp)
					pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
			}
        }

        for(int j = 0; shape_add_exts[j] != NULL; ++j )
        {
            if(wxGISEQUAL(szExt, shape_add_exts[j]))
            {
                pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
                break;
            }
        }

    }
	return true;
}

wxGxObject* wxGxShapeFactory::GetGxObject(wxGxObject* pParent, const wxString &soName, const CPLString &szPath, wxGISEnumDatasetType type)
{
#ifdef CHECK_DUBLES
    if(IsNameExist(pParent, soName))
    {
        return NULL;
    }
#endif //CHECK_DUBLES

    switch(type)
    {
    case enumGISFeatureDataset:
        {
	    wxGxFeatureDataset* pDataset = new wxGxFeatureDataset(enumVecESRIShapefile, pParent, soName, szPath);
        return wxStaticCast(pDataset, wxGxObject);
        }
    case enumGISTableDataset:
        {
        wxGxTableDataset* pDataset = new wxGxTableDataset(enumTableDBF, pParent, soName, szPath);
        return wxStaticCast(pDataset, wxGxObject);
        }
    }
    return NULL;
}

