/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Catalog Views Commands class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2012 Dmitry Baryshnikov
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
#pragma once

#include "wxgis/framework/command.h"
#include "wxgis/framework/applicationbase.h"
#include "wxgis/catalogui/gxtabview.h"


enum wxGISEnumCatalogViewsCmdType{
    enumGISCatalogViewsCmdView = 0,
	enumGISCatalogViewsCmdSelectAll,
	enumGISCatalogViewsCmdTreePanel,
	enumGISCatalogViewsCmdFind,
    enumGISCatalogViewsCmdMax
};

/** @class wxGISCatalogViewsCmd

    The main catalog views commands includes: "ContentsView states", "Select All", "Show/hide tree view".

    @library {catalogui}
*/


class wxGISCatalogViewsCmd :
    public wxGISCommand,
    public IDropDownCommand
{
    DECLARE_DYNAMIC_CLASS(wxGISCatalogViewsCmd)
    
public:
	wxGISCatalogViewsCmd(void);
	virtual ~wxGISCatalogViewsCmd(void);
	//wxGISCommand
	virtual wxIcon GetBitmap(void);
	virtual wxString GetCaption(void);
	virtual wxString GetCategory(void);
	virtual bool GetChecked(void);
	virtual bool GetEnabled(void);
	virtual wxString GetMessage(void);
	virtual wxGISEnumCommandKind GetKind(void);
	virtual void OnClick(void);
	virtual bool OnCreate(wxGISApplicationBase* pApp);
	virtual wxString GetTooltip(void);
	virtual unsigned char GetCount(void);
    //IDropDownCommand
   	virtual wxMenu* GetDropDownMenu(void);
    virtual void OnDropDownCommand(int nID);
protected:
	wxGISApplicationBase* m_pApp;
    WINDOWARRAY m_anContentsWinIDs;
    wxWindow* m_pTreeView, *m_pFindView;
	wxIcon m_IconViews, m_IconSelAll, m_IconTreeView, m_IconFind;
};
