/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxShapeFactory class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2014 Dmitry Baryshnikov
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
#include "wxgis/catalog/gxgnmfactory.h"

#if GDAL_VERSION_NUM >= 2000000 //TODO: Add check if GNM is compiled in GDAL

#include "wxgis/catalog/gxgnm.h"

#include "gnm/gnm.h"


//------------------------------------------------------------------------------
// wxGxGNMFactory
//------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGxGNMFactory, wxGxObjectFactory)

wxGxGNMFactory::wxGxGNMFactory(void)
{
}

wxGxGNMFactory::~wxGxGNMFactory(void)
{
}

bool wxGxGNMFactory::GetChildren(wxGxObject* pParent, char** &pFileNames, wxArrayLong & pChildrenIds)
{
    bool bCheckNames = CSLCount(pFileNames) < CHECK_DUBLES_MAX_COUNT;
    bool bHasModel = false;
    //char** paPossibleModelLayers = NULL;
    CPLString szMeta, szClasses;
    for(int i = CSLCount(pFileNames) - 1; i >= 0; i-- )
    {
        wxGxObject* pGxObj = NULL;
        CPLString szExt = CPLGetExtension(pFileNames[i]);
        CPLString szName = CPLGetBasename(pFileNames[i]);
        CPLString szPath;

        if (wxGISEQUAL(szName, GNM_SYSLAYER_META))
        {
            bHasModel = true;
            szMeta = pFileNames[i];
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        }
        if (wxGISEQUAL(szName, GNM_SYSLAYER_GRAPH))
        {
            bHasModel = true;
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        }
        if (wxGISEQUAL(szName, GNM_SYSLAYER_CLASSES))
        {
            bHasModel = true;
            szClasses = pFileNames[i];
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        }
        else if (EQUALN(szName, "gnm_", 4))
        {
            //paPossibleModelLayers = CSLAddString(paPossibleModelLayers, pFileNames[i]);
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        }
 
    }

    if (bHasModel)
    {
        //TODO: open meta and classes
        GetGxObject(pParent, wxT("test gmn"), CPLGetPath(szMeta), bCheckNames);
    }
	return true;
}

wxGxObject* wxGxGNMFactory::GetGxObject(wxGxObject* pParent, const wxString &soName, const CPLString &szPath, bool bCheckNames)
{
    if(bCheckNames && IsNameExist(pParent, soName))
    {
        return NULL;
    }

    wxGxGNMConnectivity* pDataset = new wxGxGNMConnectivity(enumVecESRIShapefile, pParent, soName, szPath);
    return wxStaticCast(pDataset, wxGxObject);
}

#endif