/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISStatusBar class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2012,2013 Bishop
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
#include "wxgis/framework/statusbar.h"

#include "wxgis/framework/applicationbase.h"

#include "../../art/globe_imglst14.xpm"

IMPLEMENT_DYNAMIC_CLASS(wxGISStatusBar, wxStatusBar)

BEGIN_EVENT_TABLE(wxGISStatusBar, wxStatusBar)
	EVT_SIZE(wxGISStatusBar::OnSize)
	EVT_TIMER( TIMER_ID, wxGISStatusBar::OnTimer )
	EVT_RIGHT_DOWN(wxGISStatusBar::OnRightDown)
	EVT_COMMAND(wxID_ANY, wxEVT_COMMAND_BUTTON_CLICKED, wxGISStatusBar::OnMessage)
END_EVENT_TABLE()

wxGISStatusBar::wxGISStatusBar() : wxStatusBar()
{
}

wxGISStatusBar::wxGISStatusBar(wxWindow *parent, wxWindowID id, long style, const wxString& name, WXDWORD panelsstyle) : wxStatusBar(parent, id, style, name), m_timer(this, TIMER_ID), m_pAni(NULL), m_pProgressBar(NULL)
{
    m_MsgPos = wxNOT_FOUND;
    m_AniPos = wxNOT_FOUND;
    m_ProgressPos = wxNOT_FOUND;
    m_PositionPos = wxNOT_FOUND;
    m_ClockPos = wxNOT_FOUND;
    m_PagePositionPos = wxNOT_FOUND;
    m_SizePos = wxNOT_FOUND;
    m_CapsLockPos = wxNOT_FOUND;
    m_NumLockPos = wxNOT_FOUND;
    m_ScrollLockPos = wxNOT_FOUND;
    m_Panels = panelsstyle;

	m_pApp = dynamic_cast<wxGISApplicationBase*>(parent);

	std::vector<STATUSPANEL> panels;
	int counter(0);
	if(panelsstyle & enumGISStatusMain)
	{
		STATUSPANEL data = {wxNOT_FOUND, wxSB_FLAT};
		panels.push_back(data);
		m_MsgPos = counter;
		counter++;
	}
	if(panelsstyle & enumGISStatusProgress)
	{
		STATUSPANEL data = {100, wxSB_FLAT};//wxSB_NORMAL
		panels.push_back(data);
		m_pProgressBar = new wxGISProgressor(this, wxID_ANY);
		m_pProgressBar->Hide();
		m_ProgressPos = counter;
		counter++;
	}
	if(panelsstyle & enumGISStatusAnimation)
	{
		STATUSPANEL data = {23, wxSB_NORMAL};
		panels.push_back(data);
		//bitmap
		m_pAni = new wxGISAnimation(this, wxID_ANY, wxBitmap(globe_imglst14_xpm),14);
		m_pAni->Hide();
		m_AniPos = counter;
		counter++;
	}
	if(panelsstyle & enumGISStatusPosition)
	{
#ifdef __WXGTK__
		STATUSPANEL data = {225, wxSB_NORMAL};
#else
		STATUSPANEL data = {200, wxSB_NORMAL};
#endif
		panels.push_back(data);
        m_PositionPos = counter;
		counter++;
	}
	if(panelsstyle & enumGISStatusPagePosition)
	{
		STATUSPANEL data = {100, wxSB_NORMAL};
		panels.push_back(data);
        m_PagePositionPos = counter;
		counter++;
	}
	if(panelsstyle & enumGISStatusSize)
	{
		STATUSPANEL data = {80, wxSB_NORMAL};
		panels.push_back(data);
        m_SizePos = counter;
		counter++;
	}
	if(panelsstyle & enumGISStatusCapsLock)
	{
		STATUSPANEL data = {40, wxSB_NORMAL};
		panels.push_back(data);
        m_CapsLockPos = counter;
		counter++;
	}
	if(panelsstyle & enumGISStatusNumLock)
	{
		STATUSPANEL data = {40, wxSB_NORMAL};
		panels.push_back(data);
        m_NumLockPos = counter;
		counter++;
	}
	if(panelsstyle & enumGISStatusScrollLock)
	{
		STATUSPANEL data = {40, wxSB_NORMAL};
		panels.push_back(data);
        m_ScrollLockPos = counter;
		counter++;
	}
	if(panelsstyle & enumGISStatusClock)
	{
#ifdef __WXGTK__
		STATUSPANEL data = {64, wxSB_NORMAL};
#else
		STATUSPANEL data = {48, wxSB_NORMAL};
#endif
		panels.push_back(data);
		m_ClockPos = counter;
		counter++;
	}
	//STATUSPANEL data = {1, wxSB_FLAT};
	//panels.push_back(data);
	//STATUSPANEL data1 = {15, wxSB_FLAT};
	//panels.push_back(data1);

	int *STATUSBAR_Sizes = new int[panels.size()];
	int *STATUSBAR_Styles = new int[panels.size()];
	for(size_t i = 0; i < panels.size(); ++i)
	{
		STATUSBAR_Sizes[i] = panels[i].size;
		STATUSBAR_Styles[i] = panels[i].style;
	}
	SetFieldsCount(panels.size(), STATUSBAR_Sizes);
	SetStatusStyles(panels.size(), STATUSBAR_Styles);

	delete [] STATUSBAR_Sizes;
	delete [] STATUSBAR_Styles;

	//set text & start timer
	if(panelsstyle & enumGISStatusClock)
	{
		SetMessage(wxDateTime::Now().Format(_("%H:%M")), m_ClockPos);
		m_timer.Start(30000);    // 1 minute interval
	}
}

