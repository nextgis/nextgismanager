/******************************************************************************
 * Project:  wxGIS (Task Manager)
 * Purpose:  Task and TaskCategoryList classes.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2012-2013 Bishop
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

#include "wxgis/tskmngr_app/task.h"
#include "wxgis/core/format.h"

#include <wx/dir.h>
#include <wx/wfstream.h>

#define TASK_LOAD_COUNT 15

//------------------------------------------------------------------
// wxGISTaskBase
//------------------------------------------------------------------
IMPLEMENT_ABSTRACT_CLASS(wxGISTaskBase, wxObject)

wxGISTaskBase::wxGISTaskBase(wxGISTaskBase *pParentTask)
{
    m_pParentTask = pParentTask;
    m_nTaskCounter = 0;
}

wxGISTaskBase::~wxGISTaskBase(void)
{
    ClearTasks();
}

int wxGISTaskBase::GetId(void) const
{
    return m_nId;
}

int wxGISTaskBase::GetNewId(void)
{
    return ++m_nTaskCounter;
}

wxString wxGISTaskBase::GetName(void) const
{
    return m_sName;
}

wxString wxGISTaskBase::GetStorePath() const
{
    return m_sStoragePath;
}

void wxGISTaskBase::ClearTasks(void)
{
    for(wxGISTaskMap::iterator it = m_omSubTasks.begin(); it != m_omSubTasks.end(); ++it)
    {
        wxDELETE(it->second);
    }
    m_omSubTasks.clear();
}

void wxGISTaskBase::Delete(wxGISTaskBase *pTask)
{
    wxCHECK_RET(pTask, wxT("Input task pointer is null"));
    wxDELETE(m_omSubTasks[pTask->GetId()]);
}

bool wxGISTaskBase::Save(void)
{
    for(wxGISTaskMap::iterator it = m_omSubTasks.begin(); it != m_omSubTasks.end(); ++it)
    {
        if(NULL != it->second) 
            return false;
        if (!it->second->Save())
            return false;
    }

    wxJSONWriter writer( wxJSONWRITER_STYLED | wxJSONWRITER_WRITE_COMMENTS );  
    writer.SetDoubleFmtString("%.10f");
    wxString  sJSONText;

    writer.Write( GetStoreConfig(), sJSONText );
    wxFile oStorageFile(m_sStoragePath, wxFile::write);
    if(!oStorageFile.Write(sJSONText))
    {
        oStorageFile.Close();
        return false;
    }
    
    return oStorageFile.Close();

}

wxString wxGISTaskBase::GetNewStorePath(const wxString &sAddToName, const wxString &sSubDir)
{
    if(NULL != m_pParentTask)
        return m_pParentTask->GetNewStorePath(sAddToName, sSubDir);
    return wxEmptyString;
}

void wxGISTaskBase::StartNextQueredTask(void)
{
    if(NULL != m_pParentTask)
        m_pParentTask->StartNextQueredTask();
}

void wxGISTaskBase::OnDestroy(void)
{
    m_pParentTask = NULL;
    for(wxGISTaskMap::iterator it = m_omSubTasks.begin(); it != m_omSubTasks.end(); ++it)
    {
        if (NULL != it->second)
        {
            it->second->OnDestroy();
        }        
    }
}

//------------------------------------------------------------------------------
// wxGISTask
//------------------------------------------------------------------------------

IMPLEMENT_CLASS2(wxGISTask, wxGISTaskBase, wxGISProcess)

wxGISTask::wxGISTask(wxGISTaskBase* pParentTask, const wxString &sPath) : wxGISTaskBase(pParentTask), wxGISProcess()
{
    m_nId = pParentTask->GetNewId();
    m_nGroupId = wxNOT_FOUND;
    m_sStoragePath = sPath;
}

wxGISTask::~wxGISTask(void)
{
    Stop();
}

int wxGISTask::GetGroupId(void) const
{
    return m_nGroupId;
}

bool wxGISTask::Load(void)
{
    if(!wxFileName::FileExists(m_sStoragePath))
    {
        return false;
    }

    wxFileInputStream StorageInputStream(m_sStoragePath);
    wxJSONReader oStorageReader;
    wxJSONValue  oStorageRoot;
    //try to load connections storage file
    int numErrorsStorage = oStorageReader.Parse( StorageInputStream, &oStorageRoot );
    if ( numErrorsStorage > 0 )  {
        wxLogError("The JSON Storage document is not well-formed");
        return false;
    }

    m_sName = oStorageRoot.Get(wxT("name"), wxJSONValue(NONAME)).AsString();
    m_sDescription = oStorageRoot.Get(wxT("desc"), wxJSONValue(NONAME)).AsString();
    m_nState = (wxGISEnumTaskStateType)oStorageRoot.Get(wxT("state"), wxJSONValue(enumGISTaskUnk)).AsLong();
    if(m_nState == enumGISTaskWork)
        m_nState = enumGISTaskQuered;
    m_nGroupId = oStorageRoot.Get(wxT("groupid"), wxJSONValue(m_nGroupId)).AsInt();
    m_nPriority = oStorageRoot.Get(wxT("prio"), wxJSONValue(wxNOT_FOUND)).AsLong();
    m_dtCreated = GetDateValue(oStorageRoot, wxT("create"), wxDateTime::Now());

    m_nVolume = oStorageRoot.Get(wxT("vol"), wxJSONValue(wxUint64(0))).AsUInt64();
    m_dfDone = oStorageRoot.Get(wxT("done"), wxJSONValue(0.0)).AsDouble();

    m_Params = oStorageRoot[wxT("params")];
    m_sExecPath = oStorageRoot.Get(wxT("exec"), wxJSONValue(wxEmptyString)).AsString();

    if(oStorageRoot.HasMember(wxT("subtasks")))
    {
        wxJSONValue subtasks = oStorageRoot[wxT("subtasks")];
        for(size_t i = 0; i < subtasks.Size(); ++i)
        {
            wxGISTask* pTask = new wxGISTask(this, subtasks[i].AsString());
            if(pTask->Load())
            {
                m_omSubTasks[pTask->GetId()] = pTask;
            }
            else
            {
                wxDELETE(pTask);   
            }
        }
    }

    if(m_nState == enumGISTaskDone)
        m_dfDone = 100;
    else
        m_dfDone = 0;
    m_dfPrevDone = m_dfDone;

    return true;
}

bool wxGISTask::Create(const wxJSONValue& TaskConfig)
{
    m_sName = TaskConfig.Get(wxT("name"), wxJSONValue(NONAME)).AsString();
    m_sDescription = TaskConfig.Get(wxT("desc"), wxJSONValue(NONAME)).AsString();
    m_nGroupId = TaskConfig.Get(wxT("groupid"), wxJSONValue(m_nGroupId)).AsInt();
    m_nState = (wxGISEnumTaskStateType)TaskConfig.Get(wxT("state"), wxJSONValue(enumGISTaskUnk)).AsLong();
    m_nPriority = m_nId;//TaskConfig.Get(wxT("prio"), wxJSONValue(m_nId)).AsLong();
    m_dtCreated = wxDateTime::Now();

    m_nVolume = 0;
    m_dfDone = 0;

    m_Params = TaskConfig[wxT("params")];
    m_sExecPath = TaskConfig.Get(wxT("exec"), wxJSONValue(wxEmptyString)).AsString();

    if(TaskConfig.HasMember(wxT("subtasks")))
    {
        wxJSONValue subtasks = TaskConfig[wxT("subtasks")];
        for(size_t i = 0; i < subtasks.Size(); ++i)
        {
            wxGISTask* pTask = new wxGISTask(this, GetNewStorePath(wxString::Format(wxT("%d"), wxNewId()), SUBTSKDIR));
            if(pTask->Create( subtasks[i] ))
            {
                m_omSubTasks[pTask->GetId()] = pTask;
            }
            else
            {
                wxDELETE(pTask);
                return false;
            }
        }
    }
    return Save();
}


wxJSONValue wxGISTask::GetStoreConfig(void)
{
    wxJSONValue val;
    val[wxT("name")] = m_sName;
    val[wxT("desc")] = m_sDescription;
    val[wxT("groupid")] = m_nGroupId;
    val[wxT("vol")] = wxUint64(m_nVolume.GetValue());
    val[wxT("done")] = m_dfDone;
    val[wxT("exec")] = m_sExecPath;
    val[wxT("state")] = m_nState;
    val[wxT("prio")] = m_nPriority;
    val[wxT("create")] = SetDateValue(m_dtCreated);
    val[wxT("params")] = m_Params;

    int nCounter(0);
    for(wxGISTaskMap::iterator it = m_omSubTasks.begin(); it != m_omSubTasks.end(); ++it)
    {
        val[wxT("subtasks")][nCounter++] = it->second->GetStorePath();
    }

    return val;
}

wxJSONValue wxGISTask::GetAsJSON(void)
{
    wxJSONValue val;
    val[wxT("id")] = m_nId;
    val[wxT("name")] = m_sName;
    val[wxT("desc")] = m_sDescription;
    val[wxT("groupid")] = m_nGroupId;
    val[wxT("exec")] = m_sExecPath;
    val[wxT("state")] = m_nState;
    val[wxT("prio")] = m_nPriority;
    val[wxT("create")] = SetDateValue(m_dtCreated);
    val[wxT("vol")] = wxUint64(m_nVolume.GetValue());
    val[wxT("done")] = m_dfDone;

    val[wxT("beg")] = SetDateValue(m_dtBeg);
    val[wxT("end")] = SetDateValue(m_dtEstEnd);

    val[wxT("params")] = m_Params;

    return val;
}

bool wxGISTask::StartTask(long nMessageId, int nUserId)
{
    m_nState = enumGISTaskQuered;
 
    if(m_pParentTask)
    {
        wxJSONValue idval;
        idval[wxT("id")] = m_nId;
        m_pParentTask->SendNetMessage(enumGISNetCmdCmd, enumGISCmdStStart, enumGISPriorityHighest, idval, _("Start succeeded"), nMessageId, nUserId);
    }

    //start 
    OnStart();
    return true;
}

bool wxGISTask::StopTask(long nMessageId, int nUserId)
{
    m_nState = enumGISTaskPaused;
    if(m_pParentTask)
    {
        wxJSONValue idval;
        idval[wxT("id")] = m_nId;
        m_pParentTask->SendNetMessage(enumGISNetCmdCmd, enumGISCmdStStop, enumGISPriorityHighest, idval, _("Stop succeeded"), nMessageId, nUserId);
    }

    //stop
    OnStop();    
    return true;
}


bool wxGISTask::Delete(long nMessageId, int nUserId)
{
    Stop();

    if(m_omSubTasks.empty())
    {
        StartNextQueredTask();
    }

    for(wxGISTaskMap::iterator it = m_omSubTasks.begin(); it != m_omSubTasks.end(); ++it)
    {
        wxGISTask* pTask = dynamic_cast<wxGISTask*>(it->second);
        if(pTask)
        {
            if(!pTask->Delete(-2, -2))
                return false;
        }
    }

    //delete config
    if(wxRemoveFile(m_sStoragePath) && m_pParentTask)
    {
        //don't send subtask delete message
        if(nMessageId != -2 && nUserId != -2)
        {
            wxJSONValue idval;
            idval[wxT("id")] = m_nId;
            m_pParentTask->SendNetMessage(enumGISNetCmdCmd, enumGISCmdStDel, enumGISPriorityHighest, idval, _("Delete succeeded"), nMessageId, nUserId);
        }
        //try to delete temp file
        wxFileName fName(m_sStoragePath);
        fName.AppendDir( wxT("tmp") );
        wxRemoveFile(fName.GetFullPath());

        m_pParentTask->Delete(this);
        return true;
    }
    return false;
}

void wxGISTask::GetChildren(long nMessageId, int nUserId)
{
    if(m_omSubTasks.empty())
        return;

    wxVector<wxJSONValue> data;
    int nCounter(0);
    for(wxGISTaskMap::iterator it = m_omSubTasks.begin(); it != m_omSubTasks.end(); ++it)
    {
        wxGISTask* pTask = dynamic_cast<wxGISTask*>(it->second);
        if(pTask)
        {
            data.push_back(pTask->GetAsJSON());
            nCounter++;
            if(nCounter > TASK_LOAD_COUNT)
            {
                 wxJSONValue out_val;
                 out_val[wxT("id")] = m_nId;
                 for(size_t i = 0; i < nCounter; ++i)
                 {
                     out_val[wxT("subtasks")][i] = data[i];
                 }
                 data.clear();
                 nCounter = 0;
                 if(m_pParentTask)
                    m_pParentTask->SendNetMessage(enumGISNetCmdCmd, enumGISCmdChildren, enumGISPriorityHigh, out_val, _("Children get succeeded"), nMessageId, nUserId);
            }
        }
    }

    wxJSONValue out_val;
    out_val[wxT("id")] = m_nId;
    for(size_t i = 0; i < nCounter; ++i)
    {
        out_val[wxT("subtasks")][i] = data[i];
    }
    if(m_pParentTask)
        m_pParentTask->SendNetMessage(enumGISNetCmdCmd, enumGISCmdChildren, enumGISPriorityHigh, out_val, _("Children get succeeded"), nMessageId, nUserId);
}

void wxGISTask::SendNetMessage(wxGISNetCommand eCmd, wxGISNetCommandState eCmdState, wxGISMessagePriority ePrio, const wxJSONValue &val, const wxString &sMsg, long nMessageId, int nUserId)
{
    if(!m_pParentTask)
        return;

    wxJSONValue parent_val;
    parent_val[wxT("id")] = m_nId;
    parent_val[wxT("subtask")] = val;
    return m_pParentTask->SendNetMessage(eCmd, eCmdState, ePrio, parent_val, sMsg, nMessageId, nUserId);
}

void wxGISTask::NetMessage(wxGISNetCommand eCmd, wxGISNetCommandState eCmdState, const wxJSONValue &val, long nMessageId, int nUserId)
{
    if(val.HasMember(wxT("subtask")) && eCmdState != enumGISCmdStAdd)
    {
        int nId = val[wxT("subtask")][wxT("id")].AsInt();
        if(m_omSubTasks[nId])
            m_omSubTasks[nId]->NetMessage(eCmd, eCmdState, val[wxT("subtask")], nMessageId, nUserId);
    }
    else
    {
        switch(eCmd)
        {
        case enumGISNetCmdCmd:
            NetCommand(eCmdState, val, nMessageId, nUserId);
            break;
        case enumGISNetCmdNote:
            wxASSERT_MSG(0, wxT("Incorrect case"));
            break;
        default:
            break;
        }
    }
}

void wxGISTask::NetCommand(wxGISNetCommandState eCmdState, const wxJSONValue &val, long nMessageId, int nUserId)
{
    switch(eCmdState)
    {
        case enumGISCmdStAdd: //TODO: Add new subtask to current task
            //if(!AddTask(val[wxT("task")], nMessageId, nUserId))
            //{
            //    wxNetMessage msgout(enumGISNetCmdNote, enumGISNetCmdStErr, enumGISPriorityLow, nMessageId);
            //    msgout.SetMessage(m_sLastError);
            //    wxJSONValue val = msgout.GetValue();
            //    val[wxT("cat")] = m_sName;
            //    msgout.SetValue(val);
            //    m_pTaskManager->SendNetMessage(msgout, nUserId);
            //}
            break;
        case enumGISCmdStDel:
            if(!Delete(nMessageId, nUserId) && m_pParentTask)
            {                
                wxJSONValue idval;
                idval[wxT("id")] = m_nId;
                m_pParentTask->SendNetMessage(enumGISNetCmdNote, enumGISNetCmdStErr, enumGISPriorityLow, idval, _("Delete failed"), nMessageId, nUserId);
            }
            break;       
        case enumGISCmdStChng://TODO: Change subtask by TaskId. Return result of operation
//            if(!ChangeTask(msg.GetXMLRoot()->GetChildren(), msg.GetId(), sErr))
//            {
//                wxNetMessage msgout(enumGISNetCmdNote, enumGISNetCmdStErr, enumGISPriorityLow, msg.GetId());
//                msgout.SetMessage(sErr);
//                SendNetMessage(msgout, event.GetId());                
//            }
            break; 
        case enumGISCmdStStart:
            if(!StartTask(nMessageId, nUserId) && m_pParentTask)
            {                
                wxJSONValue idval;
                idval[wxT("id")] = m_nId;
                m_pParentTask->SendNetMessage(enumGISNetCmdNote, enumGISNetCmdStErr, enumGISPriorityLow, idval, _("Start failed"), nMessageId, nUserId);
            }
            break; 
        case enumGISCmdStStop:
            if(!StopTask(nMessageId, nUserId) && m_pParentTask)
            {                
                wxJSONValue idval;
                idval[wxT("id")] = m_nId;
                m_pParentTask->SendNetMessage(enumGISNetCmdNote, enumGISNetCmdStErr, enumGISPriorityLow, idval, _("Stop failed"), nMessageId, nUserId);
            }
            break; 
///*            case enumGISCmdStPriority: TODO:
//            if(!ChangeTaskPriority(msg.GetXMLRoot()->GetChildren(), msg.GetId(), sErr))
//            {
//                wxNetMessage msgout(enumGISNetCmdNote, enumGISNetCmdStErr, enumGISPriorityLow, msg.GetId());
//                msgout.SetMessage(sErr);
//                SendNetMessage(msgout, event.GetId());                
//            }
//            break; */         
        case enumGISCmdChildren://Get subtasks lists
            GetChildren(nMessageId, nUserId);
            break; 
        case enumGISCmdDetails://Get subtask details
            //{
            //    int nId = msg.GetValue()[wxT("task")].Get(wxT("id"), wxJSONValue(wxNOT_FOUND)).AsInt();
            //    if(nId == wxNOT_FOUND || m_omSubTasks[nId] == NULL)
            //    {
            //        wxNetMessage msgout(enumGISNetCmdNote, enumGISNetCmdStErr, enumGISPriorityLow, msg.GetId());
            //        msgout.SetMessage(_("Invalid task id"));
            //        wxJSONValue val = msgout.GetValue();
            //        val[wxT("cat")] = m_sName;
            //        msgout.SetValue(val);
            //        m_pTaskManager->SendNetMessage(msgout, nUserId);
            //    }
            //    else
            //    {
            //        wxNetMessage msgout(enumGISNetCmdCmd, enumGISCmdGetChildren, enumGISPriorityHigh, msg.GetId());
            //    
            //        wxJSONValue val;
            //        val[wxT("cat")] = m_sName;
            //        val[wxT("task")] = m_omSubTasks[nId]->GetConfig();
            //        msgout.SetValue(val);
            //        m_pTaskManager->SendNetMessage(msgout, nUserId);
            //    }
            //}
            break;             
        default:            
            break;
    }
}

