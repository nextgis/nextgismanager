/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISNewMenu class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011,2012,2014 Dmitry Baryshnikov
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
#include "wxgis/catalogui/gxcatalogui.h"
#include "wxgis/catalogui/gxselection.h"
#include "wxgis/framework/applicationbase.h"
#include "wxgis/framework/commandbar.h"

#define NEWMENUNAME wxT("Application.CreateNewMenu")
#define NEWMENUID ID_PLUGINCMDMAX - 1

/** @class wxGISNewMenu

    The create menu class. Consist of create new objects items

    @library {catalogui}
*/

class WXDLLIMPEXP_GIS_CLU wxGISNewMenu :
	public wxGISMenu,
	public wxGISCommand
{
public:
	wxGISNewMenu(const wxString& sName = NEWMENUNAME, const wxString& sCaption = _("Create"), wxGISEnumCommandBars type = enumGISCBSubMenu, const wxString& title = wxEmptyString, long style = 0);
	virtual ~wxGISNewMenu(void);
	//wxGISMenu
	virtual void AddCommand(wxGISCommand* pCmd);
	//virtual void RemoveCommand(size_t nIndex){};
	virtual void MoveCommandLeft(size_t nIndex){};
	virtual void MoveCommandRight(size_t nIndex){};
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
    //wxGISMenu
	virtual void Update(wxGxSelection* Selection);
protected:
	wxGISApplicationBase* m_pApp;
	wxGxCatalogUI* m_pCatalog;
	//wxGISConnectionPointContainer* m_pConnectionPointSelection;
	//long m_ConnectionPointSelectionCookie;
};


