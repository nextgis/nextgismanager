/******************************************************************************
 * Project:  wxGIS (Task Manager)
 * Purpose:  Task manager class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2012-2014 Dmitry Baryshnikov
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

#include "wxgis/tskmngr_app/tskmngr.h"
#include "wxgis/tskmngr_app/task.h"
#include "wxgis/version.h"
#include "wxgis/core/format.h"

#include <wx/cmdline.h>
#include <wx/dir.h>

//----------------------------------------------------------------------------
// wxGISServer
//----------------------------------------------------------------------------
IMPLEMENT_CLASS(wxGISTaskManager, wxObject)

wxGISTaskManager::wxGISTaskManager(void)
{
    m_pNetworkService = NULL;
    m_bExitState = false;

    m_nExitState = enumGISNetCmdStExit;
    m_nMaxExecTasks = wxThread::GetCPUCount();
    wxGISAppConfig oConfig = GetConfig();
	if(oConfig.IsOk())
    {
        m_nExitState = (wxGISNetCommandState)oConfig.ReadInt(enumGISHKCU, wxString(wxT("wxGISTaskNamager/app/exit_state")), enumGISNetCmdStExit);
        m_nMaxExecTasks = oConfig.ReadInt(enumGISHKCU, wxString(wxT("wxGISTaskNamager/app/max_exec_tasks")), m_nMaxExecTasks);
    }
    //m_nIdCounter = 0;
}

wxGISTaskManager::~wxGISTaskManager(void)
{
    Exit();
}

void wxGISTaskManager::Exit(void)
{
    //delete network service
    if(m_pNetworkService)
    {

        m_pNetworkService->Stop();
        SetNetworkService(NULL);
        m_pNetworkService = NULL;
    }

    //yield
    wxDateTime dtBeg = wxDateTime::Now();
    wxTimeSpan Elapsed = wxDateTime::Now() - dtBeg;
    wxFprintf(stdout, wxString(_("Exiting\n")));
    int nSec = 3;
    while (Elapsed.GetSeconds() < 3)
    {
        wxTheApp->Yield(true);
        Elapsed = wxDateTime::Now() - dtBeg;
        int nTest = 3 - Elapsed.GetSeconds().ToLong();
        if (nSec != nTest)
        {
            wxFprintf(stdout, wxString::Format(wxT("%d sec.\r"), nTest));
            nSec = nTest;
        }

    }

    DestroyCategories();

    wxGISTaskCategoryMap::iterator it;
    for( it = m_omCategories.begin(); it != m_omCategories.end(); ++it )
    {
        //save and delete
        wxGISTaskCategory* pTaskCategory = it->second;
        wxDELETE(pTaskCategory);
    }
}

void wxGISTaskManager::DestroyCategories()
{
    wxGISTaskCategoryMap::iterator it;
    for (it = m_omCategories.begin(); it != m_omCategories.end(); ++it)
    {
        wxGISTaskCategory* pTaskCategory = it->second;
        if (pTaskCategory != NULL)
        {
            pTaskCategory->OnDestroy();
        }
    }
}

bool wxGISTaskManager::Init(void)
{
    //load tasks and etc.

	wxGISAppConfig oConfig = GetConfig();
	if(oConfig.IsOk())
	{
		m_sUserConfigDir = oConfig.GetLocalConfigDirNonPortable() + wxFileName::GetPathSeparator() + wxString(TSKDIR);
        LoadCategories(m_sUserConfigDir);
	}

    //create network service
    m_pNetworkService = new wxGISLocalNetworkService(static_cast<INetEventProcessor*>(this));
    SetNetworkService(m_pNetworkService);

    if(!m_pNetworkService->Start())
    {
        wxDELETE(m_pNetworkService);
        return false;
    }
    return true;
}

void wxGISTaskManager::LoadCategories(const wxString &sPathToCategories)
{
    wxLogMessage(_("Read task categories from '%s'"), sPathToCategories.c_str());
    wxDir dir(sPathToCategories);
    if( dir.IsOpened() )
    {
        wxString sCategoryName;

        bool bContinue = dir.GetFirst(&sCategoryName, wxEmptyString, wxDIR_DIRS);
        while ( bContinue )
        {
            wxGISTaskCategory* pGISTaskCategory = new wxGISTaskCategory(sPathToCategories + wxFileName::GetPathSeparator() + sCategoryName, this);
            if(pGISTaskCategory->Load())
            {
                wxLogMessage(_("The category '%s' loaded"), sCategoryName.c_str());
                m_omCategories[sCategoryName] = pGISTaskCategory;
            }
            else
            {
                wxLogError(_("The category '%s' load failed "), sCategoryName.c_str());
                wxDELETE(pGISTaskCategory);
            }
            bContinue = dir.GetNext(&sCategoryName);
        }
    }
}


void wxGISTaskManager::ProcessNetEvent(wxGISNetEvent& event)
{
    wxNetMessage msg = event.GetNetMessage();
    wxCHECK_RET(msg.IsOk(), wxT("Invalid message"));

    switch(msg.GetCommand())
    {
    case enumGISNetCmdCmd:
        ProcessNetCommand(msg, event.GetId());
        break;
    case enumGISNetCmdSetExitSt:
        SetExitState(msg.GetState());
        break;
    case enumGISNetCmdBye://get from client exit state: exit if no connection, stay executing tasks or wait any new connections
        if(m_pNetworkService->GetConnectionCount() <= 0)
        {
            //exit or not
            OnExit();
        }
        break;
    }
}

wxJSONValue wxGISTaskManager::GetParamsAsJSON(const wxJSONValue &val)
{
    wxJSONValue out = val;
    out[wxT("max_exec_task_count")] = m_nMaxExecTasks;
    out[wxT("exit_state")] = m_nExitState;
    return out;
}

wxJSONValue wxGISTaskManager::GetChildrenAsJSON(const wxJSONValue &val)
{
    wxJSONValue out = val;
    short nCounter(0);
    for(wxGISTaskCategoryMap::iterator it = m_omCategories.begin(); it != m_omCategories.end(); ++it)
    {
        wxGISTaskCategory *pCat = it->second;
        if(pCat)
        {
            out[wxT("categories")][nCounter++] = pCat->GetName();
        }
    }
    return out;
}

void wxGISTaskManager::ProcessNetCommand(const wxNetMessage &msg, int nUserId)
{
    //check if command for some category
    wxJSONValue val = msg.GetValue();

    if(val.IsValid() && val.HasMember(wxT("cat")))
    {
        wxString sCategory = val[wxT("cat")].AsString();
        if(m_omCategories[sCategory])
        {
            return m_omCategories[sCategory]->NetMessage(msg.GetCommand(), msg.GetState(), val, msg.GetId(), nUserId);
        }
    }

    //if command for manager - process it
    switch(msg.GetState())
    {
        case enumGISCmdDetails://Get manager details - properties + category list
            {
                wxNetMessage msgout(enumGISNetCmdCmd, enumGISCmdDetails, enumGISPriorityHigh, msg.GetId());
                msgout.SetValue(GetParamsAsJSON(msgout.GetValue()));
                msgout.SetValue(GetChildrenAsJSON(msgout.GetValue()));
                SendNetMessage(msgout, nUserId);
            }
            break;
        case enumGISCmdChildren://Get category list
            {
                wxNetMessage msgout(enumGISNetCmdCmd, enumGISCmdChildren, enumGISPriorityHigh, msg.GetId());
                msgout.SetValue(GetChildrenAsJSON(msgout.GetValue()));
                SendNetMessage(msgout, nUserId);
            }
            break;
        case enumGISCmdSetParam://Set max parallel executed tasks in each category
            {
                int nMaxExec = val.Get(wxT("max_exec_task_count"), wxJSONValue(m_nMaxExecTasks)).AsInt();
                SetMaxExecTaskCount(nMaxExec);

                wxGISNetCommandState nExitSt = (wxGISNetCommandState)val.Get(wxT("exit_state"), wxJSONValue(m_nExitState)).AsLong();
                SetExitState(nExitSt);

                wxNetMessage msgout(enumGISNetCmdCmd, enumGISCmdSetParam, enumGISPriorityNormal,  msg.GetId());
                msgout.SetValue(GetParamsAsJSON(msgout.GetValue()));
                SendNetMessage(msgout, nUserId);
            }
            break;
        case enumGISCmdGetParam:
            {
                wxNetMessage msgout(enumGISNetCmdCmd, enumGISCmdGetParam, enumGISPriorityNormal,  msg.GetId());
                msgout.SetValue(GetParamsAsJSON(msgout.GetValue()));
                SendNetMessage(msgout, nUserId);
            }
            break;
        case enumGISCmdStAdd:
            {
                wxString sCatName = val[wxT("name")].AsString();
                wxLogMessage("Create category directory %s", sCatName);
                if(sCatName.IsEmpty() || m_omCategories[sCatName])
                {
                    wxLogError(_("The category name is empty or category is already exist"));
                    wxNetMessage msgout(enumGISNetCmdCmd, enumGISNetCmdStErr, enumGISPriorityLow, msg.GetId());
                    msgout.SetMessage(_("The category name is empty or category is already exist"));
                    SendNetMessage(msgout, nUserId);
                }
                else
                {
                    wxString sCategoryName = ReplaceForbiddenCharsInFileName(sCatName);
                    wxString sCatPath = m_sUserConfigDir + wxFileName::GetPathSeparator() + sCategoryName;
                    if (wxFileName::Mkdir(sCatPath, wxPOSIX_USER_READ | wxPOSIX_USER_WRITE | wxPOSIX_USER_EXECUTE | wxPOSIX_GROUP_READ | wxPOSIX_GROUP_EXECUTE | wxPOSIX_OTHERS_READ | wxPOSIX_OTHERS_EXECUTE, wxPATH_MKDIR_FULL)) //0755
                    {
                        wxLogMessage("Successfully created category directory %s", sCatName);
                        wxGISTaskCategory* pGISTaskCategory = new wxGISTaskCategory(sCatPath, this);
                        m_omCategories[sCategoryName] = pGISTaskCategory;

                        wxNetMessage msgout(enumGISNetCmdCmd, enumGISCmdStAdd, enumGISPriorityNormal, msg.GetId());
                        wxJSONValue val;
                        val[wxT("name")] = sCatName;
                        msgout.SetValue(val);
                        SendNetMessage(msgout, nUserId);
                    }
                    else
                    {
                        wxLogError(_("Create category directory failed"));
                        wxNetMessage msgout(enumGISNetCmdCmd, enumGISNetCmdStErr, enumGISPriorityHigh,  msg.GetId());
                        msgout.SetMessage(_("Create category directory failed"));
                        SendNetMessage(msgout, nUserId);
                    }
                }
            }
            break;
       case enumGISCmdStDel:
           //TODO:
           break;
       case enumGISCmdStChng:
           //TODO:
           break;
        default:
           break;
    }
}

void wxGISTaskManager::SetExitState(wxGISNetCommandState nExitState)
{
    wxLogMessage(wxT("Set exit state %d"), nExitState);
    m_nExitState = nExitState;
}

void wxGISTaskManager::OnExit(void)
{
    wxLogMessage(wxT("OnExit, state %d"), m_nExitState);

    if(m_nExitState == enumGISNetCmdStNoExit)
        return;
    if (m_nExitState == enumGISNetCmdStExitAfterExec && GetExecTaskCount() > 0)
    {
        m_bExitState = true;
        return;
    }

    //stop all tasks, and then exit
    DestroyCategories();

    wxTheApp->Exit();
}

int wxGISTaskManager::GetExecTaskCount(void) const
{
    int nRes(0);
    for(wxGISTaskCategoryMap::const_iterator it = m_omCategories.begin(); it != m_omCategories.end(); ++it)
    {
        wxGISTaskCategory *pCat = it->second;
        if(pCat)
        {
            nRes += pCat->GetRunningTaskCount();
        }
    }
    return nRes;
}

int wxGISTaskManager::GetMaxExecTaskCount(void) const
{
    return m_nMaxExecTasks;
}

void wxGISTaskManager::SetMaxExecTaskCount(int nMaxExecTasks)
{
    m_nMaxExecTasks = nMaxExecTasks;
}

void wxGISTaskManager::SendNetMessage(const wxNetMessage & msg, int nId)
{
    if(m_pNetworkService)
    {
        m_pNetworkService->SendNetMessage(msg, nId);
    }
}

wxString wxGISTaskManager::ReplaceForbiddenCharsInFileName(const wxString &name, const wxString &ch)
{
    wxString sOutput = name;
    wxString forbidden = wxFileName::GetForbiddenChars();
    forbidden += wxT(" ()[]{}");
    int size=forbidden.Length();
    for (int i = 0; i < size; ++i)
        sOutput.Replace( wxString(forbidden[i]), ch, true);
    sOutput.Replace( ch + ch, ch, true);
    return sOutput.MakeLower();
}

wxString wxGISTaskManager::GetNewStorePath(const wxString &sAddToName, const wxString &sSubDir)
{
    wxString sSubTaskConfigDir = m_sUserConfigDir + wxFileName::GetPathSeparator() + sSubDir;
    if(!wxDirExists(sSubTaskConfigDir))
        wxFileName::Mkdir(sSubTaskConfigDir, wxPOSIX_USER_READ | wxPOSIX_USER_WRITE | wxPOSIX_USER_EXECUTE | wxPOSIX_GROUP_READ | wxPOSIX_GROUP_EXECUTE | wxPOSIX_OTHERS_READ | wxPOSIX_OTHERS_EXECUTE, wxPATH_MKDIR_FULL); //0755
    wxDateTime dt(wxDateTime::Now());
    wxString sTaskConfigPath = sSubTaskConfigDir + wxFileName::GetPathSeparator() + wxString::Format(wxT("%s%s.json"), dt.GetValue().ToString().c_str(), ReplaceForbiddenCharsInFileName(sAddToName).c_str());
    return sTaskConfigPath;
}

void wxGISTaskManager::OnCategoryExecutionFinished(const wxGISTaskCategory* pCat)
{
    wxGISTaskCategoryMap::iterator it;
    for (it = m_omCategories.begin(); it != m_omCategories.end(); ++it)
    {
        wxGISTaskCategory* pTaskCategory = it->second;
        if (pTaskCategory != NULL)
        {
            if (pTaskCategory->GetRunningTaskCount() > 0)
                return;
        }
    }

    if (m_pNetworkService != NULL && m_pNetworkService->GetConnectionCount() == 0 && m_bExitState) //no new connections and exit state is set to true
    {
        OnExit();
    }
}

/*
bool wxGISTaskManager::AddTask(const wxXmlNode* pTaskNode, int nId, wxString &sErrMsg)
{
    wxCHECK_MSG(pTaskNode, false, wxT("Input wxXmlNode* is null"));
    if(!ValidateTask(pTaskNode))
    {
        sErrMsg = wxString(_("Add task failed. Ivalid task description"));
        return false;
    }
    wxCriticalSectionLocker locker(m_TaskLock);

    wxString sCat = pTaskNode->GetAttribute(wxT("cat"), wxT("default"));
    wxLogMessage(_("Add task to category '%s' "), sCat);
    if(!m_omCategories[sCat])
    {
        wxString sNewPath = m_sUserConfigDir + wxFileName::GetPathSeparator() + ReplaceForbiddenCharsInFileName(sCat) + wxString(wxT(".xml"));
        m_omCategories[sCat] = new wxGISTaskCategory( sNewPath, this, sCat);
        int nMaxTaskExec = GetDecimalValue(pTaskNode, wxT("cat_max_task_exec"), wxThread::GetCPUCount());
        if(!m_omCategories[sCat]->SetMaxExecTasks(nMaxTaskExec, sErrMsg))
            return false;
    }
    return m_omCategories[sCat]->AddTask(pTaskNode, nId, sErrMsg);
}

bool wxGISTaskManager::DelTask(const wxXmlNode* pTaskNode, int nId, wxString &sErrMsg)
{
    wxCHECK_MSG(pTaskNode, false, wxT("Input wxXmlNode* is null"));
    if(!pTaskNode->HasAttribute(wxT("id")))
        return false;

    wxCriticalSectionLocker locker(m_TaskLock);

    wxString sCat = pTaskNode->GetAttribute(wxT("cat"), wxEmptyString);
    int nTaskId = GetDecimalValue(pTaskNode, wxT("id"), wxNOT_FOUND);
    wxLogMessage(_("Delete task #%d from category '%s' "), nTaskId, sCat);
    if(m_omCategories[sCat])
        return m_omCategories[sCat]->DelTask(nTaskId, nId, sErrMsg);
    sErrMsg = wxString::Format(_("The category '%s' is undefined"), sCat.c_str());
    return false;
}

bool wxGISTaskManager::StartTask(const wxXmlNode* pTaskNode, int nId, wxString &sErrMsg)
{
    wxCHECK_MSG(pTaskNode, false, wxT("Input wxXmlNode* is null"));
    if(!pTaskNode->HasAttribute(wxT("id")))
        return false;

    wxCriticalSectionLocker locker(m_TaskLock);

    wxString sCat = pTaskNode->GetAttribute(wxT("cat"), wxEmptyString);
    int nTaskId = GetDecimalValue(pTaskNode, wxT("id"), wxNOT_FOUND);
    wxLogMessage(_("StartTask task #%d from category '%s' "), nTaskId, sCat);
    if(m_omCategories[sCat])
        return m_omCategories[sCat]->StartTask(nTaskId, nId, sErrMsg);
    sErrMsg = wxString::Format(_("The category '%s' is undefined"), sCat.c_str());
    return false;
}

bool wxGISTaskManager::StopTask(const wxXmlNode* pTaskNode, int nId, wxString &sErrMsg)
{
    wxCHECK_MSG(pTaskNode, false, wxT("Input wxXmlNode* is null"));
    if(!pTaskNode->HasAttribute(wxT("id")))
        return false;

    wxCriticalSectionLocker locker(m_TaskLock);

    wxString sCat = pTaskNode->GetAttribute(wxT("cat"), wxEmptyString);
    int nTaskId = GetDecimalValue(pTaskNode, wxT("id"), wxNOT_FOUND);
    wxLogMessage(_("StopTask task #%d from category '%s' "), nTaskId, sCat);
    if(m_omCategories[sCat])
        return m_omCategories[sCat]->StopTask(nTaskId, nId, sErrMsg);
    sErrMsg = wxString::Format(_("The category '%s' is undefined"), sCat.c_str());
    return false;
}

/* TODO:
bool wxGISTaskManager::ChangeTasksPriority(const wxXmlNode* pTaskNode, int nId, wxString &sErrMsg)
{
    wxCHECK_MSG(pTaskNode, false, wxT("Input wxXmlNode* is null"));
    if(!pTaskNode->HasAttribute(wxT("id")))
        return false;

    wxCriticalSectionLocker locker(m_TaskLock);

    wxString sCat = pTaskNode->GetAttribute(wxT("cat"), wxEmptyString);
    int nTaskId = GetDecimalValue(pTaskNode, wxT("id"), wxNOT_FOUND);
    long nTaskPriority = GetDecimalValue(pTaskNode, wxT("prio"), wxNOT_FOUND);
    wxLogMessage(_("Change task #%d from category '%s' priority to %d"), nTaskId, sCat, nTaskPriority);
    if(m_omCategories[sCat])
        return m_omCategories[sCat]->ChangeTaskPriority(nTaskId, nTaskPriority, nId, sErrMsg);
    sErrMsg = wxString::Format(_("The category '%s' is undefined"), sCat.c_str());
    return false;
}
*//*
bool wxGISTaskManager::ChangeTask(const wxXmlNode* pTaskNode, int nId, wxString &sErrMsg)
{
    wxCHECK_MSG(pTaskNode, false, wxT("Input wxXmlNode* is null"));
    if(!pTaskNode->HasAttribute(wxT("id")))
        return false;
    wxCriticalSectionLocker locker(m_TaskLock);

    wxString sCat = pTaskNode->GetAttribute(wxT("cat"), wxEmptyString);
    int nTaskId = GetDecimalValue(pTaskNode, wxT("id"), wxNOT_FOUND);
    wxLogMessage(_("Change task #%d from category '%s' "), nTaskId, sCat);
    if(m_omCategories[sCat])
        return m_omCategories[sCat]->ChangeTask(pTaskNode, nId, sErrMsg);
    sErrMsg = wxString::Format(_("The category '%s' is undefined"), sCat.c_str());
    return false;
}

bool wxGISTaskManager::ValidateTask(const wxXmlNode* pTaskNode)
{
    return pTaskNode->HasAttribute(wxT("name")) && pTaskNode->HasAttribute(wxT("exec")) && pTaskNode->HasAttribute(wxT("state"));
}

int wxGISTaskManager::GetNewId()
{
    wxCriticalSectionLocker locker(m_CounterLock);

    return m_nIdCounter++;
}

void wxGISTaskManager::GetTaskDetails(const wxXmlNode* pTaskNode, int nId)
{
    wxCHECK_RET(pTaskNode, wxT("Input wxXmlNode* is null"));
    if(!pTaskNode->HasAttribute(wxT("id")))
        return;

    wxString sCat = pTaskNode->GetAttribute(wxT("cat"), wxEmptyString);
    int nTaskId = GetDecimalValue(pTaskNode, wxT("id"), wxNOT_FOUND);

    if(m_omCategories[sCat])
    {
        m_omCategories[sCat]->GetTaskMessages(nTaskId, nId);
    }
}

*/
//TODO: timer for task execution (look for categories where no running task and task quered)
//If found - it->second->StartNextQueredTask();
