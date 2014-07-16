/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxPendingUI class. Show pending item in tree or content view
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010,2012,2013 Dmitry Baryshnikov
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

#include "wxgis/catalogui/gxpending.h"
#include "wxgis/catalogui/gxcatalogui.h"

//---------------------------------------------------------------------
// wxGxPendingUI
//---------------------------------------------------------------------
#define UPDATE_TIMER 150
#define DESTROY_COUNTDOW 7

IMPLEMENT_CLASS(wxGxPendingUI, wxGxObject)

BEGIN_EVENT_TABLE(wxGxPendingUI, wxGxObject)
  EVT_TIMER(TIMER_ID, wxGxPendingUI::OnTimer)
END_EVENT_TABLE()

wxGxPendingUI::wxGxPendingUI(wxVector<wxIcon> *pImageListSmall, wxVector<wxIcon> *pImageListLarge, wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxObject(oParent, soName, soPath), m_timer(this, TIMER_ID)
{
    m_bFinal = false;
    m_nFinalCountDown = 0;

    m_nCurrentImage = 0;
    m_pImageListSmall = pImageListSmall;
    m_pImageListLarge = pImageListLarge;
    if(m_pImageListSmall && m_pImageListLarge)
        m_nImageCount = wxMin(m_pImageListSmall->size(), m_pImageListLarge->size());
    else
        m_nImageCount = 0;
    m_timer.Start(UPDATE_TIMER);
}

wxGxPendingUI::~wxGxPendingUI(void)
{
    Stop();
}

void wxGxPendingUI::Stop(void)
{
    m_timer.Stop();
}

void wxGxPendingUI::StopAndDestroy(void)
{
    m_bFinal = true;
    m_nFinalCountDown = 0;

    //Stop();
    //wxPostEvent(this, wxCommandEvent(wxEVT_COMMAND_BUTTON_CLICKED));
}

wxIcon wxGxPendingUI::GetLargeImage(void)
{
    if(m_pImageListLarge && m_pImageListLarge->size() > m_nCurrentImage)
    {
        return m_pImageListLarge->at(m_nCurrentImage);
    }
    else
    {
        return wxNullIcon;
    }
}

wxIcon wxGxPendingUI::GetSmallImage(void)
{
    if(m_pImageListSmall && m_pImageListSmall->size() > m_nCurrentImage)
    {
        return m_pImageListSmall->at(m_nCurrentImage);
    }
    else
    {
        return wxNullIcon;
    }
}

void wxGxPendingUI::OnTimer( wxTimerEvent& event )
{
    if (m_bFinal)
    {
        if (++m_nFinalCountDown == DESTROY_COUNTDOW)
        {
            m_timer.Stop();
            Destroy();
        }
    }
    else
    {
        wxCriticalSectionLocker locker(m_CritSect);
        m_sName = wxString(_("Waiting...")) + wxString::Format(_(" (%ld sec)"), m_sw.Time() / 1000); //sec.

        m_nCurrentImage++;
        if(m_nCurrentImage >= m_nImageCount)
            m_nCurrentImage = 0;
        wxGIS_GXCATALOG_EVENT(ObjectChanged);
    }
}

