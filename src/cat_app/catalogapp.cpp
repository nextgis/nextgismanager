/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Main application class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2012,2013 Dmitry Baryshnikov
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
#include "wxgis/cat_app/catalogapp.h"
#include "wxgis/core/config.h"
#include "wxgis/app/splash.h"
#include "wxgis/core/format.h"

#include <locale.h>

//#include "ogrsf_frmts.h"
#include "ogr_api.h"
#include "gdal_priv.h"

#include "../../art/splash.xpm"

IMPLEMENT_APP(wxGISCatalogApp)

wxGISCatalogApp::wxGISCatalogApp(void) : wxApp()
{
    m_vendorName = wxString(VENDOR);
    m_vendorDisplayName = wxString(wxT("wxGIS"));

    m_appName = wxString(wxT("wxGISCatalog"));
    m_appDisplayName = wxString(wxT("NextGIS Manager"));
    m_className = wxString(wxT("wxGISCatalogApp"));

#if defined(__WXGTK__) && !wxCHECK_VERSION(2, 9, 5)
//    wxSetEnv(wxT("UBUNTU_MENUPROXY"), wxT("0"));
#endif
    m_pMainFrame = NULL;
    m_bMainFrameCreated = false;
}

wxGISCatalogApp::~wxGISCatalogApp(void)
{
 	GDALDestroyDriverManager();
	OGRCleanupAll();

	UnLoadLibs();
}

bool wxGISCatalogApp::OnInit()
{
    // call the base class initialization method, currently it only parses a
    // few common command-line options but it could be do more in the future
    if ( !wxApp::OnInit() )
    {
        wxLogError(_("Application initialization failed"));
		return false;
    }

    wxGISAppConfig oConfig = GetConfig();
    if (!oConfig.IsOk())
    {
        wxLogError(_("No valid configuration exist"));
		return false;
    }

    //create application/main frame
    m_pMainFrame = new wxGISCatalogFrame(NULL, wxID_ANY, wxString(_("wxGIS Catalog")), wxDefaultPosition, wxSize(800, 480) );

	//setup loging
	wxString sLogDir = oConfig.GetLogDir();
    if (!m_pMainFrame->SetupLog(sLogDir))
    {
        wxLogError(_("The log directory '%s' is not valid"), sLogDir.c_str());
        return false;
    }
    oConfig.ReportPaths();

	//setup locale
	wxString sLocale = oConfig.GetLocale();
	wxString sLocaleDir = oConfig.GetLocaleDir();
    if (!m_pMainFrame->SetupLoc(sLocale, sLocaleDir))
    {
        wxLogError(_("Setup locale failed. Locale files directory '%s', locale: %s"), sLocaleDir.c_str(), sLocale.c_str());
        return false;
    }

    //check time out and show splash
    wxXmlNode* pSplashNode = oConfig.GetConfigNode(enumGISHKCU, wxT("wxGISCommon/splash"));
    bool bShowSplash = GetBoolValue(pSplashNode, wxT("show"), true);//false;//
    if (bShowSplash)
    {
        long nTimeout = GetDecimalValue(pSplashNode, wxT("timeout"), 7000);
        wxBitmap splash_bmp = PrepareSplashScreen(m_pMainFrame, 0, 10);
        wxSplashScreen* pSplash = new wxSplashScreen(splash_bmp, wxSPLASH_CENTRE_ON_SCREEN | wxSPLASH_TIMEOUT, nTimeout, m_pMainFrame, wxID_ANY);
        //#if !defined(__WXGTK20__)
        // we don't need it at least on wxGTK with GTK+ 2.12.9
        wxYield();
        //#endif
    }

   	//setup sys
    wxString sSysDir = oConfig.GetSysDir();
    if (!m_pMainFrame->SetupSys(sSysDir))
    {
        wxLogError(_("System directory '%s' is not valid"), sSysDir.c_str());
        return false;
    }

   	//setup debug
	bool bDebugMode = oConfig.GetDebugMode();
    m_pMainFrame->SetDebugMode(bDebugMode);

    //disable loading drivers on GDAL_DRIVER_PATH env value
    CPLSetConfigOption("GDAL_DRIVER_PATH", "disabled");
    wxString sTempDir = oConfig.GetTempDir();
    if(!wxDirExists(sTempDir))
		wxFileName::Mkdir(sTempDir, 0777, wxPATH_MKDIR_FULL);

    CPLSetConfigOption("CPL_TMPDIR", sTempDir.mb_str(wxConvUTF8));


	OGRRegisterAll();
	GDALAllRegister();

	wxLogVerbose(_("wxGISCatalogApp: Start main frame"));

	//store values

	oConfig.SetLogDir(sLogDir);
	oConfig.SetLocale(sLocale);
	oConfig.SetLocaleDir(sLocaleDir);
	oConfig.SetSysDir(sSysDir);
	oConfig.SetDebugMode(bDebugMode);

    wxString sKey(wxT("wxGISCommon/libs"));
    //load libs
	wxXmlNode* pLibsNode = oConfig.GetConfigNode(enumGISHKCU, sKey);
	if(pLibsNode)
		LoadLibs(pLibsNode);
	pLibsNode = oConfig.GetConfigNode(enumGISHKLM, sKey);
	if(pLibsNode)
		LoadLibs(pLibsNode);

	return true;
}

int wxGISCatalogApp::OnExit()
{
//    SerializeLibs();
//the config state storing to files while destruction config class (smart pointer)
//on linux saving file in destructor produce segmentation fault
	wxGISAppConfig oConfig = GetConfig();
	if(oConfig.IsOk())
		oConfig.Save(enumGISHKCU);
    return 0;
}

// create the file system watcher here, because it needs an active loop
void wxGISCatalogApp::OnEventLoopEnter(wxEventLoopBase* loop)
{

    if(m_pMainFrame && !m_bMainFrameCreated)
    {
        if(!m_pMainFrame->CreateApp())
            return;

        m_bMainFrameCreated = true;
        m_pMainFrame->Show(true);
    }
}
