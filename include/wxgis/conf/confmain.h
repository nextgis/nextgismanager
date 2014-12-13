/******************************************************************************
* Project:  wxGIS (GIS common)
* Purpose:  wxGIS config modification application.
* Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
******************************************************************************
*   Copyright (C) 2012-2014 Dmitry Baryshnikov
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

#include "wxgis/core/config.h"
#include "wxgis/version.h"
#include "wxgis/core/app.h"

#include <wx/app.h>
#include <wx/cmdline.h>

/** @class wxGISConfigApp

    Console utility to modify configuration files
*/
class wxGISConfigApp :
    public wxAppConsole
{
public:
    wxGISConfigApp(void);
    virtual ~wxGISConfigApp(void);
    //wxAppConsole
    virtual int OnRun();
    void OnInitCmdLine(wxCmdLineParser& pParser);
    bool OnCmdLineParsed(wxCmdLineParser& pParser);
    virtual wxString GetAppVersionString(void) const { return wxString(wxGIS_VERSION_NUM_DOT_STRING_T); };
    virtual void OnAppAbout(void);
    //exceptions
    virtual bool 	OnExceptionInMainLoop();
    virtual void 	OnFatalException();
    virtual void 	OnUnhandledException();
};

DECLARE_APP(wxGISConfigApp)