/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISTask and wxGISTaskCategories classes
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2013,2014 Dmitry Baryshnikov
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

#include "wxgis/geoprocessing/task.h"
#include "wxgis/core/format.h"


#include <wx/arrimpl.cpp> // This is a magic incantation which must be done!
WX_DEFINE_USER_EXPORTED_OBJARRAY(wxGISTaskMessagesArray);


//------------------------------------------------------------------
// wxGISTaskEvent
//------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGISTaskEvent, wxEvent)

wxDEFINE_EVENT( wxGISTASK_ADD, wxGISTaskEvent );
wxDEFINE_EVENT( wxGISTASK_DEL, wxGISTaskEvent );
wxDEFINE_EVENT( wxGISTASK_CHNG, wxGISTaskEvent );
wxDEFINE_EVENT( wxGISTASK_MESSAGEADDED, wxGISTaskEvent );

//------------------------------------------------------------------
// wxGISTaskBase
//------------------------------------------------------------------
IMPLEMENT_CLASS(wxGISTaskBase, wxObject)

wxGISTaskBase::wxGISTaskBase(wxGISTaskBase *pParentTask)
{
    m_pParentTask = pParentTask;
}

wxGISTaskBase::~wxGISTaskBase(void)
{
    ClearTasks();
}

int wxGISTaskBase::GetId(void) const
{
    return m_nId;
}

wxString wxGISTaskBase::GetName(void) const
{
    return m_sName;
}

wxString wxGISTaskBase::GetLastError() const
{
    return m_sLastError;
}

void wxGISTaskBase::ClearTasks(void)
{
    for(wxGISTaskMap::iterator it = m_omSubTasks.begin(); it != m_omSubTasks.end(); ++it)
    {
        wxDELETE(it->second);
    }
    m_omSubTasks.clear();
}

bool wxGISTaskBase::Delete(void)
{
    wxJSONValue val;
    val[wxT("id")] = m_nId;
    if(m_pParentTask->SendNetMessageSync(enumGISNetCmdCmd, enumGISCmdStDel, val) == enumGISCmdStDel)
    {
        NetCommand(enumGISCmdStDel, val); 
        return true;
    }
    return false;
}

wxJSONValue wxGISTaskBase::GetConfig(void)
{
    wxJSONValue val;
    val[wxT("id")] = m_nId;
    val[wxT("name")] = m_sName;

    int nCount(0);
    for(wxGISTaskMap::iterator it = m_omSubTasks.begin(); it != m_omSubTasks.end(); ++it)
    {
        val[wxT("subtask")][nCount++] = it->second->GetConfig();
    }
    return val;
}

void wxGISTaskBase::NetMessage(wxGISNetCommand eCmd, wxGISNetCommandState eCmdState, const wxJSONValue &val)
{
    if(val.HasMember(wxT("subtask")))
    {
        int nId = val[wxT("subtask")][wxT("id")].AsInt();
        if (m_omSubTasks[nId])
        {
            m_omSubTasks[nId]->NetMessage(eCmd, eCmdState, val[wxT("subtask")]);
            return;
        }
    }

    switch(eCmd)
    {
    case enumGISNetCmdCmd:
        NetCommand(eCmdState, val);
        break;
    case enumGISNetCmdNote:
        NetNote(eCmdState, val);
        break;
    default:
        break;
    }
}

void wxGISTaskBase::NetCommand(wxGISNetCommandState eCmdState, const wxJSONValue &val)
{
    switch(eCmdState)
    {
    case enumGISCmdStAdd:
        AddTask(val);
        break;
    case enumGISCmdStDel:
        if(m_pParentTask)
        {
            AddEvent(wxGISTaskEvent(GetId(), wxGISTASK_DEL));
            m_pParentTask->Delete(this);
        }
        break;
    case enumGISCmdChildren:
        if(val.HasMember(wxT("subtasks")))
        {
            wxJSONValue stval = val[wxT("subtasks")];
            for(size_t i = 0; i < stval.Size(); ++i)
            {
                if(m_omSubTasks[stval[i][wxT("id")].AsInt()] == NULL)
                {
                    wxGISTask* pTask = new wxGISTask(this, stval[i]);
                    wxASSERT(m_omSubTasks[pTask->GetId()] == NULL);
                    m_omSubTasks[pTask->GetId()] = pTask;

                    if (pTask->GetChildrenCount() > 0)
                    {
                        //request subtasks asynchronously
                        wxJSONValue outval;
                        outval[wxT("id")] = pTask->GetId();
                        SendNetMessageAsync(enumGISNetCmdCmd, enumGISCmdChildren, outval);
                    }
                        
                    AddEvent(wxGISTaskEvent(pTask->GetId(), wxGISTASK_ADD));
                }
            }
        }
        break;
    case enumGISCmdStChng:
        ChangeTask(val);
        break;
    default:
        break;
    }
}