void wxGISTask::OnStart(void)
{
    m_nState = enumGISTaskQuered;

    if(m_omSubTasks.empty())
    {
        //change task and send message
        ChangeTask();

        StartNextQueredTask();

        Save();
    }
    else
    {
        for(wxGISTaskMap::iterator it = m_omSubTasks.begin(); it != m_omSubTasks.end(); ++it)
        {
            wxGISTask* pSubTask = dynamic_cast<wxGISTask*>(it->second);
            if(pSubTask)
            {
                pSubTask->OnStart();
            }
        }
    }
}

void wxGISTask::OnStop(void)
{
    
    if(m_omSubTasks.empty())
    {
        if(m_nState == enumGISTaskWork)
        {
            Stop();
            StartNextQueredTask();
        }
        m_nState = enumGISTaskPaused;

        //change task and send message
        ChangeTask();

        StartNextQueredTask();

        Save();
    }
    else
    {
        m_nState = enumGISTaskPaused;

        for(wxGISTaskMap::iterator it = m_omSubTasks.begin(); it != m_omSubTasks.end(); ++it)
        {
            wxGISTask* pSubTask = dynamic_cast<wxGISTask*>(it->second);
            if(pSubTask)
            {
                pSubTask->OnStop();
            }
        }
    }
}

void wxGISTask::OnTerminate(int pid, int status)
{
    wxGISProcess::OnTerminate(pid, status);
    m_dfPrevDone = 0;
    StartNextQueredTask();
    Save();
    ChangeTask();
}

