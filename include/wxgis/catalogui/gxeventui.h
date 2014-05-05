/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  event UI classes special for Selection events.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011 Bishop
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

#include "wxgis/catalogui/gxselection.h"

#include <wx/event.h>


class WXDLLIMPEXP_FWD_GIS_CLU wxGxSelectionEvent;

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_GIS_CLU, wxGXSELECTION_CHANGED, wxGxSelectionEvent);


/** \class wxGxSelectionEvent gxeventui.h
    \brief The GxSelection class event.
*/

class WXDLLIMPEXP_GIS_CLU wxGxSelectionEvent : public wxEvent
{
public:
    wxGxSelectionEvent(wxEventType eventType = wxGXSELECTION_CHANGED, wxGxSelection* pSelection = NULL, long nInitiator = wxNOT_FOUND) : wxEvent(0, eventType), m_nInitiator(nInitiator), m_pSelection(pSelection)
	{
	}
	wxGxSelectionEvent(const wxGxSelectionEvent& event) : wxEvent(event), m_nInitiator(event.m_nInitiator), m_pSelection(event.m_pSelection)
	{
	}

    void SetInitiator(long nInitiator) { m_nInitiator = nInitiator; }
    long GetInitiator() const { return m_nInitiator; }
    void SetSelection(wxGxSelection* pSelection) { m_pSelection = pSelection; }
    wxGxSelection* GetSelection() const { return m_pSelection; }

    virtual wxEvent *Clone() const { return new wxGxSelectionEvent(*this); }

protected:
    long m_nInitiator;
	wxGxSelection* m_pSelection;

private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxGxSelectionEvent)
};

typedef void (wxEvtHandler::*wxGxSelectionEventFunction)(wxGxSelectionEvent&);

#define wxGxSelectionEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxGxSelectionEventFunction, func)

#define EVT_GXSELECTION_CHANGED(func)  wx__DECLARE_EVT0(wxGXSELECTION_CHANGED, wxGxSelectionEventHandler(func))

