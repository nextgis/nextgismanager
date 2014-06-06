/******************************************************************************
 * Project:  wxGIS
 * Purpose:  Splash screen functions.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2013,2014 Dmitry Baryshnikov
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

wxBitmap PrepareSplashScreen(const IApplication *pApp, int nDeltH, int nDeltaV)
{
    wxBitmap bkBitmap(splash_xpm);
    int width(0), height(0);
    int nHCenter = bkBitmap.GetWidth() / 2;
    int nVCenter = bkBitmap.GetHeight() / 2;

    wxMemoryDC splash_dc(bkBitmap);
    splash_dc.SetPen(*wxBLACK_PEN);
    wxFont font_n(18, wxFONTFAMILY_DEFAULT , wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false);
    splash_dc.SetFont(font_n);
    wxString sName = pApp->GetAppDisplayNameShort();

    splash_dc.GetTextExtent(sName, &width, &height);
    splash_dc.DrawText(sName, wxPoint(nHCenter  - width / 2, nVCenter + 10 - height / 2));

    wxFont font_v(12, wxFONTFAMILY_DEFAULT , wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false);
    splash_dc.SetFont(font_v);
    wxString v = wxString::Format(_("Version: %s"), pApp->GetAppVersionString().c_str());
    splash_dc.GetTextExtent(v, &width, &height);
    nDeltaV += 40;
    splash_dc.DrawText(v, wxPoint(nHCenter  - width / 2, nVCenter + nDeltaV - height / 2));
    wxString d = wxString::Format(_("Build: %s"), wxString(__DATE__,wxConvLibc).c_str());
    splash_dc.GetTextExtent(d, &width, &height);
    nDeltaV += 20;
    splash_dc.DrawText(d, wxPoint(nHCenter  - width / 2, nVCenter + nDeltaV - height / 2));

    splash_dc.SetBrush(*wxTRANSPARENT_BRUSH);
    splash_dc.DrawRectangle(wxRect(0, 0, bkBitmap.GetWidth(), bkBitmap.GetHeight()));

    splash_dc.SelectObject(wxNullBitmap);

    return bkBitmap;
}



