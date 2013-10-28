/******************************************************************************
 * Project:  wxGIS
 * Purpose:  Splash scree class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2013 Bishop
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

#include "wxgis/core/app.h"
#include <wx/splash.h>

/** \class wxGISSplashScreen splash.h
 *  \brief Standard splash for wxGIS applications
 */
class wxGISSplashScreen : public wxSplashScreen
{
	enum
	{
		ID_CHECK = wxID_HIGHEST + 1
	};
public:
	wxGISSplashScreen( int milliseconds = 6000, wxWindow *parent = NULL, wxWindowID id = wxID_ANY, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, long style = wxBORDER_NONE|wxFRAME_NO_TASKBAR|wxSTAY_ON_TOP );
	virtual ~wxGISSplashScreen();
    virtual int FilterEvent(wxEvent& event);
protected:
    wxCheckBox* m_checkBoxShow;
    IApplication* m_pApp;
};
