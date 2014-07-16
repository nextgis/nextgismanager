/******************************************************************************
 * Project:  wxGIS (GIS Server)
 * Purpose:  Some plugin interface and implementation classes
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2008-2010 Bishop
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
/*
#include "wxgissrv/srv_framework/plugin.h"

wxGISWorkPlugin::wxGISWorkPlugin(void) : m_pLocale(NULL), m_pszOldLocale(NULL), m_pConfig(NULL)
{
}

wxGISWorkPlugin::~wxGISWorkPlugin(void)
{
}

bool wxGISWorkPlugin::Start(wxString sAppName, wxString sConfigDir, wxCmdLineParser& parser)
{
	m_pConfig = new wxGISAppConfig(sAppName, sConfigDir);

	wxString sLogDir = m_pConfig->GetLogDir();
	if(!SetupLog(sLogDir, wxT("plg")))
        return false;

	wxString sLocale = m_pConfig->GetLocale();
	wxString sLocaleDir = m_pConfig->GetLocaleDir();
	wxString sLocalePath = sLocaleDir + wxFileName::GetPathSeparator() + sLocale;
	if(!SetupLoc(sLocale, sLocalePath))
        return false;

	//setup sys dir
	wxString sSysDir = m_pConfig->GetSysDir();
	if(!SetupSys(sSysDir))
        return false;

	bool bDebugMode = m_pConfig->GetDebugMode();
	SetDebugMode(bDebugMode);

	//store values
	//m_pConfig->SetLogDir(sLogDir);
	//m_pConfig->SetLocale(sLocale);
	//m_pConfig->SetLocaleDir(sLocaleDir);
	//m_pConfig->SetSysDir(sSysDir);
	//m_pConfig->SetDebugMode(bDebugMode);

    //load libs
	wxXmlNode* pLibsNode = m_pConfig->GetConfigNode(enumGISHKCU, wxString(wxT("libs")));
	if(pLibsNode)
		LoadLibs(pLibsNode);
	pLibsNode = m_pConfig->GetConfigNode(enumGISHKLM, wxString(wxT("libs")));
	if(pLibsNode)
		LoadLibs(pLibsNode);
	return true;
}

void wxGISWorkPlugin::Stop()
{
	UnLoadLibs();

    if(m_pszOldLocale != NULL)
		setlocale(LC_NUMERIC, m_pszOldLocale);
    wxDELETE(m_pLocale);
	wxDELETE(m_pszOldLocale);

	wxDELETE(m_pConfig);
}

void wxGISWorkPlugin::LoadLibs(wxXmlNode* pRootNode)
{
	wxXmlNode *child = pRootNode->GetChildren();
	while(child)
	{
		wxString sPath = child->GetPropVal(wxT("path"), wxT(""));
		if(sPath.Len() > 0)
		{
			//check for doubles
			if(m_LibMap[sPath] != NULL)
			{
				child = child->GetNext();
				continue;
			}

			wxDynamicLibrary* pLib = new wxDynamicLibrary(sPath);
			if(pLib != NULL)
			{
				wxLogMessage(_("wxGISWorkPlugin: Library %s loaded"), sPath.c_str());
				m_LibMap[sPath] = pLib;
			}
			else
				wxLogError(_("wxGISWorkPlugin: Error loading library %s"), sPath.c_str());
		}
		child = child->GetNext();
	}
}

void wxGISWorkPlugin::UnLoadLibs()
{
    for(LIBMAP::iterator item = m_LibMap.begin(); item != m_LibMap.end(); ++item)
		wxDELETE(item->second);
}

bool wxGISWorkPlugin::SetupSys(wxString sSysPath)
{
	if(!wxDirExists(sSysPath))
	{
		wxLogError(wxString::Format(_("wxGISWorkPlugin: System dir is absent! Lookup path '%s'"), sSysPath.c_str()));
		return false;
	}
    return true;
}

void wxGISWorkPlugin::SetDebugMode(bool bDebugMode)
{
}

bool wxGISWorkPlugin::SetupLog(wxString sLogPath, wxString sNamePrefix)
{
	if(sLogPath.IsEmpty())
	{
		wxLogError(_("wxGISWorkPlugin: Failed to get log dir"));
        return false;
	}
	if(!wxDirExists(sLogPath))
		wxFileName::Mkdir(sLogPath, 0777, wxPATH_MKDIR_FULL);

	wxDateTime dt(wxDateTime::Now());
    wxString logfilename = sLogPath + wxFileName::GetPathSeparator() + wxString::Format(wxT("%slog_%.4d%.2d%.2d.log"),sNamePrefix.c_str(), dt.GetYear(), dt.GetMonth() + 1, dt.GetDay());

	if(!m_LogFile.Open(logfilename.GetData(), wxT("a+")))
		wxLogError(_("wxGISWorkPlugin: Failed to open log file %s"), logfilename.c_str());

	wxLog::SetActiveTarget(new wxLogStderr(m_LogFile.fp()));

	wxLogMessage(wxT(" ")); 
	wxLogMessage(wxT("####################################################################")); 
	wxLogMessage(wxT("##                    %s                    ##"),wxNow().c_str()); 
	wxLogMessage(wxT("####################################################################")); 
	wxLogMessage(_("HOST '%s': OS desc - %s, free memory - %u Mb"), wxGetFullHostName().c_str(),wxGetOsDescription().c_str(), wxGetFreeMemory()/1048576);

	wxLogMessage(_("wxGISWorkPlugin: Initializing..."));
	wxLogMessage(_("wxGISWorkPlugin: Log file: %s"), logfilename.c_str());
	wxLogMessage(_("wxGISWorkPlugin: Initialize locale"));	

    return true;
}

bool wxGISWorkPlugin::SetupLoc(wxString sLoc, wxString sLocPath)
{
    wxLogMessage(_("wxGISWorkPlugin: Initialize locale"));

    if(m_pszOldLocale != NULL)
		setlocale(LC_NUMERIC, m_pszOldLocale);
	wxDELETE(m_pszOldLocale);
    wxDELETE(m_pLocale);

	//init locale
    if ( !sLoc.IsEmpty() )
    {
		int iLocale(0);
		const wxLanguageInfo* loc_info = wxLocale::FindLanguageInfo(sLoc);
		if(loc_info != NULL)
		{
			iLocale = loc_info->Language;
			wxLogMessage(_("wxGISWorkPlugin: Language set to %s"), loc_info->Description.c_str());
		}

        // don't use wxLOCALE_LOAD_DEFAULT flag so that Init() doesn't return
        // false just because it failed to load wxstd catalog
        m_pLocale = new wxLocale();
        if ( !m_pLocale->Init(iLocale, wxLOCALE_CONV_ENCODING) )
        {
            wxLogError(wxT("wxGISWorkPlugin: This language is not supported by the system."));
            return false;
        }
    }

	//m_locale.Init(wxLANGUAGE_DEFAULT);

    // normally this wouldn't be necessary as the catalog files would be found
    // in the default locations, but when the program is not installed the
    // catalogs are in the build directory where we wouldn't find them by
    // default
	if(wxDirExists(sLocPath))
	{
		wxLocale::AddCatalogLookupPathPrefix(sLocPath);

		// Initialize the catalogs we'll be using
		//load multicat from locale
		wxArrayString trans_arr;
		wxDir::GetAllFiles(sLocPath, &trans_arr, wxT("*.mo"));//_srv because using _cat dll's!

		for(size_t i = 0; i < trans_arr.size(); i++)
		{
			wxFileName name(trans_arr[i]);
			m_pLocale->AddCatalog(name.GetName());

			//m_locale.AddCatalog(wxT("wxMonClient"));
		}

		// this catalog is installed in standard location on Linux systems and
		// shows that you may make use of the standard message catalogs as well
		//
		// if it's not installed on your system, it is just silently ignored
	#ifdef __LINUX__
		{
			wxLogNull noLog;
			m_pLocale->AddCatalog(_T("fileutils"));
		}
	#endif
	}

	//support of dot in doubles and floats
	m_pszOldLocale = strdup(setlocale(LC_NUMERIC, NULL));
    if( setlocale(LC_NUMERIC, "C") == NULL )
        m_pszOldLocale = NULL;

    return true;
}
*/