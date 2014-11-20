/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISPropertyDlg class - dialog/dock window with the GxObject properties.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2014 Dmitry Baryshnikov
*   Copyright (C) 2014 NextGIS
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
#include "wxgis/catalogui/propertydlg.h"


//-------------------------------------------------------------------
// wxGISPropertyDlg
//-------------------------------------------------------------------
IMPLEMENT_DYNAMIC_CLASS(wxGISPropertyDlg, wxPanel)

BEGIN_EVENT_TABLE(wxGISPropertyDlg, wxPanel)

END_EVENT_TABLE()


wxGISPropertyDlg::wxGISPropertyDlg( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
	Create(parent, id, pos, size, style);
}

wxGISPropertyDlg::wxGISPropertyDlg(void)
{
}

bool wxGISPropertyDlg::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
	if(!wxPanel::Create( parent, id, pos, size, style, name ))
		return false;

	wxGISAppConfig oConfig = GetConfig();
    m_sAppName = GetApplication()->GetAppName();
	if(oConfig.IsOk())
    {
    }

	m_bMainSizer = new wxBoxSizer( wxVERTICAL );


	this->SetSizer( m_bMainSizer );
	this->Layout();

    return true;
}

wxGISPropertyDlg::~wxGISPropertyDlg()
{
	wxGISAppConfig oConfig = GetConfig();
	if(oConfig.IsOk())
    {
    }
}


//-------------------------------------------------------------------
// wxAxPropertyView
//-------------------------------------------------------------------
IMPLEMENT_DYNAMIC_CLASS(wxAxPropertyView, wxGISPropertyDlg)

BEGIN_EVENT_TABLE(wxAxPropertyView, wxGISPropertyDlg)
END_EVENT_TABLE()

wxAxPropertyView::wxAxPropertyView(void)
{
}

wxAxPropertyView::wxAxPropertyView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size) : wxGISPropertyDlg(parent, id, pos, size, wxNO_BORDER | wxTAB_TRAVERSAL)
{
    Create(parent, id, pos, size);
}

wxAxPropertyView::~wxAxPropertyView(void)
{
}

bool wxAxPropertyView::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    m_sViewName = wxString(_("Properties"));
    return wxGISPropertyDlg::Create(parent, id, pos, size, style, name);
}

bool wxAxPropertyView::Activate(IApplication* const pApplication, wxXmlNode* const pConf)
{
    m_pApp = dynamic_cast<wxGISApplicationBase*>(pApplication);
    if(NULL == m_pApp)
        return false;

	return true;
}

void wxAxPropertyView::Deactivate(void)
{
}
