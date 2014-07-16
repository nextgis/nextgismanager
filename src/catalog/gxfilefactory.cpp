/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxFileFactory class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2011,2013 Dmitry Baryshnikov
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
#include "wxgis/catalog/gxfilefactory.h"
#include "wxgis/catalog/gxfile.h"
#include "wxgis/datasource/sysop.h"
#include "wxgis/core/format.h"

#include "wx/tokenzr.h"

//------------------------------------------------------------------------------
// wxGxFileFactory
//------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGxFileFactory, wxGxObjectFactory)

wxGxFileFactory::wxGxFileFactory(void)
{
}

wxGxFileFactory::~wxGxFileFactory(void)
{
}

bool wxGxFileFactory::GetChildren(wxGxObject* pParent, char** &pFileNames, wxArrayLong & pChildrenIds)
{
    for(int i = CSLCount(pFileNames) - 1; i >= 0; i-- )
    {
        wxGxObject* pGxObj = NULL;
        CPLString szExt = CPLGetExtension(pFileNames[i]);
        wxString sExt(szExt, wxConvUTF8);
        for(size_t j = 0; j < m_ExtArray.size(); ++j)
        {
            if(m_ExtArray[j].IsSameAs (sExt,false))
            {
                pGxObj = GetGxObject(pParent, GetConvName(pFileNames[i]), pFileNames[i]);
                if(pGxObj)
                {
                    pChildrenIds.Add(pGxObj->GetId());
                    pGxObj = NULL;

                    pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
                }

            }
        }
    }
	return true;
}

void wxGxFileFactory::Serialize(wxXmlNode* const pConfig, bool bStore)
{
    if(bStore)
    {
        if(pConfig->HasAttribute(wxT("factory_name")))
            pConfig->DeleteAttribute(wxT("factory_name"));
        pConfig->AddAttribute(wxT("factory_name"), GetClassName());  
        if(pConfig->HasAttribute(wxT("is_enabled")))
            pConfig->DeleteAttribute(wxT("is_enabled"));
        SetBoolValue(pConfig, wxT("is_enabled"), m_bIsEnabled);    
        wxString sExt;
        for(size_t j = 0; j < m_ExtArray.size(); ++j)
        {
            sExt += m_ExtArray[j];
            sExt += wxT("|");
        }
        if(pConfig->HasAttribute(wxT("extensions")))
            pConfig->DeleteAttribute(wxT("extensions"));
        pConfig->AddAttribute(wxT("extensions"), sExt);    
    }
    else
    {
        m_bIsEnabled = GetBoolValue(pConfig, wxT("is_enabled"), true);
        wxString sExts = pConfig->GetAttribute(wxT("extensions"), wxT("txt|log|nfo|"));
	    wxStringTokenizer tkz(sExts, wxString(wxT("|")), wxTOKEN_RET_EMPTY );
	    while ( tkz.HasMoreTokens() )
	    {
		    wxString token = tkz.GetNextToken();
		    token.MakeLower();
            m_ExtArray.Add(token);
	    }
    }
}

wxGxObject* wxGxFileFactory::GetGxObject(wxGxObject* pParent, const wxString &soName, const CPLString &szPath)
{
    wxGxTextFile* pFile = new wxGxTextFile(pParent, soName, szPath);
    return wxStaticCast(pFile, wxGxObject);
}
