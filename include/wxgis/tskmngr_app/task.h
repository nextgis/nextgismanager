/******************************************************************************
 * Project:  wxGIS (Task Manager)
 * Purpose:  Task and TaskCategoryList classes.
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

#pragma once

#include "wxgis/tskmngr_app/tskmngr.h"
#include "wxgis/core/process.h"
#include "wxgis/core/json/jsonval.h"

#include <wx/dynarray.h>
#include <wx/thread.h>
#define SUBTSKDIR wxT("subtasks")

class wxGISTaskBase;
WX_DECLARE_HASH_MAP( int, wxGISTaskBase*, wxIntegerHash, wxIntegerEqual, wxGISTaskMap );


/** @class wxGISTaskBase

    The base task class.

    @library{gp}
*/

class wxGISTaskBase :
    public wxObject
{
    DECLARE_ABSTRACT_CLASS(wxGISTaskBase)
public:
    wxGISTaskBase(wxGISTaskBase *pParentTask);
    virtual ~wxGISTaskBase(void);
    virtual wxString GetName() const;
    virtual int GetId(void) const;
    virtual int GetNewId(void);
    virtual void Delete(wxGISTaskBase *pTask);
    virtual bool HasName(const wxString &sName) const;
    //start/stop
    virtual void OnDestroy(void);
    virtual void StartNextQueredTask(void);
    //sorage
    virtual wxString GetNewStorePath(const wxString &sAddToName, const wxString &sSubDir = wxEmptyString );
    virtual wxString GetStorePath() const;
    virtual bool Save(void);
    virtual bool Load(void) = 0;
    //network
    virtual void SendNetMessage(wxGISNetCommand eCmd, wxGISNetCommandState eCmdState, wxGISMessagePriority ePrio, const wxJSONValue &val, const wxString &sMsg, long nMessageId = wxNOT_FOUND, int nUserId = wxNOT_FOUND) = 0;
    virtual void NetMessage(wxGISNetCommand eCmd, wxGISNetCommandState eCmdState, const wxJSONValue &val, long nMessageId = wxNOT_FOUND, int nUserId = wxNOT_FOUND) = 0;
    virtual void NetCommand(wxGISNetCommandState eCmdState, const wxJSONValue &val, long nMessageId = wxNOT_FOUND, int nUserId = wxNOT_FOUND) = 0;
protected:
    virtual wxJSONValue GetStoreConfig(void) = 0;
    virtual void ClearTasks(void);

    //virtual void SendNetMessageAsync(wxGISNetCommand eCmd, wxGISNetCommandState eCmdState, const wxJSONValue &val);
    //virtual wxGISNetCommandState SendNetMessageSync(wxGISNetCommand eCmd, wxGISNetCommandState eCmdState, const wxJSONValue &val);
    //virtual void NetMessage(wxGISNetCommand eCmd, wxGISNetCommandState eCmdState, const wxJSONValue &val);
    //virtual void NetCommand(wxGISNetCommandState eCmdState, const wxJSONValue &val);
    //virtual void NetNote(wxGISNetCommandState eCmdState, const wxJSONValue &val);
protected:
    wxGISTaskBase *m_pParentTask;
    int m_nId, m_nTaskCounter;
    wxString m_sName, m_sStoragePath;
    wxString m_sLastError;
    wxGISTaskMap m_omSubTasks;
};

class wxGISTask;

WX_DEFINE_ARRAY(wxGISTask *, wxGISQueredTasksArray);

/** @class wxGISTask

    The task class.

    @library{gp}
*/

