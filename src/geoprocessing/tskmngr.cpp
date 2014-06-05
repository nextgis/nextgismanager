/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISTaskManager - class to communicate with task manager server
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2012-2013 Dmitry Barishnikov
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

#include "wxgis/geoprocessing/tskmngr.h"
#include "wxgis/geoprocessing/task.h"
#include "wxgis/core/config.h"
#include "wxgis/core/format.h"

static wxGISTaskManager *g_pTaskManager( NULL );

extern WXDLLIMPEXP_GIS_GP wxGISTaskManager* const GetTaskManager(void)
{
    if(g_pTaskManager == NULL)
        g_pTaskManager = new wxGISTaskManager();
    g_pTaskManager->Reference();
	return g_pTaskManager;
}

//------------------------------------------------------------------
// wxGISTaskManagerEvent
//------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGISTaskManagerEvent, wxEvent)

wxDEFINE_EVENT( wxGISTASKMNGR_CONNECT, wxGISTaskManagerEvent );
wxDEFINE_EVENT( wxGISTASKMNGR_DISCONNECT, wxGISTaskManagerEvent );


//------------------------------------------------------------------
// wxClientTCPNetConnection
//------------------------------------------------------------------
#define HOST wxT("127.0.0.1")
#define PORT 9980
#define TIMEOUT 25

IMPLEMENT_CLASS(wxGISLocalClientConnection, INetConnection)

BEGIN_EVENT_TABLE( wxGISLocalClientConnection, INetConnection )
  EVT_SOCKET(SOCKET_ID,  wxGISLocalClientConnection::OnSocketEvent)
END_EVENT_TABLE()

wxGISLocalClientConnection::wxGISLocalClientConnection(void) : INetConnection()
{
    m_pSock = NULL;
}

wxGISLocalClientConnection::~wxGISLocalClientConnection(void)
{
    DestroyThreads();
}

bool wxGISLocalClientConnection::Connect(void)
{
    if (m_bIsConnected || m_bIsConnecting)
		return true;

    wxString sHost(HOST);
    unsigned short nPort(PORT);
    unsigned short nTimeOut(TIMEOUT);
    wxGISAppConfig oConfig = GetConfig();
    if(oConfig.IsOk())
    {
        sHost = oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/taskmngr/net/host")), sHost);
        nPort = oConfig.ReadInt(enumGISHKCU, wxString(wxT("wxGISCommon/taskmngr/net/port")), nPort);
        nTimeOut = oConfig.ReadInt(enumGISHKCU, wxString(wxT("wxGISCommon/taskmngr/net/timeout")), nTimeOut);
    }

	//start conn
	IPaddress addr;
	addr.Hostname(sHost);
	addr.Service(nPort);

    // Create the socket
    wxSocketClient* pSock = new wxSocketClient(wxSOCKET_WAITALL | wxSOCKET_BLOCK | wxSOCKET_REUSEADDR);
	m_pSock = pSock;
    m_pSock->SetEventHandler(*this, SOCKET_ID);
    m_pSock->Notify(true);
    m_pSock->SetNotify(wxSOCKET_CONNECTION_FLAG|wxSOCKET_LOST_FLAG);//|wxSOCKET_INPUT
	m_pSock->SetTimeout(nTimeOut);
    pSock->Connect(addr, false);

    m_bIsConnecting = true;

    return CreateAndRunThreads();
}

bool wxGISLocalClientConnection::Disconnect(void)
{
	if(!m_bIsConnected)
		return true;

	m_bIsConnected = false;
    m_bIsConnecting = false;

    DestroyThreads();
    return true;
}

wxString wxGISLocalClientConnection::GetLastError(void) const
{
    return wxString::Format(_("Error (%d): %s"), m_pSock->LastError(), GetSocketErrorMsg(m_pSock->LastError()).c_str());
}

