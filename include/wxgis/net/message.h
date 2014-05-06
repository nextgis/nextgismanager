/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  wxNetMessage class. Network message class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2008-2010,2012,2013  Bishop
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

#include "wxgis/net/net.h"
#include "wxgisdefs.h"

#include "wxgis/core/json/jsonval.h"
#include "wxgis/core/json/jsonreader.h"
#include "wxgis/core/json/jsonwriter.h"

#include "wx/xml/xml.h"

#define WXNETVER 3


/** @class wxNetMessage

    A network message class. Used in messaging between server and client

    @library{net}
*/


class WXDLLIMPEXP_GIS_NET wxNetMessage : public wxObject
{
    DECLARE_DYNAMIC_CLASS(wxNetMessage)
public:
    /** \fn wxNetMessage(const char* pBuff, size_t len)
     *  \brief A constructor.
	 *	\param pBuff input buffer
	 *	\param len input buffer size
     */    
    wxNetMessage(const wxJSONValue& val);
    wxNetMessage(wxGISNetCommand nNetCmd = enumGISNetCmdUnk, wxGISNetCommandState nNetCmdState = enumGISNetCmdStUnk, short nPriority = enumGISPriorityNormal, long nId = wxNOT_FOUND);

    bool IsOk() const;

    bool operator == ( const wxNetMessage& obj ) const;
    bool operator != (const wxNetMessage& obj) const { return !(*this == obj); };
    bool operator < (const wxNetMessage& obj) const;

    short GetPriority(void) const;
    void SetPriority(short nPriority);
    wxGISNetCommand GetCommand(void) const;
    void SetCommand(wxGISNetCommand nCmd);
    wxGISNetCommandState GetState(void) const;
    void SetState(wxGISNetCommandState nState);

    long GetId(void) const;
    void SetId(long nId);
    
    wxString GetMessage(void) const;
    void SetMessage(const wxString& sMsg);    

    void SetValue(wxJSONValue val);
    wxJSONValue GetValue(void) const;
    wxJSONValue GetInternalValue() const;
protected:
    virtual wxObjectRefData *CreateRefData() const;
    virtual wxObjectRefData *CloneRefData(const wxObjectRefData *data) const;
};

/** @class wxNetMessageRefData
    
    The reference data class for wxNetMessage

    @library{net}
*/

class  wxNetMessageRefData : public wxObjectRefData
{
    friend class wxNetMessage;
public:
    wxNetMessageRefData( const wxNetMessageRefData& data );
    wxNetMessageRefData( const wxJSONValue& val );
    wxNetMessageRefData( wxGISNetCommand nNetCmd = enumGISNetCmdUnk, wxGISNetCommandState nNetCmdState = enumGISNetCmdStUnk, short nPriority = enumGISPriorityNormal, long nId = wxNOT_FOUND );
    virtual ~wxNetMessageRefData();
    
    bool operator == (const wxNetMessageRefData& data) const;
    bool IsOk(void);

    void SetPriority(short nPriority);
    void SetState(wxGISNetCommandState nState);
    void SetMessage(const wxString& sMsg);
    void SetId(long nId);
    void SetCommand(wxGISNetCommand nState);
protected:
	wxJSONValue m_Val;
	short m_nPriority;
	wxGISNetCommand m_nCmd;
	wxGISNetCommandState m_nState;
    wxString m_sMessage;
    long m_nId;
};

#ifdef wxGIS_USE_EMAIL
/** @class wxMailMessage

    The e-mail message

    @library{net}
*/

class WXDLLIMPEXP_GIS_NET wxMailMessage
{
public:
    wxMailMessage(const wxString& subject, const wxString& body, const wxString& from = wxEmptyString, const wxString& attachment = wxEmptyString, const wxString& attachmentTitle = wxEmptyString);
    wxMailMessage(void);

    //// Accessors

    void AddAttachment(const wxString& attach, const wxString& title = wxEmptyString);

    void SetSubject(const wxString& subject);
    void SetBody(const wxString& body);
    void SetFrom(const wxString& from);

public:
    wxString       m_from;             //The From: email address (optional)
    wxString       m_subject;          //The Subject of the message
    wxString       m_body;             //The Body of the message
    wxArrayString  m_attachments;      //Files to attach to the email
    wxArrayString  m_attachmentTitles; //Titles to use for the email file attachments
};

#endif //wxGIS_USE_EMAIL