/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxFileUI classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2011,2013 Dmitry Barishnikov
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

#include "wxgis/catalogui/gxfileui.h"

#include "wxgis/framework/application.h"

//propertypages
#include "wxgis/catalogui/spatrefpropertypage.h"

#include "../../art/properties.xpm"

#include "wx/propdlg.h"


//--------------------------------------------------------------
//class wxGxFileUI
//--------------------------------------------------------------
IMPLEMENT_CLASS(wxGxFileUI, wxGxFile)

wxGxFileUI::wxGxFileUI(wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxFile(oParent, soName, soPath)
{
}

wxGxFileUI::~wxGxFileUI(void)
{
}


//--------------------------------------------------------------
//class wxGxPrjFileUI
//--------------------------------------------------------------
IMPLEMENT_CLASS(wxGxPrjFileUI, wxGxPrjFile)

wxGxPrjFileUI::wxGxPrjFileUI(wxGISEnumPrjFileType eType, wxGxObject *oParent, const wxString &soName, const CPLString &soPath, const wxIcon &LargeIcon, const wxIcon &SmallIcon) : wxGxPrjFile(eType, oParent, soName, soPath)
{
    m_oLargeIcon = LargeIcon;
    m_oSmallIcon = SmallIcon;
}

wxGxPrjFileUI::~wxGxPrjFileUI(void)
{
}

wxIcon wxGxPrjFileUI::GetLargeImage(void)
{
    return m_oLargeIcon;
}

wxIcon wxGxPrjFileUI::GetSmallImage(void)
{
    return m_oSmallIcon;
}

void wxGxPrjFileUI::EditProperties(wxWindow *parent)
{
    wxPropertySheetDialog PropertySheetDialog;
    if (!PropertySheetDialog.Create(parent, wxID_ANY, _("Properties"), wxDefaultPosition, wxSize( 480,640 ), wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER))
        return;
    PropertySheetDialog.SetIcon(properties_xpm);
    PropertySheetDialog.CreateButtons(wxOK);
    wxWindow* pParentWnd = static_cast<wxWindow*>(PropertySheetDialog.GetBookCtrl());

    wxGISSpatialReferencePropertyPage* SpatialReferencePropertyPage = new wxGISSpatialReferencePropertyPage(GetSpatialReference(), pParentWnd);
    PropertySheetDialog.GetBookCtrl()->AddPage(SpatialReferencePropertyPage, SpatialReferencePropertyPage->GetPageName());

    //PropertySheetDialog.LayoutDialog();
    PropertySheetDialog.SetSize(480,640);
    PropertySheetDialog.Center();

    PropertySheetDialog.ShowModal();
}

bool wxGxPrjFileUI::Invoke(wxWindow* pParentWnd)
{
    EditProperties(pParentWnd);
    return true;
}
//--------------------------------------------------------------
//class wxGxTextFileUI
//--------------------------------------------------------------
IMPLEMENT_CLASS(wxGxTextFileUI, wxGxTextFile)

wxGxTextFileUI::wxGxTextFileUI(wxGxObject *oParent, const wxString &soName, const CPLString &soPath, const wxIcon &LargeIcon, const wxIcon &SmallIcon) : wxGxTextFile(oParent, soName, soPath)
{
    m_oLargeIcon = LargeIcon;
    m_oSmallIcon = SmallIcon;
}

wxGxTextFileUI::~wxGxTextFileUI(void)
{
}

wxIcon wxGxTextFileUI::GetLargeImage(void)
{
	return m_oLargeIcon;
}

wxIcon wxGxTextFileUI::GetSmallImage(void)
{
	return m_oSmallIcon;
}

void wxGxTextFileUI::EditProperties(wxWindow *parent)
{
}
