/******************************************************************************
 * Project:  wxGIS (GIS Map)
 * Purpose:  wxMxApplication code.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011 Bishop
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

#include "wxgis/mapui/mxapplication.h"

//-----------------------------------------------
// wxMxApplication
//-----------------------------------------------
IMPLEMENT_CLASS(wxMxApplication, wxGISApplication)

wxMxApplication::wxMxApplication(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxGISApplicationEx(parent, id, title, pos, size, style)
{
}

wxMxApplication::~wxMxApplication(void)
{
}

bool wxMxApplication::Create(void)
{
	m_mgr.SetManagedWindow(this);

    wxGISApplication::Create();


	m_pMapView = new wxMxMapView(this);
	//if(m_pMapView->Activate(this, NULL))
 //   {
        bool bAdd = m_mgr.AddPane(m_pMapView, wxAuiPaneInfo().Name(wxT("main_map_window")).CenterPane());//.PaneBorder(true)
        RegisterChildWindow(m_pMapView);
		m_pMapView->Activate(this);
		//m_pTrackCancel = new ITrackCancel();
		//m_pTrackCancel->SetProgressor(GetStatusBar()->GetAnimation());
		//m_pMapView->SetTrackCancel(m_pTrackCancel);

    //}
    //else
    //    wxDELETE(m_pMapView);
	for(size_t i = 0; i < m_CommandBarArray.size(); ++i)
	{
		if(m_CommandBarArray[i]->GetType() == enumGISCBToolbar)
		{
			wxGISToolBar* pToolBar = dynamic_cast<wxGISToolBar*>(m_CommandBarArray[i]);
			if(pToolBar)
			{
				m_mgr.AddPane(pToolBar, wxAuiPaneInfo().Name(pToolBar->GetName()).Caption(pToolBar->GetCaption()).ToolbarPane().Top().Position(i).LeftDockable(pToolBar->GetLeftDockable()).RightDockable(pToolBar->GetRightDockable()).BestSize(-1,-1));
				pToolBar->Activate(this);
			}
		}
	}

	SerializeFramePosEx(false);

	m_mgr.Update();

	wxLogMessage(_("wxMxApplication: Creation complete"));

    return true;
}