void wxGISTaskBase::AddTask(const wxJSONValue &val)
{
    if(val.HasMember(wxT("task")))
    {
        if(m_omSubTasks[val[wxT("id")].AsInt()] == NULL)
        {
            wxGISTask* pTask = new wxGISTask(this, val);
            wxASSERT(m_omSubTasks[pTask->GetId()] == NULL);
            m_omSubTasks[pTask->GetId()] = pTask;

            if (pTask->GetChildrenCount() > 0)
            {
                //request subtasks asynchronously
                wxJSONValue outval;
                outval[wxT("id")] = m_nId;
                SendNetMessageAsync(enumGISNetCmdCmd, enumGISCmdChildren, outval);
            }

            AddEvent(wxGISTaskEvent(pTask->GetId(), wxGISTASK_ADD));
        }
    }
}

void wxGISTaskBase::ChangeTask(const wxJSONValue &val)
{
    m_sName = val.Get(wxT("name"), wxJSONValue(m_sName)).AsString();
    m_nState = (wxGISEnumTaskStateType)val.Get(wxT("state"), wxJSONValue(m_nState)).AsLong();

    AddEvent(wxGISTaskEvent(GetId(), wxGISTASK_CHNG));
    m_pParentTask->OnSubTaskChanged(GetId());
}

bool wxGISTaskBase::RenameTask(const wxString& sNewName)
{
    //check if name is already exist
    return false;
}

void wxGISTaskBase::OnSubTaskChanged(int nId)
{
    AddEvent(wxGISTaskEvent(GetId(), wxGISTASK_CHNG));
}

void wxGISTaskBase::NetNote(wxGISNetCommandState eCmdState, const wxJSONValue &val)
{
    if (!val.HasMember(wxT("msg")))
        return;
    wxString sMsg = val[wxT("msg")].AsString();
    switch(eCmdState)
    {
    case enumGISNetCmdStOk:
        wxLogMessage(sMsg);
        break;
    case enumGISNetCmdStErr:
        wxLogError(sMsg);
        break;
    case enumGISCmdNoteMsg:
    default:
        wxLogVerbose(sMsg);
        break;
    }
}

void wxGISTaskBase::SendNetMessageAsync(wxGISNetCommand eCmd, wxGISNetCommandState eCmdState, const wxJSONValue &val)
{
    wxJSONValue parent_val;
    parent_val[wxT("id")] = m_nId;
    parent_val[wxT("subtask")] = val;
    return m_pParentTask->SendNetMessageAsync(eCmd, eCmdState, parent_val);
}

wxGISNetCommandState wxGISTaskBase::SendNetMessageSync(wxGISNetCommand eCmd, wxGISNetCommandState eCmdState, const wxJSONValue &val)
{
    wxJSONValue parent_val;
    parent_val[wxT("id")] = m_nId;
    parent_val[wxT("subtask")] = val;
    return m_pParentTask->SendNetMessageSync(eCmd, eCmdState, parent_val);
}

void wxGISTaskBase::Delete(wxGISTaskBase *pTask)
{
    wxCHECK_RET(pTask, wxT("Input task pointer is null"));
    wxDELETE(m_omSubTasks[pTask->GetId()]);
}

size_t wxGISTaskBase::GetSubTaskCount(void) const
{
    return m_omSubTasks.size();
}

wxGISTaskBase* wxGISTaskBase::GetSubTask(size_t nIndex) const
{
    wxGISTaskMap::const_iterator it = m_omSubTasks.begin();
    for(size_t i = 0; i < nIndex; ++i)
        it++;
    //std::advance(it, nIndex);
    if(it != m_omSubTasks.end())
        return it->second;
    return NULL;
}