bool wxGISTask::Start(void)
{
    m_dfPrevDone = 0;
    bool bReturn = wxGISProcess::Start();
    if(bReturn)
    {
        ChangeTask();
    }
    return bReturn;
}

void wxGISTask::UpdatePercent(const wxString &sPercentData)
{
    wxGISProcess::UpdatePercent(sPercentData);
    if(m_dfDone - m_dfPrevDone > 0.1)
    {
        m_dfPrevDone = m_dfDone;
        ChangeTask();
    }
}

void wxGISTask::ChangeTask()
{
    if(!m_pParentTask)
        return;
    wxJSONValue val;
    val[wxT("id")] = m_nId;
    val[wxT("state")] = m_nState;
    val[wxT("done")] = m_dfDone;
    val[wxT("vol")] = wxUint64(m_nVolume.GetValue());
    val[wxT("beg")] = SetDateValue(m_dtBeg);
    val[wxT("end")] = SetDateValue(m_dtEstEnd);
    m_pParentTask->SendNetMessage(enumGISNetCmdCmd, enumGISCmdStChng, enumGISPriorityHighest, val, _("Task changed"), wxNOT_FOUND);
}

void wxGISTask::ChangeTaskMsg(wxGISEnumMessageType nType, const wxString &sInfoData)
{
    if(!m_pParentTask || sInfoData.IsEmpty())
        return;

    wxJSONValue val;
    val[wxT("id")] = m_nId;
    val[wxT("msg")] = sInfoData;
    val[wxT("date")] = SetDateValue(wxDateTime::Now());
    val[wxT("type")] = nType;
    val[wxT("msg_id")] = wxNewId();

    m_pParentTask->SendNetMessage(enumGISNetCmdNote, enumGISCmdNoteMsg, enumGISPriorityLow, val, _("Task new message"), wxNOT_FOUND);
}


