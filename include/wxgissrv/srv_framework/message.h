/******************************************************************************
 * Project:  wxGIS (GIS Server)
 * Purpose:  wxNetMessage class. Network message class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2008-2010  Bishop
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
/*
#include "wxgissrv/srv_framework/framework.h"

#define WXNETVER 2

// ----------------------------------------------------------------------------
// wxNetMessage
// ----------------------------------------------------------------------------

//#define WEMESSAGE wxT("<?xml version=\"1.0\" encoding=\"UTF-8\"?><weMsg version=\"%d\" state=\"%d\" message=\"%s\" priority=\"%u\">%s</weMsg>")

class WXDLLIMPEXP_GIS_FRW wxNetMessage : public INetMessage
{
public:
    wxNetMessage(void);
    wxNetMessage(wxString sMsgData, long nID);
    wxNetMessage(long nID, wxGISMessageState nState = enumGISMsgStUnk, short nPriority = enumGISPriorityNormal, wxString sMessage = wxEmptyString, wxString sModuleSrc = wxEmptyString, wxString sModuleDst = wxEmptyString);
    virtual ~wxNetMessage(void);
    virtual short GetPriority(void);
    virtual void SetPriority(short nPriority);
    //virtual bool operator< (const INetMessage& msg) const;
    //virtual INetMessage& operator= (const INetMessage& oSource);
    virtual bool IsOk(void);
    virtual wxGISMessageDirection GetDirection(void);
    virtual void SetDirection(wxGISMessageDirection nDirection);
    virtual wxXmlNode* GetRoot(void);
    virtual wxString GetData(void);
protected:
    wxXmlDocument *m_pXmlDocument;
    long m_nID;
	bool m_bIsOk;
    short m_nPriority;
    wxGISMessageDirection m_nDirection;
    wxGISMessageState m_nState;
    wxString m_sMessage;
};
//
////std::priority_queue<Message, std::deque<Message> > pq;
*/

