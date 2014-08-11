/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxSpreadsheetFactory class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2014 Dmitry Baryshnikov
*   Copyright (C) 2014 NextGIS
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

#include "wxgis/catalog/gxssfactory.h"
#include "wxgis/catalog/gxdataset.h"
#include "wxgis/catalog/gxssdataset.h"
#include "wxgis/datasource/sysop.h"

//---------------------------------------------------------------------------
// wxGxSpreadsheetFactory
//---------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGxSpreadsheetFactory, wxObject)

wxGxSpreadsheetFactory::wxGxSpreadsheetFactory(void)
{
    m_bHasODSDriver = NULL != OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(GetDriverByType(enumGISTableDataset, enumTableODS).mb_str());
    m_bHasXLSDriver = NULL != OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(GetDriverByType(enumGISTableDataset, enumTableXLS).mb_str());
    m_bHasXLSXDriver = NULL != OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(GetDriverByType(enumGISTableDataset, enumTableXLSX).mb_str());
}

wxGxSpreadsheetFactory::~wxGxSpreadsheetFactory(void)
{
}

bool wxGxSpreadsheetFactory::GetChildren(wxGxObject* pParent, char** &pFileNames, wxArrayLong & pChildrenIds)
{
    bool bCheckNames = CSLCount(pFileNames) < CHECK_DUBLES_MAX_COUNT;
    for(int i = CSLCount(pFileNames) - 1; i >= 0; i-- )
    {
        wxGxObject* pGxObj = NULL;
        CPLString szExt = CPLGetExtension(pFileNames[i]);
        CPLString szPath;

        if(wxGISEQUAL(szExt, "ods") && (m_bHasODSDriver) )
        {
            pGxObj = GetGxObject(pParent, GetConvName(pFileNames[i]), pFileNames[i], enumTableODS, bCheckNames);
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        }
        else if(wxGISEQUAL(szExt, "xls") && m_bHasXLSDriver)
        {
            pGxObj = GetGxObject(pParent, GetConvName(pFileNames[i]), pFileNames[i], enumTableXLS, bCheckNames);
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        }
        else if(wxGISEQUAL(szExt, "xlsx") && m_bHasXLSXDriver)
        {
            pGxObj = GetGxObject(pParent, GetConvName(pFileNames[i]), pFileNames[i], enumTableXLSX, bCheckNames);
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

wxGxObject* wxGxSpreadsheetFactory::GetGxObject(wxGxObject* pParent, const wxString &soName, const CPLString &szPath, wxGISEnumTableDatasetType type, bool bCheckNames)
{
    if(bCheckNames && IsNameExist(pParent, soName))
    {
        return NULL;
    }

    wxGxSpreadsheetDataset* pDataset = new wxGxSpreadsheetDataset(type, pParent, soName, szPath);
    return wxStaticCast(pDataset, wxGxObject);
}