wxGISTaskBase* wxGISTaskBase::GetSubTask(int nId) const
{
    wxGISTaskMap::const_iterator it = m_omSubTasks.find(nId);
    if(it != m_omSubTasks.end())
        return it->second;
    return NULL;
}


wxGISEnumTaskStateType wxGISTaskBase::GetState(void) const
{
    wxGISEnumTaskStateType out(enumGISTaskPaused);
    short nDoneCount(0), nPausedCount(0), nQueredCount(0);

    for(wxGISTaskMap::const_iterator it = m_omSubTasks.begin(); it != m_omSubTasks.end(); ++it)
    {
        wxGISTaskBase* pTask = it->second;
        if(pTask)
        {
            if(pTask->GetState() == enumGISTaskError)
                return enumGISTaskError;
            if(pTask->GetState() == enumGISTaskWork)
                return enumGISTaskWork;
            if(pTask->GetState() == enumGISTaskDone)
                nDoneCount++;
            if(pTask->GetState() == enumGISTaskQuered)
                nQueredCount++;
            if(pTask->GetState() == enumGISTaskPaused)
                nPausedCount++;  
        }
    }

    //All subtasks done
    if(nDoneCount != 0 && nDoneCount == m_omSubTasks.size())
        return enumGISTaskDone;
    if(nQueredCount != 0)
        return enumGISTaskQuered;
    if(nPausedCount != 0)
        return enumGISTaskPaused;

    return m_nState;
}

int wxGISTaskBase::GetRunTaskCount(void) const
{
    if(m_omSubTasks.size() == 0)
    {
        return GetState() == enumGISTaskWork;
    }
    else
    {
        int nCount = 0;
        for(wxGISTaskMap::const_iterator it = m_omSubTasks.begin(); it != m_omSubTasks.end(); ++it)
        {
            wxGISTaskBase* pTask = it->second;
            if(pTask && pTask->GetState() == enumGISTaskWork)
                nCount++;
        }
        return nCount;
   }
}


//------------------------------------------------------------------
// wxGISTask
//------------------------------------------------------------------
IMPLEMENT_CLASS(wxGISTask, wxGISTaskBase)

wxGISTask::wxGISTask(wxGISTaskBase *pParentTask, const wxJSONValue &TaskConfig) : wxGISTaskBase(pParentTask)
{
    if(!TaskConfig.IsNull())
    {
        //load
        m_nId = TaskConfig.Get(wxT("id"), wxJSONValue(wxNOT_FOUND)).AsInt();
        m_sName = TaskConfig.Get(wxT("name"), wxJSONValue(NONAME)).AsString();
        m_sDescription = TaskConfig.Get(wxT("desc"), wxJSONValue(NONAME)).AsString();
        m_sExecPath = TaskConfig.Get(wxT("exec"), wxJSONValue(NONAME)).AsString();
        m_nState = (wxGISEnumTaskStateType)TaskConfig.Get(wxT("state"), wxJSONValue(enumGISTaskUnk)).AsLong();
        m_nPriority = TaskConfig.Get(wxT("prio"), wxJSONValue(wxNOT_FOUND)).AsLong();
        m_nGroupId = TaskConfig.Get(wxT("groupid"), wxJSONValue(wxNOT_FOUND)).AsInt();

        m_dtBeg = GetDateValue(TaskConfig, wxT("beg"), wxDateTime::Now());
        m_dtEnd = GetDateValue(TaskConfig, wxT("end"), wxDateTime::Now());

        m_dtCreated = GetDateValue(TaskConfig, wxT("create"), wxDateTime::Now());
        m_nVolume = TaskConfig.Get(wxT("vol"), wxJSONValue(wxUint64(0))).AsUInt64();
        m_dfDone = TaskConfig.Get(wxT("done"), wxJSONValue(0.0)).AsDouble();

        m_nChildrenCount = TaskConfig.Get(wxT("subtask_count"), 0).AsInt();

        if (TaskConfig.HasMember(wxT("params")))
            m_Params = TaskConfig[wxT("params")];
#ifdef _DEBUG
//        wxJSONWriter writer( wxJSONWRITER_NONE );  
//        wxString sOut;
//        writer.Write( m_Params, sOut );
//        wxLogDebug(wxT("task %d params: %s"), m_nId, sOut);
#endif

        if(TaskConfig.HasMember(wxT("subtasks")))
        {
            m_nChildrenCount = 0;
            wxJSONValue subtasks = TaskConfig[wxT("subtasks")];
            for(size_t i = 0; i < subtasks.Size(); ++i)
            {
                wxGISTask* pTask = new wxGISTask(this, subtasks[i]);
                AddSubTask(pTask);        
            }
        }
    }
    else
    {
        m_dtCreated = wxDateTime::Now();
        m_nVolume = 0;
        m_dfDone = 0;
        m_nPriority = wxNOT_FOUND;
        m_nId = wxNewId();
    }
}

