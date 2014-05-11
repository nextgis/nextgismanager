/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  network classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010,2012-2014 Bishop
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
#include "wxgis/net/network.h"
#include "wxgis/net/netevent.h"

#include <wx/sckstrm.h>

#define NET_WAIT_TIMEOUT 20

// ----------------------------------------------------------------------------
// wxNetTCPReader
// ----------------------------------------------------------------------------
wxNetReaderThread::wxNetReaderThread(INetConnection* pNetConnection) : wxThread(wxTHREAD_DETACHED)
{
    m_pNetConnection = pNetConnection;
}

void *wxNetReaderThread::Entry()
{
	if(!m_pNetConnection)
		return (ExitCode)-1;

	while(!TestDestroy())
	{
		//WaitForRead
        //wxCriticalSectionLocker lock(m_CritSect);
        //if (m_pNetConnection == NULL)
        //    break;
        if (!m_pNetConnection->ProcessInputNetMessage())
        {
            wxThread::Sleep(SLEEP);
        }
            //wxThread::Sleep(SLEEP);
	}
	return (wxThread::ExitCode)wxTHREAD_NO_ERROR;
}

void wxNetReaderThread::OnExit()
{
    m_pNetConnection = NULL;
}


// ----------------------------------------------------------------------------
// wxNetTCPWriter
// ----------------------------------------------------------------------------
wxNetWriterThread::wxNetWriterThread(INetConnection* pNetConnection) : wxThread(wxTHREAD_DETACHED)
{
    m_pNetConnection = pNetConnection;
}

void *wxNetWriterThread::Entry()
{
	if(!m_pNetConnection)
		return (wxThread::ExitCode)wxTHREAD_MISC_ERROR;

	while(!TestDestroy())
	{
        //WaitForWrite
        //wxCriticalSectionLocker lock(m_CritSect);
        //if (m_pNetConnection == NULL)
        //    break;
        if (!m_pNetConnection->ProcessOutputNetMessage())
        {
            wxThread::Sleep(SLEEP);
        }
            //wxThread::Sleep(SLEEP);
	}
	return (wxThread::ExitCode)wxTHREAD_NO_ERROR;
}

void wxNetWriterThread::OnExit()
{
    m_pNetConnection = NULL;
}

//--------------------------------------------------------------------------
// INetConnection
//--------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(INetConnection, wxEvtHandler)

INetConnection::INetConnection(void) : wxEvtHandler()
{
    m_bIsConnected = false;
    m_bIsConnecting = false;
    m_pInThread = NULL;
    m_pOutThread = NULL;
    m_nUserId = wxNOT_FOUND;
}

INetConnection::~INetConnection()
{
}

void INetConnection::SendNetMessageAsync(const wxNetMessage & msg)
{
    wxCriticalSectionLocker lock(m_dataCS);
    m_aoMessages.push(msg);
}

wxNetMessage INetConnection::SendNetMessageSync(wxNetMessage & msg)
{
    m_dataCS.Enter();
    long nWaitId = wxNewId();
    msg.SetId(nWaitId);
    //push message and wait the results from server for NET_WAIT_TIMEOUT sec.
    m_laWaitIds.Add(nWaitId);
    m_aoMessages.push(msg);
    m_dataCS.Leave();

    //can be work in parallel threads
    wxDateTime dtNow(wxDateTime::Now());
    wxTimeSpan sp = wxDateTime::Now() - dtNow;
    while(sp.GetSeconds() < NET_WAIT_TIMEOUT)
    {
        for(size_t i = 0; i < m_oaSyncMessages.size(); ++i)
        {
            if(m_oaSyncMessages[i].GetId() == nWaitId)
            {
                wxNetMessage out = m_oaSyncMessages[i];
                wxCriticalSectionLocker lock(m_msgCS);
                m_oaSyncMessages.erase(m_oaSyncMessages.begin() + i);
                
                m_laWaitIds.Remove(nWaitId);

                return out;
            }
        }

        wxTheApp->Yield(true);
        sp = wxDateTime::Now() - dtNow;
        wxMilliSleep(WAITFOR);
    };

    wxCriticalSectionLocker lock(m_msgCS);
    m_laWaitIds.Remove(nWaitId);
    return wxNetMessage(enumGISNetCmdNote, enumGISNetCmdStTimeout);
}

