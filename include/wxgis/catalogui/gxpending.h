/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxPendingUI class. Show pending item in tree or content view
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2012,2013 Dmitry Baryshnikov
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

#include "wxgis/catalogui/catalogui.h"
#include "wxgis/catalog/gxobject.h"

#include <wx/imaglist.h>
#include <wx/stopwatch.h>

/** \class wxGxPendingUI gxpending.h
    \brief The pending item in tree or content view
*/

class WXDLLIMPEXP_GIS_CLU wxGxPendingUI :
    public wxGxObject,
    public IGxObjectUI,
    public IGxObjectNoFilter
{
    DECLARE_CLASS(wxGxPendingUI)
    enum
    {
        TIMER_ID = 1014
    };
public:
	wxGxPendingUI(wxVector<wxIcon> *pImageListSmall = NULL, wxVector<wxIcon> *pImageListLarge = NULL, wxGxObject *oParent = NULL, const wxString &soName = wxString(_("Waiting...")), const CPLString &soPath = "");
	virtual ~wxGxPendingUI(void);
	//IGxObject
	virtual wxString GetCategory(void) const {return wxString(_("Waiting..."));};
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void) const {return wxEmptyString;};
	virtual wxString NewMenu(void) const {return wxEmptyString;};
    //
    virtual void Stop(void);
    virtual void StopAndDestroy(void);
protected:
    //events
    void OnTimer( wxTimerEvent & event);
protected:
    short m_nCurrentImage;
    wxTimer m_timer;
    wxStopWatch m_sw;
    wxVector<wxIcon> *m_pImageListLarge;
    wxVector<wxIcon> *m_pImageListSmall;
    short m_nImageCount;
    wxCriticalSection m_CritSect;
    unsigned char m_nFinalCountDown;
    bool m_bFinal;
private:
    DECLARE_EVENT_TABLE()
};

