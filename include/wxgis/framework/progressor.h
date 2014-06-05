/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISProgressor class. Progress of some process
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011 Dmitry Barishnikov
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
#include "wx/gauge.h"

/** @class wxGISProgressor

    The progress bar (gauge) class.

    @library{framework}
*/
class WXDLLIMPEXP_GIS_FRW wxGISProgressor : 
	public wxGauge,
	public IProgressor
{
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxGISProgressor)
    enum
    {
        SETVALUE_ID = 1050,
        PULSE_ID, 
        SHOW_ID,
        RANGE_ID
    };
public:
    wxGISProgressor(void);
	wxGISProgressor(wxWindow * parent, wxWindowID id = wxID_ANY, int range = 100, const wxPoint & pos = wxDefaultPosition, const wxSize & size = wxDefaultSize, long style = wxGA_HORIZONTAL | wxGA_SMOOTH, const wxString name = wxT("GISProgressor"));
	virtual ~wxGISProgressor(void);
	//IProgressor
	virtual bool ShowProgress(bool bShow);
    virtual void SetRange(int range);
    virtual int GetRange() const;
    virtual void SetValue(int value);
    virtual int GetValue() const;
	virtual void Play(void);
	virtual void Stop(void);
	virtual void SetYield(bool bYield = false);
//events
    virtual void OnSetValue(wxCommandEvent &event);
protected:
    int m_nValue;
	bool m_bYield;
private:
	DECLARE_EVENT_TABLE()
};
