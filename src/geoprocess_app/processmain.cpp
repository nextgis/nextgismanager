/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  Main application class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010,2011 Bishop
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

#include "wxgis/geoprocess/processmain.h"
#include "wxgis/geoprocessing/geoprocessing.h"
#include "wxgis/geoprocessing/gptoolmngr.h"
#include "wxgis/core/config.h"
#include "wxgis/catalog/gxcatalog.h"
#include "wxgis/version.h"

#include <wx/app.h>
//#include <wx/defs.h>
#include <wx/cmdline.h>
//#include <wx/init.h>
//#include <wx/wxcrtvararg.h>

#include <locale.h>


#ifdef HAVE_PROJ
    #include "proj_api.h"
#endif

int main(int argc, char **argv)
{
#if wxUSE_UNICODE
    wxChar **wxArgv = new wxChar *[argc + 1];
    {
        int n;

        for (n = 0; n < argc; n++ )
        {
            wxMB2WXbuf warg = wxConvertMB2WX(argv[n]);
            wxArgv[n] = wxStrdup(warg);
        }

        wxArgv[n] = NULL;
    }
#else
    #define wxArgv argv
#endif // wxUSE_UNICODE

	wxApp::CheckBuildOptions(WX_BUILD_OPTIONS_SIGNATURE, "program");

    wxInitializer initializer;
    if ( !initializer )
    {
        fprintf(stderr, _("Failed to initialize the wxWidgets library, aborting.").mb_str(wxConvLocal));
        return -1;
    }

	bool success( false );

    // Create the commandline parser
    static const wxCmdLineEntryDesc my_cmdline_desc[] =
    {
        { wxCMD_LINE_SWITCH, "h", "help",       _( "Show this help message" ).mb_str(wxConvLocal), wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
        { wxCMD_LINE_SWITCH, "v", "version",    _( "The version of this program" ).mb_str(wxConvLocal) },
		{ wxCMD_LINE_OPTION, "n", "name",       _( "The tool name" ).mb_str(wxConvLocal), wxCMD_LINE_VAL_STRING },
		{ wxCMD_LINE_OPTION, "p", "parameters", _( "The tool parameters" ).mb_str(wxConvLocal), wxCMD_LINE_VAL_STRING },
		{ wxCMD_LINE_NONE }
    };

    wxCmdLineParser my_parser( my_cmdline_desc, argc, wxArgv );
    my_parser.SetLogo(wxString::Format(_("The wxGISGeoprocess (%s)\nAuthor: Bishop (aka Baryshnikov Dmitriy), polimax@mail.ru\nCopyright (c) 2010-2011"), wxGIS_VERSION_NUM_STRING_T));

    // Parse command line arguments
    success = parse_commandline_parameters(my_parser);

#if wxUSE_UNICODE
    {
        for ( int n = 0; n < argc; n++ )
            free(wxArgv[n]);

        delete [] wxArgv;
    }
#endif // wxUSE_UNICODE

    wxUnusedVar(argc);
    wxUnusedVar(argv);

	return success == true ? EXIT_SUCCESS : EXIT_FAILURE;
}

bool parse_commandline_parameters( wxCmdLineParser& parser )
{
    // Parse the parameters
    int my_parse_success = parser.Parse( );
    // Print help if the specify /? or if a syntax error occured.
    if( my_parse_success != 0 )
    {
        return false;
    }

	if( parser.Found( wxT( "v" ) ) )
	{
	    wxString out = wxString::Format(_("The wxGISGeoprocess (%s)\nAuthor: Bishop (aka Baryshnikov Dmitriy), polimax@mail.ru\nCopyright (c) 2010-2011\n"), wxGIS_VERSION_NUM_STRING_T);
	    wxFprintf(stdout, out);
		return true;
	}


    if(parser.Found( wxT( "n" )) && parser.Found( wxT( "p" )) )
    {
        wxGPTaskExecutor executor;
		bool success = executor.Initialize(wxT("wxGISGeoprocess"), wxString(wxT("gp")), parser);
		executor.Uninitialize();
		return success;
    }

	parser.Usage();

    // Either we are using the defaults or the provided parameters were valid.

    return true;

}

//////////////////////////////////////////////////////////////////////////////
// wxGPTaskExecutor
//////////////////////////////////////////////////////////////////////////////

wxGPTaskExecutor::wxGPTaskExecutor() : ITrackCancel(), wxGISInitializer()
{
	m_nRange = 100;
    SetProgressor(this);
}

wxGPTaskExecutor::~wxGPTaskExecutor()
{
}

bool wxGPTaskExecutor::OnExecute(wxString sToolName, wxString sToolParameters)
{
	//create output stream
    m_StdOutFile.Attach(stdout);
	wxFFileOutputStream OutStream(m_StdOutFile);
	m_pOutTxtStream = new wxTextOutputStream(OutStream);

//    wxGISAppConfig* pConfig;
//#ifdef WXGISPORTABLE
//    pConfig = new wxGISAppConfig(TOOLBX_NAME, CONFIG_DIR, true);
//#else
//    pConfig = new wxGISAppConfig(TOOLBX_NAME, CONFIG_DIR, false);
//#endif
//    wxXmlNode* pToolsChild = pConfig->GetConfigNode(wxT("tools"), false, true);
//    if(!pToolsChild)
//    {
//        PutMessage(wxString(_("Get wxGISToolbox config failed!")), -1, enumGISMessageErr);
//        return false;
//    }

    //PutMessage(wxString::Format(_("tool - %s params - %s"), sToolName.c_str(), sToolParameters.c_str()), -1, enumGISMessageInfo);
    //PutMessage(wxString::Format(_("tool - %s"), sToolName.c_str()), -1, enumGISMessageInfo);
    //PutMessage(wxString::Format(_("params - %s"), sToolParameters.c_str()), -1, enumGISMessageInfo);


    wxGxCatalog GxCatalog;
    GxCatalog.Init();

    wxGISGPToolManager oGISGPToolManager;
	if(!oGISGPToolManager.IsOk())
	{
        PutMessage(wxString(_("Get wxGISToolbox config failed!")), -1, enumGISMessageErr);
		return false;
	}

    IGPToolSPtr pTool = oGISGPToolManager.GetTool(sToolName, &GxCatalog);
    if(!pTool)
    {
        PutMessage(wxString::Format(_("Get Geoprocessing tool %s failed!"), sToolName.c_str()), -1, enumGISMessageErr);
        return false;
    }

    if(!pTool->SetFromString(sToolParameters))
    {
        PutMessage(wxString::Format(_("Set Geoprocessing tool %s parameters failed!"), sToolName.c_str()), -1, enumGISMessageErr);
        return false;
    }

    if(!pTool->Validate())
    {
        PutMessage(wxString::Format(_("Geoprocessing tool %s validation failed!"), sToolName.c_str()), -1, enumGISMessageErr);
        return false;
    }

    bool bResult = pTool->Execute(this);

    GxCatalog.Detach();

    OutStream.Close();
    wxSleep(1);
    wxDELETE(m_pOutTxtStream);
    return bResult;
}

void wxGPTaskExecutor::PutMessage(wxString sMessage, size_t nIndex, wxGISEnumMessageType nType)
{
    if(m_sPrevMsg == sMessage)
        return;
    wxString sOut;

    switch(nType)
    {
    case enumGISMessageErr:
        sOut.Append(wxT("ERR: "));
        break;
    case enumGISMessageWarning:
        sOut.Append(wxT("WARN: "));
        break;
    case enumGISMessageQuestion:
    case enumGISMessageUnk:
    case enumGISMessageNorm:
    case enumGISMessageInfo:
    case enumGISMessageTitle:
    case enumGISMessageOK:
        sOut.Append(wxT("INFO: "));
        break;
    default:
        break;
    }
    sOut.Append(sMessage);
    sOut.Append(wxT("\r\n"));
    m_pOutTxtStream->WriteString(sOut);
	m_StdOutFile.Flush();
	//wxFprintf(stdout, sOut);
	//wxPrintf(sOut);
    m_sPrevMsg = sMessage;
}

void wxGPTaskExecutor::SetValue(int value)
{
	int nPercent = double(value) * 100 / m_nRange;
    if(m_nValue == nPercent)
        return;
    m_nValue = nPercent;
	wxString sOut = wxString::Format(wxT("DONE: %d%%\r\n"), nPercent);
	//wxFprintf(stdout, sOut);
	//wxPrintf(sOut);

    m_pOutTxtStream->WriteString(sOut);
	m_StdOutFile.Flush();
}

bool wxGPTaskExecutor::Initialize(const wxString &sAppName, const wxString &sLogFilePrefix, wxCmdLineParser& parser)
{
	wxGISAppConfig oConfig = GetConfig();
	if(!oConfig.IsOk())
		return false;

	//GDAL
	wxString sGDALCacheMax = oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/GDAL/cachemax")), wxString(wxT("128")));
	CPLSetConfigOption( "GDAL_CACHEMAX", sGDALCacheMax.mb_str() );
    wxString sSysPath = oConfig.GetSysDir();
	wxString sGdalDataDir = sSysPath + wxFileName::GetPathSeparator() + wxString(wxT("gdal")) + wxFileName::GetPathSeparator();
	CPLSetConfigOption("GDAL_DATA", sGdalDataDir.mb_str(wxConvUTF8) );
#ifdef HAVE_PROJ
	sGdalDataDir = sSysPath + wxFileName::GetPathSeparator() + wxString(wxT("proj")) + wxFileName::GetPathSeparator();
	//CPLSetConfigOption("PROJ_LIB", sGdalDataDir.mb_str(wxConvUTF8) );
    CPLString pszPROJ_LIB(sGdalDataDir.mb_str(wxConvUTF8));
    const char *path = pszPROJ_LIB.c_str();
    pj_set_searchpath(1, &path);
#endif

	OGRRegisterAll();
	GDALAllRegister();
	//END GDAL

	if(!wxGISInitializer::Initialize(sAppName, sLogFilePrefix, parser))
		return false;

    wxString sToolName, sToolParameters;
    if(parser.Found( wxT( "n" ), &sToolName ) && parser.Found( wxT( "p" ), &sToolParameters ) )
	{
		sToolParameters = sToolParameters.Trim(true).Trim(false);
        if(sToolParameters.StartsWith(wxString("\"")))
			sToolParameters = sToolParameters.Mid(1, sToolParameters.Len() - 2);
		return OnExecute(sToolName.Trim(true).Trim(false), sToolParameters);
	}
	return false;
}

void wxGPTaskExecutor::Uninitialize()
{
	//GDAL
	GDALDestroyDriverManager();
	OGRCleanupAll();
	//END GDAL

	wxGISInitializer::Uninitialize();
}
