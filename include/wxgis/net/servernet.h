/******************************************************************************
 * Project:  wxGIS
 * Purpose:  network server class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2008-2010,2014 Dmitry Baryshnikov
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

#include "wxgis/net/netconn.h"
#include "wxgis/net/netevent.h"

/** @class INetPlugin

    A Server side INetPlugin interface.

    @library{net}
*/
class WXDLLIMPEXP_GIS_NET INetPlugin : public wxEvtHandler
{
    DECLARE_ABSTRACT_CLASS(INetPlugin)
public:
    virtual ~INetPlugin(void);
    virtual bool Start(INetService* pNetService, const wxXmlNode* pConfig) = 0;
    virtual bool Stop(void) = 0;
};

WX_DECLARE_LIST_2(wxGISNetServerConnection, wxGISNetServerConnectionList, wxGISNetServerConnectionNode, class WXDLLIMPEXP_GIS_NET);

/** @class wxGISNetworkService

    A Server side Network Service.

    @library{net}
*/
class WXDLLIMPEXP_GIS_NET wxGISNetworkService :
    public wxEvtHandler,
    public INetService
{
    DECLARE_CLASS(wxGISNetworkService)
public:
    wxGISNetworkService(INetEventProcessor* pNetEvtProc);
    virtual ~wxGISNetworkService(void);
    ////events
    //void OnSocketEvent(wxSocketEvent& pEvent);
    //INetService
    virtual bool Start(void);
    virtual bool Stop(void);
    virtual wxString GetServerName(void) const;
    virtual bool CanConnect(const wxString &sName, const wxString &sPass);
    virtual void AddConnection(wxGISNetServerConnection* pConn);
    virtual bool DestroyConnection( wxGISNetServerConnection* pConn );
    virtual void RemoveConnection( wxGISNetServerConnection* pConn );
    //wxGISNetworkService
    virtual bool DestroyConnections();
    virtual void SendNetMessage(const wxNetMessage & msg, int nId = wxNOT_FOUND);
    virtual void AddNetworkPlugin(const wxString &sClassName, const wxXmlNode *pConfigNode);
protected:
    virtual wxGISNetServerConnectionList& GetConnections() { return m_plNetworkConnections; }
    //events
    virtual void OnGISNetEvent(wxGISNetEvent& event);
protected:
    short m_nMaxConnectoinCount;
    int m_nConnectionCounter;
    wxVector<INetPlugin*> m_paNetworkPlugins;
    wxString m_sServerName;

    wxGISNetServerConnectionList m_plNetworkConnections;
    int m_nConnectionIdCounter;
    INetEventProcessor* m_pNetEvtProc;
private:
    DECLARE_EVENT_TABLE()
};

/** @fn wxGISNetworkService* const GetNetworkService(void)
 *
 *  Global NetworkService getter.
 *
 *  @library{net}
 */

WXDLLIMPEXP_GIS_NET wxGISNetworkService* const GetNetworkService(void);

/** \fn void SetNetworkService(wxGISNetworkService* pService)
    \brief Global NetworkService setter.
	\param pService The NetworkService pointer.
 */

WXDLLIMPEXP_GIS_NET void SetNetworkService(wxGISNetworkService* pService);


