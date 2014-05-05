/******************************************************************************
 * Project:  wxGIS
 * Purpose:  Splash scree class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2013 Bishop
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

#include "wxgis/app/splash.h"
#include "wxgis/core/config.h"

#include <wx/font.h>

#include "../../art/splash.xpm"

//----------------------------------------------------------------------------
// wxGISSplashScreen
//----------------------------------------------------------------------------

wxGISSplashScreen::wxGISSplashScreen(int milliseconds, wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style) : wxSplashScreen(wxBitmap(splash_xpm), wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_TIMEOUT, milliseconds, parent, id, pos, size, style)
{
    m_checkBoxShow = NULL;

    m_pApp = dynamic_cast<IApplication*>(parent);
    wxWindow *win = GetSplashWindow();

#ifdef __WXMSW__
    wxWindowDC dc(win);
    int width, height;

    wxSize sz = GetSize();
    int nHCenter = sz.GetWidth() / 2;
    int nVCenter = sz.GetHeight() / 2;

	wxFont font_n(18, wxFONTFAMILY_DEFAULT , wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false);
    dc.SetFont(font_n);
    wxString sName = wxString::Format(wxT("%s"), m_pApp->GetAppDisplayNameShort().c_str());
    dc.GetTextExtent(sName, &width, &height);
    dc.DrawText(sName, wxPoint(nHCenter  - width / 2, nVCenter + 10 - height / 2));

	wxFont font_v(12, wxFONTFAMILY_DEFAULT , wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false);
    dc.SetFont(font_v);
    wxString v = wxString::Format(_("Version: %s"), m_pApp->GetAppVersionString().c_str());
    dc.GetTextExtent(v, &width, &height);
    dc.DrawText(v, wxPoint(nHCenter  - width / 2, nVCenter + 50 - height / 2));
    wxString d = wxString::Format(_("Build: %s"), wxString(__DATE__,wxConvLibc).c_str());
    dc.GetTextExtent(d, &width, &height);
    dc.DrawText(d, wxPoint(nHCenter  - width / 2, nVCenter + 70 - height / 2));

    int nWidth = sz.GetWidth() - 1;
    int nHeight = sz.GetHeight() - 1;
    dc.DrawLine(1, 1, nWidth, 1);
    dc.DrawLine(nWidth, 1, nWidth, nHeight);
    dc.DrawLine(nWidth, nHeight, 1, nHeight);
    dc.DrawLine(1, nHeight, 1, 1);

    m_checkBoxShow = new wxCheckBox( win, ID_CHECK, _("Don't show this screen in future"), wxPoint(10, sz.GetHeight() - 25) );
#else
    m_checkBoxShow = new wxCheckBox( win, ID_CHECK, _("Don't show this screen in future"), wxPoint(10, GetSize().GetHeight() - 60) );
#endif
    m_checkBoxShow->SetBackgroundColour(*wxWHITE);
}

wxGISSplashScreen::~wxGISSplashScreen()
{
    wxGISAppConfig oConfig = GetConfig();
	if(!oConfig.IsOk() || m_checkBoxShow == NULL)
        return;
    wxCheckBoxState st = m_checkBoxShow->Get3StateValue();
    bool bShow = st == wxCHK_CHECKED ? false : true;
    oConfig.Write(enumGISHKCU, wxT("wxGISCommon/splash/show"), bShow);
}

int wxGISSplashScreen::FilterEvent(wxEvent& event)
{
    if(event.GetId() == ID_CHECK)
    {
        return -1;
    }

     wxWindow *win = GetSplashWindow();

#ifdef __WXGTK__
    wxWindowDC dc(win);
    int width, height;

    wxSize sz = GetSize();
    int nHCenter = sz.GetWidth() / 2;
    int nVCenter = sz.GetHeight() / 2;
    int nWidth = sz.GetWidth() - 3;
    int nHeight = sz.GetHeight() - 30;

    wxFont font_n(16, wxFONTFAMILY_DEFAULT , wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false);
    dc.SetFont(font_n);
    //wxString sName = wxString::Format(wxT("%s"), m_pApp->GetUserAppNameShort().c_str());
    wxString sName = wxString::Format(wxT("%s"), m_pApp->GetAppDisplayNameShort().c_str());
    dc.GetTextExtent(sName, &width, &height);
    dc.DrawText(sName, wxPoint(nHCenter  - width / 2, nVCenter - height / 2 - 5));

    wxFont font_v(10, wxFONTFAMILY_DEFAULT , wxFONTSTYLE_NORMAL, wxFONTWEIGHT_LIGHT, false);
    dc.SetFont(font_v);
    wxString v = wxString::Format(_("Version: %s"), m_pApp->GetAppVersionString().c_str());
    dc.GetTextExtent(v, &width, &height);
    dc.DrawText(v, wxPoint(nHCenter  - width / 2, nVCenter + 15 - height / 2));
    wxString d = wxString::Format(_("Build: %s"), wxString(__DATE__,wxConvLibc).c_str());
    dc.GetTextExtent(d, &width, &height);
    dc.DrawText(d, wxPoint(nHCenter  - width / 2, nVCenter + 30 - height / 2));

    dc.DrawLine(0, 0, nWidth, 0);
    dc.DrawLine(nWidth, 0, nWidth, nHeight);
    dc.DrawLine(nWidth, nHeight, 0, nHeight);
    dc.DrawLine(0, nHeight, 0, 0);
#endif

    return wxSplashScreen::FilterEvent(event);
}


