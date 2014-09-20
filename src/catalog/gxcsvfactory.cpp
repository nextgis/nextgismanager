/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxCSVFileFactory class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011,2013,2014 Dmitry Baryshnikov
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
#include "wxgis/catalog/gxcsvfactory.h"
#include "wxgis/catalog/gxdataset.h"
#include "wxgis/datasource/sysop.h"

//------------------------------------------------------------------------------
// wxGxCSVFileFactory
//------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGxCSVFileFactory, wxGxObjectFactory)

wxGxCSVFileFactory::wxGxCSVFileFactory(void)
{
    m_bHasDriver = NULL != GetOGRCompatibleDriverByName("CSV");
}

wxGxCSVFileFactory::~wxGxCSVFileFactory(void)
{
}

bool wxGxCSVFileFactory::GetChildren(wxGxObject* pParent, char** &pFileNames, wxArrayLong & pChildrenIds)
{
    bool bCheckNames = CSLCount(pFileNames) < CHECK_DUBLES_MAX_COUNT;

    for(int i = CSLCount(pFileNames) - 1; i >= 0; i-- )
    {
        wxGxObject* pGxObj = NULL;
        CPLString szExt = CPLGetExtension(pFileNames[i]);
        CPLString szPath;

        if(wxGISEQUAL(szExt, "csv"))
        {
            if(m_bHasDriver)
            {
                pGxObj = GetGxObject(pParent, GetConvName(pFileNames[i]), pFileNames[i], bCheckNames);
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

bool wxGxCSVFileFactory::HasGeometryField(const CPLString &szPath) const
{
    char szBuffer[1024+1];
    VSILFILE* fp = VSIFOpenL(szPath, "r");
    if (fp == NULL)
        return false;

    int nRead = VSIFReadL(szBuffer, 1, 1024, fp);
    szBuffer[nRead] = 0;

    VSIFCloseL(fp);

    CPLString str(szBuffer);
    return str.find("WKT") != -1;
}

wxGxObject* wxGxCSVFileFactory::GetGxObject(wxGxObject* pParent, const wxString &soName, const CPLString &szPath, bool bCheckNames)
{
    if(bCheckNames && IsNameExist(pParent, soName))
    {
        return NULL;
    }

    if(HasGeometryField(szPath))
    {
        wxGxFeatureDataset* pDataset = new wxGxFeatureDataset(enumVecCSV, pParent, soName, szPath);
        return wxStaticCast(pDataset, wxGxObject);
    }
    else
    {
        wxGxTable* pDataset = new wxGxTable(enumTableCSV, pParent, soName, szPath);
        return wxStaticCast(pDataset, wxGxObject);
    }
}
