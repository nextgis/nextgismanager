/******************************************************************************
 * Project:  wxGIS
 * Purpose:  Initializer class for logs, locale, libs and etc.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2012 Bishop
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

#include "wxgis/core/core.h"
#include "wxgis/core/app.h"
#include "wxgis/core/config.h"

#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/file.h>
#include <wx/ffile.h>
#include <wx/datetime.h>
#include <wx/intl.h>
#include <wx/dynload.h>
#include <wx/dynlib.h>
#include <wx/cmdline.h> 

/** \class wxGISAppWithLibs init.h
    \brief The library loader and unloader class.
*/

class WXDLLIMPEXP_GIS_CORE wxGISAppWithLibs
{
public:
	wxGISAppWithLibs(void);
	virtual ~wxGISAppWithLibs(void);
    typedef std::map<wxString, wxDynamicLibrary*> LIBMAP;
    virtual void LoadLib(const wxString &sPath, bool bStore = true);
protected:
	virtual void LoadLibs(const wxXmlNode* pRootNode);
	virtual void SerializeLibs();
	virtual void UnLoadLibs();
protected:
    wxArrayString m_asNoStore;
    LIBMAP m_LibMap;
};

/** \class wxGISInitializer init.h
    \brief The Initializer class for logs, locale, libs and etc.

	This class load config from xml files, libraries from paths stored in config, initialize locale, log directory path and system directory path. In Stop method the loaded libraries unloaded.
*/

/** \code The usage example of Initializer
    wxGISInitializer oInitializer;
	wxCmdLineParser parser;
	if(!oInitializer.Initialize(wxT("TestApplication"), wxT("ta_"), parser))
	{
        oInitializer.Uninitialize();
		return false;
	}
	//do some other work
	oInitializer.Uninitialize();
*/


class WXDLLIMPEXP_GIS_CORE wxGISInitializer :
	public wxGISAppWithLibs,
    public IApplication
{
public:
	wxGISInitializer(void);
	virtual ~wxGISInitializer(void);
	virtual bool Initialize(const wxString &sAppName, const wxString &sLogFilePrefix);
	virtual void Uninitialize();
    // IApplication
    virtual bool SetupSys(const wxString &sSysPath);
    virtual void SetDebugMode(bool bDebugMode);
    virtual bool SetupLog(const wxString &sLogPath, const wxString &sNamePrefix);
	virtual bool SetupLoc(const wxString &sLoc, const wxString &sLocPath);
    virtual wxString GetDecimalPoint(void) const{return m_sDecimalPoint;};
    virtual bool CreateApp(void) {return true;};
protected:
    wxLocale* m_pLocale; // locale we'll be using
	wxFFile m_LogFile;
    wxString m_sDecimalPoint;
};


 