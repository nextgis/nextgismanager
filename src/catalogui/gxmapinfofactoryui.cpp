/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxMapInfoFactoryUI class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2011,2014 Dmitry Baryshnikov
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
#include "wxgis/catalogui/gxmapinfofactoryui.h"

#include "wxgis/catalogui/gxdatasetui.h"
#include <wx/ffile.h>

#include "../../art/mi_dset_16.xpm"
#include "../../art/mi_dset_48.xpm"
#include "../../art/md_dset_16.xpm"
#include "../../art/md_dset_48.xpm"
#include "../../art/table_tab_16.xpm"
#include "../../art/table_tab_48.xpm"

//------------------------------------------------------------------------------
// wxGxMapInfoFactoryUI
//------------------------------------------------------------------------------
static const char *mi_filter_exts[] = {
    "dat", NULL
};


IMPLEMENT_DYNAMIC_CLASS(wxGxMapInfoFactoryUI, wxGxMapInfoFactory)

wxGxMapInfoFactoryUI::wxGxMapInfoFactoryUI(void) : wxGxMapInfoFactory()
{
}

wxGxMapInfoFactoryUI::~wxGxMapInfoFactoryUI(void)
{
}

wxGxObject* wxGxMapInfoFactoryUI::GetGxObject(wxGxObject* pParent, const wxString &soName, const CPLString &szPath, wxGISEnumVectorDatasetType type, bool bCheckNames)
{
    if(!m_bHasDriver)
        return NULL;

    if(bCheckNames && IsNameExist(pParent, soName))
    {
        return NULL;
    }

    switch(type)
    {
    case enumVecMapinfoTab:
        {
            CPLString sTestPath;
            wxGxObject *pCompoundObjectPart = NULL;
            wxGxObjectContainer* pParentCont = wxDynamicCast(pParent, wxGxObjectContainer);

            for (int j = 0; mi_filter_exts[j] != NULL; ++j)
            {
                sTestPath = (char*)CPLResetExtension(szPath, mi_filter_exts[j]);

                wxGxObjectList::const_iterator iter;
                for (iter = pParentCont->GetChildren().begin(); iter != pParentCont->GetChildren().end(); ++iter)
                {
                    wxGxObject *current = *iter;
                    if (wxGISEQUAL(current->GetPath(), sTestPath))
                    {
                        current->Destroy();
                        break;
                    }
                }
            }

            if(!m_SmallTabIcon.IsOk())
                m_SmallTabIcon = wxIcon(mi_dset_16_xpm);
            if(!m_LargeTabIcon.IsOk())
                m_LargeTabIcon = wxIcon(mi_dset_48_xpm);

            return wxStaticCast(new wxGxFeatureDatasetUI(type, pParent, soName, szPath, m_LargeTabIcon, m_SmallTabIcon), wxGxObject);
        }
    case enumVecMapinfoMif:
        if(!m_SmallMifIcon.IsOk())
            m_SmallMifIcon = wxIcon(md_dset_16_xpm);
        if(!m_LargeMifIcon.IsOk())
            m_LargeMifIcon = wxIcon(md_dset_48_xpm);
        return wxStaticCast(new wxGxFeatureDatasetUI(type, pParent, soName, szPath, m_LargeMifIcon, m_SmallMifIcon), wxGxObject);
    case enumVecMAX + 1:
        if(!m_SmallTabTIcon.IsOk())
            m_SmallTabTIcon = wxIcon(table_tab_16_xpm);
        if(!m_LargeTabTIcon.IsOk())
            m_LargeTabTIcon = wxIcon(table_tab_48_xpm);
        return wxStaticCast(new wxGxTableDatasetUI(enumTableMapinfoTab, pParent, soName, szPath, m_LargeTabTIcon, m_SmallTabTIcon), wxGxObject);
    case enumVecMAX + 2:
        if(!m_SmallTabTIcon.IsOk())
            m_SmallTabTIcon = wxIcon(table_tab_16_xpm);
        if(!m_LargeTabTIcon.IsOk())
            m_LargeTabTIcon = wxIcon(table_tab_48_xpm);
        return wxStaticCast(new wxGxTableDatasetUI(enumTableMapinfoMif, pParent, soName, szPath, m_LargeTabTIcon, m_SmallTabTIcon), wxGxObject);
    default:
        return NULL;
    }
}

