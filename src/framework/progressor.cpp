/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISProgressor class. Progress of some process
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011 Bishop
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
#include "wxgis/framework/progressor.h"

//---------------------------------------------------------------------------
// wxGISProgressor
//---------------------------------------------------------------------------
IMPLEMENT_DYNAMIC_CLASS(wxGISProgressor, wxGauge)

BEGIN_EVENT_TABLE(wxGISProgressor, wxGauge)
	EVT_COMMAND(wxID_ANY, wxEVT_COMMAND_BUTTON_CLICKED, wxGISProgressor::OnSetValue)
END_EVENT_TABLE()

wxGISProgressor::wxGISProgressor() : wxGauge()
{
    m_nValue = 0;
    m_bYield = false;
}

wxGISProgressor::wxGISProgressor(wxWindow * parent, wxWindowID id, int range, const wxPoint & pos, const wxSize & size, long style, const wxString name) : wxGauge(parent, id, range, pos, size, style, wxDefaultValidator, name)
{
    m_nValue = 0;
    m_bYield = false;
}

wxGISProgressor::~wxGISProgressor()
{
}

bool wxGISProgressor::ShowProgress(bool bShow)
{
    if(wxIsMainThread())
    {
    	return Show(bShow);
    }
    else
    {
        wxCommandEvent ValueEvent( wxEVT_COMMAND_BUTTON_CLICKED ); // Keep it simple, don't give a specific event ID
        ValueEvent.SetId(SHOW_ID);
        ValueEvent.SetInt(bShow);
        wxPostEvent(this, ValueEvent);

        return true;
    }
}

void wxGISProgressor::SetRange(int range)
{
    if(wxIsMainThread())
    {
	    wxGauge::SetRange(range);
    }
    else
    {
        wxCommandEvent ValueEvent( wxEVT_COMMAND_BUTTON_CLICKED ); // Keep it simple, don't give a specific event ID
        ValueEvent.SetId(RANGE_ID);
        ValueEvent.SetInt(range);
        wxPostEvent(this, ValueEvent);
    }
}

int wxGISProgressor::GetRange() const
{
	return wxGauge::GetRange();
}

void wxGISProgressor::SetValue(int value)
{
    if(GetRange() < value)
        return;
	m_nValue = value;

    if(wxIsMainThread())
    {
        wxGauge::SetValue(value);
        if(m_bYield)
            ::wxSafeYield(NULL, true);
    }
    else
    {
        //send message to itself
        wxCommandEvent ValueEvent( wxEVT_COMMAND_BUTTON_CLICKED ); // Keep it simple, don't give a specific event ID
        ValueEvent.SetId(SETVALUE_ID);
        ValueEvent.SetInt(value);
        wxPostEvent(this, ValueEvent);
    }
}

int wxGISProgressor::GetValue() const
{
	return wxGauge::GetValue();
}

void wxGISProgressor::Play(void)
{
    if(wxIsMainThread())
    {
	    wxGauge::Pulse();
        if(m_bYield)
            ::wxSafeYield(NULL, true);
    }
    else
    {
        //send message to itself
        wxCommandEvent ValueEvent( wxEVT_COMMAND_BUTTON_CLICKED ); // Keep it simple, don't give a specific event ID
        ValueEvent.SetId(PULSE_ID);
        wxPostEvent(this, ValueEvent);
    }
}

void wxGISProgressor::Stop(void)
{
    if(wxIsMainThread())
    {
	    wxGauge::SetValue(m_nValue);
        if(m_bYield)
            ::wxSafeYield(NULL, true);
    }
    else
    {
        //send message to itself
        wxCommandEvent ValueEvent( wxEVT_COMMAND_BUTTON_CLICKED ); // Keep it simple, don't give a specific event ID
        ValueEvent.SetId(SETVALUE_ID);
        ValueEvent.SetInt(m_nValue);
        wxPostEvent(this, ValueEvent);
    }
}

void wxGISProgressor::SetYield(bool bYield)
{
	m_bYield = bYield;
}

void wxGISProgressor::OnSetValue(wxCommandEvent &event)
{
    switch(event.GetId())
    {
    case SETVALUE_ID:
        wxGauge::SetValue(event.GetInt());
        break;
    case PULSE_ID:
        wxGauge::Pulse();
        break;
    case SHOW_ID:
        wxGauge::Show(event.GetInt() == TRUE);
        break;
    case RANGE_ID:
        wxGauge::SetRange(event.GetInt());
        break;
    default:
        break;
    }
}