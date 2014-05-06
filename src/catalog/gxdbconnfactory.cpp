/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxDBConnectionFactory class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011,2013,2014 Bishop
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
#include "wxgis/catalog/gxdbconnfactory.h"
#include "wxgis/catalog/gxremoteconn.h"

#include "wxgis/datasource/gdalinh.h"
#include "wxgis/datasource/sysop.h"
#include "wxgis/core/format.h"

//------------------------------------------------------------------------------
// wxGxDBConnectionFactory
//------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGxDBConnectionFactory, wxGxObjectFactory)

wxGxDBConnectionFactory::wxGxDBConnectionFactory(void)
{
    m_bHasDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("PostgreSQL") || GDALGetDriverByName("PostGISRaster");
    m_bLoadSystemTablesAndSchemes = false;
}

wxGxDBConnectionFactory::~wxGxDBConnectionFactory(void)
{
}

bool wxGxDBConnectionFactory::GetChildren(wxGxObject* pParent, char** &pFileNames, wxArrayLong & pChildrenIds)
{
    bool bCheckNames = CSLCount(pFileNames) < CHECK_DUBLES_MAX_COUNT;

    wxGxCatalogBase* pCatalog = GetGxCatalog();
    for(int i = CSLCount(pFileNames) - 1; i >= 0; i-- )
    {
        CPLString szExt = CPLGetExtension(pFileNames[i]);
		if(wxGISEQUAL(szExt, "xconn"))
		{
            if( m_bHasDriver )
            {
                wxGxObject* pObj = GetGxObject(pParent, GetConvName(pFileNames[i]), pFileNames[i], bCheckNames);
                if(pObj)
                    pChildrenIds.Add(pObj->GetId());
            }
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
		}
    }
	return true;
}

wxGxObject* wxGxDBConnectionFactory::GetGxObject(wxGxObject* pParent, const wxString &soName, const CPLString &szPath, bool bCheckNames)
{
    if (bCheckNames && IsNameExist(pParent, soName))
    {
        return NULL;
    }

    //TODO: other DB like MySQL my have different ifdefs
#ifdef wxGIS_USE_POSTGRES
	wxGxRemoteConnection* pDataset = new wxGxRemoteConnection(pParent, soName, szPath);
	return static_cast<wxGxObject*>(pDataset);
#else
    return NULL;
#endif //wxGIS_USE_POSTGRES
}

void wxGxDBConnectionFactory::Serialize(wxXmlNode* const pConfig, bool bStore)
{
    wxGxObjectFactory::Serialize(pConfig, bStore);
    if(bStore)
    {
        if(pConfig->HasAttribute(wxT("load_system_tables_schemes")))
            pConfig->DeleteAttribute(wxT("load_system_tables_schemes"));
        SetBoolValue(pConfig, wxT("load_system_tables_schemes"), m_bLoadSystemTablesAndSchemes);    
    }
    else
    {
        m_bLoadSystemTablesAndSchemes = GetBoolValue(pConfig, wxT("load_system_tables_schemes"), m_bLoadSystemTablesAndSchemes);
    }
}
