/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxPrjFactory class.
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
#include "wxgis/catalog/gxprjfactory.h"
#include "wxgis/catalog/gxfile.h"
#include "wxgis/catalog/gxdataset.h"
#include "wxgis/datasource/sysop.h"

#include "wx/filename.h"
#include "wx/dir.h"

//----------------------------------------------------------------------------
// wxGxPrjFactory
//----------------------------------------------------------------------------
static const char *prj_notadd_exts[] = {
    "shp", "bmp", "gif", "jpg", "png", NULL
};

IMPLEMENT_DYNAMIC_CLASS(wxGxPrjFactory, wxGxObjectFactory)

wxGxPrjFactory::wxGxPrjFactory(void)
{
}

wxGxPrjFactory::~wxGxPrjFactory(void)
{
}

bool wxGxPrjFactory::GetChildren(wxGxObject* pParent, char** &pFileNames, wxArrayLong & pChildrenIds)
{
    bool bCheckNames = CSLCount(pFileNames) < CHECK_DUBLES_MAX_COUNT;
    for(int i = CSLCount(pFileNames) - 1; i >= 0; i-- )
    {
        CPLString szExt = CPLGetExtension(pFileNames[i]);

        wxGxObject* pGxObj = NULL;
 
        if(wxGISEQUAL(szExt, "prj"))
        {
            bool bAdd = true;
            for(int j = 0; prj_notadd_exts[j] != NULL; ++j )
            {
                if(CPLCheckForFile((char*)CPLResetExtension(pFileNames[i], prj_notadd_exts[j]), NULL))
                {
                    bAdd = false;
                    break;
                }
            }

            if(bAdd)
            {
                pGxObj = GetGxObject(pParent, GetConvName(pFileNames[i]), pFileNames[i], enumESRIPrjFile, bCheckNames);
            }

            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        }
        else if(wxGISEQUAL(szExt, "qpj"))
        {
            bool bAdd = true;
            for(int j = 0; prj_notadd_exts[j] != NULL; ++j )
            {
                if(CPLCheckForFile((char*)CPLResetExtension(pFileNames[i], prj_notadd_exts[j]), NULL))
                {
                    bAdd = false;
                    break;
                }
            }

            if(bAdd)
            {
                pGxObj = GetGxObject(pParent, GetConvName(pFileNames[i]), pFileNames[i], enumQPJfile, bCheckNames);
            }

            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        }
        else if(wxGISEQUAL(szExt, "spr"))
        {
            pGxObj = GetGxObject(pParent, GetConvName(pFileNames[i]), pFileNames[i], enumSPRfile, bCheckNames);

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


wxGxObject* wxGxPrjFactory::GetGxObject(wxGxObject* pParent, const wxString &soName, const CPLString &szPath, wxGISEnumPrjFileType nType, bool bCheckNames)
{
    if(bCheckNames && IsNameExist(pParent, soName))
    {
        return NULL;
    }

    wxGxPrjFile* pFile = new wxGxPrjFile(nType, pParent, soName, szPath);
	return wxStaticCast(pFile, wxGxObject);
}