class wxGISTask :
    public wxGISTaskBase,
    public wxGISProcess
{
    DECLARE_CLASS(wxGISTask)
public:
    wxGISTask(wxGISTaskBase* pParentTask, const wxString &sPath);
    virtual ~wxGISTask(void);
    virtual int GetGroupId(void) const;
    virtual bool Load(void);
    virtual bool Delete(long nMessageId = -2, int nUserId = -2);
    virtual void GetChildren(long nMessageId, int nUserId);
    virtual bool StartTask(long nMessageId, int nUserId);
    virtual bool StopTask(long nMessageId, int nUserId);
    virtual bool ChangeTask(const wxJSONValue& TaskVal, long nMessageId, int nUserId);
    virtual bool Create(const wxJSONValue& TaskConfig);
    virtual wxJSONValue GetAsJSON(void);
    virtual long GetCommonPriority(void);


    //start/process/stop
    virtual void OnDestroy(void);
    virtual void OnStart(void);
    virtual void OnStop(void);
    virtual void OnTerminate(int pid, int status);
    virtual int GetRunningTaskCount(void) const;
    virtual void GetQueredTasks(wxGISQueredTasksArray &oaTasks);
    virtual void ChangeTask(void);
    virtual void ChangeTaskMsg(wxGISEnumMessageType nType, const wxString &sInfoData);

    //
    virtual long Execute(void);
    virtual bool IsGroupIdExecuting(int nGroupId) const;
    virtual void AddInfo(wxGISEnumMessageType nType, const wxString &sInfoData);

    virtual void UpdatePercent(const wxString &sPercentData);
    //virtual void SetPriority(long nNewPriority);
    //virtual long GetPriority(void) const {return m_nPriority;};
    virtual bool Start(void);
    //virtual size_t GetMessageCount() const {return m_staMessages.size();};

    typedef struct _message
    {
        wxString m_sMessage;
        wxDateTime m_dt;
        wxGISEnumMessageType m_nType;
    }MESSAGE;
    //virtual wxGISTask::MESSAGE GetMessage(size_t nIndex) const {return m_staMessages[nIndex];};
    virtual void SendNetMessage(wxGISNetCommand eCmd, wxGISNetCommandState eCmdState, wxGISMessagePriority ePrio, const wxJSONValue &val, const wxString &sMsg, long nMessageId = wxNOT_FOUND, int nUserId = wxNOT_FOUND);
    virtual void NetMessage(wxGISNetCommand eCmd, wxGISNetCommandState eCmdState, const wxJSONValue &val, long nMessageId = wxNOT_FOUND, int nUserId = wxNOT_FOUND);
    virtual void NetCommand(wxGISNetCommandState eCmdState, const wxJSONValue &val, long nMessageId = wxNOT_FOUND, int nUserId = wxNOT_FOUND);
protected:
    virtual wxJSONValue GetStoreConfig(void);
protected:
    wxString m_sDescription;
    int m_nGroupId;
    wxULongLong m_nVolume;//value in, eg. bytes
    wxString m_sExecPath;
    long m_nPriority;
    wxJSONValue m_Params;
    double m_dfPrevDone;
    wxDateTime m_dtCreated;
    //wxVector<MESSAGE> m_staMessages;
};

/** @class wxGISTaskCategory

    The tasks category class.

    @libray{gp}
*/

class wxGISTaskCategory :
    public wxGISTaskBase
{
    DECLARE_CLASS(wxGISTaskCategory)
public:
    wxGISTaskCategory(const wxString &sPath, wxGISTaskManager* pTaskManager);
    virtual ~wxGISTaskCategory(void);
    virtual bool Load(void);
    virtual void SetMaxExecTaskCount(int nMaxExecTasks);
    virtual wxString GetNewStorePath(const wxString &sAddToName, const wxString &sSubDir = wxEmptyString );
    virtual bool Save(void);
    virtual void GetChildren(long nMessageId, int nUserId);
    virtual bool AddTask(const wxJSONValue &TaskConfig, long nMessageId, int nUserId);
    //start/stop
    virtual bool IsGroupIdExecuting(int nGroupId) const;
    virtual int GetRunningTaskCount(void) const;
    virtual void GetQueredTasks(wxGISQueredTasksArray &oaTasks);

//    bool ChangeTaskPriority(int nTaskId, long nPriority, int nId, wxString &sErrMsg);
//    bool ChangeTask(const wxXmlNode* pTaskNode, int nId, wxString &sErrMsg);
//    bool GetTasks(int nId, wxString &sErrMsg);
//    void GetTaskMessages(int nTaskId, int nId);
//    wxString GetTaskConfigPath(const wxString& sCatName);
//    bool SetMaxExecTasks(int nMaxTaskExec, wxString &sErrMsg);
    virtual void StartNextQueredTask(void);
    virtual void SendNetMessage(wxGISNetCommand eCmd, wxGISNetCommandState eCmdState, wxGISMessagePriority ePrio, const wxJSONValue &val, const wxString &sMsg, long nMessageId = wxNOT_FOUND, int nUserId = wxNOT_FOUND);
    virtual void NetMessage(wxGISNetCommand eCmd, wxGISNetCommandState eCmdState, const wxJSONValue &val, long nMessageId = wxNOT_FOUND, int nUserId = wxNOT_FOUND);
    virtual void NetCommand(wxGISNetCommandState eCmdState, const wxJSONValue &val, long nMessageId = wxNOT_FOUND, int nUserId = wxNOT_FOUND);
protected:
    virtual wxJSONValue GetStoreConfig(void);
    virtual wxGISTask* const GetQueredTask(void);
protected:
    wxGISTaskManager* m_pTaskManager;
    short m_nMaxTasks;
    wxCriticalSection m_CritSect;
};
