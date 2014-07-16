/******************************************************************************
* Project:  wxGIS (GIS Catalog)
* Purpose:  wxGxGisProjectFactory class. Create new GxGisProjects objects
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
#include "wxgis/catalog/gxgisprojfactory.h"
#include "wxgis/catalog/gxgisproj.h"
#include "wxgis/datasource/sysop.h"

//#include "wx/filename.h"

//------------------------------------------------------------------------------
// wxGxGisProjectFactory
//------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGxGisProjectFactory, wxGxObjectFactory)

wxGxGisProjectFactory::wxGxGisProjectFactory(void)
{
}

wxGxGisProjectFactory::~wxGxGisProjectFactory(void)
{
}

bool wxGxGisProjectFactory::GetChildren(wxGxObject* pParent, char** &pFileNames, wxArrayLong & pChildrenIds)
{
    bool bCheckNames = CSLCount(pFileNames) < CHECK_DUBLES_MAX_COUNT;
    for (int i = CSLCount(pFileNames) - 1; i >= 0; i--)
    {
        wxGxObject* pGxObj = NULL;
        CPLString szExt = CPLGetExtension(pFileNames[i]);
        CPLString szPath;

        if (wxGISEQUAL(szExt, "qgs"))
        {
            pGxObj = GetGxObject(pParent, GetConvName(pFileNames[i]), pFileNames[i], enumGisProjQGIS, bCheckNames);
            pFileNames = CSLRemoveStrings(pFileNames, i, 1, NULL);
        }
        else if (wxGISEQUAL(szExt, "wor"))
        {
            pGxObj = GetGxObject(pParent, GetConvName(pFileNames[i]), pFileNames[i], enumGisProjWor, bCheckNames);
            pFileNames = CSLRemoveStrings(pFileNames, i, 1, NULL);
        }

        if (pGxObj != NULL)
        {
            pChildrenIds.Add(pGxObj->GetId());
            pGxObj = NULL;
        }
    }

    return true;

}

wxGxObject* wxGxGisProjectFactory::GetGxObject(wxGxObject* pParent, const wxString &soName, const CPLString &szPath, wxGISEnumGisProjectType eType, bool bCheckNames)
{
    if (bCheckNames && IsNameExist(pParent, soName))
    {
        return NULL;
    }

    switch (eType)
    {
    case enumGisProjQGIS:
    {
        wxGxQGISProjFile* pProjFile = new wxGxQGISProjFile(pParent, soName, szPath);
        return wxStaticCast(pProjFile, wxGxObject);// static_cast<wxGxObject*>(pProjFile);
    }
    case enumGisProjWor:
        //TODO: add support to Mapinfo WOR file
        //wxGxWORProjFile
        return NULL;
    default:
        return NULL;
    };
}
