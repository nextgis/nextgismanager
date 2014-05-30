/******************************************************************************
* Project:  wxGIS (GIS)
* Purpose:  differrent classes for smart pointers, point connections, etc.
* Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
******************************************************************************
*   Copyright (C) 20014 Dmitry Baryshnikov
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
#include "wxgis/core/pointer.h"

//--------------------------------------------------------------------------
// wxGISConnectionPointContainer
//--------------------------------------------------------------------------

wxGISConnectionPointContainer::wxGISConnectionPointContainer()
{
}

wxGISConnectionPointContainer::~wxGISConnectionPointContainer(void)
{
}

long wxGISConnectionPointContainer::Advise(wxEvtHandler* pEvtHandler)
{
    wxCHECK(pEvtHandler, wxNOT_FOUND);

    wxVector<wxEvtHandler*>::const_iterator pos = std::find(m_pPointsArray.begin(), m_pPointsArray.end(), pEvtHandler);
    if (pos != m_pPointsArray.end())
        return pos - m_pPointsArray.begin();
    m_pPointsArray.push_back(pEvtHandler);
    return m_pPointsArray.size() - 1;
}

void wxGISConnectionPointContainer::Unadvise(long nCookie)
{
    wxCHECK_RET(nCookie >= 0 && nCookie < m_pPointsArray.size(), wxT("Wrong cookie index"));
    m_pPointsArray[nCookie] = NULL;
}

void wxGISConnectionPointContainer::AddEvent(wxEvent &event)
{
    wxLogNull logNo;
    for (size_t i = 0; i < m_pPointsArray.size(); ++i)
    {
        if (m_pPointsArray[i] != NULL)
            m_pPointsArray[i]->AddPendingEvent(event);
    }
}
    
void wxGISConnectionPointContainer::PostEvent(wxEvent *event)
{
    wxLogNull logNo;
    wxCHECK_RET(event, wxT("Input event pointer is NULL"));
    for (size_t i = 0; i < m_pPointsArray.size(); ++i)
    {
        if (m_pPointsArray[i] != NULL)
            m_pPointsArray[i]->QueueEvent(event->Clone());
    }
    wxDELETE(event);
}

//--------------------------------------------------------------------------
// wxGISPointer
//--------------------------------------------------------------------------

wxGISPointer::wxGISPointer()
{
    m_RefCount = 0;
}

wxGISPointer::~wxGISPointer(void)
{
}

    //ref count
wxInt32 wxGISPointer::Reference(void)
{
    return m_RefCount++;
}

wxInt32 wxGISPointer::Dereference(void)
{ 
    return m_RefCount--;
}

wxInt32 wxGISPointer::Release(void)
{
    Dereference();
    if (m_RefCount < 1)
    {
        delete this;
        return 0;
    }
    else
    {
        return m_RefCount;
    }
}
