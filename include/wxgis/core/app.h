/******************************************************************************
 * Project:  wxGIS
 * Purpose:  global functions.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011 Dmitry Barishnikov
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

#include "wxgis/core/core.h"

/** \class wxGISApplicationBase app.h
    \brief Base class for wxGIS application.
*/

class WXDLLIMPEXP_GIS_CORE IApplication : public wxObject
{
    DECLARE_ABSTRACT_CLASS(IApplication)
public:
    /** \fn virtual ~IApplication(void)
     *  \brief A destructor.
     */
    virtual ~IApplication(void) {};
	//pure virtual
	virtual void OnAppAbout(void) = 0;
    virtual void OnAppOptions(void) = 0;
	virtual wxString GetAppName(void) const = 0;
	virtual wxString GetAppDisplayName(void) const = 0;
	virtual wxString GetAppDisplayNameShort(void) const = 0;
	virtual wxString GetAppVersionString(void) const = 0;
    virtual bool CreateApp(void) = 0;
    virtual bool SetupLog(const wxString &sLogPath, const wxString &sNamePrefix = wxEmptyString) = 0;
	virtual wxString GetDecimalPoint(void) const = 0;
    virtual bool SetupLoc(const wxString &sLoc, const wxString &sLocPath) = 0;
    virtual bool SetupSys(const wxString &sSysPath) = 0;
    virtual void SetDebugMode(bool bDebugMode) = 0;
};

/** \fn IApplication* GetApplication()
 *  \brief Global application getter.
 */	
WXDLLIMPEXP_GIS_CORE IApplication* const GetApplication();
/** \fn void SetApplication(IApplication* pApp)
    \brief Global application setter.
	\param pApp The application pointer.
 */	
WXDLLIMPEXP_GIS_CORE void SetApplication(IApplication* pApp);

