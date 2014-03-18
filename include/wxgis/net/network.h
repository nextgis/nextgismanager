/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  network classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010,2012-2014 Bishop
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
#pragma once

#include "wxgis/net/message.h"

#include "wx/thread.h"
#include "wx/socket.h"

#define SLEEP 140
#define WAITFOR 500
#define BUFF_SIZE 16384
#define USE_STREAMS

class WXDLLIMPEXP_GIS_NET INetConnection;

/** \class wxNetReaderThread network.h
 *  \brief The network connection reader thread.
 */
class WXDLLIMPEXP_GIS_NET wxNetReaderThread : public wxThread
{
public:
    wxNetReaderThread(INetConnection* pNetConnection);
    virtual void *Entry();
    virtual void OnExit();
protected:
    INetConnection* m_pNetConnection;
};

/** \class wxNetWriterThread network.h
 *  \brief The network connection writer thread.
 */
class WXDLLIMPEXP_GIS_NET wxNetWriterThread : public wxThread
{
public:
    wxNetWriterThread(INetConnection* pNetConnection);
    virtual void *Entry();
    virtual void OnExit();
protected:
    INetConnection* m_pNetConnection;
};

/** \class INetConnection network.h
    \brief The network connection interface class.
*/
class WXDLLIMPEXP_GIS_NET INetConnection : 
    public wxGISConnectionPointContainer,
    public wxEvtHandler
{
    DECLARE_ABSTRACT_CLASS(INetConnection)
    friend class wxNetReaderThread;
    friend class wxNetWriterThread;
public:
    INetConnection(void);
    virtual ~INetConnection(void);
    virtual bool Connect(void){return false;};
	virtual bool Disconnect(void){return false;};
	virtual bool IsConnected(void){return m_bIsConnected;};
    virtual void SendNetMessageAsync(const wxNetMessage & msg);
    virtual wxNetMessage SendNetMessageSync(wxNetMessage & msg);
    typedef std::priority_queue< wxNetMessage, std::deque<wxNetMessage> > WXGISMSGQUEUE;
	virtual int GetId(void) const {return m_nUserId;};
	virtual void SetId(const int nUserId){m_nUserId = nUserId;};
protected:
    bool CreateAndRunThreads(void);
    void DestroyThreads(void);
    virtual bool ProcessOutputNetMessage(void);
    virtual bool ProcessInputNetMessage(void);
protected:
    WXGISMSGQUEUE m_aoMessages;
	int m_nUserId;	//user ID for server, and -1 for client	
    wxCriticalSection m_dataCS; // protects field above
    wxCriticalSection m_msgCS; // protects field above
	bool m_bIsConnected, m_bIsConnecting;
    wxSocketBase* m_pSock;//TODO: should be something universal XMPP, TCP, etc.
    wxNetWriterThread* m_pOutThread;
    wxNetReaderThread* m_pInThread;
    wxVector<wxNetMessage> m_oaSyncMessages;
    wxArrayLong m_laWaitIds;
    char m_Buffer[BUFF_SIZE];
};

bool WXDLLIMPEXP_GIS_NET SendUDP(IPaddress addr, wxNetMessage & msg, bool broadcast);
wxString WXDLLIMPEXP_GIS_NET GetSocketErrorMsg(int pSockError);
