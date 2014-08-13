/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxDiscConnectionUI class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2013 Dmitry Baryshnikov
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
#include "wxgis/catalogui/gxdiscconnectionui.h"
#include "wxgis/catalogui/gxcatalogui.h"

//------------------------------------------------------------------------------------
// wxGxDiscConnectionUI
//------------------------------------------------------------------------------------
IMPLEMENT_DYNAMIC_CLASS(wxGxDiscConnectionUI, wxGxDiscConnection)

wxGxDiscConnectionUI::wxGxDiscConnectionUI() : wxGxAutoRenamer(), wxGxDiscConnection(), wxThreadHelper()
{
    m_nIsReadable = wxNOT_FOUND;
}

wxGxDiscConnectionUI::wxGxDiscConnectionUI(wxGxObject *oParent, int nStoreId, const wxString &soName, const CPLString &soPath, const wxIcon &SmallIco, const wxIcon &LargeIco, const wxIcon &SmallIcoDsbl, const wxIcon &LargeIcoDsbl) : wxGxAutoRenamer(), wxGxDiscConnection(oParent, nStoreId, soName, soPath), wxThreadHelper()
{
    m_nIsReadable = wxNOT_FOUND;
	m_Conn16 = SmallIco;
	m_Conn48 = LargeIco;
	m_ConnDsbld16 = SmallIcoDsbl;
	m_ConnDsbld48 = LargeIcoDsbl;
}

wxGxDiscConnectionUI::~wxGxDiscConnectionUI(void)
{
}

bool wxGxDiscConnectionUI::CheckReadable(void)
{
    if(m_nIsReadable == wxNOT_FOUND)
    {
        m_nIsReadable = 0;
        CreateAndRunCheckThread();
    }
    return m_nIsReadable == 0 ? false: true;
}

//thread to check if remote folder is readable
//before exit we assume that folder is not readable
wxThread::ExitCode wxGxDiscConnectionUI::Entry()
{
    while (!GetThread()->TestDestroy())
    {
        if(m_nIsReadable == -2)
            return (wxThread::ExitCode)wxTHREAD_NO_ERROR;

        bool bIsOk = wxIsReadable(wxString(m_sPath, wxConvUTF8));
        char nIsReadable = bIsOk == true ? 1 : 0;
        if (nIsReadable != m_nIsReadable)
        {
            m_nIsReadable = nIsReadable;
            if (m_nIsReadable == FALSE)
            {
                wxGxDiscConnection::DestroyChildren();
            }

            wxGIS_GXCATALOG_EVENT(ObjectChanged);
        }

        wxThread::Sleep(TM_CHECKING);
    }

    return (wxThread::ExitCode)wxTHREAD_NO_ERROR;
}

bool wxGxDiscConnectionUI::CreateAndRunCheckThread(void)
{
    if (CreateThread(wxTHREAD_JOINABLE) != wxTHREAD_NO_ERROR)
    {
        wxLogError(_("Could not create the thread!"));
        return false;
    }

    if (GetThread()->Run() != wxTHREAD_NO_ERROR)
    {
        wxLogError(_("Could not run the thread!"));
        return false;
    }
    return true;
}

void wxGxDiscConnectionUI::DestroyCheckThread(void)
{
    if (GetThread() && GetThread()->IsRunning())
    {
        m_nIsReadable = -2;
        GetThread()->Wait();//Kill();//Delete();//
    }
}

bool wxGxDiscConnectionUI::Destroy(void)
{
    DestroyCheckThread();
    wxGxDiscConnection::Destroy();
}

wxIcon wxGxDiscConnectionUI::GetLargeImage(void)
{
	if(CheckReadable())
		return m_Conn48;
	else
		return m_ConnDsbld48;
}

wxIcon wxGxDiscConnectionUI::GetSmallImage(void)
{
	if(CheckReadable())
		return m_Conn16;
	else
		return m_ConnDsbld16;
}

void wxGxDiscConnectionUI::EditProperties(wxWindow *parent)
{
}

wxDragResult wxGxDiscConnectionUI::CanDrop(wxDragResult def)
{
    return def;
}

bool wxGxDiscConnectionUI::Drop(const wxArrayString& saGxObjectPaths, bool bMove)
{
    return FolderDrop(m_sPath, saGxObjectPaths, bMove);
}