wxGISTask::~wxGISTask(void)
{
}

int wxGISTask::GetChildrenCount() const
{
    return m_nChildrenCount;
}

void wxGISTask::AddSubTask(wxGISTask* pTask)
{
    wxCHECK_RET(pTask, wxT("Input task pointer is null"));
    m_omSubTasks[pTask->GetId()] = pTask;        
}

wxString wxGISTask::GetDescription(void) const
{
    return m_sDescription;
}

wxString wxGISTask::GetExecutable(void) const
{
    return m_sExecPath;
}

wxDateTime wxGISTask::GetDateBegin(void) const
{
    wxDateTime out(wxDateTime::Now());
    for(wxGISTaskMap::const_iterator it = m_omSubTasks.begin(); it != m_omSubTasks.end(); ++it)
    {
        wxGISTask* pTask = wxDynamicCast(it->second, wxGISTask);
        if(pTask)
        {
            if(pTask->GetDateBegin() < out)
                out = pTask->GetDateBegin();
        }
    }
    return m_dtBeg < out ? m_dtBeg : out;
}

wxDateTime wxGISTask::GetDateEnd(void) const
{
    wxDateTime out(wxDateTime::Now());
    for(wxGISTaskMap::const_iterator it = m_omSubTasks.begin(); it != m_omSubTasks.end(); ++it)
    {
        wxGISTask* pTask = wxDynamicCast(it->second, wxGISTask);
        if(pTask)
        {
            if(pTask->GetDateEnd() > out)
                out = pTask->GetDateEnd();
        }
    }
    return m_dtEnd > out ? m_dtEnd : out;
}

wxDateTime wxGISTask::GetDateCreated(void) const
{
    return m_dtCreated;
}

wxULongLong  wxGISTask::GetVolume(void) const
{
    wxULongLong out(0);
    for(wxGISTaskMap::const_iterator it = m_omSubTasks.begin(); it != m_omSubTasks.end(); ++it)
    {
        wxGISTask* pTask = wxDynamicCast(it->second, wxGISTask);
        if(pTask)
        {
            out += pTask->GetVolume();
        }
    }
    return out + m_nVolume;
}

long wxGISTask::GetPriority(void) const
{
    return m_nPriority;
}

void wxGISTask::SetPriority(long nPriority)
{
    m_nPriority = nPriority;
}

double wxGISTask::GetDone(void) const
{
    if (GetState() == enumGISTaskDone)
        return 100;
    if(m_omSubTasks.empty())
        return m_dfDone;

    double out(0);
    for(wxGISTaskMap::const_iterator it = m_omSubTasks.begin(); it != m_omSubTasks.end(); ++it)
    {
        wxGISTask* pTask = wxDynamicCast(it->second, wxGISTask);
        if(pTask)
        {
            out += pTask->GetDone();
        }
    }
    return out / m_omSubTasks.size();
}

wxGISTaskMessagesArray wxGISTask::GetMessages(void) const
{
    return m_oaMessages;
}

void wxGISTask::SetState(wxGISEnumTaskStateType eState)
{
    m_nState = eState;
}

wxJSONValue wxGISTask::GetParameters(void) const
{
    return m_Params;
}