bool wxGISTask::IsGroupIdExecuting(int nGroupId) const
{
    if(nGroupId == wxNOT_FOUND)
        return false;
    if(m_omSubTasks.empty())
    {
        return m_nState == enumGISTaskWork && nGroupId == m_nGroupId; 
    }
    else
    {
        for(wxGISTaskMap::const_iterator it = m_omSubTasks.begin(); it != m_omSubTasks.end(); ++it)
        {
            wxGISTask* pSubTask = dynamic_cast<wxGISTask*>(it->second);
            if(pSubTask && pSubTask->IsGroupIdExecuting(nGroupId))
            {
                return true;
            }
        }
    }
    return false;
}

long wxGISTask::Execute(void)
{
    wxString sCmd = m_sExecPath + wxT(" --execonf \"") + m_sStoragePath + wxT("\"");
    if(!m_Params.IsNull())
    {
        const wxJSONInternalMap* map = m_Params.AsMap();
        wxJSONInternalMap::const_iterator it;
        for ( it = map->begin(); it != map->end(); ++it )
        {
            wxString key = it->first;
            const wxJSONValue& Param = it->second;
            wxString sParam;

            if(Param.IsArray())
            {
                //wxJSONWriter writer( wxJSONWRITER_NONE );
                //writer.SetDoubleFmtString("%.10f");
                //writer.Write( Param, sParam );
                continue;
            }
            else
            {
                sParam = Param.AsString();
            }

            sCmd += wxT(" --") + key;
            if(!Param.IsBool())
            {
                if(sParam.Find(wxT(" ")) == wxNOT_FOUND)
                    sCmd += wxT(" ") + sParam;
                else
                    sCmd += wxT(" \"") + sParam + wxT("\"");
            }
        }
    }
    return wxExecute(sCmd, wxEXEC_ASYNC, this);
}

void wxGISTask::AddInfo(wxGISEnumMessageType nType, const wxString &sInfoData)
{
    if(m_pParentTask == NULL || sInfoData.IsEmpty())
        return;
    wxString sRest;
    if(sInfoData.StartsWith(wxT("VOL: "), &sRest))
    {
        unsigned long long nVol;
        sRest.ToULongLong(&nVol);
        m_nVolume = nVol;
        ChangeTask();
    }
    else
    {
        //MESSAGE msg = {sInfoData, wxDateTime::Now(), nType};
        //m_staMessages.push_back(msg);

        //don't accum, just send to listeners
        
        ChangeTaskMsg(nType, sInfoData);
    }
}

void wxGISTask::GetQueredTasks(wxGISQueredTasksArray &oaTasks)
{
    if(m_omSubTasks.empty())
    {
         if(m_nState == enumGISTaskQuered)
            oaTasks.Add(this);
    }
    else
    {
        for(wxGISTaskMap::const_iterator it = m_omSubTasks.begin(); it != m_omSubTasks.end(); ++it)
        {
            wxGISTask* pSubTask = dynamic_cast<wxGISTask*>(it->second);
            if(pSubTask)
            {
                pSubTask->GetQueredTasks(oaTasks);
            }
        }
    }
}