void wxGISLocalClientConnection::OnSocketEvent(wxSocketEvent& event)
{
    event.Skip(false);
    wxLogDebug(wxT("wxClientTCPNetConnection: event"));
    switch(event.GetSocketEvent())
    {
        case wxSOCKET_INPUT:
            wxLogDebug(wxT("wxClientTCPNetConnection: INPUT"));
        break;
        case wxSOCKET_OUTPUT:
            wxLogDebug(wxT("wxClientTCPNetConnection: OUTPUT"));
            break;
        case wxSOCKET_CONNECTION:
            wxLogDebug(wxT("wxClientTCPNetConnection: CONNECTION"));
            m_bIsConnected = true;
            m_bIsConnecting = false;
            {
                wxNetMessage msgin(enumGISNetCmdHello, enumGISNetCmdStUnk, enumGISPriorityHigh);
            PostEvent(new wxGISNetEvent(wxNOT_FOUND, wxGISNET_MSG, msgin));
            }
        break;
        case wxSOCKET_LOST:
            wxLogDebug(wxT("wxClientTCPNetConnection: LOST"));
            m_bIsConnecting = false;
            m_bIsConnected = false;
            {
                DestroyThreads();
                wxNetMessage msgin(enumGISNetCmdBye, enumGISNetCmdStUnk, enumGISPriorityHigh);
                PostEvent(new wxGISNetEvent(wxNOT_FOUND, wxGISNET_MSG, msgin));
            }
        break;
        default:
            wxLogDebug(wxT("wxClientTCPNetConnection: default"));
            break;
    }
}


//------------------------------------------------------------------
// wxGISTaskManager
//------------------------------------------------------------------
IMPLEMENT_CLASS(wxGISTaskManager, wxEvtHandler)

BEGIN_EVENT_TABLE( wxGISTaskManager, wxEvtHandler )
  EVT_GISNET_MSG(wxGISTaskManager::OnGISNetEvent)
  EVT_TIMER(TIMER_ID, wxGISTaskManager::OnTimer)
END_EVENT_TABLE()

wxGISTaskManager::wxGISTaskManager() : wxGISPointer(), m_timer(this, TIMER_ID)
{
    m_bDetailesFilled = false;
    //create local connection object
    m_pConn = new wxGISLocalClientConnection();
    m_nConnectionPointConnCookie = m_pConn->Advise(this);
    //start task manager server
    StartTaskManagerServer();
}

wxGISTaskManager::~wxGISTaskManager()
{
    m_timer.Stop();

    if(m_nConnectionPointConnCookie != wxNOT_FOUND)
        m_pConn->Unadvise(m_nConnectionPointConnCookie);
    wxDELETE(m_pConn);

    ClearCategories();

    g_pTaskManager = NULL;
}

void wxGISTaskManager::StartTaskManagerServer()
{
#ifdef __WINDOWS__
    wxString sTaskMngrServerPath(wxT("wxgistaskmanager.exe"));
#else
    wxString sTaskMngrServerPath(wxT("wxgistaskmanager"));
#endif
    wxGISAppConfig oConfig = GetConfig();
    bool bIsService = false;
    bool bIsRemote = false;
    if (oConfig.IsOk())
    {
        sTaskMngrServerPath = oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/taskmngr/exe_path")), sTaskMngrServerPath);
        bIsService = oConfig.ReadBool(enumGISHKLM, wxString(wxT("wxGISCommon/taskmngr/is_service")), bIsService);
        bIsRemote = !oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/taskmngr/net/host")), HOST).IsSameAs(HOST);
    }

    //try to connect
    if (!(bIsService || bIsRemote))
    {
        if(wxExecute(sTaskMngrServerPath + wxT(" -a"), wxEXEC_ASYNC | wxEXEC_HIDE_CONSOLE ) == 0)
        {
            wxLogError(_("Task Manager Server start failed. Path '%s'"), sTaskMngrServerPath.c_str());
        }
    }
    //start timer to connect task manager server
    m_timer.Start(5500, false); //2,5 sec. disconnect timer
}

bool wxGISTaskManager::IsValid(void) const
{
    return m_bDetailesFilled;
}