void wxGISTask::ChangeTask(const wxJSONValue &val)
{
    m_sName = val.Get(wxT("name"), wxJSONValue(m_sName)).AsString();
    m_sDescription = val.Get(wxT("desc"), wxJSONValue(m_sDescription)).AsString();
    m_sExecPath = val.Get(wxT("exec"), wxJSONValue(m_sExecPath)).AsString();
    m_nState = (wxGISEnumTaskStateType)val.Get(wxT("state"), wxJSONValue(m_nState)).AsLong();
    m_nPriority = val.Get(wxT("prio"), wxJSONValue(m_nPriority)).AsLong();
    m_nGroupId = val.Get(wxT("groupid"), wxJSONValue(m_nGroupId)).AsInt();

    m_dtBeg = GetDateValue(val, wxT("beg"), m_dtBeg);
    m_dtEnd = GetDateValue(val, wxT("end"), m_dtEnd);

    m_dtCreated = GetDateValue(val, wxT("create"), m_dtCreated);
    m_nVolume = val.Get(wxT("vol"), wxJSONValue(wxUint64(m_nVolume.GetValue()))).AsUInt64();
    m_dfDone = val.Get(wxT("done"), wxJSONValue(m_dfDone)).AsDouble();

    if(val.HasMember(wxT("params")))
        m_Params = val[wxT("params")];

    AddEvent(wxGISTaskEvent(GetId(), wxGISTASK_CHNG));
    m_pParentTask->OnSubTaskChanged(GetId());
}

void wxGISTask::NetNote(wxGISNetCommandState eCmdState, const wxJSONValue &val)
{
    switch(eCmdState)
    {
    case enumGISCmdNoteMsg:
        if (val.HasMember(wxT("msg")) && val.HasMember(wxT("date")) && val.HasMember(wxT("type")) && val.HasMember(wxT("msg_id")))
        {

            wxString sMsg = val[wxT("msg")].AsString();
            wxDateTime dtm = GetDateValue(val, wxT("date"), wxDateTime::Now());
            wxGISEnumMessageType eType = (wxGISEnumMessageType)val[wxT("type")].AsLong();
            long nMsgId = val[wxT("msg_id")].AsLong();
            //if(nType != enumGISMessageUnk)
            //{
                AddMessage(new wxGISTaskMessage(nMsgId, sMsg, eType, dtm));
            //}
        }
        break;
    case enumGISNetCmdStOk:
    case enumGISNetCmdStErr:
    default:
        wxGISTaskBase::NetNote(eCmdState, val);
        break;
    }

}

wxGISTaskMessage wxGISTask::GetMessage(long nMessageId) const
{
    for(size_t i = 0; i < m_oaMessages.GetCount(); ++i)
    {
        if(m_oaMessages[i].m_nId == nMessageId)
        {
            return m_oaMessages[i];
        }
    }
    return wxGISTaskMessage(wxNOT_FOUND);
}

void wxGISTask::AddMessage(wxGISTaskMessage* pMessage)
{
    for (size_t i = 0; i < m_oaMessages.GetCount(); ++i)
    {
        if(m_oaMessages[i].m_nId == pMessage->m_nId)
        {
            wxDELETE(pMessage);
            return;
        }
    }
    m_oaMessages.Add(pMessage);
    
    AddEvent(wxGISTaskEvent(GetId(), wxGISTASK_MESSAGEADDED, pMessage->m_nId));
}

wxJSONValue wxGISTask::GetConfig(void)
{
    wxJSONValue val;
    val[wxT("id")] = m_nId;
    val[wxT("name")] = m_sName;
    val[wxT("desc")] = m_sDescription;
    val[wxT("groupid")] = m_nGroupId;
    val[wxT("state")] = m_nState;
    val[wxT("prio")] = m_nPriority;
    val[wxT("exec")] = m_sExecPath;
    //val[wxT("beg")] = SetDateValue(m_dtBeg);
    //val[wxT("end")] = SetDateValue(m_dtEnd);
    val[wxT("vol")] = wxUint64(m_nVolume.GetValue());
    val[wxT("done")] = m_dfDone;
    val[wxT("params")] = m_Params;
    //val[wxT("subtask_count")] = m_nChildrenCount;

    int nCount(0);
    for(wxGISTaskMap::iterator it = m_omSubTasks.begin(); it != m_omSubTasks.end(); ++it)
    {
        val[wxT("subtasks")][nCount++] = it->second->GetConfig();
    }
    return val;
}

bool wxGISTask::StartTask(bool bWait)
{
    wxJSONValue val;
    val[wxT("id")] = m_nId;
    if (bWait)
    {
        if(m_pParentTask->SendNetMessageSync(enumGISNetCmdCmd, enumGISCmdStStart, val) == enumGISCmdStStart)
        {
            return true;
        }
        return false;
    }
    else
    {
        m_pParentTask->SendNetMessageAsync(enumGISNetCmdCmd, enumGISCmdStStart, val);
        return true;
    }
}