int wxGISTask::GetRunningTaskCount(void) const
{
    if(m_omSubTasks.empty())
    {
         if(m_nState == enumGISTaskWork)
            return 1;
    }
    else
    {
        int nRunningTaskCount = 0;
        for(wxGISTaskMap::const_iterator it = m_omSubTasks.begin(); it != m_omSubTasks.end(); ++it)
        {
            wxGISTask* pSubTask = dynamic_cast<wxGISTask*>(it->second);
            if(pSubTask)
            {
                nRunningTaskCount += pSubTask->GetRunningTaskCount();
            }
        }
        return nRunningTaskCount;
    }
    return 0;
}

long wxGISTask::GetCommonPriority()
{
    long nParentPriority = 0;
    wxGISTask* pTask = dynamic_cast<wxGISTask*>(m_pParentTask);
    if(pTask)
        nParentPriority = pTask->GetCommonPriority();
    return m_nPriority + nParentPriority * 100;//only 99 subtasks may be present 
}

void wxGISTask::OnDestroy(void)
{
    Stop();
    wxGISTaskBase::OnDestroy();
}

/*
void wxGISTask::ChangeTask(const wxXmlNode* pTaskNode)
{
    Stop();
    //load
    m_sName = pTaskNode->GetAttribute(wxT("name"), m_sName);
    m_sDescription = pTaskNode->GetAttribute(wxT("desc"), m_sDescription);
    m_nState = (wxGISEnumTaskStateType)GetDecimalValue(pTaskNode, wxT("state"), m_nState);
    if(m_nState == enumGISTaskWork)
        m_nState = enumGISTaskQuered;
    m_nPriority = GetDecimalValue(pTaskNode, wxT("prio"), m_nPriority);
    wxRemoveFile(m_sExeConfPath);
    if(pTaskNode->GetChildren())
        m_pParams = new wxXmlNode(*pTaskNode->GetChildren());
    else
        m_pParams = NULL;
    m_dfDone = 0;
}

void wxGISTask::SetPriority(long nNewPriority)
{
    m_nPriority = nNewPriority;
}
*/


//------------------------------------------------------------------------------
// wxGISTaskCategory
//------------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGISTaskCategory, wxGISTaskBase)

wxGISTaskCategory::wxGISTaskCategory(const wxString &sPath, wxGISTaskManager* pTaskManager) : wxGISTaskBase(NULL)
{
    m_pTaskManager = pTaskManager;
    m_sStoragePath = sPath;
    m_nMaxTasks = pTaskManager->GetMaxExecTaskCount();

    wxFileName oFName(sPath);
    m_sName = oFName.GetName();
    m_nId = wxNOT_FOUND;
}

wxGISTaskCategory::~wxGISTaskCategory(void)
{
    Save();
}

bool wxGISTaskCategory::Load(void)
{
    wxDir dir(m_sStoragePath); 
    if( dir.IsOpened() )
    {
        wxString sTaskName;
        
        bool bContinue = dir.GetFirst(&sTaskName, wxT("*.json"), wxDIR_FILES);
        while ( bContinue )
        {
            wxGISTaskBase* pGISTask = new wxGISTask(this, m_sStoragePath + wxFileName::GetPathSeparator() + sTaskName);
            if(pGISTask->Load())
            {
                m_omSubTasks[pGISTask->GetId()] = pGISTask;
            }
            else
            {
                wxDELETE(pGISTask);   
            }
            bContinue = dir.GetNext(&sTaskName);
        }
    }
    StartNextQueredTask();
    return true;
}

void wxGISTaskCategory::SetMaxExecTaskCount(int nMaxExecTasks)
{
    m_nMaxTasks = nMaxExecTasks;
}

int wxGISTaskCategory::GetRunningTaskCount(void) const
{
    int nRunningTaskCount = 0;
    for(wxGISTaskMap::const_iterator it = m_omSubTasks.begin(); it != m_omSubTasks.end(); ++it)
    {
        wxGISTask* pSubTask = dynamic_cast<wxGISTask*>(it->second);
        if(pSubTask)
        {
            nRunningTaskCount += pSubTask->GetRunningTaskCount();
        }
    }
    return nRunningTaskCount;
}

void wxGISTaskCategory::NetMessage(wxGISNetCommand eCmd, wxGISNetCommandState eCmdState, const wxJSONValue &val, long nMessageId, int nUserId)
{
    if(val.HasMember(wxT("task")) && eCmdState != enumGISCmdStAdd)
    {
        int nId = val[wxT("task")][wxT("id")].AsInt();
        if(m_omSubTasks[nId])
            m_omSubTasks[nId]->NetMessage(eCmd, eCmdState, val[wxT("task")], nMessageId, nUserId);
    }
    else
    {
        switch(eCmd)
        {
        case enumGISNetCmdCmd:
            NetCommand(eCmdState, val, nMessageId, nUserId);
            break;
        case enumGISNetCmdNote:
            wxASSERT_MSG(0, wxT("Incorrect case"));
            break;
        default:
            break;
        }
    }
}

