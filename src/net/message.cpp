/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  wxNetMessage class. Network message class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2008-2010,2012,2013  Bishop
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

#include "wxgis/net/message.h"

#include <wx/stream.h>
#include <wx/sstream.h>

//-----------------------------------------------------------------------------
// wxNetMessage
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxNetMessage, wxObject)

wxNetMessage::wxNetMessage(const wxJSONValue& val)
{
     m_refData = new wxNetMessageRefData(val);
}

wxNetMessage::wxNetMessage(wxGISNetCommand nNetCmd, wxGISNetCommandState nNetCmdState, short nPriority, long nId)
{
     m_refData = new wxNetMessageRefData(nNetCmd, nNetCmdState, nPriority, nId);
}

wxObjectRefData *wxNetMessage::CreateRefData() const
{
    return new wxNetMessageRefData();
}

wxObjectRefData *wxNetMessage::CloneRefData(const wxObjectRefData *data) const
{
    return new wxNetMessageRefData(*(wxNetMessageRefData *)data);
}

bool wxNetMessage::IsOk() const
{ 
    return m_refData != NULL && ((wxNetMessageRefData *)m_refData)->IsOk(); 
}

short wxNetMessage::GetPriority(void) const
{
    return ((wxNetMessageRefData *)m_refData)->m_nPriority;
}

void wxNetMessage::SetPriority(short nPriority)
{
    ((wxNetMessageRefData *)m_refData)->SetPriority( nPriority );
}

wxGISNetCommand wxNetMessage::GetCommand(void) const
{
    return ((wxNetMessageRefData *)m_refData)->m_nCmd;
}

void wxNetMessage::SetCommand(wxGISNetCommand nCmd)
{
    ((wxNetMessageRefData *)m_refData)->SetCommand( nCmd );
}

wxString wxNetMessage::GetMessage(void) const
{
    return ((wxNetMessageRefData *)m_refData)->m_sMessage;
}

void wxNetMessage::SetMessage(const wxString& sMsg)
{
    ((wxNetMessageRefData *)m_refData)->SetMessage( sMsg );
}

wxGISNetCommandState wxNetMessage::GetState(void) const
{
    return ((wxNetMessageRefData *)m_refData)->m_nState;
}

void wxNetMessage::SetState(wxGISNetCommandState nState)
{
    ((wxNetMessageRefData *)m_refData)->SetState( nState );
}

long wxNetMessage::GetId(void) const
{
    return ((wxNetMessageRefData *)m_refData)->m_nId;
}

void wxNetMessage::SetId(long nId)
{
    ((wxNetMessageRefData *)m_refData)->SetId( nId );
}

bool wxNetMessage::operator< (const wxNetMessage& msg) const
{
    return GetPriority() < msg.GetPriority();
}

bool wxNetMessage::operator == ( const wxNetMessage& obj ) const
{
    if (m_refData == obj.m_refData)
        return true;
    if (!m_refData || !obj.m_refData)
        return false;

    return ( *(wxNetMessageRefData*)m_refData == *(wxNetMessageRefData*)obj.m_refData );
}

wxJSONValue wxNetMessage::GetValue(void) const
{
    return ((wxNetMessageRefData *)m_refData)->m_Val[wxT("data")];
}

void wxNetMessage::SetValue(wxJSONValue val)
{
    ((wxNetMessageRefData *)m_refData)->m_Val[wxT("data")] = val;
}

wxJSONValue wxNetMessage::GetInternalValue() const
{
    return ((wxNetMessageRefData *)m_refData)->m_Val;
}

//-----------------------------------------------------------------------------
// wxNetMessageRefData
//-----------------------------------------------------------------------------

wxNetMessageRefData::wxNetMessageRefData( const wxNetMessageRefData& data ) : wxObjectRefData()
{
    m_Val = data.m_Val;
    m_nPriority = data.m_nPriority;
    m_nCmd = data.m_nCmd;
    m_nState = data.m_nState;
    m_sMessage = data.m_sMessage;
    m_nId = data.m_nId;
}

wxNetMessageRefData::wxNetMessageRefData( const wxJSONValue& val ) : wxObjectRefData()
{
    m_nPriority = val.Get(wxT("prio"), wxJSONValue(enumGISPriorityNormal)).AsInt();
    m_nCmd = (wxGISNetCommand)val.Get(wxT("cmd"), wxJSONValue(enumGISNetCmdUnk)).AsInt();
    m_nState = (wxGISNetCommandState)val.Get(wxT("st"), wxJSONValue(enumGISNetCmdStUnk)).AsInt();
    m_nId = val.Get(wxT("id"), wxJSONValue(wxNOT_FOUND)).AsLong();
    m_sMessage = val[wxT("msg")].AsString();
    m_Val = val;

    //m_Val[wxT("ver")] = WXNETVER;
}

wxNetMessageRefData::wxNetMessageRefData( wxGISNetCommand nNetCmd, wxGISNetCommandState nNetCmdState, short nPriority, long nId ) : wxObjectRefData()
{
    m_nPriority = nPriority;
    m_nState = nNetCmdState;
    m_nCmd = nNetCmd;
    m_nId = nId;

    m_Val[wxT("ver")] = WXNETVER;
    m_Val[wxT("prio")] = m_nPriority;
    m_Val[wxT("st")] = m_nState;
    m_Val[wxT("cmd")] = m_nCmd;
    m_Val[wxT("id")] = m_nId;
}

wxNetMessageRefData::~wxNetMessageRefData()
{
}  

bool wxNetMessageRefData::operator == (const wxNetMessageRefData& data) const
{
    return m_Val.IsSameAs(data.m_Val) && 
        m_nPriority == data.m_nPriority && 
        m_nState == data.m_nState &&
        m_nCmd == data.m_nCmd &&
        m_nId == data.m_nId;
}

bool wxNetMessageRefData::IsOk(void)
{
    return m_Val.IsValid();
}

void wxNetMessageRefData::SetPriority(short nPriority)
{
    m_Val[wxT("prio")] = nPriority;
    m_nPriority = nPriority;
}

void wxNetMessageRefData::SetState(wxGISNetCommandState nState)
{
    m_Val[wxT("st")] = nState;
    m_nState = nState;
}

void wxNetMessageRefData::SetCommand(wxGISNetCommand nCmd)
{
    m_Val[wxT("st")] = nCmd;
    m_nCmd = nCmd;
}

void wxNetMessageRefData::SetMessage(const wxString& sMsg)
{
    m_Val[wxT("msg")] = sMsg;
    m_sMessage = sMsg;
}

void wxNetMessageRefData::SetId(long nId)
{
    m_Val[wxT("id")] = nId;
    m_nId = nId;
}