bool INetConnection::CreateAndRunThreads(void)
{
    if (NULL == m_pOutThread)
    {
        m_pOutThread = new wxNetWriterThread(this);
        if (!CreateAndRunThread(m_pOutThread, wxT("wxNetWriterThread"), wxT("NetWriterThread")))
            return false;
    }

    wxMilliSleep(SLEEP);

    if (NULL == m_pInThread)
    {
        m_pInThread = new wxNetReaderThread(this);
        if (!CreateAndRunThread(m_pInThread, wxT("wxNetReaderThread"), wxT("NetReaderThread")))
            return false;
    }

    return true;
}

void INetConnection::DestroyThreads(void)
{
    if (NULL != m_pInThread && m_pInThread->IsRunning())
    {
        m_pInThread->Delete();
        m_pInThread = NULL;
    }

    if (NULL != m_pOutThread && m_pOutThread->IsRunning())
    {
        m_pOutThread->Delete();
        m_pOutThread = NULL;
    }
       
    if(m_pSock)
    {
        if( m_pSock->Destroy() )
            m_pSock = NULL;
        else
            wxLogDebug(wxT("Socket not destroyed!!!"));
    }

    //wxCriticalSectionLocker lock(m_dataCS);
    //cleare quere
    while(m_aoMessages.size() > 0)
        m_aoMessages.pop();
}

bool INetConnection::ProcessOutputNetMessage(void)
{
    if(!m_pSock)
    {
        return true;
    }

    if(m_pSock->IsDisconnected())
    {
        return false;
    }

    wxCriticalSectionLocker lock(m_dataCS);   
    if(m_aoMessages.empty())
    {
        return false;
    }

    if(m_pSock->WaitForWrite(0, WAITFOR))
    {
        if(!m_pSock)
            return true;
        //m_pSock->SetTimeout(SLEEP);
        //m_pSock->SetFlags(wxSOCKET_WAITALL | wxSOCKET_BLOCK);
        wxNetMessage msgout = m_aoMessages.top();
        wxJSONWriter writer( wxJSONWRITER_NONE ); 

#ifdef USE_STREAMS
#ifdef _DEBUG
        //wxString sOut;
        //writer.Write(msgout.GetInternalValue(), sOut);
        //wxLogMessage(sOut);
#endif //_DEBUG


        wxSocketOutputStream out(*m_pSock);
        writer.Write( msgout.GetInternalValue(), out );
        //write EOF
        out.PutC(-1);
#else
        wxString sOut;
        writer.Write( msgout.GetInternalValue(), sOut );
#ifdef _DEBUG
        wxLogDebug(wxString::Format(wxT("snd:%s"), sOut));
#endif //_DEBUG


        wxUint32 nSize = m_pSock->WriteMsg(sOut.data(), sOut.size()).LastCount();
        if(nSize != sOut.size())
        {
            wxLogError(_("Failed to send network message"));
            return true;
        }
#endif //USE_STREAMS

        m_aoMessages.pop();

        return true;
    }
    else
    {
        return false;
    }
}

