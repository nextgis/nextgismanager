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
#include "wxgis/conf/confmain.h"
#include "wxgis/core/config.h"

//-----------------------------------------------------------------------------
// wxGISConfigApp
//-----------------------------------------------------------------------------

IMPLEMENT_APP_CONSOLE(wxGISConfigApp)

wxGISConfigApp::wxGISConfigApp(void) : wxAppConsole()
{
    m_vendorName = wxString(VENDOR); 
    m_vendorDisplayName = wxString(wxT("NextGIS"));
    m_appName = wxString(wxT("wxgisconfigapp"));
    m_appDisplayName = wxString(wxT("NextGIS manager console configurator"));
    m_className = wxString(wxT("wxGISConfigApp"));
}

wxGISConfigApp::~wxGISConfigApp(void)
{
}

void wxGISConfigApp::OnInitCmdLine(wxCmdLineParser& pParser)
{
    wxAppConsole::OnInitCmdLine(pParser);
    //common
    pParser.AddSwitch(wxT("v"), wxT("version"), _("The version of this program"));
    //main parameters
    pParser.AddSwitch(wxT("s"), wxT("set"), _("Set a value to config path"));
    pParser.AddSwitch(wxT("d"), wxT("del"), _("Delete a config path"));
    pParser.AddOption(wxT("k"), wxT("key"), _("Config key [HKLM|HKCU]"));
    pParser.AddOption(wxT("p"), wxT("path"), _("Path to value in config"));
    pParser.AddOption(wxT("pv"), wxT("path_value"), _("A value to set"));
    pParser.SetLogo(wxString::Format(_("The NextGIS Manager configuration utility (%s)\nAuthor: Dmitry Baryshnikov (aka Bishop), polimax@mail.ru\nCopyright (c) NextGIS %d"), GetAppVersionString().c_str(), __YEAR__));
}

void wxGISConfigApp::OnAppAbout(void)
{
    wxString out = wxString::Format(_("The NextGIS Manager configuration utility (%s)\nAuthor: Dmitry Baryshnikov (aka Bishop), polimax@mail.ru\nCopyright (c) NextGIS %d"), GetAppVersionString().c_str(), __YEAR__);
    wxFprintf(stdout, out);
}

bool wxGISConfigApp::OnCmdLineParsed(wxCmdLineParser& pParser)
{
    if (pParser.Found(wxT("v")))
    {
        OnAppAbout();
        exit(EXIT_SUCCESS);
        return true;
    }

    wxString sKey;
    if (!pParser.Found(wxT("k"), &sKey))
    {
        pParser.Usage();
        return false;
    }

    wxGISEnumConfigKey key;
    if (sKey.IsSameAs(wxT("HKLM"), false))
        key = enumGISHKLM;
    else if (sKey.IsSameAs(wxT("HKCU"), false))
        key = enumGISHKCU;
    else
    {
        pParser.Usage();
        return false;
    }

    wxString sPath;
    if (!pParser.Found(wxT("p"), &sPath))
    {
        pParser.Usage();
        return false;
    }

    bool bSet = pParser.Found(wxT("s"));
    bool bDel = pParser.Found(wxT("d"));

    if (bSet && bDel)
    {
        pParser.Usage();
        return false;
    }

    wxString sVal;
    if (!pParser.Found(wxT("pv"), &sVal) && !bDel)
    {
        pParser.Usage();
        return false;
    }

    wxGISAppConfig oConfig = GetConfig();
    if (!oConfig.IsOk())
    {
        wxFprintf(stdout, _("Get config failed"));
        return false;
    }



    if (bSet)
    {
        if (oConfig.Write(key, sPath, sVal))
            return oConfig.Save();
    }

    if (bDel)
    {
        if (oConfig.Delete(key, sPath))
            return oConfig.Save();
    }

    pParser.Usage();

    return wxAppConsole::OnCmdLineParsed(pParser);
}

bool wxGISConfigApp::OnExceptionInMainLoop()
{
    wxLogError(_("Unhandled Exception"));
    exit(EXIT_FAILURE);
}

void wxGISConfigApp::OnFatalException()
{
    wxLogError(_("Unhandled Exception"));
    exit(EXIT_FAILURE);
}

void wxGISConfigApp::OnUnhandledException()
{
    wxLogError(_("Unhandled Exception"));
    exit(EXIT_FAILURE);
}

int wxGISConfigApp::OnRun()
{
    return EXIT_SUCCESS;//success == true ? EXIT_SUCCESS : EXIT_FAILURE;//EXIT_FAILURE;//
}
