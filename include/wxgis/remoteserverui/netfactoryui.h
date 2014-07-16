/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  TCP network UI classes.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010,2012 Bishop
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

#include "wxgis/remoteserverui/remoteserverui.h"
#include "wxgis/net/tcp.h"

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/statbox.h>
#include <wx/panel.h>

/** \class INetConnFactoryUI netfactoryui.h
    \brief The network connection factory UI interface class.
*/
class INetConnFactoryUI
{
public:
    virtual ~INetConnFactoryUI(void){};
	virtual wxWindow* GetPropertyPage(wxWindow* pParent) = 0;
};

/** \class INetConnFactoryUI netfactoryui.h
    \brief The network connection factory UI interface class.
*/
class wxNetPropertyPage :
	public wxPanel
{
    DECLARE_ABSTRACT_CLASS(wxNetPropertyPage)
public:
	wxNetPropertyPage(INetConnFactoryUI* pFactory, wxWindow* pParent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxCLIP_CHILDREN | wxNO_BORDER | wxTAB_TRAVERSAL) : wxPanel(pParent, id, pos, size, style)
	{
		m_pFactory = pFactory;
	}
	virtual ~wxNetPropertyPage(void){};
	virtual wxXmlNode* OnSave(void) = 0;
	virtual wxString GetLastError(void){return m_sErrorMsg;};
protected:
	INetConnFactoryUI* m_pFactory;
	wxString m_sErrorMsg;
};


class wxClientTCPNetFactoryUI;
/** \class wxGISTCPClientPanel netfactoryui.h
    \brief The tcp network connection property page.
*/

class wxGISTCPClientPanel : public wxNetPropertyPage 
{
    DECLARE_CLASS(wxGISTCPClientPanel)
public:
  enum
  {
		ID_SEARCHBT = wxID_HIGHEST + 4501,
  };
public:
	wxGISTCPClientPanel(wxClientTCPNetFactoryUI* pFactory, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 280, 160 ) );
//	wxGISTCPClientPanel(INetConnection* pConn, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 280, 160 ) );
	~wxGISTCPClientPanel();
	//virtual void ShowSearch(bool bShow = false);
	//events
	void OnSearchBtnClick( wxCommandEvent& event );
	//wxNetPropertyPage
	virtual wxXmlNode* OnSave(void);	

private:
    wxString m_sPort;
    wxString m_UserName;
    wxString m_Password;
    wxString m_IPAddress;
    wxString m_sName;
	wxClientTCPNetFactory* m_pFactory;

protected:
	wxStaticText* m_staticText01;
	wxTextCtrl* m_textCtr_name;
	wxStaticText* m_staticText11;
	wxTextCtrl* m_textCtr_addr;
	wxStaticText* m_staticText12;
	wxTextCtrl* m_textCtr_port;
	wxButton* m_button_search;
	wxStaticText* m_staticText13;
	wxTextCtrl* m_textCtrl_user;
	wxStaticText* m_staticText14;
	wxTextCtrl* m_textCtrl_pass;
private:
	DECLARE_EVENT_TABLE()	
};

/** \class wxClientTCPNetFactoryUI netfactoryui.h
    \brief The UI factory to create net connections.
*/
class wxClientTCPNetFactoryUI : 
	public wxClientTCPNetFactory,
	public INetConnFactoryUI
{
    DECLARE_DYNAMIC_CLASS(wxClientTCPNetFactoryUI)
public:
	wxClientTCPNetFactoryUI(void);
	~wxClientTCPNetFactoryUI(void);
	//INetConnFactoryUI
	virtual wxWindow* GetPropertyPage(wxWindow* pParent);
};


