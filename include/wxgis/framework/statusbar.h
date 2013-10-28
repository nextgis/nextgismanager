/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISStatusBar class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2012,2013 Bishop
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

#include "wxgis/framework/animation.h"
#include "wxgis/framework/progressor.h"

#include <wx/statusbr.h>

class WXDLLIMPEXP_GIS_FRW wxGISApplicationBase;

/** \enum wxGISEnumStatusBarPanes
    \brief A statusbar panels.

    This is predefined statusbar panels list.
*/
enum wxGISEnumStatusBarPanels
{
	enumGISStatusMain           = 0x0001,
	enumGISStatusAnimation      = 0x0002,
	enumGISStatusPosition       = 0x0004,
	enumGISStatusPagePosition   = 0x0008,
	enumGISStatusSize           = 0x0010,
	enumGISStatusCapsLock       = 0x0020,
	enumGISStatusNumLock        = 0x0040,
	enumGISStatusScrollLock     = 0x0080,
	enumGISStatusClock          = 0x0100,
	enumGISStatusProgress       = 0x0200
};

/** \class wxGISStatusBar statusbar.h
    \brief wxGIS statusbar class.

    This is base class for wxGIS application statusbar.
*/
class WXDLLIMPEXP_GIS_FRW wxGISStatusBar :	public wxStatusBar
{
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxGISStatusBar)

    enum
    {
        TIMER_ID = 1011
    };
public:
    wxGISStatusBar();
	wxGISStatusBar(wxWindow *parent, wxWindowID id, long style = wxST_SIZEGRIP, const wxString& name = wxT("statusBar"), WXDWORD panelsstyle = enumGISStatusMain | enumGISStatusProgress | enumGISStatusAnimation | enumGISStatusPosition | enumGISStatusClock);
	~wxGISStatusBar(void);
//methods
     /** \fn virtual void SetMessage(const wxString& text, int i = 0)
     *  \brief Put message to the status bar.
     *  \param text The message text
     *  \param i The panel index (default 0 - the first panel)
     */
	void SetMessage(const wxString& text, int i = 0);
    /** \fn virtual wxString GetMessage
     *  \brief Get message from the status bar.
     *  \param i The panel index (default 0 - the first panel)
     *  \return The message in i-th panel of status bar
     */
	wxString GetMessage(int i = 0) const;
    /** \fn virtual int GetPanePos(wxGISEnumStatusBarPanes nPane)
     *  \brief Get the panel pos by it's type.
     *  \return The panel index
     */
    int GetPanelPos(wxGISEnumStatusBarPanels nPanel);
    /** \fn virtual IProgressor* GetAnimation(void)
     *  \brief Get the animated progressor (rotating globe).
     *  \return The IProgressor pointer
     */
	IProgressor* GetAnimation(void) const
	{
		if(m_Panels & enumGISStatusAnimation)
			return static_cast<IProgressor*>(m_pAni);
		return NULL;
	};
    /** \fn virtual IProgressor* GetProgressor(void)
     *  \brief Get the progressor (progress bar).
     *  \return The IProgressor pointer
     */
	IProgressor* GetProgressor(void) const
	{
		if(m_Panels & enumGISStatusProgress)
			return static_cast<IProgressor*>(m_pProgressBar);
		return NULL;
	};

    /** \fn virtual WXDWORD GetPanes(void)
     *  \brief Get the status bar style (the combination of wxGISEnumStatusBarPanes)
     *  \return The status bar style
     */
    WXDWORD GetPanels(void) const {return m_Panels;};
    /** \fn virtual void SetPanes(WXDWORD Panes)
     *  \brief Set the panel style (the combination of wxGISEnumStatusBarPanes)
     *  \param Panes The status bar style
     */
    void SetPanels(WXDWORD Panels){m_Panels = Panels;};
	//events
	void OnSize(wxSizeEvent &event);
	void OnRightDown(wxMouseEvent& event);
    void OnTimer( wxTimerEvent & event);
    void OnMessage(wxCommandEvent &event);
typedef struct _statuspanel
	{
		int size;
		long style;
	}STATUSPANEL;

protected:
	wxTimer m_timer;
	wxGISAnimation* m_pAni;
    wxGISProgressor* m_pProgressBar;
	int m_MsgPos, m_AniPos, m_ProgressPos, m_PositionPos, m_ClockPos, m_PagePositionPos, m_SizePos, m_CapsLockPos, m_NumLockPos, m_ScrollLockPos;
	wxGISApplicationBase* m_pApp;
protected:
	WXDWORD m_Panels;    /*!< a status bar style (the combination of wxGISEnumStatusBarPanes)*/
private:

	DECLARE_EVENT_TABLE()
};
