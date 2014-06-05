/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxFolderUI class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2013 Dmitry Barishnikov
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
#include "wxgis/catalogui/gxfolderui.h"
#include "wxgis/catalogui/gxcatalogui.h"

//---------------------------------------------------------------------------
// wxGxFolderUI
//---------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGxFolderUI, wxGxFolder)

wxGxFolderUI::wxGxFolderUI(wxGxObject *oParent, const wxString &soName, const CPLString &soPath, const wxIcon & LargeIcon, const wxIcon & SmallIcon) : wxGxFolder(oParent, soName, soPath), wxGxAutoRenamer()
{
    m_oLargeIcon = LargeIcon;
    m_oSmallIcon = SmallIcon;

    m_pGxViewToRename = NULL;
}

wxGxFolderUI::~wxGxFolderUI(void)
{
}

wxIcon wxGxFolderUI::GetLargeImage(void)
{
    return m_oLargeIcon;
}

wxIcon wxGxFolderUI::GetSmallImage(void)
{
    return m_oSmallIcon;
}

void wxGxFolderUI::EditProperties(wxWindow *parent)
{
    //TODO: Linux folder props
#ifdef __WXMSW__
    SHELLEXECUTEINFO SEInf = {0};
    SEInf.cbSize = sizeof(SEInf);    
    SEInf.hwnd = parent->GetHWND();
    SEInf.lpVerb = wxT("properties");
    SEInf.fMask = SEE_MASK_INVOKEIDLIST;

    wxString myString(m_sPath, wxConvUTF8);
    LPCWSTR pszPathStr = myString.wc_str();

    SEInf.lpFile = pszPathStr;
    SEInf.nShow = SW_SHOW;
    ShellExecuteEx(&SEInf);
#endif
}

wxDragResult wxGxFolderUI::CanDrop(wxDragResult def)
{
    return def;
}

bool wxGxFolderUI::Drop(const wxArrayString& saGxObjectPaths, bool bMove)
{
    return FolderDrop(m_sPath, saGxObjectPaths, bMove);
}