void wxGISTaskCategory::NetCommand(wxGISNetCommandState eCmdState, const wxJSONValue &val, long nMessageId, int nUserId)
{
    switch(eCmdState)
    {
        case enumGISCmdStAdd: //Add new task to specified category. Return result of operation
            if(!AddTask(val[wxT("task")], nMessageId, nUserId))
            {
                wxNetMessage msgout(enumGISNetCmdNote, enumGISNetCmdStErr, enumGISPriorityLow, nMessageId);
                msgout.SetMessage(m_sLastError);
                wxJSONValue val = msgout.GetValue();
                val[wxT("cat")] = m_sName;
                msgout.SetValue(val);
                m_pTaskManager->SendNetMessage(msgout, nUserId);
            }
            break;
        case enumGISCmdStDel: //Delete category
            wxFAIL_MSG(wxT("Unsuported operation. Category deletion is not supported yet."));
            break;       
        case enumGISCmdStChng://Change task by TaskId. Return result of operation
//            if(!ChangeTask(msg.GetXMLRoot()->GetChildren(), msg.GetId(), sErr))
//            {
//                wxNetMessage msgout(enumGISNetCmdNote, enumGISNetCmdStErr, enumGISPriorityLow, msg.GetId());
//                msgout.SetMessage(sErr);
//                SendNetMessage(msgout, event.GetId());                
//            }
            break; 
//        case enumGISCmdStStart:
//            if(!StartTask(msg.GetXMLRoot()->GetChildren(), msg.GetId(), sErr))
//            {
//                wxNetMessage msgout(enumGISNetCmdNote, enumGISNetCmdStErr, enumGISPriorityLow, msg.GetId());
//                msgout.SetMessage(sErr);
//                SendNetMessage(msgout, event.GetId());                
//            }
//            break; 
//        case enumGISCmdStStop:
//            if(!StopTask(msg.GetXMLRoot()->GetChildren(), msg.GetId(), sErr))
//            {
//                wxNetMessage msgout(enumGISNetCmdNote, enumGISNetCmdStErr, enumGISPriorityLow, msg.GetId());
//                msgout.SetMessage(sErr);
//                SendNetMessage(msgout, event.GetId());                
//            }
//            break;             
///*            case enumGISCmdStPriority: TODO:
//            if(!ChangeTaskPriority(msg.GetXMLRoot()->GetChildren(), msg.GetId(), sErr))
//            {
//                wxNetMessage msgout(enumGISNetCmdNote, enumGISNetCmdStErr, enumGISPriorityLow, msg.GetId());
//                msgout.SetMessage(sErr);
//                SendNetMessage(msgout, event.GetId());                
//            }
//            break; */         
        case enumGISCmdChildren://Get tasks lists
            GetChildren(nMessageId, nUserId);
            break; 
        case enumGISCmdDetails://Get task details
            //{
            //    int nId = msg.GetValue()[wxT("task")].Get(wxT("id"), wxJSONValue(wxNOT_FOUND)).AsInt();
            //    if(nId == wxNOT_FOUND || m_omSubTasks[nId] == NULL)
            //    {
            //        wxNetMessage msgout(enumGISNetCmdNote, enumGISNetCmdStErr, enumGISPriorityLow, msg.GetId());
            //        msgout.SetMessage(_("Invalid task id"));
            //        wxJSONValue val = msgout.GetValue();
            //        val[wxT("cat")] = m_sName;
            //        msgout.SetValue(val);
            //        m_pTaskManager->SendNetMessage(msgout, nUserId);
            //    }
            //    else
            //    {
            //        wxNetMessage msgout(enumGISNetCmdCmd, enumGISCmdGetChildren, enumGISPriorityHigh, msg.GetId());
            //    
            //        wxJSONValue val;
            //        val[wxT("cat")] = m_sName;
            //        val[wxT("task")] = m_omSubTasks[nId]->GetConfig();
            //        msgout.SetValue(val);
            //        m_pTaskManager->SendNetMessage(msgout, nUserId);
            //    }
            //}
            break;             
        default:            
            break;
    }
}

bool wxGISTaskCategory::Save(void)
{
    bool bRes = true;
    for(wxGISTaskMap::iterator it = m_omSubTasks.begin(); it != m_omSubTasks.end(); ++it)
    {
        if(it->second)
        {
            if(!it->second->Save())
            {
                bRes = false;
                break;
            }
        }
    }
    return bRes;
}

void wxGISTaskCategory::SendNetMessage(wxGISNetCommand eCmd, wxGISNetCommandState eCmdState, wxGISMessagePriority ePrio, const wxJSONValue &val, const wxString &sMsg, long nMessageId, int nUserId)
{
    wxJSONValue parent_val;
    parent_val[wxT("cat")] = m_sName;
    parent_val[wxT("task")] = val;
    wxNetMessage msg(eCmd, eCmdState, ePrio, nMessageId);
    msg.SetValue(parent_val);
    msg.SetMessage(sMsg);
    return m_pTaskManager->SendNetMessage(msg, nUserId);
}

bool wxGISTaskCategory::AddTask(const wxJSONValue &TaskConfig, long nMessageId, int nUserId)
{
    wxGISTask *pTask = new wxGISTask(this, GetNewStorePath(wxString::Format(wxT("%d"), wxNewId())));

    //store in file
    if( pTask->Create(TaskConfig) )
    {
        m_omSubTasks[pTask->GetId()] = pTask;
        SendNetMessage(enumGISNetCmdCmd, enumGISCmdStAdd, enumGISPriorityHigh, pTask->GetAsJSON(), _("Add succeeded"), nMessageId, nUserId);

        StartNextQueredTask();

        return true;
    }
    else
    {
        wxDELETE(pTask);
        m_sLastError = wxString::Format(_("Save changes to file '%s' failed"), pTask->GetStorePath().c_str());
        return false;
    }
}

wxString wxGISTaskCategory::GetNewStorePath(const wxString &sAddToName, const wxString &sSubDir)
{
    return m_pTaskManager->GetNewStorePath(sAddToName, m_sName + wxFileName::GetPathSeparator() + sSubDir);
}

wxJSONValue wxGISTaskCategory::GetStoreConfig(void)
{
    return wxJSONValue();
}

void wxGISTaskCategory::GetChildren(long nMessageId, int nUserId)
{
    if(m_omSubTasks.empty())
        return;

    wxVector<wxJSONValue> data;
    int nCounter(0);
    for(wxGISTaskMap::iterator it = m_omSubTasks.begin(); it != m_omSubTasks.end(); ++it)
    {
        wxGISTask* pTask = dynamic_cast<wxGISTask*>(it->second);
        if(pTask)
        {
            data.push_back(pTask->GetAsJSON());
            nCounter++;
            if(nCounter > TASK_LOAD_COUNT)
            {
                wxJSONValue out_val;
                out_val[wxT("cat")] = m_sName;
                for(size_t i = 0; i < nCounter; ++i)
                {
                    out_val[wxT("tasks")][i] = data[i];
                }
                data.clear();
                nCounter = 0;
                wxNetMessage msgout(enumGISNetCmdCmd, enumGISCmdChildren, enumGISPriorityHigh, nMessageId);
                msgout.SetValue(out_val);
                m_pTaskManager->SendNetMessage(msgout, nUserId);
            }
        }
    }

    wxJSONValue out_val;
    out_val[wxT("cat")] = m_sName;
    for(size_t i = 0; i < nCounter; ++i)
    {
        out_val[wxT("tasks")][i] = data[i];
    }
    wxNetMessage msgout(enumGISNetCmdCmd, enumGISCmdChildren, enumGISPriorityHigh, nMessageId);
    msgout.SetValue(out_val);
    m_pTaskManager->SendNetMessage(msgout, nUserId);
}

void wxGISTaskCategory::StartNextQueredTask()
{
    if(GetRunningTaskCount() >= m_nMaxTasks)
        return;

    for(short i = 0; i < m_nMaxTasks; ++i)
    {
        if(GetRunningTaskCount() >= m_nMaxTasks)
            return;

        wxGISTask* const pTask = GetQueredTask();
        if(pTask)
        {
            pTask->Start();
        }
    }
}

