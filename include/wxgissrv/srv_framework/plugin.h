/******************************************************************************
 * Project:  wxGIS (GIS Server)
 * Purpose:  Some plugin interface and implementation classes
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2008-2010 Bishop
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
/*
#include "wxgissrv/srv_framework/framework.h"

#include "wx/dir.h"
#include "wx/filename.h"
#include "wx/file.h"
#include "wx/ffile.h"
#include "wx/datetime.h"
#include "wx/intl.h"
#include "wx/dynload.h"
#include "wx/dynlib.h"


/** \class wxGISWorkPlugin auth.h
    \brief The base interface class for auth plugins.
*/

/** \code The usage example of plugin
    wxGISWorkPlugin Server;
	wxCmdLineParser parser;
	if(!Server.Start(wxT("Test"), wxT("Folder"), parser))
	{
        Server.Stop();
		return false;
	}
	//do some other work
	Server.Stop();
*/
/*
class WXDLLIMPEXP_GIS_FRW wxGISWorkPlugin
{
public:
	wxGISWorkPlugin(void);
	virtual ~wxGISWorkPlugin(void);
	virtual bool Start(wxString sAppName, wxString sConfigDir, wxCmdLineParser& parser);
	virtual void Stop();
    typedef std::map<wxString, wxDynamicLibrary*> LIBMAP;
protected:
	virtual void LoadLibs(wxXmlNode* pRootNode);
	virtual void UnLoadLibs(void);
    virtual bool SetupSys(wxString sSysPath);
    virtual void SetDebugMode(bool bDebugMode);
    virtual bool SetupLog(wxString sLogPath, wxString sNamePrefix);
	virtual bool SetupLoc(wxString sLoc, wxString sLocPath);
protected:
    wxLocale* m_pLocale; // locale we'll be using
	char* m_pszOldLocale;
	wxFFile m_LogFile;
    wxGISAppConfig* m_pConfig;
    LIBMAP m_LibMap;
};
*/