/******************************************************************************
* Project:  wxGIS (GIS Catalog)
* Purpose:  wxGxLocalDBFactory class. Create new local databases GxObjects
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
#include "wxgis/catalog/gxlocaldbfactory.h"
#include "wxgis/catalog/gxlocaldb.h"

//------------------------------------------------------------------------------
// wxGxFolderFactory
//------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGxLocalDBFactory, wxGxObjectFactory)

wxGxLocalDBFactory::wxGxLocalDBFactory(void)
{
    m_bHasOFGDBDriver = NULL != OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("OpenFileGDB");
}

wxGxLocalDBFactory::~wxGxLocalDBFactory(void)
{
}

bool wxGxLocalDBFactory::GetChildren(wxGxObject* pParent, char** &pFileNames, wxArrayLong & pChildrenIds)
{
    wxGxCatalogBase* pCatalog = GetGxCatalog();
    bool bCheckNames = CSLCount(pFileNames) < CHECK_DUBLES_MAX_COUNT;
    for(int i = CSLCount(pFileNames) - 1; i >= 0; i-- )
    {
        VSIStatBufL BufL;
        int ret = VSIStatL(pFileNames[i], &BufL);
        if(ret == 0)
        {
            if (VSI_ISDIR(BufL.st_mode) && wxGISEQUAL(CPLGetExtension(pFileNames[i]), "gdb"))
		    {
                wxGxObject* pObj = GetGxObject(pParent, wxString(CPLGetFilename(pFileNames[i]), wxConvUTF8), pFileNames[i], enumContGDBFolder, bCheckNames);
                if(pObj)
                    pChildrenIds.Add(pObj->GetId());
                pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
		    }
            //TODO: mdb, sqlite, db extensions
        }
    }
	return true;
}

wxGxObject* wxGxLocalDBFactory::GetGxObject(wxGxObject* pParent, const wxString &soName, const CPLString &szPath, wxGISEnumContainerType eType, bool bCheckNames)
{
    if (eType == enumContGDBFolder && !m_bHasOFGDBDriver)
        return NULL;

    if (bCheckNames && IsNameExist(pParent, soName))
    {
        return NULL;
    }

    if (eType == enumContGDBFolder)
    {
        wxGxOpenFileGDB* pDB = new wxGxOpenFileGDB(pParent, soName, szPath);
        return static_cast<wxGxObject*>(pDB);
    }

    return NULL;
}