wxGISTask* const wxGISTaskCategory::GetQueredTask()
{
    long nMinPriority = LONG_MAX;
    wxGISTask* pOutTask = NULL;
    wxGISQueredTasksArray oaTasks;
    GetQueredTasks(oaTasks);
    for(size_t i = 0; i < oaTasks.GetCount(); ++i)
    {
        wxGISTask* pTask = oaTasks[i];
        if(pTask && !IsGroupIdExecuting(pTask->GetGroupId()))//check if task from this group is executing
        {
           if(nMinPriority > pTask->GetCommonPriority())//find the lowerest priority - this mean the high priority (closest to 0)
           {
               nMinPriority = pTask->GetCommonPriority();
               pOutTask = pTask;
           }
        }
    }
    return pOutTask;
}

void wxGISTaskCategory::GetQueredTasks(wxGISQueredTasksArray &oaTasks)
{
    for(wxGISTaskMap::const_iterator it = m_omSubTasks.begin(); it != m_omSubTasks.end(); ++it)
    {
        wxGISTask* pSubTask = dynamic_cast<wxGISTask*>(it->second);
        if(pSubTask)
        {
            pSubTask->GetQueredTasks(oaTasks);
        }
    }
}


bool wxGISTaskCategory::IsGroupIdExecuting(int nGroupId) const
{
    if(nGroupId == wxNOT_FOUND)
        return false;
    for(wxGISTaskMap::const_iterator it = m_omSubTasks.begin(); it != m_omSubTasks.end(); ++it)
    {
        wxGISTask* pSubTask = dynamic_cast<wxGISTask*>(it->second);
        if(pSubTask && pSubTask->IsGroupIdExecuting(nGroupId))
        {
            return true;
        }
    }
    return false;
}


