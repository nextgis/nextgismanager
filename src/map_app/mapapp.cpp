/******************************************************************************
 * Project:  wxGIS (GIS Map)
 * Purpose:  Main application class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 20011 Bishop
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
#include "wxgis/map_app/mapapp.h"
#include "wxgis/map_app/mapframe.h"
//#include "wxgis/catalog/catalog.h"

#include <locale.h>

#include "gdal.h"
#include "ogrsf_frmts/ogrsf_frmts.h"
#include "ogr_api.h"

IMPLEMENT_APP(wxGISMapApp);

wxGISMapApp::wxGISMapApp(void)
{
}

wxGISMapApp::~wxGISMapApp(void)
{
	SerializeLibs();

	GDALDestroyDriverManager();
	OGRCleanupAll();

	ReleaseConfig();

	UnLoadLibs();
}

bool wxGISMapApp::OnInit()
{
	wxGISMapFrame* frame = new wxGISMapFrame(NULL, wxID_ANY, _("wxGIS Map"), wxDefaultPosition, wxSize(800, 480) );
	wxGISAppConfigSPtr pConfig = GetConfig();
	if(!pConfig)
		return false;

	//setup loging
	wxString sLogDir = pConfig->GetLogDir();
    if(!frame->SetupLog(sLogDir))
        return false;

	//setup locale
	wxString sLocale = pConfig->GetLocale();
	wxString sLocaleDir = pConfig->GetLocaleDir();
    if(!frame->SetupLoc(sLocale, sLocaleDir))
        return false;

   	//setup sys
    wxString sSysDir = pConfig->GetSysDir();
    if(!frame->SetupSys(sSysDir))
        return false;

   	//setup debug
	bool bDebugMode = pConfig->GetDebugMode();
    frame->SetDebugMode(bDebugMode);

    //some default GDAL
	wxString sGDALCacheMax = pConfig->Read(enumGISHKCU, wxString(wxT("wxGISCommon/GDAL/cachemax")), wxString(wxT("128")));
	CPLSetConfigOption( "GDAL_CACHEMAX", sGDALCacheMax.mb_str() );
    //CPLSetConfigOption ( "LIBKML_USE_DOC.KML", "no" );
    //GDAL_MAX_DATASET_POOL_SIZE
    //OGR_ARC_STEPSIZE

	OGRRegisterAll();
	GDALAllRegister();

	wxLogMessage(_("wxGISMapApp: Start main frame"));

	//store values
	pConfig->SetLogDir(sLogDir);
	pConfig->SetLocale(sLocale);
	pConfig->SetLocaleDir(sLocaleDir);
	pConfig->SetSysDir(sSysDir);
	pConfig->SetDebugMode(bDebugMode);
	//gdal
	pConfig->Write(enumGISHKCU, wxString(wxT("wxGISCommon/GDAL/cachemax")), sGDALCacheMax);

    //load libs
	wxXmlNode* pLibsNode = pConfig->GetConfigNode(enumGISHKCU, wxString(wxT("wxGISCommon/libs")));
	if(pLibsNode)
		LoadLibs(pLibsNode);
	pLibsNode = pConfig->GetConfigNode(enumGISHKLM, wxString(wxT("wxGISCommon/libs")));
	if(pLibsNode)
		LoadLibs(pLibsNode);

    if(!frame->Create())
        return false;
    SetTopWindow(frame);
    frame->Show();

	return true;
}
