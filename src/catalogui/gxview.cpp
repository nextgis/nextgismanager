/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxView class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011,2012 Dmitry Baryshnikov
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
#include "wxgis/catalogui/gxview.h"
#include "wxgis/catalog/gxdiscconnection.h"
#include "wxgis/catalog/gxdataset.h"

/*
#include "wxgis/catalog/gxcatalog.h"
#include "wxgis/catalog/gxdiscconnections.h"
*/

//---------------------------------------------------------------------------
// wxGxAutoRenamer
//---------------------------------------------------------------------------
wxGxAutoRenamer::wxGxAutoRenamer()
{
    m_pGxViewToRename = NULL;
}

wxGxAutoRenamer::~wxGxAutoRenamer()
{
}

void wxGxAutoRenamer::BeginRenameOnAdd(wxGxView* const pGxView, const CPLString &szPath)
{
    m_pGxViewToRename = pGxView;
    m_szPathToRename = szPath;
    m_szPathToRename = m_szPathToRename.tolower();
}

bool wxGxAutoRenamer::IsBeginRename(wxGxView* const pGxView, const CPLString &szPath)
{
    CPLString szCmpPath = szPath;
    if(m_pGxViewToRename == pGxView && m_szPathToRename == szCmpPath.tolower())
    {
        m_pGxViewToRename = NULL;
        return true;
    }
    return false;
}

//-----------------------------------------------
// wxGxView
//-----------------------------------------------

wxGxView::wxGxView(void)
{
    m_Icon = wxNullIcon;
}

wxGxView::~wxGxView(void)
{
}

bool wxGxView::Activate(IApplication* const pApplication, wxXmlNode* const pConf)
{
	m_pXmlConf = pConf;
	return true;
}

void wxGxView::Deactivate(void)
{
	m_pXmlConf = NULL;
}

bool wxGxView::Applies(wxGxSelection* const pSelection)
{
	return NULL == pSelection ? false : true;
};

wxString wxGxView::GetViewName(void) const
{
	return m_sViewName;
}

wxIcon wxGxView::GetViewIcon(void)
{
    return m_Icon;
}

void wxGxView::SetViewIcon(wxIcon Icon)
{
    m_Icon = Icon;
}

int GxObjectCompareFunction(wxGxObject* const pObject1, wxGxObject* const pObject2, long sortData)
{
    if (!(pObject1 && pObject2))
        return -1;

	IGxObjectSort* pGxObjectSort1 = dynamic_cast<IGxObjectSort*>(pObject1);
    IGxObjectSort* pGxObjectSort2 = dynamic_cast<IGxObjectSort*>(pObject2);
    if(pGxObjectSort1 && !pGxObjectSort2)
		return sortData == 0 ? 1 : -1;
    if(!pGxObjectSort1 && pGxObjectSort2)
		return sortData == 0 ? -1 : 1;
    if(pGxObjectSort1 && pGxObjectSort2)
    {
        bool bAlwaysTop1 = pGxObjectSort1->IsAlwaysTop();
        bool bAlwaysTop2 = pGxObjectSort2->IsAlwaysTop();
        if(bAlwaysTop1 && !bAlwaysTop2)
		    return sortData == 0 ? 1 : -1;
        if(!bAlwaysTop1 && bAlwaysTop2)
		    return sortData == 0 ? -1 : 1;
        bool bSortEnables1 = pGxObjectSort1->IsSortEnabled();
        bool bSortEnables2 = pGxObjectSort2->IsSortEnabled();
        if(!bSortEnables1 || !bSortEnables1)
            return 0;
    }

    bool bDiscConnection1 = pObject1->IsKindOf(wxCLASSINFO(wxGxDiscConnection));
    bool bDiscConnection2 = pObject2->IsKindOf(wxCLASSINFO(wxGxDiscConnection));
    if(bDiscConnection1 && !bDiscConnection2)
		return sortData == 0 ? 1 : -1;
    if(!bDiscConnection1 && bDiscConnection2)
		return sortData == 0 ? -1 : 1;
    if(bDiscConnection1 && bDiscConnection2)
    {
        wxString sShareBeg = wxFileName::GetPathSeparator();
        sShareBeg += wxFileName::GetPathSeparator();
        bool bShare1 = pObject1->GetName().Left(2) == sShareBeg;//wxT("\\\\") ;
        bool bShare2 = pObject2->GetName().Left(2) == sShareBeg;//wxT("\\\\");
        if(bShare1 && !bShare2)
            return 1;
        if(!bShare1 && bShare2)
            return 0;
    }

    bool bContainerDst1 = pObject1->IsKindOf(wxCLASSINFO(wxGxDataset)) || pObject1->IsKindOf(wxCLASSINFO(wxGxDatasetContainer));
    bool bContainerDst2 = pObject2->IsKindOf(wxCLASSINFO(wxGxDataset)) || pObject2->IsKindOf(wxCLASSINFO(wxGxDatasetContainer));
    bool bContainer1 = pObject1->IsKindOf(wxCLASSINFO(wxGxObjectContainer));
    bool bContainer2 = pObject2->IsKindOf(wxCLASSINFO(wxGxObjectContainer));
    if(bContainer1 && !bContainerDst1 && bContainerDst2)
	    return sortData == 0 ? 1 : -1;
    if(bContainer2 && !bContainerDst2 && bContainerDst1)
	    return sortData == 0 ? -1 : 1;
    if(bContainer1 && !bContainer2)
	    return sortData == 0 ? 1 : -1;
    if(!bContainer1 && bContainer2)
	    return sortData == 0 ? -1 : 1;

	return pObject1->GetName().CmpNoCase(pObject2->GetName()) * (sortData == 0 ? -1 : 1);
}