void wxGISTaskManager::OnGISNetEvent(wxGISNetEvent& event)
{
    wxNetMessage msg = event.GetNetMessage();
    if(!msg.IsOk())
    {
        wxLogDebug(wxT("Input network message is not valid"));
        return;
    }

    wxString sCategory = msg.GetValue().Get(wxT("cat"), wxJSONValue(wxEmptyString)).AsString();
    if(sCategory.IsEmpty())
    {
        switch(msg.GetCommand())
        {
        case enumGISNetCmdBye: //server disconnected
            PostEvent(new wxGISTaskManagerEvent(wxGISTASKMNGR_DISCONNECT));
            ClearCategories();
            //start task manager server
            StartTaskManagerServer();
            break;
        case enumGISNetCmdHello:
            SendNetMessageAsync(wxNetMessage(enumGISNetCmdCmd, enumGISCmdDetails, enumGISPriorityHigh));
            break;
        case enumGISNetCmdNote:
            NetNote(msg);
            break;
        case enumGISNetCmdCmd: //do something usefull
            NetCommand(msg);
            break;
        default:
            break;
        }
    }
    else
    {
        if(m_omCategories[sCategory])
            m_omCategories[sCategory]->ProcessNetMessage(msg);
    }
}

void wxGISTaskManager::SendNetMessageAsync(const wxNetMessage& msg)
{
    m_pConn->SendNetMessageAsync(msg);
}

wxNetMessage wxGISTaskManager::SendNetMessageSync(const wxNetMessage& msg)
{
    return m_pConn->SendNetMessageSync(msg);
}

bool wxGISTaskManager::SetMaxTaskExec(int nMaxExecTaskCount)
{
    wxNetMessage msg(enumGISNetCmdCmd, enumGISCmdSetParam, enumGISPriorityNormal);
    wxJSONValue val;
    val[wxT("max_exec_task_count")] = nMaxExecTaskCount;
    val[wxT("exit_state")] = m_eExitState;
    msg.SetValue(val);
    wxNetMessage msgout = SendNetMessageSync(msg);
    if(msgout.GetState() == enumGISCmdSetParam)
    {
        m_nMaxExecTasks = nMaxExecTaskCount;
        return true;
    }
    return false;
}

bool wxGISTaskManager::SetExecState(wxGISNetCommandState eExitState)
{
    wxNetMessage msg(enumGISNetCmdCmd, enumGISCmdSetParam, enumGISPriorityNormal);
    wxJSONValue val;
    val[wxT("max_exec_task_count")] = m_nMaxExecTasks;
    val[wxT("exit_state")] = eExitState;
    msg.SetValue(val);
    wxNetMessage msgout = SendNetMessageSync(msg);
    if(msgout.GetState() == enumGISCmdSetParam)
    {
        m_eExitState = eExitState;
        return true;
    }
    return false;
}

void wxGISTaskManager::NetNote(const wxNetMessage &msg)
{
    switch(msg.GetState())
    {
    case enumGISNetCmdStOk:
        wxLogMessage(msg.GetMessage());
        break;
    case enumGISNetCmdStErr:
        wxLogError(msg.GetMessage());
        break;
    case enumGISCmdNoteMsg:
    default:
        wxLogVerbose(msg.GetMessage());
        break;
    }
}

void wxGISTaskManager::NetCommand(const wxNetMessage &msg)
{
    switch(msg.GetState())
    {
    case enumGISCmdChildren:
        FillDetails(msg.GetValue());
        break;
    case enumGISCmdDetails:
        FillDetails(msg.GetValue());
        break;
    case enumGISCmdSetParam:
        m_eExitState = (wxGISNetCommandState)msg.GetValue().Get(wxT("exit_state"), m_eExitState).AsLong();
        m_nMaxExecTasks = msg.GetValue().Get(wxT("max_exec_task_count"), m_nMaxExecTasks).AsInt();
        for(wxGISTaskCategoryMap::iterator it = m_omCategories.begin(); it != m_omCategories.end(); ++it)
        {
            if(it->second)
                it->second->SetMaxTaskExec(m_nMaxExecTasks);
        }
        break;
    case enumGISCmdStAdd:
        {
            wxString sCatName = msg.GetValue().Get(wxT("name"), wxJSONValue(wxEmptyString) ).AsString();
            if(!sCatName.IsEmpty())
            {
                wxGISTaskCategory* pGISTaskCategory = new wxGISTaskCategory(sCatName, this);
                m_omCategories[sCatName] = pGISTaskCategory;
            }
        }
        break;
    case enumGISCmdStDel:
        wxFAIL_MSG(wxT("wxGISTaskManager cannot be deleted"));
        break;
    default:
        break;
    }
}

void wxGISTaskManager::DeleteCategory(wxGISTaskCategory* pCategory)
{
    wxDELETE(m_omCategories[pCategory->GetName()]);
}

