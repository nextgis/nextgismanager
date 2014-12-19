/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxArchiveFactory class. Create new GxFolder objects
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
#include "wxgis/catalog/gxarchivefactory.h"
#include "wxgis/catalog/gxarchfolder.h"
#include "wxgis/datasource/gdalinh.h"

#include "wx/filename.h"
#include "wx/dir.h"

#include "cpl_vsi_virtual.h"

//------------------------------------------------------------------------------
// wxGxArchiveFactory
//------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGxArchiveFactory, wxGxObjectFactory)

wxGxArchiveFactory::wxGxArchiveFactory(void)
{
    m_bHasDriver = VSIFileManager::GetHandler("/vsizip/") != NULL;
}

wxGxArchiveFactory::~wxGxArchiveFactory(void)
{
}

bool wxGxArchiveFactory::GetChildren(wxGxObject* pParent, char** &pFileNames, wxArrayLong & pChildrenIds)
{
    bool bCheckNames = CSLCount(pFileNames) < CHECK_DUBLES_MAX_COUNT;
    for(int i = CSLCount(pFileNames) - 1; i >= 0; i-- )
    {
        if (wxGISEQUALN(pFileNames[i], "/vsi", 4))
            continue;
        
        wxString path(pFileNames[i], wxConvUTF8);
        wxGxObject* pGxObj = NULL;

        CPLString szExt = CPLGetExtension(pFileNames[i]);
        if(wxGISEQUAL(szExt, "zip"))
        {
            if(m_bHasDriver)
            {
                CPLString pArchiveName("/vsizip/");
                pArchiveName += pFileNames[i];

                pGxObj = GetGxObject(pParent, wxString(CPLGetFilename(pFileNames[i]), wxConvUTF8), pArchiveName, bCheckNames);
            }
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        }
		//else if(wxGISEQUAL(szExt, "gz"))
  //      {
  //          CPLString pArchiveName("/vsigzip/");
  //          pArchiveName += pFileNames[i];

  //          wxFileName FName(path);
  //          wxString sName = FName.GetFullName();

  //          IGxObject* pGxObj = GetGxObject(pArchiveName, sName);
  //          ObjArray.push_back(pGxObj);
  //          pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
  //      }
        if(pGxObj)
        {
            pChildrenIds.Add(pGxObj->GetId());
            pGxObj = NULL;
        }
    }
	return true;
}

wxGxObject* wxGxArchiveFactory::GetGxObject(wxGxObject* pParent, const wxString &soName, const CPLString &szPath, bool bCheckNames)
{
    if (bCheckNames && IsNameExist(pParent, soName))
    {
        return NULL;
    }

    wxGxArchive* pFolder = new wxGxArchive(pParent, soName, szPath);
	return wxStaticCast(pFolder, wxGxObject);
}