/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  wxAxToolboxView class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2011,2013 Bishop
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
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

#include "wxgis/geoprocessingui/geoprocessingui.h"
#include "wxgis/catalogui/gxview.h"
#include "wxgis/catalogui/gxtreeview.h"
//#include "wxgis/geoprocessingui/gxtoolexecview.h"


#include "wx/aui/aui.h"

/** @class wxGxToolboxTreeView
    
	The wxGxToolboxTreeView class is view with geoprocessing tools and it's categories (folders) in some kind of hierarchy.

	@library{gpui}
*/

class WXDLLIMPEXP_GIS_GPU wxGxToolboxTreeView : 
	public wxGxTreeView
{
    DECLARE_DYNAMIC_CLASS(wxGxToolboxTreeView)
public:
    wxGxToolboxTreeView(void);
	wxGxToolboxTreeView(wxWindow* parent, wxWindowID id = TREECTRLID, long style = wxTR_HAS_BUTTONS | wxTR_TWIST_BUTTONS | wxBORDER_NONE | wxTR_EDIT_LABELS);
	virtual ~wxGxToolboxTreeView(void);
    //wxGxTreeView
	virtual bool Activate(IApplication* const pApplication, wxXmlNode* const pConf);
	virtual void UpdateGxSelection(void);
};

/** @class wxAxToolboxView
    
	The class wxAxToolboxView is the dockable windown in wxGISCatalog, wxGISMap, etc. which host the wxGxToolboxTreeView view with tabs - geoprocessing tools tree, executed tool list, etc.

	@library{gpui}
*/
class WXDLLIMPEXP_GIS_GPU wxAxToolboxView :
	public wxAuiNotebook,
	public wxGxView
{
    DECLARE_DYNAMIC_CLASS(wxAxToolboxView)
public:
    wxAxToolboxView(void);
	wxAxToolboxView(wxWindow* parent, wxWindowID id = TOOLVIEWCTRLID, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
	virtual ~wxAxToolboxView(void);
//wxGxView
    virtual bool Create(wxWindow* parent, wxWindowID id = TOOLVIEWCTRLID, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxAUI_NB_BOTTOM | wxNO_BORDER | wxAUI_NB_TAB_MOVE, const wxString& name = wxT("ToolboxView"));
	virtual bool Activate(IApplication* const pApplication, wxXmlNode* const pConf);
	virtual void Deactivate(void);
protected:
	wxGISApplicationBase* m_pApp;
    wxGxToolboxTreeView* m_pGxToolboxView;
    //wxGxToolExecuteView *m_pGxToolExecuteView; //TODO: new/old view compatible for tray exec list view. panels with progress and buttons pause/start + priority with drag'n'drop
};
