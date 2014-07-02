/******************************************************************************
 * Project:  wxGIS
 * Purpose:  vector operations.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011-2013 Dmitry Baryshnikov
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

#include "wxgis/datasource/gdalinh.h"
#include "wxgis/datasource/spatialtree.h"

#include "wx/event.h"

class WXDLLIMPEXP_GIS_DS wxFeatureDSEvent;

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_GIS_DS, wxDS_FEATURES_ADDED, wxFeatureDSEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_GIS_DS, wxDS_FEATURE_ADDED, wxFeatureDSEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_GIS_DS, wxDS_FEATURE_DELETED, wxFeatureDSEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_GIS_DS, wxDS_FEATURE_CHANGED, wxFeatureDSEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_GIS_DS, wxDS_CLOSED, wxFeatureDSEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_GIS_DS, wxDS_CHANGED, wxFeatureDSEvent);

/** \class wxFeatureDSEvent vectorop.h
    \brief The FeatureDS class event.
*/
class WXDLLIMPEXP_GIS_DS wxFeatureDSEvent : public wxEvent
{
public:
    wxFeatureDSEvent(wxEventType eventType = wxDS_FEATURES_ADDED, const wxGISSpatialTreeCursor &Cursor = wxNullSpatialTreeCursor) : wxEvent(0, eventType)
	{
        m_Cursor = Cursor;
        m_nFid = wxNOT_FOUND;
	}
    wxFeatureDSEvent(wxEventType eventType, long nFid) : wxEvent(0, eventType)
	{
        m_Cursor = wxNullSpatialTreeCursor;
        m_nFid = nFid;
	}
	wxFeatureDSEvent(const wxFeatureDSEvent& event) : wxEvent(event)
	{
        for (size_t i = 0; i < event.m_Cursor.GetCount(); ++i)
        {
            if (event.m_Cursor[i])
                m_Cursor.Add(event.m_Cursor[i]->Clone());
        }
        m_nFid = event.m_nFid;
	}

	void SetCursor(const wxGISSpatialTreeCursor &Cursor){m_Cursor = Cursor;};
	wxGISSpatialTreeCursor GetCursor(void) const {return m_Cursor;};
	long GetFID(void) const {return m_nFid;};

    virtual wxEvent *Clone() const { return new wxFeatureDSEvent(*this); }

protected:
    wxGISSpatialTreeCursor m_Cursor;
    long m_nFid;
private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxFeatureDSEvent)
};

typedef void (wxEvtHandler::*wxFeatureDSEventFunction)(wxFeatureDSEvent&);

#define wxFeatureDSEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxFeatureDSEventFunction, func)

#define EVT_DS_FEATURES_ADDED(func)  wx__DECLARE_EVT0(wxDS_FEATURES_ADDED, wxFeatureDSEventHandler(func))
#define EVT_DS_FEATURE_ADDED(func)  wx__DECLARE_EVT0(wxDS_FEATURE_ADDED, wxFeatureDSEventHandler(func))
#define EVT_DS_FEATURE_DELETED(func)  wx__DECLARE_EVT0(wxDS_FEATURE_DELETED, wxFeatureDSEventHandler(func))
#define EVT_DS_FEATURE_CHANGED(func)  wx__DECLARE_EVT0(wxDS_FEATURE_CHANGED, wxFeatureDSEventHandler(func))
#define EVT_DS_CLOSED(func)  wx__DECLARE_EVT0(wxDS_CLOSED, wxFeatureDSEventHandler(func))
#define EVT_DS_CHANGED(func)  wx__DECLARE_EVT0(wxDS_CHANGED, wxFeatureDSEventHandler(func))


WXDLLIMPEXP_GIS_DS void IncreaseEnvelope(OGREnvelope &Env, double dSize);
WXDLLIMPEXP_GIS_DS void SetEnvelopeRatio(OGREnvelope &Env, double dRatio);
WXDLLIMPEXP_GIS_DS void MoveEnvelope(OGREnvelope &MoveEnv, const OGREnvelope &Env);

/** \fn wxGISGeometry EnvelopeToGeometry(const OGREnvelope &Env, const wxGISSpatialReference &SpaRef)
 *  \brief Create wxGISGeometry from OGREnvelope.
 *  \param Env Input envelope
 *  \param SpaRef Spatial Refernce of output geometry
 *  \return Geometry
 */
WXDLLIMPEXP_GIS_DS wxGISGeometry EnvelopeToGeometry(const OGREnvelope &Env, const wxGISSpatialReference &SpaRef = wxNullSpatialReference);
