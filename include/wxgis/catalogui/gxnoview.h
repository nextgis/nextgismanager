/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxNoView class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011,2012 Bishop
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
#include "wxgis/catalogui/gxview.h"


class wxGxNoView :
	public wxControl,
	public wxGxView
{
    DECLARE_DYNAMIC_CLASS(wxGxNoView)
public:
    wxGxNoView(void);
	wxGxNoView(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
	virtual ~wxGxNoView(void);	
    void OnPaint(wxPaintEvent& WXUNUSED(evt));
    void OnEraseBackground(wxEraseEvent& WXUNUSED(evt));
    void OnSize(wxSizeEvent& WXUNUSED(evt));
//IGxView
    virtual bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxNO_BORDER, const wxString& name = wxT("NoView"));
	virtual bool Activate(IApplication* const application, wxXmlNode* const pConf);
	virtual void Deactivate(void);
protected:
    void Serialize(wxXmlNode* pRootNode, bool bStore);
private:
	unsigned char R,G,B;
	unsigned char FontSize;
	bool IsBold, IsItalic;

    DECLARE_EVENT_TABLE()
};