wxGISStatusBar::~wxGISStatusBar(void)
{
	wxDELETE(m_pAni);
}


void wxGISStatusBar::OnSize(wxSizeEvent &event)
{
	wxRect	r;
	if( m_pAni && GetFieldRect(m_AniPos, r) )
	{
		r.Deflate(2);
		m_pAni->SetSize(r);
	}

	if( m_pProgressBar && GetFieldRect(m_ProgressPos, r) )
	{
		//r.Deflate(2);
		m_pProgressBar->SetSize(r);
	}
	event.Skip();
}

void wxGISStatusBar::SetMessage(const wxString& text, int i)
{
    wxCommandEvent MessageEvent( wxEVT_COMMAND_BUTTON_CLICKED ); // Keep it simple, don't give a specific event ID
    MessageEvent.SetString(text);
    MessageEvent.SetInt(i);
    wxPostEvent(this, MessageEvent);
}

void wxGISStatusBar::OnMessage(wxCommandEvent &event)
{
	SetStatusText(event.GetString(), event.GetInt());
}

wxString wxGISStatusBar::GetMessage(int i) const
{
	return GetStatusText(i);
}

void wxGISStatusBar::OnTimer( wxTimerEvent& event )
{
	SetMessage(wxDateTime::Now().Format(_("%H:%M")), m_ClockPos);
}


void wxGISStatusBar::OnRightDown(wxMouseEvent& event)
{
	event.Skip();
	m_pApp->ShowToolBarMenu();
}

int wxGISStatusBar::GetPanelPos(wxGISEnumStatusBarPanels nPanel)
{
    switch(nPanel)
    {
    case enumGISStatusMain:
        return m_MsgPos;
    case enumGISStatusAnimation:
        return m_AniPos;
    case enumGISStatusPosition:
        return m_PositionPos;
    case enumGISStatusClock:
        return m_ClockPos;
    case enumGISStatusProgress:
        return m_ProgressPos;
    case enumGISStatusPagePosition:
        return m_PagePositionPos;
    case enumGISStatusSize:
        return m_SizePos;
    case enumGISStatusCapsLock:
        return m_CapsLockPos;
    case enumGISStatusNumLock:
        return m_NumLockPos;
    case enumGISStatusScrollLock:
        return m_ScrollLockPos;
    default:
        return -1;
    }
    return -1;
}
