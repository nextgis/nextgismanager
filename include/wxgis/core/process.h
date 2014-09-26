/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  external process common classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2012,2014 Dmitry Baryshnikov
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

#include <wx/process.h>
#include <wx/thread.h>

/** @class wxGISThreadHelper

    The gis classes thread helper. The main purpose - garanted destroy

    @library{core}
*/

class WXDLLIMPEXP_GIS_CORE wxGISThreadHelper : public wxThreadHelper
{
public:
    wxGISThreadHelper(wxThreadKind kind = wxTHREAD_DETACHED);
    virtual ~wxGISThreadHelper();
protected:
    void KillThread();
    bool TestDestroy();
    bool CreateAndRunThread(void);
    void DestroyThread(void);
protected:
    bool m_bKill;
};

class WXDLLIMPEXP_GIS_CORE wxGISProcess;

/** @class IGISProcessParent

    The wxGISProcess parent interface class.

    @library{core}
 */
class IGISProcessParent
{
public:
	virtual ~IGISProcessParent(void){};
    virtual void OnFinish(wxGISProcess* pProcess, bool bHasErrors) = 0;
};

/** @class wxGISProcess

    The process class which stores the application execution data.

    @library{core}
 */
class WXDLLIMPEXP_GIS_CORE wxGISProcess :
	public wxProcess,
    public wxGISThreadHelper
{
public:
    wxGISProcess(IGISProcessParent* pParent = NULL);
    virtual ~wxGISProcess(void);
    // wxProcess
    virtual void OnTerminate(int pid, int status);
    //wxGISProcess
    virtual bool Start(void);
    virtual void Stop(void);
    //
	virtual void ProcessInput(wxString & sInputData);
    virtual void SetState(wxGISEnumTaskStateType nState){m_nState = nState;};
	virtual wxGISEnumTaskStateType GetState(void) const {return m_nState;};
    virtual wxDateTime GetStart(void) const {return m_dtBeg;};
    virtual wxDateTime GetFinish(void) const {return m_dtEstEnd;};
protected:
    virtual wxThread::ExitCode Entry();
    virtual long Execute(void) = 0;
    virtual void UpdatePercent(const wxString &sPercentData);
    virtual void AddInfo(wxGISEnumMessageType eType, const wxString &sInfoData) = 0;
protected:
    IGISProcessParent* m_pParent;
    wxCriticalSection m_ExitLock;
    //
	wxDateTime m_dtBeg;
	wxDateTime m_dtEstEnd;
    wxGISEnumTaskStateType m_nState;
    double m_dfDone;
};