bool wxGISTask::StopTask(bool bWait)
{
    wxJSONValue val;
    val[wxT("id")] = m_nId;
    if (bWait)
    {
        if (m_pParentTask->SendNetMessageSync(enumGISNetCmdCmd, enumGISCmdStStop, val) == enumGISCmdStStop)
        {
            return true;
        }
        return false;
    }
    else
    {
        m_pParentTask->SendNetMessageAsync(enumGISNetCmdCmd, enumGISCmdStStop, val);
        return true;
    }
}

//void wxGISTask::LoadMessages(wxXmlNode* const pMessages)
//{
//    wxCHECK_RET(pMessages, wxT("Input wxXmlNode pointer  is null"));
//
//    /*if(m_oaMessages.GetCount() > 100)
//        m_oaMessages.RemoveAt(0, 50);
//
//    wxXmlNode* pMsg = pMessages->GetChildren();
//    while(pMsg)
//    {
//        wxGISEnumMessageType nType = (wxGISEnumMessageType)GetDecimalValue(pMsg, wxT("type"), (int)enumGISMessageUnk);
//        wxDateTime dt = GetDateValue(pMsg, wxT("dt"), wxDateTime::Now());
//        wxString sMsg = pMsg->GetAttribute(wxT("text"));
//        int nId = GetDecimalValue(pMsg, wxT("id"), wxNOT_FOUND);
//
//        bool bAdd(true);
//        for(size_t i = 0; i < m_oaMessages.size(); ++i)
//        {
//            if(m_oaMessages[i].m_nId == nId)
//            {
//                bAdd = false;
//                break;
//            }
//        }
//
//        if(bAdd && !sMsg.IsEmpty() && nId != wxNOT_FOUND)
//            m_oaMessages.Add(new wxGISTaskMessage(nId, sMsg, nType, dt));
//
//        pMsg = pMsg->GetNext();
//    }*//*
//
//    wxXmlNode* pMsg = pMessages->GetChildren();
//    while(pMsg)
//    {
//        wxGISEnumMessageType nType = (wxGISEnumMessageType)GetDecimalValue(pMsg, wxT("type"), (int)enumGISMessageUnk);
//        wxDateTime dt = GetDateValue(pMsg, wxT("dt"), wxDateTime::Now());
//        wxString sMsg = pMsg->GetAttribute(wxT("text"));
//        int nId = GetDecimalValue(pMsg, wxT("id"), wxNOT_FOUND);
//        m_oaMessages.Add(new wxGISTaskMessage(nId, sMsg, nType, dt));
//
//        pMsg = pMsg->GetNext();
//    }
//}

//------------------------------------------------------------------
// wxGISTaskEdit
//------------------------------------------------------------------
IMPLEMENT_CLASS(wxGISTaskEdit, wxGISTask)

wxGISTaskEdit::wxGISTaskEdit()
{
    m_nGroupId = wxNOT_FOUND;
}

wxGISTaskEdit::~wxGISTaskEdit()
{
}

void wxGISTaskEdit::SetName(const wxString& sName)
{
    m_sName = sName;
}

void wxGISTaskEdit::SetDescription(const wxString& sDescription)
{
    m_sDescription = sDescription;
}

void wxGISTaskEdit::SetExecutable(const wxString& sExecutable)
{
    m_sExecPath = sExecutable;
}

//------------------------------------------------------------------
// wxGISTaskCategory
//------------------------------------------------------------------
IMPLEMENT_CLASS(wxGISTaskCategory, wxGISTaskBase)

wxGISTaskCategory::wxGISTaskCategory(const wxString &sName, wxGISTaskManager* pTaskManager) : wxGISTaskBase(NULL)
{
    m_sName = sName;
    m_pTaskManager = pTaskManager;
}

wxGISTaskCategory::~wxGISTaskCategory()
{
}

