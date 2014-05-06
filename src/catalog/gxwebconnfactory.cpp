/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxWebConnectionFactory class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2013,2014 Bishop
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
#include "wxgis/catalog/gxwebconnfactory.h"
#include "wxgis/catalog/gxremoteconn.h"

#include "wxgis/datasource/gdalinh.h"
#include "wxgis/datasource/sysop.h"

//------------------------------------------------------------------------------
// wxGxWebConnectionFactory
//------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGxWebConnectionFactory, wxGxObjectFactory)

wxGxWebConnectionFactory::wxGxWebConnectionFactory(void)
{
    m_bHasDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("WFS") || GDALGetDriverByName("WMS");
}

wxGxWebConnectionFactory::~wxGxWebConnectionFactory(void)
{
}

bool wxGxWebConnectionFactory::GetChildren(wxGxObject* pParent, char** &pFileNames, wxArrayLong & pChildrenIds)
{
    bool bCheckNames = CSLCount(pFileNames) < CHECK_DUBLES_MAX_COUNT;
    wxGxCatalogBase* pCatalog = GetGxCatalog();
    for(int i = CSLCount(pFileNames) - 1; i >= 0; i-- )
    {
        CPLString szExt = CPLGetExtension(pFileNames[i]);
        if (wxGISEQUAL(szExt, "wconn"))
		{
            if( m_bHasDriver )
            {
    			wxGxObject* pObj = GetGxObject(pParent, GetConvName(pFileNames[i]), pFileNames[i], bCheckNames); 
                if (pObj != NULL)
                {
                    pChildrenIds.Add(pObj->GetId());
                }
            }
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
		}
        else if (wxGISEQUAL(szExt, "xml"))
        {
            bool bAdd = false;
            if (m_bHasDriver)
            {
                wxGxObject* pObj = GetGxObject(pParent, GetConvName(pFileNames[i]), pFileNames[i], bCheckNames);
                if (pObj != NULL)
                {
                    pChildrenIds.Add(pObj->GetId());
                    bAdd = true;
                }
            }

            if (bAdd)
            {
                pFileNames = CSLRemoveStrings(pFileNames, i, 1, NULL);
            }
        }
    }
	return true;
}

wxGxObject* wxGxWebConnectionFactory::GetGxObject(wxGxObject* pParent, const wxString &soName, const CPLString &szPath, bool bCheckNames)
{
    if(bCheckNames && IsNameExist(pParent, soName))
    {
        return NULL;
    }

    wxXmlDocument config(wxString::FromUTF8(szPath));
    if (config.IsOk())
    {
        wxXmlNode* pRoot = config.GetRoot();
        if (pRoot != NULL)
        {
            if (pRoot->GetName().IsSameAs(wxT("NGW"), false))
            {
                wxGxNGWService* pDataset = new wxGxNGWService(pParent, soName, szPath);
	            return wxStaticCast(pDataset, wxGxObject);
            }
            else if (pRoot->GetName().IsSameAs(wxT("GDAL_WMS"), false))
            {
                wxGxTMSWebService* pDataset = new wxGxTMSWebService(pParent, soName, szPath);
	            return wxStaticCast(pDataset, wxGxObject);
            }
        }
    }
    return NULL;
}