/*
bool wxGISTaskCategory::DelTask(int nTaskId, int nId, wxString &sErrMsg)
{
    if(nTaskId == wxNOT_FOUND)
    {
        sErrMsg = wxString::Format(_("The task id %d not found"), nTaskId);
        return false;
    }

    if(m_omTasks[nTaskId])
    {
        if(m_omTasks[nTaskId]->GetState() == enumGISTaskWork)
        {
            m_nRunningTasks--;
        }
        if( m_omTasks[nTaskId]->Delete() )
        {
            m_omTasks.erase(nTaskId);
        }
    }
    //store in xml
    if( Save() )
    {
        wxNetMessage msgout(enumGISNetCmdCmd, enumGISCmdStDel, enumGISPriorityHigh, nId);
        sErrMsg = wxString(_("Delete task succeeded"));
        msgout.SetMessage(sErrMsg);
        wxXmlNode* pRootNode = msgout.GetXMLRoot();
        wxXmlNode* pTaskNode = new wxXmlNode(pRootNode, wxXML_ELEMENT_NODE, wxT("task"));
        pTaskNode->AddAttribute(wxT("cat"), m_sName);
        SetDecimalValue(pTaskNode, wxT("id"), nTaskId);
        pRootNode->AddChild(pTaskNode);

        m_pTaskManager->SendNetMessage(msgout);

        StartNextQueredTask();

        return true;
    }
    else
    {
        sErrMsg = wxString::Format(_("Save chnges to xml file '%s' failed"), m_sXmlDocPath.c_str());
        return false;
    }
}

bool wxGISTaskCategory::StartTask(int nTaskId, int nId, wxString &sErrMsg)
{
    if(nTaskId == wxNOT_FOUND)
    {
        sErrMsg = wxString::Format(_("The task id %d not found"), nTaskId);
        return false;
    }

    if( m_omTasks[nTaskId] )
    {
        m_omTasks[nTaskId]->SetState(enumGISTaskQuered);
    }

    //store in xml
    if( Save() )
    {
        wxNetMessage msgout(enumGISNetCmdCmd, enumGISCmdStStart, enumGISPriorityHigh, nId);
        sErrMsg = wxString(_("Start task succeeded"));
        msgout.SetMessage(sErrMsg);
        wxXmlNode* pRootNode = msgout.GetXMLRoot();
        wxXmlNode* pTaskNode = new wxXmlNode(pRootNode, wxXML_ELEMENT_NODE, wxT("task"));
        pTaskNode->AddAttribute(wxT("cat"), m_sName);
        SetDecimalValue(pTaskNode, wxT("id"), nTaskId);
        SetDecimalValue(pTaskNode, wxT("state"), enumGISTaskQuered);
        pRootNode->AddChild(pTaskNode);

        m_pTaskManager->SendNetMessage(msgout);

        StartNextQueredTask();

        return true;
    }
    else
    {
        sErrMsg = wxString::Format(_("Save chnges to xml file '%s' failed"), m_sXmlDocPath.c_str());
        return false;
    }
}

bool wxGISTaskCategory::StopTask(int nTaskId, int nId, wxString &sErrMsg)
{
    if(nTaskId == wxNOT_FOUND)
    {
        sErrMsg = wxString::Format(_("The task id %d not found"), nTaskId);
        return false;
    }

    if( m_omTasks[nTaskId] )
    {
        m_omTasks[nTaskId]->Stop();
        m_nRunningTasks--;
    }

    //store in xml
    if( Save() )
    {
        wxNetMessage msgout(enumGISNetCmdCmd, enumGISCmdStStop, enumGISPriorityHigh, nId);
        sErrMsg = wxString(_("Stop task succeeded"));
        msgout.SetMessage(sErrMsg);
        wxXmlNode* pRootNode = msgout.GetXMLRoot();
        wxXmlNode* pTaskNode = new wxXmlNode(pRootNode, wxXML_ELEMENT_NODE, wxT("task"));
        pTaskNode->AddAttribute(wxT("cat"), m_sName);
        SetDecimalValue(pTaskNode, wxT("id"), nTaskId);
        SetDecimalValue(pTaskNode, wxT("state"), enumGISTaskPaused);
        pRootNode->AddChild(pTaskNode);

        m_pTaskManager->SendNetMessage(msgout);

        StartNextQueredTask();

        return true;
    }
    else
    {
        sErrMsg = wxString::Format(_("Save changes to xml file '%s' failed"), m_sXmlDocPath.c_str());
        return false;
    }
}

bool wxGISTaskCategory::ChangeTaskPriority(int nTaskId, long nPriority, int nId, wxString &sErrMsg)
{
    if(nTaskId == wxNOT_FOUND)
    {
        sErrMsg = wxString::Format(_("The task id %d not found"), nTaskId);
        return false;
    }
    if(nPriority == wxNOT_FOUND)
    {
        sErrMsg = wxString::Format(_("The task priority %d is undefined"), nPriority);
        return false;
    }

    if( m_omTasks[nTaskId] )
    {
        m_omTasks[nTaskId]->SetPriority(nPriority);
    }

    if( Save() )
    {
        wxNetMessage msgout(enumGISNetCmdCmd, enumGISCmdStPriority, enumGISPriorityHigh, nId);
        sErrMsg = wxString(_("Set task new priority succeeded"));
        msgout.SetMessage(sErrMsg);
        wxXmlNode* pRootNode = msgout.GetXMLRoot();
        wxXmlNode* pTaskNode = new wxXmlNode(pRootNode, wxXML_ELEMENT_NODE, wxT("task"));
        pTaskNode->AddAttribute(wxT("cat"), m_sName);
        SetDecimalValue(pTaskNode, wxT("id"), nTaskId);
        SetDecimalValue(pTaskNode, wxT("prio"), nPriority);
        pRootNode->AddChild(pTaskNode);

        m_pTaskManager->SendNetMessage(msgout);

        return true;
    }
    else
    {
        sErrMsg = wxString::Format(_("Save changes to xml file '%s' failed"), m_sXmlDocPath.c_str());
        return false;
    }

}

bool wxGISTaskCategory::ChangeTask(const wxXmlNode* pTaskNode, int nId, wxString &sErrMsg)
{
    sErrMsg = wxString(_("Input pointer wxXmlNode is null"));
    wxCHECK_MSG(pTaskNode, false, wxT("Input pointer wxXmlNode is null"));
    int nTaskId = GetDecimalValue(pTaskNode, wxT("id"), wxNOT_FOUND);
    if(nTaskId == wxNOT_FOUND)
    {
        sErrMsg = wxString::Format(_("The task id %d not found"), nTaskId);
        return false;
    }
    if(m_omTasks[nTaskId])
        m_omTasks[nTaskId]->ChangeTask(pTaskNode);
   //store in xml
    if( Save() )
    {
        wxNetMessage msgout(enumGISNetCmdCmd, enumGISCmdStChng, enumGISPriorityHigh, nId);
        sErrMsg = wxString(_("Change task succeeded"));
        msgout.SetMessage(sErrMsg);
        wxXmlNode* pRootNode = msgout.GetXMLRoot();
        if(m_omTasks[nTaskId])
        {
            wxXmlNode* pTaskNode = m_omTasks[nTaskId]->GetXmlDescription();
            pTaskNode->AddAttribute(wxT("cat"), m_sName);
            pRootNode->AddChild(pTaskNode);
        }

        m_pTaskManager->SendNetMessage(msgout);

        StartNextQueredTask();

        return true;
    }
    else
    {
        sErrMsg = wxString::Format(_("Save changes to xml file '%s' failed"), m_sXmlDocPath.c_str());
        return false;
    }
}

int wxGISTaskCategory::GetNewId()
{
    if(m_pTaskManager)
        return m_pTaskManager->GetNewId();
    return wxNOT_FOUND;
}

wxString wxGISTaskCategory::GetTaskConfigPath(const wxString& sCatName)
{
    if(m_pTaskManager)
        return m_pTaskManager->GetTaskConfigPath(sCatName);
    return wxEmptyString;
}

void wxGISTaskCategory::StartNextQueredTask()
{
    wxCriticalSectionLocker lock(m_CritSect);

    if(m_pTaskManager->GetExecTaskCount() >= m_pTaskManager->GetMaxExecTaskCount())
        return;

    for(short i = 0; i < m_nMaxTasks; ++i)
    {
        if(m_nRunningTasks >= m_nMaxTasks)
            return;

        int nTaskId = GetQueredTaskId();
        if(nTaskId != wxNOT_FOUND)
        {
            if(m_omTasks[nTaskId]->Start())
                m_nRunningTasks++;
        }

        //the max exec task total count in categories may be grater than max exec task in manager
        if(m_pTaskManager->GetExecTaskCount() >= m_pTaskManager->GetMaxExecTaskCount())
            return;
    }
}

int wxGISTaskCategory::GetQueredTaskId()
{
    long nMinPriority = LONG_MAX;
    int nOutTaskId = wxNOT_FOUND;
    for(wxGISTaskMap::iterator it = m_omTasks.begin(); it != m_omTasks.end(); ++it)
    {
        //get next task by priority
        if(it->second && it->second->GetState() == enumGISTaskQuered)
        {
            if(nMinPriority > it->second->GetPriority())
            {
                nMinPriority = it->second->GetPriority();
                nOutTaskId = it->second->GetId();
            }
        }
    }
    return nOutTaskId;
}

bool wxGISTaskCategory::SetMaxExecTasks(int nMaxTaskExec, wxString &sErrMsg)
{
    m_nMaxTasks = nMaxTaskExec;
    if( !Save() )
    {
        sErrMsg = wxString::Format(_("Save chnges to xml file '%s' failed"), m_sXmlDocPath.c_str());
        return false;
    }
    return true;
}


void wxGISTaskCategory::ChangeTask(int nTaskId)
{
    if( Save() )
    {
        wxNetMessage msgout(enumGISNetCmdCmd, enumGISCmdStChng, enumGISPriorityHigh);
        wxXmlNode* pRootNode = msgout.GetXMLRoot();
        wxXmlNode* pTaskNode = m_omTasks[nTaskId]->GetXmlDescription();
        pTaskNode->AddAttribute(wxT("cat"), m_sName);
        pRootNode->AddChild(pTaskNode);

        m_pTaskManager->SendNetMessage(msgout);
    }
}

void wxGISTaskCategory::GetTaskMessages(int nTaskId, int nId)
{
    wxCriticalSectionLocker lock(m_MsgCritSect);
    for(size_t i = 0; i < m_omTasks[nTaskId]->GetMessageCount(); ++i)
        ChangeTaskMsg(m_omTasks[nTaskId]->GetMessage(i).m_nType, m_omTasks[nTaskId]->GetMessage(i).m_sMessage, m_omTasks[nTaskId]->GetMessage(i).m_dt, i, nTaskId);
}
*/