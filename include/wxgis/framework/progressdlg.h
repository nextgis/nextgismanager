/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISProgressDlg class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011-2014 Dmitry Baryshnikov
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

#include "wxgis/framework/framework.h"

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/dialog.h>


#ifdef __WXMSW__
    #include <shobjidl.h>
#endif

#ifdef wxGIS_HAVE_UNITY_INTEGRATION
    #include <unity.h>
    #define DESKTOP_FILE_NAME "ngm.desktop"
#endif // wxGIS_HAVE_UNITY_INTEGRATION

/** @class wxGISProgressDlg

    The dialog showing progress and some buttons.

    @library {framework}
*/
class WXDLLIMPEXP_GIS_FRW wxGISProgressDlg :
	public wxDialog,
	public ITrackCancel,
	public IProgressor
{
    DECLARE_CLASS(wxGISProgressDlg)
public:
    wxGISProgressDlg(const wxString &title, const wxString &message, int  maximum = 100, wxWindow *  parent = NULL, int style = wxCAPTION );
	virtual ~wxGISProgressDlg(void);
	//IProgressor
    virtual bool ShowProgress(bool bShow = true);
    virtual void SetRange(int range);
    virtual int GetRange(void) const ;
    virtual void SetValue(int value);
    virtual int GetValue(void) const;
    virtual void Play(void);
    virtual void Stop(void);
	virtual void SetYield(bool bYield = false);
	//ITrackCancel
	virtual void Cancel(void);
	virtual bool Continue(void);
	virtual void Reset(void);
	virtual void PutMessage(const wxString &sMessage, size_t nIndex = wxNOT_FOUND, wxGISEnumMessageType eType = enumGISMessageUnknown);
    virtual wxString GetLastMessage(void) const {return m_sLastMessage;};
    //
    virtual void SetAddPercentToMessage(bool bAdd = false);
    virtual size_t GetWarningCount() const;
    virtual const wxVector<MESSAGE>& GetWarnings() const;
    //events
    virtual void OnCancel(wxCommandEvent& event);
protected:
    wxButton* m_sdbSizerCancel;
    wxStdDialogButtonSizer* m_sdbSizer;
    wxStaticText* m_staticElapsedText;
    IProgressor *m_pProgressBar;
    wxStaticText* m_staticText;
protected:
	wxString m_sLastMessage;
    bool m_bAddPercentToMessage;
    wxVector<MESSAGE> m_saWarnings;
    wxDateTime m_dtStart;
    int m_nPrevDone;
#ifdef __WXMSW__
    ITaskbarList3 *m_pTaskbarList;
#endif

#ifdef wxGIS_HAVE_UNITY_INTEGRATION
    UnityLauncherEntry* m_pLauncher;
#endif // wxGIS_HAVE_UNITY_INTEGRATION
private:
    DECLARE_EVENT_TABLE()
};