void wxGISTaskManager::FillDetails(const wxJSONValue &val)
{
    if (m_bDetailesFilled || !val.IsValid())
        return;
    m_eExitState = (wxGISNetCommandState)val.Get(wxT("exit_state"), m_eExitState).AsLong();
    m_nMaxExecTasks = val.Get(wxT("max_exec_task_count"), m_nMaxExecTasks).AsInt();
    if (val.HasMember(wxT("categories")))
    {
        wxJSONValue oCategories = val[wxT("categories")];
        for( int i = 0; i < oCategories.Size(); ++i )
        {
            wxString sCatName = oCategories[i].AsString();
            wxGISTaskCategory* pGISTaskCategory = new wxGISTaskCategory(sCatName, this);
            m_omCategories[sCatName] = pGISTaskCategory;
            //request category tasks asynchronously
            wxNetMessage msg_gettasks(enumGISNetCmdCmd, enumGISCmdChildren, enumGISPriorityHigh);
            wxJSONValue val;
            val[wxT("cat")] = sCatName;
            msg_gettasks.SetValue(val);
            SendNetMessageAsync(msg_gettasks);
        }
    }
    m_bDetailesFilled = true;
    PostEvent(new wxGISTaskManagerEvent(wxGISTASKMNGR_CONNECT));
}

void wxGISTaskManager::OnTimer( wxTimerEvent & event)
{
    wxCHECK_RET(m_pConn, wxT("Network connection object is null"));
    if(m_pConn->IsConnected())
    {
        m_timer.Stop();
        return;
    }

    m_pConn->Connect();
}

wxGISTaskCategory* const wxGISTaskManager::GetCategory(const wxString& sName)
{
    return m_omCategories[sName];
}

bool wxGISTaskManager::CreateCategory(const wxString& sName)
{
    m_sLastError.Clear();
    wxNetMessage msg(enumGISNetCmdCmd, enumGISCmdStAdd, enumGISPriorityHigh);
    wxJSONValue val;
    val[wxT("name")] = sName;
    msg.SetValue(val);
    wxNetMessage msg_res = SendNetMessageSync(msg);
    if(msg_res.GetState() == enumGISCmdStAdd)
    {
        NetCommand(msg_res);
        return true;
    }
    else
    {
        m_sLastError = msg_res.GetMessage();
        return false;
    }
}

wxString wxGISTaskManager::GetLastError(void) const
{
    return m_sLastError;
}

bool wxGISTaskManager::DeleteCategory(const wxString& sName)
{
    m_sLastError.Clear();
    wxNetMessage msg(enumGISNetCmdCmd, enumGISCmdStDel, enumGISPriorityHigh);
    wxJSONValue val;
    val[wxT("name")] = sName;
    msg.SetValue(val);
    wxNetMessage msg_res = SendNetMessageSync(msg);
    if(msg_res.GetState() == enumGISNetCmdStErr)
    {
        NetCommand(msg_res);
        return true;
    }
    else
    {
        m_sLastError = msg_res.GetMessage();
        return false;
    }
}

void wxGISTaskManager::ClearCategories(void)
{
    for(wxGISTaskCategoryMap::iterator it = m_omCategories.begin(); it != m_omCategories.end(); ++it)
    {
        wxDELETE(it->second);
    }
    m_omCategories.clear();
    m_bDetailesFilled = false;
}

