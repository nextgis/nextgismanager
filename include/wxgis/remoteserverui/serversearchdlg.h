/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  wxGISSearchServerDlg class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010,2012 Bishop
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

#include "wxgis/net/netfactory.h"
#include "wxgis/net/netevent.h"

#include <wx/listctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/button.h>
#include <wx/sizer.h>
//#include <wx/gauge.h>
#include <wx/dialog.h>
#include <wx/imaglist.h>
#include <wx/icon.h>
#include <wx/socket.h>
#include <wx/log.h>
#include <wx/msgdlg.h>
#include <wx/xml/xml.h>

/** \class wxGISSearchServerDlg serversearchdlg.h
    \brief The dialog to search remote servers.
*/

class wxGISSearchServerDlg : 
	public wxDialog
{
    DECLARE_CLASS(wxGISSearchServerDlg)
public:
  enum
  {
		ID_SEARCHBT = wxID_HIGHEST + 4511,
		ID_STOPBT,
		ID_ACCEPT
  };
public:
	wxGISSearchServerDlg(INetConnFactory* pFactory, wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Search Server"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 400,300 ), long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER );
	~wxGISSearchServerDlg();
	virtual wxXmlNode* GetConnectionProperties() const {return m_pConnProps;};
	//virtual INetClientConnection* GetConnection();
protected:
	virtual void CreateControls(bool bShowGauge);
	virtual void AddServer(wxXmlNode* pServerData);
    //events
	void OnAccept( wxCommandEvent& event );
	void OnAcceptUI( wxUpdateUIEvent& event );
    void OnSearch( wxCommandEvent& event );
    void OnSearchUI( wxUpdateUIEvent& event );
	void OnStop( wxCommandEvent& event );
	void OnStopUI( wxUpdateUIEvent& event );
    void OnNetMsg ( wxGISNetEvent& event );
    /*void OnClose(wxCloseEvent& event);*/
protected:
	wxListCtrl* m_listCtrl;
	wxButton* m_button_search;
	wxButton* m_button_stop;
	wxButton* m_button_accept;
	//wxGauge* m_gauge;	

	wxString m_ipaddress;
	wxImageList m_ImageList;
	INetConnFactory* m_pFactory;
	wxXmlNode* m_pConnProps;

    long m_nConnectionPointCookie;

private:
	DECLARE_EVENT_TABLE()		
};