void wxGISTaskCategory::ProcessNetMessage(const wxNetMessage &msg)
{
    //TODO: check if this the subtask created
    wxJSONValue val = msg.GetValue();
    if (val.HasMember(wxT("task")))
    {
        int nTaskId = val[wxT("task")].Get(wxT("id"), wxJSONValue(wxNOT_FOUND)).AsInt();
        if (m_omSubTasks[nTaskId])
        {
            m_omSubTasks[nTaskId]->NetMessage(msg.GetCommand(), msg.GetState(), msg.GetValue()[wxT("task")]);
            return;
        }
    }

    switch(msg.GetCommand())
    {
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

void wxGISTaskCategory::SetMaxTaskExec(short nMaxExecTaskCount)
{
    m_nMaxTasks = nMaxExecTaskCount;
}

bool wxGISTaskCategory::CreateTask(wxGISTask* const pTask)
{
    wxCHECK_MSG(pTask, false, wxT("Input task pointer is null"));
    wxJSONValue TaskConfig = pTask->GetConfig();

    wxNetMessage msg(enumGISNetCmdCmd, enumGISCmdStAdd, enumGISPriorityHighest);
    wxJSONValue val;
    val[wxT("cat")] = m_sName;
    val[wxT("task")] = TaskConfig;
    msg.SetValue(val);
    wxNetMessage msg_res = m_pTaskManager->SendNetMessageSync(msg);
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

void wxGISTaskCategory::NetNote(const wxNetMessage &msg)
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


void wxGISTaskCategory::NetCommand(const wxNetMessage &msg)
{
    switch(msg.GetState())
    {    
    case enumGISCmdChildren:
        if(msg.GetValue().HasMember(wxT("tasks")))
        {
            wxJSONValue val = msg.GetValue()[wxT("tasks")];
            for(size_t i = 0; i < val.Size(); ++i)
            {
                if(m_omSubTasks[val[i][wxT("id")].AsInt()] == NULL)
                {
                    wxGISTask* pTask = new wxGISTask(this, val[i]);
                    wxASSERT(m_omSubTasks[pTask->GetId()] == NULL);
                    m_omSubTasks[pTask->GetId()] = pTask;

                    if (pTask->GetChildrenCount() > 0)
                    {
                        //request subtasks asynchronously
                        wxJSONValue outval;
                        outval[wxT("id")] = pTask->GetId();
                        SendNetMessageAsync(enumGISNetCmdCmd, enumGISCmdChildren, outval);
                    }

                    AddEvent(wxGISTaskEvent(pTask->GetId(), wxGISTASK_ADD));
                }
            }
        }
        break;
    case enumGISCmdDetails:
        break;
    case enumGISCmdSetParam:
        break;
    case enumGISCmdStAdd:
        if(msg.GetValue().HasMember(wxT("task")))
        {
            wxJSONValue val = msg.GetValue()[wxT("task")];
            if(m_omSubTasks[val[wxT("id")].AsInt()] == NULL)
            {
                wxGISTask* pTask = new wxGISTask(this, val);
                wxASSERT(m_omSubTasks[pTask->GetId()] == NULL);
                m_omSubTasks[pTask->GetId()] = pTask;

                if (pTask->GetChildrenCount() > 0)
                {
                    //request subtasks asynchronously
                    wxJSONValue outval;
                    outval[wxT("id")] = pTask->GetId();
                    SendNetMessageAsync(enumGISNetCmdCmd, enumGISCmdChildren, outval);
                }

                AddEvent(wxGISTaskEvent(pTask->GetId(), wxGISTASK_ADD));
            }
        }
        break;
    case enumGISCmdStDel:
        m_pTaskManager->DeleteCategory(this);
        break;
    default:
        break;
    }
}

void wxGISTaskCategory::SendNetMessageAsync(wxGISNetCommand eCmd, wxGISNetCommandState eCmdState, const wxJSONValue &val)
{
    wxJSONValue parent_val;
    parent_val[wxT("cat")] = m_sName;
    parent_val[wxT("task")] = val;
    wxNetMessage msg(eCmd, eCmdState);
    msg.SetValue(parent_val);
    return m_pTaskManager->SendNetMessageAsync(msg);
}

wxGISNetCommandState wxGISTaskCategory::SendNetMessageSync(wxGISNetCommand eCmd, wxGISNetCommandState eCmdState, const wxJSONValue &val)
{
    wxJSONValue parent_val;
    parent_val[wxT("cat")] = m_sName;
    parent_val[wxT("task")] = val;
    wxNetMessage msg(eCmd, eCmdState);
    msg.SetValue(parent_val);
    m_sLastError.Clear();
    wxNetMessage msg_out = m_pTaskManager->SendNetMessageSync(msg);
    if(msg_out.GetState() == enumGISNetCmdStErr)
    {
        m_sLastError = msg_out.GetMessage();
    }
    return msg_out.GetState();
}