/*
void wxGISTaskManager::LoadTasks(const wxXmlNode* pTasksNode)
{
    wxCHECK_RET(pTasksNode, wxT("Input wxXmlNode pointer is null"));
    //if(m_bTaskLoaded)
    //    return;
    if(!pTasksNode->GetName().IsSameAs(wxT("tasks"), false))
    {
        wxLogError(_("wxGISTaskManager: Input data is not tasks list"));
        return;
    }
    wxXmlNode* pTaskNode = pTasksNode->GetChildren();
    while(pTaskNode)
    {
        AddTask(pTaskNode);
        pTaskNode = pTaskNode->GetNext();
    }
    //m_bTaskLoaded = true;
}

wxGISEnumReturnType wxGISTaskManager::QuereTasks(wxString & sMsg)
{
    long nId = wxNewId();
    wxNetMessage msg(enumGISNetCmdCmd, enumGISCmdGetChildren, enumGISPriorityHigh, nId);
    m_pConn->SendNetMessage(msg);
    return Wait(nId, sMsg);
}

/*
wxGISEnumReturnType wxGISTaskManager::AddTask(const wxString &sCategory, int nMaxExecTaskCount, const wxString &sName, const wxString &sDesc, const wxString &sPath, long nPriority, const wxXmlNode *pParameters, wxString & sMsg)
{
    long nId = wxNewId();

    wxNetMessage msg(enumGISNetCmdCmd, enumGISCmdStAdd, enumGISPriorityHigh, nId);
    wxXmlNode* pRootNode = msg.GetXMLRoot();
    if(pRootNode)
    {
        wxXmlNode* pNode = new wxXmlNode(pRootNode, wxXML_ELEMENT_NODE, wxT("task"));
        pNode->AddAttribute(wxT("cat"), sCategory);
        //for new cat creation
        SetDecimalValue(pNode, wxT("cat_max_task_exec"), nMaxExecTaskCount);
        pNode->AddAttribute(wxT("name"), sName);
        pNode->AddAttribute(wxT("desc"), sDesc);
        pNode->AddAttribute(wxT("exec"), sPath);
        SetDecimalValue(pNode, wxT("prio"), nPriority);
        SetDecimalValue(pNode, wxT("state"), enumGISTaskPaused);//enumGISTaskQuered
        pNode->AddChild(new wxXmlNode(*pParameters));

        m_pConn->SendNetMessage(msg);
    }

    return Wait(nId, sMsg);
}

wxGISEnumReturnType wxGISTaskManager::SetMaxTaskExec(const wxString &sCategory, int nMaxExecTaskCount, wxString & sMsg)
{
    long nId = wxNewId();

    wxNetMessage msg(enumGISNetCmdCmd, enumGISCmdSetParam, enumGISPriorityHigh, nId);
    wxXmlNode* pRootNode = msg.GetXMLRoot();
    if(pRootNode)
    {
        wxXmlNode* pNode = new wxXmlNode(pRootNode, wxXML_ELEMENT_NODE, wxT("cat"));
        pNode->AddAttribute(wxT("name"), sCategory);
        SetDecimalValue(pNode, wxT("cat_max_task_exec"), nMaxExecTaskCount);
        m_pConn->SendNetMessage(msg);
    }

    return Wait(nId, sMsg);
}

void wxGISTaskManager::AddTask(const wxXmlNode* pIniNode)
{
    wxCHECK_RET(pIniNode, wxT("Input wxXmlNode pointer is null"));
    long nId = GetDecimalValue(pIniNode, wxT("id"), wxNOT_FOUND);
    if(m_moTasks[nId])
        //the task is already exist
        return;

    wxGISTask* pTask = new wxGISTask(pIniNode);
    m_moTasks[pTask->GetId()] = pTask;
    //notify
    wxGISTaskEvent event(pTask->GetId(), wxGISTASK_ADD);
    PostEvent(event);

    //quere messages
    wxNetMessage msg(enumGISNetCmdCmd, enumGISCmdGetDetails, enumGISPriorityLow, nId);
    m_pConn->SendNetMessage(msg);
}

void wxGISTaskManager::DeleteTask(const wxXmlNode* pIniNode)
{
    wxCHECK_RET(pIniNode, wxT("Input wxXmlNode pointer is null"));
    int nTaskId = GetDecimalValue(pIniNode, wxT("id"), wxNOT_FOUND);
    if(nTaskId == wxNOT_FOUND)
        return;
    wxDELETE(m_moTasks[nTaskId]);
    m_moTasks.erase(nTaskId);
    //notify
    wxGISTaskEvent event(nTaskId, wxGISTASK_DEL);
    PostEvent(event);
}

void wxGISTaskManager::ChangeTask(const wxXmlNode* pIniNode)
{
    wxCHECK_RET(pIniNode, wxT("Input wxXmlNode pointer is null"));
    int nTaskId = GetDecimalValue(pIniNode, wxT("id"), wxNOT_FOUND);
    if(nTaskId == wxNOT_FOUND)
        return;
    if(m_moTasks[nTaskId] == NULL)
        return;
    m_moTasks[nTaskId]->ChangeTask(pIniNode);
    //notify
    wxGISTaskEvent event(nTaskId, wxGISTASK_CHNG);
    PostEvent(event);
}

wxGISTaskMap wxGISTaskManager::GetTasks(void)
{
    return m_moTasks;
}

wxGISTask* const wxGISTaskManager::GetTask(int nTaskId)
{
    return m_moTasks[nTaskId];
}

wxGISEnumReturnType wxGISTaskManager::DeleteTask(int nTaskId, wxString & sMsg)
{
    long nId = wxNewId();
    wxNetMessage msg(enumGISNetCmdCmd, enumGISCmdStDel, enumGISPriorityHigh, nId);
    wxXmlNode* pRootNode = msg.GetXMLRoot();
    if(pRootNode)
    {
        wxXmlNode* pNode = new wxXmlNode(pRootNode, wxXML_ELEMENT_NODE, wxT("task"));
        wxGISTask* pTask = GetTask(nTaskId);
        pNode->AddAttribute(wxT("cat"), pTask->GetCategory());
        SetDecimalValue(pNode, wxT("id"), nTaskId);
        m_pConn->SendNetMessage(msg);
    }
    else
    {
        sMsg = wxString(_("Net message create failed"));
        return enumGISReturnFailed;
    }
    return Wait(nId, sMsg);
}

wxGISEnumReturnType wxGISTaskManager::StartTask(int nTaskId, wxString & sMsg)
{
    //check if task is already started
    if(m_moTasks[nTaskId] && (m_moTasks[nTaskId]->GetState() == enumGISTaskQuered || m_moTasks[nTaskId]->GetState() == enumGISTaskQuered))
    {
        sMsg = wxString::Format(_("The task %d already started"), nTaskId);
        return enumGISReturnFailed;
    }
    long nId = wxNewId();
    wxNetMessage msg(enumGISNetCmdCmd, enumGISCmdStStart, enumGISPriorityHigh, nId);
    wxXmlNode* pRootNode = msg.GetXMLRoot();
    if(pRootNode)
    {
        wxXmlNode* pNode = new wxXmlNode(pRootNode, wxXML_ELEMENT_NODE, wxT("task"));
        wxGISTask* pTask = GetTask(nTaskId);
        pNode->AddAttribute(wxT("cat"), pTask->GetCategory());
        SetDecimalValue(pNode, wxT("id"), nTaskId);
        m_pConn->SendNetMessage(msg);
    }
    else
    {
        sMsg = wxString(_("Net message create failed"));
        return enumGISReturnFailed;
    }
    return Wait(nId, sMsg);
}

wxGISEnumReturnType wxGISTaskManager::StopTask(int nTaskId, wxString & sMsg)
{
    if(m_moTasks[nTaskId] && (m_moTasks[nTaskId]->GetState() == enumGISTaskPaused || m_moTasks[nTaskId]->GetState() == enumGISTaskError || m_moTasks[nTaskId]->GetState() == enumGISTaskDone))
    {
        sMsg = wxString::Format(_("The task %d already stopped"), nTaskId);
        return enumGISReturnFailed;
    }
    long nId = wxNewId();
    wxNetMessage msg(enumGISNetCmdCmd, enumGISCmdStStop, enumGISPriorityHigh, nId);
    wxXmlNode* pRootNode = msg.GetXMLRoot();
    if(pRootNode)
    {
        wxXmlNode* pNode = new wxXmlNode(pRootNode, wxXML_ELEMENT_NODE, wxT("task"));
        wxGISTask* pTask = GetTask(nTaskId);
        pNode->AddAttribute(wxT("cat"), pTask->GetCategory());
        SetDecimalValue(pNode, wxT("id"), nTaskId);
        m_pConn->SendNetMessage(msg);
    }
    else
    {
        sMsg = wxString(_("Net message create failed"));
        return enumGISReturnFailed;
    }
    return Wait(nId, sMsg);
}
/* TODO: Add const array id <-> priority
wxGISEnumReturnType wxGISTaskManager::ChangeTasksPriority(int nTaskId, long nPriority, wxString & sMsg)
{
    long nId = wxNewId();
    wxNetMessage msg(enumGISNetCmdCmd, enumGISCmdStPriority, enumGISPriorityHigh, nId);
    wxXmlNode* pRootNode = msg.GetXMLRoot();
    if(pRootNode)
    {
        wxXmlNode* pNode = new wxXmlNode(pRootNode, wxXML_ELEMENT_NODE, wxT("task"));
        wxGISTask* pTask = GetTask(nTaskId);
        pNode->AddAttribute(wxT("cat"), pTask->GetCategory());
        SetDecimalValue(pNode, wxT("id"), nTaskId);
        SetDecimalValue(pNode, wxT("prio"), nPriority);
        m_pConn->SendNetMessage(msg);
    }
    else
    {
        sMsg = wxString(_("Net message create failed"));
        return enumGISReturnFailed;
    }
    return Wait(nId, sMsg);
}
*/
/*
int wxGISTaskManager::GetRunTaskCount(const wxString& sCat)
{
    int nCount(0);
    for(wxGISTaskMap::const_iterator it = m_moTasks.begin(); it != m_moTasks.end(); ++it)
    {
        if(it->second && it->second->GetCategory().StartsWith(sCat))
        {
            if(it->second->GetState() == enumGISTaskWork)
                nCount++;
        }
    }
    return nCount;
}

//void wxGISTaskManager::UpdateVolume(const wxXmlNode* pIniNode)
//{
//    wxCHECK_RET(pIniNode, wxT("Input wxXmlNode pointer is null"));
//    int nTaskId = GetDecimalValue(pIniNode, wxT("id"), wxNOT_FOUND);
//    if(nTaskId == wxNOT_FOUND)
//        return;
//    wxGISTask* pTasks = m_moTasks[nTaskId];
//    if(pTasks == NULL)
//        return;
//
//    long nNewVolume = GetDecimalValue(pIniNode, wxT("vol"), pTasks->GetVolume());
//    pTasks->SetVolume(nNewVolume);
//    //notify
//    wxGISTaskEvent event(nTaskId, wxGISTASK_VOLCHNG);
//    PostEvent(event);
//}
//
//void wxGISTaskManager::UpdatePercent(const wxXmlNode* pIniNode)
//{
//    wxCHECK_RET(pIniNode, wxT("Input wxXmlNode pointer is null"));
//    int nTaskId = GetDecimalValue(pIniNode, wxT("id"), wxNOT_FOUND);
//    if(nTaskId == wxNOT_FOUND)
//        return;
//    wxGISTask* pTasks = m_moTasks[nTaskId];
//    if(pTasks == NULL)
//        return;
//
//    double dfDone = GetFloatValue(pIniNode, wxT("done"), pTasks->GetDone());
//    wxDateTime dtEnd = GetDateValue(pIniNode, wxT("end"), pTasks->GetDateEnd());
//    wxGISEnumTaskStateType eState = (wxGISEnumTaskStateType)GetDecimalValue(pIniNode, wxT("state"), (int)pTasks->GetState());
//    pTasks->SetDone(dfDone);
//    pTasks->SetDateEnd(dtEnd);
//    pTasks->SetState(eState);
//
//    //notify
//    wxGISTaskEvent event(nTaskId, wxGISTASK_PERCENTCHNG);
//    PostEvent(event);
//}

void wxGISTaskManager::AddMessage(const wxXmlNode* pIniNode)
{
    wxCHECK_RET(pIniNode, wxT("Input wxXmlNode pointer is null"));
    int nTaskId = GetDecimalValue(pIniNode, wxT("id"), wxNOT_FOUND);
    if(nTaskId == wxNOT_FOUND)
        return;
    wxGISTask* pTasks = m_moTasks[nTaskId];
    if(pTasks == NULL)
        return;

    long nMsgId = GetDecimalValue(pIniNode, wxT("msg_id"), wxNOT_FOUND);
    if(nMsgId == wxNOT_FOUND)
        return;

    wxGISEnumMessageType eType = (wxGISEnumMessageType)GetDecimalValue(pIniNode, wxT("msg_type"), enumGISMessageUnk);
    wxDateTime dt = GetDateValue(pIniNode, wxT("msg_dt"), wxDateTime::Now());
    wxString sInfoData = pIniNode->GetAttribute(wxT("msg"));

    if(!dt.IsValid() || sInfoData.IsEmpty())
        return;

    pTasks->AddMessage(new wxGISTaskMessage(nMsgId, sInfoData, eType, dt));

    //notify
    wxGISTaskEvent event(nTaskId, wxGISTASK_MESSAGEADDED, nMsgId);
    PostEvent(event);
}
*/