bool INetConnection::ProcessInputNetMessage(void)
{
    if(!m_pSock)
    {
        return true;
    }

    if(m_pSock->IsDisconnected())
    {
        return false;
    }

    if(m_pSock->WaitForRead(0, WAITFOR))
    {
        if(!m_pSock)
            return true;

        wxJSONValue  value;
        wxJSONReader reader;

#ifdef USE_STREAMS
        wxSocketInputStream in(*m_pSock);
        int numErrors = reader.Parse( in, &value );
#else
        RtlZeroMemory(m_Buffer, sizeof(m_Buffer));
        wxUint32 nRead(0);
        nRead = m_pSock->ReadMsg(m_Buffer, BUFF_SIZE).LastCount();
        wxString sIn(m_Buffer, nRead);
#ifdef _DEBUG
        wxLogDebug(wxString::Format(wxT("rcv:%d bits, %s"), nRead, sIn));
#endif
        //wxCriticalSectionLocker lock(m_msgCS);            

        //m_pSock->SetTimeout(SLEEP);
        //m_pSock->SetFlags(wxSOCKET_WAITALL | wxSOCKET_BLOCK);

        int numErrors = reader.Parse( sIn, &value );
#endif
        if ( numErrors > 0 )  
        {
            const wxArrayString& errors = reader.GetErrors();
            wxString sErrMsg(_("Invalid input message"));
            for (size_t i = 0; i < errors.GetCount(); ++i)
            {
                wxString sErr = errors[i];
                sErrMsg.Append(wxT("\n"));
                sErrMsg.Append(wxString::Format(wxT("%d. %s"), i, sErr.c_str()));
            }
            wxLogVerbose(sErrMsg);
            return false;
        }

        wxNetMessage msg(value);
        if(!msg.IsOk())
        {
            wxLogVerbose(_("Invalid input net message"));
            return false;
        }

        if(msg.GetId() != wxNOT_FOUND && m_laWaitIds.Index(msg.GetId()) != wxNOT_FOUND)
        {
            wxCriticalSectionLocker lock(m_msgCS);            
            m_oaSyncMessages.push_back(msg);
        }
        else
        {                
            //wxGISNetEvent event(m_nUserId, wxGISNET_MSG, msg);
            //PostEvent(event);
            PostEvent(new wxGISNetEvent(m_nUserId, wxGISNET_MSG, msg));
        }
        return true;
    }
    else
    {
        return false;
    }
}

//--------------------------------------------------------------------------
// Non class functions
//--------------------------------------------------------------------------
bool SendUDP(IPaddress addr, wxNetMessage & msg, bool broadcast)
{
    IPaddress addrLocal;
    addrLocal.Hostname();	
	// Set up a temporary UDP socket for sending datagrams
	wxDatagramSocket send_udp(addrLocal, (broadcast ? wxSOCKET_BROADCAST | wxSOCKET_NOBIND : wxSOCKET_NOBIND));// | wxSOCKET_NOWAIT | wxSOCKET_NOWAIT
    if ( !send_udp.IsOk() )
    {
        wxLogError(_("SendUDP: failed to create UDP socket"));
        return false;
    }
    
    wxLogDebug(wxT("SendUDP: Created UDP socket at %s:%u"), addrLocal.IPAddress(), addrLocal.Service());
    wxLogDebug(wxT("SendUDP: Testing UDP with peer at %s:%u"), addr.IPAddress(), addr.Service());

    if(!msg.IsOk())
    {
        wxLogError(_("SendUDP: invalid net message"));
        return false;
    }

    wxJSONValue val = msg.GetInternalValue();
    if(!val.IsValid())
    {
        return false;
    }

    wxJSONWriter writer;
    wxString sVal;
    writer.Write(val, sVal);
    int nSize = sVal.Len() * sizeof(sVal.GetChar(0));
    if ( send_udp.SendTo(addr, sVal.GetData(), nSize).LastCount() != nSize )
    {
        wxLogError(_("SendUDP: failed to send data"));
        return false;
    }    

    return true;
}

wxString GetSocketErrorMsg(int pSockError)
{
    switch(pSockError)
    {
        case wxSOCKET_NOERROR:
            return wxString(_("wxSOCKET_NOERROR"));

        case wxSOCKET_INVOP:
            return wxString(_("wxSOCKET_INVOP"));

        case wxSOCKET_IOERR:
            return wxString(_("wxSOCKET_IOERR"));

        case wxSOCKET_INVADDR:
            return wxString(_("wxSOCKET_INVADDR"));

        case wxSOCKET_NOHOST:
            return wxString(_("wxSOCKET_NOHOST"));

        case wxSOCKET_INVPORT:
            return wxString(_("wxSOCKET_INVPORT"));

        case wxSOCKET_WOULDBLOCK:
            return wxString(_("wxSOCKET_WOULDBLOCK"));

        case wxSOCKET_TIMEDOUT:
            return wxString(_("wxSOCKET_TIMEDOUT"));

        case wxSOCKET_MEMERR:
            return wxString(_("wxSOCKET_MEMERR"));

        default:
            return wxString(_("Unknown"));
    }
}
