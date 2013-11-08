/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxCSVFileFactory class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011,2013 Bishop
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
#include "wxgis/catalog/gxcsvfactory.h"
#include "wxgis/catalog/gxdataset.h"
#include "wxgis/datasource/sysop.h"

//------------------------------------------------------------------------------
// wxGxCSVFileFactory
//------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGxCSVFileFactory, wxGxObjectFactory)

wxGxCSVFileFactory::wxGxCSVFileFactory(void)
{
    m_bHasDriver = NULL != OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("CSV");
}

wxGxCSVFileFactory::~wxGxCSVFileFactory(void)
{
}

bool wxGxCSVFileFactory::GetChildren(wxGxObject* pParent, char** &pFileNames, wxArrayLong & pChildrenIds)
{
    for(int i = CSLCount(pFileNames) - 1; i >= 0; i-- )
    {
        wxGxObject* pGxObj = NULL;
        CPLString szExt = CPLGetExtension(pFileNames[i]);
        CPLString szPath;

        if(wxGISEQUAL(szExt, "csv"))
        {
            if(m_bHasDriver)
            {
                pGxObj = GetGxObject(pParent, GetConvName(pFileNames[i]), pFileNames[i]);
                if(pGxObj)
                    pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
            }
        }
        else if(wxGISEQUAL(szExt, "csvt"))
        {
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        }

        if(pGxObj)
        {
            pChildrenIds.Add(pGxObj->GetId());
            pGxObj = NULL;
        }

    }
	return true;
}

wxGxObject* wxGxCSVFileFactory::GetGxObject(wxGxObject* pParent, const wxString &soName, const CPLString &szPath)
{
#ifdef CHECK_DUBLES
    if(IsNameExist(pParent, soName))
    {
        return NULL;
    }
#endif //CHECK_DUBLES

    wxGxTableDataset* pDataset = new wxGxTableDataset(enumTableCSV, pParent, soName, szPath);
    return wxStaticCast(pDataset, wxGxObject);
}
